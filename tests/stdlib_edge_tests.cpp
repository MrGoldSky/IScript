#include <gtest/gtest.h>
#include <lib/interpreter.h>

// Для простоты: макрос, который упрощает вызов interpret и сравнение результата.
// Если нужен только вывод (без проверки interpret-статуса), можно убрать ASSERT_TRUE.
#define RUN(code, expected)                    \
    do {                                       \
        std::istringstream input(code);        \
        std::ostringstream output;             \
        ASSERT_TRUE(interpret(input, output)); \
        ASSERT_EQ(output.str(), expected);     \
    } while (0)

#define RUN_ERR(code)                           \
    do {                                        \
        std::istringstream input(code);         \
        std::ostringstream output;              \
        ASSERT_FALSE(interpret(input, output)); \
    } while (0)

// 1. ТЕСТЫ ДЛЯ ФУНКЦИЙ РАБОТЫ С ЧИСЛАМИ (abs, ceil, floor, round, sqrt, rnd,
// parse_num, to_string)                                          :contentReference[oaicite:0]{index=0}

TEST(NumberLibEdgeSuite, AbsEdgeCases) {
    // 1.1. Отрицательное число → положительное
    RUN(
        "print(abs(-123.5))",
        "123.5");

    // 1.2. Ноль остаётся нулём
    RUN(
        "print(abs(0))",
        "0");

    // 1.3. Большое отрицательное → корректное значение
    RUN(
        "print(abs(-1e6))",
        "1000000");
}

TEST(NumberLibEdgeSuite, CeilFloorRoundEdgeCases) {
    RUN(
        "print(ceil(2.1)) print(ceil(-2.1))",
        "3-2");
    RUN(
        "print(floor(2.9)) print(floor(-2.9))",
        "2-3");
    RUN(
        "print(round(2.5)) print(round(3.5)) print(round(-2.5))",
        "34-3");
}

TEST(NumberLibEdgeSuite, SqrtEdgeCases) {
    // 1. Положительное число
    RUN(
        "print(sqrt(16))",
        "4");
    // 2. Ноль
    RUN(
        "print(sqrt(0))",
        "0");
    // 3. Отрицательное число → ошибка
    RUN_ERR(
        "print(sqrt(-4))");
}

TEST(NumberLibEdgeSuite, RndEdgeCases) {
    // 1. rnd(0) всегда возвращает 0 (по документации: if max <= 0 → 0.0)
    RUN(
        "print(rnd(0))",
        "0");
    // 2. rnd(1) возвращает только 0 (диапазон [0..0])
    RUN(
        "print(rnd(1))",
        "0");
    // 3. rnd с двумя аргументами, когда a > b (меняются местами внутри): rnd(5,2) → одно из {2,3,4}
    //    Здесь мы проверим, что вызов не падает и возвращает число ≥2 и <5.
    {
        std::string code = R"(
            v = rnd(5, 2)
            // Т.к. rnd генерирует значение от 2 до 4 включительно, проверим условие в if:
            if v < 2 then
                print("ERR")
            else if v >= 5 then
                print("ERR")
            else
                print("OK")
            end if
        )";
        RUN(code, "OK");
    }
}

TEST(NumberLibEdgeSuite, ParseNumAndToStringEdgeCases) {
    // 1. parse_num на строку, содержащую число
    RUN(
        "print(parse_num(\"123.0\"))",
        "123");
    // 2. parse_num на строку, не являющуюся числом → nil (печати нет), потом 1
    RUN(
        "print(parse_num(\"abc\")) print(1)",
        "nil1");
    // 3. to_string на числе и булеве
    RUN(
        "print(to_string(3.1415)) print(to_string(true))",
        "3.1415true");
    // 4. Составной тест: to_string(parse_num(...)) → идентичное исходному числу (если строка корректна)
    RUN(
        "print(to_string(parse_num(\"42\")))",
        "42");
}

