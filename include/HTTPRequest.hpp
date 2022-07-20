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

#ifndef BUFFER
# define BUFFER 30000
#endif /* BUFFER */

class HTTPRequest {
private:
	typedef std::vector<std::string>	vectorString;
public:
    enum TYPE {
        GET, POST, DELETE, ERROR
    };
	enum REQ_INFO {
		USER_AGENT, HOSTNAME, LANG_SUPP, ENCODING, CON_TYPE, CONTENT_TYPE, CON_LENGTH, EXPECT ,DEFAULT
	};

	bool			loaded;
	static int		checktype(std::string& word);

    TYPE                getType() const;
    void                set_payload(const std::string& data, Socket& _socket) throw(std::exception);
    const std::string & get_payload() const;

    const URI &         getURI() const;
    void                setURI(const URI &uri);

    unsigned int        getPeerAddress() const;
    void                setPeerAddress(unsigned int peerAddress);

    const std::string &  getPeerName() const;
    void                 setPeerName(const std::string & peerName);

    int                  getUsedPort() const;
    void                 setUsedPort(int);

	std::string          unchunkedPayload(const std::string &data, size_t cursor);
	void                 isChunkedRequest(const std::string &data);
	void				 loadPayload();

	explicit HTTPRequest(HTTPRequest::TYPE type, std::vector<std::string> &file, std::string &raw, Socket &_socket);
	REQ_INFO http_token_comp(std::string& word);

	size_t	load_string(std::vector<std::string>& file, size_t index, std::string& target);
	size_t	load_vec_str(std::vector<std::string>& file, size_t index, vectorString& target);
	size_t	load_connection(std::vector<std::string>& file, size_t index, bool& target);
	size_t	load_size(std::vector<std::string>& file, size_t index, size_t& target);

	bool	is_payload(size_t index);
	size_t	ff_newline(std::vector<std::string>& file, size_t index);
	protected:
	explicit HTTPRequest(TYPE);
	bool ff_nextline();

private:
    const TYPE  _type;
    URI          uri;
    unsigned int peerAddress;
    std::string  peerName;
    int          port;
	Socket&      _chunked_socket;
	bool         _chunked_head_or_load;
	long long    _chunked_curr_line_expect_count;
	std::string  raw_expect;
	long long    _chunked_curr_line_read_count;
	std::string  raw_read;
	bool         fast_fowarded;

public:	//TODO: make private with get and set
	std::string		_copy_raw;
    std::string		_http_version;
    std::string		_path;
    std::string		_user_agent;
    std::string		_host;
    vectorString	_lang;
    vectorString	_encoding;
    bool			_keep_alive;
    bool			_content;
    bool			_chunked;
	size_t			_content_length;
	vectorString	_content_type;
	std::string		_payload;
	std::string		_expect;

};


#endif //WEBSERV_HTTPREQUEST_HPP

//"GET / HTTP/1.1\r\nHost: localhost\r\nUpgrade-Insecure-Requests: 1\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\nUser-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/15.3 Safari/605.1.15\r\nAccept-Language: en-us\r\nAccept-Encoding: gzip, deflate\r\nConnection: keep-alive\r\n\r\n"

