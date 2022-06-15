//
// Created by Manuel Hahn on 6/15/22.
//

#ifndef WEBSERV_CGICALL_HPP
#define WEBSERV_CGICALL_HPP

#include "CGIResponse.hpp"

class CGICall: public CGIResponse {
public:
    CGICall(HTTPRequest &);
    ~CGICall();

    void run(Socket &socket);
};


#endif //WEBSERV_CGICALL_HPP
