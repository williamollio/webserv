//
// Created by Manuel Hahn on 6/15/22.
//

#include <map>
#include <csignal>
#include "CGICall.hpp"
#include "HTTPException.hpp"

CGICall::CGICall(HTTPRequest * request)
        : CGIResponse(request), uri(request->getURI()), method("REQUEST_METHOD="), child(-1) {}

CGICall::~CGICall() {
    if (child > -1) {
        kill(child, SIGTERM);
    }
}

void CGICall::run(Socket & socket) {
    switch (_request->getType()) {
        case HTTPRequest::GET:    method += "GET";    break;
        case HTTPRequest::POST:   method += "POST";   break;
        case HTTPRequest::DELETE: method += "DELETE"; break;
    }
    const std::string pwd = getcwd(NULL, 0);
    protocol = "SERVER_PROTOCOL=HTTP/1.1";
    gatewayInterface = "GATEWAY_INTERFACE=CGI/1.1";
    pathinfo = "PATH_INFO=" + uri.getPathInfo();
    queryString = "QUERY_STRING=" + uri.getQuery();
    remoteAddress = "REMOTE_ADDR=" + int_to_ipv4(_request->getPeerAddress());
    remoteHost = "REMOTE_HOST=" + int_to_ipv4(_request->getPeerAddress());
    scriptName = "SCRIPT_NAME=" + pwd + uri.getFile();
    serverName = "SERVER_NAME=" + _request->_host;
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
    int in[2], out[2];
    if (pipe(in) < 0) throw HTTPException(500);
    if (pipe(out) < 0) {
        close(in[0]);
        close(in[1]);
        throw HTTPException(500);
    }
    write(in[1], _request->get_payload().c_str(), _request->get_payload().size());
    close(in[1]);
    execute(in[0], out[1], requestedFile);
    int status;
    waitpid(child, &status, WUNTRACED);
    close(in[0]);
    close(out[1]);
    if (status != 0) throw HTTPException(500);
    socket.send(parseCGIResponse(out[0]).tostring());
    socket.send("\r\n\r\n");
    ssize_t r, w;
    char b;
    while ((r = read(out[0], &b, 1)) > 0) {
        w = write(socket.get_fd(), &b, 1);
    }
    // TODO: @team please discuss asynchronous CGI @Done, result: yes, using multithreading!
    close(out[0]);
    if (r < 0 || w < 0) throw HTTPException(500);
}

void CGICall::execute(const int in, const int out, const std::string & requestedFile) {
    child = fork();
    if (child < 0) throw HTTPException(500);
    if (child > 0) return;
    dup2(in, STDIN_FILENO);
    dup2(out, STDOUT_FILENO);
    close(in);
    close(out);
    char ** env = new char * [_request->_content ? 14 : 12]();
    env[0]  = const_cast<char *>(method.c_str());
    env[1]  = const_cast<char *>(protocol.c_str());
    env[2]  = const_cast<char *>(pathinfo.c_str());
    env[3]  = const_cast<char *>(gatewayInterface.c_str());
    env[4]  = const_cast<char *>(queryString.c_str());
    env[5]  = const_cast<char *>(scriptName.c_str());
    env[6]  = const_cast<char *>(serverName.c_str());
    env[7]  = const_cast<char *>(serverPort.c_str());
    env[8]  = const_cast<char *>(serverSoftware.c_str());
    env[9]  = const_cast<char *>(remoteHost.c_str());
    env[10] = const_cast<char *>(remoteAddress.c_str());
    if (_request->_content) {
        env[11] = const_cast<char *>(contentLength.c_str());
        env[12] = const_cast<char *>(contentType.c_str());
    }
    char ** args = new char * [2]();
    args[0] = const_cast<char *>(requestedFile.c_str());
    if (execve(requestedFile.c_str(), args, env) < 0) {
        exit(-1);
    }
}

bool CGICall::isRunning() {
    if (child == -1) return false;
    int status;
    const pid_t result = waitpid(child, &status, WNOHANG);
    return result == 0;
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
