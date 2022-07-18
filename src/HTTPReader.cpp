//
// Created by Manuel Hahn on 6/8/22.
//

#include "HTTPReader.hpp"
#include "CGIResponseGet.hpp"
#include "CGIResponsePost.hpp"
#include "CGIResponseDelete.hpp"
#include "CGIResponseError.hpp"
#include "HTTPException.hpp"
#include "URI.hpp"
#include "CGICall.hpp"
#include "HTTPRequest.hpp"

#include <cstdlib>
#include <iostream>

std::list<Cookie> HTTPReader::session_management;

HTTPReader::HTTPReader(): _socket(), response(NULL), request(NULL) {}

HTTPReader::HTTPReader(Socket & socket): _socket(socket), response(NULL), request(NULL) {}

HTTPReader::~HTTPReader() {
    if (response != NULL) delete response;
    if (request != NULL) delete request;
    try {
        _socket.close_socket();
    } catch (std::exception & exception) {
        std::cerr << exception.what() << std::endl;
    }
}

Cookie HTTPReader::get_cookie(Cookie cookie) {
	std::list<Cookie>::iterator it;

	it = std::find(session_management.begin(), session_management.end(), cookie);
	if (it == session_management.end()) {
		cookie.set_identifier("sessionID", Cookie::generate_session_id());
		cookie.set_user_agent(request->_user_agent);
		session_management.push_back(cookie);
		return cookie;
	}
	return *it;
}

void HTTPReader::run() {
    try {
        request = _parse();
		Cookie cookie = get_cookie(request->get_cookie());
        request->setURI(URI(request->_path));
        request->setPeerAddress(peerAddress);
        request->setPeerName(peerName);
        request->setUsedPort(port);
        if (request->getURI().isCGIIdentifier() && _isCGIMethod(request->getType())) {
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
	catch (HTTPException & ex) {
        std::cerr << ":" << ex.what() << std::endl;
        CGIResponseError error;
		error.set_error_code(ex.get_error_code());
		error.run(_socket);
    }
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

static bool catch_special(std::string& line, size_t li) {
	switch (line[li]) {
		case '\n':
			return false;
		case ':':
			return false;
		case '#':
			return false;
		case ';':
			return false;
		case '{':
			return false;
		case '}':
			return false;
		case ',':
			return false;
		default:
			return true;
	}
}

static std::vector<std::string>	split_line(std::string& line) {
	size_t i = 0;
	size_t li = 0;
	size_t fl = line.find('\n', 0);
	int		br = 0;
	std::vector<std::string>	ret;
	while (li != line.length()) {
		i = li;
		if (i < fl) {
			while (i != line.length() && (line[i] == ' ' || line[i] == '\t')) { i++; }
			li = i;
			while (li != line.length() && line[li] != ' ' && line[li] != '\t' && catch_special(line, li)) {
				li++;
			}
			if (li != i)
				ret.push_back(line.substr(i, li - i));
			if (!catch_special(line, li)) {
				ret.push_back(line.substr(li, 1));
				li++;
			}
		} else  {
			while (line[i] == ' ') { i++; }
			while (li != line.length() && (br != 0 || catch_special(line, li))) {
				if (line[li] == '(')
					br++;
				if (line[li] == ')' && br > 0)
					br--;
				li++;
			}
			if (li != i)
				ret.push_back(line.substr(i, li - i));
			if (!catch_special(line, li)) {
				ret.push_back(line.substr(li, 1));
				li++;
			}
		}
	}
	return ret;
}

int	HTTPRequest::checktype(std::string& word) {
	if (word == "GET")
		return GET;
	if (word == "POST" || word == "PUT")
		return POST;
	if (word == "DELETE")
		return DELETE;
	return ERROR;
}


HTTPRequest* HTTPReader::_parse() throw(std::exception) {
//	char * buff = new char[BUFFER + 1]();
//	if (!read(_socket.get_fd(), buff, BUFFER)) {
//		throw HTTPException(504);
//	}
//    delete[] buff;
//	size_t	cursor = raw.find("\r\n\r\n", 0);
//	if (cursor == raw.npos)
//		cursor = raw.length();

	char buff[2] = {'\0', '\0'};
	std::string raw(buff);

	while (raw.find("\r\n\r\n", 0) == std::string::npos) {
		if (!read(_socket.get_fd(), buff, 1))
			throw HTTPException(504);
		raw += buff;
	}
	std::string	head = raw;
	std::vector<std::string> file = split_line(head);
	switch(HTTPRequest::checktype(file[0])) {
		case HTTPRequest::GET:
			return new HTTPRequest(HTTPRequest::GET, file, raw, _socket);
		case HTTPRequest::POST:
			return new HTTPRequest(HTTPRequest::POST, file, raw, _socket);
		case HTTPRequest::DELETE:
			return new HTTPRequest(HTTPRequest::DELETE, file, raw, _socket);
        default:
			throw HTTPException(405);
	}

}

bool HTTPReader::isRunning() const {
    return response == NULL || response->isRunning();
}

unsigned int HTTPReader::getPeerAddress() const {
    return peerAddress;
}

void HTTPReader::setPeerAddress(unsigned int peerAddress) {
    HTTPReader::peerAddress = peerAddress;
}

const std::string &HTTPReader::getPeerName() const {
    return peerName;
}

void HTTPReader::setPeerName(const std::string &peerName) {
    HTTPReader::peerName = peerName;
}

const Socket & HTTPReader::getSocket() const {
    return _socket;
}

void HTTPReader::setSocket(const Socket & socket) {
    _socket = socket;
}

int HTTPReader::getUsedPort() const {
    return port;
}

void HTTPReader::setUsedPort(const int port) {
    HTTPReader::port = port;
}

bool HTTPReader::_isCGIMethod(HTTPRequest::TYPE type) {
    const std::vector<std::string> & ref = Configuration::getInstance().get_cgi_methods();
    for (std::vector<std::string>::const_iterator it = ref.begin(); it != ref.end(); ++it) {
        if ((*it == "POST" || *it == "post") && type == HTTPRequest::POST) {
            return true;
        } else if ((*it == "GET" || *it == "get") && type == HTTPRequest::GET) {
            return true;
        } else if ((*it == "DELETE" || *it == "delete") && type == HTTPRequest::DELETE) {
            return true;
        }
    }
    return false;
}
