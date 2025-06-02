#include <gtest/gtest.h>
#include <lib/interpreter.h>

TEST(TypesTestSuite, NilEqualityComparison) {
    std::string code = R"(
        x = 42
        y = true
        z = "hi"
        lst = [1,2,3]

        print(nil == nil)    // true
        print(nil == 0)      // false
        print(nil == false)  // false
        print(nil == "hi")   // false
        print(nil == [])     // false
        print(x  == nil)     // false
        print(y  == nil)     // false
        print(z  == nil)     // false
        print(lst == nil)    // false
    )";
    std::string expected =
        "true"    // nil == nil
        "false"   // nil == 0
        "false"   // nil == false
        "false"   // nil == "hi"
        "false"   // nil == []
        "false"   // x   == nil
        "false"   // y   == nil
        "false"   // z   == nil
        "false";  // lst == nil

    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(TypesTestSuite, NilInequalityComparison) {
    std::string code = R"(
        x = 42
        y = true
        z = "hi"
        lst = [1,2,3]

        print(nil != nil)    // false
        print(nil != 0)      // true
        print(nil != false)  // true
        print(nil != "hi")   // true
        print(nil != [])     // true
        print(x   != nil)    // true
        print(y   != nil)    // true
        print(z   != nil)    // true
        print(lst != nil)    // true
    )";
    std::string expected =
        "false"  // nil != nil
        "true"   // nil != 0
        "true"   // nil != false
        "true"   // nil != "hi"
        "true"   // nil != []
        "true"   // x   != nil
        "true"   // y   != nil
        "true"   // z   != nil
        "true";  // lst != nil

    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}