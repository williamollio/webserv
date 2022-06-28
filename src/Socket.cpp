#include "Socket.hpp"

Socket::Socket() {}

Socket::~Socket() {}

Socket::Socket(int fd) throw (IOException) : _fd(fd)
{
	if (fd < 0)
		throw IOException("Invalid socket descriptor!");
}

std::string Socket::read_socket() throw (IOException)
{
	char*	read_buffer = new char[30001];
	u_long	amount = 0;
	amount = read(_fd, read_buffer, 30000);
	if (amount < 0)
		throw IOException("Could not read from the socket!");
	std::string tmp = std::string(read_buffer);
	delete[] read_buffer;
	return(tmp);
}

void Socket::send_header(const std::string & type)
{
	_type_header = type;
}

void Socket::send(const std::string & content) throw (IOException)
{
	if (write(_fd, content.c_str(), content.size()) < 0)
		throw IOException("Could not send the data! Data: " + content);
}

void Socket::close_socket() const throw (IOException)
{
	if (close(_fd) < 0)
        throw IOException("Could not close socket!");
}

int Socket::get_fd() const _NOEXCEPT { return _fd; }