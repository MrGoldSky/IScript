#include <lib/interpreter.h>
#include <gtest/gtest.h>

#include "gtest/gtest.h"
#include "interpreter.h"
#include <sstream>

TEST(BranchingTestSuite, IfThenTrue) {
    std::string code = R"(
        if true then
            print(1)
        end if
    )";
    std::string expected = "1";

    std::istringstream  input(code);
    std::ostringstream  output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(BranchingTestSuite, IfThenFalseNoElse) {
    std::string code = R"(
        if false then
            print(1)
        end if
    )";
    std::string expected = "";

    std::istringstream  input(code);
    std::ostringstream  output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(BranchingTestSuite, IfThenElseTrueBranch) {
    std::string code = R"(
        if 5 > 2 then
            print(100)
        else
            print(200)
        end if
    )";
    std::string expected = "100";

    std::istringstream  input(code);
    std::ostringstream  output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(BranchingTestSuite, IfThenElseFalseBranch) {
    std::string code = R"(
        if 1 == 0 then
            print(100)
        else
            print(200)
        end if
    )";
    std::string expected = "200";

    std::istringstream  input(code);
    std::ostringstream  output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(BranchingTestSuite, ElseIfChain) {
    std::string code = R"(
        v = 100*2 + 10*3 + 9  // 239
        if v == 30 then
            print(30)
        else if v == 366 then
            print(366)
        else if v == 239 then
            print(239)
        else
            print(0)
        end if
    )";
    std::string expected = "239";

    std::istringstream  input(code);
    std::ostringstream  output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(BranchingTestSuite, NestedIfElse) {
    std::string code = R"(
        if true then
            if false then
                print(10)
            else
                print(11)
            end if
        end if
    )";
    std::string expected = "11";

    std::istringstream  input(code);
    std::ostringstream  output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(BranchingTestSuite, AssignmentInsideBranchPersists) { //!
    std::string code = R"(
        x = 0
        if true then
            x = 5
        end if
        print(x)
    )";
    std::string expected = "5";

    std::istringstream  input(code);
    std::ostringstream  output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(BranchingTestSuite, IfExpressionAssignment) {
    std::string code = R"(
        x = if 2 > 1 then 42 else 24 end if
        print(x)
    )";
    std::string expected = "42";

    std::istringstream  input(code);
    std::ostringstream  output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(BranchingTestSuite, NumericTruthiness) {
    std::string code = R"(
        if -1 then
            print(1)
        end if
        if 0 then
            print(2)
        else
            print(3)
        end if
    )";
    std::string expected = "13";

    std::istringstream  input(code);
    std::ostringstream  output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(BranchingTestSuite, NilConditionTreatedAsFalse) {
    std::string code = R"(
        if nil then
            print(1)
        else
            print(2)
        end if
    )";
    std::string expected = "2";

    std::istringstream  input(code);
    std::ostringstream  output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
