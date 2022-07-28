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
    ssize_t ret;
    if ((ret = ::write(_fd, &data, 1)) < 0) {
        throw IOException("Could not write the data!");
    }
    ++total_written;
    return ret;
}

ssize_t Socket::write(const char * buffer, size_t size) throw(IOException) {
    ssize_t ret;
    if ((ret = ::write(_fd, buffer, size)) < 0) {
        throw IOException("Could not write the data!");
    }
    total_written += ret;
    return ret;
}

ssize_t Socket::write(const std::string & data) throw(IOException) {
    return write(data.c_str(), data.size());
}

void Socket::read_buffer() throw(IOException) {
    if (_state == CLOSED) throw IOException("Socket has been closed!");

    _read_index = 0;
    ssize_t tmp = ::read(_fd, _buffer, BUFFER_SIZE);
    debug(_fd << " read: " << tmp);
    if (tmp < 0) {
        _state = BAD;
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
    _state = CLOSED;
}

int Socket::get_fd() const _NOEXCEPT { return _fd; }
