#ifndef TOOL_HPP
#define TOOL_HPP

#include <string>

#define PRINT_ERROR_CODE(message, code) (std::cout << __FILE__ << ":" << __LINE__ << " " <<  "\033[1;31m" << message << code << "\033[0m" << std::endl)

std::string	get_message(int error_code);
std::string int_to_ipv4(unsigned int address);

#endif