
#include "../include/CGIResponseError.hpp"

void CGIResponseError::set_error_code(int error_code)
{
	_error_code = error_code;
}

void CGIResponseError::setBody(int error_code, std::string &body)
{
	std::string _default_error_file("../error404.html");

	std::map<int, std::string>::iterator it = _error_pages.find(error_code);
	if (it != _error_pages.end())
		body = read_file(it->second);
	else
		body = read_file(_default_error_file);
	return;
}

void CGIResponseError::run(Socket & socket)
{
	HTTPHeader header;
	std::string body;

	header.setStatusCode(_error_code);
	header.setStatusMessage(get_message(_error_code));

	setBody(_error_code, body);
	header.set_content_length(body.length());

	socket.send(header.tostring() + "\r\n\r\n" + body);
}

CGIResponseError::CGIResponseError() : CGIResponse(NULL)
{
	Configuration config = Configuration::getInstance();

	_error_pages  = config.get_server_error_page_location();
	_accept_file = config.get_server_file_acceptance();
	_server_root = config.get_server_root_folder();
	_server_index = config.get_server_index_file();
	_server_location_log = set_absolut_path(_server_root);
}
