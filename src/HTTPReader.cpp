//
// Created by Manuel Hahn on 6/8/22.
//

#include "HTTPReader.hpp"
#include "CGIResponseGet.hpp"
#include "CGIResponsePost.hpp"
#include "CGIResponseDelete.hpp"
#include "CGIResponseError.hpp"
#include "CGIResponseRedirect.hpp"
#include "HTTPException.hpp"
#include "URI.hpp"
#include "CGICall.hpp"
#include "HTTPRequest.hpp"

#include <iostream>

std::list<Cookie> HTTPReader::session_management;

HTTPReader::HTTPReader(int fd): _socket(fd), response(NULL), request(NULL), errorHead(false) {}

HTTPReader::~HTTPReader() {
    if (response != NULL) delete response;
    if (request != NULL) delete request;
}

std::string HTTPReader::isRedirect() {

	Configuration &config = Configuration::getInstance();
	const std::vector<Configuration::loc_inf> &server_location_info = config.get_location_specifier();
	URI uri(request->getPath());
	for (std::vector<Configuration::loc_inf>::const_iterator it = server_location_info.begin(); it != server_location_info.end(); it++)
	{
        if (it->directory.size() > uri.getOriginal().size()) continue;
        const std::string & tmp = uri.getOriginal().substr(it->directory.size());
		if (uri.startsWith((*it).directory) && (tmp.empty() || tmp.front() == '/') && !it->redirect.empty())
		{
            request->getPath() = tmp;
			return ((*it).redirect);
		}
	}
	return ("");
}

bool HTTPReader::runForFD(int, bool hup) {
    bool ret = false;
    if (hup) {
        _socket.close();
        return true;
    }
    _socket.clear_state();
    try {
        if (request == NULL) {
            request = _parse();
        } else {
            request->loadPayload();
        }
        if (request != NULL && request->isLoaded()) {
            debug("Loaded, size " << request->get_payload().size() << " bytes");
            Cookie cookie = get_cookie(request->parse_cookie());
            request->set_cookie(cookie);
            request->setURI(URI(request->getPath()));
            request->setPeerAddress(peerAddress);
            request->setPeerName(peerName);
            request->setUsedPort(port);
			std::string redirect = isRedirect();
			if (!redirect.empty())
				response = new CGIResponseRedirect(request, _socket, *this, redirect);
            else if (request->getURI().isCGIIdentifier() && _isCGIMethod(request->getType())) {
                response = new CGICall(request, _socket, *this);
                ret = true;
            } else {
                switch (request->getType()) {
                    case HTTPRequest::GET:    response = new CGIResponseGet(request, _socket, *this);    break;
                    case HTTPRequest::POST:   response = new CGIResponsePost(request, _socket, *this);   break;
                    case HTTPRequest::DELETE: response = new CGIResponseDelete(request, _socket, *this); break;
                    default:
                        throw HTTPException(400);
                }
            }
            response->run();
        }
    }
    catch (HTTPException & ex) {
        ret = false;
        debug(ex.what());
        std::cout << ex.what() << std::endl;
        if (response != NULL) delete response;
        CGIResponseError * error = new CGIResponseError(_socket, *this);
        response = error;
        error->set_error_code(ex.get_error_code());
        error->set_head_only(errorHead);
        error->run();
    }
    return ret;
}

Cookie HTTPReader::get_cookie(Cookie cookie) {
	std::list<Cookie>::iterator it;

	it = std::find(session_management.begin(), session_management.end(), cookie);
	if (it == session_management.end()) {
		cookie.set_identifier("sessionID", Cookie::generate_session_id());
		cookie.set_user_agent(request->getUserAgent());
		session_management.push_back(cookie);
		return cookie;
	}
	return *it;
}

std::vector<std::string>	split_str_vector(const std::string& tosplit, const std::string& needle) {
	size_t						cursor	= 0;
	std::vector<std::string>	str;
	size_t						pos		= tosplit.find(needle.c_str(), cursor, needle.length());
	while (pos != std::string::npos) {
		str.push_back(tosplit.substr(cursor, pos - cursor));
		cursor = pos + needle.length();
		pos = tosplit.find(needle.c_str(), cursor, needle.length());
	}
	str.push_back(tosplit.substr(cursor));
	return str;
}

