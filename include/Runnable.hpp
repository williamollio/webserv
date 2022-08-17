//
// Created by Manuel Hahn on 8/15/22.
//

#ifndef WEBSERV_RUNNABLE_HPP
#define WEBSERV_RUNNABLE_HPP

class Runnable {
public:
    virtual bool runForFD(int fd, bool hup) = 0;
    virtual void setMarked(bool) = 0;
	virtual ~Runnable() {}
};

#endif //WEBSERV_RUNNABLE_HPP
