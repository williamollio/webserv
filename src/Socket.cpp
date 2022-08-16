#include <iostream>
#include "Socket.hpp"
#include "Tool.hpp"

Socket::~Socket() {
    if (!closed()) {
        try {
            close();
        } catch (IOException & exception) {
            std::cerr << "Socket: " << exception.what() << std::endl;
        }
    }
    debug("fd: " << _fd << ", total bytes read: " << total_read << ", total bytes written: " << total_written);
}

Socket::Socket(int fd) throw (IOException)
    : _fd(fd), _read_index(0), _buffer_fill(), _buffer(), _state(READY), total_read(0), total_written(0) {
	if (fd < 0)
		throw IOException("Invalid socket descriptor!");
}

ssize_t Socket::write(char data) throw(IOException) {
    if (_state == CLOSED) throw IOException("Socket has been closed!");
    ssize_t ret;
    if ((ret = ::write(_fd, &data, 1)) < 0) {
        throw IOException("Could not write the data!");
    }
    ++total_written;
    return ret;
}

ssize_t Socket::write(const char * buffer, size_t size) throw(IOException) {
    if (_state == CLOSED) throw IOException("Socket has been closed!");
    ssize_t ret;
    if ((ret = ::write(_fd, buffer, size)) < 0) {
        throw IOException("Could not write the data!");
    }
    total_written += ret;
    return ret;
}

ssize_t Socket::write(const std::string & data) throw(IOException) {
    if (_state == CLOSED) throw IOException("Socket has been closed!");
    return write(data.c_str(), data.size());
}

void Socket::read_buffer() throw(IOException) {
    _read_index = 0;
    ssize_t tmp = ::read(_fd, _buffer, BUFFER_SIZE);
    if (tmp < 0) {
        _state = BAD;
        _buffer_fill = 0;
        throw IOException("Could not read any data!");
    } else if (tmp == 0) {
        _state = EOT;
    } else if (_state != READY) {
        _state = READY;
    }
    total_read += tmp;
    _buffer_fill = tmp;
}

char Socket::read() throw(IOException) {
    if (_state == CLOSED) throw IOException("Socket has been closed!");
    if (_read_index == _buffer_fill) {
        read_buffer();
    }
    return _buffer_fill == 0 ? static_cast<char>(0) : _buffer[_read_index++];
}

ssize_t Socket::read(char * buffer, size_t size) _NOEXCEPT {
    ssize_t ret = 0;
    try {
        char c;
        for (size_t i = 0; i < size && (c = read()) > 0; ++i, ++ret) {
            buffer[i] = c;
        }
    } catch (IOException &) {
        ret = ret == 0 ? -1 : ret;
    }
    return ret;
}

void Socket::move(Socket & other, bool shouldClose) throw(IOException) {
    if (shouldClose) {
        close();
    }
    _fd = other._fd;
    _read_index = other._read_index;
    _buffer_fill = other._buffer_fill;
    _state = other._state;
    total_read = other.total_read;
    total_written = other.total_written;
    for (unsigned long i = 0; i < BUFFER_SIZE; ++i) {
        _buffer[i] = other._buffer[i];
    }
    other._state = CLOSED;
}

bool Socket::bad() const _NOEXCEPT {
    return _state == BAD;
}

bool Socket::eof() const _NOEXCEPT {
    return _state == EOT;
}

bool Socket::closed() const _NOEXCEPT {
    return _state == CLOSED;
}

bool Socket::ready() const _NOEXCEPT {
    return _state == READY;
}

Socket::State Socket::get_state() const _NOEXCEPT {
    return _state;
}

void Socket::close() throw (IOException) {
    if (_state == CLOSED) {
        throw IOException("Socket has already been closed!");
    } else if (::close(_fd) < 0) {
        throw IOException("Could not close socket!");
    }
    debug("Closing socket fd " << _fd);
    _state = CLOSED;
}

int Socket::get_fd() const _NOEXCEPT { return _fd; }
