//
// Created by Manuel Hahn on 6/8/22.
//

#ifndef WEBSERV_HTTPREADER_HPP
#define WEBSERV_HTTPREADER_HPP


#include "Socket.hpp"
#include "HTTPRequest.hpp"
#include "CGIResponse.hpp"
#include "Cookie.hpp"
#include "Runnable.hpp"
#include <list>

class HTTPReader: public Runnable {
public:
    explicit HTTPReader(int);
    HTTPReader(const HTTPReader &);
    ~HTTPReader();

    bool                runForFD(int, bool);
    void                setPeerAddress(unsigned int);
    void                setPeerName(const std::string &);
    void                setUsedPort(int port);
    void                setMarked(bool);
    Cookie              get_cookie(Cookie cookie);
    std::string         isRedirect();
    Socket &            getSocket();
    bool                isMarked()                       const;
    int                 getUsedPort()                    const;
    unsigned int        getPeerAddress()                 const;
    HTTPRequest *       getRequest()                     const;
    const std::string & getPeerName()                    const;
    const Socket &      getSocket()                      const;

private:
    Socket        _socket;
    CGIResponse * response;
    HTTPRequest * request;
    unsigned int  peerAddress;
    std::string   peerName;
    int           port;
    bool          errorHead;
    bool          mark;
    Cookie        cookie;
    std::string   head;

    static std::list<Cookie> session_management;

    HTTPRequest * _parse() throw(std::exception);
    bool          _isCGIMethod(HTTPRequest::TYPE);
};


#endif //WEBSERV_HTTPREADER_HPP
