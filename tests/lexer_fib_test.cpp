#include <gtest/gtest.h>

#include <sstream>
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

TEST(LexerInlineTest, FibonacciScript) {
    std::string code = R"(
        fib = function(n)
            if n == 0 then
                return 0
            end if

            a = 0
            b = 1

            for i in range(n - 1)
                c = a + b
                a = b
                b = c
            end for

            return b
        end function

        print(fib(10))
    )";

    auto tokens = lexAll(code);

    ASSERT_GE(tokens.size(), 6u);
    EXPECT_EQ(tokens[0].lexeme, "fib");
    EXPECT_EQ(tokens[0].type, TokenType::Identifier);

    EXPECT_EQ(tokens[1].lexeme, "=");
    EXPECT_EQ(tokens[1].type, TokenType::Assign);

    EXPECT_EQ(tokens[2].lexeme, "function");
    EXPECT_EQ(tokens[2].type, TokenType::Function);

    EXPECT_EQ(tokens[3].lexeme, "(");
    EXPECT_EQ(tokens[3].type, TokenType::LParen);

    EXPECT_EQ(tokens[4].lexeme, "n");
    EXPECT_EQ(tokens[4].type, TokenType::Identifier);

    EXPECT_EQ(tokens[5].lexeme, ")");
    EXPECT_EQ(tokens[5].type, TokenType::RParen);

    bool sawIfZero = false;
    for (size_t i = 6; i + 3 < tokens.size(); ++i) {
        if (tokens[i].type == TokenType::If &&
            tokens[i + 1].lexeme == "n" && tokens[i + 1].type == TokenType::Identifier &&
            tokens[i + 2].lexeme == "==" && tokens[i + 2].type == TokenType::Equal &&
            tokens[i + 3].lexeme == "0" && tokens[i + 3].type == TokenType::Number) {
            sawIfZero = true;
            break;
        }
    }
    EXPECT_TRUE(sawIfZero) << "Expected to find `if n == 0`";

    bool sawPrintFib10 = false;
    for (size_t i = 0; i + 6 < tokens.size(); ++i) {
        if (tokens[i].lexeme == "print" && tokens[i].type == TokenType::Identifier &&
            tokens[i + 1].type == TokenType::LParen &&
            tokens[i + 2].lexeme == "fib" && tokens[i + 2].type == TokenType::Identifier &&
            tokens[i + 3].type == TokenType::LParen &&
            tokens[i + 4].type == TokenType::Number &&
            tokens[i + 5].type == TokenType::RParen &&
            tokens[i + 6].type == TokenType::RParen) {
            sawPrintFib10 = true;
            break;
        }
    }
    EXPECT_TRUE(sawPrintFib10) << "Expected to find `print(fib(10))`";

    EXPECT_EQ(tokens.back().type, TokenType::EndOfFile);
}
