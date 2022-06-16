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

URI::URI(const URI &other): original(other.original), tokens(), stream(original) {
    tokenize();
}

URI::URI(): original(), tokens() {}

URI::~URI() {}

bool URI::isCGIIdentifier() const {
    std::string cgiFile = determineFileWithExtension();
    if (!cgiFile.empty()) {
        // TODO Ask configuration for CGI file extensions
        return true;
    } else {
        return false;
    }
}

std::string URI::determineFileWithExtension() const {
    std::stringstream buffer;
    for (std::list<Token>::const_iterator it = tokens.cbegin();
         it != tokens.cend() && isPathType(it->getType());
         ++it) {
        const std::string & content = it->getContent();
        buffer << content;
        if (hasExtension(content)) break;
    }
    const std::string & tmp = buffer.str();
    return hasExtension(tmp) ? tmp : std::string();
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
    while (!stream.eof() && !isSpecial(static_cast<char>(stream.peek()))) {
        buffer += static_cast<char>(stream.get());
    }
    return Token(buffer, Token::TEXT, pos, (stream.eof() ? static_cast<unsigned long>(pos + buffer.size())
                                                                                        : static_cast<unsigned long>(stream.tellg())));
}

bool URI::isSpecial(char c) {
    return !(isalpha(c) || c == '.' || c == '%' || c == '-' || c =='_');
}

bool URI::isPathType(URI::Token::Type type) {
    return type == Token::SLASH || type == Token::TEXT;
}

bool URI::hasExtension(const std::string & str) {
    const unsigned long pos = str.rfind('.');
    return pos != std::string::npos && pos != str.size() - 1 && isCleanString(str, pos);
}

bool URI::isCleanString(const std::string & str, unsigned long pos) {
    for (std::string::const_iterator it = str.cbegin() + static_cast<long>(pos);
         it != str.cend();
         ++it) {
        if (isSpecial(*it)) return false;
    }
    return true;
}

URI &URI::operator=(const URI &other) {
    original = other.original;
    tokens = other.tokens;
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

const std::string & URI::Token::getContent() const {
    return str;
}

unsigned long URI::Token::getStartPos() const {
    return startPos;
}

unsigned long URI::Token::getEndPos() const {
    return endPos;
}

URI::Token::Type URI::Token::getType() const {
    return type;
}
