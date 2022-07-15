//
// Created by Manuel Hahn on 16.06.22.
//

#ifndef WEBSERV_URISYNTAXEXCEPTION_HPP
#define WEBSERV_URISYNTAXEXCEPTION_HPP

#include <exception>
#include "URI.hpp"

class URISyntaxException: public std::exception {
public:
    URISyntaxException() _NOEXCEPT;
    explicit URISyntaxException(const URI::Token &) _NOEXCEPT;
    URISyntaxException(const URI::Token &, const std::string &) _NOEXCEPT;
    URISyntaxException(const URISyntaxException &) _NOEXCEPT;
    ~URISyntaxException() _NOEXCEPT;

    const char *       what() const _NOEXCEPT;
    const URI::Token & getToken() const;

private:
    const URI::Token  token;
    const std::string message;
};


#endif //WEBSERV_URISYNTAXEXCEPTION_HPP
