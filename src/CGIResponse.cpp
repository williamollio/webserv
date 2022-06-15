//
// Created by Manuel Hahn on 6/10/22.
//

#include "../include/CGIResponse.hpp"

std::string CGIResponse::set_server_location(std::string path_from_configuration) throw (IOException)
{
	char buf[256];
	if (getcwd(buf, sizeof(buf)) == NULL)
		throw IOException("error : getcwd failed");
	std::string path(buf);
	path += path_from_configuration;
	const char *path_tmp = path.c_str();
	std::string tmp(path_tmp);
	if (chdir(path_tmp) != 0)
		throw IOException("error : chdir failed");
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

CGIResponse::CGIResponse(HTTPRequest &request): _request(request)
{
	/* CONFIGURATION */
	_server_location_log = set_server_location("/server");
	_default_file = set_default_file("../index.html");
}

CGIResponse::~CGIResponse() {}
