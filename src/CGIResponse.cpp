//
// Created by Manuel Hahn on 6/10/22.
//

#include "../include/CGIResponse.hpp"
#include "../include/Configuration.hpp"
#include "../include/URI.hpp"


void CGIResponse::trim_slash_begin(std::string& str)
{
	if (str.front() == '/')
		str.erase(0,1);
}

void CGIResponse::trim_slash_end(std::string& str)
{
	if (str.back() == '/')
		str.pop_back();
}

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

std::string CGIResponse::set_absolut_path(std::string& folder)
{
	std::string new_path;
	const char* path_tmp;

	new_path = get_current_path() + folder;
	path_tmp = new_path.c_str();
	if (access(path_tmp, R_OK) < 0)
	{
		PRINT_ERROR_CODE("Error code : ", 404);
		throw HTTPException(404);
	}
	return (new_path);
}

void CGIResponse::add_slash_first_if_needed(std::string& file)
{
	if (file.front() != '/' && _server_location_log.back() != '/')
		file = _server_location_log + "/" + file;
	else
		file = _server_location_log + file;
}

std::string CGIResponse::read_file(std::string file)
{
	std::ifstream is;
	std::stringstream buffer;

	add_slash_first_if_needed(file);
	is.open(file);
	if (!is.is_open())
	{
		PRINT_ERROR_CODE("Error code : ", 404);
		throw HTTPException(404);
	}
	buffer << is.rdbuf();
	is.close();
	return (buffer.str());
}

void CGIResponse::set_rules_location(std::vector<Configuration::loc_inf>::const_iterator it)
{
	_GET = (*it).GET;
	_POST = (*it).POST;
	_DELETE = (*it).DELETE;
	_loc_root = _server_root + (*it).root;
	_dir_listing = (*it).dir_listing;
	_server_index = (*it).def_file;
	return;
}

int CGIResponse::is_request_defined_location(const std::string &request_path, std::vector<Configuration::loc_inf> server_location_info)
{
	URI uri(request_path);
	_directory_location = uri.getFileDirectory();
	trim_slash_end(_directory_location);
	for (std::vector<Configuration::loc_inf>::const_iterator it = server_location_info.begin(); it != server_location_info.end(); it++)
	{
		if (_directory_location == (*it).directory)
		{
			set_rules_location(it);
			return (1);
		}
	}
	_directory_location = '/';
	return (0);
}

CGIResponse::CGIResponse(const HTTPRequest *request): _request(request), _GET(true), _POST(true), _DELETE(false), _dir_listing(false), _directory_location(""), _loc_root("")
{}

CGIResponse::~CGIResponse() {}

bool CGIResponse::isRunning() {
    return false;
}
