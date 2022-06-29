//
// Created by Manuel Hahn on 6/10/22.
//

#ifndef WEBSERV_HTTPREQUEST_HPP
#define WEBSERV_HTTPREQUEST_HPP

#include <string>
#include <vector>
#include <iostream>
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
        GET, POST, DELETE
    };
	enum REQ_INFO {
		USER_AGENT, HOSTNAME, LANG_SUPP, ENCODING, CON_TYPE, CONTENT_TYPE, CON_LENGTH
	};

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

	explicit HTTPRequest(TYPE, std::vector<std::string>& file, std::string& raw, Socket& _socket);
	REQ_INFO http_token_comp(std::string& word);

	size_t	load_string(std::vector<std::string>& file, size_t index, std::string& target);
	size_t	load_vec_str(std::vector<std::string>& file, size_t index, vectorString& target);
	size_t	load_connection(std::vector<std::string>& file, size_t index, bool& target);
	size_t	load_size(std::vector<std::string>& file, size_t index, size_t& target);

	bool	is_payload(vectorString& file, size_t index);
	size_t	ff_newline(std::vector<std::string>& file, size_t index);
	protected:
	explicit HTTPRequest(TYPE);

private:
    const TYPE  _type;
    URI          uri;
    unsigned int peerAddress;
    std::string  peerName;

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
	size_t			_content_length;
	vectorString	_content_type;
	std::string		_payload;
};


#endif //WEBSERV_HTTPREQUEST_HPP

//"GET / HTTP/1.1\r\nHost: localhost\r\nUpgrade-Insecure-Requests: 1\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\nUser-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/15.3 Safari/605.1.15\r\nAccept-Language: en-us\r\nAccept-Encoding: gzip, deflate\r\nConnection: keep-alive\r\n\r\n"

