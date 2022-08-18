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

CGICall::CGICall(HTTPRequest * request, Socket & socket, Runnable & parent)
        : CGIResponse(request, socket, parent),
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
          httpUserAgent("HTTP_USER_AGENT="),
          httpHost("HTTP_HOST="),
          httpLang("HTTP_ACCEPT_LANGUAGE="),
          httpEncoding("HTTP_ACCEPT_ENCODING="),
          httpAccept("HTTP_ACCEPT="),
          httpConnection("HTTP_CONNECTION="),
          httpContentLength("HTTP_CONTENT_LENGTH="),
          httpExpect("HTTP_EXCEPT="),
		  x_arguments_name(request->getXArgsName()),
          socket(socket),
          child(-1),
          threadID(),
          in(),
          out(),
          payloadCounter(),
          socketCounter(),
          error(NULL),
          timedOut(false),
          timedOutMutex() {
    pthread_mutex_init(&timedOutMutex, NULL);
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
        pthread_cancel(threadID);
    }
    pthread_mutex_destroy(&timedOutMutex);
    pthread_join(threadID, NULL);
}

bool CGICall::writePayload(bool hup) {
    if (hup) {
        close(in[1]);
        return true;
    }
    ssize_t ret = write(in[1], _request->get_payload().c_str() + payloadCounter, _request->get_payload().size() - payloadCounter < 65536 ? _request->get_payload().size() - payloadCounter : 65536);
    if (ret < 0) {
        throw IOException("Fatal error!");
    }
    payloadCounter += ret;
    if (payloadCounter < _request->get_payload().size()) {
        return false;
    }
    debug("Write with fd " << in[1] << " size " << payloadCounter);
    debug("Closing server -> cgi");
    close(in[1]);
    return true;
}

bool CGICall::readPayload(bool hup) {
    if (hup) {
        close(out[0]);
        if (timedOut) {
            sendError(408);
        } else {
            debug("Processing CGI output (" << buffer.size() << " bytes)");
            processCGIOutput();
        }
        return true;
    } else {
        ssize_t ret = read(out[0], rawBuffer, 65536);
        if (ret < 0) {
            throw IOException("Fatal error!");
        }
        buffer.append(rawBuffer, ret);
        return false;
    }
}

bool CGICall::writeSocket(bool hup) {
    if (hup) {
        _socket.close();
        return true;
    }
    try {
        ssize_t ret = socket.write(payload.c_str() + socketCounter, payload.size() - socketCounter < 65536 ? payload.size() - socketCounter : 65536);
        socketCounter += ret;
        if (socketCounter < payload.size()) {
            return false;
        }
        debug("Write with socket fd " << socket.get_fd() << " size " << socketCounter << " real " << payload.size());
        debug("Closing socket fd " << socket.get_fd());
        socket.close();
        return true;
    } catch (IOException &) {
        debug("Write with socket fd " << socket.get_fd() << " size " << socketCounter);
        return false;
    }
}

bool CGICall::runForFD(int fd, bool hup) {
    if (fd == in[1]) {
        return writePayload(hup);
    } else if (fd == out[0]) {
        return readPayload(hup);
    } else if (fd == socket.get_fd()) {
        return writeSocket(hup);
    } else {
        return true;
    }
}

