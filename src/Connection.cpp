#include "Connection.hpp"
#include "HTTPReader.hpp"
#include "IOException.hpp"
#include "Configuration.hpp"
#include "CGIResponseError.hpp"
#include <poll.h>
#include <netdb.h>

#define INFTIM -1

Connection * Connection::currentInstance = NULL;

Connection & Connection::getInstance() _NOEXCEPT {
    return *currentInstance;
}

Connection::Connection() : _timeout(INFTIM), address(), _fds() {
    const std::vector<int> & ports = Configuration::getInstance().get_server_ports();
    for (unsigned long i = 0; i < ports.size(); ++i) {
        addrlen = sizeof(address);
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(ports[i]);

        int server_fd;
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            throw IOException("Could not create socket!");
        }
        on = 1;
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        fcntl(server_fd, F_SETFL, O_NONBLOCK);
        if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
            throw IOException("Could not bind file descriptor to the address!");
        }
        if (listen(server_fd, 10) < 0) {
            throw IOException("Cannot listen on the socket descriptor!");
        }
        _fds[i].fd = server_fd;
        _fds[i].events = POLLIN;
        server_fds[server_fd] = ports[i];
    }
    currentInstance = this;
}

static void forceRemoveHTTPReader(HTTPReader* & reader) {
    delete reader;
}

Connection::~Connection() {
    std::for_each(list.begin(), list.end(), forceRemoveHTTPReader);
}

void Connection::establishConnection() {
    int rc;
    unsigned long current_size;

    nfds = server_fds.size();
    while ((rc = poll(_fds, nfds, _timeout)) > 0) {
        current_size = nfds;
        for (unsigned long i = 0; i < current_size; i++) {
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
                    // TODO: Remove protection
                    if (nfds == NUM_FDS) {
                        clearPollArray();
                        if (nfds == NUM_FDS) {
                            denyConnection(socketDescriptor);
                            continue;
                        }
                    }
                    // -----------------------
                    connectionPairs[socketDescriptor] = _fds[i].fd;
                    addFD(socketDescriptor);
                }
            } else {
                handleConnection(i);
            }
        }
        clearPollArray();
        cleanReaders();
    }
    for (unsigned long i = 0; i < nfds; i++) {
    	if(_fds[i].fd >= 0) {
            close(_fds[i].fd);
        }
    }
    if (rc == 0) {
        std::cout << "Time out, exiting..."      << std::endl;
    } else {
        std::cerr << "Polling error! Exiting..." << std::endl;
    }
}

void Connection::addFD(int fd, bool read) _NOEXCEPT {
    if (nfds == NUM_FDS) {
        clearPollArray();
        if (nfds == NUM_FDS) {
            // FIXME: What happens if there is no space left for polling?
            throw false;
        }
    }
    _fds[nfds].fd = fd;
    _fds[nfds].events = read ? POLLIN : POLLOUT;
    nfds++;
}

void Connection::removeFD(const unsigned long index) _NOEXCEPT {
    connectionPairs.erase(_fds[index].fd);
    _fds[index].fd = -1;
}

void Connection::denyConnection(const int fd, const int errorCode) const _NOEXCEPT {
    try {
        Socket socket = fd;
        CGIResponseError response;
        response.set_error_code(errorCode);
        response.run(socket);
    } catch (std::exception & ex) {
        std::cerr << "Could not send error " << errorCode << "!" << std::endl
                  << "Exception: " << ex.what()                  << std::endl;
    }
    close(fd);
}

void Connection::handleConnection(const unsigned long index) _NOEXCEPT {
    const int fd = _fds[index].fd;
	HTTPReader * reader;
    try {
        Socket socket = fd;
		ReaderByFDFinder rfd(fd);
		std::list<HTTPReader *>::iterator my_reader = std::find_if(list.begin(), list.end(), rfd);
		if (my_reader != list.end()) {
			reader = *my_reader;
            if (reader->runForFD(fd)) {
                removeFD(index);
            }
		} else {
			reader = new HTTPReader(socket);
            getpeername(socket.get_fd(), (struct sockaddr *) &address, (socklen_t *) &addrlen);
            reader->setPeerAddress(ntohl(address.sin_addr.s_addr));
            char * host = new char[50]();
            getnameinfo((struct sockaddr *) &address, (socklen_t) addrlen, host, (socklen_t) 50, NULL, 0, 0);
            reader->setPeerName(host);
            delete[] host;
            reader->setUsedPort(server_fds[connectionPairs[socket.get_fd()]]);
            list.push_back(reader);
            reader->run();
            removeFD(index);
		}
    } catch (std::bad_alloc & ex) {
        denyConnection(fd, 507);
    } catch (std::exception & ex) {
        std::cerr << ">>>>>>> " << ex.what() << " <<<<<<<" << std::endl;
        denyConnection(fd, 500);
    }
}

static void maybeDeleteReader(HTTPReader* & reader) {
    if (!reader->isRunning()) {
        delete reader;
        reader = NULL;
    }
}

void Connection::cleanReaders() _NOEXCEPT {
    std::for_each(list.begin(), list.end(), maybeDeleteReader);
    list.remove(NULL);
}

bool Connection::isServingFD(int fd) _NOEXCEPT {
    std::map<int, int>::const_iterator it = server_fds.find(fd);
    return it != server_fds.end();
}

void Connection::clearPollArray() _NOEXCEPT {
    unsigned long i, j;
    for (i = 0, j = 0; i < nfds; ++i) {
        if (_fds[i].fd != -1) {
            _fds[j++] = _fds[i];
        }
    }
    nfds = j;
}

// R E A D E R B Y F D F I N D E R   I M P L E M E N T A T I O N

Connection::ReaderByFDFinder::ReaderByFDFinder(int fd) : fd(fd) {}

bool Connection::ReaderByFDFinder::operator()(HTTPReader *  & reader) const {
    return reader->hasFD(fd);
    //return reader->getSocket().get_fd() == fd;
}
