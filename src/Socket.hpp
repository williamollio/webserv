#pragma once

#include <string>

class Socket {
private:
	std::string	_req_info;
	std::string	_host;
	std::string	_connection;
	std::string	cache_control;
	int			_upgrade_insec_req;
	std::string	_user_agent;
	std::string	_accept;
	int			_dnt;
	std::string	_encoding;
	std::string	_lang;
	std::string	_buffer;
	int			_this_socket;
public:
	Socket();
	~Socket();
};