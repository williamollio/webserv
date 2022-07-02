#pragma once
#include "Socket.hpp"
#include "HTTPReader.hpp"
#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <list>

#define NUM_FDS 200

class Connection {
private:
    // First: Port --- Second: server fd
    std::map<int, int>      server_fds;

    // First: client fd --- Second: server fd
    std::map<int, int>      connectionPairs;
    int                     addrlen;
    int                     on;
    int                     _timeout;
    struct sockaddr_in      address;
    struct pollfd           _fds[NUM_FDS];
    std::list<HTTPReader *> list;

    void          cleanReaders()                                    _NOEXCEPT;
    unsigned long clearPollArray(unsigned long nfds)                _NOEXCEPT;
    void          denyConnection(int fd, unsigned int = 429) const  _NOEXCEPT;
    void          handleConnection(unsigned long index)             _NOEXCEPT;
    bool          isServingFD(int fd)                               _NOEXCEPT;
    void          removeFD(unsigned long index)                     _NOEXCEPT;

    class ReaderByFDFinder {
    public:
        explicit ReaderByFDFinder(int fd);

        bool operator()(HTTPReader* &) const;

    private:
        const int fd;
    };

public:
    Connection();
    ~Connection();

    void establishConnection();
};
