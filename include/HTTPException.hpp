//
// Created by Jascha Kasper on 2022-06-12.
//

#ifndef WEBSERV_HTTPEXCEPTION_HPP
#define WEBSERV_HTTPEXCEPTION_HPP

#include <exception>
#include <string>

class HTTPException : public std::exception {
private:
	int                 _code;
    unsigned int        _line;
    std::string         _file;
    std::string         _what;

public:
	HTTPException(int, const std::string &, unsigned int) _NOEXCEPT;
    explicit HTTPException(int)                           _NOEXCEPT;
    HTTPException(const HTTPException &)                  _NOEXCEPT;
	~HTTPException()                                      _NOEXCEPT;

	int                 get_error_code()  const;
    unsigned int        get_line_number() const;
    const std::string & get_file()        const;

    const char * what() const _NOEXCEPT;
};

#define HTTPException(code) (HTTPException(code, __FILE__, __LINE__))

#endif //WEBSERV_HTTPEXCEPTION_HPP
