//
// Created by Manuel Hahn on 6/10/22.
//

#ifndef WEBSERV_CGIRESPONSEDELETE_HPP
#define WEBSERV_CGIRESPONSEDELETE_HPP

#include "CGIResponse.hpp"

class CGIResponseDelete : public CGIResponse {
public:
	explicit CGIResponseDelete(HTTPRequest *);

	void run(Socket &socket);
	void send_response(Socket &socket);
	void extract_path();
	void set_up_location();
private:
	std::string _sub_path;
	std::string _file;
	const char *_location_folder;
	const char *_location_file;
};


#endif //WEBSERV_CGIRESPONSEDELETE_HPP
