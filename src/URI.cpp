//
// Created by Manuel Hahn on 6/14/22.
//

#include "URI.hpp"
#include "URISyntaxException.hpp"

URI::URI(const std::string & uri): original(uri), tokens(), stream(original), syntaxThrowing(false) {
    tokenize();
}

URI::URI(const URI & other): original(other.original), tokens(), stream(original), syntaxThrowing(other.isSyntaxExceptionEnabled()) {
    tokenize();
}

URI::URI(): original(), tokens(), syntaxThrowing(false) {}

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
    const std::string & tmp = determineFile();
    return hasExtension(tmp) ? tmp : std::string();
}

std::string URI::determineFile() const {
    std::stringstream buffer;
    for (std::list<Token>::const_iterator it = tokens.begin();
         it != tokens.end() && isPathType(it->getType());
         ++it) {
        const std::string & content = it->getContent();
        buffer << content;
        if (hasExtension(content)) break;
    }
    return buffer.str();
}

void URI::tokenize() {
    Token t("", Token::END, 0, 0);
    do {
        t = nextToken();
        tokens.push_back(t);
    } while (t.getType() != Token::END);
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
    if (buffer.empty()) return Token("", Token::END, pos, pos + 1);
    return Token(buffer, Token::TEXT, pos, (stream.eof() ? static_cast<unsigned long>(pos + buffer.size())
                                                         : static_cast<unsigned long>(stream.tellg())));
}

bool URI::isSpecial(char c) {
    return !(isalnum(c) || c == '.' || c == '%' || c == '-' || c =='_');
}

bool URI::isPathType(URI::Token::Type type) {
    return type == Token::SLASH || type == Token::TEXT;
}

bool URI::hasExtension(const std::string & str) {
    const unsigned long pos = str.rfind('.');
    return pos != std::string::npos && pos != str.size() - 1 && isCleanString(str, pos);
}

bool URI::isCleanString(const std::string & str, unsigned long pos) {
    for (std::string::const_iterator it = str.begin() + static_cast<long>(pos);
         it != str.end();
         ++it) {
        if (isSpecial(*it)) return false;
    }
    return true;
}

std::map<std::string, std::string> URI::getVars() const {
    std::map<std::string, std::string> vars;
    std::list<Token>::const_iterator it;
    for (it = tokens.begin();
         it != tokens.end() && isPathType(it->getType());
         ++it) {
        if (hasExtension(it->getContent())) break;
    }
    std::stringstream request;
    for (++it; it != tokens.end() && it->getType() != Token::QUESTION; ++it) {
        request << it->getContent();
    }
    vars["REQUEST"] = request.str();
    do {
        ++it;
        if (!expect(Token::TEXT, *it)) {
            skipToNext(Token::AND, it);
            continue;
        }
        std::string name = it->getContent();
        if (!expect(Token::EQUAL, *(++it))) {
            skipToNext(Token::AND, it);
            continue;
        }
        if (!expect(Token::TEXT, *(++it))) {
            skipToNext(Token::AND, it);
            continue;
        }
        vars[name] = it->getContent();
        ++it;
    } while (it->getType() != Token::END && ensureTokenIs(Token::AND, *it));
    return vars;
}

bool URI::expect(URI::Token::Type type, const Token & token) const {
    const bool ret = token.getType() == type;
    if (!ret && syntaxThrowing) throw URISyntaxException(token, "Expected '"
                                                               + Token::tokenTypeString(type)
                                                               + "', got '"
                                                               + Token::tokenTypeString(token.getType())
                                                               + "'!");
    return ret;
}

bool URI::ensureTokenIs(URI::Token::Type type, const Token & token) const {
    expect(type, token);
    return true;
}

bool URI::isSyntaxExceptionEnabled() const {
    return syntaxThrowing;
}

void URI::setSyntaxExceptionEnabled(bool enabled) {
    syntaxThrowing = enabled;
}

void URI::skipToNext(URI::Token::Type type, std::list<Token>::const_iterator iterator) {
    for (; iterator->getType() != Token::END || iterator->getType() == type; ++iterator);
}

std::string URI::getFile() const {
    return determineFile();
}

std::string URI::getQuery() const {
    std::list<Token>::const_iterator it;
    for (it = tokens.begin(); it->getType() != Token::END && it->getType() != Token::QUESTION; ++it);
    std::list<Token>::const_iterator tmp = it;
    ++tmp;
    if (it->getType() == Token::END || tmp->getType() == Token::END) return "";
    std::string ret;
    for (; it->getType() != Token::END; ++it) {
        ret += it->getContent();
    }
    return ret;
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

std::string URI::Token::tokenTypeString(URI::Token::Type type) {
    switch (type) {
        case SLASH:    return "Slash ('/')";
        case QUESTION: return "Question-mark ('?')";
        case EQUAL:    return "Equal-sign ('=')";
        case TEXT:     return "Literal";
        case AND:      return "Ampersand ('&')";
        case END:      return "End of string";
        default:
            return "Default";
    }
}
