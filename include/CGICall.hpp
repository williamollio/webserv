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
    pid_t       child;

    void childExecute(int, int);
    void execute(int, int);

    static std::string   nextLine(int);
    static HTTPHeader    parseCGIResponse(int);
    static unsigned long skipWhitespaces(const std::string &, unsigned long);
};


#endif //WEBSERV_CGICALL_HPP
