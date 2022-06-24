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

        unsigned int getPeerAddress() const;
        void         setPeerAddress(unsigned int peerAddress);

        const std::string & getPeerName() const;
        void                setPeerName(const std::string &peerName);

        const Socket & getSocket() const;
        void           setSocket(const Socket &);

private:
        Socket        _socket;
        CGIResponse * response;
        unsigned int  peerAddress;
        std::string   peerName;

        HTTPRequest * _parse() throw(std::exception);
};


#endif //WEBSERV_HTTPREADER_HPP
