//
// Created by Manuel Hahn on 6/15/22.
//

#include <map>
#include <csignal>
#include <sys/time.h>
#include "CGICall.hpp"
#include "HTTPException.hpp"
#include "CGIResponseError.hpp"
#include "Configuration.hpp"
#include "Connection.hpp"
#include <fcntl.h>

std::list<int> CGICall::pipeFds = std::list<int>();

CGICall::CGICall(HTTPRequest * request, Socket & socket)
        : CGIResponse(request),
          uri(request->getURI()),
          method("REQUEST_METHOD="),
          protocol("SERVER_PROTOCOL=HTTP/1.1"),
          pathinfo("PATH_INFO="),
          gatewayInterface("GATEWAY_INTERFACE=CGI/1.1"),
          queryString("QUERY_STRING="),
          scriptName("SCRIPT_NAME="),
          serverName("SERVER_NAME="),
          serverPort("SERVER_PORT="),
          serverSoftware("SERVER_SOFTWARE=webserv/1.0 (2022/06)"),
          remoteAddress("REMOTE_ADDR="),
          remoteHost("REMOTE_HOST="),
          requestUri("REQUEST_URI="),
          httpUserAgent("HTTP_User-Agent="),
          httpHost("HTTP_Host="),
          httpLang("HTTP_Accept_language="),
          httpEncoding("HTTP_Accept-Encoding="),
          httpAccept("HTTP_Accept="),
          httpConnection("HTTP_Connection="),
          httpContentLength("HTTP_Content-Length="),
          httpExpect("HTTP_Except="),
          socket(socket),
          child(-1),
          threadID(),
          in(),
          out(),
          payloadCounter(),
          socketCounter(),
          running(false),
          runningMutex() {
    pthread_mutex_init(&runningMutex, NULL);
    Configuration config = Configuration::getInstance();
    _server_root = config.get_server_root_folder();
    _server_location_log = set_absolut_path(_server_root);

    if (is_request_defined_location(request->getPath(), config.get_location_specifier())) {
        _server_location_log = set_absolut_path(_loc_root);
    }
}

CGICall::~CGICall() {
    if (child > -1) {
        kill(child, SIGTERM);
    }
    pthread_cancel(threadID);
    pthread_join(threadID, NULL);
    pthread_mutex_destroy(&runningMutex);
}

bool CGICall::hasFD(int fd) {
    return fd == socket.get_fd() || fd == out[0] || fd == in[1];
}

bool CGICall::writePayload() {
    for (; payloadCounter < _request->get_payload().size(); ++payloadCounter) {
        if (write(in[1], _request->get_payload().c_str() + payloadCounter, 1) < 0) {
            debug("Write with fd " << in[1] << " size " << payloadCounter);
            return false;
        }
    }
    debug("Write with fd " << in[1] << " size " << payloadCounter);
    debug("Closing server -> cgi");
    close(in[1]);
    return true;
}

bool CGICall::readPayload() {
    ssize_t r;
    char    b;

    while ((r = read(out[0], &b, 1)) > 0) {
        buffer += b;
    }
    debug("Read with fd " << out[0] << " size " << buffer.size() << ", r: " << r);
    if (r < 0) {
        return false;
    }
    close(out[0]);
    debug("Processing CGI output (" << buffer.size() << " bytes)");
    processCGIOutput();
    return true;
}

bool CGICall::writeSocket() {
    try {
        for (; socketCounter < payload.size(); ++socketCounter) {
            socket.write(payload[socketCounter]);
        }
        debug("Write with socket fd " << socket.get_fd() << " size " << socketCounter);
        debug("Closing socket fd " << socket.get_fd());
        Connection::getInstance().removeFD(socket.get_fd());
        socket.close();
        pthread_mutex_lock(&runningMutex);
        running = false;
        pthread_mutex_unlock(&runningMutex);
        return true;
    } catch (IOException &) {
        debug("Write with socket fd " << socket.get_fd() << " size " << socketCounter);
        return false;
    }
}

bool CGICall::runForFD(int fd) {
    if (fd == in[1]) {
        return writePayload();
    } else if (fd == out[0]) {
        return readPayload();
    } else if (fd == socket.get_fd()) {
        return writeSocket();
    } else {
        return true;
    }
}

