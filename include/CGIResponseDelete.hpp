//
// Created by Manuel Hahn on 6/10/22.
//

#ifndef WEBSERV_CGIRESPONSEDELETE_HPP
#define WEBSERV_CGIRESPONSEDELETE_HPP

#include "CGIResponse.hpp"
#include "HTTPRequestDelete.hpp"

class CGIResponseDelete : public CGIResponse {
public:
    explicit CGIResponseDelete(HTTPRequest &);

    void run(Socket &socket);
};


#endif //WEBSERV_CGIRESPONSEDELETE_HPP
