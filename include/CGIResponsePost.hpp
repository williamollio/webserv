//
// Created by Manuel Hahn on 6/10/22.
//

#ifndef WEBSERV_CGIRESPONSEPOST_HPP
#define WEBSERV_CGIRESPONSEPOST_HPP


#include "CGIResponse.hpp"

class CGIResponsePost : public CGIResponse {
public:
    explicit CGIResponsePost(HTTPRequest *, Socket &, Runnable &);

    void run();

private:
	std::string _filename;
	std::string _upload;
    std::string _payload;
    size_t      _payloadCounter;
	size_t		_max_size_body;
	bool		_max_size_body_bool;

	void saveFile(std::string payload);
	std::string setFilename(std::string &payload);
	std::string setFilenameUnknown(std::string extension);
	void trimPayload(std::string &payload);
	std::string getDelimiter(std::string &tmp);
	void createFile(std::string &payload);
	bool isUploadAccepted();
	bool isBodySizeForbidden(size_t payload_size);
	void get_date(std::string &time);

public:
    bool runForFD(int i);
};


#endif //WEBSERV_CGIRESPONSEPOST_HPP
