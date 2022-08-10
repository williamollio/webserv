
#ifndef WEBSERV_CGIRESPONSEERROR_HPP
#define WEBSERV_CGIRESPONSEERROR_HPP

#include "CGIResponse.hpp"

class CGIResponseError : public CGIResponse {
	public:
		explicit CGIResponseError(Socket &);

		void set_error_code(int error_code);
        void set_head_only(bool);
		void setBody(int error_code, std::string &body);
		void run(Socket & socket);

    bool runForFD(int i);
    bool isRunning();

private:
		int  _error_code;
        bool _head;
        bool _running;
        std::string _payload;
        unsigned long _payloadCounter;
};


#endif //WEBSERV_CGIRESPONSEERROR_HPP
