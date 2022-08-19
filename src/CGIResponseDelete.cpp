//
// Created by Manuel Hahn on 6/10/22.
//

#include "CGIResponseDelete.hpp"
#include "Connection.hpp"

void CGIResponseDelete::send_response()
{
	HTTPHeader header;
	std::string body("{\"success\":\"true\"}");

	header.setStatusCode(200);
	header.setStatusMessage(get_message(200));
	header.set_content_length(body.length());
    _payload = header.tostring() + "\r\n\r\n" + body;
    Connection::getInstance().add_fd(_socket.get_fd(), this, false);
}

void CGIResponseDelete::extract_path() {
	size_t pos = _request->getPath().rfind('/');
	if (pos == std::string::npos)
		throw HTTPException(500);
	_sub_path = _request->getPath().substr(1, pos);
	_file = _request->getPath().substr(pos+1, _request->getPath().length());
}

void CGIResponseDelete::set_up_location() {
	if (_file.front() != '/' && _sub_path.back() != '/')
		_location_file = _server_location_log + _sub_path + "/" + _file;
	else
		_location_file = _server_location_log + _sub_path + _file;
	_location_folder = _server_location_log + _sub_path ;
}

void CGIResponseDelete::run() {

	if (_DELETE == false)
		throw HTTPException(405);
	extract_path();
	set_up_location();
	if (access(_location_file.c_str(), F_OK) < 0)
		throw HTTPException(404);
	if (access(_location_file.c_str(), W_OK) < 0)
		throw HTTPException(401);
	if (remove(_location_file.c_str()) == 0)
		send_response();
	else
		throw HTTPException(403);
}

bool CGIResponseDelete::runForFD(int, bool hup) {
    if (hup) {
        _socket.close();
        return true;
    }
    try {
        ssize_t ret = _socket.write(_payload.c_str() + _payloadCounter, _payload.size() - _payloadCounter < 65536 ? _payload.size() - _payloadCounter : 65536);
        _payloadCounter += ret;
        if (_payloadCounter < _payload.size()) {
            return false;
        }
        debug("Write with socket fd " << _socket.get_fd() << " size " << _payloadCounter << " real " << _payload.size());
        debug("Closing socket fd " << _socket.get_fd());
        _socket.close();
        return true;
    } catch (IOException &) {
        debug("Write with socket fd " << _socket.get_fd() << " size " << _payloadCounter);
        return false;
    }
}

CGIResponseDelete::CGIResponseDelete(HTTPRequest *request, Socket & socket, Runnable & parent):  CGIResponse(request, socket, parent), _payloadCounter(0)
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
