//
// Created by Manuel Hahn on 6/8/22.
//

#include "HTTPHeader.hpp"
#include "HTTPRequest.hpp"
#include "Tool.hpp"

HTTPHeader::HTTPHeader()
{
	_status_message = "";
	_status_code = 0;
	_connection = "";
	_content_encoding = "";
	_content_type = "";
	_content_length = -1;
	_transfer_encoding = "";
}

void HTTPHeader::get_set_cookies(std::stringstream &str) const{

	for (std::vector<std::string>::const_iterator it = _set_cookies.begin(); it != _set_cookies.end(); ++it)
		str << "Set-Cookie: " << *it << "\n";
}

std::string HTTPHeader::tostring() const
{
	std::stringstream str;
	str << "HTTP/1.1 " << _status_code << " " << _status_message << "\n";
	if (!_connection.empty())
		str << "Connection: " << _connection << "\n";
	if (!_content_encoding.empty())
		str << "Content-Encoding: " << _content_encoding << "\n";
	if (!_content_type.empty())
		str << "Content-Type: " << _content_type << "\n";
	if (_content_length != -1)
		str << "Content-Length: " << _content_length << "\n";
	if (!_transfer_encoding.empty())
		str << "Transfer-Encoding: " << _transfer_encoding << "\n";
	if (!_set_cookies.empty())
		get_set_cookies(str);
    std::string tmp = str.str();
    tmp.erase(tmp.end() - 1);
	return (tmp);
}

const std::string & HTTPHeader::getStatusMessage() const {
    return _status_message;
}

int HTTPHeader::getStatusCode() const {
    return _status_code;
}

const std::string & HTTPHeader::getConnection() const {
    return _connection;
}

const std::string & HTTPHeader::getContentEncoding() const {
    return _content_encoding;
}

const std::string & HTTPHeader::getTransferEncoding() const {
    return _transfer_encoding;
}

void HTTPHeader::set_content_type(const std::string & str) {
    _content_type = str;
}

void HTTPHeader::set_content_length(int size) {
    _content_length = size;
}

const std::string & HTTPHeader::getContentType() const {
    return _content_type;
}

int HTTPHeader::getContentLength() const {
    return _content_length;
}

void HTTPHeader::setStatusMessage(const std::string & statusMessage) {
    _status_message = statusMessage;
}

void HTTPHeader::setStatusCode(int statusCode) {
    _status_code = statusCode;
}

void HTTPHeader::setConnection(const std::string & connection) {
    _connection = connection;
}

void HTTPHeader::setContentEncoding(const std::string & contentEncoding) {
    _content_encoding = contentEncoding;
}

void HTTPHeader::setTransferEncoding(const std::string & transferEncoding) {
    _transfer_encoding = transferEncoding;
}

void HTTPHeader::setCookie(Cookie &cookie) {
    const std::map<std::string, std::string> &identifier = cookie.get_identifier();
	std::map<std::string, std::string>::const_iterator it;
	_set_cookies.reserve(identifier.size());
	for (it = identifier.begin(); it != identifier.end(); ++it) {
		_set_cookies.push_back(it->first + "=" + it->second);
	}

}
