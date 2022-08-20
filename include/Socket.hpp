#pragma once

#include <string>
#include <unistd.h>
#include "IOException.hpp"

#define BUFFER_SIZE 65536

class Socket {
    enum State { BAD, EOT, CLOSED, POLL_AGAIN, READY };

    int    _fd;
    size_t _read_index;
    size_t _buffer_fill;
    char   _buffer[BUFFER_SIZE];
    State  _state;

    size_t total_read;
    size_t total_written;

    void read_buffer() throw (IOException);

    Socket();                           // = delete;
    Socket(const Socket &);             // = delete;
    Socket & operator=(const Socket &); // = delete;

public:
    explicit Socket(int fd) throw (IOException);
    ~Socket();

    char        read()                            throw (IOException);
    ssize_t     write(const std::string &)        throw (IOException);
    ssize_t     write(const char *, size_t)       throw (IOException);
    ssize_t     write(char)                       throw (IOException);
    void        close()                           throw (IOException);
    void        move(Socket &, bool = true)       throw (IOException);
    void        move(const Socket &, bool = true) throw (IOException);
    ssize_t     read(char *, size_t)              _NOEXCEPT;
    void        clear_state()                     _NOEXCEPT;
    void        invalidate()                      _NOEXCEPT;
    int         get_fd()                    const _NOEXCEPT;
    bool        bad()                       const _NOEXCEPT;
    bool        eof()                       const _NOEXCEPT;
    bool        closed()                    const _NOEXCEPT;
    bool        ready()                     const _NOEXCEPT;
    State       get_state()                 const _NOEXCEPT;
};
