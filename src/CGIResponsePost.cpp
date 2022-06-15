//
// Created by Manuel Hahn on 6/10/22.
//

#include "../include/CGIResponsePost.hpp"


void CGIResponsePost::run(Socket &socket) {

	HTTPHeader header;

	header.setStatusCode(201);
	header.setStatusMessage(get_message(201));
	std::stringstream code;
	code << header.getStatusCode();
	std::string body = code.str() + " " + header.getStatusMessage();
	header.set_content_length(body.length());
	std::cout << "header sent back :\n" << header.tostring() << std::endl;
	std::cout << "body sent back :\n" << body << std::endl;
	socket.send(header.tostring() + "\r\n\r\n" + body);
}

CGIResponsePost::CGIResponsePost(HTTPRequest &request) : CGIResponse(request) {}
