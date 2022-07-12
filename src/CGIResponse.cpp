//
// Created by Manuel Hahn on 6/10/22.
//

#include "CGIResponse.hpp"

std::string CGIResponse::set_extension(std::string& file)
{
	if (_file_extension != "")
		return _file_extension;
	size_t pos = file.rfind('.');
	if (pos == std::string::npos)
		return ("");
	else
		return (file.substr(pos + 1));
}

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

bool CGIResponse::is_sub_folder_location(const std::string& file)
{
	size_t pos = file.rfind("/");
	size_t pos2 = _server_location_log.rfind("/");
	if ((pos == std::string::npos) || (pos2 == std::string::npos))
		throw HTTPException(418);
	std::string tmp = file.substr(pos);
	std::string tmp2 = _server_location_log.substr(pos2);
	return (tmp.compare(tmp2));
}

void CGIResponse::construct_file_path(std::string& file)
{
	if (file.front() != '/' && _server_location_log.back() != '/')
		file = _server_location_log + "/" + file;
	else
		file = _server_location_log + file;

	PRINT_CGIRESPONSE("file", file);
	PRINT_CGIRESPONSE("_server_location_log", _server_location_log);
	if (!_loc_root.empty())
	{
		DIR* dir;
		dir = opendir(file.c_str());
		if (dir && is_sub_folder_location(file))
		{
			closedir(dir);
			throw HTTPException(404);
		}
		closedir(dir);
	}
}

bool CGIResponse::is_request_folder(const std::string& path)
{
	DIR* dir;
	std::string tmp(path);
	construct_file_path(tmp);
	if ((_file_extension = set_extension(tmp)) != "")
		return (false);
	const char *path_tmp = tmp.c_str();
	dir = opendir(path_tmp);
	if (dir)
	{
		closedir(dir);
		return (true);
	}
	closedir(dir);
	return (false);
}
std::string CGIResponse::read_file(std::string& file)
{
	std::ifstream is;
	std::stringstream buffer;
	construct_file_path(file);
	PRINT_CGIRESPONSEERROR("file:", file);
	if (_request != NULL)
		PRINT_CGIRESPONSE("request_path", _request->_path);
	PRINT_CGIRESPONSE("_server_location_log", _server_location_log);
	PRINT_CGIRESPONSE("_loc_root", _loc_root);
	is.open(file);
	if (!is.is_open())
	{
		is.close();
		PRINT_ERROR_CODE("Error code : ", 404);
		throw HTTPException(404);
	}
	buffer << is.rdbuf();
	is.close();
	return (buffer.str());
}

void CGIResponse::set_rules_location(std::string &request_path, std::vector<Configuration::loc_inf>::const_iterator it)
{
	_GET = (*it).GET;
	_POST = (*it).POST;
	_DELETE = (*it).DELETE;
	_loc_root = _server_root + (*it).root;
	_dir_listing = (*it).dir_listing;
	_server_index = (*it).def_file;

	size_t pos = request_path.find(_directory_location);
	size_t length = _directory_location.length();
	request_path.erase(pos, length);

	return;
}

int CGIResponse::is_request_defined_location(std::string &request_path, std::vector<Configuration::loc_inf> server_location_info)
{
	URI uri(request_path);
	_directory_location = uri.getFileDirectory();
	trim_slash_end(_directory_location);
	PRINT_CGIRESPONSE("_directory_location", _directory_location);
	for (std::vector<Configuration::loc_inf>::const_iterator it = server_location_info.begin(); it != server_location_info.end(); it++)
	{
		PRINT_CGIRESPONSE("(*it).directory", (*it).directory);
		if (_directory_location == (*it).directory)
		{
			std::cout << "1" << std::endl;
			set_rules_location(request_path, it);
			return (1);
		}
	}
	if (is_request_folder(request_path) == true && request_path != "/")
		throw HTTPException(401);
	_directory_location = '/';
	return (0);
}

CGIResponse::CGIResponse(HTTPRequest *request): _request(request), _GET(true), _POST(true), _DELETE(false), _dir_listing(false), _directory_location(""), _loc_root("") {
}

CGIResponse::~CGIResponse() {}

bool CGIResponse::isRunning() {
    return false;
}