static bool catch_special(std::string& line, size_t li) {
	switch (line[li]) {
		case '\n':
			return false;
		case ':':
			return false;
		case '#':
			return false;
		case ';':
			return false;
		case '{':
			return false;
		case '}':
			return false;
		case ',':
			return false;
		default:
			return true;
	}
}

static std::vector<std::string>	split_line(std::string& line) {
	size_t i = 0;
	size_t li = 0;
	size_t fl = line.find('\n', 0);
	int		br = 0;
	std::vector<std::string>	ret;
	while (li != line.length()) {
		i = li;
		if (i < fl) {
			while (i != line.length() && (line[i] == ' ' || line[i] == '\t')) { i++; }
			li = i;
			while (li != line.length() && line[li] != ' ' && line[li] != '\t' && catch_special(line, li)) {
				li++;
			}
			if (li != i)
				ret.push_back(line.substr(i, li - i));
			if (!catch_special(line, li)) {
				ret.push_back(line.substr(li, 1));
				li++;
			}
		} else  {
			while (line[i] == ' ') { i++; }
			while (li != line.length() && (br != 0 || catch_special(line, li))) {
				if (line[li] == '(')
					br++;
				if (line[li] == ')' && br > 0)
					br--;
				li++;
			}
			if (li != i)
				ret.push_back(line.substr(i, li - i));
			if (!catch_special(line, li)) {
				ret.push_back(line.substr(li, 1));
				li++;
			}
		}
	}
	return ret;
}

int	HTTPRequest::checktype(std::string& word) {
	if (word == "GET")
		return GET;
	if (word == "POST" || word == "PUT")
		return POST;
	if (word == "DELETE")
		return DELETE;
    else if (word == "HEAD") {
        return HEAD;
    }
	return ERROR;
}


HTTPRequest* HTTPReader::_parse() throw(std::exception) {
    try {
        while (head.find("\r\n\r\n") == std::string::npos) {
            head += _socket.read();
        }
    } catch (IOException &) {
        return NULL;
    }
	std::vector<std::string> file = split_line(head);
	switch(HTTPRequest::checktype(file[0])) {
		case HTTPRequest::GET:
			return new HTTPRequest(HTTPRequest::GET, file, head, _socket);
		case HTTPRequest::POST:
			return new HTTPRequest(HTTPRequest::POST, file, head, _socket);
		case HTTPRequest::DELETE:
			return new HTTPRequest(HTTPRequest::DELETE, file, head, _socket);
        case HTTPRequest::HEAD:
            errorHead = true;
        default:
			std::cerr << file[0] << std::endl;
			throw HTTPException(405);
	}

}

unsigned int HTTPReader::getPeerAddress() const {
    return peerAddress;
}

void HTTPReader::setPeerAddress(unsigned int peerAddress) {
    HTTPReader::peerAddress = peerAddress;
}

const std::string & HTTPReader::getPeerName() const {
    return peerName;
}

void HTTPReader::setPeerName(const std::string & peerName) {
    HTTPReader::peerName = peerName;
}

Socket & HTTPReader::getSocket() {
    return _socket;
}

int HTTPReader::getUsedPort() const {
    return port;
}

void HTTPReader::setUsedPort(const int port) {
    HTTPReader::port = port;
}

bool HTTPReader::_isCGIMethod(HTTPRequest::TYPE type) {
    const std::vector<std::string> & ref = Configuration::getInstance().get_cgi_methods();
    for (std::vector<std::string>::const_iterator it = ref.begin(); it != ref.end(); ++it) {
        if ((*it == "POST" || *it == "post") && type == HTTPRequest::POST) {
            return true;
        } else if ((*it == "GET" || *it == "get") && type == HTTPRequest::GET) {
            return true;
        } else if ((*it == "DELETE" || *it == "delete") && type == HTTPRequest::DELETE) {
            return true;
        }
    }
    return false;
}

HTTPRequest * HTTPReader::getRequest() const {
	return request;
}

void HTTPReader::setMarked(bool marked) {
    mark = marked;
}

bool HTTPReader::isMarked() const {
    return mark;
}
