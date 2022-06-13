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
	return ret.str().c_str();
}


