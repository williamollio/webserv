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

CGICall::CGICall(HTTPRequest * request)
        : CGIResponse(request),
          uri(request->getURI()),
          method("REQUEST_METHOD="),
          protocol("SERVER_PROTOCOL=HTTP/1.1"),
          pathinfo("PATH_INFO="),
          gatewayInterface("GATEWAY_INTERFACE=CGI/1.1"),
          queryString("QUERY_STRING="),
          scriptName("SCRIPT_NAME="),
          serverName("SERVER_NAME="),
          serverSoftware("SERVER_SOFTWARE=webserv/1.0 (2022/06)"),
          remoteAddress("REMOTE_ADDR="),
          remoteHost("REMOTE_HOST="),
          child(-1),
          threadID(),
          in(),
          out(),
          running(false),
          runningMutex() {
    pthread_mutex_init(&runningMutex, NULL);
}

CGICall::~CGICall() {
    if (child > -1) {
        kill(child, SIGTERM);
    }
    pthread_cancel(threadID);
    pthread_join(threadID, NULL);
    pthread_mutex_destroy(&runningMutex);
}

void CGICall::run(Socket & _socket) {
    socket = _socket;
    switch (_request->getType()) {
        case HTTPRequest::GET:    method += "GET";    break;
        case HTTPRequest::POST:   method += "POST";   break;
        case HTTPRequest::DELETE: method += "DELETE"; break;
		default: throw HTTPException(500);
    }
    pathinfo += uri.getPathInfo();
    queryString += uri.getQuery();
    remoteAddress += int_to_ipv4(_request->getPeerAddress());
    remoteHost += _request->getPeerName();
    serverName += Configuration::getInstance().get_server_names().at(0);
    {
        std::stringstream s;
        s << "SERVER_PORT=" << _request->getUsedPort();
        serverPort = s.str();
    }
    if (_request->_content) {
        std::stringstream s;
        s << "CONTENT_LENGTH=" << _request->_content_length;
        contentLength = s.str();
        contentType = "CONTENT_TYPE=";
        for (std::vector<std::string>::const_iterator it = _request->_content_type.begin(); it != _request->_content_type.end(); ++it) {
            contentType += *it + ",";
        }
    }
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
    running = true;
    write(in[1], _request->get_payload().c_str(), _request->get_payload().size());
    close(in[1]);
    execute(in[0], out[1], requestedFile);
    pthread_create(&threadID, NULL, reinterpret_cast<void *(*)(void *)>(CGICall::async), this);
}

void CGICall::async(CGICall * self) {
    try {
        self->waitOrThrow();
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
}

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
    close(in);
    close(out);
    char ** arguments = new char * [2]();
    char ** environment = new char * [_request->_content ? 14 : 12]();
    environment[0]  = strdup(method.c_str());
    environment[1]  = strdup(protocol.c_str());
    environment[2]  = strdup(pathinfo.c_str());
    environment[3]  = strdup(gatewayInterface.c_str());
    environment[4]  = strdup(queryString.c_str());
    environment[5]  = strdup(scriptName.c_str());
    environment[6]  = strdup(serverName.c_str());
    environment[7]  = strdup(serverPort.c_str());
    environment[8]  = strdup(serverSoftware.c_str());
    environment[9]  = strdup(remoteHost.c_str());
    environment[10] = strdup(remoteAddress.c_str());
    if (_request->_content) {
        environment[11] = strdup(contentLength.c_str());
        environment[12] = strdup(contentType.c_str());
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

void CGICall::waitOrThrow() {
    int status, ret;
    unsigned int timeElapsed;
    struct timeval start, now;
    gettimeofday(&start, NULL);
    do {
        usleep(100000);
        gettimeofday(&now, NULL);
        timeElapsed = ((now.tv_sec - start.tv_sec) * 1000) + ((now.tv_usec - start.tv_usec) / 1000);
        ret = waitpid(child, &status, WNOHANG);
    } while (ret == 0 && timeElapsed <= (TIMEOUT * 1000));
    if (ret == 0) {
        kill(child, SIGTERM);
    }
    close(in[0]);
    close(out[1]);
    if (ret == 0) throw HTTPException(408);
    else if (status != 0) throw HTTPException(500);
}

std::string CGICall::computeRequestedFile() {
    char * c_pwd = getcwd(NULL, 0);
    const std::string ret = c_pwd + Configuration::getInstance().get_server_root_folder();
    free(c_pwd);
    return ret +uri.getFile();// (uri.isFolder() ? "/cgi/directory_listing.php" : uri.getFile());
}

std::string CGICall::computeScriptDirectory() {
    char * c_pwd = getcwd(NULL, 0);
    const std::string ret = c_pwd + Configuration::getInstance().get_server_root_folder() + uri.getFileDirectory();
    free(c_pwd);
    return ret;
}

HTTPHeader CGICall::parseCGIResponse(const int fd) {
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
}

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
