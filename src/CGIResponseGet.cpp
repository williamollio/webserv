//
// Created by Manuel Hahn on 6/10/22.
//

#include "../include/CGIResponseGet.hpp"
#include "HTTPHeader.hpp"
#include "CGICallBuiltin.hpp"
#include <fstream>
#include <algorithm>

std::string CGIResponseGet::set_extension(std::string &file)
{
	size_t pos = file.find('.', 0);
	if (pos == std::string::npos)
		return ("");
	else
		return (file.substr(pos + 1));
}

std::string CGIResponseGet::set_file(std::string path, Socket & socket)
{
	std::string tmp;

	#if DEBUG
		std::cout << "path: " << path
		<< "\n_directory_location: " << _directory_location
		<< "\ndir listing: " << _dir_listing
		<< std::endl;
 	#endif

	if ((path == "/" || _directory_location == path ) && _dir_listing == true)
	{
		CGICall *cgicall = new CGICallBuiltin(_request, "/cgi/directory_listing.php");
		cgicall->run(socket);
	}
	else if ((path == "/" || _directory_location == path ) && _dir_listing == false)
		tmp = _server_index;
	else
		tmp = path;

	#if DEBUG
		std::cout << "tmp: " << tmp << std::endl;
	#endif

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

	if (_GET == false)
		throw HTTPException(405);
	std::string file = set_file(_request->_path, socket);
	body = read_file(file);
	header.set_content_type(construct_content_type());
	header.set_content_length(body.size());
    header.setStatusCode(200);
    header.setStatusMessage(get_message(200));
	socket.send(header.tostring() + "\r\n\r\n" + body);
}

CGIResponseGet::CGIResponseGet(const HTTPRequest *request) : CGIResponse(request) {}
