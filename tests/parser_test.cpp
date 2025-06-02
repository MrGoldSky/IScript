#include "parser.h"

#include <gtest/gtest.h>

#include "lexer.h"

// Вспомогательная функция: пытается распарсить последовательность топ-левел выражений.
// Возвращает true, если parseModule вернул true и количество разобранных функций совпало с expectedCount.
static bool tryParse(const std::string& code, size_t expectedCount) {
    std::istringstream in(code);
    Lexer lexer(in);
    Parser parser(lexer);
    std::vector<std::unique_ptr<FunctionAST>> functions;
    if (!parser.parseModule(functions)) return false;
    EXPECT_EQ(functions.size(), expectedCount);
    return true;
}

TEST(ParserTestSuite, SimpleNumber) {
    EXPECT_TRUE(tryParse("123", 1));
}

TEST(ParserTestSuite, UnaryPlus) {
    EXPECT_TRUE(tryParse("+123", 1));
}

TEST(ParserTestSuite, UnaryMinus) {
    EXPECT_TRUE(tryParse("-123", 1));
}

TEST(ParserTestSuite, NestedUnary) {
    EXPECT_TRUE(tryParse("+-+ -5", 1));
}

TEST(ParserTestSuite, UnaryBinaryMix) {
    EXPECT_TRUE(tryParse("-3+4", 1));
}

TEST(ParserTestSuite, ComplexExpression) {
    EXPECT_TRUE(tryParse("(1+2)*(-1)", 1));
}

TEST(ParserTestSuite, MultipleTopLevelExpressions) {
    EXPECT_TRUE(tryParse("1 2 3", 3));
}

TEST(ParserTestSuite, InvalidExpression) {
    std::istringstream in("+");
    Lexer lex(in);
    Parser parser(lex);
    std::vector<std::unique_ptr<FunctionAST>> functions;
    EXPECT_FALSE(parser.parseModule(functions));
}
