//
// Created by Manuel Hahn on 6/10/22.
//

#include "CGIResponsePost.hpp"

//bool CGIResponsePost::isUploadAccepted() {return _accept_file;}

std::string CGIResponsePost::setFilename(std::string &payload) {
	std::string filename;

	size_t posbegin, posend;
	posbegin = payload.find("filename=\"");
	if (posbegin == std::string::npos && !_request->_path.empty())
		return (_request->_path);
	else if (posbegin == std::string::npos)
		return setFilenameUnknown(".txt");
	posbegin += 10;
	posend = payload.find("\"", posbegin + 1);
	filename = payload.substr(posbegin, posend - posbegin);
	trimPayload(payload);
	return (filename);
}

std::string CGIResponsePost::setFilenameUnknown(std::string extension)
{
	std::string filenameChunked;

	filenameChunked = "test"; // insert date of the day
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

	posend = tmp.find(delimiter, posbegin);
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
	PRINT_CGIRESPONSEPOST("_filename: ", _filename);
	std::ofstream ofs(_filename);
	//PRINT_CGIRESPONSEPOST("payload: ", payload);

	ofs << payload << std::endl;
	ofs.close();
}

void CGIResponsePost::saveFile(std::string payload) {
	DIR* dir;
	trim_slash_begin(_upload);
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

void CGIResponsePost::run(Socket &socket) {

	HTTPHeader	header;
	int			code;

	PRINT_CGIRESPONSEPOST("_POST", _POST);
	PRINT_CGIRESPONSEPOST("_accept_file", _accept_file);
	if (_request->_path == "/file_should_exist_after")
		std::cout << "breakpoint" << std::endl;

	if (!_POST && !_accept_file)
		throw HTTPException(405);
	code = 201;
	saveFile(_request->_payload);
	header.setStatusCode(code);
	header.setStatusMessage(get_message(code));
	std::string body = int_to_string(code) + " " + header.getStatusMessage();
	header.set_content_length(body.length());
	socket.send(header.tostring() + "\r\n\r\n" + body);
}

CGIResponsePost::CGIResponsePost(HTTPRequest *request): CGIResponse(request)
{
	Configuration config = Configuration::getInstance();

	_error_pages  = config.get_server_error_page_location();
	_accept_file = config.get_server_file_acceptance();
	_server_root = config.get_server_root_folder();
	_server_index = config.get_server_index_file();
	_upload = _server_root + config.get_upload_location_cl();

	_server_location_log = set_absolut_path(_server_root);
	if (is_request_defined_location(request->_path, config.get_location_specifier())) {
			if (_request != NULL)
				PRINT_CGIRESPONSE("request_path", _request->_path);
		PRINT_CGIRESPONSE("_server_location_log", _server_location_log);
		PRINT_CGIRESPONSE("_loc_root", _loc_root);
    	_server_location_log = set_absolut_path(_loc_root);
    }

	PRINT_CGIRESPONSEPOST("_upload: ", _upload);
}
