//
// Created by Manuel Hahn on 7/7/22.
//

#include "CGICallBuiltin.hpp"

CGICallBuiltin::CGICallBuiltin(const HTTPRequest * request, const std::string & executable)
    : CGICall(request), executable(executable) {

}

const std::string & CGICallBuiltin::getExecutable() const {
    return executable;
}

void CGICallBuiltin::setExecutable(const std::string & executable) {
    CGICallBuiltin::executable = executable;
}

std::string CGICallBuiltin::computeRequestedFile() {
    char * c_pwd = getcwd(NULL, 0);
    const std::string ret = c_pwd + ("/" + executable);
    free(c_pwd);
    return ret;
}
