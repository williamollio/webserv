//
// Created by Manuel Hahn on 6/15/22.
//

#ifndef WEBSERV_CGICALL_HPP
#define WEBSERV_CGICALL_HPP

#include "CGIResponse.hpp"

class CGICall: public CGIResponse {
public:
    explicit CGICall(HTTPRequest *);
    ~CGICall();

    void run(Socket & socket);
    bool isRunning();

private:
    const URI & uri;
    std::string method;
    std::string protocol;
    std::string pathinfo;
    std::string contentLength;
    std::string contentType;
    std::string gatewayInterface;
    std::string queryString;
    std::string scriptName;
    std::string serverName;
    std::string serverPort;
    std::string serverSoftware;
    pid_t       child;

    void execute(int, int, const std::string &);

    static std::string   nextLine(int);
    static HTTPHeader    parseCGIResponse(int);
    static unsigned long skipWhitespaces(const std::string &, unsigned long);
};


#endif //WEBSERV_CGICALL_HPP
