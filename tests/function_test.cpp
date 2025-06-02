#include <gtest/gtest.h>
#include <lib/interpreter.h>

TEST(FunctionTestSuite, SimpleFunctionTest) {
    std::string code = R"(
        incr = function(value)
            return value + 1
        end function

        x = incr(2)
        print(x)
    )";

    std::string expected = "3";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionTestSuite, FunctionAsArgTest) {
    std::string code = R"(
        incr = function(value)
            return value + 1
        end function

        printresult = function(value, func)
            result = func(value)
            print(result)
        end function

        printresult(2, incr)
    )";

    std::string expected = "3";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionTestSuite, NestedFunctionTest) {
    std::string code = R"(
        // NB: inner and outer `value` are different symbols.
        // You are not required to implement closures (aka lambdas).

        incr_and_print = function(value)
            incr = function(value)
                return value + 1
            end function

            print(incr(value))
        end function

        incr_and_print(2)
    )";

    std::string expected = "3";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionTestSuite, FunnySyntaxTest) {
    std::string code = R"(
        funcs = [
            function() return 1 end function,
            function() return 2 end function,
            function() return 3 end function,
        ]

        print(funcs[0]())
        print(funcs[1]())
        print(funcs[2]())
    )";

    std::string expected = "123";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionEdgeCaseSuite, SimpleIncrement) {
    std::string code = R"(
        incr = function(x)
            return x + 1
        end function

        print(incr(5))
    )";
    std::string expected = "6";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionEdgeCaseSuite, NoReturnReturnsNil) {  //!
    std::string code = R"(
        noRet = function()
            x = 10
            x = x * 2
        end function

        r = noRet()
        print(r)
        print(1)
    )";
    std::string expected = "nil1";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionEdgeCaseSuite, LocalVariableDoesNotAffectGlobal) {  //!
    std::string code = R"(
        x = 100
        foo = function()
            x = 50
            print(x)      // ожидаем "50" внутри функции
        end function

        foo()
        print(x)          // ожидаем "100" после вызова
    )";
    std::string expected = "50100";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionEdgeCaseSuite, ParameterShadowsGlobal) {  //!
    std::string code = R"(
        x = 7
        foo = function(x)
            return x * 2
        end function

        print(foo(3))     // 6
        print(x)          // 7 (глобальный x не изменился)
    )";
    std::string expected = "67";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionEdgeCaseSuite, MultipleParameters) {
    std::string code = R"(
        sum3 = function(a, b, c)
            return a + b + c
        end function

        print(sum3(1, 2, 3))   // 6
        print(sum3(5, 5, -2))  // 8
    )";
    std::string expected = "68";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionEdgeCaseSuite, FunctionAsArgument) {  //!
    std::string code = R"(
        apply = function(f, v)
            return f(v)
        end function

        square = function(x)
            return x * x
        end function

        print(apply(square, 4))   // 16
        print(apply(square, 7))   // 49
    )";
    std::string expected = "1649";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionEdgeCaseSuite, ReturnFunction) {  //!
    std::string code = R"(
        makeAdder = function(n)
            return function(x)
                return x + n
            end function
        end function

        add5 = makeAdder(5)
        print(add5(10))   // 15
        add10 = makeAdder(10)
        print(add10(3))   // 13
    )";
    std::string expected = "1513";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

// 8. Рекурсивная функция для вычисления факториала
TEST(FunctionEdgeCaseSuite, RecursiveFactorial) {  //!!
    std::string code = R"(
        fact = function(n)
            if n == 0 then
                return 1
            else
                return n * fact(n - 1)
            end if
        end function

        print(fact(0))   // 1
        print(fact(1))   // 1
        print(fact(5))   // 120
    )";
    std::string expected = "11120";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

// TEST(FunctionEdgeCaseSuite, MutualRecursionEvenOdd) { //!
//     std::string code = R"(
//         isEven = function(n)
//             if n == 0 then
//                 return true
//             else
//                 return isOdd(n - 1)
//             end if
//         end function

//         isOdd = function(n)
//             if n == 0 then
//                 return false
//             else
//                 return isEven(n - 1)
//             end if
//         end function

//         print(isEven(4))   // true
//         print(isOdd(4))    // false
//         print(isEven(7))   // false
//         print(isOdd(7))    // true
//     )";
//     std::string expected = "truefalsetruefalse";
//     std::istringstream input(code);
//     std::ostringstream output;
//     ASSERT_TRUE(interpret(input, output));
//     ASSERT_EQ(output.str(), expected);
// }

