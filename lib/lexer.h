#pragma once
#include <istream>

#include "keywords.h"
#include "token.h"

class Lexer {
   public:
    explicit Lexer(std::istream& input_) : input(input_), line(1) {
    }

    Token nextToken();
    bool isAtEnd() const;

    char advance();
    void rawAdvance();
    char peek() const;
    char peekNext() const;
    bool match(char expected);
    void skipWhitespace();

    Token scanNumber();
    Token scanString();
    Token scanIdentifier();
    Token scanOperator();
    Token makeToken(TokenType type) const;
    Token errorToken(const std::string& msg) const;

   private:
    std::istream& input;
    size_t start = 0, current = 0;
    std::string currentLexeme;
    int line = 1;
};