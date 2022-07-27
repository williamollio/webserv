//
// Created by Manuel Hahn on 7/7/22.
//

#include "CGICallBuiltin.hpp"

CGICallBuiltin::CGICallBuiltin(HTTPRequest * request, Socket & socket, const std::string & executable)
    : CGICall(request, socket), executable(executable) {

}

const std::string & CGICallBuiltin::getExecutable() const {
    return executable;
}

void CGICallBuiltin::setExecutable(const std::string & executable) {
    CGICallBuiltin::executable = executable;
}

std::string CGICallBuiltin::computeRequestedFile() {
    char * c_pwd = getcwd(NULL, 0);
    const std::string ret = c_pwd + Configuration::getInstance().get_server_root_folder() + ("/" + executable);
    free(c_pwd);
    return ret;
}

void CGICallBuiltin::waitForExecution() {
    while (isRunning());
}

void CGICallBuiltin::run(Socket & socket) {
    CGICall::run(socket);
    waitForExecution();
}