////////////////////////////////////////////////////////////////////////////////
// 2. ТЕСТЫ ДЛЯ ФУНКЦИЙ РАБОТЫ СО СТРОКАМИ (len, lower, upper, split, join, replace)
//                                                          :contentReference[oaicite:1]{index=1}

TEST(StringLibEdgeSuite, LenLowerUpperEdgeCases) {
    // len:
    RUN(
        "print(len(\"\"))",  // пустая строка
        "0");
    RUN(
        "print(len(\"hello\"))",  // обычная строка
        "5");
    // lower / upper на пустой строке → пустая строка
    RUN(
        "print(lower(\"\")) print(upper(\"\"))",
        "");
    // lower / upper на смешанном регистре
    RUN(
        "print(lower(\"AbC123!\")) print(upper(\"AbC123!\"))",
        "abc123!ABC123!");
}

TEST(StringLibEdgeSuite, SplitEdgeCases) {
    // 1. split по умолчанию (пустой delim) с разными пробелами
    RUN(
        "lst = split(\"  a  b c   \")\n"
        "print(to_string(len(lst)))",  // должно быть 3 элемента: \"a\",\"b\",\"c\"
        "3");
    // 2. split по конкретному разделителю, который отсутствует → вернётся список из одного элемента (вся строка)
    RUN(
        "lst = split(\"abc\",\"|\")\n"
        "print(to_string(len(lst))) print(lst[0])",
        "1abc");
    // 3. split с некоротким delim, который встречается на концах
    RUN(
        "lst = split(\"*a*b*c*d\",\"*\")\n"
        "print(to_string(len(lst))) print(lst[0]) print(lst[1]) print(lst[2]) print(lst[3])",
        "4abcd"  // lst = [\"\",\"a\",\"b\",\"\"], печатаем len=4, затем \"\",\"a\",\"b\",\"\"
    );
}

TEST(StringLibEdgeSuite, JoinEdgeCases) {
    // 1. join пустого списка → пустая строка
    RUN(
        "lst = []\n"
        "print(join(lst, \",\"))",
        "");
    // 2. join списка из одного элемента (число и строка)
    RUN(
        "lst = [1]\n"
        "print(join(lst, \",\"))",
        "1");
    RUN(
        "lst = [\"a\"]\n"
        "print(join(lst, \"-\"))",
        "a");
    // 3. join нескольких элементов с кастомним разделителем
    RUN(
        "lst = [\"x\",\"y\",\"z\"]\n"
        "print(join(lst, \"|\"))",
        "x|y|z");
}

TEST(StringLibEdgeSuite, ReplaceEdgeCases) {
    // 1. Замена подстроки, которая есть → удаляется, даже если встречается несколько раз
    RUN(
        "print(replace(\"ababab\",\"ab\",\"x\"))",
        "xxx");
    // 2. Замена подстроки, которая отсутствует → строка без изменений
    RUN(
        "print(replace(\"hello\",\"xyz\",\"Q\"))",
        "hello");
    // 3. Замена пустой подстроки (по специфике: заменяем каждую позицию? Предположим, что replace делает ничего)
    RUN(
        "print(replace(\"hi\",\"\",\"X\"))",
        "hi");
}

////////////////////////////////////////////////////////////////////////////////
// 3. ТЕСТЫ ДЛЯ ФУНКЦИЙ РАБОТЫ СО СПИСКАМИ (range, len, push, pop, insert, remove, sort)
//                                                          :contentReference[oaicite:2]{index=2}

TEST(ListLibEdgeSuite, RangeEdgeCases) {
    // 1. Обычный положительный шаг
    RUN(
        "lst = range(0,5,2)\n"  // [0,2,4]
        "print(to_string(len(lst))) print(lst[0]) print(lst[1]) print(lst[2])",
        "3024");
    // 2. Отрицательный шаг (start > end)
    RUN(
        "lst = range(5,0,-2)\n"  // [5,3,1]
        "print(to_string(len(lst))) print(lst[0]) print(lst[1]) print(lst[2])",
        "3531");
}

