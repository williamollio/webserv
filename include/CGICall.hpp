//
// Created by Manuel Hahn on 6/15/22.
//

#ifndef WEBSERV_CGICALL_HPP
#define WEBSERV_CGICALL_HPP

#include "CGIResponse.hpp"

class CGICall: public CGIResponse {
public:
    explicit CGICall(HTTPRequest *, Socket &, Runnable &);
    virtual ~CGICall();

    void run();
    bool runForFD(int);

protected:
    virtual std::string computeRequestedFile();
    virtual std::string computeScriptDirectory();

private:
    const URI &                  uri;
    std::string                  method;
    std::string                  protocol;
    std::string                  pathinfo;
    std::string                  contentLength;
    std::string                  contentType;
    std::string                  gatewayInterface;
    std::string                  queryString;
    std::string                  scriptName;
    std::string                  serverName;
    std::string                  serverPort;
    std::string                  serverSoftware;
    std::string                  remoteAddress;
    std::string                  remoteHost;
    std::string                  requestUri;
    std::string                  httpUserAgent;
    std::string                  httpHost;
    std::string                  httpLang;
    std::string                  httpEncoding;
    std::string                  httpAccept;
    std::string                  httpConnection;
    std::string                  httpContentLength;
    std::string                  httpExpect;
    std::string                  buffer;
    std::string                  payload;
	std::vector<std::string>     x_arguments_name;
	std::vector<std::string>     x_arguments;
    Socket &                     socket;
    pid_t                        child;
    pthread_t                    threadID;
    int                          in[2];
    int                          out[2];
    size_t                       payloadCounter;
    size_t                       socketCounter;

    void execute(int, int, const std::string &);
    void sendError(int errorCode) _NOEXCEPT;
    void processCGIOutput();
    bool writePayload();
    bool writeSocket();
    bool readPayload();

    static std::list<int> pipeFds;

    static int           close(int);
    static std::string   vectorToString(const std::vector<std::string> &);
    static std::string   nextLine(int);
    static bool          isFolder(const std::string &);
    //static HTTPHeader    parseCGIResponse(int);
    static HTTPHeader    parseCGIResponse(std::stringstream &);
    static unsigned long skipWhitespaces(const std::string &, unsigned long);
    static void          waitOrThrow(CGICall *);
    //static void          async(CGICall *);

    /**
     * The time in seconds after which child processes are killed if they did not finish.
     */
    static const unsigned int TIMEOUT = 10000;
};


#endif //WEBSERV_CGICALL_HPP
