#include "Connection.hpp"
#include "HTTPReader.hpp"
#include "IOException.hpp"
#include <poll.h>
#include <netdb.h>

#define INFTIM -1

Connection::Connection() : _timeout(INFTIM), address(), _fds()
{
    addrlen = sizeof(address);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        throw IOException("Could not create socket!");
    on = 1;
    setsockopt (server_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof (on));
    fcntl(server_fd, F_SETFL, O_NONBLOCK);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
        throw IOException("Could not bind file descriptor to the address!");
    if (listen(server_fd, 10) < 0)
        throw IOException("Cannot listen on the socket descriptor!");
    _initialization_poll();
}

void Connection::_initialization_poll()
{
    _fds[0].fd = server_fd;
    _fds[0].events = POLLIN;
}

static void forceRemoveHTTPReader(HTTPReader* & reader) {
    delete reader;
}

Connection::~Connection() {
    std::for_each(list.begin(), list.end(), forceRemoveHTTPReader);
}

void Connection::establishConnection()
{
    int rc;
    int nfds = 1;
    int current_size;
    bool end_server = false;

    do {
        rc = poll(_fds, nfds, _timeout);
        if (rc <= 0) {
            std::cout << "poll() failed or timeout" << std::endl;
            break;
        }
        current_size = nfds;
        for (int i = 0; i < current_size; i++)
        {
      	    if (_fds[i].revents == 0)
                continue;
//            if(_fds[i].revents != POLLIN)
//      	    {
//                std::cout << "Error! revents = " << _fds[i].revents << std::endl;
//                end_server = true;
//                break;
//      	    }
            else if (_fds[i].revents == POLLERR || _fds[i].revents == POLL_HUP) {
                ReaderByFDFinder finder(_fds[i].fd);
                std::list<HTTPReader *>::iterator it = std::find_if(list.begin(), list.end(), finder);
                if (it != list.end()) {
                    delete *it;
                    *it = NULL;
                }
                list.remove(NULL);
                close(_fds[i].fd);
                _fds[i].fd = -1;
                continue;
            } else if (_fds[i].revents == POLLNVAL) {
                _fds[i].fd = -1;
                continue;
            }
            if (_fds[i].fd == server_fd)
      	    {
                int socketDescriptor;
                while ((socketDescriptor = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &addrlen)) >= 0) {
                    _fds[nfds].fd = socketDescriptor;
                    _fds[nfds].events = POLLIN;
                    nfds++;
                }
      	    } else {
                Socket socket = _fds[i].fd;
                HTTPReader * reader = new HTTPReader(socket);
                getpeername(socket.get_fd(), (struct sockaddr *) &address, (socklen_t *) &addrlen);
                reader->setPeerAddress(ntohl(address.sin_addr.s_addr));
                char * host = new char[50]();
                getnameinfo((struct sockaddr *) &address, (socklen_t) addrlen, host, (socklen_t) 50, NULL, 0, 0);
                reader->setPeerName(host);
                delete[] host;
                list.push_back(reader);
                reader->run();
      	        _fds[i].fd = -1;
      	    }
        }
        cleanReaders();
    } while (!end_server);
    for (int i = 0; i < nfds; i++) {
    	if(_fds[i].fd >= 0)
    	    close(_fds[i].fd);
    }
}

static void maybeDeleteReader(HTTPReader* & reader) {
    if (!reader->isRunning()) {
        delete reader;
        reader = NULL;
    }
}

void Connection::cleanReaders() {
    std::for_each(list.begin(), list.end(), maybeDeleteReader);
    list.remove(NULL);
}

// R E A D E R B Y F D F I N D E R   I M P L E M E N T A T I O N

Connection::ReaderByFDFinder::ReaderByFDFinder(int fd) : fd(fd) {}

bool Connection::ReaderByFDFinder::operator()(HTTPReader *  & reader) const {
    return reader->getSocket().get_fd() == fd;
}
