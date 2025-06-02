#include "lexer.h"

#include <gtest/gtest.h>

#include <sstream>
#include <variant>

#include "interpreter.h"

static std::vector<Token> lexAll(const std::string& source) {
    std::istringstream input(source);
    Lexer lexer(input);
    std::vector<Token> tokens;
    while (true) {
        Token tok = lexer.nextToken();
        std::cerr
            << "lexeme=\"" << tok.lexeme << "\""
            << "  type=" << static_cast<int>(tok.type)
            << std::endl;
        tokens.push_back(tok);
        if (tok.type == TokenType::EndOfFile) break;
    }
    return tokens;
}

TEST(LexerTestSuite, NumberToken) {
    auto tokens = lexAll("123 45.67");
    ASSERT_GE(tokens.size(), 3);
    EXPECT_EQ(tokens[0].type, TokenType::Number);
    EXPECT_DOUBLE_EQ(std::get<double>(tokens[0].literal), 123.0);
    EXPECT_EQ(tokens[0].lexeme, "123");
    EXPECT_EQ(tokens[1].type, TokenType::Number);
    EXPECT_DOUBLE_EQ(std::get<double>(tokens[1].literal), 45.67);
    EXPECT_EQ(tokens[1].lexeme, "45.67");
    EXPECT_EQ(tokens[2].type, TokenType::EndOfFile);
}

TEST(LexerTestSuite, StringLiteral) {
    auto tokens = lexAll("\"hello world\"");
    ASSERT_GE(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::String);
    EXPECT_EQ(std::get<std::string>(tokens[0].literal), "hello world");
    EXPECT_EQ(tokens[0].lexeme, "\"hello world\"");
    EXPECT_EQ(tokens[1].type, TokenType::EndOfFile);
}

TEST(LexerTestSuite, BooleanLiteral) {
    auto tokens = lexAll("true false");
    ASSERT_GE(tokens.size(), 3);
    EXPECT_EQ(tokens[0].type, TokenType::Boolean);
    EXPECT_TRUE(std::get<bool>(tokens[0].literal));
    EXPECT_EQ(tokens[0].lexeme, "true");
    EXPECT_EQ(tokens[1].type, TokenType::Boolean);
    EXPECT_FALSE(std::get<bool>(tokens[1].literal));
    EXPECT_EQ(tokens[1].lexeme, "false");
    EXPECT_EQ(tokens[2].type, TokenType::EndOfFile);
}

TEST(LexerTestSuite, IdentifierAndKeyword) {
    auto tokens = lexAll("if foo else while my_var");
    ASSERT_GE(tokens.size(), 5);
    EXPECT_EQ(tokens[0].type, TokenType::If);
    EXPECT_EQ(tokens[0].lexeme, "if");
    EXPECT_EQ(tokens[1].type, TokenType::Identifier);
    EXPECT_EQ(tokens[1].lexeme, "foo");
    EXPECT_EQ(tokens[2].type, TokenType::Else);
    EXPECT_EQ(tokens[2].lexeme, "else");
    EXPECT_EQ(tokens[3].type, TokenType::While);
    EXPECT_EQ(tokens[3].lexeme, "while");
    EXPECT_EQ(tokens[4].type, TokenType::Identifier);
    EXPECT_EQ(tokens[4].lexeme, "my_var");
}

TEST(LexerTestSuite, OperatorsAndDelimiters) {
    const std::vector<std::pair<std::string, TokenType>> tests = {
        {"+", TokenType::Plus},
        {"-", TokenType::Minus},
        {"*", TokenType::Star},
        {"/", TokenType::Slash},
        {"%", TokenType::Percent},
        {"^", TokenType::Caret},
        {"=", TokenType::Assign},
        {"==", TokenType::Equal},
        {"!=", TokenType::NotEqual},
        {"<=", TokenType::LessEqual},
        {">=", TokenType::GreaterEqual},
        {"<", TokenType::Less},
        {">", TokenType::Greater},
        {"+=", TokenType::PlusAssign},
        {"-=", TokenType::MinusAssign},
        {"*=", TokenType::StarAssign},
        {"/=", TokenType::SlashAssign},
        {"%=", TokenType::PercentAssign},
        {"^=", TokenType::CaretAssign},
        {"++", TokenType::PlusPlus},
        {"--", TokenType::MinusMinus},
        {"(", TokenType::LParen},
        {")", TokenType::RParen},
        {"[", TokenType::LBracket},
        {"]", TokenType::RBracket},
        {",", TokenType::Comma},
        {";", TokenType::Semicolon},
        {"@", TokenType::At}};
    std::string source;
    for (auto& p : tests) source += p.first + " ";
    auto tokens = lexAll(source);
    ASSERT_GE(tokens.size(), tests.size() + 1);
    for (size_t i = 0; i < tests.size(); ++i) {
        EXPECT_EQ(tokens[i].type, tests[i].second) << "Token " << tests[i].first;
        EXPECT_EQ(tokens[i].lexeme, tests[i].first);
    }
    EXPECT_EQ(tokens[tests.size()].type, TokenType::EndOfFile);
}

TEST(LexerTestSuite, CommentSkipping) {
    auto tokens = lexAll("// this is a comment\n123");
    ASSERT_GE(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::Number);
    EXPECT_DOUBLE_EQ(std::get<double>(tokens[0].literal), 123.0);
    EXPECT_EQ(tokens[0].lexeme, "123");
    EXPECT_EQ(tokens[1].type, TokenType::EndOfFile);
}

TEST(NumberExponentialSuite, PositiveExponent) {
    std::string program =
        "x = 1e3\n"
        "print(x)\n";
    std::istringstream input(program);

    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), "1000");
}

TEST(NumberExponentialSuite, NegativeExponent) {
    std::string program =
        "y = 1.5e-2\n"
        "print(y)\n";
    std::istringstream input(program);

    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), "0.015");
}

TEST(NumberExponentialSuite, UppercaseAndPlusSign) {
    std::string program =
        "a = 2E+2\n"
        "print(a)\n";
    std::istringstream input(program);

    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), "200");
}

TEST(NumberExponentialSuite, ArithmeticCombination) {
    std::string program =
        "z = 1e2 + 2.5E1\n"
        "print(z)\n";
    std::istringstream input(program);

    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), "125");
}

TEST(NumberExponentialSuite, ZeroExponent) {
    std::string program =
        "b = 3.14e0\n"
        "print(b)\n";
    std::istringstream input(program);

    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), "3.14");
}

TEST(NumberExponentialSuite, InvalidExponentialLiteral) {
    std::string program =
        "c = 1e\n"
        "print(239)\n";
    std::istringstream input(program);

    std::ostringstream output;
    ASSERT_FALSE(interpret(input, output));
    ASSERT_FALSE(output.str().ends_with("239"));
}