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

std::string CGIResponsePost::get_delimiter(std::string &tmp) {
	std::string del;
	size_t posbegin = 2, posend;

	posend = tmp.find("\n", posbegin);
	del = tmp.substr(posbegin + 1, posend - posbegin);
	del.erase(del.length() - 2);
	return (del);
}

void CGIResponsePost::trim_payload(std::string &payload) {

	std::string delimiter;
	std::string tmp(payload);
	payload.clear();
	size_t posbegin, posend;

	delimiter = get_delimiter(tmp);
	posbegin = tmp.find("\r\n\r\n");
	if (posbegin == std::string::npos)
		throw HTTPException(400);
	posbegin += 4;

	posend = tmp.find(delimiter, posbegin);
	if (posend == std::string::npos)
		throw HTTPException(400);
	posend -= 4;

	payload = tmp.substr(posbegin, posend - posbegin);
}

void CGIResponsePost::create_file(std::string &payload) {
	std::ofstream ofs(_filename);
	ofs << payload << std::endl;
	ofs.close();
}

void CGIResponsePost::saveFile(std::string payload) {
	_filename = setFilename(payload);
	trim_payload(payload);
	create_file(payload);
	const char *upload = _upload.c_str();
	DIR* dir = opendir(upload);
	if (dir) {
		std::cout << "fine" << std::endl;
		closedir(dir);
	}
	else if (ENOENT == errno) { // can't find folder
		throw HTTPException(501);
	}
	else { // permissions aren't correct
		throw HTTPException(500);
	}
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
