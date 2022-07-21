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
    unsigned long           nfds;
    struct sockaddr_in      address;
    std::list<HTTPReader *> list;
    struct pollfd           _fds[NUM_FDS];

    void clearPollArray()                        _NOEXCEPT;
    void cleanReaders()                          _NOEXCEPT;
    void denyConnection(int fd, int = 429) const _NOEXCEPT;
    void handleConnection(unsigned long index)   _NOEXCEPT;
    bool isServingFD(int fd)                     _NOEXCEPT;

    static Connection * currentInstance;

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
    void addFD(int fd, bool read = true) _NOEXCEPT;
    void removeFD(unsigned long index)   _NOEXCEPT;

    static Connection & getInstance() _NOEXCEPT;
};
