#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include "Configuration.hpp"


Configuration::Configuration() : e_line(0), _accept_file(false) {
	///TODO: initialize standard values
}

Configuration::Configuration(std::string location) : e_line(0), _accept_file(false) {
		load_config_file(location);
}



bool catch_special(std::string& line, size_t li) {
	switch (line[li]) {
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

std::vector<std::string>	split_line(std::string& line) {
	size_t i = 0;
	size_t li = 0;
	std::vector<std::string>	ret;
	while (li != line.length()) {
		i = li;
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
	}
	return ret;
}

void Configuration::gettokens(std::fstream &file, vectorString& line) {
	std::string tmp;
	if (file.eof())
		throw UnexpectedToken(e_line, "EOF");
	std::getline(file, tmp);
	line = split_line(tmp);
	e_line++;


}

Configuration::word Configuration::conf_token_cmp(Configuration::vectorString &line, size_t index) {
	if (line[index] == "server" || line[index] == "Server")
		return server;
	return w_errortype;
}

size_t	Configuration::parse_vec_str(std::fstream& file, vectorString& line, size_t index, vectorString& output) {
	bool	ob = false;

	index++;
	if (index >= line.size() || line[index] == "#") {
		index = 0;
		gettokens(file, line);
	}
	if (line[index++] == "{")
		ob = true;
	bool	del = false;
	while ((ob && line[index] != "}") || (!ob && line[index] != ";")) {
		if (index >= line.size() || line[index] == "#") {
			index = 0;
			gettokens(file, line);
		} else {
			if (!del && line[index] == ",")
				del = true;
			else if (del && line[index] == ",")
				throw UnexpectedToken(e_line, line[index]);
			else if (!ob && delim_token("{}", line[index]))
				throw UnexpectedToken(e_line, line[index]);
			else if (ob && delim_token("{;", line[index]))
				throw UnexpectedToken(e_line, line[index]);
			else {
				output.push_back(line[index]);
				del = false;
			}
			index++;
		}
	}
	return ++index;
}

size_t	Configuration::parse_vec_int(std::fstream& file, vectorString& line, size_t index, vectorInt& output) {
	bool	ob = false;

	index++;
	if (index >= line.size() || line[index] == "#") {
		index = 0;
		gettokens(file, line);
	}
	if (line[index++] == "{")
		ob = true;
	bool	del = false;
	while ((ob && line[index] != "}") || (!ob && line[index] != ";")) {
		if (index >= line.size() || line[index] == "#") {
			index = 0;
			gettokens(file, line);
		} else {
			if (!del && line[index] == ",")
				del = true;
			else if (del && line[index] == ",")
				throw UnexpectedToken(e_line, line[index]);
			else if (!ob && delim_token(":{}", line[index]))
				throw UnexpectedToken(e_line, line[index]);
			else if (ob && delim_token(":{;", line[index]))
				throw UnexpectedToken(e_line, line[index]);
			else {
				output.push_back(atoi(line[index].c_str()));
				del = false;
			}
			index++;
		}
	}
	return ++index;
}

size_t	Configuration::parse_map_int_str(std::fstream& file, vectorString& line, size_t index, intMapString& output) {
	bool	ob = false;
	std::string	first;

	index++;
	if (index >= line.size() || line[index] == "#") {
		index = 0;
		gettokens(file, line);
	}
	if (line[index++] == "{")
		ob = true;
	bool	del = true;
	bool	page = true;

	while ((ob && line[index] != "}") || (!ob && line[index] != ";")) {
		if (index >= line.size() || line[index] == "#") {
			index = 0;
			gettokens(file, line);
		} else {
			if (del && !page && line[index] != ":")
				throw UnexpectedToken(e_line, line[index]);
			else if (del && !page && line[index] == ":")
				index++;
			if (!del && line[index] == ",")
				del = true;
			else if ((del && line[index] == ",")
					|| (!ob && delim_token("{}", line[index]))
					|| (ob && delim_token("{;", line[index])))
				throw UnexpectedToken(e_line, line[index]);
			else {
				if (page) {
					first = line[index];
					page = false;
				} else {
					output.insert(std::pair<int, std::string>(atoi(first.c_str()), line[index]));
					del = false;
					page = true;
				}
			}
			index++;
		}
	}
	return ++index;
}

size_t	Configuration::parse_str(std::fstream& file, vectorString& line, size_t index, std::string& output) {
	bool	ob = false;
	std::string	first;

	index++;
	if (index >= line.size() || line[index] == "#") {
		index = 0;
		gettokens(file, line);
	}
	if (line[index++] == "{")
		ob = true;
	bool	del = true;
	bool	page = true;

	while ((ob && line[index] != "}") || (!ob && line[index] != ";")) {
		if (index >= line.size() || line[index] == "#") {
			index = 0;
			gettokens(file, line);
		} else {
			if (del && !page && line[index] != ":")
				throw UnexpectedToken(e_line, line[index]);
			else if (del && !page && line[index] == ":")
				index++;
			if (!del && line[index] == ",")
				del = true;
			else if (del && line[index] == ",")
				throw UnexpectedToken(e_line, line[index]);
			else if (!ob && delim_token("{}", line[index]))
				throw UnexpectedToken(e_line, line[index]);
			else if (ob && delim_token("{;", line[index]))
				throw UnexpectedToken(e_line, line[index]);
			else {
				output = line[index];
			}
			index++;
		}
	}
	return ++index;
}

size_t	Configuration::parse_bool(std::fstream& file, vectorString& line, size_t index, bool& output) {
	bool	ob = false;
	std::string	first;

	index++;
	if (index >= line.size() || line[index] == "#") {
		index = 0;
		gettokens(file, line);
	}
	if (line[index++] == "{")
		ob = true;
	bool	del = true;
	bool	page = true;

	while ((ob && line[index] != "}") || (!ob && line[index] != ";")) {
		if (index >= line.size() || line[index] == "#") {
			index = 0;
			gettokens(file, line);
		} else {
			if (del && !page && line[index] != ":")
				throw UnexpectedToken(e_line, line[index]);
			else if (del && !page && line[index] == ":")
				index++;
			if (!del && line[index] == ",")
				del = true;
			else if (del && line[index] == ",")
				throw UnexpectedToken(e_line, line[index]);
			else if (!ob && delim_token("{}", line[index]))
				throw UnexpectedToken(e_line, line[index]);
			else if (ob && delim_token("{;", line[index]))
				throw UnexpectedToken(e_line, line[index]);
			else {
				if (line[index] == "true" || line[index] == "TRUE" || line[index] == "1")
					output = true;
				else if (line[index] == "false" || line[index] == "False" || line[index] == "0")
					output = false;
				else
					throw UnexpectedToken(e_line, line[index]);
			}
			index++;
		}
	}
	return ++index;
}


void Configuration::load_config_file(std::string &path) {
	std::fstream	file(path, std::fstream::in | std::fstream::out);
	std::string		line;
	vectorString	splitted_line;
	size_t			index = 0;
	if (!file.is_open())
		throw BadConfig("wrong configuration file path");
	while (!file.eof()) {
		if (index >= splitted_line.size()) {
			index = 0;
			std::getline(file, line);
			splitted_line = split_line(line);
			e_line++;
		}
		switch (conf_token_cmp(splitted_line, index)) {
			case server:
				index = parse_server(file, splitted_line, index);
				break;
			default:
				throw UnexpectedToken(e_line, splitted_line[index]);
		}

	}
}
size_t	Configuration::parse_server(std::fstream& file, vectorString& s_line, size_t index) {
	if (!delim_token("{", s_line[++index]))
		throw UnexpectedToken(e_line, s_line[index]);
	index++;
	while (!file.eof()) {
		if (index >= s_line.size() || s_line[index] == "#") {
			index = 0;
			gettokens(file, s_line);
		}
		while (index < s_line.size() && s_line[index] != "#") {
			if (delim_token("}", s_line[index]))
				return index;
			else {
				switch(server_token_cmp(s_line[index])) {
					case name:
						std::cout << "server names: " << "[WARNING]	" << "cannot parse <name>:<portnumber>" << std::endl;
						index = parse_vec_str(file, s_line, index, _server_names);
						break;
					case port:
						std::cout << "port" << std::endl;
						index = parse_vec_int(file, s_line, index, _ports);
						break;
					case location:
						std::cout << "loc" << std::endl;
						index = parse_vec_str(file, s_line, index, _server_locations);
						break;
					case location_error:
						std::cout << "loc err" << std::endl;
						index = parse_map_int_str(file, s_line, index, _server_locations_error_pages);
						break;
					case location_log:
						std::cout << "log" << std::endl;
						index = parse_str(file, s_line, index, _server_location_log);
						break;
					case file_acc:
						index = parse_bool(file, s_line, index, _accept_file);
						break;
					default:
						throw UnexpectedToken(e_line, s_line[index]);
			}}
		}
	}
	return index;
}

Configuration::server_word Configuration::server_token_cmp(const std::string &word) {
	if (word == "server_names" || word == "server_name" || word == "Server_Names" || word == "Server_names" || word == "names"|| word == "Names")
		return name;
	else if (word == "port" || word == "Port" || word == "Ports" || word == "ports")
		return port;
	else if (word == "location" || word == "locations" || word == "loc" || word == "files" || word == "Files" || word == "server_location" || word == "server_locations")
		return location;
	else if (word == "error_location" || word == "error_locations" || word == "error_loc" || word == "error_pages" || word == "Error_Pages" || word == "error_page_location" || word == "error_page_locations")
		return location_error;
	else if (word == "log" || word == "log_loc" || word == "log_location" || word == "Log")
		return location_log;
	else if (word == "Accept_Files" || word == "accept_files" || word == "upload" || word == "uploads" || word ==  "user_upload" || word == "user_uploads" || word == "Upload" || word == "Uploads" || word ==  "User_Upload" || word == "User_Uploads")
		return file_acc;
	return s_errortype;
}

bool Configuration::delim_token(const std::string& delims, std::string &word) {
	size_t i;
	for ( i = 0; delims.c_str()[i]; i++) {
		if (delims.substr(i, 1) == word)
			return true;
	}
	return false;
}

//GETTER FUNCTIONS

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



//void Configuration::check_portnum() {
//	for (vectorInt::iterator it = _ports.begin(); it != _ports.end(); it++)
//		if (*it > 65535 || *it < 0)
//			throw BadConfig("Bad Portnumber");
//}


//CLASS UNEXPECTED-TOKEN
//Configuration::UnexpectedToken::UnexpectedToken(size_t _in_line, std::string& tok) _NOEXCEPT : _line(_in_line) {
//    std::stringstream ret;
//    ret << "line: " << _line << " unexpected token: " << tok;
//    _token  = ret.str();
//}

Configuration::UnexpectedToken::UnexpectedToken(size_t _in_line, std::string tok) _NOEXCEPT : _line(_in_line){
	std::stringstream ret;
	ret << "line: " << _line << " unexpected token: " << tok;
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