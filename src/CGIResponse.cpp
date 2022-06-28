//
// Created by Manuel Hahn on 6/10/22.
//

#include "../include/CGIResponse.hpp"

std::string CGIResponse::set_server_location(std::string path_from_configuration)
{
	char buf[256];

	static std::string tmp = "";

	if (tmp == "")
	{
		if (getcwd(buf, sizeof(buf)) == NULL)
			throw HTTPException(404);
		std::string path(buf);
		path += path_from_configuration;
		const char * path_tmp = path.c_str();
		tmp = std::string(path_tmp);
		if (chdir(path_tmp) != 0)
			throw HTTPException(404);
	}
	return (tmp);
}

std::string CGIResponse::read_file(std::string file)
{
	std::ifstream is;
	is.open(file);
	if (!is.is_open())
		throw HTTPException(404);

	std::stringstream buffer;
	buffer << is.rdbuf();
	is.close();
	return (buffer.str());
}

std::string CGIResponse::set_default_file(std::string file)
{
	std::string tmp;

	tmp += _server_location_log;
	if (_server_location_log.back() != '/')
		tmp += "/";
	tmp += file;
	return (read_file(tmp));
}

CGIResponse::CGIResponse(HTTPRequest *request): _request(request)
{
	/* CONFIGURATION */
	_server_location_log = set_server_location("/server");
	_default_file = set_default_file("index.html");
	_upload = "./upload";
}

CGIResponse::~CGIResponse() {}

bool CGIResponse::isRunning() {
    return false;
}
