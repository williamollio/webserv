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

class Connection {
private:
    // First: Port --- Second: server_fd
    std::map<int, int>      server_fds;

    // First: client fd --- Second: server fd
    std::map<int, int>      connectionPairs;
    int                     addrlen;
    int                     on;
    int                     _timeout;
    struct sockaddr_in      address;
    struct pollfd           _fds[200];
    std::list<HTTPReader *> list;

    void          cleanReaders();
    unsigned long clearPollArray(unsigned long nfds);
    void          removeFD(unsigned long index);
    void          handleConnection(unsigned long index) _NOEXCEPT;
    bool          isServingFD(int fd);

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
