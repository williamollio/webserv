#include <iostream>
#include "Socket.hpp"

Socket::~Socket() {
    std::cerr << "SOCKET: fd: " << _fd << ", total bytes: " << total_read << std::endl;
}

Socket::Socket(int fd) throw (IOException)
    : _fd(fd), _read_index(0), _buffer_fill(), _buffer(), _state(1), total_read(0)
{
	if (fd < 0)
		throw IOException("Invalid socket descriptor!");
}

ssize_t Socket::write(char data) throw(IOException) {
    ssize_t ret;
    if ((ret = ::write(_fd, &data, 1)) < 0) {
        throw IOException("Could not write the data!");
    }
    return ret;
}

ssize_t Socket::write(const char * buffer, size_t size) throw(IOException) {
    ssize_t ret;
    if ((ret = ::write(_fd, buffer, size)) < 0) {
        throw IOException("Could not write the data!");
    }
    return ret;
}

ssize_t Socket::write(const std::string & data) throw(IOException) {
    return write(data.c_str(), data.size());
}

void Socket::read_buffer() throw(IOException) {
    _read_index = 0;
    ssize_t tmp = ::read(_fd, _buffer, BUFFER_SIZE);
    std::cerr << "SOCKET: " << _fd << " read: " << tmp << std::endl;
    if (tmp < 0) {
        _state = -1;
        throw IOException("Could not read any data!");
    } else if (tmp == 0) {
        _state = 0;
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
    } catch (IOException & ex) {
        ret = ret == 0 ? -1 : ret;
    }
    return ret;
}

bool Socket::bad() const _NOEXCEPT {
    return _state == -1;
}

bool Socket::eof() const _NOEXCEPT {
    return _state == 0;
}

void Socket::send(const std::string & data) throw(IOException) {
    std::cerr << "Socket: send(data) -> " << data << std::endl;
    (void) write(data);
}

void Socket::close() const throw (IOException)
{
	if (::close(_fd) < 0)
        throw IOException("Could not close socket!");
}

int Socket::get_fd() const _NOEXCEPT { return _fd; }