void CGICall::run() {
    switch (_request->getType()) {
        case HTTPRequest::GET:    method += "GET";    break;
        case HTTPRequest::POST:   method += "POST";   break;
        case HTTPRequest::DELETE: method += "DELETE"; break;
		default: throw HTTPException(500);
    }
    const std::string combined = uri.getPathInfo().empty() ? "/Arsch" : uri.getPathInfo();
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
    if (httpHost.back() == '\r') httpHost.pop_back();
    httpLang += vectorToString(_request->getLang());
    httpEncoding += vectorToString(_request->getEncoding());
    httpAccept += vectorToString(_request->getContentType());
    httpContentLength += int_to_string(static_cast<int>(_request->getContentLength()));
    httpExpect += _request->getExpect();
    httpConnection += _request->isKeepAlive() ? "keep-alive" : "";
    scriptName += computeRequestedFile();
	x_arguments = _request->getXArgs();
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
    fcntl(in[1], F_SETFL, O_NONBLOCK);
    Connection::getInstance().add_fd(in[1], this, false);
    fcntl(out[0], F_SETFL, O_NONBLOCK);
    Connection::getInstance().add_fd(out[0], this);
    execute(in[0], out[1], requestedFile);
    close(in[0]);
    close(out[1]);
    pthread_create(&threadID, NULL, reinterpret_cast<void *(*)(void *)>(CGICall::async), this);
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
        payload = header.tostring() + "\r\n\r\n" + payload;
        Connection::getInstance().add_fd(socket.get_fd(), this, false);
        cleanUp = false;
    } catch (HTTPException & ex) {
        sendError(ex.get_error_code());
    } catch (std::exception & ex) {
        std::clog << "INFO: Socket has been closed" << std::endl
                  << "INFO: " << ex.what()          << std::endl;
    }
    if (cleanUp) {
        debug("Closing socket (fd: " << socket.get_fd() << ")");
        socket.close();
    }
}

void CGICall::sendError(const int errorCode) {
    try {
        error = new CGIResponseError(_socket, *this);
        error->set_error_code(errorCode);
        error->run();
    } catch (IOException & exception) {
        std::clog << "INFO: Socket has been closed" << std::endl
                  << "INFO: " << exception.what()   << std::endl;
    } catch (HTTPException & exception) {
        std::clog << "INFO: " << exception.what() << std::endl;
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
    char ** environment = new char * [(_request->hasContent() ? 23 : 21) + x_arguments.size()]();
	int i = 0;
    environment[i++]  = strdup(method.c_str());
    environment[i++]  = strdup(protocol.c_str());
    environment[i++]  = strdup(pathinfo.c_str());
    environment[i++]  = strdup(gatewayInterface.c_str());
    environment[i++]  = strdup(queryString.c_str());
    environment[i++]  = strdup(serverName.c_str());
    environment[i++]  = strdup(serverPort.c_str());
    environment[i++]  = strdup(serverSoftware.c_str());
    environment[i++]  = strdup(remoteHost.c_str());
    environment[i++]  = strdup(remoteAddress.c_str());
    environment[i++] = strdup(httpConnection.c_str());
    environment[i++] = strdup(httpExpect.c_str());
    environment[i++] = strdup(httpContentLength.c_str());
    environment[i++] = strdup(httpAccept.c_str());
    environment[i++] = strdup(httpEncoding.c_str());
    environment[i++] = strdup(httpLang.c_str());
    environment[i++] = strdup(httpHost.c_str());
    environment[i++] = strdup(httpUserAgent.c_str());
    environment[i++] = strdup(requestUri.c_str());
    environment[i++] = strdup(scriptName.c_str());
    if (_request->hasContent()) {
        environment[i++] = strdup(contentLength.c_str());
        environment[i++] = strdup(contentType.c_str());
    }
	std::string tmp;
	for (size_t ii = 0; ii < x_arguments.size(); ii++) {
		tmp = "HTTP_" + x_arguments_name.at(ii);
        for (std::string::iterator it = tmp.begin(); it != tmp.end(); ++it) {
            if (*it == '-') {
                *it = '_';
            } else {
                *it = static_cast<char>(toupper(*it));
            }
        }
		tmp += "=";
		tmp += x_arguments[ii];
		environment[i++] = strdup(tmp.c_str());
		tmp.clear();
	}
    arguments[0] = strdup(requestedFile.c_str());
    chdir(computeScriptDirectory().c_str());
    if (execve(requestedFile.c_str(), arguments, environment) < 0) {
        exit(-1);
    }
}

void CGICall::async(CGICall * self) {
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
        pthread_mutex_lock(&self->timedOutMutex);
        kill(self->child, SIGTERM);
        debug("CGI killed");
        self->timedOut = true;
        pthread_mutex_unlock(&self->timedOutMutex);
    }
    debug("CGI finished");
    close(self->out[1]);
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
        if (line.back() == '\r') line = std::string(line.c_str(), line.size() - 1);
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
