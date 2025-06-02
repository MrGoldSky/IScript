#include "lexer.h"

#include <cctype>
#include <cstdlib>
#include <string>

void Lexer::rawAdvance() {
    int c = input.get();
    if (c == '\n') {
        ++line;
    }
}

char Lexer::advance() {
    char c = input.get();
    if (c == '\n') {
        ++line;
    }
    currentLexeme.push_back(c);
    return c;
}

char Lexer::peek() const {
    int c = input.peek();
    return c == EOF ? '\0' : static_cast<char>(c);
}

char Lexer::peekNext() const {
    int first = input.peek();
    if (first == EOF) return '\0';
    char c = input.get();
    int next = input.peek();
    input.unget();
    return next == EOF ? '\0' : static_cast<char>(next);
}

bool Lexer::match(char expected) {
    if (peek() != expected) return false;
    advance();
    return true;
}

bool Lexer::isAtEnd() const {
    return peek() == '\0';
}

void Lexer::skipWhitespace() {
    while (true) {
        char c = peek();
        char c2 = peekNext();
        if (c == '/' && c2 == '/') {
            rawAdvance();
            rawAdvance();
            while (peek() != '\n' && !isAtEnd()) {
                rawAdvance();
            }
        } else if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            rawAdvance();
        } else {
            break;
        }
    }
}

Token Lexer::nextToken() {
    currentLexeme.clear();
    skipWhitespace();
    start = current;

    if (isAtEnd()) {
        return makeToken(TokenType::EndOfFile);
    }

    char c = peek();
    if (std::isdigit(c)) {
        return scanNumber();
    }
    if (std::isalpha(c) || c == '_') {
        return scanIdentifier();
    }
    if (c == '"') {
        return scanString();
    }
    return scanOperator();
}

Token Lexer::scanNumber() {
    while (std::isdigit(peek())) {
        advance();
    }
    if (peek() == '.' && std::isdigit(peekNext())) {
        advance();
        while (std::isdigit(peek())) {
            advance();
        }
    }

    //  Обработка экспоненциальной части: e или E, потом [+|-] и степень
    if (peek() == 'e' || peek() == 'E') {
        advance();
        if (peek() == '+' || peek() == '-') {
            advance();
        }
        if (!std::isdigit(peek())) {
            return errorToken("Malformed number literal: expected digits after exponent");
        }
        while (std::isdigit(peek())) {
            advance();
        }
    }

    double value = std::strtod(currentLexeme.c_str(), nullptr);
    Token token = makeToken(TokenType::Number);
    token.literal = value;
    return token;
}

Token Lexer::scanString() {
    advance();
    while (!isAtEnd()) {
        if (peek() == '\\') {
            advance();
            if (!isAtEnd())
                advance();
        } else if (peek() == '"') {
            break;
        } else {
            advance();
        }
    }
    if (isAtEnd()) {
        return errorToken("Unclosed string");
    }
    advance();

    std::string raw = currentLexeme.substr(1, currentLexeme.size() - 2);
    std::string text;
    text.reserve(raw.size());
    for (size_t i = 0; i < raw.size(); ++i) {
        if (raw[i] == '\\' && i + 1 < raw.size()) {
            char next = raw[++i];
            switch (next) {
                case 'n':
                    text.push_back('\n');
                    break;
                case 't':
                    text.push_back('\t');
                    break;
                case '\\':
                    text.push_back('\\');
                    break;
                case '"':
                    text.push_back('"');
                    break;
                default:
                    text.push_back(next);
                    break;
            }
        } else {
            text.push_back(raw[i]);
        }
    }
    Token token = makeToken(TokenType::String);
    token.literal = text;
    return token;
}

Token Lexer::scanIdentifier() {
    while (std::isalnum(peek()) || peek() == '_') {
        advance();
    }
    auto it = keywords.find(currentLexeme);
    if (it != keywords.end()) {
        TokenType type = it->second;
        Token token = makeToken(type);
        if (type == TokenType::Boolean) {
            token.literal = (currentLexeme == "true");
        }
        return token;
    }
    return makeToken(TokenType::Identifier);
}

Token Lexer::scanOperator() {
    advance();
    TokenType type;
    switch (currentLexeme[0]) {
        case '+':
            if (match('+'))
                type = TokenType::PlusPlus;
            else if (match('='))
                type = TokenType::PlusAssign;
            else
                type = TokenType::Plus;
            break;
        case '-':
            if (match('-'))
                type = TokenType::MinusMinus;
            else if (match('='))
                type = TokenType::MinusAssign;
            else
                type = TokenType::Minus;
            break;
        case '*':
            if (match('='))
                type = TokenType::StarAssign;
            else
                type = TokenType::Star;
            break;
        case '/':
            if (match('='))
                type = TokenType::SlashAssign;
            else
                type = TokenType::Slash;
            break;
        case '%':
            if (match('='))
                type = TokenType::PercentAssign;
            else
                type = TokenType::Percent;
            break;
        case '^':
            if (match('='))
                type = TokenType::CaretAssign;
            else
                type = TokenType::Caret;
            break;
        case '=':
            if (match('='))
                type = TokenType::Equal;
            else
                type = TokenType::Assign;
            break;
        case '!':
            if (match('='))
                type = TokenType::NotEqual;
            else
                type = TokenType::Bang;
            break;
        case '<':
            if (match('='))
                type = TokenType::LessEqual;
            else
                type = TokenType::Less;
            break;
        case '>':
            if (match('='))
                type = TokenType::GreaterEqual;
            else
                type = TokenType::Greater;
            break;
        case '(':
            type = TokenType::LParen;
            break;
        case ')':
            type = TokenType::RParen;
            break;
        case '[':
            type = TokenType::LBracket;
            break;
        case ']':
            type = TokenType::RBracket;
            break;
        case ',':
            type = TokenType::Comma;
            break;
        case ';':
            type = TokenType::Semicolon;
            break;
        case '@':
            type = TokenType::At;
            break;
        case ':':
            type = TokenType::Colon;
            break;
        default:
            return errorToken("Unknown operator");
    }
    return makeToken(type);
}

Token Lexer::makeToken(TokenType type) const {
    return Token{type, currentLexeme, {}, line};
}

Token Lexer::errorToken(const std::string& message) const {
    return Token{TokenType::EndOfFile, currentLexeme.empty() ? message : currentLexeme, std::string(message), line};
}