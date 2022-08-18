#ifndef WEBSERV_CGIRESPONSEREDIRECT_HPP
#define WEBSERV_CGIRESPONSEREDIRECT_HPP


#include "CGIResponse.hpp"

class CGIResponseRedirect : public CGIResponse {

	public :
		CGIResponseRedirect(HTTPRequest*, Socket &, Runnable &, const std::string &);
		void run(void);
		bool runForFD(int , bool);

	private :
		std::string _payload;
		size_t _counter;
		const std::string &_redirect;
};

#endif
