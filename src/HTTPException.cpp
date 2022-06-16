//
// Created by Jascha Kasper on 2022-06-12.
//

#include "HTTPException.hpp"
#include <cstdlib>
#include <sstream>

HTTPException::HTTPException(int code) _NOEXCEPT {
	_code = code;
}

int HTTPException::get_error_code() {
	return (_code);
}



