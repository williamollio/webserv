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
    IOException(const std::string &, const std::string &, unsigned int) _NOEXCEPT;
    explicit IOException(const std::string &) _NOEXCEPT;
    ~IOException() _NOEXCEPT;

    unsigned int get_line_number() const;
    const std::string & get_file() const;

    const char *what() const _NOEXCEPT;

private:
    std::string  _message;
    std::string  _file;
    unsigned int _line;
};

#define IOException(string) IOException(string, __FILE__, __LINE__)

#endif //WEBSERV_IOEXCEPTION_HPP
