#include "interpreter.h"

#include <environment.h>

#include <limits>
#include <random>

#include "parser.h"
#include "lexer.h"

static void registerBuiltins(Environment& globals, std::ostream& out) {
    // print(something)
    globals.set("print",
                Value{FunctionValue{
                    [&out](std::vector<Value> args) -> Value {
                        for (auto& v : args)
                            out << v.toString();
                        return Value{};
                    }}});

    // println(something)
    globals.set("println",
                Value{FunctionValue{
                    [&out](std::vector<Value> args) -> Value {
                        for (auto& v : args)
                            out << v.toString();
                        out << '\n';
                        return Value{};
                    }}});

    // abs(x)
    globals.set("abs",
                Value{FunctionValue{
                    [](std::vector<Value> args) -> Value {
                        double x = Value::asNumeric(args[0]);
                        return Value{std::fabs(x)};
                    }}});

    // sqrt(x)
    globals.set("sqrt",
                Value{FunctionValue{
                    [](std::vector<Value> args) -> Value {
                        double x = Value::asNumeric(args[0]);
                        if (x < 0)
                            throw std::runtime_error("sqrt: negative argument");
                        return Value{std::sqrt(x)};
                    }}});

    // ceil(x)
    globals.set("ceil",
                Value{FunctionValue{
                    [](std::vector<Value> args) -> Value {
                        double x = Value::asNumeric(args[0]);
                        return Value{std::ceil(x)};
                    }}});

    // floor(x)
    globals.set("floor",
                Value{FunctionValue{
                    [](std::vector<Value> args) -> Value {
                        double x = Value::asNumeric(args[0]);
                        return Value{std::floor(x)};
                    }}});

    // round(x)
    globals.set("round",
                Value{FunctionValue{
                    [](std::vector<Value> args) -> Value {
                        double x = Value::asNumeric(args[0]);
                        return Value{std::round(x)};
                    }}});

    // rnd([min,] max)
    globals.set("rnd",
                Value{FunctionValue{
                    [](std::vector<Value> args) -> Value {
                        static thread_local std::mt19937_64 gen(std::random_device{}());

                        if (args.empty()) {
                            std::uniform_real_distribution<double> dist(0.0, 1.0);
                            return Value{dist(gen)};
                        }
                        if (args.size() == 1 && args[0].isNumber()) {
                            long long max = static_cast<long long>(args[0].asNumber());
                            if (max <= 0) return Value{0.0};
                            std::uniform_int_distribution<long long> dist(0, max - 1);
                            return Value{static_cast<double>(dist(gen))};
                        }
                        if (args.size() == 2 && args[0].isNumber() && args[1].isNumber()) {
                            long long a = static_cast<long long>(args[0].asNumber());
                            long long b = static_cast<long long>(args[1].asNumber());
                            if (a > b) std::swap(a, b);
                            if (a == b) return Value{static_cast<double>(a)};
                            std::uniform_int_distribution<long long> dist(a, b - 1);
                            return Value{static_cast<double>(dist(gen))};
                        }

                        throw std::runtime_error(
                            "rnd(): expected 0, 1 or 2 numeric arguments");
                    }}});

    // max(a,b, ...)
    globals.set("max",
                Value{FunctionValue{
                    [](std::vector<Value> args) -> Value {
                        double max = std::numeric_limits<double>::min();

                        if (args.size() == 1 && args[0].isList()) {
                            for (size_t i = 0; i < args[0].asList().size(); ++i) {
                                double a = Value::asNumeric(args[0].asList()[i]);
                                max = std::max(max, Value::asNumeric(args[0].asList()[i]));
                            }
                        } else
                            for (size_t i = 0; i < args.size(); ++i) {
                                double a = Value::asNumeric(args[i]);
                                max = std::max(max, Value::asNumeric(args[i]));
                            }
                        return Value{max};
                    }}});

    // min(a,b, ...)
    globals.set("min",
                Value{FunctionValue{
                    [](std::vector<Value> args) -> Value {
                        double min = std::numeric_limits<double>::max();
                        if (args.size() == 1 && args[0].isList()) {
                            for (size_t i = 0; i < args[0].asList().size(); ++i) {
                                double a = Value::asNumeric(args[0].asList()[i]);
                                min = std::min(min, Value::asNumeric(args[0].asList()[i]));
                            }
                        } else
                            for (size_t i = 0; i < args.size(); ++i) {
                                double a = Value::asNumeric(args[i]);
                                min = std::min(min, Value::asNumeric(args[i]));
                            }
                        return Value{min};
                    }}});

    // len(s)
    globals.set("len",
                Value{FunctionValue{
                    [](std::vector<Value> args) -> Value {
                        if (args[0].isString()) {
                            const std::string s = args[0].asString();
                            return Value(static_cast<double>(s.length()));
                        } else if (args[0].isList()) {
                            return Value{static_cast<double>(args[0].asList().size())};
                        } else
                            return Value{};
                    }}});

    // lower(s)
    globals.set("lower",
                Value{FunctionValue{
                    [](std::vector<Value> args) -> Value {
                        std::string s = args[0].asString();
                        for_each(s.begin(), s.end(), [](char& c) {
                            c = std::tolower(c);
                        });
                        return Value(s);
                    }}});

    // upper(s)
    globals.set("upper",
                Value{FunctionValue{
                    [](std::vector<Value> args) -> Value {
                        std::string s = args[0].asString();
                        for_each(s.begin(), s.end(), [](char& c) {
                            c = std::toupper(c);
                        });
                        return Value(s);
                    }}});

    // split(s, delim)
    globals.set("split",
                Value{FunctionValue{
                    [](std::vector<Value> args) -> Value {
                        if (args.empty()) {
                            return Value{Value::RawList{}};
                        }
                        const std::string& s = args[0].asString();
                        std::string sep;
                        if (args.size() == 2) {
                            sep = args[1].asString();
                        }

                        Value::RawList parts;
                        if (sep.empty()) {
                            size_t i = 0, n = s.size();
                            while (i < n) {
                                while (i < n && std::isspace(static_cast<unsigned char>(s[i]))) ++i;
                                if (i >= n) break;
                                size_t j = i;
                                while (j < n && !std::isspace(static_cast<unsigned char>(s[j]))) ++j;
                                parts.emplace_back(s.substr(i, j - i));
                                i = j;
                            }
                        } else {
                            size_t start = 0, pos;
                            while ((pos = s.find(sep, start)) != std::string::npos) {
                                if (pos > start) {
                                    parts.emplace_back(s.substr(start, pos - start));
                                }
                                start = pos + sep.size();
                            }
                            if (start < s.size()) {
                                parts.emplace_back(s.substr(start));
                            }
                        }
                        return Value{std::move(parts)};
                    }}});

    // parse_num(s)
    globals.set("parse_num",
                Value{FunctionValue{
                    [](std::vector<Value> args) -> Value {
                        if (args.empty()) {
                            return Value{};
                        }
                        const Value& x = args[0];
                        if (x.isNumber() || x.isBool()) {
                            return Value{Value::asNumeric(x)};
                        } else if (x.isString()) {
                            const auto& str = x.asString();
                            try {
                                size_t idx = 0;
                                double d = std::stod(str, &idx);
                                if (idx == str.size()) {
                                    return Value{d};
                                }
                            } catch (...) {
                            }
                        }
                        return Value{};
                    }}});

    // range(start, end, step)
    globals.set("range",
                Value{FunctionValue{
                    [](std::vector<Value> args) -> Value {
                        double start, end, step;

                        if (args.size() == 1) {
                            start = 0.0;
                            end = Value::asNumeric(args[0]);
                            step = 1.0;
                        } else if (args.size() == 2) {
                            start = Value::asNumeric(args[0]);
                            end = Value::asNumeric(args[1]);
                            step = 1.0;
                        } else if (args.size() == 3) {
                            start = Value::asNumeric(args[0]);
                            end = Value::asNumeric(args[1]);
                            step = Value::asNumeric(args[2]);
                        } else {
                            throw std::runtime_error("range: expected 1 to 3 numeric arguments");
                        }

                        if (step == 0.0) {
                            throw std::runtime_error("range: step cannot be zero");
                        }

                        Value::RawList result;
                        if (step > 0) {
                            for (double v = start; v < end; v += step) {
                                result.emplace_back(v);
                            }
                        } else {
                            for (double v = start; v > end; v += step) {
                                result.emplace_back(v);
                            }
                        }
                        return Value{std::move(result)};
                    }}});

    // to_string(something)
    globals.set("to_string",
                Value{FunctionValue{
                    [](std::vector<Value> args) -> Value {
                        return Value(args[0].toString());
                    }}});

    // Функции списков

    // join(list, delim)
    globals.set("join",
                Value{FunctionValue{
                    [](std::vector<Value> args) -> Value {
                        auto lst = args[0].asList();
                        std::string delim = " ";
                        if (args.size() == 2)
                            delim = args[1].asString();
                        std::string result;
                        for (size_t i = 0; i < lst.size(); ++i) {
                            result += lst[i].toString();
                            if (i + 1 < lst.size())
                                result += delim;
                        }
                        return Value(result);
                    }}});

    // push(list, value)
    globals.set("push",
                Value{FunctionValue{
                    [](std::vector<Value> args) -> Value {
                        if (args.size() != 2 || !args[0].isList()) {
                            throw std::runtime_error("push(list, elem): expected a list and an element");
                        }
                        args[0].asList().push_back(args[1]);
                        return Value{};
                    }}});

    // insert(list, index, value)
    globals.set("insert",
                Value{FunctionValue{
                    [](std::vector<Value> args) -> Value {
                        if (args.size() != 3 || !args[0].isList() || !args[1].isNumber()) {
                            throw std::runtime_error(
                                "insert(list, index, value): expected (list, number, any)");
                        }
                        auto& list = args[0].asList();
                        ptrdiff_t idx = static_cast<ptrdiff_t>(Value::asNumeric(args[1]));
                        if (idx < 0 || static_cast<size_t>(idx) > list.size()) {
                            throw std::out_of_range("insert: index out of range");
                        }
                        list.insert(list.begin() + idx, args[2]);
                        return Value{};
                    }}});

    // pop(list)
    globals.set("pop",
                Value{FunctionValue{
                    [](std::vector<Value> args) -> Value {
                        if (args.size() != 1 || !args[0].isList()) {
                            throw std::runtime_error("push(list, elem): expected a list");
                        }
                        Value v = Value{args[0].asList().back()};
                        args[0].asList().pop_back();
                        return v;
                    }}});

    // remove(list, index)
    globals.set("remove",
                Value{FunctionValue{
                    [](std::vector<Value> args) -> Value {
                        if (args.size() != 2 || !args[0].isList() || !args[1].isNumber()) {
                            throw std::runtime_error(
                                "remove(list, index): expected (list, number)");
                        }
                        auto& list = args[0].asList();
                        ptrdiff_t idx = static_cast<ptrdiff_t>(Value::asNumeric(args[1]));
                        if (idx < 0 || static_cast<size_t>(idx) >= list.size()) {
                            throw std::out_of_range("remove: index out of range");
                        }
                        list.erase(list.begin() + idx);
                        return Value{};
                    }}});

    // sort(list): сортирует копию списка “по toString()”
    globals.set("sort",
                Value{FunctionValue{
                    [](std::vector<Value> args) -> Value {
                        if (args.size() != 1 || !args[0].isList()) {
                            throw std::runtime_error("sort: expected a single list argument");
                        }
                        auto vec = args[0].asList();

                        std::sort(vec.begin(), vec.end(), [](const Value& a, const Value& b) {
                            const std::string sa = a.toString();
                            const std::string sb = b.toString();
                            return sa < sb;
                        });

                        return Value(Value::RawList(std::move(vec)));
                    }}});

    // replace(s, old, new)
    globals.set("replace",
                Value{FunctionValue{
                    [](std::vector<Value> args) -> Value {
                        if (args.size() != 3 || !args[0].isString() || !args[1].isString() || !args[2].isString()) {
                            throw std::runtime_error("replace: expected (string, string, string)");
                        }
                        std::string s = args[0].asString();
                        std::string old = args[1].asString();
                        std::string nw = args[2].asString();

                        if (old.empty()) {
                            return Value(s);
                        }
                        size_t pos = 0;
                        while ((pos = s.find(old, pos)) != std::string::npos) {
                            s.replace(pos, old.length(), nw);
                            pos += nw.length();
                        }
                        return Value(s);
                    }}});

    // read(): читает строку из std::cin и возвращает её как строку
    globals.set("read",
                Value{FunctionValue{
                    [](std::vector<Value> args) -> Value {
                        if (!args.empty()) {
                            throw std::runtime_error("read: expected no arguments");
                        }
                        std::string line;
                        if (!std::getline(std::cin, line)) {
                            return Value{};
                        }
                        return Value(line);
                    }}});

    // stacktrace(): возвращает список (LAIST) из имён функций, начиная с самого раннего вызова
    globals.set("stacktrace",
                Value{FunctionValue{
                    [](std::vector<Value> args) -> Value {
                        if (!args.empty()) {
                            throw std::runtime_error("stacktrace: expected no arguments");
                        }
                        Value::RawList lst;
                        for (const auto& name : g_callStack) {
                            lst.emplace_back(Value(name));
                        }
                        return Value(std::move(lst));
                    }}});
}

bool interpret(std::istream& input, std::ostream& output) {
    Lexer lexer(input);
    Parser parser(lexer);
    try {
        std::vector<std::unique_ptr<FunctionAST>> functions;
        if (!parser.parseModule(functions))
            return false;

        Environment globals;
        registerBuiltins(globals, output);

        auto globalsPtr = std::make_shared<Environment>(globals);
        for (auto& fn : functions) {
            auto const& proto = fn->getProto();
            globalsPtr->set(proto.getName(), Value(FunctionValue{fn.get(), globalsPtr}));
        }

        for (auto& fn : functions) {
            if (fn->getProto().getName() == "__anon_expr") {
                fn->getBody().eval(*globalsPtr);
            }
        }

        return true;
    } catch (std::exception& e) {
        output << "Error: " << e.what();
        return false;
    }
}
