//
// Created by Manuel Hahn on 6/8/22.
//

#ifndef WEBSERV_HTTPREADER_HPP
#define WEBSERV_HTTPREADER_HPP


#include "Socket.hpp"
#include "HTTPRequest.hpp"
#include "CGIResponse.hpp"
#include "Cookie.hpp"
#include <list>

class HTTPReader {
    public:
        HTTPReader();
        explicit HTTPReader(Socket &);
        ~HTTPReader();

        void run();
        bool isRunning() const;

        unsigned int getPeerAddress() const;
        void         setPeerAddress(unsigned int peerAddress);

        const std::string & getPeerName() const;
        void                setPeerName(const std::string &peerName);

        const Socket & getSocket() const;
        void           setSocket(const Socket &);

        int  getUsedPort() const;
        void setUsedPort(int port);
		Cookie get_cookie(Cookie cookie);

private:
        Socket        _socket;
        CGIResponse * response;
        HTTPRequest * request;
        unsigned int  peerAddress;
        std::string   peerName;
        int           port;
		Cookie        cookie;
		static std::list<Cookie> session_management;

        HTTPRequest * _parse() throw(std::exception);
        bool          _isCGIMethod(HTTPRequest::TYPE);
};


#endif //WEBSERV_HTTPREADER_HPP
