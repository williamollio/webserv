//
// Created by Manuel Hahn on 6/10/22.
//

#ifndef WEBSERV_CGIRESPONSEPOST_HPP
#define WEBSERV_CGIRESPONSEPOST_HPP


#include "CGIResponse.hpp"
#include "HTTPRequestPost.hpp"

class CGIResponsePost : public CGIResponse {
public:
    explicit CGIResponsePost(HTTPRequest &);

    void run(Socket &socket);
};


#endif //WEBSERV_CGIRESPONSEPOST_HPP
