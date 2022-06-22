//
// Created by Manuel Hahn on 6/8/22.
//

#ifndef WEBSERV_HTTPREADER_HPP
#define WEBSERV_HTTPREADER_HPP


#include "Socket.hpp"
#include "HTTPRequest.hpp"
#include "CGIResponse.hpp"

class HTTPReader {
    public:
        HTTPReader();
        explicit HTTPReader(Socket &);
        ~HTTPReader();

        void run();
        bool isRunning() const;

    private:
        Socket        _socket;
        CGIResponse * response;

        HTTPRequest * _parse() throw(std::exception);
};


#endif //WEBSERV_HTTPREADER_HPP
