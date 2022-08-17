//
// Created by Manuel Hahn on 6/10/22.
//

#include "CGIResponseGet.hpp"
#include "HTTPHeader.hpp"
#include "CGICallBuiltin.hpp"
#include "Connection.hpp"
#include <fstream>
#include <algorithm>

bool CGIResponseGet::is_request_location(std::string path)
{
	trim_slash_end(path);
	return (_directory_location == path);
}

std::string CGIResponseGet::set_file(std::string path)
{
	std::string tmp;

	if ((path == "/" || is_request_location(path)) && _dir_listing == true)
	{
        cgicall = new CGICallBuiltin(_request, _socket, *this, "cgi/directory_listing.php");
        cgicall->run();
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

void CGIResponseGet::run() {
	HTTPHeader header;
	std::string body;
	std::string file;

	if (_GET == false)
		throw HTTPException(405);
    try {
        file = set_file(_request->getPath());
    } catch (HTTPException &) { return; }

    body = read_file(file);
	header.set_content_type(construct_content_type());
	header.set_content_length(body.size());
    header.setStatusCode(200);
    header.setStatusMessage(get_message(200));
	header.setCookie(_request->get_cookie());
    payload = header.tostring() + "\r\n\r\n" + body;
    if (!runForFD(0)) {
        Connection::getInstance().add_fd(_socket.get_fd(), this, false);
    }
	//socket.write(header.tostring() + "\r\n\r\n" + body);
}

CGIResponseGet::CGIResponseGet(HTTPRequest *request, Socket & socket, Runnable & parent): CGIResponse(request, socket, parent), socketCounter(0),
                                                                                          cgicall(NULL)
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

bool CGIResponseGet::runForFD(int) {
    try {
        ssize_t ret = _socket.write(payload.c_str() + socketCounter, payload.size() - socketCounter < 65536 ? payload.size() - socketCounter : 65536);
        socketCounter += ret;
        if (socketCounter < payload.size()) {
            return false;
        }
        debug("Write with socket fd " << _socket.get_fd() << " size " << socketCounter << " real " << payload.size());
        debug("Closing socket fd " << _socket.get_fd());
        _socket.close();
        return true;
    } catch (IOException &) {
        debug("Write with socket fd " << _socket.get_fd() << " size " << socketCounter);
        return false;
    }
}
