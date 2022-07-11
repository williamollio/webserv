//
// Created by Manuel Hahn on 6/10/22.
//

#ifndef WEBSERV_CGIRESPONSEGET_HPP
#define WEBSERV_CGIRESPONSEGET_HPP

#include "CGIResponse.hpp"

class CGIResponseGet : public CGIResponse {
public:
    explicit CGIResponseGet(HTTPRequest*);

    void		run(Socket& socket);
	std::string set_file(std::string path, Socket& socket);
	std::string construct_content_type();
	bool		is_request_location(std::string path);
};


#endif //WEBSERV_CGIRESPONSEGET_HPP
