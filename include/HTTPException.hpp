//
// Created by Jascha Kasper on 2022-06-12.
//

#ifndef WEBSERV_HTTPEXCEPTION_HPP
#define WEBSERV_HTTPEXCEPTION_HPP

#include <exception>
#include <string>

class HTTPException : std::exception {
public:
	static const char *what(const std::string& code) throw();
};


#endif //WEBSERV_HTTPEXCEPTION_HPP
