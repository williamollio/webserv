//
// Created by Manuel Hahn on 7/7/22.
//

#include "CGICallBuiltin.hpp"

CGICallBuiltin::CGICallBuiltin(HTTPRequest * request, const std::string & executable)
    : CGICall(request), executable(executable) {

}

const std::string & CGICallBuiltin::getExecutable() const {
    return executable;
}

void CGICallBuiltin::setExecutable(const std::string & executable) {
    CGICallBuiltin::executable = executable;
}

std::string CGICallBuiltin::computeRequestedFile() {
    return executable;
}

std::string CGICallBuiltin::computeScriptDirectory() {
    std::string ret;
    for (std::string::const_reverse_iterator it = executable.rbegin(); it != executable.rend() && *it != '/'; ++it) {
        ret += *it;
    }
    return ret;
}
