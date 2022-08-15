//
// Created by Manuel Hahn on 8/15/22.
//

#ifndef WEBSERV_RUNNABLE_HPP
#define WEBSERV_RUNNABLE_HPP

class Runnable {
public:
    virtual bool runForFD(int fd) = 0;
};

#endif //WEBSERV_RUNNABLE_HPP
