#include "Connection.hpp"
#include "IOException.hpp"
#include "Configuration.hpp"
#include "CGIResponseError.hpp"
#include <netdb.h>
#include <fcntl.h>

#define INFTIM -1

static volatile bool end_server = false;

static void stopper(int) {
    end_server = true;
}

Connection * Connection::currentInstance = NULL;

Connection & Connection::getInstance() _NOEXCEPT {
    return *currentInstance;
}

Connection::Connection() : _timeout(INFTIM), nfds(), address(), _fds() {
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
        if (bind(server_fd, reinterpret_cast<struct sockaddr *>(&address), sizeof(address)) < 0) {
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

void wrapper() {
    Connection::getInstance().printPollArray();
}

void Connection::establishConnection() {
    int rc;
    unsigned long current_size;

    signal(SIGINT, stopper);
    signal(SIGTERM, stopper);
    signal(SIGKILL, stopper);
    signal(SIGUSR1, reinterpret_cast<void (*)(int)>(wrapper));
    nfds = server_fds.size();
    while ((rc = poll(_fds, nfds, _timeout)) > 0 && !end_server) {
        //printPollArray();
        current_size = nfds;
        for (unsigned long i = 0; i < current_size; i++) {
            if (_fds[i].revents == 0) {
                // Ignore...
            } else if (_fds[i].revents == POLLERR || _fds[i].revents == POLLHUP) {
                ReaderByFDFinder finder(_fds[i].fd);
                std::list<HTTPReader *>::iterator it = std::find_if(list.begin(), list.end(), finder);
                if (it != list.end()) {
                    delete *it;
                    *it = NULL;
                }
                list.remove(NULL);
                debug("Closing " << _fds[i].fd);
                close(_fds[i].fd);
                removeFD(_fds[i].fd);
            } else if (_fds[i].revents == POLLNVAL) {
                removeFD(_fds[i].fd);
            } else if (isServingFD(_fds[i].fd)) {
                int socketDescriptor;
                while ((socketDescriptor = accept(_fds[i].fd, NULL, NULL)) >= 0) {
                    if (!addFD(socketDescriptor)) {
                        denyConnection(socketDescriptor);
                        continue;
                    }
                    connectionPairs[socketDescriptor] = _fds[i].fd;
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
    if (end_server) {
        std::cout << "Interrupted, exiting..." << std::endl;
    } else if (rc == 0) {
        std::cout << "Time out, exiting..."      << std::endl;
    } else {
        std::cerr << "Polling error! Exiting..." << std::endl;
    }
    signal(SIGINT, SIG_DFL);
    signal(SIGKILL, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
}

bool Connection::addFD(int fd, bool read) _NOEXCEPT {
    if (nfds == NUM_FDS) {
        clearPollArray();
        if (nfds == NUM_FDS) {
            return false;
        }
    }
    _fds[nfds].fd = fd;
    _fds[nfds].events = read ? POLLIN : POLLOUT;
    debug("Added " << fd << " (" << nfds << ")");
    nfds++;
    return true;
}

bool Connection::removeFD(const int fd) _NOEXCEPT {
    unsigned long i;
    for (i = 0; i < nfds && _fds[i].fd != fd; ++i);
    if (i != nfds) {
        connectionPairs.erase(_fds[i].fd);
        debug("Removed " << _fds[i].fd << " (" << i << ")");
        _fds[i].fd = -1;
    }
    // TODO: Performance?
    bool ret = i != nfds;
    clearPollArray();
    return ret;
}

void Connection::denyConnection(const int fd, HTTPReader * reader, const int errorCode) _NOEXCEPT {
    try {
        Socket socket(fd);
        if (reader != NULL) {
            socket.move(reader->getSocket(), false);
            list.remove(reader);
            delete reader;
        }
        removeFD(fd);
        CGIResponseError response(socket);
        response.set_error_code(errorCode);
        response.run(socket);
    } catch (std::exception & ex) {
        std::cerr << "Could not send error " << errorCode << "!" << std::endl
                  << "Exception: " << ex.what()                  << std::endl;
    }
}

void Connection::handleConnection(const unsigned long index) _NOEXCEPT {
    const int fd = _fds[index].fd;
	HTTPReader * reader = NULL;
    try {
		ReaderByFDFinder rfd(fd);
		std::list<HTTPReader *>::iterator my_reader = std::find_if(list.begin(), list.end(), rfd);
		if (my_reader != list.end()) {
			reader = *my_reader;
            debug("Continuing " << fd);
            if (reader->runForFD(fd)) {
                removeFD(fd);
            }
		} else {
			reader = new HTTPReader(fd);
            getpeername(fd, reinterpret_cast<struct sockaddr *>(&address), reinterpret_cast<socklen_t *>(&addrlen));
            reader->setPeerAddress(ntohl(address.sin_addr.s_addr));
            char * host = new char[50]();
            getnameinfo(reinterpret_cast<struct sockaddr *>(&address), static_cast<socklen_t>(addrlen), host, static_cast<socklen_t>(50), NULL, 0, 0);
            reader->setPeerName(host);
            delete[] host;
            reader->setUsedPort(server_fds[connectionPairs[fd]]);
            list.push_back(reader);
            debug("Starting " << fd);
            if (reader->run()) {
                removeFD(fd);
            }
		}
    } catch (std::bad_alloc &) {
        denyConnection(fd, reader, 507);
    } catch (std::exception & ex) {
        std::cerr << ">>>>>>> " << ex.what() << " <<<<<<<" << std::endl;
        denyConnection(fd, reader, 500);
    }
}

static void maybeDeleteReader(HTTPReader* & reader) {
	//if (reader->getSocket().closed()) {
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

void Connection::printPollArray() _NOEXCEPT {
/*#ifdef DEBUG
 #if DEBUG == 2*/
    std::cout << std::endl << __FILE__ << ":" << __LINE__ << " Poll array" << std::endl;
    for (unsigned long i = 0; i < nfds; ++i) {
        std::cout << "fd:      " << _fds[i].fd     << std::endl
                  << "events:  " << _fds[i].events << std::endl
                  << "revents: ";
        switch (_fds[i].revents) {
            case POLLIN:     std::cout << "POLLIN";     break;
            case POLLERR:    std::cout << "POLLERR";    break;
            case POLLHUP:    std::cout << "POLLHUP";    break;
            case POLLOUT:    std::cout << "POLLOUT";    break;
            case POLLPRI:    std::cout << "POLLPRI";    break;
            case POLLNVAL:   std::cout << "POLLNVAL";   break;
            case POLLRDBAND: std::cout << "POLLRDBAND"; break;
            case POLLRDNORM: std::cout << "POLLRDNORM"; break;
            case POLLWRBAND: std::cout << "POLLWRBAND"; break;
            default: std::cout << _fds[i].revents;
        }

        std::cout << std::endl << std::endl;
    }
    std::cout << __FILE__ << ":" << __LINE__ << " ---------" << std::endl << std::endl;
 //#endif
//#endif*/
}

// R E A D E R B Y F D F I N D E R   I M P L E M E N T A T I O N

Connection::ReaderByFDFinder::ReaderByFDFinder(int fd) : fd(fd) {}

bool Connection::ReaderByFDFinder::operator()(HTTPReader *  & reader) const {
    return reader->hasFD(fd);
}
