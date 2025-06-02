#include <lib/interpreter.h>
#include <gtest/gtest.h>
#include <string>
#include <sstream>

TEST(FunctionComboTestSuite, StringConcatFunction) {
    std::string code = R"(
        concat = function(a, b)
            return a + b
        end function
        print(concat("Hello, ", "World!"))
    )";
    std::string expected = "Hello, World!";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionComboTestSuite, StringRepeatFunction) {
    std::string code = R"(
        repeat = function(s, n)
            return s * n
        end function
        print(repeat("ab", 2.5))
    )";
    // "ab" * 2 → "abab", 0.5 остатка даёт 1 символ "a", в сумме "ababa"
    std::string expected = "ababa";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionComboTestSuite, ListNegativeIndexFunction) {
    std::string code = R"(
        getLast = function(lst)
            return lst[-1]
        end function
        print(getLast([1, 2, 3, 4]))
    )";
    std::string expected = "4";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionComboTestSuite, StringSliceFunction) {
    std::string code = R"(
        slice = function(s, start, ends)
            return s[start:ends]
        end function
        print(slice("abcdef", 1, 4))
    )";
    std::string expected = "bcd";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionComboTestSuite, ListConcatFunction) {
    std::string code = R"(
        combine = function(a, b)
            return a + b
        end function
        print(combine([1, 2], [3, 4]))
    )";
    // toString списка выводит "[1, 2, 3, 4]"
    std::string expected = "[1, 2, 3, 4]";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionComboTestSuite, ListLengthFunction) {
    std::string code = R"(
        lenlst = function(lst)
            return len(lst)
        end function
        print(lenlst([10, 20, 30]))
    )";
    std::string expected = "3";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionComboTestSuite, SplitJoinFunction) {
    std::string code = R"(
        splitjoin = function(s, delim)
            return join(split(s, delim), "|")
        end function
        print(splitjoin("a b c", " "))
    )";
    // split("a b c", " ") → ["a","b","c"], join с "|" → "a|b|c"
    std::string expected = "a|b|c";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

// // Тест вложенной функции и теней переменных
// TEST(FunctionComboTestSuite, ShadowingInnerFunction) { // !
//     std::string code = R"(
//         shadow = function(x)
//             inner = function()
//                 return x
//             end function
//             x = x + 1
//             return inner()
//         end function
//         print(shadow(5))
//     )";
//     // inner() захватывает родительскую среду (x становится 6 после прибавления), возвращает 6
//     std::string expected = "6";
//     std::istringstream input(code);
//     std::ostringstream output;
//     ASSERT_TRUE(interpret(input, output));
//     ASSERT_EQ(output.str(), expected);
// }

TEST(FunctionComboTestSuite, SumListWithLoop) {
    std::string code = R"(
        sumList = function(lst)
            total = 0
            for i in lst
                total = total + i
            end for
            return total
        end function
        print(sumList([1, 2, 3, 4, 5]))
    )";
    std::string expected = "15";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionComboTestSuite, ReturnListAndIndex) {
    std::string code = R"(
        makeList = function(n)
            lst = []
            i = 0
            while i < n
                lst = lst + [i]
                i = i + 1
            end while
            return lst
        end function
        print(makeList(4)[2])
    )";
    // makeList(4) → [0,1,2,3], [2] → 2
    std::string expected = "2";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionComboTestSuite, MaxOfThreeFunction) {
    std::string code = R"(
        max3 = function(a, b, c)
            return max(a, b, c)
        end function
        print(max3(3, 7, 5))
    )";
    std::string expected = "7";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionComboTestSuite, ParseNumFunction) {
    std::string code = R"(
        toNum = function(s)
            return parse_num(s)
        end function
        print(toNum("42"))
        print(toNum("abc"))
    )";
    // toNum("42") → 42, toNum("abc") → nil (ничего не печатается)
    std::string expected = "42nil";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionComboTestSuite, MapFunctionReturnsList) {
    std::string code = R"(
        map = function(f, lst)
            res = []
            for x in lst
                res = res + [f(x)]
            end for
            return res
        end function
        square = function(x)
            return x * x
        end function
        print(map(square, [1, 2, 3, 4])[2])
    )";
    // square([1,2,3,4]) → [1,4,9,16], [2] → 9
    std::string expected = "9";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionComboTestSuite, LogicalFunction) {
    std::string code = R"(
        check = function(a, b)
            if a and b then
                return true
            else
                return false
            end if
        end function
        print(check(true, false))
        print(check(true, true))
    )";
    // check(true,false) → false, check(true,true) → true
    std::string expected = "falsetrue";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionComboTestSuite, ModifyListInsideFunction) {
    std::string code = R"(
        modify = function(lst)
            push(lst, 10)
            remove(lst, 0)
            return lst
        end function
        l = [1, 2, 3]
        print(modify(l)[2])
    )";
    // l = [1,2,3] → push 10 → [1,2,3,10], remove index 0 → [2,3,10], [2] → 10
    std::string expected = "10";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionComboTestSuite, StringReplaceFunction) {
    std::string code = R"(
        replaceStr = function(s, old, new)
            return replace(s, old, new)
        end function
        print(replaceStr("foobar", "foo", "bar"))
    )";
    // "foobar".replace("foo","bar") → "barbar"
    std::string expected = "barbar";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionComboTestSuite, NoReturnReturnsNil) {
    std::string code = R"(
        f = function()
            a = 1
        end function
        print(f())
        print(1)
    )";
    // f() → nil, print(nil) ничего не выводит, затем print(1) → "1"
    std::string expected = "nil1";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionComboTestSuite, IsPositiveFunction) {
    std::string code = R"(
        isPos = function(n)
            if n > 0 then
                return true
            else
                return false
            end if
        end function
        print(isPos(-1))
    )";
    std::string expected = "false";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionComboTestSuite, ExponentModuloFunction) {
    std::string code = R"(
        compute = function(x)
            return (x ^ 2) % 5
        end function
        print(compute(3))
    )";
    // 3^2 = 9, 9 % 5 = 4
    std::string expected = "4";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionComboTestSuite, ListSliceFunction) {
    std::string code = R"(
        sliceList = function(lst, a, b)
            return lst[a:b]
        end function
        print(sliceList([10, 20, 30, 40, 50], 1, 4)[1])
    )";
    // [10,20,30,40,50][1:4] → [20,30,40], [1] → 30
    std::string expected = "30";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
