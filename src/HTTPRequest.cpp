//
// Created by Manuel Hahn on 6/10/22.
//

#include "HTTPRequest.hpp"
#include "HTTPException.hpp"
#include "CGIResponseError.hpp"
#include <cstdlib>


HTTPRequest::TYPE HTTPRequest::getType() const {
    return _type;
}

HTTPRequest::HTTPRequest(HTTPRequest::TYPE type): _type(type), _keep_alive(false), _content(false), _chunked(false), _content_length(0){}

bool	HTTPRequest::is_payload(size_t index) {
	return 	_copy_raw.find("\r\n\r\n", 0) <= index;
}

HTTPRequest::REQ_INFO HTTPRequest::http_token_comp(std::string &word) {
	if (word == "user-agent" || word == "User-Agent")
		return USER_AGENT;
	if (word == "host" || word == "Host")
		return HOSTNAME;
	if (word == "Accept-Language" || word == "accept-language")
		return LANG_SUPP;
	if (word == "Accept-Encoding" || word == "accept-encoding")
		return ENCODING;
	if (word == "Accept" || word == "accept")
		return CONTENT_TYPE;
	if (word == "Content-Length" || word == "content-length")
		return CON_LENGTH;
	if (word == "Connection" || word == "connection")
		return CON_TYPE;
	if (word == "Expect" || word == "expect")
		return EXPECT;
	return DEFAULT;
}

size_t	HTTPRequest::load_string(std::vector<std::string>& file, size_t index, std::string& target) {
	index += 2;
	while (index < file.size() && file.at(index) != "\n" && file.at(index) != ";")
		target += file.at(index++);
	while (index < file.size() && file.at(index) != "\n")
		index++;
	return index + 1;
}

size_t HTTPRequest::load_vec_str(std::vector<std::string> &file, size_t index, vectorString &target) {
	index += 2;
	while (index < file.size() && file.at(index) != "\n" && file.at(index) != ";") {
		if (file.at(index) == ",")
			index++;
		else
			target.push_back(file.at(index++));
	}
	while (index < file.size() && file.at(index) != "\n")
		index++;
	return index + 1;
}

size_t	HTTPRequest::load_connection(std::vector<std::string> &file, size_t index, bool &target) {
	index += 2;
	if (!file.at(index).compare(0, 10, "keep-alive") || !file.at(index).compare(0, 10, "Keep-Alive")) {
		target = true;
	}
	else
		target = false;
	return index + 2;
}

size_t	HTTPRequest::load_size(std::vector<std::string> &file, size_t index, size_t &target) {
	index += 2;
	target = strtol(file.at(index).c_str(), NULL, 0);
	return index + 2;
}

size_t	HTTPRequest::ff_newline(std::vector<std::string>& file, size_t index) {
	while (index < file.size() && file.at(index) != "\n" && !is_payload(index))
		index++;
	return index + 1;
}

