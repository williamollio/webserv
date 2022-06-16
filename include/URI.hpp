//
// Created by Manuel Hahn on 6/14/22.
//

#ifndef WEBSERV_URI_HPP
#define WEBSERV_URI_HPP

#include <string>
#include <list>
#include <sstream>
#include <map>

class URISyntaxException;

class URI {
public:
    URI();
    explicit URI(const std::string &);
    URI(const URI &);
    ~URI();

    URI & operator=(const URI &);

    bool                               isCGIIdentifier() const;
    std::map<std::string, std::string> getVars() const;

    class Token {
    public:
        enum Type {
            SLASH, QUESTION, EQUAL, TEXT, AND, END
        };
        Token(const std::string &, const Token::Type &, unsigned long startPos, unsigned long endPos);
        Token(char, const Token::Type &, unsigned long startPos, unsigned long endPos);
        Token(const Token &);
        ~Token();

        Token & operator=(const Token &);

        const std::string & getContent() const;
        unsigned long       getStartPos() const;
        unsigned long       getEndPos() const;
        Type                getType() const;

    private:
        std::string   str;
        unsigned long startPos;
        unsigned long endPos;
        Token::Type   type;
    };

private:

    std::string           original;
    std::list<URI::Token> tokens;
    std::stringstream     stream;

    std::string        determineFileWithExtension() const;
    void               tokenize();
    URI::Token         nextToken();
    static bool        hasExtension(const std::string &);
    static bool        isCleanString(const std::string &, unsigned long pos);
    static inline void expect(Token::Type, const Token &);
    static inline bool ensureTokenIs(Token::Type, const Token &);
    static inline bool isSpecial(char c);
    static inline bool isPathType(URI::Token::Type);
};


#endif //WEBSERV_URI_HPP