void CGICall::run(Socket &) {
    switch (_request->getType()) {
        case HTTPRequest::GET:    method += "GET";    break;
        case HTTPRequest::POST:   method += "POST";   break;
        case HTTPRequest::DELETE: method += "DELETE"; break;
		default: throw HTTPException(500);
    }
    const std::string combined = uri.getPathInfo().empty() ? "Arsch" : uri.getPathInfo();
    pathinfo += combined;
    requestUri += combined;
    queryString += uri.getQuery();
    remoteAddress += int_to_ipv4(_request->getPeerAddress());
    remoteHost += _request->getPeerName();
    serverName += Configuration::getInstance().get_server_names().at(0);
    serverPort += int_to_string(_request->getUsedPort());
    if (_request->hasContent()) {
        contentLength = "CONTENT_LENGTH=" + int_to_string(static_cast<int>(_request->getContentLength()));
        contentType = "CONTENT_TYPE=";
        contentType += vectorToString(_request->getContentType());
    }
    httpUserAgent += _request->getUserAgent();
    httpHost += _request->getHost();
    httpLang += vectorToString(_request->getLang());
    httpEncoding += vectorToString(_request->getEncoding());
    httpAccept += vectorToString(_request->getContentType());
    httpContentLength += int_to_string(static_cast<int>(_request->getContentLength()));
    httpExpect += _request->getExpect();
    httpConnection += _request->isKeepAlive() ? "keep-alive" : "";
    scriptName += computeRequestedFile();
    const std::string & requestedFile = computeRequestedFile();
    if (access(requestedFile.c_str(), F_OK) < 0) throw HTTPException(404);
    if (access(requestedFile.c_str(), X_OK) < 0) throw HTTPException(403);
    if (pipe(in) < 0) throw HTTPException(500);
	if (pipe(out) < 0) {
        close(in[0]);
        close(in[1]);
        throw HTTPException(500);
    }
    pipeFds.push_back(in[0]);
    pipeFds.push_back(in[1]);
    pipeFds.push_back(out[0]);
    pipeFds.push_back(out[1]);
    running = true;
    fcntl(in[1], F_SETFL, O_NONBLOCK);
    if (!writePayload()) {
        Connection::getInstance().addFD(in[1], false);
    }
    fcntl(out[0], F_SETFL, O_NONBLOCK);
    Connection::getInstance().addFD(out[0]);
    execute(in[0], out[1], requestedFile);
    pthread_create(&threadID, NULL, reinterpret_cast<void *(*)(void *)>(CGICall::waitOrThrow), this);
    //pthread_create(&threadID, NULL, reinterpret_cast<void *(*)(void *)>(CGICall::async), this);
}

void CGICall::processCGIOutput() {
    bool cleanUp = true;
    std::stringstream s(buffer);
    try {
        HTTPHeader header = parseCGIResponse(s);
        if (header.getTransferEncoding() == "chunked") {
            header.setTransferEncoding("");
            std::string line;
            while (std::getline(s, line) && !(line.empty() || line == "\r")) {
                unsigned long length = strtol(line.c_str(), NULL, 10); // Or base 16?
                std::getline(s, line);
                payload.append(line.c_str(), length);
            }
        } else {
            payload = buffer.substr(s.tellg());
        }
        header.set_content_length(static_cast<int>(payload.size()));
        socket.write(header.tostring());
        socket.write("\r\n\r\n");
        if (!writeSocket()) {
            Connection::getInstance().addFD(socket.get_fd(), false);
            cleanUp = false;
        } else {
            cleanUp = false;
        }
    } catch (HTTPException & ex) {
        sendError(ex.get_error_code());
    } catch (std::exception & ex) {
        std::clog << "INFO: Socket has been closed" << std::endl
                  << "INFO: " << ex.what()          << std::endl;
    }
    if (cleanUp) {
        Connection::getInstance().removeFD(socket.get_fd());
        pthread_mutex_lock(&runningMutex);
        running = false;
        pthread_mutex_unlock(&runningMutex);
        debug("Closing socket (fd: " << socket.get_fd() << ")");
        socket.close();
    }
}

/*void CGICall::async(CGICall * self) {
    try {
        //self->waitOrThrow();
        waitOrThrow(self);
        HTTPHeader header = parseCGIResponse(self->out[0]);
        std::string payload;
        if (header.getTransferEncoding() == "chunked") {
            header.setTransferEncoding("");
            std::string line;
            while (!(line = nextLine(self->out[0])).empty()) {
                unsigned long length = strtol(line.c_str(), NULL, 10);
                line = nextLine(self->out[0]);
                payload.append(line.c_str(), length);
            }
        } else {
            ssize_t r;
            char b;
            while ((r = read(self->out[0], &b, 1)) > 0) payload += b;
            if (r < 0) throw HTTPException(500);
        }
        header.set_content_length(static_cast<int>(payload.size()));
        self->socket.send(header.tostring());
        self->socket.send("\r\n\r\n");
        self->socket.send(payload);
    } catch (HTTPException & httpException) {
        self->sendError(httpException.get_error_code());
    } catch (std::exception & exception) {
        std::clog << "INFO: Socket has been closed" << std::endl
                  << "INFO: " << exception.what()   << std::endl;
    }
    close(self->out[0]);
    close(self->socket.get_fd());
    pthread_mutex_lock(&self->runningMutex);
    self->running = false;
    pthread_mutex_unlock(&self->runningMutex);
}*/

