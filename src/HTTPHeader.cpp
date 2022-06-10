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
	str << _status_code << " " << _status_message << "\n";
	str << "Connection: " << _connection << "\n";
	str << "Content-Encoding: " << _content_encoding << "\n";
	str << "Content-Type: " << _content_type << "\n";
	str << "Content-Length: " << _content_length << "\n";
	str << "Date: " << "Mon, 18 Jul 2016 16:06:00 GMT\n";
	str << "Transfer-Encoding: " << _transfer_encoding << "\n";
	return (str.str());
}
