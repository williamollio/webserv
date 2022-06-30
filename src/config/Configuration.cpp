#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include "Configuration.hpp"

Configuration Configuration::instance = Configuration();

Configuration & Configuration::getInstance() {
    return instance;
}

Configuration::Configuration() : e_line(0), _cmbs(0), _accept_file(false) {
	///TODO: initialize standard values
}

Configuration::Configuration(std::string& location) : e_line(0), _cmbs(0), _accept_file(false) {
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
	if (file.eof()) {
		file.close();
		throw UnexpectedToken(e_line, "EOF");
	}
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

	index += 2;
	if (index >= line.size() || line[index] == "#") {
		index = 0;
		gettokens(file, line);
	}
	if (line[index] == "{") {
		ob = true;
		index++;
	}
	bool	del = false;
	while ((ob && line[index] != "}") || (!ob && line[index] != ";")) {
		if (index >= line.size() || line[index] == "#") {
			index = 0;
			gettokens(file, line);
		} else {
			if (!del && line[index] == ",")
				del = true;
			else if ((del && line[index] == ",") || (!ob && delim_token(":{}", line[index])) || (ob && delim_token(":{;", line[index]))) {
				file.close();
				throw UnexpectedToken(e_line, line[index]);
			}
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

	index += 2;
	if (index >= line.size() || line[index] == "#") {
		index = 0;
		gettokens(file, line);
	}
	if (line[index] == "{") {
		ob = true;
		index++;
	}
	bool	del = false;
	while ((ob && line[index] != "}") || (!ob && line[index] != ";")) {
		if (index >= line.size() || line[index] == "#") {
			index = 0;
			gettokens(file, line);
		} else {
			if (!del && line[index] == ",")
				del = true;
			else if ((del && line[index] == ",") || (!ob && delim_token(":{}", line[index])) || (ob && delim_token(":{;", line[index]))) {
				file.close();
				throw UnexpectedToken(e_line, line[index]);
			} else {
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

	index += 2;
	if (index >= line.size() || line[index] == "#") {
		index = 0;
		gettokens(file, line);
	}
	if (line[index] == "{") {
		ob = true;
		index++;
	}
	bool	del = true;
	bool	page = true;

	while ((ob && line[index] != "}") || (!ob && line[index] != ";")) {
		if (index >= line.size() || line[index] == "#") {
			index = 0;
			gettokens(file, line);
		} else {
			if (del && !page && line[index] != ":") {
				file.close();
				throw UnexpectedToken(e_line, line[index]);
			}
			else if (del && !page && line[index] == ":")
				index++;
			if (!del && line[index] == ",")
				del = true;
			else if ((del && page && line[index] == ",")
					|| (!ob && delim_token(":{}", line[index]))
					|| (ob && delim_token(":{;", line[index]))) {
				file.close();
				throw UnexpectedToken(e_line, line[index]);
			}
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

	index += 2;
	if (index >= line.size() || line[index] == "#") {
		index = 0;
		gettokens(file, line);
	}
	if (line[index] == "{") {
		ob = true;
		index++;
	}
	while ((ob && line[index] != "}") || (!ob && line[index] != ";")) {
		if (index >= line.size() || line[index] == "#") {
			index = 0;
			gettokens(file, line);
		} else {

			if (line[index] == "," || (!ob && delim_token("{}", line[index])) || (ob && delim_token("{;", line[index]))) {
				file.close();
				throw UnexpectedToken(e_line, line[index]);
			}
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

	index += 2;
	if (index >= line.size() || line[index] == "#") {
		index = 0;
		gettokens(file, line);
	}
	if (line[index] == "{") {
		ob = true;
		index++;
	}

	while ((ob && line[index] != "}") || (!ob && line[index] != ";")) {
		if (index >= line.size() || line[index] == "#") {
			index = 0;
			gettokens(file, line);
		} else {
			if ((line[index] == ",") || (!ob && delim_token("{}", line[index])) || (ob && delim_token("{;", line[index]))) {
				file.close();
				throw UnexpectedToken(e_line, line[index]);
			}
			else {
				if (line[index] == "true" || line[index] == "TRUE" || line[index] == "1" || line[index] == "on")
					output = true;
				else if (line[index] == "false" || line[index] == "False" || line[index] == "0" || line[index] == "off")
					output = false;
				else {
					file.close();
					throw UnexpectedToken(e_line, line[index]);
				}
			}
			index++;
		}
	}
	return ++index;
}

size_t	Configuration::parse_sizet(std::fstream& file, vectorString& line, size_t index, size_t& output) {
	bool	ob = false;
	std::string	first;

	index += 2;
	if (index >= line.size() || line[index] == "#") {
		index = 0;
		gettokens(file, line);
	}
	if (line[index] == "{") {
		ob = true;
		index++;
	}
	while ((ob && line[index] != "}") || (!ob && line[index] != ";")) {
		if (index >= line.size() || line[index] == "#") {
			index = 0;
			gettokens(file, line);
		} else {

			if ((line[index] == ",") || (!ob && delim_token("{}", line[index])) || (ob && delim_token("{;", line[index]))) {
				file.close();
				throw UnexpectedToken(e_line, line[index]);
			}
			else {
				output = strtol(line[index].c_str(), NULL, 0);
			}
			index++;
		}
	}
	return ++index;
}

void	Configuration::fill_default() {
	if (_server_names.empty())
		_server_names.push_back("localhost");
	if (_ports.empty())
		_ports.push_back(80);
	if (_server_locations.empty())
		_server_locations.push_back("./");
}

void Configuration::load_config_file() {
	try {
		load_config_file("server.conf");
	} catch (BadConfig& ex) {
		remove("server.conf");
		std::fstream	file("server.conf", std::ios::out);
		if (!file.is_open())
			throw BadConfig("cannot create configuration file");
		fill_default();
		file << "server {\n\tserver_names: localhost;\n\tport: 80;\n\troot: ./;\n\tloc: ./;\n\taccept_files: false;\n\tcgi_path: ./;\n\t./ {\n\t\tlisting: off;\n\t\tmethods: GET;\n\t}\n}\n";
		file.close();
	}
}

void Configuration::load_config_file(const std::string &path) {
	if (path.find(".conf\0") == path.npos)
		throw  BadConfig("Bad configuration file");
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
		} else {
			switch (conf_token_cmp(splitted_line, index)) {
				case server:
					index = parse_server(file, splitted_line, index);
					break;
				default: {
					file.close();
					throw UnexpectedToken(e_line, splitted_line[index]);
				}
			}
		}
	}
	file.close();
	check_portnum();
	fill_default();
}

size_t	Configuration::parse_server(std::fstream& file, vectorString& s_line, size_t index) {
	if (!delim_token("{", s_line[++index])) {
		file.close();
		throw UnexpectedToken(e_line, s_line[index]);
	}
	index++;
	while (s_line[index] != "}") {
		if (index >= s_line.size() || s_line[index] == "#") {
			index = 0;
			gettokens(file, s_line);
		}
		while (index < s_line.size() && s_line[index] != "#") {
			if (delim_token("}", s_line[index]))
				return ++index;
			else {
				switch(server_token_cmp(s_line[index])) {
					case name:
						index = parse_vec_str(file, s_line, index, _server_names);
						break;
					case port:
						index = parse_vec_int(file, s_line, index, _ports);
						break;
					case location:
						index = parse_vec_str(file, s_line, index, _server_locations);
						break;
					case location_error:
						index = parse_map_int_str(file, s_line, index, _server_locations_error_pages);
						break;
					case location_log:
						index = parse_str(file, s_line, index, _server_location_log);
						break;
					case file_acc:
						index = parse_bool(file, s_line, index, _accept_file);
						break;
					case upload_cmbs:
						index = parse_sizet(file, s_line, index, _cmbs);
						break;
					case root:
						index = parse_str(file, s_line, index, _server_root);
						break;
					case upload_location_cl:
						index = parse_str(file, s_line, index, _client_upload_location);
						break;
					case cgi_ext:
						index = parse_vec_str(file, s_line, index, _cgi_extensions);
						break;
					case cgi_loc:
						index = parse_str(file, s_line, index, _cgi_root);
						break;
					default:
						if (find_n_fill_loc(file, s_line, index)) {
							file.close();
							throw UnexpectedToken(e_line, s_line[index]);
						}
						break;
			}}
		}
	}
	return index;
}

Configuration::server_word Configuration::server_token_cmp(const std::string &word) {
	if (word == "server_names" || word == "server_name" || word == "Server_Names" || word == "Server_names" || word == "names"|| word == "Names")
		return name;
	else if (word == "port" || word == "Port" || word == "Ports" || word == "ports" || word == "listen" || word == "Listen")
		return port;
	else if (word == "location" || word == "locations" || word == "loc" || word == "files" || word == "Files" || word == "server_location" || word == "server_locations")
		return location;
	else if (word == "root" || word == "root_location" || word == "Root")
		return root;
	else if (word == "error_location" || word == "error_locations" || word == "error_loc" || word == "error_pages" || word == "Error_Pages" || word == "error_page_location" || word == "error_page_locations")
		return location_error;
	else if (word == "log" || word == "log_loc" || word == "log_location" || word == "Log")
		return location_log;
	else if (word == "Accept_Files" || word == "accept_files" || word == "upload" || word == "uploads" || word ==  "user_upload" || word == "user_uploads" || word == "Upload" || word == "Uploads" || word ==  "User_Upload" || word == "User_Uploads")
		return file_acc;
	else if (word == "upload" || word == "Upload" || word == "client-upload")
		return upload_location_cl;
	else if (word == "client_max_body_size")
		return upload_cmbs;
	else if (word == "cgi_ext" || word == "CGI_ext" || word == "cgi_extension" || word == "CGI_extension")
		return cgi_ext;
	else if (word == "cgi_path" || word == "CGI_path" || word == "CGI_root" || word == "cgi_root")
		return cgi_loc;
	return s_errortype;
}

Configuration::loc_word		Configuration::loc_inf_token_cmp(const std::string& word) {
	if (word == "method" || word == "methods")
		return methods;
	else if (word == "root" || word == "directory" || word == "dir")
		return local_root;
	else if (word == "def_file" || word == "default" || word == "default_file")
		return default_file;
	else if (word == "directory_listing" || word == "listing" || word == "dir_listing")
		return directory_listing;
	return l_errortype;
}

size_t	Configuration::parse_methods(std::fstream &file, vectorString &line, size_t index, loc_inf& output) {
	bool	ob = false;

	index += 2;
	if (index >= line.size() || line[index] == "#") {
		index = 0;
		gettokens(file, line);
	}
	if (line[index] == "{") {
		ob = true;
		index++;
	}
	bool	del = false;
	while ((ob && line[index] != "}") || (!ob && line[index] != ";")) {
		if (index >= line.size() || line[index] == "#") {
			index = 0;
			gettokens(file, line);
		} else {
			if (!del && line[index] == ",")
				del = true;
			else if ((del && line[index] == ",") || (!ob && delim_token(":{}", line[index])) || (ob && delim_token(":{;", line[index]))) {
				file.close();
				throw UnexpectedToken(e_line, line[index]);
			}
			else {
				if (line[index] == "GET" || line[index] == "get")
					output.GET = true;
				else if (line[index] == "DELETE" || line[index] == "delete")
					output.DELETE = true;
				else if (line[index] == "POST" || line[index] == "post")
					output.POST = true;
				else {
					file.close();
					throw UnexpectedToken(e_line, line[index]);
				}
				del = false;
			}
			index++;
		}
	}
	return ++index;
}

static void	init_data(Configuration::loc_inf& data, size_t id, const std::string& loc) {
	data.GET = false;
	data.DELETE = false;
	data.POST = false;
	data.id = id;
	data.directory = loc;
}

size_t	Configuration::skip_token(std::fstream& file, vectorString& line, size_t index) {
	bool	ob = false;

	index += 2;
	if (index >= line.size() || line[index] == "#") {
		index = 0;
		gettokens(file, line);
	}
	if (line[index] == "{") {
		ob = true;
		index++;
	}
	bool	del = false;
	while ((ob && line[index] != "}") || (!ob && line[index] != ";")) {
		if (index >= line.size() || line[index] == "#") {
			index = 0;
			gettokens(file, line);
		} else {
			if (!del && line[index] == ",")
				del = true;
			else if ((del && line[index] == ",") || (!ob && delim_token(":{}", line[index])) || (ob && delim_token(":{;", line[index]))) {
				file.close();
				throw UnexpectedToken(e_line, line[index]);
			} else
				del = false;
			index++;
		}
	}
	return ++index;
}

size_t Configuration::parse_loc_info(std::fstream &file, vectorString &line, size_t index, size_t id) {
	if (!delim_token("{", line[++index])) {
		file.close();
		throw UnexpectedToken(e_line, line[index]);
	}
	index++;
	loc_inf	data;
	init_data(data, id, _server_locations[id]);
	while (line[index] != "}") {
		if (index >= line.size() || line[index] == "#") {
			index = 0;
			gettokens(file, line);
		}
		while (index < line.size() && line[index] != "#") {
			if (delim_token("}", line[index])) {
				_server_location_info.push_back(data);
				return ++index;
			}
			else {
				switch (loc_inf_token_cmp(line[index])) {
					case methods:
						index = parse_methods(file, line, index, data);
						break;
					case directory_listing:
						index = parse_bool(file, line, index, data.dir_listing);
						break;
					case default_file:
						index = parse_str(file, line, index, data.def_file);
						break;
					case local_root:
						index = parse_str(file, line, index, data.root);
						break;
					case skip:
						index = skip_token(file, line, index);
						break;
					default: {
						file.close();
						throw UnexpectedToken(e_line, line[index]);
					}
				}
			}
		}
	}
	_server_location_info.push_back(data);
	return index;
}

bool Configuration::find_n_fill_loc(std::fstream &file, vectorString &line, size_t &index) {
	size_t	id = 0;
	for (vectorString::iterator name = _server_locations.begin(); name != _server_locations.end(); name++) {
		if (*name == _server_locations[index]) {
			index = parse_loc_info(file, line, index, id);
			return false;
		}
		id++;
	}
	return true;
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

const Configuration::vectorString & Configuration::get_server_names() const {
	return _server_names;
}

const Configuration::vectorInt & Configuration::get_server_ports() const {
	return _ports;
}

const Configuration::vectorString & Configuration::get_server_location() const {
	return _server_locations;
}

const Configuration::intMapString & Configuration::get_server_error_page_location() const {
	return _server_locations_error_pages;
}

const std::string & Configuration::get_server_log_location() const {
	return _server_location_log;
}

bool Configuration::get_server_file_acceptance() const {
	return _accept_file;
}



void Configuration::check_portnum() {
	for (vectorInt::iterator it = _ports.begin(); it != _ports.end(); it++)
		if (*it > 65535 || *it < 0)
			throw std::invalid_argument("Configuration-File: Error: Bad Portnumber");
}

const Configuration::vectorString & Configuration::get_cgi_extensions() const {
    return _cgi_extensions;
}

const std::string & Configuration::get_cgi_root() const {
    return _cgi_root;
}

size_t Configuration::get_server_max_upload_size() const {
	return _cmbs;
}

const std::string Configuration::get_server_root_folder() const {
	return _server_root;
}

const std::string Configuration::get_upload_location_cl() const {
	return _client_upload_location;
}

const std::vector<Configuration::loc_inf> &Configuration::get_location_specifier() const{
	return _server_location_info;
}


Configuration::UnexpectedToken::UnexpectedToken(size_t _in_line, std::string tok) _NOEXCEPT : _line(_in_line){
	std::stringstream ret;
	ret << "Configuration-File: Error: Line: " << _line << ", unexpected token: " << tok;
	_token  = ret.str();
}

const char *Configuration::UnexpectedToken::what() const _NOEXCEPT {

    return _token.c_str();
}



Configuration::BadConfig::BadConfig() _NOEXCEPT {
	_token = "unknown configuration error";
}

Configuration::BadConfig::BadConfig(std::string where) _NOEXCEPT {
	std::stringstream ret;
	ret << where;
	_token = ret.str();
}

const char *Configuration::BadConfig::what() const _NOEXCEPT {
	return _token.c_str();
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
	os << "server root:" << std::endl;
	os << "	" << conf.get_server_root_folder() << std::endl;
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
	os << "server client upload location:" << std::endl;
	os << "	" << conf.get_upload_location_cl() << std::endl;
	if (conf.get_server_file_acceptance()) {
		os << "server max file size:" << std::endl;
		os << "	" << conf.get_server_max_upload_size() << std::endl;
	}
	for (std::vector<Configuration::loc_inf>::const_iterator it = conf.get_location_specifier().begin(); it != conf.get_location_specifier().end(); it++) {
		os << "location: " << std::endl;
			os << (*it).directory << std::endl;
		os << "	root:" << std::endl;
		os << "		" << (*it).root << std::endl;
		os << "	Get:" << std::endl;
		os << "		" << (*it).GET << std::endl;
		os << "	Post:" << std::endl;
		os << "		" << (*it).POST << std::endl;
		os << "	Delete:" << std::endl;
		os << "		" << (*it).DELETE << std::endl;
		os << "	listing:" << std::endl;
		os << "		" << (*it).dir_listing << std::endl;
		os << "	default file:" << std::endl;
		os << "		" << (*it).def_file << std::endl;
		os << "	id:" << std::endl;
		os << (*it).id << std::endl;

	}

	return os;
}