void CGICall::sendError(const int errorCode) _NOEXCEPT {
    try {
        CGIResponseError error;
        error.set_error_code(errorCode);
        error.run(socket);
    } catch (std::exception & exception) {
        std::clog << "INFO: Socket has been closed" << std::endl
                  << "INFO: " << exception.what()   << std::endl;
    }
}

void CGICall::execute(const int in, const int out, const std::string & requestedFile) {
    child = fork();
    if (child < 0) throw HTTPException(500);
    if (child > 0) return;
    dup2(in, STDIN_FILENO);
    dup2(out, STDOUT_FILENO);
    std::for_each(pipeFds.begin(), pipeFds.end(), ::close);
    char ** arguments = new char * [2]();
    char ** environment = new char * [_request->hasContent() ? 23 : 21]();
    environment[0]  = strdup(method.c_str());
    environment[1]  = strdup(protocol.c_str());
    environment[2]  = strdup(pathinfo.c_str());
    environment[3]  = strdup(gatewayInterface.c_str());
    environment[4]  = strdup(queryString.c_str());
    environment[5]  = strdup(serverName.c_str());
    environment[6]  = strdup(serverPort.c_str());
    environment[7]  = strdup(serverSoftware.c_str());
    environment[8]  = strdup(remoteHost.c_str());
    environment[9]  = strdup(remoteAddress.c_str());
    environment[10] = strdup(httpConnection.c_str());
    environment[11] = strdup(httpExpect.c_str());
    environment[12] = strdup(httpContentLength.c_str());
    environment[13] = strdup(httpAccept.c_str());
    environment[14] = strdup(httpEncoding.c_str());
    environment[15] = strdup(httpLang.c_str());
    environment[16] = strdup(httpHost.c_str());
    environment[17] = strdup(httpUserAgent.c_str());
    environment[18] = strdup(requestUri.c_str());
    environment[19] = strdup(scriptName.c_str());
    if (_request->hasContent()) {
        environment[20] = strdup(contentLength.c_str());
        environment[21] = strdup(contentType.c_str());
    }
    arguments[0] = strdup(requestedFile.c_str());
    chdir(computeScriptDirectory().c_str());
    if (execve(requestedFile.c_str(), arguments, environment) < 0) {
        exit(-1);
    }
}

bool CGICall::isRunning() {
    bool ret;
    pthread_mutex_lock(&runningMutex);
    ret = running;
    pthread_mutex_unlock(&runningMutex);
    return ret;
}

void CGICall::waitOrThrow(CGICall * self) {
    int status, ret;
    unsigned int timeElapsed;
    struct timeval start, now;
    gettimeofday(&start, NULL);
    do {
        usleep(100000);
        gettimeofday(&now, NULL);
        timeElapsed = ((now.tv_sec - start.tv_sec) * 1000) + ((now.tv_usec - start.tv_usec) / 1000);
        ret = waitpid(self->child, &status, WNOHANG);
    } while (ret == 0 && timeElapsed <= (TIMEOUT * 1000));
    if (ret == 0) {
        kill(self->child, SIGTERM);
        debug("CGI killed");
    }
    debug("CGI finished");
    close(self->out[1]);
    //if (ret == 0) throw HTTPException(408);
    //else if (status != 0) throw HTTPException(500);
}

std::string CGICall::computeRequestedFile() {
    std::string tmp = _request->getPath();
    construct_file_path(tmp);
    URI tmpUri(tmp);
    tmp = tmpUri.getFile();
    std::string exe, ext = set_extension(tmp);
    const std::map<std::string, std::string> & exts = Configuration::getInstance().get_cgi_bin_map();
    ext = "." + ext;
    try {
        exe = exts.at(ext);
    } catch (std::out_of_range &) {
        if (isFolder(tmp)) {
            if (_dir_listing) {
                exe = "directory_listing.php";
                construct_file_path(exe);
            } else if (!_server_index.empty()) {
                exe = _server_index;
                construct_file_path(exe);
            } else throw HTTPException(401);
        } else {
            exe = tmp;
        }
    }
    return exe;
}

std::string CGICall::computeScriptDirectory() {
    return _server_location_log;
}

