//
// Created by Manuel Hahn on 6/15/22.
//

#ifndef WEBSERV_CGICALL_HPP
#define WEBSERV_CGICALL_HPP

#include "CGIResponse.hpp"

class CGICall: public CGIResponse {
public:
    explicit CGICall(HTTPRequest *);
    virtual ~CGICall();

    void run(Socket & socket);
    bool isRunning();

protected:
    virtual std::string computeRequestedFile();
    virtual std::string computeScriptDirectory();

private:
    const URI &     uri;
    std::string     method;
    std::string     protocol;
    std::string     pathinfo;
    std::string     contentLength;
    std::string     contentType;
    std::string     gatewayInterface;
    std::string     queryString;
    std::string     scriptName;
    std::string     serverName;
    std::string     serverPort;
    std::string     serverSoftware;
    std::string     remoteAddress;
    std::string     remoteHost;
    std::string     requestUri;
    std::string     httpUserAgent;
    std::string     httpHost;
    std::string     httpLang;
    std::string     httpEncoding;
    std::string     httpAccept;
    std::string     httpConnection;
    std::string     httpContentLength;
    std::string     httpExpect;
    Socket          socket;
    pid_t           child;
    pthread_t       threadID;
    int             in[2];
    int             out[2];
    bool            running;
    pthread_mutex_t runningMutex;

    void execute(int, int, const std::string &);
    void waitOrThrow();
    void sendError(int errorCode) _NOEXCEPT;

    static std::string   nextLine(int);
    static bool          isFolder(const std::string &);
    static HTTPHeader    parseCGIResponse(int);
    static unsigned long skipWhitespaces(const std::string &, unsigned long);
    static void          async(CGICall *);

    /**
     * The time in seconds after which child processes are killed if they did not finish.
     */
    static const unsigned int TIMEOUT = 10000;
};


#endif //WEBSERV_CGICALL_HPP
