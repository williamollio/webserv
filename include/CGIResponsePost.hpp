//
// Created by Manuel Hahn on 6/10/22.
//

#ifndef WEBSERV_CGIRESPONSEPOST_HPP
#define WEBSERV_CGIRESPONSEPOST_HPP


#include "CGIResponse.hpp"
#include "HTTPRequestPost.hpp"
#include <dirent.h>
#include <errno.h>
#include <fstream>

class CGIResponsePost : public CGIResponse {
public:
    explicit CGIResponsePost(HTTPRequest *);

    void run(Socket &socket);

private:
	std::string _filename;

	void saveFile(std::string payload);
	std::string setFilename(std::string payload);
	void trim_payload(std::string &payload);
	std::string get_delimiter(std::string &tmp);
	void create_file(std::string &payload);
};


#endif //WEBSERV_CGIRESPONSEPOST_HPP
