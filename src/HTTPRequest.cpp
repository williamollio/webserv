//
// Created by Manuel Hahn on 6/10/22.
//

#include "HTTPRequest.hpp"
#include "HTTPException.hpp"
#include "Tool.hpp"
#include <cstdlib>

HTTPRequest::TYPE HTTPRequest::getType() const {
    return _type;
}

bool HTTPRequest::is_payload(size_t index) const {
	return _copy_raw.find("\r\n\r\n", 0) <= index;
}

HTTPRequest::REQ_INFO HTTPRequest::http_token_comp(std::string &word) {
	if (!word.compare(0, 2, "X-") || !word.compare(0, 2, "x-")) {
//		for (size_t i = 2; word.size() > i; i++) {
//			word.at(i) = toupper(word.at(i));
//			if (word.at(i) == '-')
//				word.at(i) = '_';
//		}
		return X_ARG;
	}
	if (word == "user-agent" || word == "User-Agent")
		return USER_AGENT;
	if (word == "host" || word == "Host")
		return HOSTNAME;
	if (word == "Accept-Language" || word == "accept-language")
		return LANG_SUPP;
	if (word == "Accept-Encoding" || word == "accept-encoding")
		return ENCODING;
	if (word == "Accept" || word == "accept")
		return CONTENT_TYPE;
	if (word == "Content-Length" || word == "content-length")
		return CON_LENGTH;
	if (word == "Connection" || word == "connection")
		return CON_TYPE;
	if (word == "Expect" || word == "expect")
		return EXPECT;
	if (word == "Cookie" || word == "cookie")
		return COOKIE;
	return DEFAULT;
}

size_t	HTTPRequest::load_string(std::vector<std::string>& file, size_t index, std::string& target) {
	index += 2;
	while (index < file.size() && file.at(index) != "\n" && file.at(index) != ";")
		target += file.at(index++);
	while (index < file.size() && file.at(index) != "\n")
		index++;
	return index + 1;
}

size_t HTTPRequest::load_vec_str(std::vector<std::string> &file, size_t index, vectorString &target) {
	index += 2;
	while (index < file.size() && file.at(index) != "\n" && file.at(index) != ";") {
		if (file.at(index) == ",")
			index++;
		else
			target.push_back(file.at(index++));
	}
	while (index < file.size() && file.at(index) != "\n")
		index++;
	return index + 1;
}

size_t	HTTPRequest::load_connection(std::vector<std::string> &file, size_t index, bool &target) {
	index += 2;
	if (!file.at(index).compare(0, 10, "keep-alive") || !file.at(index).compare(0, 10, "Keep-Alive")) {
		target = true;
	}
	else
		target = false;
	return index + 2;
}

size_t	HTTPRequest::load_size(std::vector<std::string> &file, size_t index, size_t &target) {
	index += 2;
	target = strtol(file.at(index).c_str(), NULL, 0);
	return index + 2;
}

size_t	HTTPRequest::ff_newline(std::vector<std::string>& file, size_t index) const {
	while (index < file.size() && file.at(index) != "\n" && !is_payload(index))
		index++;
	return index + 1;
}

HTTPRequest::HTTPRequest(HTTPRequest::TYPE type, std::vector<std::string> &file, std::string &raw, Socket &_socket)
    : _type(type), port(), loaded(true), peerAddress(), _chunked_head_or_load(false), wasFullLine(false), _keep_alive(false),
        _content(false), _chunked(), _chunked_curr_line_expect_count(), _content_length(0), _chunked_socket(_socket) {
	size_t	index = 1;
	_copy_raw = raw;
	_path = file[index++];
	if (file[index].compare(0, 5, "HTTP/") == 0 && file[index + 1] == "\n") {
		_http_version = file[index].substr(5);
		index += 2;
	}
	else
		throw HTTPException(400);
	std::string tmp_str;
	while (file.size() > index + 2 && !is_payload(index)) {
		switch(http_token_comp(file[index])) {
			case USER_AGENT:
//				std::cerr << "passed USERAGENT" << std::endl;
				index = load_string(file, index, _user_agent);
//				std::cerr << "passed USERAGENT" << std::endl;
				break;
			case HOSTNAME:
//				std::cerr << "passed HOSTNAME" << std::endl;
				index = load_string(file, index, _host);
//				std::cerr << "passed HOSTNAME" << std::endl;
				break;
			case LANG_SUPP:
//				std::cerr << "passed LANGUAGE" << std::endl;
				index = load_vec_str(file, index, _lang);
//				std::cerr << "passed LANGUAGE" << std::endl;
				break;
			case ENCODING:
//				std::cerr << "passed ENCODING" << std::endl;
				index = load_vec_str(file, index, _encoding);
//				std::cerr << "passed ENCODING" << std::endl;
				break;
			case CON_TYPE:
//				std::cerr << "passed CONNECTION_TYPE" << std::endl;
				index = load_connection(file, index, _keep_alive);
//				std::cerr << "passed CONNECTION_TYPE" << std::endl;
				break;
			case CON_LENGTH:
//				std::cerr << "passed CONTENT_LENGTH" << std::endl;
				index = load_size(file, index, _content_length);
//				std::cerr << "passed CONTENT_LENGTH" << std::endl;
				break;
			case CONTENT_TYPE:
//				std::cerr << "passed CONTENT_TYPE" << std::endl;
				index = load_vec_str(file, index, _content_type);
//				std::cerr << "passed CONTENT_TYPE" << std::endl;
				break;
			case EXPECT :
				index = load_string(file, index, _expect);
				break;
			case X_ARG:
				_x_arguments_name.push_back(file[index]);
				index = load_string(file, index, tmp_str);
				_x_arguments.push_back(tmp_str);
				tmp_str.clear();
				break;
			case COOKIE :
				index = load_vec_str(file, index, _cookie_vector);
				break;
			default:
				index = ff_newline(file, index);
		}
	}

	isChunkedRequest(raw);
	if (_content_length != 0 && _content_type.empty()) {
        throw HTTPException(400);
    }
	else if (_content_length != 0 || _chunked) {
		_content = true;
        loadPayload();
	}
	else
		_content = false;
}
void HTTPRequest::set_cookie(Cookie &cookie) {
	_cookie = cookie;
}

