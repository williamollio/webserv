
#ifndef WEBSERV_CGIRESPONSEERROR_HPP
#define WEBSERV_CGIRESPONSEERROR_HPP

#include "CGIResponse.hpp"

class CGIResponseError : public CGIResponse {
	public:
		CGIResponseError();

		void set_error_code(int error_code);
        void set_head_only(bool);
		void setBody(int error_code, std::string &body);
		void run(Socket & socket);

	private:
		int  _error_code;
        bool _head;
};


#endif //WEBSERV_CGIRESPONSEERROR_HPP
