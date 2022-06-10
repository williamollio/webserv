//
// Created by Manuel Hahn on 6/10/22.
//

#include "../include/CGIResponseGet.hpp"
#include "HTTPHeader.hpp"
#include <fstream>

std::string CGIResponseGet::read_file(std::string file)
{
	std::ifstream is;
	is.open(file);
	if (!is.is_open())
		throw std::exception();

	std::stringstream buffer;
	buffer << is.rdbuf();
	is.close();

	return (buffer.str());
}

void CGIResponseGet::run(Socket & socket) {
	std::string body;

	HTTPHeader header;
	std::string file = "index2.html";
	body = read_file(file);
	header.set_content_type ("text/html");
	header.set_content_length (body.size());
	socket.send(header.tostring());
}

CGIResponseGet::CGIResponseGet(HTTPRequest &request) : CGIResponse(request) {}
