//
// Created by Manuel Hahn on 6/10/22.
//

#include "../include/CGIResponsePost.hpp"
#include "HTTPHeader.hpp"

void CGIResponsePost::run(Socket &socket) {

	//	if (CGI_specified)
	// {
	//		CGI_send_body
	//		return;
	// }
	HTTPHeader header;
	header.setStatusCode(201);
	header.setStatusMessage("Created");
	std::stringstream code;
	code << header.getStatusCode();
	std::string body = code.str() + " " + header.getStatusMessage();
	header.set_content_length(body.length());
	std::cout << header.tostring() << std::endl;
	std::cout << body << std::endl;
	socket.send(header.tostring() + "\n\n" + body);
}

CGIResponsePost::CGIResponsePost(HTTPRequest &request) : CGIResponse(request) {}