HTTPHeader CGICall::parseCGIResponse(std::stringstream & s) {
    HTTPHeader header;
    header.setStatusCode(200);
    header.setStatusMessage(get_message(200));
    std::map<std::string, std::string> vars;
    std::string line;
    while (std::getline(s, line) && !(line.empty() || line == "\r")) {
        unsigned long i = line.find(':');
        if (i != std::string::npos) {
            std::string varName = line.substr(0, i);
            i = skipWhitespaces(line, ++i);
            std::string arg = line.substr(i, line.size());
            if (vars.find(varName) == vars.end()) {
                vars[varName] = arg;
            } else throw HTTPException(500);
        }
    }
    if (vars.empty()) throw HTTPException(500);
    for (std::map<std::string, std::string>::const_iterator it = vars.begin(); it != vars.end(); ++it) {
        const std::string & varName = it->first;
        const std::string & arg = it->second;
        if (varName == "Content-Type") header.set_content_type(arg);
        else if (varName == "Status") {
            const char * const str = arg.c_str();
            char * pos;
            long status = strtol(str, &pos, 10);
            header.setStatusCode(static_cast<int>(status));
            unsigned long i = pos - str;
            i = skipWhitespaces(arg, i);
            header.setStatusMessage(arg.substr(i, arg.size()));
        } else if (varName == "Content-Length") {
            header.set_content_length(static_cast<int>(strtol(arg.c_str(), NULL, 10)));
        } else if (varName == "Connection") {
            header.setConnection(arg);
        } else if (varName == "Transfer-Encoding") {
            header.setTransferEncoding(arg);
        } else if (varName == "Content-Encoding") {
            header.setContentEncoding(arg);
        } else if (varName.compare(0, 6, "X-CGI-") == 0) {
            // Ignore...
        } else throw HTTPException(500);
    }
    return header;
}

/*HTTPHeader CGICall::parseCGIResponse(const int fd) {
    HTTPHeader header;
    header.setStatusCode(200);
    header.setStatusMessage(get_message(200));
    std::map<std::string, std::string> vars;
    std::string line;
    while (!(line = nextLine(fd)).empty()) {
        unsigned long i = line.find(':');
        if (i != std::string::npos) {
            std::string varName = line.substr(0, i);
            i = skipWhitespaces(line, ++i);
            std::string arg = line.substr(i, line.size());
            if (vars.find(varName) == vars.end()) {
                vars[varName] = arg;
            } else throw HTTPException(500);
        }
    }
    if (vars.empty()) throw HTTPException(500);
    for (std::map<std::string, std::string>::const_iterator it = vars.begin(); it != vars.end(); ++it) {
        const std::string & varName = it->first;
        const std::string & arg = it->second;
        if (varName == "Content-Type") header.set_content_type(arg);
        else if (varName == "Status") {
            const char * const str = arg.c_str();
            char * pos;
            long status = strtol(str, &pos, 10);
            header.setStatusCode(static_cast<int>(status));
            unsigned long i = pos - str;
            i = skipWhitespaces(arg, i);
            header.setStatusMessage(arg.substr(i, arg.size()));
        } else if (varName == "Content-Length") {
            header.set_content_length(static_cast<int>(strtol(arg.c_str(), NULL, 10)));
        } else if (varName == "Connection") {
            header.setConnection(arg);
        } else if (varName == "Transfer-Encoding") {
            header.setTransferEncoding(arg);
        } else if (varName == "Content-Encoding") {
            header.setContentEncoding(arg);
        } else if (varName.compare(0, 6, "X-CGI-") == 0) {
            // Ignore...
        } else throw HTTPException(500);
    }
    return header;
}*/

std::string CGICall::nextLine(const int fd) {
    std::string ret;
    ssize_t r;
    char c;
    while ((r = read(fd, &c, 1)) > 0 && c != '\n') {
        ret += c;
    }
    if (ret.back() == '\r') ret.erase(ret.end() - 1);
    if (r < 0) throw HTTPException(500);
    return ret;
}

unsigned long CGICall::skipWhitespaces(const std::string & str, unsigned long pos) {
    for (; pos < str.size() && isblank(str[pos]); ++pos);
    return pos;
}

bool CGICall::isFolder(const std::string & path) {
    DIR * d;
    d = opendir(path.c_str());
    if (d != NULL) {
        closedir(d);
        return true;
    }
    return false;
}

std::string CGICall::vectorToString(const std::vector<std::string> & vector) {
    std::string ret;
    for (std::vector<std::string>::const_iterator it = vector.begin(); it != vector.end(); ++it) {
        ret += *it;
        if ((it + 1) != vector.end()) {
            ret += ",";
        }
    }
    return ret;
}

int CGICall::close(int fd) {
    pipeFds.remove(fd);
    return ::close(fd);
}
