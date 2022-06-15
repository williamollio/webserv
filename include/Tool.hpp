#ifndef TOOL_HPP
#define TOOL_HPP

#include <string>

std::string	get_message(const int & error_code);
int			get_code(const std::string & message);

#endif