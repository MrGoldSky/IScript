#pragma once
#include <string>
#include <unordered_map>

#include "token.h"

static const std::unordered_map<std::string, TokenType> keywords = {
    {"if", TokenType::If},
    {"then", TokenType::Then},
    {"else", TokenType::Else},
    {"end", TokenType::End},

    {"while", TokenType::While},
    {"for", TokenType::For},
    {"in", TokenType::In},
    {"break", TokenType::Break},
    {"continue", TokenType::Continue},

    {"function", TokenType::Function},
    {"return", TokenType::Return},

    {"true", TokenType::Boolean},
    {"false", TokenType::Boolean},
    {"and", TokenType::And},
    {"or", TokenType::Or},
    {"not", TokenType::Not},
    {"nil", TokenType::Nil},
};
