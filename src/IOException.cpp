//
// Created by Manuel Hahn on 6/10/22.
//

#include "IOException.hpp"
#include <sstream>

#undef IOException

const char * IOException::what() const throw() {
    return _message.c_str();
}

IOException::IOException() _NOEXCEPT: std::exception(), _message(), _line() {}

IOException::IOException(const IOException & other) _NOEXCEPT: std::exception(other), _message(other.what()), _line() {}

IOException::IOException(const std::string & message) _NOEXCEPT: std::exception(), _message(message), _line() {}

IOException::IOException(const std::string & message, const std::string & file, unsigned int line) _NOEXCEPT
    : std::exception(), _message(message), _file(file), _line(line) {
    std::stringstream s;
    s << "IOException: " << message << ". File: " << _file << ":" << _line;
    _message = s.str();
}

IOException::~IOException() _NOEXCEPT {}

unsigned int IOException::get_line_number() const {
    return _line;
}

const std::string & IOException::get_file() const {
    return _file;
}
