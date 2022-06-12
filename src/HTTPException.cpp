//
// Created by Jascha Kasper on 2022-06-12.
//

#include "HTTPException.hpp"
#include <cstdlib>


const char *HTTPException::what(const std::string& code) throw() {
	std::string ret("Error: " + code);
	return ret.c_str();
}


