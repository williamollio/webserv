#pragma once
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>

class Configuration {
public:
	typedef struct location_info {
		std::string	directory;
		bool		GET;
		bool		POST;
		bool		DELETE;

		bool		dir_listing;
		std::string	def_file;
		std::string	root;	//if root empty, it should use the server_root
		size_t		id;
		size_t		upload_size;
	} loc_inf;
private:
	typedef unsigned long				fsize_type;
	typedef std::vector<std::string>	vectorString;
	typedef std::vector<int>			vectorInt;
	typedef std::map<int, std::string>	intMapString;

	enum word {server, w_errortype};
	enum server_word {name, port, root, index_file, upload_location_cl, location, location_error, location_log, file_acc, upload_cmbs, cgi_ext, cgi_loc, cgi_methods, cgi_bin, s_errortype};
	enum loc_word {methods, upload_size, directory_listing, local_root, default_file, skip, l_errortype};
	size_t			e_line;

	enum word			conf_token_cmp(vectorString& line, size_t index);
	enum server_word	server_token_cmp(const std::string& word);
	enum loc_word		loc_inf_token_cmp(const std::string& word);

	vectorString			_server_names;					///standard localhost:PORT
	vectorInt				_ports;							///standard 80
	vectorString			_cgi_extensions;
    vectorString            _cgi_methods;
	std::string				_cgi_root;
    vectorString            _cgi_bin;
    std::map<std::string, std::string> _cgi_bin_map;
	vectorString			_server_locations;
	std::vector<loc_inf>	_server_location_info;
	std::string				_server_root;
	std::string				_index_file;
	std::string				_client_upload_location;
	intMapString			_server_locations_error_pages;
	std::string				_server_location_log;
	size_t					_cmbs;
	bool					_accept_file;					///standard OFF

	size_t	parse_server(std::fstream& file, vectorString& s_line, size_t index);
	void	check_portnum();
    void    checkCGIExtensions(vectorString& s_line, size_t index);
	bool	delim_token(const std::string& delims, std::string& word);


	size_t	parse_vec_str(std::fstream& file, vectorString& input, size_t index, vectorString& output);
	size_t	parse_vec_int(std::fstream& file, vectorString& input, size_t index, vectorInt& output);
	size_t	parse_map_int_str(std::fstream& file, vectorString& input, size_t index, intMapString& output);
	size_t	parse_str(std::fstream& file, vectorString& input, size_t index, std::string& output);
	size_t	parse_bool(std::fstream& file, vectorString& input, size_t index, bool& output);
	size_t	parse_sizet(std::fstream& file, vectorString& line, size_t index, size_t& output);
	size_t	parse_loc_info(std::fstream& file, vectorString& line, size_t index, size_t id);
	size_t	parse_methods(std::fstream &file, std::vector<std::string> &line, size_t index, loc_inf& output);

	bool	find_n_fill_loc(std::fstream& file, vectorString& line, size_t& index);
	size_t	skip_token(std::fstream& file, vectorString& line, size_t index);
	void fill_default();
	void normalize_path(std::string& str);

	void gettokens(std::fstream &file, vectorString& line);

    static Configuration instance;

	Configuration();
	Configuration(std::string& location);
public:
	void	load_config_file(const std::string& path);
	void	load_config_file();

	const vectorString & get_server_names()               const;
	const vectorInt &    get_server_ports()               const;
	const vectorString & get_server_location()            const;
	const intMapString & get_server_error_page_location() const;
	const std::string &  get_server_log_location()        const;
	bool                 get_server_file_acceptance()     const;
    const vectorString & get_cgi_extensions()             const;
    const vectorString & get_cgi_methods()                const;
    const std::map<std::string, std::string> & get_cgi_bin_map() const;
    const std::string &  get_cgi_root()                   const;
	size_t					get_server_max_upload_size()  const;
	const std::string		get_server_root_folder()      const;
	const std::string		get_server_index_file()       const;
	const std::string		get_upload_location_cl()      const;
	const std::vector<loc_inf>& get_location_specifier()  const;

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


