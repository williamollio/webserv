#include "Socket.hpp"

Socket::Socket() {}

Socket::~Socket() {}

Socket::Socket(int fd) throw (IOException)
    : _fd(fd), _read_index(0), _buffer()
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
    if (tmp < 0) {
        throw IOException("Could not read any data!");
    }
    _buffer_fill = tmp;
}

char Socket::read() throw(IOException) {
    if (_read_index == _buffer_fill) {
        read_buffer();
    }
    return _buffer_fill == 0 ? 0 : _buffer[_read_index++];
}

/*std::string Socket::read() throw (IOException)
{
	char*	read_buffer = new char[30001];
	u_long	amount = 0;
	amount = read(_fd, read_buffer, 30000);
	if (amount < 0)
		throw IOException("Could not read from the socket!");
	std::string tmp = std::string(read_buffer);
	delete[] read_buffer;
	return(tmp);
}*/

/*void Socket::send_header(const std::string & type)
{
	_type_header = type;
}*/

/*void Socket::send(const std::string & content) throw (IOException)
{
	#if DEBUG
		std::cout << "\n\n"<< std::endl;
	#endif
    for (size_t i = 0; i < content.length(); ++i) {
        if (write(_fd, content.c_str() + i, 1) < 0)
            throw (IOException("Could not send the data! Data: " + content));
    }
}*/

void Socket::close() const throw (IOException)
{
	if (::close(_fd) < 0)
        throw IOException("Could not close socket!");
}

int Socket::get_fd() const _NOEXCEPT { return _fd; }