Cookie& HTTPRequest::get_cookie() {
	return (_cookie);
}

Cookie HTTPRequest::parse_cookie() {
	Cookie cookie;

	for (vectorString::iterator it = _cookie_vector.begin(); it != _cookie_vector.end(); ++it) {
		size_t pos = it->find('=');
		if (pos == std::string::npos) continue;
		std::string name = it->substr(0, pos);
		std::string value = it->substr(pos + 1);
		if (value.back() == '\r')
			value.pop_back();
		cookie.set_identifier(name, value);
	}
	return cookie;
}

const URI & HTTPRequest::getURI() const {
    return uri;
}

void HTTPRequest::setURI(const URI & uri) {
    HTTPRequest::uri = uri;
}

void HTTPRequest::isChunkedRequest(const std::string &data)
{
	if (data.find("Transfer-Encoding: chunked") != std::string::npos || data.find("Expect: 100-continue") != std::string::npos)
		_chunked = true;
}

bool HTTPRequest::readLine() {
    std::string tmp;
    char c;
    try {
        while ((c = _chunked_socket.read()) > 0 && c != '\n') {
            tmp += c;
        }
        if (c == 0) throw std::exception(); // TODO: Handle HUP more gracefully
        if (tmp.back() == '\r') {
            tmp.erase(tmp.end() - 1);
        }
        if (wasFullLine) {
            line = tmp;
        } else {
            line += tmp;
        }
        wasFullLine = true;
        return true;
    } catch (IOException &) {
        if (wasFullLine) {
            line = tmp;
        } else {
            line += tmp;
        }
        wasFullLine = false;
        return false;
    }
}

void HTTPRequest::loadPayload() {
    debug("Chunked payload: " << (_chunked ? "true" : "false"));
    if (_chunked) {
        loadChunkedPayload();
    } else {
        loadNormalPayload();
    }
}

void HTTPRequest::loadNormalPayload() {
    while (_payload.size() < _content_length) {
        if (!readLine()) return;
        _payload += line;
    }
}

void HTTPRequest::loadChunkedPayload() {
    loaded = false;

    while (!loaded) {
		// if (max size) return error;
        if (!readLine()) return; // Poll again...
        if (!_chunked_head_or_load) {
            // Chunk size
            std::stringstream s(line);
            s << std::hex;
            s >> _chunked_curr_line_expect_count;
            _chunked_head_or_load = true;
        } else {
            // Body
            if (_chunked_curr_line_expect_count < 0) {
                throw HTTPException(400);
            } else if (_chunked_curr_line_expect_count == 0) {
                loaded = true;
                debug("Finished loading payload");
                return; // For never coming back again...
            } else {
                _payload.reserve(_payload.size() + _chunked_curr_line_expect_count);
                _payload.append(line.c_str(), _chunked_curr_line_expect_count);
                _chunked_head_or_load = false;
            }
        }
        debug("Payload size: " << _payload.size());
    }
}

const std::string & HTTPRequest::get_payload() const {
    return _payload;
}

unsigned int HTTPRequest::getPeerAddress() const {
    return peerAddress;
}

void HTTPRequest::setPeerAddress(unsigned int peerAddress) {
    HTTPRequest::peerAddress = peerAddress;
}

const std::string & HTTPRequest::getPeerName() const {
    return peerName;
}

void HTTPRequest::setPeerName(const std::string & peerName) {
    HTTPRequest::peerName = peerName;
}

int HTTPRequest::getUsedPort() const {
    return port;
}

void HTTPRequest::setUsedPort(const int port) {
    HTTPRequest::port = port;
}

std::string & HTTPRequest::getPath() {
    return _path;
}

bool HTTPRequest::isLoaded() const {
    return loaded;
}

const std::string & HTTPRequest::getUserAgent() const {
    return _user_agent;
}

const std::string & HTTPRequest::getHost() const {
    return _host;
}

const HTTPRequest::vectorString & HTTPRequest::getLang() const {
    return _lang;
}

const HTTPRequest::vectorString & HTTPRequest::getEncoding() const {
    return _encoding;
}

bool HTTPRequest::hasContent() const {
    return _content;
}

const HTTPRequest::vectorString & HTTPRequest::getContentType() const {
    return _content_type;
}

const std::string & HTTPRequest::getExpect() const {
    return _expect;
}

bool HTTPRequest::isKeepAlive() const {
    return _keep_alive;
}

size_t HTTPRequest::getContentLength() const {
    return _content_length;
}

const std::vector<std::string> & HTTPRequest::getXArgs() const {
    return _x_arguments;
}

const std::vector<std::string> & HTTPRequest::getXArgsName() const {
    return _x_arguments_name;
}
