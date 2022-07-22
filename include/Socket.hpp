#pragma once

#include <string>
#include <unistd.h>
#include "IOException.hpp"

#define BUFFER_SIZE 65536

class Socket {
    int    _fd;
    size_t _read_index;
    size_t _buffer_fill;
    char   _buffer[BUFFER_SIZE];

    size_t total_read;

    void read_buffer() throw (IOException);

public:
    Socket();
    Socket(int fd) throw (IOException);
    ~Socket();

    char        read()                            throw (IOException);
    ssize_t     read(char *, size_t)              _NOEXCEPT;
    void        send(const std::string &)         throw (IOException) __attribute__ ((deprecated));
    ssize_t     write(const std::string &)        throw (IOException);
    ssize_t     write(const char *, size_t)       throw (IOException);
    ssize_t     write(char)                       throw (IOException);
    void        close()                     const throw (IOException);
    int         get_fd()                    const _NOEXCEPT;
};