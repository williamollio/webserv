//
// Created by Manuel Hahn on 6/10/22.
//

#include "../include/CGIResponse.hpp"
#include "../include/Configuration.hpp"
#include "../include/URI.hpp"

std::string CGIResponse::get_current_path()
{
	char buf[256];

	if (getcwd(buf, sizeof(buf)) == NULL)
	{
		PRINT_ERROR_CODE("Error code : ", 404);
		throw HTTPException(404);
	}
	std::string current_path(buf);
	return (current_path);
}


std::string CGIResponse::set_absolut_path(std::string path_from_configuration)
{
	std::string tmp = "";

	_current_path = get_current_path();
	_current_path += path_from_configuration;
	const char * path_tmp = _current_path.c_str();
	tmp = std::string(path_tmp);
	if (chdir(path_tmp) != 0)
	{
		PRINT_ERROR_CODE("Error code : ", 404);
		throw HTTPException(404);
	}
	return (tmp);
}

std::string CGIResponse::read_file(std::string file)
{
	std::ifstream is;
	is.open(file);
	if (!is.is_open())
	{
		PRINT_ERROR_CODE("Error code : ", 404);
		throw HTTPException(404);
	}

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

void CGIResponse::check_existing_dir(std::string &directory)
{
	const char *directory_location;
	DIR *dir;
	std::string directory_absolut_path;

	directory = _server_root + directory;
	directory_absolut_path = set_absolut_path(directory);
	directory_location = directory_absolut_path.c_str();
	dir = opendir(directory_location);
	#if DEBUG
		std::cout << "directory_absolut_path: " << directory_absolut_path << std::endl;
	#endif
	if (!dir)
	{
		PRINT_ERROR_CODE("Error code : ", 404);
		throw HTTPException(404);
	}
	closedir(dir);
}

void CGIResponse::set_rules_location(std::vector<Configuration::loc_inf>::const_iterator it)
{
	_GET = (*it).GET;
	_POST = (*it).POST;
	_DELETE = (*it).DELETE;
	_loc_root = (*it).root;
	_dir_listing = (*it).dir_listing;
	_server_index = (*it).def_file;
	return;
}

int CGIResponse::is_request_defined_location(const std::string &request_path, std::vector<Configuration::loc_inf> server_location_info)
{
	URI uri(request_path);
	_directory_location = uri.getFileDirectory();
	for (std::vector<Configuration::loc_inf>::const_iterator it = server_location_info.begin(); it != server_location_info.end(); it++)
	{
		if (_directory_location == (*it).directory)
		{
			set_rules_location(it);
			return (1);
		}
	}
	return (0);
}

CGIResponse::CGIResponse(const HTTPRequest *request): _request(request), _GET(true), _POST(true), _DELETE(false), _dir_listing(false), _directory_location(""), _loc_root("")
{}

CGIResponse::~CGIResponse() {}

bool CGIResponse::isRunning() {
    return false;
}
