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

	class Tree {
	private:
		Tree*	next;
		Tree*	prev;
		Tree*	child;
		Tree*	parent;
		enum	type {boolean, integer, string, vectorString, vectorInt, intMapString};
	public:
		Tree();
		~Tree();

	};

	template<class T>
	class	Token {
	private:
		typedef T*	type_pointer;
	public:
		Token(const std::string& name, type_pointer target, void (*read_token_fn) (const std::string&, size_t));
		~Token() {};
		void	(*read_t_token) (const std::string&, size_t);
		const std::string	token_name;
		const type_pointer	token_target;		///WARNING: token_target can be NULL!
	};


	vectorString	_token_list;

	vectorString	_server_names;					///standard localhost:PORT
	vectorInt		_ports;							///standard 80
	vectorString	_server_locations;
	intMapString	_server_locations_error_pages;
	std::string		_server_location_log;
	bool			_accept_file;					///standard OFF

public:
	Configuration();
	~Configuration();
	void	load_config_file(int fd);
	void	load_config_file(std::string path);
private:
	void load_token_list();
};



