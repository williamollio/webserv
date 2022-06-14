//
// Created by Manuel Hahn on 6/10/22.
//

#ifndef WEBSERV_IOEXCEPTION_HPP
#define WEBSERV_IOEXCEPTION_HPP


#include <exception>
#include <string>

class IOException: public std::exception {
public:
    IOException() _NOEXCEPT;
    IOException(const IOException &) _NOEXCEPT;
    explicit IOException(const std::string &) _NOEXCEPT;
    ~IOException() _NOEXCEPT;

    const char *what() const _NOEXCEPT;

private:
    const std::string _message;
};


#endif //WEBSERV_IOEXCEPTION_HPP
