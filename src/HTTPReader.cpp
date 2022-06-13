//
// Created by Manuel Hahn on 6/8/22.
//

#include "HTTPReader.hpp"
#include "HTTPRequest.hpp"
#include "HTTPRequestGet.hpp"
#include "CGIResponse.hpp"
#include "CGIResponseGet.hpp"
#include "CGIResponsePost.hpp"
#include "CGIResponseDelete.hpp"
#include "HTTPException.hpp"
#include <cstdlib>
#include <iostream>

HTTPReader::HTTPReader(): _socket() {}

HTTPReader::HTTPReader(Socket &socket): _socket(socket) {}

HTTPReader::~HTTPReader() {
    try {
        _socket.close_socket();
    } catch (std::exception &exception) {
        std::cerr << exception.what() << std::endl;
    }
}

void HTTPReader::run() {
	HTTPRequest*	request;
    try {
        request = _parse();
        CGIResponse * response;
        switch (request->getType()) {
            case HTTPRequest::GET:    response = new CGIResponseGet(*request);    break;
            case HTTPRequest::POST:   response = new CGIResponsePost(*request);   break;
            case HTTPRequest::DELETE: response = new CGIResponseDelete(*request); break;
        }
        response->run(_socket);
        delete request;
    } catch (std::exception& ex) {
        // TODO: Error
//        sendError(ex.getErrorCode());
    }
    delete request;
}

std::vector<std::string>	split_str_vector(const std::string& tosplit, const std::string& needle) {
	size_t						cursor	= 0;
	std::vector<std::string>	str;
	size_t						pos		= tosplit.find(needle.c_str(), cursor, needle.length());
	do {
		str.push_back(tosplit.substr(cursor, pos - cursor));
		cursor = pos + 2;
		pos = tosplit.find(needle.c_str(), cursor, 2);
	}
	while (pos != std::string::npos);
	return str;
}

void HTTPRequest::get_payload(const std::string& data) throw(std::exception) {
	size_t	cursor = data.find("\n\n", 0);
	if (cursor == std::string::npos)
		throw HTTPException(400);
	cursor += 2;
	_payload = data.substr(cursor);
}

HTTPRequest* HTTPReader::_parse() throw(std::exception) {
	char buff[30001];
	if (!read(_socket.get_fd(), buff, 30000)) {
		throw HTTPException(504);
	}
	std::string raw(buff);
	size_t old_nl;
	old_nl = raw.find('\n', 0);
	if (raw.npos == old_nl) {
		throw HTTPException(400);
	}
	HTTPRequest*	retval;
	if (!raw.compare(0, 3, "GET"))
		retval = new HTTPRequestGet();
	else if (!raw.compare(0, 4, "POST"))
		retval = new HTTPRequestPost();
	else if (!raw.compare(0, 6, "DELETE"))
		retval = new HTTPRequestDelete();
	else
		throw HTTPException(400);
	if (raw.find("HTTP/1.1", 0, 8) == raw.npos)
		throw HTTPException(400);
	retval->_http_version = "1.1";
	{///path
		size_t	first;
		size_t	last;
		if (!raw.compare(0, 3, "GET"))
			first = 4;
		else if (!raw.compare(0, 4, "POST"))
			first = 5;
		else if (!raw.compare(0, 6, "DELETE"))
			first = 6;
		else
			throw HTTPException(400);
		last = raw.find("HTTP/1.1", 0, 8) - 1;
		retval->_path = raw.substr(first, last);
	}

	size_t	new_nl = raw.find('\n', old_nl + 1);
	while (new_nl != raw.npos) {
		if (raw.find("User-Agent:", old_nl, 11) != raw.npos)
			retval->_user_agent = raw.substr(old_nl + 12, new_nl - (old_nl + 12));
		else if (raw.find("Host:", old_nl, 5) != raw.npos)
			retval->_host = raw.substr(old_nl + 6, new_nl - (old_nl + 6));
		else if (raw.find("Accept-Language:", old_nl, 16) != raw.npos)
			retval->_lang = split_str_vector(raw.substr(old_nl + 17, new_nl - (old_nl + 17)), ", ");
		else if (raw.find("Accept-Encoding:", old_nl, 16) != raw.npos)
			retval->_encoding = split_str_vector(raw.substr(old_nl + 17, new_nl - (old_nl + 17)), ", ");
		else if (raw.find("Content-Type:", old_nl, 13) != raw.npos)
			retval->_content_type = raw.substr(old_nl + 14, new_nl - (old_nl + 14));
		else if (raw.find("Content-Length:", old_nl, 15) != raw.npos)
			retval->_content_length = strtol(raw.substr(old_nl + 16, new_nl - (old_nl + 16)).c_str(), NULL, 0);
		else if (raw.find("Connection:", old_nl, 11) != raw.npos && raw.find("Keep-Alive", old_nl, 10) != raw.npos)
			retval->_keep_alive = true;
		old_nl = new_nl;
		new_nl = raw.find('\n', old_nl + 1);
	}
	if (retval->_content_length != 0 && retval->_content_type.length() == 0)
		throw HTTPException(400);
	else if (retval->_content_length != 0) {
		retval->_content = true;
		retval->get_payload(raw);
	}
	else
		retval->_content = false;
	return retval;
}
