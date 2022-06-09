#include "Socket.hpp"

Socket::Socket() {}

Socket::~Socket() {}

Socket::Socket(int fd) : _fd(fd)
{
	if (fd < 0)
		throw std::exception();
	fcntl(fd, F_SETFL, O_NONBLOCK);
}

std::string Socket::read_socket()
{
	char*	read_buffer = new char[30001];
	u_long	amount = 0;
	amount = read(_fd, read_buffer, 30000);
	if (amount < 0)
		throw std::exception();
	std::string tmp = std::string(read_buffer);
	delete[] read_buffer;
	return(tmp);
}

void Socket::send_header(const std::string & type)
{
	_type_header = type;
}

void Socket::send_file(const std::string & name)
{
	_filename = name;
	std::ifstream html_file(_filename);
	std::stringstream	str_stream;
	str_stream << html_file.rdbuf();
	std::string content("HTTP/1.1 200 OK\nContent-Type: text/");
    content += _type_header + "\nContent-Length: ";
	content += std::to_string(str_stream.str().size());
	content += "\n\n";
	content += str_stream.str();
    std::cout << content;
	this->send(content);
}

void Socket::send(const std::string & content)
{
	if (write(_fd, content.data(), content.size()) < 0)
		throw std::exception();
}

void Socket::close_socket() const
{
	if (close(_fd) < 0)
        throw std::exception();
}