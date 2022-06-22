//
// Created by Manuel Hahn on 6/10/22.
//

#ifndef WEBSERV_CGIRESPONSE_HPP
#define WEBSERV_CGIRESPONSE_HPP

#include "Socket.hpp"
#include "HTTPRequest.hpp"
#include "HTTPException.hpp"
#include "HTTPHeader.hpp"
#include "Tool.hpp"

class CGIResponse {
public:
    explicit CGIResponse(HTTPRequest *);
    virtual ~CGIResponse();

    virtual void run(Socket & socket) = 0;
    virtual bool isRunning();
	std::string  set_server_location(std::string path_from_configuration);
	std::string  set_default_file(std::string file);
	std::string  read_file(std::string file);

protected:
	const HTTPRequest * _request;
	std::string         _server_location_log;
	std::string         _default_file;
};


#endif //WEBSERV_CGIRESPONSE_HPP
