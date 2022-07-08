//
// Created by Manuel Hahn on 6/10/22.
//

#ifndef WEBSERV_CGIRESPONSEGET_HPP
#define WEBSERV_CGIRESPONSEGET_HPP

#include "CGIResponse.hpp"

class CGIResponseGet : public CGIResponse {
private:
	std::string _file_extension;
public:
    explicit CGIResponseGet(const HTTPRequest *);

    void run(Socket & socket);
	std::string set_file(std::string path, Socket & socket);
	std::string construct_content_type();
};


#endif //WEBSERV_CGIRESPONSEGET_HPP
