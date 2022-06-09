#include "Connection.hpp"
#include "HTTPReader.hpp"

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

static void removeHTTPReader(HTTPReader* & reader) {
    delete reader;
}

Connection::~Connection() {
    std::for_each(list.begin(), list.end(), ::removeHTTPReader);
}

void Connection::establishConnection()
{
	while (1)
	{
		try
		{
			Socket socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
            HTTPReader* reader = new HTTPReader(socket);
            list.push_back(reader);
            reader->run();
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}
	}
}
