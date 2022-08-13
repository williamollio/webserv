#pragma once
#include "HTTPReader.hpp"
#include <sys/socket.h>
#include <list>
#include <map>
#include <netinet/in.h>
#include <sys/poll.h>

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
    nfds_t                  nfds;
    struct sockaddr_in      address;
    std::list<HTTPReader *> list;
    struct pollfd           _fds[NUM_FDS];

    void clearPollArray()                                       _NOEXCEPT;
    void cleanReaders()                                         _NOEXCEPT;
    void denyConnection(int fd, HTTPReader * = NULL, int = 429) _NOEXCEPT;
    void handleConnection(unsigned long index)                  _NOEXCEPT;
    bool isServingFD(int fd)                                    _NOEXCEPT;

    static Connection * currentInstance;

    class ReaderByFDFinder {
    public:
        explicit ReaderByFDFinder(int fd);

        bool operator()(HTTPReader* &) const;

    private:
        const int fd;
    };

public:
    void printPollArray()                                       _NOEXCEPT;
    Connection();
    ~Connection();

    void establishConnection();
    bool addFD(int fd, bool read = true) _NOEXCEPT;
    bool removeFD(int)                   _NOEXCEPT;

    static Connection & getInstance() _NOEXCEPT;
};
