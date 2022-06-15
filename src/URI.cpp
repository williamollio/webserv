//
// Created by Manuel Hahn on 6/14/22.
//

#include <iostream>
#include "URI.hpp"

URI::URI(const std::string &uri): original(uri), tokens(), stream(original) {
    tokenize();
//    for (std::list<Token>::const_iterator it = tokens.cbegin(); it != tokens.cend(); ++it) {
//        std::cout << it->type << ", "
//                  << it->startPos << " until " << it->endPos << ", "
//                  << "Content: '" << it->str << "'" << std::endl;
//    }
}

URI::URI(const URI &other): original(other.original), tokens(other.tokens), stream(original) {}

URI::URI(): original(), tokens() {}

URI::~URI() {}

bool URI::isCGIIdentifier() const {
    std::string cgiFile = determineFileWithExtension();
    return false;
}

std::string URI::determineFileWithExtension() const {
    return std::string();
}

void URI::tokenize() {
    while (!stream.eof()) {
        tokens.push_back(nextToken());
    }
}
URI::Token URI::nextToken() {
    unsigned long pos = stream.tellg();
    switch (stream.peek()) {
        case '/': return Token(static_cast<char>(stream.get()), Token::SLASH, pos, stream.tellg());
        case '?': return Token(static_cast<char>(stream.get()), Token::QUESTION, pos, stream.tellg());
        case '&': return Token(static_cast<char>(stream.get()), Token::AND, pos, stream.tellg());
        case '=': return Token(static_cast<char>(stream.get()), Token::EQUAL, pos, stream.tellg());
    }
    std::string buffer;
    while (!stream.eof() && !isSpecial()) {
        buffer += static_cast<char>(stream.get());
    }
    return Token(buffer, Token::TEXT, pos, (stream.eof() ? static_cast<unsigned long>(pos + buffer.size())
                                                                                        : static_cast<unsigned long>(stream.tellg())));
}

bool URI::isSpecial() {
    const char tmp = static_cast<char>(stream.peek());
    return !(isalpha(tmp) || tmp == '.' || tmp == '%' || tmp == '-');
}

URI &URI::operator=(const URI &other) {
    original = other.original;
    tokens = (other.tokens);
    stream.setf(EOF);
    return *this;
}


// U R I : : T O K E N   I M P L E M E N T A T I O N

URI::Token::~Token() {}

URI::Token::Token(const std::string &content, const URI::Token::Type &type, unsigned long startPos, unsigned long endPos):
        str(content), startPos(startPos), endPos(endPos), type(type) {}

URI::Token::Token(char c, const URI::Token::Type &type, unsigned long startPos, unsigned long endPos):
        str(std::string() += c), startPos(startPos), endPos(endPos), type(type) {}

URI::Token::Token(const URI::Token &other):
        str(other.str), startPos(other.startPos), endPos(other.endPos), type(other.type) {}

URI::Token &URI::Token::operator=(const URI::Token &other) {
    str = other.str;
    startPos = other.startPos;
    endPos = other.endPos;
    type = other.type;
    return *this;
}
