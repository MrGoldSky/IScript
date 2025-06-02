#include <lib/interpreter.h>
#include <gtest/gtest.h>

TEST(ListSharedPtrTestSuite, PushAffectsAllAliases) { //!
    std::string code = R"(
        mass = [123, 10]
        mass1 = mass
        push(mass, "WOW")
        print(mass1)
    )";

    std::string expected = R"([123, 10, "WOW"])";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(ArrayEdgeCaseSuite, EmptyList) {
    std::string code = R"(
        arr = []
        print(len(arr))
    )";
    std::string expected = "0";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(ArrayEdgeCaseSuite, SingleElementList) {
    std::string code = R"(
        arr = [1]
        print(len(arr))
        print(arr[0])
    )";
    // len(arr) = 1, arr[0] = 1
    std::string expected = "11";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(ArrayEdgeCaseSuite, ListConcatenation) {
    std::string code = R"(
        a = [1, 2]
        b = [3, 4]
        c = a + b
        print(c)
    )";
    std::string expected = "[1, 2, 3, 4]";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(ArrayEdgeCaseSuite, ListRepetition) {
    std::string code = R"(
        a = [1, 2]
        b = a * 2
        print(b)
    )";
    std::string expected = "[1, 2, 1, 2]";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(ArrayEdgeCaseSuite, NestedLists) {
    std::string code = R"(
        arr = [1, [2, 3], 4]
        print(len(arr))
        print(arr[1][1])
    )";
    // len(arr)=3, arr[1][1]=3
    std::string expected = "33";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(ArrayEdgeCaseSuite, PositiveIndexing) {
    std::string code = R"(
        arr = [10, 20, 30]
        print(arr[1])
    )";
    std::string expected = "20";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(ArrayEdgeCaseSuite, NegativeIndexing) { //!
    std::string code = R"(
        arr = [10, 20, 30]
        print(arr[-1])
    )";
    std::string expected = "30";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(ArrayEdgeCaseSuite, OutOfBoundsIndexing) {
    std::string code = R"(
        arr = [1, 2]
        x = arr[2]
        print(1) // unreachable
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_FALSE(interpret(input, output));
}

TEST(ArrayEdgeCaseSuite, FullSlice) {
    std::string code = R"(
        arr = [1, 2, 3, 4]
        print(arr[:])
    )";
    std::string expected = "[1, 2, 3, 4]";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(ArrayEdgeCaseSuite, SliceStartOnly) {
    std::string code = R"(
        arr = [1, 2, 3, 4]
        print(arr[2:])
    )";
    std::string expected = "[3, 4]";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(ArrayEdgeCaseSuite, SliceEndOnly) {
    std::string code = R"(
        arr = [1, 2, 3, 4]
        print(arr[:2])
    )";
    std::string expected = "[1, 2]";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(ArrayEdgeCaseSuite, MiddleSlice) {
    std::string code = R"(
        arr = [1, 2, 3, 4]
        print(arr[1:3])
    )";
    std::string expected = "[2, 3]";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(ArrayEdgeCaseSuite, NegativeSliceIndices) {
    std::string code = R"(
        arr = [1, 2, 3, 4]
        print(arr[-3:-1])
    )";
    std::string expected = "[2, 3]";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(ArrayEdgeCaseSuite, InsertElement) {
    std::string code = R"(
        arr = [1, 3]
        insert(arr, 1, 2)
        print(arr)
    )";
    std::string expected = "[1, 2, 3]";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(ArrayEdgeCaseSuite, RemoveElement) {
    std::string code = R"(
        arr = [1, 2, 3]
        remove(arr, 1)
        print(arr)
    )";
    std::string expected = "[1, 3]";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(ArrayEdgeCaseSuite, PushPopOperations) {
    std::string code = R"(
        arr = [1, 2]
        push(arr, 3)
        print(pop(arr))
        print(len(arr))
    )";
    // push → [1,2,3]; pop возвращает 3, затем arr снова [1,2], len = 2
    std::string expected = "32";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(ArrayEdgeCaseSuite, RangeFunction) {
    std::string code = R"(
        lst = range(0, 3, 1)
        print(lst)
    )";
    std::string expected = "[0, 1, 2]";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(ArrayEdgeCaseSuite, JoinFunction) {
    std::string code = R"(
        arr = [1, 2, 3]
        print(join(arr, ","))
    )";
    std::string expected = "1,2,3";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(ArrayEdgeCaseSuite, SplitFunction) {
    std::string code = R"(
        parts = split("a b c", " ")
        print(parts[1])
    )";
    std::string expected = "b";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(ArrayEdgeCaseSuite, TypeMixingListAndNumber) {
    std::string code = R"(
        arr = [1, 2]
        c = arr + 3
        print(1) // unreachable
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_FALSE(interpret(input, output));
}
