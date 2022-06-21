//
// Created by Manuel Hahn on 6/15/22.
//

#include <map>
#include <csignal>
#include "CGICall.hpp"
#include "CGIResponseError.hpp"
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
    // TODO Extensive checks before executing CGI!
    int in[2], out[2];
    if (pipe(in) < 0) throw HTTPException(500);
    if (pipe(out) < 0) {
        close(in[0]);
        close(in[1]);
        throw HTTPException(500);
    }
    write(in[1], _request->get_payload().c_str(), _request->get_payload().size());
    close(in[1]);
    execute(in[0], out[1]);
    int status;
    waitpid(child, &status, WUNTRACED);
    close(in[0]);
    close(out[1]);
    // Hacky way to do this, I know ;)
    std::string str = Socket(out[0]).read_socket();
    // TODO Parse the answer of the CGI Script
    HTTPHeader header;
    header.setConnection("close-connection");
    header.setStatusCode(200);
    header.setStatusMessage(get_message(200));
    header.set_content_type("text/plain");
    header.set_content_length(str.size());
    socket.send(header.tostring());
    socket.send("\r\n\r\n");
    socket.send(str);
}

void CGICall::execute(const int in, const int out) {
    child = fork();
    if (child < 0) throw HTTPException(500);
    if (child > 0) return;
    dup2(in, STDIN_FILENO);
    dup2(out, STDOUT_FILENO);
    dup2(out, STDERR_FILENO); // Or maybe not?
    char ** env = new char * [4]();
    env[0] = const_cast<char *>(method.c_str());
    env[1] = const_cast<char *>(protocol.c_str());
    env[2] = const_cast<char *>(pathinfo.c_str());
    const std::string filename = "/Users/mhahn/Documents/Programmierung/42/webserv/a.out";
    char ** args = new char * [2]();
    args[0] = const_cast<char *>(filename.c_str());
    if (execve(filename.c_str(), args, env) < 0) {
        CGIResponseError response;
        response.set_error_code(500);
	Socket s(out);
        response.run(s);
	close(in);
	close(out);
	exit(-1);
    }
}

bool CGICall::isRunning() {
    if (child == -1) return false;
    int status;
    const pid_t result = waitpid(child, &status, WNOHANG);
    return result == 0;
}
