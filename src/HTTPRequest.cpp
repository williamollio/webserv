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
	return (data.find("Transfer-Encoding: chunked") != std::string::npos);
}

void HTTPRequest::set_payload(const std::string& data) throw(std::exception) {
	size_t	cursor = data.find("\r\n\r\n", 0);
	if (cursor == std::string::npos)
		throw HTTPException(400);
	cursor += 2;
	if (isChunkedRequest(data))
		_payload = unchunkedPayload(data, cursor);
	else
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
