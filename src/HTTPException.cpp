//
// Created by Jascha Kasper on 2022-06-12.
//

#include "HTTPException.hpp"
#include <cstdlib>
#include <sstream>

HTTPException::HTTPException(int code) _NOEXCEPT {
	_code = code;
}

const char *HTTPException::what() const _NOEXCEPT {
	std::stringstream ret;
	ret << _code;
	std::string tmp_str = ret.str();
	const char *tmp = tmp_str.c_str();
	return tmp;
}

int HTTPException::get_error_code() {
	return (_code);
}



