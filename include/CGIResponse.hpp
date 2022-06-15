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
	std::string set_server_location(std::string path_from_configuration);

protected:
	const HTTPRequest	& _request;
	std::string			_server_location_log;
};


#endif //WEBSERV_CGIRESPONSE_HPP
