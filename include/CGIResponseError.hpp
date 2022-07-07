
#ifndef WEBSERV_CGIRESPONSEERROR_HPP
#define WEBSERV_CGIRESPONSEERROR_HPP

#include "CGIResponse.hpp"

class CGIResponseError : public CGIResponse {
	public:
		CGIResponseError();

		void set_error_code(int error_code);
		void setBody(int error_code, std::string &body);
		void run(Socket & socket);

	private:
		int _error_code;
};


#endif //WEBSERV_CGIRESPONSEDELETE_HPP
