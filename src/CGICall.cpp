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
    std::map<std::string, std::string> vars = uri.getVars();
    switch (_request->getType()) {
        case HTTPRequest::GET:    method += "GET";    break;
        case HTTPRequest::POST:   method += "POST";   break;
        case HTTPRequest::DELETE: method += "DELETE"; break;
    }
    protocol = "SERVER_PROTOCOL=HTTP/1.1";
    pathinfo = "PATH_INFO=" + uri.getFile();
    HTTPHeader header;
    header.setStatusMessage(get_message(200));
    header.setStatusCode(200);
    header.setConnection("keep-alive");
    header.set_content_type("plain/text");
    header.set_content_length(500);
    socket.send(header.tostring() + "\r\n\r\n");
    execute(socket);
}

void CGICall::execute(const Socket & socket) {
    child = fork();
    if (child < 0) throw HTTPException(500);
    if (child > 0) return;
    dup2(socket.get_fd(), STDIN_FILENO);
    dup2(socket.get_fd(), STDOUT_FILENO);
    dup2(socket.get_fd(), STDERR_FILENO);
    char ** env = new char * [4]();
    env[0] = const_cast<char *>(method.c_str());
    env[1] = const_cast<char *>(protocol.c_str());
    env[2] = const_cast<char *>(pathinfo.c_str());
    const std::string filename = "/Users/mhahn/Documents/webserv/a.out";
    char ** args = new char * [2]();
    args[0] = const_cast<char *>(filename.c_str());
    if (execve(filename.c_str(), args, env) < 0) throw HTTPException(500);
}

bool CGICall::isRunning() {
    if (child == -1) return false;
    int status;
    const pid_t result = waitpid(child, &status, WNOHANG);
    if (result < 0) throw HTTPException(500);
    return result == 0;
}
