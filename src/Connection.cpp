#include "Connection.hpp"
#include "HTTPReader.hpp"
#include "IOException.hpp"
#include "Configuration.hpp"
#include "CGIResponseError.hpp"
#include <poll.h>
#include <netdb.h>

#define INFTIM -1

Connection::Connection() : _timeout(INFTIM), address(), _fds()
{
    const std::vector<int> & ports = Configuration::getInstance().get_server_ports();
    for (unsigned long i = 0; i < ports.size(); ++i) {
        addrlen = sizeof(address);
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(ports[i]);

        int server_fd;
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
            throw IOException("Could not create socket!");
        on = 1;
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        fcntl(server_fd, F_SETFL, O_NONBLOCK);
        if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0)
            throw IOException("Could not bind file descriptor to the address!");
        if (listen(server_fd, 10) < 0)
            throw IOException("Cannot listen on the socket descriptor!");
        _fds[i].fd = server_fd;
        _fds[i].events = POLLIN;
        server_fds[server_fd] = ports[i];
    }
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
    unsigned long nfds = server_fds.size();
    unsigned long current_size;
    bool end_server = false;

    do {
        rc = poll(_fds, nfds, _timeout);
        if (rc <= 0) {
            std::cout << "poll() failed or timeout" << std::endl;
            break;
        }
        current_size = nfds;
        for (unsigned long i = 0; i < current_size; i++)
        {
            if (_fds[i].revents == 0) {
                // Ignore...
            } else if (_fds[i].revents == POLLERR || _fds[i].revents == POLL_HUP) {
                ReaderByFDFinder finder(_fds[i].fd);
                std::list<HTTPReader *>::iterator it = std::find_if(list.begin(), list.end(), finder);
                if (it != list.end()) {
                    delete *it;
                    *it = NULL;
                }
                list.remove(NULL);
                close(_fds[i].fd);
                removeFD(i);
            } else if (_fds[i].revents == POLLNVAL) {
                removeFD(i);
            } else if (isServingFD(_fds[i].fd)) {
                int socketDescriptor;
                while ((socketDescriptor = accept(_fds[i].fd, NULL, NULL)) >= 0) {
                    connectionPairs[socketDescriptor] = _fds[i].fd;
                    _fds[nfds].fd = socketDescriptor;
                    _fds[nfds].events = POLLIN;
                    nfds++;
                    // TODO: Too many requests!
                }
      	    } else {
                handleConnection(i);
      	    }
        }
        nfds = clearPollArray(nfds);
        cleanReaders();
    } while (!end_server);
    for (unsigned long i = 0; i < nfds; i++) {
    	if(_fds[i].fd >= 0)
    	    close(_fds[i].fd);
    }
}

void Connection::removeFD(const unsigned long index) {
    connectionPairs.erase(_fds[index].fd);
    _fds[index].fd = -1;
}

void Connection::handleConnection(const unsigned long index) _NOEXCEPT {
    Socket socket = _fds[index].fd;
    try {
        HTTPReader * reader = new HTTPReader(socket);
        getpeername(socket.get_fd(), (struct sockaddr *) &address, (socklen_t *) &addrlen);
        reader->setPeerAddress(ntohl(address.sin_addr.s_addr));
        char * host = new char[50]();
        getnameinfo((struct sockaddr *) &address, (socklen_t) addrlen, host, (socklen_t) 50, NULL, 0, 0);
        reader->setPeerName(host);
        reader->setUsedPort(server_fds[connectionPairs[socket.get_fd()]]);
        delete[] host;
        list.push_back(reader);
        reader->run();
    } catch (std::bad_alloc & ex) {
        CGIResponseError response;
        response.set_error_code(507);
        response.run(socket);
    } catch (std::exception & ex) {
        CGIResponseError response;
        response.set_error_code(500);
        response.run(socket);
    }
    removeFD(index);
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

bool Connection::isServingFD(int fd) {
    std::map<int, int>::const_iterator it = server_fds.find(fd);
    return it != server_fds.end();
}

unsigned long Connection::clearPollArray(unsigned long nfds) {
    unsigned long i, j;
    for (i = 0, j = 0; i < nfds; ++i) {
        if (_fds[i].fd != -1) {
            _fds[j++] = _fds[i];
        }
    }
    return j;
}

// R E A D E R B Y F D F I N D E R   I M P L E M E N T A T I O N

Connection::ReaderByFDFinder::ReaderByFDFinder(int fd) : fd(fd) {}

bool Connection::ReaderByFDFinder::operator()(HTTPReader *  & reader) const {
    return reader->getSocket().get_fd() == fd;
}
