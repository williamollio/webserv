#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>

class Configuration {
private:
	typedef unsigned long				fsize_type;
	typedef std::vector<std::string>	vectorString;
	typedef std::vector<int>			vectorInt;
	typedef std::map<int, std::string>	intMapString;

	enum word {server, w_errortype};
	enum server_word {name, port, location, location_error, location_log, file_acc, eos, s_errortype};
	size_t			e_line;

	enum word	conf_token_cmp(vectorString& line, size_t index);
	enum server_word	server_token_cmp(const std::string& word);

	vectorString	_server_names;					///standard localhost:PORT
	vectorInt		_ports;							///standard 80
	vectorString	_server_locations;
	intMapString	_server_locations_error_pages;
	std::string		_server_location_log;
	bool			_accept_file;					///standard OFF

	size_t	parse_server(std::fstream& file, vectorString& s_line, size_t index);
	void	get_next_delim_char(std::fstream& file, char delim, bool ws);
	void	check_portnum();
	std::string	getword(vectorString& line);
	bool	delim_token(const std::string& delims, std::string& word);


	size_t	parse_vec_str(std::fstream& file, vectorString& input, size_t index, vectorString& output);
	size_t	parse_vec_int(std::fstream& file, vectorString& input, size_t index, vectorInt& output);
	size_t	parse_map_int_str(std::fstream& file, vectorString& input, size_t index, intMapString& output);
	size_t	parse_str(std::fstream& file, vectorString& input, size_t index, std::string& output);
	size_t	parse_bool(std::fstream& file, vectorString& input, size_t index, bool& output);

	void fill_default();

	void gettokens(std::fstream &file, vectorString& line);

    static Configuration instance;

	Configuration();
	Configuration(std::string location);
public:
	void	load_config_file(const std::string& path);

	const vectorString & get_server_names()               const;
	const vectorInt &    get_server_ports()               const;
	const vectorString & get_server_location()            const;
	const intMapString & get_server_error_page_location() const;
	const std::string &  get_server_log_location()        const;
	bool                 get_server_file_acceptance()     const;

    static Configuration & getInstance();

	//EXCEPTIONS
	class UnexpectedToken : public std::exception {
	private:
		size_t	_line;
		std::string _token;
	public:
		//UnexpectedToken(size_t _in_line, std::string& tok) _NOEXCEPT;
		UnexpectedToken(size_t _in_line, std::string tok) _NOEXCEPT;
		~UnexpectedToken() _NOEXCEPT {}
		const char *what() const _NOEXCEPT;
	};

	class BadConfig : public std::exception {
	private:
		std::string	_token;
	public:
		BadConfig() _NOEXCEPT;
		BadConfig(std::string where) _NOEXCEPT;
		~BadConfig() _NOEXCEPT {}
		const char *what() const _NOEXCEPT;
	};
};


std::ostream& operator<<(std::ostream& os, const Configuration& conf);


