#ifndef TOOL_HPP
#define TOOL_HPP

#include <string>
#include "CGIResponse.hpp"

#define PRINT_ERROR_CODE(message, code) (std::cout << __FILE__ << ":" << __LINE__ << " " <<  "\033[1;31m" << message << code << "\033[0m" << std::endl)
#define PRINT_CGIRESPONSE(message, variable) (std::cout << __FILE__ << ":" << __LINE__ << " " <<  "\033[1;33m" << message << variable << "\033[0m" << std::endl)
#define PRINT_CGIRESPONSEGET(message, variable) (std::cout << __FILE__ << ":" << __LINE__ << " " <<  "\033[1;35m" << message << variable << "\033[0m" << std::endl)
#define PRINT_CGIRESPONSEPOST(message, variable) (std::cout << __FILE__ << ":" << __LINE__ << " " <<  "\033[1;37m" << message << variable << "\033[0m" << std::endl)
#define PRINT_CGIRESPONSEERROR(message, variable) (std::cout << __FILE__ << ":" << __LINE__ << " " <<  "\033[1;39m" << message << variable << "\033[0m" << std::endl)

std::string	get_message(int error_code);
std::string int_to_ipv4(unsigned int address);
std::string int_to_string(int nbr);

#endif