#pragma once
#include <string>
#include <vector>
#include <map>

class Configuration {
private:
	typedef unsigned long				fsize_type;
	typedef std::vector<std::string>	vectorString;
	typedef std::vector<int>			vectorInt;
	typedef std::map<int, std::string>	intMapString;

	enum word {server, w_errortype};
	enum server_word {name, port, location, location_error, location_log, file_acc, s_errortype};
	size_t			e_line;

	enum word	conf_token_cmp(const std::string& word);
	enum server_word	server_token_cmp(const std::string& word);

	vectorString	_server_names;					///standard localhost:PORT
	vectorInt		_ports;							///standard 80
	vectorString	_server_locations;
	intMapString	_server_locations_error_pages;
	std::string		_server_location_log;
	bool			_accept_file;					///standard OFF

	void	parse_server(std::fstream& file);
	void	get_next_delim_char(std::fstream& file, char delim, bool ws);
	void	check_portnum();


	void	parse_vec_str(std::fstream& file, vectorString& output, std::string& c_line);
	void	parse_vec_int(std::fstream& file, vectorInt& output, std::string& c_line);
	//void	parse_map_int_str(std::fstream& file, intMapString& output, std::string& c_line);	//TODO
	void	parse_str(std::string& output, std::string& c_line);
	void	parse_bool(bool& output, std::string& c_line);
public:
	Configuration();
	void	load_config_file(std::string& path);

	class UnexpectedToken : public std::exception {
	private:
		size_t	_line;
	public:
		UnexpectedToken(size_t _in_line) _NOEXCEPT;
		const char *what() const _NOEXCEPT;
	};

	class BadConfig : public std::exception {
	private:
		const char*	_token;
	public:
		BadConfig() _NOEXCEPT;
		explicit BadConfig(const std::string& where) _NOEXCEPT;
		const char *what() const _NOEXCEPT;
	};
};



