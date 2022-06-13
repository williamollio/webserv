//
// Created by Manuel Hahn on 6/10/22.
//

#ifndef WEBSERV_HTTPREQUEST_HPP
#define WEBSERV_HTTPREQUEST_HPP

#include <string>
#include <vector>

class HTTPRequest {
private:
	typedef std::vector<std::string>	vectorString;
public:
    enum TYPE {
        GET, POST, DELETE
    };

    TYPE getType() const;
	void get_payload(const std::string& data) throw(std::exception);
protected:
    explicit HTTPRequest(TYPE);

private:
    const TYPE		_type;
public:	//TODO: make private with get and set
	std::string		_http_version;
	std::string		_path;
	std::string		_user_agent;
	std::string		_host;
	vectorString	_lang;
	vectorString	_encoding;
	bool			_keep_alive;
	bool			_content;
	size_t			_content_length;
	std::string		_content_type;
	std::string		_payload;
};


#endif //WEBSERV_HTTPREQUEST_HPP
