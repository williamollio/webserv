//
// Created by Manuel Hahn on 6/10/22.
//

#include "../include/CGIResponse.hpp"
#include "../include/IOException.hpp"

std::string CGIResponse::set_server_location(std::string path_from_configuration)
{
	char buf[256];
	if (getcwd(buf, sizeof(buf)) == NULL)
		throw IOException("getcwd failed");
	std::string path(buf);
	std::cout << "path current:" << path << std::endl;
	path += path_from_configuration;
	std::cout << "path_from_configuration :" << path_from_configuration << std::endl;
	const char *path_tmp = path.c_str();
	std::string tmp(path_tmp);
	std::cout << "path changed:" << path << std::endl;
	if (chdir(path_tmp) != 0)
		throw IOException("chdir failed");
	return (tmp);
}

CGIResponse::CGIResponse(HTTPRequest &request): _request(request)
{
	/* Call Configuration getter static object here */
	_server_location_log = set_server_location("/server");

}

CGIResponse::~CGIResponse() {}
