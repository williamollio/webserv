//
// Created by Manuel Hahn on 6/8/22.
//

#include "HTTPHeader.hpp"
#include "HTTPRequest.hpp"
#include "HTTPRequestGet.hpp"
#include "CGIResponse.hpp"
#include "CGIResponseGet.hpp"
#include "CGIResponsePost.hpp"
#include "CGIResponseDelete.hpp"

HTTPHeader::HTTPHeader() {}

std::string HTTPHeader::tostring() const
{
	std::stringstream str;
	str << "HTTP/1.1 " << _status_code << " " << _status_message << "\n";
	str << "Connection: " << _connection << "\n";
//	str << "Content-Encoding: " << _content_encoding << "\n";
	str << "Content-Type: " << _content_type << "\n";
	str << "Content-Length: " << _content_length << "\n";
	str << "Date: " << "Mon, 18 Jul 2016 16:06:00 GMT\n";
//	str << "Transfer-Encoding: " << _transfer_encoding << "\n";
	return (str.str());
}

const std::string &HTTPHeader::getStatusMessage() const {
    return _status_message;
}

int HTTPHeader::getStatusCode() const {
    return _status_code;
}

const std::string &HTTPHeader::getConnection() const {
    return _connection;
}

const std::string &HTTPHeader::getContentEncoding() const {
    return _content_encoding;
}

const std::string &HTTPHeader::getTransferEncoding() const {
    return _transfer_encoding;
}

void HTTPHeader::set_content_type(const std::string &str) {
    _content_type = str;
}

void HTTPHeader::set_content_length(int size) {
    _content_length = size;
}

const std::string &HTTPHeader::getContentType() const {
    return _content_type;
}

int HTTPHeader::getContentLength() const {
    return _content_length;
}

void HTTPHeader::setStatusMessage(const std::string &statusMessage) {
    _status_message = statusMessage;
}

void HTTPHeader::setStatusCode(int statusCode) {
    _status_code = statusCode;
}

void HTTPHeader::setConnection(const std::string &connection) {
    _connection = connection;
}

void HTTPHeader::setContentEncoding(const std::string &contentEncoding) {
    _content_encoding = contentEncoding;
}

void HTTPHeader::setTransferEncoding(const std::string &transferEncoding) {
    _transfer_encoding = transferEncoding;
}
