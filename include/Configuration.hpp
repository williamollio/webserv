#pragma once
#include <string>
#include <vector>
#include <map>

class Configuration {
private:
	typedef unsigned long				fsize_type;
	typedef std::vector<std::string>	vectorString;
	typedef std::vector<std::string>	vectorInt;
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
	void	get_next_delim_char(std::fstream& file, char delim);


	void	parse_vec_str(std::fstream& file, vectorString& output);		//TODO
	void	parse_vec_int(std::fstream& file, vectorInt& output);			//TODO
	void	parse_map_int_str(std::fstream& file, intMapString& output);	//TODO
	void	parse_str(std::fstream& file, std::string& output);				//TODO
	void	parse_bool(std::fstream& file, bool& output);					//TODO
public:
	Configuration();
	~Configuration();
	void	load_config_file(std::string& path);

	class UnexpectedToken : public std::exception {
	private:
		size_t	_line;
	public:
		UnexpectedToken(size_t _in_line) _NOEXCEPT;
		~UnexpectedToken() _NOEXCEPT {};
		const char *what() const _NOEXCEPT;
	};
};



