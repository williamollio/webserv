//
// Created by Manuel Hahn on 6/10/22.
//

#ifndef WEBSERV_CGIRESPONSE_HPP
#define WEBSERV_CGIRESPONSE_HPP

#include "Socket.hpp"
#include "HTTPRequest.hpp"
#include "IOException.hpp"
#include "HTTPHeader.hpp"
#include "Tool.hpp"

class CGIResponse {
public:
    explicit CGIResponse(HTTPRequest &);
    virtual ~CGIResponse();

    virtual void run(Socket & socket) = 0;
	std::string set_server_location(std::string path_from_configuration) throw (IOException);
	std::string set_default_file(std::string file) throw (IOException);
	std::string read_file(std::string file) throw (IOException);

	void send_error_code(Socket & socket, const int & error_code);
	void send_error_message(Socket & socket, const std::string & message);
protected:
	const HTTPRequest	& _request;
	std::string			_server_location_log;
	std::string			_default_file;
};


#endif //WEBSERV_CGIRESPONSE_HPP
