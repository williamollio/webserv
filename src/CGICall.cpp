//
// Created by Manuel Hahn on 6/15/22.
//

#include "CGICall.hpp"

CGICall::CGICall(HTTPRequest & request) : CGIResponse(request) {}

CGICall::~CGICall() {}

void CGICall::run(Socket &socket) {
    // TODO
    std::cout << "CGI to be called!" << std::endl;
}
