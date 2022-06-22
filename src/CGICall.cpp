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
    socket.send(parseCGIResponse(out[0]).tostring());
    socket.send("\r\n\r\n");
    ssize_t r, w;
    char b;
    while ((r = read(out[0], &b, 1)) > 0) {
        w = write(socket.get_fd(), &b, 1);
    }
    // TODO: @team please discuss asynchronous CGI
    close(out[0]);
    if (r < 0 || w < 0) throw HTTPException(500);
}

void CGICall::execute(const int in, const int out) {
    child = fork();
    if (child < 0) throw HTTPException(500);
    if (child > 0) return;
    int code = 0;
    try {
        childExecute(in, out);
    } catch (std::exception & ex) {
        CGIResponseError response;
        response.set_error_code(500);
        Socket s(out);
        response.run(s);
        code = -1;
    }
    exit(code);
}

void CGICall::childExecute(const int in, const int out) {
    dup2(in, STDIN_FILENO);
    dup2(out, STDOUT_FILENO);
    close(in);
    close(out);
    char ** env = new char * [4]();
    env[0] = const_cast<char *>(method.c_str());
    env[1] = const_cast<char *>(protocol.c_str());
    env[2] = const_cast<char *>(pathinfo.c_str());
    const std::string filename = "/Users/mhahn/Documents/webserv/test.pl";
    char ** args = new char * [2]();
    args[0] = const_cast<char *>(filename.c_str());
    if (execve(filename.c_str(), args, env) < 0) {
        throw HTTPException(500);
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
        } else throw HTTPException(500);
        // TODO Add some more...
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
