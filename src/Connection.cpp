#include <fcntl.h>
#include <netinet/in.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <csignal>

#include "Connection.hpp"
#include "Configuration.hpp"
#include "IOException.hpp"

static void stopper(int);

Connection::Connection()
    : _timeout(-1), _nfds(0), _fds() {
    bzero(_fds, sizeof(_fds));
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    const std::vector<int> & ports = Configuration::getInstance().get_server_ports();
    for (unsigned long i = 0; i < ports.size(); ++i) {
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(ports[i]);

        int server_fd;
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            throw IOException("Could not create socket!");
        }
        int on = 1;
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        fcntl(server_fd, F_SETFL, O_NONBLOCK);
        if (bind(server_fd, reinterpret_cast<struct sockaddr *>(&address), sizeof(address)) < 0) {
            throw IOException("Could not bind file descriptor to the address!");
        }
        if (listen(server_fd, 10) < 0) {
            throw IOException("Cannot listen on the socket descriptor!");
        }
    }
    signal(SIGINT, stopper);
    signal(SIGTERM, stopper);
    signal(SIGKILL, stopper);
}

Connection::~Connection() {}

volatile bool end_server = false;

static void stopper(int) {
    end_server = true;
}

void Connection::_clear_poll_array() {
    unsigned long i, j;
    for (i = 0, j = 0; i < _nfds; ++i) {
        if (_fds[i].fd != -1) {
            _fds[j++] = _fds[i];
        }
    }
    _nfds = j;
}

bool Connection::_is_serving_fd(int fd) {
    return _server_fds.find(fd) != _server_fds.end();
}

void Connection::establishConnection() {
    int rc;

    _nfds = _server_fds.size();
    while (!end_server && (rc = poll(_fds, _nfds, _timeout)) > 0) {
        nfds_t currentSize = _nfds;
        for (nfds_t i = 0; i < currentSize; ++i) {
            if (_fds[i].revents == 0) {
                // Ignore...
            } else if (_is_serving_fd(_fds[i].fd)) {
                accept(i);
            } else {
                handle(i);
            }
        }
        _clear_poll_array();
    }
}

void Connection::accept(nfds_t i) {
    // TODO: Accept incoming connections.
}

void Connection::handle(nfds_t i) {
    // TODO: Handle the file descriptor.
}
