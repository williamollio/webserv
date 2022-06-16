//
// Created by Manuel Hahn on 16.06.22.
//

#include "URISyntaxException.hpp"

URISyntaxException::URISyntaxException() _NOEXCEPT
        : token("", URI::Token::END, 0, 0), message("URI syntax error!") {}

URISyntaxException::URISyntaxException(const URI::Token & token) _NOEXCEPT
        : token(token), message((std::string("URI syntax error on token: '") += token.getContent()) += std::string("'!")) {}

URISyntaxException::URISyntaxException(const URI::Token & token, const std::string & message) _NOEXCEPT
        : token(token), message(message) {}

URISyntaxException::~URISyntaxException() _NOEXCEPT {}

const char * URISyntaxException::what() const _NOEXCEPT {
    return message.c_str();
}

const URI::Token &URISyntaxException::getToken() const {
    return token;
}

URISyntaxException::URISyntaxException(const URISyntaxException & other) _NOEXCEPT
        : std::exception(other), token(other.getToken()), message(other.message) {}
