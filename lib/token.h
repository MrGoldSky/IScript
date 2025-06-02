#pragma once

#include <string>
#include <variant>

enum class TokenType {
    // end of input
    EndOfFile,

    // Типы
    Identifier,
    Number,
    Boolean,
    String,

    Colon,  // :

    // sоператоры
    Plus,     // +
    Minus,    // -
    Star,     // *
    Slash,    // /
    Percent,  // %
    Caret,    // ^

    Assign,   // =
    Bang,     // !
    Less,     // <
    Greater,  // >

    // Составные операторы
    PlusPlus,       // ++
    MinusMinus,     // --
    PlusAssign,     // +=
    MinusAssign,    // -=
    StarAssign,     // *=
    SlashAssign,    // /=
    PercentAssign,  // %=
    CaretAssign,    // ^=

    Equal,         // ==
    NotEqual,      // !=
    LessEqual,     // <=
    GreaterEqual,  // >=

    // Разделители
    LParen,     // (
    RParen,     // )
    LBracket,   // [
    RBracket,   // ]
    Comma,      // ,
    Semicolon,  // ;
    At,         // @
    In,

    // Ключевые слова
    If,
    Then,
    Else,
    End,

    While,
    For,
    Break,
    Continue,

    Function,
    Return,
    And,
    Not,
    Or,
    Nil,
};

using Literal = std::variant<std::monostate, bool, double, std::string>;

struct Token {
    TokenType type;
    std::string lexeme;
    Literal literal;
    int line;
};
