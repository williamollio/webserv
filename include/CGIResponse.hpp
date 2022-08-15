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
#include "Runnable.hpp"
#include <dirent.h>
#include <errno.h>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

class CGIResponseError;
class CGIResponse: public Runnable {
public:
    explicit CGIResponse(HTTPRequest *, Socket &);
    virtual ~CGIResponse();

    virtual void	run() = 0;
    virtual bool    hasFD(int);
    virtual bool	isRunning();
	std::string		set_absolut_path(std::string& folder);
	std::string		read_file(std::string& file);
	std::string		get_current_path();
	void			set_rules_location(std::string &request_path, std::vector<Configuration::loc_inf>::const_iterator it);
	int				is_request_defined_location(std::string& request_path, std::vector<Configuration::loc_inf> server_location_info);
	void			check_existing_dir(std::string& dir);
	void			construct_file_path(std::string& file);
	void			trim_slash_end(std::string& str);
	void			trim_slash_begin(std::string& str);
	bool			is_request_folder(const std::string& path);
	std::string		set_extension(std::string& file);
	bool			is_sub_folder_location(std::string& file);
	void			trim_directory_path(std::string &request_path);

protected:
	HTTPRequest *	_request;
	bool				_GET;
	bool				_POST;
	bool				_DELETE;
	bool				_dir_listing;
	bool				_accept_file;
	std::string			_directory_location;
	std::string			_loc_root;
	std::string			_server_location_log;
	std::string			_server_root;
	std::string			_server_index;
	std::string			_file_extension;
	std::map<int, std::string>	_error_pages;
	size_t				_upload_size;
	bool				_upload_size_bool;
    Socket &            _socket;
};


#endif //WEBSERV_CGIRESPONSE_HPP
