//
// Created by Manuel Hahn on 6/10/22.
//

#include "../include/CGIResponsePost.hpp"

std::string CGIResponsePost::setFilename(std::string &payload) {
	std::string filename;

	size_t posbegin, posend;
	posbegin = payload.find("filename=\"");
	if (posbegin == std::string::npos)
		throw HTTPException(400);
	posbegin += 10;
	posend = payload.find("\"", posbegin + 1);
	filename = payload.substr(posbegin, posend - posbegin);
	trimPayload(payload);
	return (filename);
}

std::string CGIResponsePost::setFilenameChunked(std::string extension)
{
	std::string filenameChunked;

	filenameChunked = _request->_user_agent;
	filenameChunked += extension;

	return (filenameChunked);
}
std::string CGIResponsePost::getDelimiter(std::string &tmp) {
	std::string del;
	size_t posbegin = 2, posend;

	posend = tmp.find("\n", posbegin);
	del = tmp.substr(posbegin + 1, posend - posbegin);
	del.erase(del.length() - 2);
	return (del);
}

void CGIResponsePost::trimPayload(std::string &payload) {

	std::string delimiter;
	std::string tmp(payload);
	payload.clear();
	size_t posbegin, posend;

	delimiter = getDelimiter(tmp);
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

void CGIResponsePost::createFile(std::string &payload) {
	std::ofstream ofs(_filename);
	ofs << payload << std::endl;
	ofs.close();
}

void CGIResponsePost::saveFile(std::string payload) {
	const char *upload;
	DIR* dir;
	std::string path_string(get_current_path());
	const char *path = path_string.c_str();

	if (_request->_chunked)
		_filename = setFilenameChunked(".txt");
	else
		_filename = setFilename(payload);
	std::cout << _filename << std::endl;
	upload = _upload.c_str();
	dir = opendir(upload);
	if (dir) {
		if (chdir(upload) != 0)
			throw HTTPException(404);

		createFile(payload);
		if (chdir(path) != 0)
			throw HTTPException(404);
		closedir(dir);
	}
	else {
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
