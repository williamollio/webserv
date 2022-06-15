//
// Created by Jascha Kasper on 2022-06-12.
//

#ifndef WEBSERV_HTTPEXCEPTION_HPP
#define WEBSERV_HTTPEXCEPTION_HPP

#include <exception>
#include <string>

class HTTPException : public std::exception {
private:
	int	_code;
public:
	HTTPException(int code) _NOEXCEPT;
	~HTTPException() _NOEXCEPT {};
	const char *what() const _NOEXCEPT;

	int get_error_code();
};


#endif //WEBSERV_HTTPEXCEPTION_HPP
