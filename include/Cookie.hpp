#ifndef COOKIE_HPP
#define COOKIE_HPP

#include "Configuration.hpp"
class Cookie {

public :
	void set_identifier(const std::string& name, const std::string& value);
	const std::map<std::string, std::string>& get_identifier();
	void set_user_agent(const std::string& str);
	const std::string &get_user_agent();
	static std::string generate_session_id();
	static Cookie generate();

bool operator==(const Cookie &cookie) {
	return (this->_identifiers == cookie._identifiers);
}
private :
	std::map<std::string, std::string> _identifiers;
	std::string _user_agent;
};


#endif