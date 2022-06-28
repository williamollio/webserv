//
// Created by Manuel Hahn on 6/10/22.
//

#include "HTTPRequest.hpp"
#include "HTTPException.hpp"

HTTPRequest::TYPE HTTPRequest::getType() const {
    return _type;
}

HTTPRequest::HTTPRequest(HTTPRequest::TYPE type): _type(type), _keep_alive(false), _content(false), _content_length(0) {}

bool	is_payload(std::vector<std::string>& file, size_t index) {return false;}

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
}

HTTPRequest::HTTPRequest(HTTPRequest::TYPE type, std::vector<std::string> &file) : _type(type), _keep_alive(false), _content(false), _content_length(0) {
	size_t	index = 1;
	_path = file[index++];
	if (file[index].compare(0, 5, "HTTP/") != file[index].npos && file[index + 1] == "\n") {
		_http_version = file[index].substr(5);
		index += 2;
	}
	else
		throw HTTPException(400);
	while (file.size() > index && !is_payload(file, index)) {
		switch(http_token_comp(file[index])) {
			case USER_AGENT:
			case HOSTNAME:
			case LANG_SUPP:
			case ENCODING:
			case CON_TYPE:
			case CON_LENGTH:
			case CONTENT_TYPE:
			default:
				throw HTTPException(400);
		}
	}
}

const URI &HTTPRequest::getURI() const {
    return uri;
}

void HTTPRequest::setURI(const URI &uri) {
    HTTPRequest::uri = uri;
}

void HTTPRequest::set_payload(const std::string& data) throw(std::exception) {
	size_t	cursor = data.find("\r\n\r\n", 0);
	if (cursor == std::string::npos)
		throw HTTPException(400);
	cursor += 2;
	_payload = data.substr(cursor);
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
