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
	std::string setFilename(std::string &payload);
	void trimPayload(std::string &payload);
	std::string getDelimiter(std::string &tmp);
	void createFile(std::string &payload);
	bool isUploadAccepted();
};


#endif //WEBSERV_CGIRESPONSEPOST_HPP
