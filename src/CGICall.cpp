//
// Created by Manuel Hahn on 6/15/22.
//

#include <map>
#include "CGICall.hpp"
#include "URISyntaxException.hpp"
#include "HTTPException.hpp"

CGICall::CGICall(HTTPRequest * request) : CGIResponse(request) {}

CGICall::~CGICall() {}

void CGICall::run(Socket & socket) {
    // TODO
    try {
        std::map<std::string, std::string> vars = _request->getURI().getVars();
    } catch (URISyntaxException & ex) {
        // TODO maybe just ignore...
        std::cerr << ex.what() << std::endl;
        throw HTTPException(400);
    }
}
