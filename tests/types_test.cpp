#include <gtest/gtest.h>
#include <lib/interpreter.h>

TEST(TypesTestSuite, IntTest) {
    std::string code = R"(
        x = 1
        y = 2
        z = 3 * x + y
        print(z)
    )";

    std::string expected = "5";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(TypesTestSuite, MultiTest) {
    std::string code = "print(-1 + (3) - (-((4) + 2) + 1))";

    std::string expected = "7";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(TypesTestSuite, UnaryMinusTest) {
    std::string code = R"(
        print(-5)
    )";
    std::string expected = "-5";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

// Модуль и возведение в степень
TEST(TypesTestSuite, ModPowTest) {
    std::string code = R"(
        print(10 % 3)
        print(2 ^ 5)
    )";
    // 10 % 3 = 1, 2^5 = 32 -> "132"
    std::string expected = "132";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

// Операторы присваивания: +=, *=, -=, /=
TEST(TypesTestSuite, AssignmentOperatorsTest) {
    std::string code = R"(
        x = 5
        x += 3    // 8
        x *= 2    // 16
        x -= 4    // 12
        x /= 2    // 6
        print(x)
    )";
    std::string expected = "6";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

// Сравнения: <, ==, >
TEST(TypesTestSuite, ComparisonTest) {
    std::string code = R"(
        print(1 < 2)
        print(2 == 2)
        print(3 > 4)
    )";
    // 1<2 -> true, 2==2 -> true, 3>4 -> false
    std::string expected = "truetruefalse";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

// Логические: and, not
TEST(TypesTestSuite, LogicalTest) {
    std::string code = R"(
        print(true and false)
        print(not false)
    )";
    // true and false -> false, not false -> true
    std::string expected = "falsetrue";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

// Строковые операции: конкатенация, вычитание суффикса, повторение, индекс и срез
TEST(TypesTestSuite, StringOperationsTest) {
    std::string code = R"(
        s = "hello"
        s += " world"         // "hello world"
        print(s)
        print("hello" - "lo") // "hel"
        print("ab" * 3)       // "ababab"
        print(s[1])           // 'e'
        print(s[1:4])         // "ell"
    )";
    std::string expected = "hello worldhelabababeell";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(TypesTestSuite, ListIndexSliceTest) {
    std::string code = R"(
        print([10,20,30][1])   // 20
        print([10,20,30][0:2]) // [10, 20]
    )";
    std::string expected = "20[10, 20]";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(TypesTestSuite, StringEscapeQuotes) {
    std::string code = R"(
        s = "Some \"string\" type"
        print(s)
    )";
    std::string expected = R"(Some "string" type)";

    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(TypesTestSuite, StringEscapeNewlineAndTab) {
    std::string code = R"(
        s = "line1\n\tline2"
        print(s)
    )";
    std::string expected = "line1\n\tline2";

    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
