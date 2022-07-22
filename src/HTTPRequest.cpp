//
// Created by Manuel Hahn on 6/10/22.
//

#include "HTTPRequest.hpp"
#include "HTTPException.hpp"
#include <cstdlib>

HTTPRequest::TYPE HTTPRequest::getType() const {
    return _type;
}

bool	HTTPRequest::is_payload(size_t index) {
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
	if (word == "Expect" || word == "expect")
		return EXPECT;
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

size_t	HTTPRequest::ff_newline(std::vector<std::string>& file, size_t index) {
	while (index < file.size() && file.at(index) != "\n" && !is_payload(index))
		index++;
	return index + 1;
}

HTTPRequest::HTTPRequest(HTTPRequest::TYPE type, std::vector<std::string> &file, std::string &raw, Socket &_socket)
		: loaded(true), _type(type), _chunked_socket(_socket), _chunked_head_or_load(false), fast_fowarded(true), _keep_alive(false), _content(false),
		  _content_length(0) {
	size_t	index = 1;
	_copy_raw = raw;
	_path = file[index++];
	if (file[index].compare(0, 5, "HTTP/") == 0 && file[index + 1] == "\n") {
		_http_version = file[index].substr(5);
		index += 2;
	}
	else
		throw HTTPException(400);
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
		set_payload(raw, _socket);
	}
	else
		_content = false;
}

const URI & HTTPRequest::getURI() const {
    return uri;
}

void HTTPRequest::setURI(const URI & uri) {
    HTTPRequest::uri = uri;
}

std::string HTTPRequest::unchunkedPayload(const std::string &data, size_t cursor)
{
	std::string payload;
	std::string buffer;
	std::string line;
	payload = data.substr(0);
	std::istringstream tmp(payload);

	size_t i = 1;
	size_t size = 0;
	do
	{
		getline(tmp, line);
//		if (i % 2 == 1)
//			std::cout << line << std::endl;
		if (i % 2 == 0)
		{
			line.pop_back();
			buffer.append(line);
			std::cout << "length: " << line.size() << std::endl;
			size += line.size();
			line.clear();
		}
		i++;
	} while (!(line.front() == '0' && line.length() == 3) && !tmp.eof());
	std::cout << "size: " << size << std::endl;
	payload.clear();
	return (buffer);
}

void HTTPRequest::isChunkedRequest(const std::string &data)
{
	if (data.find("Transfer-Encoding: chunked") != std::string::npos || data.find("Expect: 100-continue") != std::string::npos)
		_chunked = true;
}

bool HTTPRequest::ff_nextline() {
	char buffer;
	bool boll = false;
    try {
        while ((buffer = _chunked_socket.read()) > 0) {
            if (buffer == '\r')
                boll = true;
            else if (boll && buffer == '\n')
                return true;
            else
                boll = false;
        }
    } catch (IOException & ex) {
        return false;
    }
	/*while (read(_chunked_socket.get_fd(), &buffer, 1) > 0) {
		if (buffer == '\r')
			boll = true;
		else if (boll && buffer == '\n')
			return true;
		else
			boll = false;
	}*/
	return false;
}

