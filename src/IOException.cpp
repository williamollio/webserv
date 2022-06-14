//
// Created by Manuel Hahn on 6/10/22.
//

#include "../include/IOException.hpp"

const char *IOException::what() const throw() {
    return _message.c_str();
}

IOException::IOException() _NOEXCEPT: std::exception(), _message() {}

IOException::IOException(const IOException &other) _NOEXCEPT: std::exception(other), _message(other.what()) {}

IOException::IOException(const std::string &message) _NOEXCEPT: std::exception(), _message(message) {}

IOException::~IOException() _NOEXCEPT {}
