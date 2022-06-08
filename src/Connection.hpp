#pragma once
#include "Socket.hpp"
#include "HTTPReader.hpp"
#include <sys/socket.h>
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


	public:
		Connection();
		~Connection();
		void	establishConnection();
	protected:
};