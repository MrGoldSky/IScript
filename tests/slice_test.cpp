#include <gtest/gtest.h>
#include <lib/interpreter.h>

TEST(TypesTestSuite, StringSliceEdgeCases) {
    std::string code = R"(
        s = "abcdef"
        print(s[:])       // "abcdef"
        print(s[:3])      // "abc"
        print(s[3:])      // "def"
        print(s[-1])      // 'f'
        print(s[-3:-1])   // "de"
    )";
    std::string expected = "abcdefabcdeffde";

    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(TypesTestSuite, ListSliceEdgeCases) {
    std::string code = R"(
        lst = [1,2,3,4,5]
        print(lst[:])       // [1, 2, 3, 4, 5]
        print(lst[:3])      // [1, 2, 3]
        print(lst[3:])      // [4, 5]
        print(lst[-1])      // 5
        print(lst[-3:-1])   // [3, 4]
    )";
    std::string expected = "[1, 2, 3, 4, 5][1, 2, 3][4, 5]5[3, 4]";

    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(TypesTestSuite, StringZeroLengthSlice) {
    std::string code = R"(
        s = "hello"
        print(s[2:2]) // ""
    )";
    std::string expected = "";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(TypesTestSuite, ListZeroLengthSlice) {
    std::string code = R"(
        lst = [9,8,7]
        print(lst[1:1]) // []
    )";
    std::string expected = "[]";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(TypesTestSuite, StringMixedSliceIndex) {
    std::string code = R"(
        s = "abcdef"
        print(s[1:5][2]) // "d"
    )";
    std::string expected = "d";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(TypesTestSuite, ListMixedSliceIndex) {
    std::string code = R"(
        lst = [1,2,3,4,5]
        print(lst[1:4][1]) // 3
    )";
    std::string expected = "3";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(TypesTestSuite, StringNegativeIndex) {
    std::string code = R"(
        s = "abcdef"
        print(s[-2]) // "e"
    )";
    std::string expected = "e";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(TypesTestSuite, ListNegativeIndex) {
    std::string code = R"(
        lst = [1,2,3,4,5]
        print(lst[-3]) // 3
    )";
    std::string expected = "3";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(TypesTestSuite, StringNestedSlices) {
    std::string code = R"(
        s = "abcdefgh"
        print(s[2:7][1:4]) // "def"
    )";
    std::string expected = "def";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(TypesTestSuite, ListNestedSlices) {
    std::string code = R"(
        lst = [10,20,30,40,50,60,70]
        print(lst[2:6][1:3]) // [40, 50]
    )";
    std::string expected = "[40, 50]";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
