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
#include "CGIResponseError.hpp"
#include "HTTPException.hpp"
#include "URI.hpp"
#include "CGICall.hpp"
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
	HTTPRequest * request  = NULL;
    CGIResponse * response = NULL;
    try {
        request = _parse();
        request->setURI(URI(request->_path));
        if (request->getURI().isCGIIdentifier()) {
            response = new CGICall(request);
        } else {
            switch (request->getType()) {
                case HTTPRequest::GET:    response = new CGIResponseGet(request);    break;
                case HTTPRequest::POST:   response = new CGIResponsePost(request);   break;
                case HTTPRequest::DELETE: response = new CGIResponseDelete(request); break;
                default:
                    throw HTTPException(400);
            }
        }
        response->run(_socket);
    }
	catch (HTTPException& ex) {
		CGIResponseError error;
		error.set_error_code(ex.get_error_code());
		error.run(_socket);
    }
	if (request != NULL) delete request;
    if (response != NULL) delete response;
}

std::vector<std::string>	split_str_vector(const std::string& tosplit, const std::string& needle) {
	size_t						cursor	= 0;
	std::vector<std::string>	str;
	size_t						pos		= tosplit.find(needle.c_str(), cursor, needle.length());
	while (pos != std::string::npos) {
		str.push_back(tosplit.substr(cursor, pos - cursor));
		cursor = pos + needle.length();
		pos = tosplit.find(needle.c_str(), cursor, needle.length());
	}
	str.push_back(tosplit.substr(cursor));
	return str;
}

void HTTPRequest::get_payload(const std::string& data) throw(std::exception) {
	size_t	cursor = data.find("\r\n\r\n", 0);
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
	old_nl = raw.find('\n');
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
	retval->_copy_raw = raw;
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
		retval->_path = raw.substr(first, last - first);
	}

	size_t	new_nl = raw.find('\n', old_nl + 2);
	while (new_nl != raw.npos) {
		if (raw.find("User-Agent:", old_nl, 11) < new_nl)
			retval->_user_agent = raw.substr(old_nl + 13, new_nl - (old_nl + 14));
		else if (raw.find("Host:", old_nl, 5) < new_nl)
			retval->_host = raw.substr(old_nl + 7, new_nl - (old_nl + 8));
		else if (raw.find("Accept-Language:", old_nl, 16) < new_nl)
			retval->_lang = split_str_vector(raw.substr(old_nl + 18, new_nl - (old_nl + 19)), ", ");
		else if (raw.find("Accept-Encoding:", old_nl, 16) < new_nl)
			retval->_encoding = split_str_vector(raw.substr(old_nl + 18, new_nl - (old_nl + 19)), ", ");
		else if (raw.find("Accept:", old_nl, 7)  < new_nl)
			retval->_content_type =  split_str_vector(raw.substr(old_nl + 8, new_nl - (old_nl + 9)), ",");
		else if (raw.find("Content-Length:", old_nl, 15) < new_nl)
			retval->_content_length = strtol(raw.substr(old_nl + 17, new_nl - (old_nl + 18)).c_str(), NULL, 0);
		else if (raw.find("Connection:", old_nl, 11) < new_nl && raw.find("keep-alive", old_nl, 10) != raw.npos)
			retval->_keep_alive = true;
		old_nl = new_nl;
		new_nl = raw.find('\n', old_nl + 1);
	}
	if (retval->_content_length != 0 && retval->_content_type.empty())
		throw HTTPException(400);
	else if (retval->_content_length != 0) {
		retval->_content = true;
		retval->get_payload(raw);
	}
	else
		retval->_content = false;
	return retval;
}
