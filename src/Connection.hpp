#pragma once
#include "Socket.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#define PORT 80
class Connection {
	private:
		int server_fd;
		struct sockaddr_in address;
		int addrlen;
		int on;
	public:
		Connection();
		~Connection();
		void	establishConnection();
	protected:
};