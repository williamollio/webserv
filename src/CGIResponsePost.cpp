//
// Created by Manuel Hahn on 6/10/22.
//

#include "../include/CGIResponsePost.hpp"

std::string CGIResponsePost::setFilename(std::string payload) {
	std::string filename;

	size_t posbegin, posend;
	posbegin = payload.find("filename=\"");
	if (posbegin == std::string::npos)
		throw HTTPException(400);
	posbegin += 10;
	posend = payload.find("\"", posbegin + 1);
	filename = payload.substr(posbegin, posend - posbegin);
	return (filename);
}

void CGIResponsePost::trim_payload(std::string &payload) {
	std::string tmp(payload);
	payload.clear();
	size_t posbegin, posend;

	std::string firstline;
	size_t first, second;

	first = tmp.find("\n");
	second = tmp.find("\n", first+1);
	std::cout << "first: " << first << std::endl;
	std::cout << "second: " << second << std::endl;

	std::string test;
	test = tmp.substr(first+1, second - first);
	test.erase(test.length() - 2);
	posbegin = tmp.find("\r\n\r\n");
	if (posbegin == std::string::npos)
		throw HTTPException(400);
	posbegin += 4;
	std::cout << "$" << test << "$" << std::endl;
	posend = tmp.find(test, posbegin);
	std::cout << "posend: " << posend << std::endl;
	payload = tmp.substr(posbegin, posend - posbegin);
	std::cout << "AFTER payload: \n$" << payload << std::endl;
}

void CGIResponsePost::saveFile(std::string payload) {
	_filename = setFilename(payload);
	trim_payload(payload);
	const char *tmp = _upload.c_str();
	DIR* dir = opendir(tmp);
	if (dir) {
		closedir(dir);
	}
	else if (ENOENT == errno) { // create folder

	}
	else
		throw HTTPException(500); // opendir failed
	// CPP01 ex04
}

void CGIResponsePost::run(Socket &socket) {

	HTTPHeader header;

	header.setStatusCode(201);
	header.setStatusMessage(get_message(201));
	saveFile(_request->_payload);
	std::stringstream code;
	code << header.getStatusCode();
	std::string body = code.str() + " " + header.getStatusMessage();
	header.set_content_length(body.length());
	socket.send(header.tostring() + "\r\n\r\n" + body);
}

CGIResponsePost::CGIResponsePost(HTTPRequest *request) : CGIResponse(request) {}
