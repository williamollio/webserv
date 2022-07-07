
#include "../include/CGIResponseError.hpp"

void CGIResponseError::set_error_code(int error_code)
{
	_error_code = error_code;
}

void CGIResponseError::setBody(int error_code, std::string &body)
{
	std::map<int, std::string>::iterator it = _error_pages.find(error_code);
	if (it != _error_pages.end())
		body = read_file(it->second);
	else
		body = read_file("error.html");
	return;
}

void CGIResponseError::run(Socket & socket)
{
	HTTPHeader header;
	std::string body;

	PRINT_ERROR_CODE("Error code : ", _error_code);
	header.setStatusCode(_error_code);
	header.setStatusMessage(get_message(_error_code));

	setBody(_error_code, body);
	header.set_content_length(body.length());

	socket.send(header.tostring() + "\r\n\r\n" + body);
}

CGIResponseError::CGIResponseError() : CGIResponse(NULL){}