TEST(ListLibEdgeSuite, LenListEdgeCases) {
    // 1. Пустой список
    RUN(
        "lst = []\n"
        "print(len(lst))",
        "0");
    // 2. Список с несколькими элементами разного типа
    RUN(
        "lst = [1, \"a\", 3]\n"
        "print(len(lst))",
        "3");
}

TEST(ListLibEdgeSuite, PushPopEdgeCases) {
    // 1. push в пустой список, затем pop → получаем то же значение
    RUN(
        "lst = []\n"
        "push(lst, 42)\n"
        "print(pop(lst))",
        "42");
}

TEST(ListLibEdgeSuite, InsertRemoveEdgeCases) {
    // 1. insert в середину списка
    RUN(
        "lst = [\"a\",\"c\"]\n"
        "insert(lst, 1, \"b\")\n"  // [\"a\",\"b\",\"c\"]
        "print(join(lst, \"\"))",
        "abc");
    // 2. insert с отрицательным индексом или за пределами → ошибка
    RUN_ERR(
        "lst = [1,2,3]\n"
        "insert(lst, -1, 5)");
    RUN_ERR(
        "lst = [1,2,3]\n"
        "insert(lst, 5, 9)");
    // 3. remove из середины
    RUN(
        "lst = [0,1,2,3]\n"
        "remove(lst, 2)\n"  // [0,1,3]
        "print(join(lst, \"\"))",
        "013");
    // 4. remove с некорректным индексом → ошибка
    RUN_ERR(
        "lst = [0]\n"
        "remove(lst, 1)");
}

TEST(ListLibEdgeSuite, SortEdgeCases) {
    // 1. Сортировка числового списка
    RUN(
        R"(
        lst = [3,1,2]
        new = sort(lst)
        print(join(new, ""))
    )",
        "123");
    // 2. Сортировка строкового списка (лексикографически)
    RUN(
        "lst = [\"b\",\"aa\",\"c\"]\n"
        "lst = sort(lst)\n"
        "print(join(lst, "
        "))",
        "aa b c");
    // 3. Сортировка «пустого» списка — никаких ошибок, остаётся пустым
    RUN(
        "lst = []\n"
        "lst = sort(lst)\n"
        "print(len(lst))",
        "0");
}

////////////////////////////////////////////////////////////////////////////////
// 4. КОМПОЗИТНЫЕ ТЕСТЫ (составные проверки нескольких функций вместе)

TEST(CompositeLibEdgeSuite, RangeLenJoinSplitCompose) {
    // Создадим список [0,1,2,3], объединим его в строку через «,», потом распарсим и измерим длину.
    RUN(
        R"(
            lst = range(0,4,1)     // [0,1,2,3]
            s = join(lst, ",")   // \"0,1,2,3\"
            parts = split(s, ",")
            print(to_string(len(parts)))
        )",
        "4");
}

TEST(CompositeLibEdgeSuite, ToStringParseNumLenCombine) {
    // to_string(parse_num(...)) + len → сначала преобразуем, затем измерим длину строки
    RUN(
        R"(
            x = to_string(parse_num("1000"))  // \"1000\"
            print(len(x))                       // 4
        )",
        "4");
}

TEST(CompositeLibEdgeSuite, MixedStringListNumberOperations) {
    // Проверим: конкатенацию строк (оператор +), вычитание «суффикса» (оператор -), умножение строки (оператор *)
    // а затем создадим список на основе этого и измерим длину
    RUN(
        R"(
            s = "abc" + "def"            // \"abcdef\"
            s2 = s - "def"                 // \"abc\"
            s3 = s2 * 2                      // \"abcabc\"
            lst = [s3, "x"]                // [\"abcabc\",\"x\"]
            print(join(lst, ":"))
        )",
        "abcabc:x");
}
