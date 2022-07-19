//
// Created by Manuel Hahn on 6/10/22.
//

#include "CGIResponseGet.hpp"
#include "HTTPHeader.hpp"
#include "CGICallBuiltin.hpp"
#include <fstream>
#include <algorithm>

bool CGIResponseGet::is_request_location(std::string path)
{
	trim_slash_end(path);
	return (_directory_location == path);
}

std::string CGIResponseGet::set_file(std::string path, Socket& socket)
{
	std::string tmp;

	if ((path == "/" || is_request_location(path)) && _dir_listing == true)
	{
        CGICallBuiltin cgicall(_request, socket, "cgi/directory_listing.php");
        cgicall.run(socket);
        throw HTTPException(100);
	}
	else if ((path == "/" || is_request_location(path)) && _dir_listing == false)
		tmp = _server_index;
	else
		tmp = path;
    _file_extension = set_extension(tmp);
	return (tmp);
}

std::string CGIResponseGet::construct_content_type()
{
	std::string tmp;

	tmp = "text/";
	if (_file_extension == "html")
		tmp += _file_extension;
	else
		tmp += "plain";
	return (tmp);
}

void CGIResponseGet::run(Socket & socket) {
	HTTPHeader header;
	std::string body;
	std::string file;

	if (_GET == false)
		throw HTTPException(405);
    try {
        file = set_file(_request->getPath(), socket);
    } catch (HTTPException &) { return; }

    body = read_file(file);
	header.set_content_type(construct_content_type());
	header.set_content_length(body.size());
    header.setStatusCode(200);
    header.setStatusMessage(get_message(200));
	header.setCookie(_request->get_cookie());
	socket.write(header.tostring() + "\r\n\r\n" + body);
}

CGIResponseGet::CGIResponseGet(HTTPRequest *request): CGIResponse(request)
{

    Configuration config = Configuration::getInstance();

	_error_pages  = config.get_server_error_page_location();
	_accept_file = config.get_server_file_acceptance();
	_server_root = config.get_server_root_folder();
	_server_index = config.get_server_index_file();
	_server_location_log = set_absolut_path(_server_root);

	if (is_request_defined_location(request->getPath(), config.get_location_specifier())) {
        _server_location_log = set_absolut_path(_loc_root);
    }


}
