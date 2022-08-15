//
// Created by Manuel Hahn on 6/10/22.
//

#ifndef WEBSERV_CGIRESPONSEDELETE_HPP
#define WEBSERV_CGIRESPONSEDELETE_HPP

#include "CGIResponse.hpp"

class CGIResponseDelete : public CGIResponse {
public:
	explicit CGIResponseDelete(HTTPRequest *, Socket &);

	void run();
	void send_response();
	void extract_path();
	void set_up_location();

    bool runForFD(int i);
    bool isRunning();

private:
	std::string _sub_path;
	std::string _file;
    std::string _payload;
    unsigned long _payloadCounter;
    bool        _running;
	const char *_location_folder;
	const char *_location_file;
};


#endif //WEBSERV_CGIRESPONSEDELETE_HPP
