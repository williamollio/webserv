//
// Created by Manuel Hahn on 6/8/22.
//

#ifndef WEBSERV_HTTPREADER_HPP
#define WEBSERV_HTTPREADER_HPP


#include "Socket.hpp"

class HTTPReader {
    public:
        HTTPReader();
        HTTPReader(Socket &);
        ~HTTPReader();
        void run();

    private:
        Socket _socket;
};


#endif //WEBSERV_HTTPREADER_HPP
