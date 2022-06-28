//
// Created by Manuel Hahn on 6/15/22.
//

#include <map>
#include <csignal>
#include <sys/time.h>
#include "CGICall.hpp"
#include "HTTPException.hpp"
#include "CGIResponseError.hpp"

CGICall::CGICall(HTTPRequest * request)
        : CGIResponse(request),
          uri(request->getURI()),
          method("REQUEST_METHOD="),
          child(-1),
          threadID(),
          in(),
          out(),
          running(false),
          runningMutex(),
          environment(NULL), arguments(NULL) {
    pthread_mutex_init(&runningMutex, NULL);
}

CGICall::~CGICall() {
    pthread_cancel(threadID);
    pthread_join(threadID, NULL);
    if (child > -1) {
        kill(child, SIGTERM);
    }
    pthread_mutex_destroy(&runningMutex);
    if (environment != NULL) delete[] environment;
    if (arguments != NULL) delete[] arguments;
}

void CGICall::run(Socket & _socket) {
    socket = _socket;
    switch (_request->getType()) {
        case HTTPRequest::GET:    method += "GET";    break;
        case HTTPRequest::POST:   method += "POST";   break;
        case HTTPRequest::DELETE: method += "DELETE"; break;
    }
    const char * c_pwd = getcwd(NULL, 0);
    const std::string pwd = c_pwd;
    delete c_pwd;
    protocol = "SERVER_PROTOCOL=HTTP/1.1";
    gatewayInterface = "GATEWAY_INTERFACE=CGI/1.1";
    pathinfo = "PATH_INFO=" + uri.getPathInfo();
    queryString = "QUERY_STRING=" + uri.getQuery();
    remoteAddress = "REMOTE_ADDR=" + int_to_ipv4(_request->getPeerAddress());
    remoteHost = "REMOTE_HOST=" + _request->getPeerName();
    scriptName = "SCRIPT_NAME=" + pwd + uri.getFile();
    serverName = "SERVER_NAME=" + _request->_host;
    arguments = new char * [2]();
    environment = new char * [_request->_content ? 14 : 12]();
    serverSoftware = "SERVER_SOFTWARE=webserv/1.0 (2022/06)";
    {
        std::stringstream s;
        s << "SERVER_PORT=" << 80; // TODO: Get the correct port!
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
    const std::string & requestedFile = pwd + uri.getFile();
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
        self->socket.send(parseCGIResponse(self->out[0]).tostring());
        self->socket.send("\r\n\r\n");
        ssize_t r, w;
        char b;
        while ((r = read(self->out[0], &b, 1)) > 0) {
            if ((w = write(self->socket.get_fd(), &b, 1)) < 0) break;
        }
        if (r < 0 || w < 0) throw IOException("Could not send or read!");
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
    environment[0]  = const_cast<char *>(method.c_str());
    environment[1]  = const_cast<char *>(protocol.c_str());
    environment[2]  = const_cast<char *>(pathinfo.c_str());
    environment[3]  = const_cast<char *>(gatewayInterface.c_str());
    environment[4]  = const_cast<char *>(queryString.c_str());
    environment[5]  = const_cast<char *>(scriptName.c_str());
    environment[6]  = const_cast<char *>(serverName.c_str());
    environment[7]  = const_cast<char *>(serverPort.c_str());
    environment[8]  = const_cast<char *>(serverSoftware.c_str());
    environment[9]  = const_cast<char *>(remoteHost.c_str());
    environment[10] = const_cast<char *>(remoteAddress.c_str());
    if (_request->_content) {
        environment[11] = const_cast<char *>(contentLength.c_str());
        environment[12] = const_cast<char *>(contentType.c_str());
    }
    arguments[0] = const_cast<char *>(requestedFile.c_str());
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
//    if (child == -1) return false;
//    int status;
//    const pid_t result = waitpid(child, &status, WNOHANG);
//    return result == 0;
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
        std::cerr << timeElapsed << std::endl;
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
