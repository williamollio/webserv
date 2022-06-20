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

Configuration::Configuration(std::string location) : e_line(0), _accept_file(false) {
		load_config_file(location);
}

void	clearws(std::string& line) {
	std::cout << line << std::endl;
	std::string::iterator	l = line.end();
	for (std::string::iterator	it = line.begin(); it != l;) {
		std::cout << "char: -" << *it << "-" << std::endl;
		if (*it == ' ' || *it == '\t') {
			std::cout << "here" << std::endl;
			line.erase(it);
		} else
			break;
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
//	if (line.find(':') < focc)
//		focc = line.find(':');
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
	std::fstream	file(path, std::fstream::in | std::fstream::out);
	std::string		line;
	if (!file.is_open())
		throw BadConfig("wrong configuration file path");
	while (file.peek() != EOF) {
		std::getline(file, line);
		switch (conf_token_cmp(getword(line))) {
			case server:
				parse_server(file, line);
				break;
			default:
				throw UnexpectedToken(e_line);
		}
		e_line++;
	}
}


Configuration::server_word Configuration::server_token_cmp(const std::string &word) {
	std::cout << "string: " << word << std::endl;
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

void Configuration::parse_server(std::fstream& file, std::string& line) {
	clearws(line);
	if (find_delim(line) == 0)
		get_next_delim_char(file, '{', true);
	while (file.peek() != EOF) {
		std::getline(file, line);
		clearws(line);
		std::cout << "-" << line << "-" << std::endl;
		switch (server_token_cmp(getword(line))) {
			case name: std::cout << line << std::endl;
				parse_vec_str(file, _server_names, line);						break;
			case port: std::cout << line << std::endl;
				parse_vec_int(file, _ports, line);	check_portnum();		break;
			case location: std::cout << line << std::endl;
				parse_vec_str(file, _server_locations, line);					break;
			case location_error: std::cout << line << std::endl;
				parse_map_int_str(file, _server_locations_error_pages, line);	break;
			case location_log: std::cout << line << std::endl;
				parse_str(_server_location_log, line);							break;
			case file_acc: std::cout << line << std::endl;
				parse_bool(_accept_file, line);									break;
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

void Configuration::parse_map_int_str(std::fstream &file, Configuration::intMapString &output, std::string &c_line) {
	bool	cb = false;
	bool	c_int = true;
	size_t	pos = c_line.find(':');
	int		int_key;
	if (pos > c_line.length()) {
		std::getline(file, c_line);
		e_line++;
		pos = 0;
	} else
		pos++;
	do {
		std::string	sub = c_line.substr(pos);
		if (c_int)
		{
			int_key = (atoi(getword(sub).c_str()));
			c_int = false;
		} else {
			output[int_key] = getword(sub);
			c_int = false;
		}
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

Configuration::vectorString Configuration::get_server_names() const {
	return _server_names;
}

Configuration::vectorInt Configuration::get_server_ports() const {
	return _ports;
}

Configuration::vectorString Configuration::get_server_location() const {
	return _server_locations;
}

Configuration::intMapString Configuration::get_server_error_page_location() const {
	return _server_locations_error_pages;
}

std::string Configuration::get_server_log_location() const {
	return _server_location_log;
}

bool Configuration::get_server_file_acceptance() const {
	return _accept_file;
}


//CLASS UNEXPECTED-TOKEN
Configuration::UnexpectedToken::UnexpectedToken(size_t _in_line) _NOEXCEPT : _line(_in_line) {
    std::stringstream ret;
    ret << "unexpected token in line: " << _line;
    _token  = ret.str();
}

const char *Configuration::UnexpectedToken::what() const _NOEXCEPT {

    return _token.c_str();
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


std::ostream& operator<<(std::ostream& os, const Configuration& conf) {
	std::vector<std::string> servnames = conf.get_server_names();
	os << "Configuration Debug:" << std::endl;
	os << "server names:" << std::endl;
	for (std::vector<std::string>::iterator it = servnames.begin(); it != servnames.end(); it++)
		os << "	" << *it << std::endl;
	os << "ports:" << std::endl;
	std::vector<int> ports = conf.get_server_ports();
	for (std::vector<int>::iterator it = ports.begin(); it != ports.end(); it++)
		os << "	" << *it << std::endl;
	os << "server locations:" << std::endl;
	std::vector<std::string> servloc = conf.get_server_location();
	for (std::vector<std::string>::iterator it = servloc.begin(); it != servloc.end(); it++)
		os << "	" << *it << std::endl;
	os << "server error-page locations:" << std::endl;
	std::map<int, std::string> serverrpages = conf.get_server_error_page_location();
	for (std::map<int, std::string>::iterator it = serverrpages.begin(); it != serverrpages.end(); it++)
		os << "	" << it->first << ": " << it->second << std::endl;
	os << "server log-file location:" << std::endl;
	os << "	" << conf.get_server_log_location() << std::endl;
	os << "server accepts files:" << std::endl;
	os << "	" << conf.get_server_file_acceptance() << std::endl;
	return os;
}