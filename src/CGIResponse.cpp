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
		throw HTTPException(404);
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
		throw HTTPException(404);
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

void CGIResponse::check_existing_dir(std::string &directory)
{
	const char *directory_location;
	DIR *dir;
	std::string directory_absolut_path;

	directory = _server_root + directory;
	directory_absolut_path = set_absolut_path(directory);
	directory_location = directory_absolut_path.c_str();
	dir = opendir(directory_location);
	std::cout << "directory_absolut_path: " << directory_absolut_path << std::endl;
	if (!dir)
		throw HTTPException(404);
	closedir(dir);
}

void CGIResponse::set_rules_location()
{
	if (_request == NULL)
		return;
	URI uri(_request->_path);
	_directory_location = uri.getFileDirectory();
	for (std::vector<Configuration::loc_inf>::const_iterator it = _server_location_info.begin(); it != _server_location_info.end(); it++)
	{
		if (_directory_location == (*it).directory)
		{
			_GET = (*it).GET;
			_POST = (*it).POST;
			_DELETE = (*it).DELETE;
			_loc_root = (*it).root;
			_dir_listing = (*it).dir_listing;
			_server_index = (*it).def_file;
			check_existing_dir(_loc_root);
		}
	}
}

CGIResponse::CGIResponse(const HTTPRequest *request): _request(request), _GET(true), _POST(true), _DELETE(false), _dir_listing(false), _directory_location(""), _loc_root("")
{

	Configuration config = Configuration::getInstance();

	//std::cout << config << std::endl;

	/* CONFIGURATION */
	_error_pages  = config.get_server_error_page_location();
	_accept_file = config.get_server_file_acceptance();
	_server_location_info = config.get_location_specifier();
	_server_root = config.get_server_root_folder();
	_server_index = config.get_server_index_file();

	set_rules_location();

	if (_loc_root == "")
		_server_location_log = set_absolut_path(_server_root);
	else
		_server_location_log = set_absolut_path(_loc_root);
	_default_file = set_default_file(_server_index); // ?

	#if DEBUG
		std::cout << "_server_index: " << _server_index
		<< "\n _server_root: " << _server_root
		<< "\n _server_location_log: " << _server_location_log
		<< std::endl;
	#endif
	/* TEMPORARY */
	_upload = "./upload";
}

CGIResponse::~CGIResponse() {}

bool CGIResponse::isRunning() {
    return false;
}
