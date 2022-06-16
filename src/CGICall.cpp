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
        std::map<std::string, std::string> vars = URI(
                "/folder/file.php/search?q=test&test=true&test").getVars(); // = _request.getURI().getVars();
    } catch (URISyntaxException & ex) {
        std::cerr << ex.what() << std::endl;
        throw HTTPException(400);
    }
}
