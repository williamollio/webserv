#include "Connection.hpp"

Connection::Connection()
{
	addrlen = sizeof(address);
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );

	memset(address.sin_zero, '\0', sizeof address.sin_zero);

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		throw std::exception();
	on = 1;
	setsockopt (server_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof (on));
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
		throw std::exception();
	if (listen(server_fd, 10) < 0)
		throw std::exception();
}

Connection::~Connection() {}

void Connection::establishConnection()
{
	while (1)
	{
		try
		{
			Socket socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
			std::cout << socket.read_socket() << std::endl;
			socket.send_header("html");
			socket.send_file("index.html");
			socket.close_socket();
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}
	}
}