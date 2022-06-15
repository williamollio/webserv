//
// Created by Manuel Hahn on 6/14/22.
//

#ifndef WEBSERV_URI_HPP
#define WEBSERV_URI_HPP

#include <string>
#include <list>
#include <sstream>

class URI {
public:
    URI();
    URI(const std::string &);
    URI(const URI &);
    ~URI();

    URI & operator=(const URI &);

    bool isCGIIdentifier() const;

private:
    class Token {
    public:
        enum Type {
            SLASH, QUESTION, EQUAL, TEXT, AND
        };
        Token(const std::string &, const Token::Type &, unsigned long startPos, unsigned long endPos);
        Token(char, const Token::Type &, unsigned long startPos, unsigned long endPos);
        Token(const Token &);
        ~Token();

        Token & operator=(const Token &);

    public:
        std::string   str;
        unsigned long startPos;
        unsigned long endPos;
        Token::Type   type;
    };

    std::string original;
    std::list<URI::Token> tokens;
    std::stringstream stream;

    std::string determineFileWithExtension() const;
    void tokenize();
    URI::Token nextToken();
    bool isSpecial();
};


#endif //WEBSERV_URI_HPP
