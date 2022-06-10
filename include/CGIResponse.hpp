//
// Created by Manuel Hahn on 6/10/22.
//

#ifndef WEBSERV_CGIRESPONSE_HPP
#define WEBSERV_CGIRESPONSE_HPP

#include "Socket.hpp"
#include "HTTPRequest.hpp"

class CGIResponse {
public:
    explicit CGIResponse(HTTPRequest &);
    virtual ~CGIResponse();

    virtual void run(Socket & socket) = 0;

private:
    const HTTPRequest & _request;
};


#endif //WEBSERV_CGIRESPONSE_HPP
