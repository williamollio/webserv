//
// Created by Manuel Hahn on 6/10/22.
//

#include "../include/CGIResponseDelete.hpp"

void CGIResponseDelete::run(Socket &socket) {
    (void)socket;
}

CGIResponseDelete::CGIResponseDelete(HTTPRequest *request) : CGIResponse(request) {}
