//
// Created by Manuel Hahn on 6/10/22.
//

#include "HTTPRequest.hpp"
#include "HTTPException.hpp"

HTTPRequest::TYPE HTTPRequest::getType() const {
    return _type;
}

HTTPRequest::HTTPRequest(HTTPRequest::TYPE type): _type(type), _keep_alive(false), _content(false), _content_length(0) {}

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
