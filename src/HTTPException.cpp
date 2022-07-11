//
// Created by Jascha Kasper on 2022-06-12.
//

#include "HTTPException.hpp"
#include <cstdlib>
#include <sstream>

#undef HTTPException

HTTPException::HTTPException(int code, const std::string & file, unsigned int line) _NOEXCEPT
    : _line(line), _file(file) {
	_code = code;
    std::stringstream s;
    s << "HTTPError: code = " << _code << " at " << file << ":" << _line;
    _what = s.str();
}

int HTTPException::get_error_code() const {
	return (_code);
}

unsigned int HTTPException::get_line_number() const {
    return _line;
}

const std::string & HTTPException::get_file() const {
    return _file;
}

const char * HTTPException::what() const _NOEXCEPT {
    return _what.c_str();
}



