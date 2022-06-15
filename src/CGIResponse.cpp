//
// Created by Manuel Hahn on 6/10/22.
//

#include "../include/CGIResponse.hpp"

std::string CGIResponse::set_server_location(std::string path_from_configuration) throw (IOException)
{
	char buf[256];

	static std::string tmp = "";

	if (tmp == "")
	{
		if (getcwd(buf, sizeof(buf)) == NULL)
			throw IOException("error : getcwd failed");
		std::string path(buf);
		path += path_from_configuration;
		const char * path_tmp = path.c_str();
		tmp = std::string(path_tmp);
		if (chdir(path_tmp) != 0)
		throw IOException("error : chdir failed");
	}
	return (tmp);
}

std::string CGIResponse::read_file(std::string file) throw (IOException)
{
	std::ifstream is;
	is.open(file);
	if (!is.is_open())
		throw IOException("error : open failed");

	std::stringstream buffer;
	buffer << is.rdbuf();
	is.close();
	return (buffer.str());
}

std::string CGIResponse::set_default_file(std::string file) throw (IOException)
{
	std::string tmp;

	tmp += _server_location_log;
	if (_server_location_log.back() != '/')
		tmp += "/";
	tmp += file;
	return (read_file(tmp));
}
void CGIResponse::send_error_code(Socket & socket, const int &error_code)
{
	HTTPHeader header;

	header.setStatusCode(error_code);
	header.setStatusMessage(get_message(error_code));
	std::stringstream code;
	code << header.getStatusCode();
	std::string body = code.str() + " " + header.getStatusMessage();
	header.set_content_length(body.length());
	std::cout << "header sent back :\n" << header.tostring() << std::endl;
	std::cout << "body sent back :\n" << body << std::endl;
	socket.send(header.tostring() + "\r\n\r\n" + body);
}

void CGIResponse::send_error_message(Socket & socket, const std::string & message)
{
	HTTPHeader header;
	int error_code;

	error_code = get_code(message);
	header.setStatusCode(error_code);
	header.setStatusMessage(get_message(error_code));
	std::stringstream code;
	code << header.getStatusCode();
	std::string body = read_file("error404.html");
	header.set_content_length(body.length());
	std::cout << "header sent back :\n$" << header.tostring() << "$" << std::endl;
	std::cout << "body sent back :\n$" << body << "$" << std::endl;
	socket.send(header.tostring() + "\r\n\r\n" + body);
}

CGIResponse::CGIResponse(HTTPRequest &request): _request(request)
{
	/* CONFIGURATION */
	_server_location_log = set_server_location("/server");
	_default_file = set_default_file("../index.html");
}

CGIResponse::~CGIResponse() {}
