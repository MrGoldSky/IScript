#pragma once
#include <algorithm>
#include <cmath>
#include <functional>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

extern std::vector<std::string> g_callStack;

class FunctionAST;
class Environment;
class Value;

template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

struct FunctionValue {
    using BuiltinFn = std::function<Value(std::vector<Value>)>;

    bool isBuiltin;
    BuiltinFn builtinFn;
    const FunctionAST* fnAST;
    std::shared_ptr<Environment> closure;

    FunctionValue(BuiltinFn fn)
        : isBuiltin(true), builtinFn(std::move(fn)), fnAST(nullptr), closure(nullptr) {}

    FunctionValue(const FunctionAST* f, std::shared_ptr<Environment> env)
        : isBuiltin(false), builtinFn(), fnAST(f), closure(std::move(env)) {}

    Value invoke(const std::vector<Value>& args) const;
};

class Value {
   public:
    using RawList = std::vector<Value>;
    using ListPtr = std::shared_ptr<RawList>;
    using Variant = std::variant<std::monostate, double, bool, std::string, ListPtr, FunctionValue>;

    static int normalizeIndex(int idx, int n) {
        if (idx < 0) idx += n;
        if (idx < 0 || idx >= n) throw std::runtime_error("Index " + std::to_string(idx) + " out of range [0," + std::to_string(n) + ")");
        return idx;
    }

    Value() : v(std::monostate{}) {}
    Value(double d) : v(d) {}
    Value(bool b) : v(b) {}
    Value(const std::string& s) : v(s) {}
    Value(std::string&& s) : v(std::move(s)) {}
    Value(RawList xs) : v(std::make_shared<RawList>(std::move(xs))) {}
    Value(FunctionValue f) : v(std::move(f)) {}

    bool isNil() const { return std::holds_alternative<std::monostate>(v); }
    bool isNumber() const { return std::holds_alternative<double>(v); }
    bool isBool() const { return std::holds_alternative<bool>(v); }
    bool isString() const { return std::holds_alternative<std::string>(v); }
    bool isList() const { return std::holds_alternative<ListPtr>(v); }
    bool isFunc() const { return std::holds_alternative<FunctionValue>(v); }

    double asNumber() const { return std::get<double>(v); }
    bool asBool() const {
        if (isBool()) return std::get<bool>(v);
        if (isNumber()) return std::get<double>(v) != 0.0;
        if (isString()) return !std::get<std::string>(v).empty();
        if (isList()) return !asList().empty();
        return false;
    }
    const std::string& asString() const { return std::get<std::string>(v); }
    std::string& asString() { return std::get<std::string>(v); }

    RawList& asList() { return *std::get<ListPtr>(v); }
    const RawList& asList() const { return *std::get<ListPtr>(v); }

    static double asNumeric(const Value& v) {
        if (v.isNumber()) return v.asNumber();
        if (v.isBool()) return static_cast<double>(v.asBool());
        throw std::runtime_error("Expected a number or bool but got '" + v.typeName() + "'");
    }

    FunctionValue asFunc() const { return std::get<FunctionValue>(v); }

    std::string toString() const;
    std::string typeName() const;

    friend Value operator+(const Value& a, const Value& b);
    friend Value operator-(const Value& a, const Value& b);
    friend Value operator*(const Value& a, const Value& b);
    friend Value operator/(const Value& a, const Value& b);
    friend Value operator%(const Value& a, const Value& b);
    friend Value operator^(const Value& a, const Value& b);

    friend bool operator==(const Value& a, const Value& b);
    friend bool operator!=(const Value& a, const Value& b);
    friend bool operator<(const Value& a, const Value& b);
    friend bool operator<=(const Value& a, const Value& b);
    friend bool operator>(const Value& a, const Value& b);
    friend bool operator>=(const Value& a, const Value& b);

    friend Value operator&&(const Value& a, const Value& b);
    friend Value operator||(const Value& a, const Value& b);
    friend Value operator!(const Value& a);

    Value atIndex(int idx) const;
    Value slice(std::optional<int> begin, std::optional<int> end) const;

   private:
    Variant v;
};