void HTTPRequest::loadPayload() {
	loaded = false;

	//char buff[2] = {'\0', '\0'};

		if (!_chunked_head_or_load) {
			if (!fast_fowarded && !ff_nextline())
				return;
			fast_fowarded = true;
			while (raw_expect.find("\r\n") == std::string::npos) {
                try {
                    raw_expect += _chunked_socket.read();
                } catch (IOException & ex) {
                    return;
                }
				//if (read(_chunked_socket.get_fd(), buff, 1) < 0)
				//	return;
				//raw_expect += buff;
			}
			_chunked_head_or_load = true;
			_chunked_curr_line_read_count = 0;
			raw_expect.erase(raw_expect.find("\r\n"), 2);
			std::stringstream ss;
			ss << std::hex << raw_expect;
			ss >> _chunked_curr_line_expect_count;
			raw_expect.clear();
			if (_chunked_curr_line_expect_count < 0) {
				throw HTTPException(400);
			} else if (_chunked_curr_line_expect_count == 0) {
				loaded = true;
				std::cout << "finished" << std::endl;
				while (raw_read.find("\r\n") != std::string::npos)
					raw_read.erase(raw_read.find("\r\n"), 2);
				_payload = raw_read;
				return;
			}
		}
		long long tmp = 0;
		char *heap_buffer = new char[_chunked_curr_line_expect_count + 1]();
		while (raw_read.find("\r\n") == std::string::npos && _chunked_curr_line_expect_count > _chunked_curr_line_read_count) {
			tmp = _chunked_socket.read(heap_buffer, _chunked_curr_line_expect_count - _chunked_curr_line_read_count);
			if (tmp <= 0)
				break;
			_chunked_curr_line_read_count += tmp;
			raw_read += heap_buffer;
		}
		if (_chunked_curr_line_expect_count <= _chunked_curr_line_read_count)
			_chunked_head_or_load = false;
		delete[] heap_buffer;
		if (raw_read.find("\r\n") == std::string::npos && !ff_nextline()) {
			fast_fowarded = false;
			return;
		} else if (raw_read.find("\r\n") != std::string::npos)
			raw_read.erase(raw_read.find("\r\n"), 2);
	///DEPRECATED
//	loaded = false;
//	char buff[2] = {'\0', '\0'};
//	std::string raw(buff);
//	std::string payload_num;
//	size_t		size = 0;
//	long long	char_count = 0;
//	long long	ret = 0;
//	bool		failed = false;
//	while (raw.find("\r\n\r\n", 0) == std::string::npos) {
//		payload_num.clear();
//		while (payload_num.find("\r\n\r\n", 0) == std::string::npos && payload_num.find("\r\n", size) == std::string::npos) {
//			if (!read(_chunked_socket.get_fd(), buff, 1))
//				throw HTTPException(504);
//			payload_num += buff;
//		}
//		failed = false;
//		std::stringstream ss;
//		ss << std::hex << payload_num;
//		ss >> char_count;
//		if (payload_num.find("\r\n\r\n", 0) != std::string::npos || char_count <= 0){
//			loaded = true;
//			break;
//		}
//		{
//			char * ex_buff = new char[char_count + 1]();
//			std::cout << char_count << std::endl;
//			ret = read(_chunked_socket.get_fd(), ex_buff, char_count);
//			raw += ex_buff;
//			if (ret < 0) {
//				delete[] ex_buff;
//				loaded = false;
//				return;
////				throw HTTPException(504);
//			}
//			if (raw.find("\r\n\r\n", 0) != std::string::npos) {
//				loaded = true;
//				break;
//			}
//			long long tmp = 0;
//			while (char_count - ret > 0 && ret < char_count && raw.find("\r\n\r\n", 0) == std::string::npos) {
//				tmp = read(_chunked_socket.get_fd(), ex_buff, char_count - ret);
//				if (tmp < 0 && !failed) {
//					failed = true;
//					usleep(100);
//					continue;
//				} else if (tmp < 0 && failed) {
//					loaded = false;
//					return;
//				}
//				ret += tmp;
//				raw += ex_buff;
//			}
//			std::cout << ret << std::endl;
//			if (char_count - ret < 0)
//				std::cout << "really??" << std::endl;
//			delete[] ex_buff;
//		}
//	}
//	std::cout << "rawsize: " << raw.size() << std::endl;
//	{
//		while (raw.find("\r\n") != std::string::npos) {
//			raw.erase(raw.find("\r\n"), 2);
//		}
//	}
//	_payload += raw;
//	std::cout << "size: " << _payload.size() << std::endl;
}

void HTTPRequest::set_payload(const std::string& data, Socket& _socket) throw(std::exception) {
	size_t	cursor = data.find("\r\n\r\n", 0);
	if (cursor == std::string::npos) {
        std::cerr << "error 4000 here" <<std::endl;
		throw HTTPException(400);
    }

	if (!_chunked) {
		char buf[2];
		while (_payload.length() < _content_length) {
			if (!read(_socket.get_fd(), buf, 1))
				throw HTTPException(504);
			_payload += buf;
		}
	} else {
		_payload.clear();
		_chunked_socket = _socket;
		loadPayload();
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

int HTTPRequest::getUsedPort() const {
    return port;
}

void HTTPRequest::setUsedPort(const int port) {
    HTTPRequest::port = port;
}
