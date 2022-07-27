//
// Created by Manuel Hahn on 6/10/22.
//

#include "CGIResponseDelete.hpp"

void CGIResponseDelete::send_response(Socket &socket)
{
	HTTPHeader header;
	std::string body("{\"success\":\"true\"}");

	header.setStatusCode(200);
	header.setStatusMessage(get_message(200));
	header.set_content_length(body.length());
	socket.write(header.tostring() + "\r\n\r\n" + body);
}

void CGIResponseDelete::extract_path() {
	size_t pos = _request->getPath().rfind('/');
	if (pos == std::string::npos)
		throw HTTPException(500);
	_sub_path = _request->getPath().substr(1, pos);
	_file = _request->getPath().substr(pos+1, _request->getPath().length());
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

	if (_DELETE == false)
	{
		PRINT_ERROR_CODE("Error code : ", 405);
		throw HTTPException(405);
	}
	extract_path();
	set_up_location();
	if (access(_location_folder, X_OK) < 0
	|| access(_location_folder, R_OK) < 0)
	{
		PRINT_ERROR_CODE("Error code : ", 401);
		throw HTTPException(401);
	}
	if (access(_location_file, F_OK) < 0)
	{
		PRINT_ERROR_CODE("Error code : ", 404);
		throw HTTPException(404);
	}
	if (remove(_location_file) == 0)
		send_response(socket);
	else
	{
		PRINT_ERROR_CODE("Error code : ", 403);
		throw HTTPException(403);
	}
}

CGIResponseDelete::CGIResponseDelete(HTTPRequest *request):  CGIResponse(request)
{
	Configuration config = Configuration::getInstance();

	_error_pages  = config.get_server_error_page_location();
	_accept_file = config.get_server_file_acceptance();
	_server_root = config.get_server_root_folder();
	_server_index = config.get_server_index_file();

	if (is_request_defined_location(request->getPath(), config.get_location_specifier()))
		_server_location_log = set_absolut_path(_loc_root);
	else
		_server_location_log = set_absolut_path(_server_root);

}
