#include <fstream>
#include <iostream>
#include <sstream>
#include "Configuration.hpp"


Configuration::Configuration() : e_line(0) {
	///TODO: initialize standard values
}
Configuration::~Configuration() {
	//free token list
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

void	Configuration::get_next_delim_char(std::fstream& file, char delim) {
	char c;
	while (file.peek() != EOF) {
		file.get(c);
		if (c == delim)
			return;
		else if (c == ' ' || c == '	')
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

	get_next_delim_char(file, '{');
	while (file.peek() != EOF) {
		std::getline(file, line);
		switch (server_token_cmp(getword(line))) {
			case name:
				parse_vec_str(file, _server_names);						break;
			case port:
				parse_vec_int(file, _ports);							break;
			case location:
				parse_vec_str(file, _server_locations);					break;
			case location_error:
				parse_map_int_str(file, _server_locations_error_pages);	break;
			case location_log:
				parse_str(file, _server_location_log);					break;
			case file_acc:
				parse_bool(file, _accept_file);							break;
			default:
				throw UnexpectedToken(e_line);
		}
		e_line++;
	}
}


//CLASS UNEXPECTED-TOKEN
Configuration::UnexpectedToken::UnexpectedToken(size_t _in_line) _NOEXCEPT : _line(_in_line) { }

const char *Configuration::UnexpectedToken::what() const _NOEXCEPT {
	std::stringstream ret;
	ret << "unexpected token in line: " << _line;
	return ret.str().c_str();
}
