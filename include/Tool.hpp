#ifndef TOOL_HPP
#define TOOL_HPP

#include <string>

#ifdef DEBUG
 #if DEBUG == 1
  #define debug(message) (std::cout << __FILE__ << ": " << message << std::endl)
 #elif DEBUG == 2
  #define debug(message) (std::cout << __FILE__ << ":" << __LINE__ << " " << message << std::endl)
 #endif
#else
 #define debug(message) ((void) 0)
#endif

#define PRINT_ERROR_CODE(message, code) (std::cout << __FILE__ << ":" << __LINE__ << " " <<  "\033[1;31m" << message << code << "\033[0m" << std::endl)
#define PRINT_CGIRESPONSE(message, variable) (std::cout << __FILE__ << ":" << __LINE__ << " " <<  "\033[1;33m" << message << variable << "\033[0m" << std::endl)
#define PRINT_CGIRESPONSEGET(message, variable) (std::cout << __FILE__ << ":" << __LINE__ << " " <<  "\033[1;35m" << message << variable << "\033[0m" << std::endl)
#define PRINT_CGIRESPONSEPOST(message, variable) (std::cout << __FILE__ << ":" << __LINE__ << " " <<  "\033[1;37m" << message << variable << "\033[0m" << std::endl)
#define PRINT_CGIRESPONSEERROR(message, variable) (std::cout << __FILE__ << ":" << __LINE__ << " " <<  "\033[1;39m" << message << variable << "\033[0m" << std::endl)

std::string	get_message(int error_code);
std::string int_to_ipv4(unsigned int address);
std::string int_to_string(int nbr);

#endif