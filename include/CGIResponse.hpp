//
// Created by Manuel Hahn on 6/10/22.
//

#ifndef WEBSERV_CGIRESPONSE_HPP
#define WEBSERV_CGIRESPONSE_HPP

#include "Socket.hpp"
#include "HTTPRequest.hpp"
#include "HTTPException.hpp"
#include "HTTPHeader.hpp"
#include "Tool.hpp"
#include "Configuration.hpp"
#include <dirent.h>
#include <errno.h>
#include <fstream>

class CGIResponse {
public:
    explicit CGIResponse(const HTTPRequest *);
    virtual ~CGIResponse();

    virtual void	run(Socket & socket) = 0;
    virtual bool	isRunning();
	std::string		set_absolut_path(std::string path_from_configuration);
	std::string		set_default_file(std::string file);
	std::string		read_file(std::string file);
	std::string		get_current_path();
	void			set_rules_location();
	void			check_existing_dir(std::string &dir);

protected:
	const HTTPRequest *	_request;
	std::string			_current_path;
	std::string			_server_location_log;
	std::string			_default_file;
	std::string			_upload;
	std::string			_server_root;
	std::string			_loc_root;
	std::string			_server_index;
	std::string			_directory_location;
	bool				_accept_file;
	bool				_GET;
	bool				_POST;
	bool				_DELETE;
	bool				_dir_listing;
	std::map<int, std::string>	_error_pages;
	std::vector<Configuration::loc_inf>	_server_location_info;
};


#endif //WEBSERV_CGIRESPONSE_HPP
