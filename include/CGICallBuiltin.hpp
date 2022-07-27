//
// Created by Manuel Hahn on 7/7/22.
//

#ifndef WEBSERV_CGICALLBUILTIN_H
#define WEBSERV_CGICALLBUILTIN_H


#include "CGICall.hpp"

class CGICallBuiltin: public CGICall {
    std::string executable;

protected:
    std::string computeRequestedFile();

public:
    explicit CGICallBuiltin(HTTPRequest *, Socket &, const std::string & executable = "");

    const std::string & getExecutable() const;
    void                setExecutable(const std::string &);

    void                run(Socket &);

    /**
     * Waits for the execution to finish. Blocks the current thread until the underlying CGICall has
     * terminated.
     */
    void waitForExecution();
};


#endif //WEBSERV_CGICALLBUILTIN_H
