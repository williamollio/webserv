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
        std::vector<int>        server_fds;
		int                     addrlen;
		int                     on;
		int                     _timeout;
		struct sockaddr_in      address;
		struct pollfd           _fds[200];
        std::list<HTTPReader *> list;

		void _initialization_poll();
        void cleanReaders();
        bool isServingFD(int fd);

        class ReaderByFDFinder {
        public:
            explicit ReaderByFDFinder(int fd);

            bool operator()(HTTPReader* &) const;

        private:
            const int fd;
        };

	public:
		Connection();
		~Connection();

		void establishConnection();
};