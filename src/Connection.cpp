#include "Connection.hpp"
#include "HTTPReader.hpp"

Connection::Connection() : address(), _fds(), _timeout(3 * 60 * 1000)
{
	addrlen = sizeof(address);
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		throw std::exception();
	on = 1;
	setsockopt (server_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof (on));
	fcntl(server_fd, F_SETFL, O_NONBLOCK);
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
		throw std::exception();
	if (listen(server_fd, 10) < 0)
		throw std::exception();
	_initialization_poll();
}

void Connection::_initialization_poll()
{
	_fds[0].fd = server_fd;
	_fds[0].events = POLLIN;
}

static void removeHTTPReader(HTTPReader* & reader) {
    delete reader;
}

Connection::~Connection() {
    std::for_each(list.begin(), list.end(), ::removeHTTPReader);
}

void Connection::establishConnection()
{
	int rc;
	int nfds = 1;
	int current_size;
	bool end_server;

	do {
		rc = poll(_fds, nfds, _timeout);
		if (rc <= 0) {
			std::cout << "poll() failed or timeout" << std::endl;
			break;
		}
		current_size = nfds;
		for (int i = 0; i < current_size; i++)
    	{
			std::cout << "nfds " << nfds << " current_size " << current_size << " i " << i << std::endl;
      	    if (_fds[i].revents == 0)
                continue;
            if(_fds[i].revents != POLLIN)
      	    {
                std::cout << "Error! revents = " << _fds[i].revents << std::endl;
                end_server = true;
                break;
      	    }
            if (_fds[i].fd == server_fd)
      	    {
                std::cout << "Can read from listening socket" << std::endl;
                int socketDescriptor;
                while ((socketDescriptor = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &addrlen)) >= 0) { // NULL NULL
                    std::cout << "New connection: " << _fds[nfds].fd << std::endl;
                    _fds[nfds].fd = socketDescriptor;
                    _fds[nfds].events = POLLIN;
                    nfds++;
                }
      	    } else {
                std::cout << "Can read from " << _fds[i].fd << std::endl;
      	        Socket socket = _fds[i].fd;
                std::cout << socket.read_socket() << std::endl; // exception
			    socket.send_header("html");
			    socket.send_file("index.html");
      	        socket.close_socket();
      	        _fds[i].fd = -1;
      	    }
	    }
    } while (!end_server);
    for (int i = 0; i < nfds; i++)
  	{
    	if(_fds[i].fd >= 0)
    	    close(_fds[i].fd);
  	}
}
