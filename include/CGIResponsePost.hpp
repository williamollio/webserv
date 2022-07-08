//
// Created by Manuel Hahn on 6/10/22.
//

#ifndef WEBSERV_CGIRESPONSEPOST_HPP
#define WEBSERV_CGIRESPONSEPOST_HPP


#include "CGIResponse.hpp"
#include "HTTPRequestPost.hpp"

class CGIResponsePost : public CGIResponse {
public:
    explicit CGIResponsePost(const HTTPRequest *);

    void run(Socket &socket);

private:
	std::string _filename;

	void saveFile(std::string payload);
	std::string setFilename(std::string &payload);
	std::string setFilenameUnknown(std::string extension);
	void trimPayload(std::string &payload);
	std::string getDelimiter(std::string &tmp);
	void createFile(std::string &payload);
	bool isUploadAccepted();
};


#endif //WEBSERV_CGIRESPONSEPOST_HPP
