//
// Created by Manuel Hahn on 6/10/22.
//

#ifndef WEBSERV_CGIRESPONSEGET_HPP
#define WEBSERV_CGIRESPONSEGET_HPP

#include "CGIResponse.hpp"

class CGIResponseGet : public CGIResponse {
public:
    explicit CGIResponseGet(HTTPRequest &);

    void run(Socket & socket);
    std::string read_file(std::string file);
};


#endif //WEBSERV_CGIRESPONSEGET_HPP
