#include <lib/interpreter.h>
#include <gtest/gtest.h>

TEST(BranchTestSuite, SimpleIfTest) {
    std::string code = R"(
        cond = true
        if cond then
            print("true")
        end if
    )";

    std::string expected = "true";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(BranchTestSuite, SimpleElseIfTest) {
    std::string code = R"(
        cond = false
        if cond then
            print("true")
        else
            print("false")
        end if
    )";

    std::string expected = "false";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}


TEST(BranchTestSuite, ComplexIfTest) {
    std::string code = R"(
        v = 100 * 2 + 10 * 3 + 9
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

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}


TEST(BranchTestSuite, OneLineIfTest) {
    std::string code = "if 2 * 2 == 4 then print(\"2 * 2 == 4\") else print(\"omg\") end if";
    std::string expected = "2 * 2 == 4";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}


TEST(LoopTestSuit, ForLoop) {
    std::string code = R"(
        for i in range(0,5,1)
            print(i)
        end for
    )";

    std::string expected = "01234";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}


TEST(LoopTestSuit, WhileLoop) {
    std::string code = R"(
        s = "ITMO"
        while  len(s) < 12
            s = s * 2
        end while
        print(s)
    )";

    std::string expected = "ITMOITMOITMOITMO";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(LoopTestSuite, For_PositiveStep) {
    std::string code = R"(
        // range с положительным шагом 2
        for i in range(0, 5, 2)
            print(i)
        end for
    )";
    std::string expected = "024";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(LoopTestSuite, For_EmptyRange_PositiveStep) {
    std::string code = R"(
        // пустой диапазон при равных границах
        for i in range(5, 5, 1)
            print(i)
        end for
    )";
    std::string expected = "";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(LoopTestSuite, For_StepGreaterThanRange) {
    std::string code = R"(
        // шаг больше длины диапазона
        for i in range(0, 5, 10)
            print(i)
        end for
    )";
    std::string expected = "0";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(LoopTestSuite, For_NegativeStep) {
    std::string code = R"(
        // обратный диапазон с отрицательным шагом
        for i in range(5, 1, -2)
            print(i)
        end for
    )";
    std::string expected = "53";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(LoopTestSuite, For_EmptyRange_NegativeStep) {
    std::string code = R"(
        // пустой диапазон при отрицательном шаге, когда start < end
        for i in range(1, 5, -1)
            print(i)
        end for
    )";
    std::string expected = "";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(LoopTestSuite, For_ZeroStepError) {
    std::string code = R"(
        // шаг равен нулю — ошибка
        for i in range(0, 1, 0)
            print(i)
        end for
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_FALSE(interpret(input, output));
}

TEST(LoopTestSuite, For_Break) {
    std::string code = R"(
        // выход из цикла по break
        for i in range(0, 10, 1)
            if i == 3 then
                break
            end if
            print(i)
        end for
    )";
    std::string expected = "012";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(LoopTestSuite, For_Continue) {
    std::string code = R"(
        // пропуск нечетных по continue
        for i in range(0, 5, 1)
            if i % 2 != 0 then
                continue
            end if
            print(i)
        end for
    )";
    std::string expected = "024";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(LoopTestSuite, For_NestedLoops) {
    std::string code = R"(
        // вложенные циклы
        for i in range(1, 3, 1)
            for j in range(1, 3, 1)
                print(i * j)
            end for
        end for
    )";
    std::string expected = "1224";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(LoopTestSuite, While_Basic) {
    std::string code = R"(
        // обычный while
        i = 0
        while i < 3
            print(i)
            i = i + 1
        end while
    )";
    std::string expected = "012";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(LoopTestSuite, While_ZeroIterations) {
    std::string code = R"(
        // условие false изначально
        i = 3
        while i < 3
            print(i)
            i = i + 1
        end while
    )";
    std::string expected = "";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(LoopTestSuite, While_Decrement) {
    std::string code = R"(
        // убывающий счетчик
        i = 3
        while i > 0
            print(i)
            i = i - 1
        end while
    )";
    std::string expected = "321";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(LoopTestSuite, While_Break) {
    std::string code = R"(
        // выход из while по break
        count = 0
        while true
            count = count + 1
            if count == 1 then
                break
            end if
        end while
        print(count)
    )";
    std::string expected = "1";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(LoopTestSuite, While_Continue) {
    std::string code = R"(
        // пропуск нечетных с помощью continue
        i = 0
        while i < 5
            i = i + 1
            if i % 2 != 0 then
                continue
            end if
            print(i)
        end while
    )";
    std::string expected = "24";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(LoopTestSuite, While_NestedLoops) {
    std::string code = R"(
        // вложенные while
        i = 1
        while i <= 2
            j = 1
            while j <= 2
                print(i + j)
                j = j + 1
            end while
            i = i + 1
        end while
    )";
    std::string expected = "2334";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(LoopTestSuite, While_NonBooleanCondition) {
    std::string code = R"(
        // число 2 приводится к true, 0 — к false
        i = 2
        while i
            print(i)
            i = i - 2
        end while
    )";
    std::string expected = "2";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
