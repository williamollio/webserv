#include "Configuration.hpp"


Configuration::Configuration() {}
Configuration::~Configuration() {
	//free token list
}

template<class T>
Configuration::Token<T>::Token(const std::string &name, Configuration::Token<T>::type_pointer target, void (*read_token_fn) (const std::string&, size_t)) : token_name(name), token_target(target) {
	read_t_token = read_token_fn;
}


void Configuration::load_token_list() {

}