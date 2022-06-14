#pragma once
#include "Socket.hpp"
#include "HTTPReader.hpp"
#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <list>

#define PORT 80
class Connection {
	private:
		int server_fd;
		struct sockaddr_in address;
		int addrlen;
		int on;
        std::list<HTTPReader*> list;
		struct pollfd _fds[200];
		int _timeout;
		void	_initialization_poll();


	public:
		Connection();
		~Connection();
		void	establishConnection();
	protected:
};