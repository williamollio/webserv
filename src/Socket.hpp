#pragma once

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>

class Socket {
	private:
		// std::string	_req_info;
		// std::string	_host;
		// std::string	_connection;
		// std::string	cache_control;
		// int			_upgrade_insec_req;
		// std::string	_user_agent;
		// std::string	_accept;
		// int			_dnt;
		// std::string	_encoding;
		// std::string	_lang;

		int			_fd;
		std::string _type_header;
		std::string _filename;
	public:
		Socket();
		Socket(int fd);
		~Socket();
		std::string read_socket();
		void send_header(std::string type);
		void send_file(std::string name);
		void send(std::string content);
		void close_socket();
};