HTTPRequest::HTTPRequest(HTTPRequest::TYPE type, std::vector<std::string> &file, std::string& raw, Socket& _socket) : _type(type), _keep_alive(false), _content(false), _content_length(0) {
	size_t	index = 1;
	_copy_raw = raw;
	_path = file[index++];
	if (file[index].compare(0, 5, "HTTP/") == 0 && file[index + 1] == "\n") {
		_http_version = file[index].substr(5);
		index += 2;
	}
	else
		throw HTTPException(400);
	while (file.size() > index + 2 && !is_payload(index)) {
		switch(http_token_comp(file[index])) {
			case USER_AGENT:
//				std::cerr << "passed USERAGENT" << std::endl;
				index = load_string(file, index, _user_agent);
//				std::cerr << "passed USERAGENT" << std::endl;
				break;
			case HOSTNAME:
//				std::cerr << "passed HOSTNAME" << std::endl;
				index = load_string(file, index, _host);
//				std::cerr << "passed HOSTNAME" << std::endl;
				break;
			case LANG_SUPP:
//				std::cerr << "passed LANGUAGE" << std::endl;
				index = load_vec_str(file, index, _lang);
//				std::cerr << "passed LANGUAGE" << std::endl;
				break;
			case ENCODING:
//				std::cerr << "passed ENCODING" << std::endl;
				index = load_vec_str(file, index, _encoding);
//				std::cerr << "passed ENCODING" << std::endl;
				break;
			case CON_TYPE:
//				std::cerr << "passed CONNECTION_TYPE" << std::endl;
				index = load_connection(file, index, _keep_alive);
//				std::cerr << "passed CONNECTION_TYPE" << std::endl;
				break;
			case CON_LENGTH:
//				std::cerr << "passed CONTENT_LENGTH" << std::endl;
				index = load_size(file, index, _content_length);
//				std::cerr << "passed CONTENT_LENGTH" << std::endl;
				break;
			case CONTENT_TYPE:
//				std::cerr << "passed CONTENT_TYPE" << std::endl;
				index = load_vec_str(file, index, _content_type);
//				std::cerr << "passed CONTENT_TYPE" << std::endl;
				break;
			case EXPECT :
				index = load_string(file, index, _expect);
				break;
			default:
				index = ff_newline(file, index);
		}
	}

	isChunkedRequest(raw);
	if (_content_length != 0 && _content_type.empty()) {

        throw HTTPException(400);
    }
	else if (_content_length != 0 || _chunked) {
		_content = true;
		set_payload(raw, _socket);
	}
	else
		_content = false;
}

const URI &HTTPRequest::getURI() const {
    return uri;
}

void HTTPRequest::setURI(const URI &uri) {
    HTTPRequest::uri = uri;
}

std::string HTTPRequest::unchunkedPayload(const std::string &data, size_t cursor)
{
	std::string payload;
	std::string line;
	std::string buffer;

	payload = data.substr(cursor);
	std::istringstream tmp(payload);
	getline(tmp, line);
	for (int i = 1; line.front() != '0'; i++)
	{
		if (i % 2)
		{
			line.pop_back();
			buffer.append(line);
			line.clear();
		}
		getline(tmp, line);
	}
	payload.clear();
	payload = buffer;
	return (payload);
}

bool HTTPRequest::isChunkedRequest(const std::string &data)
{
	if (data.find("Transfer-Encoding: chunked") != std::string::npos || data.find("Expect: 100-continue") != std::string::npos)
		_chunked = true;
	return (_chunked);
}

void HTTPRequest::set_payload(const std::string& data, Socket& _socket) throw(std::exception) {
	size_t	cursor = data.find("\r\n\r\n", 0);
	if (cursor == std::string::npos) {
        std::cerr << "error 4000 here" <<std::endl;
		throw HTTPException(400);
    }
	cursor += 2;

	if (_chunked) {
//        char c;
//        std::cerr << "Starting read" << std::endl;
//        while (read(_socket.get_fd(), &c, 1) > 0) {
//            std::cerr << c;
//        }
//        std::cerr << std::endl;
//        std::cerr << "Original: \n" << data.substr(cursor);
    	_payload = unchunkedPayload(data, cursor);
//        std::cerr << std::endl << std::endl;
//        std::cerr << "After: \n" << _payload << std::endl;
        return;
    }
	else
		_payload = data.substr(cursor);
	char buf[2];
	while (_payload.length() < _content_length) {
		if (!read(_socket.get_fd(), buf, 1))
			throw HTTPException(504);
		_payload += buf;
	}
}

const std::string & HTTPRequest::get_payload() const {
    return _payload;
}

unsigned int HTTPRequest::getPeerAddress() const {
    return peerAddress;
}

void HTTPRequest::setPeerAddress(unsigned int peerAddress) {
    HTTPRequest::peerAddress = peerAddress;
}

const std::string &HTTPRequest::getPeerName() const {
    return peerName;
}

void HTTPRequest::setPeerName(const std::string &peerName) {
    HTTPRequest::peerName = peerName;
}

int HTTPRequest::getUsedPort() const {
    return port;
}

void HTTPRequest::setUsedPort(const int port) {
    HTTPRequest::port = port;
}
