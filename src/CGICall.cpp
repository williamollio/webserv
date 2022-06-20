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
    const URI & uri = _request->getURI();
    std::map<std::string, std::string> vars = uri.getVars();
}
