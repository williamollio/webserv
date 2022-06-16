//
// Created by Manuel Hahn on 6/15/22.
//

#include <map>
#include "CGICall.hpp"

CGICall::CGICall(HTTPRequest & request) : CGIResponse(request) {}

CGICall::~CGICall() {}

void CGICall::run(Socket & socket) {
    // TODO
    std::map<std::string, std::string> vars = URI("/folder/file.php/search?q=test&test=true&test=").getVars(); // = _request.getURI().getVars();
    for (std::map<std::string, std::string>::const_iterator it = vars.cbegin(); it != vars.cend(); ++it) {
        std::cerr << it->first << " : " << it->second << std::endl;
    }
    std::cout << "CGI to be called!" << std::endl;
}
