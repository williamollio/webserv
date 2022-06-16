#pragma once

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <fcntl.h>
#include "IOException.hpp"

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
		Socket(int fd) throw (IOException);
		~Socket();
		std::string read_socket() throw (IOException);
		void send_header(const std::string & type);
		void send(const std::string & content) throw (IOException);
		void close_socket() const throw(IOException);
		int get_fd() const _NOEXCEPT;
};