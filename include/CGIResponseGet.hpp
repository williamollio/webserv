//
// Created by Manuel Hahn on 6/10/22.
//

#ifndef WEBSERV_CGIRESPONSEGET_HPP
#define WEBSERV_CGIRESPONSEGET_HPP

#include "CGIResponse.hpp"
#include "CGICallBuiltin.hpp"

class CGIResponseGet : public CGIResponse {
public:
    explicit CGIResponseGet(HTTPRequest*, Socket &, Runnable &);

    void		run();
	std::string set_file(std::string path);
	std::string construct_content_type();
	bool		is_request_location(std::string path);
    bool        runForFD(int i);

private:
    std::string payload;
    unsigned long socketCounter;
    CGICallBuiltin * cgicall;
};


#endif //WEBSERV_CGIRESPONSEGET_HPP
