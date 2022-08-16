//
// Created by Manuel Hahn on 6/10/22.
//

#include "CGIResponsePost.hpp"
#include "Connection.hpp"
#include <time.h>
#include <sys/time.h>

std::string CGIResponsePost::setFilename(std::string &payload) {
	std::string filename;

	size_t posbegin, posend;
	posbegin = payload.find("filename=\"");
	if (posbegin == std::string::npos)
		return setFilenameUnknown(".txt");
	posbegin += 10;
	posend = payload.find("\"", posbegin + 1);
	filename = payload.substr(posbegin, posend - posbegin);
	trimPayload(payload);
	return (filename);
}

void CGIResponsePost::get_date(std::string &time) {
	struct timeval tv;
	time_t t;
	struct tm *info;

	gettimeofday(&tv, NULL);
	t = tv.tv_sec;
	info = localtime(&t);
	char *tmp = asctime (info);
	time = tmp;
}

std::string CGIResponsePost::setFilenameUnknown(std::string extension)
{
	std::string filenameChunked;

	std::string time;
	get_date(time);

	filenameChunked = time;
	filenameChunked += extension;

	return (filenameChunked);
}
std::string CGIResponsePost::getDelimiter(std::string &tmp) {
	std::string del;
	size_t posbegin = 2, posend;

	posend = tmp.find("\n", posbegin);
	del = tmp.substr(posbegin + 1, posend - posbegin);
	del.erase(del.length() - 2);
	return (del);
}

void CGIResponsePost::trimPayload(std::string &payload) {
	std::string delimiter;
	std::string tmp(payload);

	payload.clear();
	size_t posbegin, posend;
	delimiter = getDelimiter(tmp);
	posbegin = tmp.find("\r\n\r\n");
	if (posbegin == std::string::npos)
		throw HTTPException(400);
	posbegin += 4;
	posend = tmp.find(delimiter);
	if (posend == std::string::npos) {
		std::cerr << "no delim found" << std::endl;
		throw HTTPException(400);
	}
	posend -= 4;
	payload = tmp.substr(posbegin, posend - posbegin);
}

void CGIResponsePost::createFile(std::string &payload) {
	_filename = setFilename(payload);
	if (_filename.front() == '/')
		_filename.erase(0,1);
	std::ofstream ofs(_filename);

	ofs << payload << std::endl;
	ofs.close();
}

bool CGIResponsePost::isBodySizeForbidden(size_t payload_size) {
	if (_upload_size_bool == true) {
		if (payload_size > _upload_size)
			return true;
		else
			return false;
	}
	else if ( _max_size_body_bool == true) {
		if (payload_size > _max_size_body)
			return true;
		else
			return false;
	}
	else
		return false;
}

void CGIResponsePost::saveFile(std::string payload) {
	DIR* dir;
	trim_slash_begin(_upload);

	if (isBodySizeForbidden(payload.size()))
		throw HTTPException(413);

	const char *upload = _upload.c_str();
	const char *server_location_log = _server_location_log.c_str();
	dir = opendir(upload);
	if (dir) {
		if (chdir(upload) != 0)
			throw HTTPException(404);

		createFile(payload);
		if (chdir(server_location_log) != 0)
			throw HTTPException(404);
		closedir(dir);
	}
	else {
		if (mkdir(upload, 0700) != 0)
			throw HTTPException(500);
	}
}

void CGIResponsePost::run() {

	HTTPHeader	header;
	int			code;

	if (!_POST && !_accept_file)
		throw HTTPException(405);
	code = 201;
	saveFile(_request->get_payload());
	header.setStatusCode(code);
	header.setStatusMessage(get_message(code));
	std::string body = int_to_string(code) + " " + header.getStatusMessage();
	header.set_content_length(body.length());
    _payload = header.tostring() + "\r\n\r\n" + body;
    if (!runForFD(0)) {
        _running = true;
        Connection::getInstance().add_fd(_socket.get_fd(), this, false);
    }
	//socket.write(header.tostring() + "\r\n\r\n" + body);
}

bool CGIResponsePost::runForFD(int) {
    try {
        ssize_t ret = _socket.write(_payload.c_str() + _payloadCounter, _payload.size() - _payloadCounter < 65536 ? _payload.size() - _payloadCounter : 65536);
        _payloadCounter += ret;
        if (_payloadCounter < _payload.size()) {
            return false;
        }
        debug("Write with socket fd " << _socket.get_fd() << " size " << _payloadCounter << " real " << _payload.size());
        debug("Closing socket fd " << _socket.get_fd());
        _socket.close();
        _running = false;
        return true;
    } catch (IOException &) {
        debug("Write with socket fd " << _socket.get_fd() << " size " << _payloadCounter);
        return false;
    }
}

bool CGIResponsePost::isRunning() {
    return _running;
}

CGIResponsePost::CGIResponsePost(HTTPRequest *request, Socket & socket, Runnable & parent): CGIResponse(request, socket, parent), _payloadCounter(0), _max_size_body(SIZE_MAX), _running(false)
{
	Configuration config = Configuration::getInstance();

	_max_size_body_bool = config.get_server_max_upload_size_bool();
	_error_pages  = config.get_server_error_page_location();
	_accept_file = config.get_server_file_acceptance();
	_server_root = config.get_server_root_folder();
	_server_index = config.get_server_index_file();
	_upload = _server_root + config.get_upload_location_cl();
	_max_size_body = config.get_server_max_upload_size();

	_server_location_log = set_absolut_path(_server_root);
	if (is_request_defined_location(request->getPath(), config.get_location_specifier()))
    	_server_location_log = set_absolut_path(_loc_root);

}
