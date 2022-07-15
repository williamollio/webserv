#include "Cookie.hpp"

const std::map<std::string, std::string>& Cookie::get_identifier() {
	return _identifiers;
}

void Cookie::set_identifier(const std::string& name, const std::string& value) {
	_identifiers.emplace(name, value);
	return;
}

void Cookie::set_user_agent(const std::string& str) {
	_user_agent = str;
	return;
}

Cookie Cookie::generate() {
	return Cookie(); // generate
}