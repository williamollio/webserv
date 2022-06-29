//
// Created by Manuel Hahn on 6/10/22.
//

#include "HTTPRequest.hpp"
#include "HTTPException.hpp"
#include <cstdlib>


HTTPRequest::TYPE HTTPRequest::getType() const {
    return _type;
}

HTTPRequest::HTTPRequest(HTTPRequest::TYPE type): _type(type), _keep_alive(false), _content(false), _content_length(0) {}

bool	HTTPRequest::is_payload(std::vector<std::string>& file, size_t index) {
	return 	_copy_raw.find("\r\n\r\n", 0) <= index;
}

HTTPRequest::REQ_INFO HTTPRequest::http_token_comp(std::string &word) {
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
}

size_t	HTTPRequest::load_string(std::vector<std::string>& file, size_t index, std::string& target) {
	index += 2;
	while (file[index] != "\n" && file[index] != ";")
		target += file[index++];
	while (file[index] != "\n")
		index++;
	return index + 1;
}

size_t HTTPRequest::load_vec_str(std::vector<std::string> &file, size_t index, vectorString &target) {
	index += 2;
	while (file[index] != "\n" && file[index] != ";") {
		if (file[index] == ",")
			index++;
		else
			target.push_back(file[index++]);
	}
	while (file[index] != "\n")
		index++;
	return index + 1;
}

size_t	HTTPRequest::load_connection(std::vector<std::string> &file, size_t index, bool &target) {
	index += 2;
	for (int i = 0; file[index].c_str()[i] != '\0'; i++)
		std:: cout << static_cast<int>(file[index].c_str()[i]) << std::endl;
	if (!file[index].compare(0, 10, "keep-alive") || !file[index].compare(0, 10, "Keep-Alive")) {
		target = true;
	}
	else
		target = false;
	return index + 2;
}

size_t	HTTPRequest::load_size(std::vector<std::string> &file, size_t index, size_t &target) {
	index += 2;
	target = strtol(file[index].c_str(), NULL, 0);
	return index + 2;
}

size_t	HTTPRequest::ff_newline(std::vector<std::string>& file, size_t index) {
	while (index < file.size() && file[index] != "\n" && !is_payload(file, index))
		index++;
	return index += 1;
}

HTTPRequest::HTTPRequest(HTTPRequest::TYPE type, std::vector<std::string> &file, std::string& raw, Socket& _socket) : _type(type), _keep_alive(false), _content(false), _content_length(0) {
	size_t	index = 1;
	_copy_raw = raw;
	_path = file[index++];
	if (file[index].compare(0, 5, "HTTP/") == 0 && file[index + 1] == "\n") {
		_http_version = file[index].substr(5);
		index += 2;
	}
	else
		throw HTTPException(400);
	while (file.size() > index + 2 && !is_payload(file, index)) {
		switch(http_token_comp(file[index])) {
			case USER_AGENT:
				index = load_string(file, index, _user_agent);
				break;
			case HOSTNAME:
				index = load_string(file, index, _host);
				break;
			case LANG_SUPP:
				index = load_vec_str(file, index, _lang);
				break;
			case ENCODING:
				index = load_vec_str(file, index, _encoding);
				break;
			case CON_TYPE:
				index = load_connection(file, index, _keep_alive);
				break;
			case CON_LENGTH:
				index = load_size(file, index, _content_length);
				break;
			case CONTENT_TYPE:
				index = load_vec_str(file, index, _content_type);
				break;
			default:
				index = ff_newline(file, index);
		}
	}

	if (_content_length != 0 && _content_type.empty()) {

		throw HTTPException(400);
    }
	else if (_content_length != 0  || retval->isChunkedRequest(raw)) {
		_content = true;
		set_payload(raw, _socket);
	}
	else
		_content = false;
}

const URI &HTTPRequest::getURI() const {
    return uri;
}

void HTTPRequest::setURI(const URI &uri) {
    HTTPRequest::uri = uri;
}

void HTTPRequest::set_payload(const std::string& data, Socket& _socket) throw(std::exception) {
	size_t	cursor = data.find("\r\n\r\n", 0);
	if (cursor == std::string::npos) {
        std::cerr << "error 4000 here" <<std::endl;
		throw HTTPException(400);
    }
	cursor += 2;
	if (isChunkedRequest(data)) {
    _payload = unchunkedPayload(data, cursor);
    return;
  }
	else
		_payload = data.substr(cursor);
	if (data.length() < _content_length && _content_length - (BUFFER - cursor) >= 0) {
		char buf[_content_length - (BUFFER - cursor) + 1];
		if (!read(_socket.get_fd(), buf, _content_length - (BUFFER - cursor)))
			throw HTTPException(504);
		_payload += buf;
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

const std::string &HTTPRequest::getPeerName() const {
    return peerName;
}

void HTTPRequest::setPeerName(const std::string &peerName) {
    HTTPRequest::peerName = peerName;
}
