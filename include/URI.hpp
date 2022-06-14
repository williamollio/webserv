//
// Created by Manuel Hahn on 6/14/22.
//

#ifndef WEBSERV_URI_HPP
#define WEBSERV_URI_HPP

#include <string>

class URI {
public:
    URI(const std::string &);
    ~URI();

    bool isCGIIdentifier() const;

private:
    const std::string original;
};


#endif //WEBSERV_URI_HPP
