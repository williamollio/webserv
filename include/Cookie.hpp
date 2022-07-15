#ifndef COOKIE_HPP
#define COOKIE_HPP

#include "Configuration.hpp"
class Cookie {

public :
	void set_identifier(const std::string& name, const std::string& value);
	const std::map<std::string, std::string>& get_identifier();
	void set_user_agent(const std::string& str);
	static Cookie generate();

bool operator==(const Cookie &cookie) {
	return (this->_identifiers == cookie._identifiers) && (this->_user_agent == cookie._user_agent);
}
private :
	std::map<std::string, std::string> _identifiers;
	std::string _user_agent;
};


#endif