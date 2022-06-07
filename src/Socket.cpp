#include "Socket.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#define PORT 82

Socket::Socket() {
	int server_fd;
	struct sockaddr_in address;
	int addrlen = sizeof(address);

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );

	memset(address.sin_zero, '\0', sizeof address.sin_zero);

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("In socket");
		exit(EXIT_FAILURE);
	}
	int on = 1;
	setsockopt (server_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof (on));
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("In bind");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 10) < 0)
	{
		perror("In listen");
		exit(EXIT_FAILURE);
	}

	_this_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
	char*	read_buffer = new char[30001];
	read( _this_socket , read_buffer, 30000);
	_buffer = read_buffer;
	std::cout << _buffer << std::endl;
	std::ifstream html_file("../index.html");
	std::stringstream	str_stream;
	str_stream << html_file.rdbuf();
	std::string output("HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ");
	output += std::to_string(str_stream.str().size());
	output += "\n";
	write(_this_socket, output.data(), output.size());
	output = str_stream.str();
	//std::cout << output;
	write(_this_socket, output.data(), output.size());
	close(server_fd);
	close(_this_socket);
}