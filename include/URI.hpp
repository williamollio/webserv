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
    bool                               isSyntaxExceptionEnabled() const;
    void                               setSyntaxExceptionEnabled(bool);
    std::string                        getFile() const;
    std::map<std::string, std::string> getVars() const;
    std::string                        getQuery() const;
    std::string                        getPathInfo() const;

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
        static std::string  tokenTypeString(Type);

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
    bool                  syntaxThrowing;

    std::string        determineFileWithExtension() const;
    std::string        determineFile() const;
    void               tokenize();
    URI::Token         nextToken();
    inline bool        ensureTokenIs(Token::Type, const Token &) const;
    inline bool        expect(Token::Type, const Token &) const;
    inline bool        isInCGIPath() const;
    static bool        hasExtension(const std::string &);
    static bool        isCleanString(const std::string &, unsigned long pos);
    static inline bool isSpecial(char c);
    static inline bool isPathType(URI::Token::Type);
    static inline void skipToNext(Token::Type, std::list<Token>::const_iterator);
};


#endif //WEBSERV_URI_HPP
