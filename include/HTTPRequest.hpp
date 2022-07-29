//
// Created by Manuel Hahn on 6/10/22.
//

#ifndef WEBSERV_HTTPREQUEST_HPP
#define WEBSERV_HTTPREQUEST_HPP

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include "Socket.hpp"
#include "URI.hpp"
#include "Cookie.hpp"

class HTTPRequest {
public:
    enum TYPE {
        GET, POST, DELETE, HEAD, ERROR
    };

    explicit HTTPRequest(HTTPRequest::TYPE type, std::vector<std::string> &file, std::string &raw, Socket &_socket);

    TYPE                             getType()          const;
    int                              getUsedPort()      const;
    bool                             isLoaded()         const;
    bool                             hasContent()       const;
    bool                             isKeepAlive()      const;
    unsigned int                     getPeerAddress()   const;
    size_t                           getContentLength() const;
    const URI &                      getURI()           const;
    const std::string &              get_payload()      const;
    const std::string &              getPeerName()      const;
    const std::string &              getUserAgent()     const;
    const std::string &              getHost()          const;
    const std::string &              getExpect()        const;
    const std::vector<std::string> & getLang()          const;
    const std::vector<std::string> & getEncoding()      const;
    const std::vector<std::string> & getContentType()   const;
    const std::vector<std::string> & getXArgs()         const;
    const std::vector<std::string> & getXArgsName()     const;
    std::string &                    getPath();
    void                             setURI(const URI & uri);
    void                             setPeerAddress(unsigned int);
    void                             setPeerName(const std::string &);
    void                             setUsedPort(int);
    void                             loadPayload();
    static int                       checktype(std::string& word);

private:
    enum REQ_INFO {
        USER_AGENT, HOSTNAME, LANG_SUPP, ENCODING, CON_TYPE, CONTENT_TYPE, CON_LENGTH, EXPECT, X_ARG, COOKIE, DEFAULT
    };
    typedef std::vector<std::string>	vectorString;

    const TYPE   _type;
    int           port;
    bool		  loaded;
    unsigned int  peerAddress;
    bool         _chunked_head_or_load;
    bool          wasFullLine;
    bool         _keep_alive;
    bool         _content;
    bool         _chunked;
    long         _chunked_curr_line_expect_count;
    size_t       _content_length;
    URI           uri;
    Socket&      _chunked_socket;
    Cookie       _cookie;
    std::string   raw_read;
    std::string   peerName;
    std::string   raw_expect;
    std::string   line;
    std::string	 _copy_raw;
    std::string  _http_version;
    std::string  _path;
    std::string  _user_agent;
    std::string  _host;
    std::string  _payload;
    std::string  _expect;
	vectorString _x_arguments;
	vectorString _x_arguments_name;
    vectorString _content_type;
    vectorString _lang;
    vectorString _encoding;
    vectorString _cookie_vector;


    void            loadChunkedPayload();
    void            loadNormalPayload();
    void            isChunkedRequest(const std::string & data);
    void            set_cookie(Cookie &cookie);
    bool            readLine();
    bool            is_payload(size_t index)                                                                      const;
    size_t          ff_newline(std::vector<std::string>& file, size_t index)                                      const;
    Cookie          parse_cookie();
    Cookie&         get_cookie();
    static size_t   load_string(std::vector<std::string>& file, size_t index, std::string& target);
    static size_t   load_vec_str(std::vector<std::string>& file, size_t index, std::vector<std::string>& target);
    static size_t   load_connection(std::vector<std::string>& file, size_t index, bool& target);
    static size_t   load_size(std::vector<std::string>& file, size_t index, size_t& target);
	static REQ_INFO http_token_comp(std::string& word);
};

#endif //WEBSERV_HTTPREQUEST_HPP
