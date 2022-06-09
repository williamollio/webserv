#include "Connection.hpp"
#include "HTTPReader.hpp"

Connection::Connection() : _fds()
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
	fcntl(server_fd, F_SETFL, O_NONBLOCK);
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
		throw std::exception();
	if (listen(server_fd, 10) < 0)
		throw std::exception();
	initialization_poll();
}

void Connection::initialization_poll()
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
	bool close_conn;

	_timeout = (3 * 60 * 1000);
	do
	{
		rc = poll(_fds, nfds, _timeout);
		if (rc <= 0)
		{
			std::cout << "poll() failed or timeout" << std::endl;
			break;
		}
		current_size = nfds;
		for (int i = 0; i < current_size; i++)
    	{
			printf("nfds %d current_size %d i %d\n", nfds, current_size, i);

      	if(_fds[i].revents == 0)
      	  continue;

      	if(_fds[i].revents != POLLIN)
      	{
      	  printf("  Error! revents = %d\n", _fds[i].revents);
      	  end_server = true;
      	  break;
      	}
      	if (_fds[i].fd == server_fd)
      	{
      	  printf("  Listening socket is readable\n");
      	  do
      	  {
				try
				{
					Socket socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen); // NULL NULL
      	    		printf("  New incoming connection - %d\n", socket.get_fd());
      	    		_fds[nfds].fd = socket.get_fd();
      	    		_fds[nfds].events = POLLIN;
      	    		nfds++;
				}
				catch(const std::exception& e)
				{
					break;
				}

      	  } while (1);
      	}
      	else
      	{
      	  printf("  Descriptor %d is readable\n", _fds[i].fd);
      	  close_conn = false;
      	  Socket socket = _fds[i].fd;
      	  std::cout << socket.read_socket() << std::endl; // exception
			socket.send_header("html");
			socket.send_file("index.html");
      	  socket.close_socket();
      	  _fds[i].fd = -1;
      	}
	  }
    } while (end_server == false);
	  for (int i = 0; i < nfds; i++)
  		{
    		if(_fds[i].fd >= 0)
      		close(_fds[i].fd);
  		}
}
