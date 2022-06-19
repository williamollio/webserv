#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include "Configuration.hpp"


Configuration::Configuration() : e_line(0), _accept_file(false) {
	///TODO: initialize standard values
}

Configuration::word Configuration::conf_token_cmp(const std::string& word) {
	if (word == "server")
		return server;
	else {
		return w_errortype;
	}
}

void	clearws(std::string& line) {
	std::string::iterator	l = line.end();
	for (std::string::iterator	it = line.begin(); it != l; it++) {
		if (*it == ' ' || *it == '	') {
			line.erase(it++);
		}
	}
}

size_t	find_delim(std::string& line) {
	size_t	focc = 0;
	focc = line.find(' ');
	if (line.find('	') < focc)
		focc = line.find('	');
	if (line.find('{') < focc)
		focc = line.find('{');
	if (line.find('}') < focc)
		focc = line.find('}');
	if (line.find(':') < focc)
		focc = line.find(':');
	if (line.find('#') < focc)
		focc = line.find('#');
	if (line.find(',') < focc)
		focc = line.find(',');
	return focc;
}

std::string	getword(std::string& line) {
	clearws(line);
	std::string word = line.substr(0, find_delim(line));
	return word;
}



void	Configuration::load_config_file(std::string& path) {
	std::fstream	file(path);
	std::string		line;
	while (file.peek() != EOF) {
		std::getline(file, line);
		switch (conf_token_cmp(getword(line))) {
			case server:
				parse_server(file);
				break;
			default:
				throw UnexpectedToken(e_line);
		}
		e_line++;
	}
}


Configuration::server_word Configuration::server_token_cmp(const std::string &word) {
	if (word == "server_names" || word == "server_name")
		return name;
	if (word == "port" || word == "ports")
		return port;
	if (word == "loc" || word == "locations" || word == "file_location")
		return location;
	if (word ==  "error_page_location" || word == "error_page_locations" || word == "error_pages")
		return location_error;
	if (word == "log" || word == "log_location" || word == "location_log")
		return location_log;
	if (word == "accept_files" || word == "upload_files")
		return file_acc;
	else
		return s_errortype;
}

void	Configuration::get_next_delim_char(std::fstream& file, char delim, bool ws) {
	char c;
	while (file.peek() != EOF) {
		file.get(c);
		if (c == delim)
			return;
		else if (ws && (c == ' ' || c == '	'))
			continue ;
		else if (c == '\n')
			e_line++;
		else
			throw UnexpectedToken(e_line);
	}
	throw UnexpectedToken(e_line);
}

void Configuration::parse_server(std::fstream& file) {
	std::string		line;

	get_next_delim_char(file, '{', true);
	while (file.peek() != EOF) {
		std::getline(file, line);
		switch (server_token_cmp(getword(line))) {
			case name:
				parse_vec_str(file, _server_names, line);						break;
			case port:
				parse_vec_int(file, _ports, line);	check_portnum();		break;
			case location:
				parse_vec_str(file, _server_locations, line);					break;
		//	case location_error:
		//		parse_map_int_str(file, _server_locations_error_pages, line);	break;
			case location_log:
				parse_str(file, _server_location_log, line);					break;
			case file_acc:
				parse_bool(file, _accept_file, line);							break;
			default:
				throw UnexpectedToken(e_line);
		}
		e_line++;
	}
}

void Configuration::parse_vec_str(std::fstream &file, Configuration::vectorString &output, std::string& c_line) {
	bool cb = false;
	size_t	pos = c_line.find(':');
	if (pos > c_line.length()) {
		std::getline(file, c_line);
		e_line++;
		pos = 0;
	} else
		pos++;
	do {
		std::string	sub = c_line.substr(pos);
		output.push_back(getword(sub));
		pos = find_delim(sub);
		if (!cb && c_line[pos] == ';')
			return;
		else if (c_line[pos] == '{') {
			cb = true;
			std::getline(file, c_line);
			e_line++;
		} else if (c_line[pos] == '}') {
			if (cb)
				return;
			else
				throw UnexpectedToken(e_line);
		} else if (cb && c_line[pos] == ',') {
			std::getline(file, c_line);
			e_line++;
		} else if (c_line[pos] == ' ' || c_line[pos] == '	')
			continue;
		else
			throw UnexpectedToken(e_line);
	} while (pos != 0 || cb);
	throw UnexpectedToken(e_line);
}

void Configuration::parse_vec_int(std::fstream &file, Configuration::vectorInt &output, std::string &c_line) {
	bool cb = false;
	size_t	pos = c_line.find(':');
	if (pos > c_line.length()) {
		std::getline(file, c_line);
		e_line++;
		pos = 0;
	} else
		pos++;
	do {
		std::string	sub = c_line.substr(pos);
		output.push_back(atoi(getword(sub).c_str()));
		pos = find_delim(sub);
		if (!cb && c_line[pos] == ';')
			return;
		else if (c_line[pos] == '{') {
			cb = true;
			std::getline(file, c_line);
			e_line++;
		} else if (c_line[pos] == '}') {
			if (cb)
				return;
			else
				throw UnexpectedToken(e_line);
		} else if (cb && c_line[pos] == ',') {
			std::getline(file, c_line);
			e_line++;
		} else if (c_line[pos] == ' ' || c_line[pos] == '	')
			continue;
		else
			throw UnexpectedToken(e_line);
	} while (pos != 0 || cb);
	throw UnexpectedToken(e_line);
}

void Configuration::check_portnum() {
	for (vectorInt::iterator it = _ports.begin(); it != _ports.end(); it++)
		if (*it > 65535 || *it < 0)
			throw BadConfig("Bad Portnumber");
}

void Configuration::parse_bool(bool &output, std::string &c_line) {
	size_t	pos = c_line.find(':');
		pos++;
	size_t	pos_end = c_line.find(';');
	if (pos > c_line.length() || pos_end > c_line.length())
		throw UnexpectedToken(e_line);
	std::string	sub = c_line.substr(pos, pos_end);
	sub = getword(sub);
	if (sub == "1" || sub  == "true" || sub == "TRUE" || sub == "True")
		output = true;
	else if (sub == "0" || sub  == "false" || sub == "FALSE" || sub == "False")
		output = false;
	else
		throw UnexpectedToken(e_line);
}

void Configuration::parse_str(std::string &output, std::string &c_line) {
	size_t	pos = c_line.find(':');
	if (pos > c_line.length() || c_line.find(';') > c_line.length())
		throw UnexpectedToken(e_line);
	std::string	sub = c_line.substr(pos);
	output = getword(sub);
}


//CLASS UNEXPECTED-TOKEN
Configuration::UnexpectedToken::UnexpectedToken(size_t _in_line) _NOEXCEPT : _line(_in_line) { }

const char *Configuration::UnexpectedToken::what() const _NOEXCEPT {
	std::stringstream ret;
	ret << "unexpected token in line: " << _line;
	return ret.str().c_str();
}

Configuration::BadConfig::BadConfig() _NOEXCEPT {
	_token = "unknown configuration error";
}

Configuration::BadConfig::BadConfig(const std::string& where) _NOEXCEPT {
	_token = where.c_str();
}

const char *Configuration::BadConfig::what() const _NOEXCEPT {
	return _token;
}
