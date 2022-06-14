//
// Created by Manuel Hahn on 6/10/22.
//

#include "HTTPRequest.hpp"

HTTPRequest::TYPE HTTPRequest::getType() const {
    return _type;
}

HTTPRequest::HTTPRequest(HTTPRequest::TYPE type): _type(type) {}
