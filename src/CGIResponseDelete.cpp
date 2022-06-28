//
// Created by Manuel Hahn on 6/10/22.
//

#include "../include/CGIResponseDelete.hpp"

void CGIResponseDelete::send_response(Socket &socket)
{
	HTTPHeader header;
	std::string body("{\"success\":\"true\"}");

	header.setStatusCode(200);
	header.setStatusMessage(get_message(200));
	header.set_content_length(body.length());
	socket.send(header.tostring() + "\r\n\r\n" + body);
}

void CGIResponseDelete::extract_path() {
	size_t pos = _request->_path.rfind('/');
	if (pos == std::string::npos)
		throw HTTPException(500);
	_sub_path = _request->_path.substr(1, pos);
	_file = _request->_path.substr(pos+1, _request->_path.length());
}

void CGIResponseDelete::set_up_location() {
	std::string location_folder;
	std::string location_file;

	location_file = _server_location_log + _sub_path + _file;
	location_folder = _server_location_log + _sub_path ;

	_location_folder = location_file.c_str();
	_location_file = location_folder.c_str();
}

void CGIResponseDelete::run(Socket &socket) {

	extract_path();
	set_up_location();
	if (access(_location_folder, X_OK) < 0
	|| access(_location_folder, R_OK) < 0)
		throw HTTPException(401);
	if (access(_location_file, F_OK) < 0)
		throw HTTPException(404);
	if (remove(_location_file) == 0)
		send_response(socket);
	else
		throw HTTPException(403);
}

CGIResponseDelete::CGIResponseDelete(HTTPRequest *request) : CGIResponse(request) {}
