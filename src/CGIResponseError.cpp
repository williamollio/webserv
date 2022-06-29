
#include "../include/CGIResponseError.hpp"

void CGIResponseError::set_error_code(int error_code)
{
	_error_code = error_code;
}

void CGIResponseError::run(Socket & socket)
{
	HTTPHeader header;

	PRINT_ERROR_CODE("Error code : ", _error_code);
	header.setStatusCode(_error_code);
	header.setStatusMessage(get_message(_error_code));
	std::stringstream code;
	code << header.getStatusCode();
	std::string body = read_file("error.html");
	header.set_content_length(body.length());
	socket.send(header.tostring() + "\r\n\r\n" + body);
}

CGIResponseError::CGIResponseError() : CGIResponse(NULL){}
