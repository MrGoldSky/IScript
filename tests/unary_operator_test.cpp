// tests/lexer_operator_test.cpp

#include <gtest/gtest.h>

#include <sstream>
#include <string>
#include <vector>

#include "lexer.h"
#include "token.h"

static std::vector<Token> lexAll(const std::string& source) {
    std::istringstream input(source);
    Lexer lexer(input);
    std::vector<Token> tokens;
    while (true) {
        Token tok = lexer.nextToken();
        tokens.push_back(tok);
        if (tok.type == TokenType::EndOfFile) break;
    }
    return tokens;
}

TEST(LexerOperatorTest, SingleCharOperators) {
    // пробелы между, чтобы каждый оператор лексировался отдельно
    auto tokens = lexAll("+ - * / % ^ = < > ! ( ) [ ] , ; @");
    std::vector<TokenType> expected = {
        TokenType::Plus,
        TokenType::Minus,
        TokenType::Star,
        TokenType::Slash,
        TokenType::Percent,
        TokenType::Caret,
        TokenType::Assign,
        TokenType::Less,
        TokenType::Greater,
        TokenType::Bang,
        TokenType::LParen,
        TokenType::RParen,
        TokenType::LBracket,
        TokenType::RBracket,
        TokenType::Comma,
        TokenType::Semicolon,
        TokenType::At,
        TokenType::EndOfFile};
    ASSERT_EQ(tokens.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected[i]) << "at index " << i;
    }
}

TEST(LexerOperatorTest, CompoundOperators) {
    struct Case {
        std::string src;
        TokenType type;
    };
    std::vector<Case> cases = {
        {"++", TokenType::PlusPlus},
        {"--", TokenType::MinusMinus},
        {"+=", TokenType::PlusAssign},
        {"-=", TokenType::MinusAssign},
        {"*=", TokenType::StarAssign},
        {"/=", TokenType::SlashAssign},
        {"%=", TokenType::PercentAssign},
        {"^=", TokenType::CaretAssign},
        {"==", TokenType::Equal},
        {"!=", TokenType::NotEqual},
        {"<=", TokenType::LessEqual},
        {">=", TokenType::GreaterEqual},
    };

    for (auto& c : cases) {
        auto tokens = lexAll(c.src);
        ASSERT_GE(tokens.size(), 2) << "lexAll returned too few tokens for \"" << c.src << "\"";
        EXPECT_EQ(tokens[0].type, c.type) << "wrong token type for \"" << c.src << "\"";
        EXPECT_EQ(tokens[0].lexeme, c.src) << "wrong lexeme for \"" << c.src << "\"";
        EXPECT_EQ(tokens[1].type, TokenType::EndOfFile);
    }
}

TEST(LexerOperatorTest, MixedPlusMinus) {
    // проверяем, что "+-" лексируется как два отдельных оператора
    auto tokens = lexAll("+-");
    ASSERT_GE(tokens.size(), 3);
    EXPECT_EQ(tokens[0].type, TokenType::Plus);
    EXPECT_EQ(tokens[0].lexeme, "+");
    EXPECT_EQ(tokens[1].type, TokenType::Minus);
    EXPECT_EQ(tokens[1].lexeme, "-");
    EXPECT_EQ(tokens[2].type, TokenType::EndOfFile);
}