TEST(FunctionEdgeCaseSuite, ImmediateInvocation) {
    std::string code = R"(
        print((function(x)
            return x * 2
        end function)(6))   // сразу вызов, ожидаем "12"
    )";
    std::string expected = "12";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionEdgeCaseSuite, MultipleStatementsWithEarlyReturn) {
    std::string code = R"(
        testEarly = function(n)
            print(1)
            if n < 0 then
                return -1
            end if
            print(2)
            return n
            print(3)    // не должно выполниться
        end function

        print(testEarly(-5))   // печатает "1", затем "-1"
        print(testEarly(7))    // печатает "1", "2", затем "7"
    )";
    std::string Expected = "1-1127";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), Expected);
}

TEST(FunctionEdgeCaseSuite, NestedFunctionScope) {
    std::string code = R"(
        x = 1
        outer = function(a)
            x = a * 2
            inner = function(b)
                return x + b
            end function
            return inner(a)
        end function

        print(outer(5))   // x локальное = 10, inner(5) = 10 + 5 = 15
        print(x)          // глобальное x = 1
    )";
    std::string expected = "151";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionEdgeCaseSuite, ListOfFunctionsWithIndex) {
    std::string code = R"(
        funcs = [
            function() return 10 end function,
            function() return 20 end function,
            function() return 30 end function,
        ]

        print(funcs[1]())   // 20
        print(funcs[0]())   // 10
        print(funcs[2]())   // 30
    )";
    std::string expected = "201030";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionEdgeCaseSuite, RedefineFunctionOverrides) { 
    std::string code = R"(
        foo = function() return 1 end function
        print(foo())   // 1
        foo = function() return 2 end function
        print(foo())   // 2
    )";
    std::string expected = "12";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionEdgeCaseSuite, BuiltinInsideFunction) {
    std::string code = R"(
        arr = [1, 2, 3, 4]
        getLen = function(lst)
            return len(lst)
        end function

        print(getLen(arr))   // 4
    )";
    std::string expected = "4";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionEdgeCaseSuite, RecursiveFibonacci) { //!
    std::string code = R"(
        fib = function(n)
            if n < 2 then
                return n
            end if
            return fib(n - 1) + fib(n - 2)
        end function

        print(fib(0))   // 0
        print(fib(1))   // 1
        print(fib(5))   // 5
        print(fib(6))   // 8
    )";
    std::string expected = "0158";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionEdgeCaseSuite, HigherOrderReturnTwice) {  //!
    std::string code = R"(
        wrap = function(f)
            return function(x)
                return f(f(x))
            end function
        end function

        inc = function(n) return n + 1 end function

        dblInc = wrap(inc)
        print(dblInc(3))   // inc(inc(3)) = 5
    )";
    std::string expected = "5";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionEdgeCaseSuite, IndependentAdders) {  //!
    std::string code = R"(
        makeCounter = function()
            count = 0
            return function()
                count = count + 1
                return count
            end function
        end function

        c1 = makeCounter()
        c2 = makeCounter()
        print(c1())   // 1
        print(c1())   // 2
        print(c2())   // 1
        print(c1())   // 3
        print(c2())   // 2
    )";
    std::string expected = "12132";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionEdgeCaseSuite, ReturnListOfFunctions) { //!
    std::string code = R"(
        makeFuncs = function(n)
            lst = []
            i = 0
            while i < n
                lst = lst + [function() return i end function]
                i = i + 1
            end while
            return lst
        end function

        fs = makeFuncs(3)
        print(fs[0]())   // 0
        print(fs[1]())   // 1
        print(fs[2]())   // 2
    )";
    std::string expected = "012";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionEdgeCaseSuite, ImplicitNilIfNoReturnBranches) {
    std::string code = R"(
        condTest = function(x)
            if x > 0 then
                return 1
            end if
            // Здесь нет return для случая x <= 0
        end function

        print(condTest(5))    // 1
        print(condTest(0))    // nil
        print(2)              // "2"
    )";
    std::string expected = "1nil2";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionEdgeCaseSuite, GlobalVariableVisibleInFunction) {
    std::string code = R"(
        x = 5                 // глобальная переменная
        foo = function()
            print(x)          // внутри функции читаем глобальную x
        end function

        foo()                 // ожидаем, что напечатает "5"
    )";
    std::string expected = "5";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionEdgeCaseSuite, LocalVariableShadowsGlobal) {
    std::string code = R"(
        x = 10                // глобальная x
        foo = function()
            x = 42            // создаём или назначаем локальную x
            print(x)          // ожидаем "42" (локальная)
        end function

        foo()
        print(x)              // после вызова foo() глобальная x должна остаться "10"
    )";
    std::string expected = "4210";
    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionEdgeCaseSuite, LocalVariableShadowsGlobal2) {
    std::string code = R"(
        x = 10
        foo = function()
            y = 42
            print(y)
        end function

        foo()
        print(y)
    )";

    std::istringstream input(code);
    std::ostringstream output;
    ASSERT_FALSE(interpret(input, output));
}
