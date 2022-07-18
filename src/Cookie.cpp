#include "Cookie.hpp"
#include <stdio.h>
#include <stdlib.h>

const std::map<std::string, std::string>& Cookie::get_identifier() {
	return _identifiers;
}

void Cookie::set_identifier(const std::string& name, const std::string& value) {
	_identifiers[name] = value;
	return;
}

void Cookie::set_user_agent(const std::string& str) {
	_user_agent = str;
	return;
}

const std::string &Cookie::get_user_agent() {
	return _user_agent;
}

std::string Cookie::generate_session_id() {
	std::string str;
	std::string tmp;
	int pos;
	str = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	for (int i = 0; i < 10; i++) {
		pos = arc4random() % (str.size() - 1);
		tmp += str[pos];
	}
	return (tmp);
}

// Cookie Cookie::generate() {
// 	return Cookie(); // generate
// }