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

        void                run();
        bool                runForFD(int);
        void                setPeerAddress(unsigned int);
        void                setPeerName(const std::string &);
        void                setSocket(const Socket &);
        void                setUsedPort(int port);
        bool                hasFD(int)                       const;
        bool                isRunning()                      const;
        unsigned int        getPeerAddress()                 const;
        const std::string & getPeerName()                    const;
        const Socket &      getSocket()                      const;
		HTTPRequest *       getRequest()                     const;
        int                 getUsedPort()                    const;

private:
        Socket        _socket;
        CGIResponse * response;
        HTTPRequest * request;
        unsigned int  peerAddress;
        std::string   peerName;
        int           port;

        HTTPRequest * _parse() throw(std::exception);
        bool          _isCGIMethod(HTTPRequest::TYPE);
};


#endif //WEBSERV_HTTPREADER_HPP
