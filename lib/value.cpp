#include "value.h"

#include <iostream>

#include "AST.h"
#include "environment.h"

std::vector<std::string> g_callStack;

std::string Value::toString() const {
    return std::visit(overloaded{
                          [](std::monostate) -> std::string { return "nil"; },
                          [](double d) -> std::string {
                              if (std::floor(d) == d) return std::to_string((long long)d);
                              std::ostringstream oss;
                              oss << d;
                              return oss.str();
                          },
                          [](bool b) -> std::string { return b ? "true" : "false"; },
                          [](const std::string& s) -> std::string { return s; },
                          [](const Value::ListPtr& vecPtr) -> std::string {
                              const auto& vec = *vecPtr;
                              std::ostringstream oss;
                              if (vec.size() != 1) oss << '[';
                              for (size_t i = 0; i < vec.size(); ++i) {
                                  if (i) oss << ", ";
                                  if (vec[i].isString())
                                      oss << '"' << vec[i].toString() << '"';
                                  else
                                      oss << vec[i].toString();
                              }
                              if (vec.size() != 1) oss << ']';
                              return oss.str();
                          },
                          [](const FunctionValue&) -> std::string { return "<function>"; }},
                      v);
}

std::string Value::typeName() const {
    return std::visit(overloaded{
                          [](std::monostate) -> std::string { return "null"; },
                          [](double) -> std::string { return "number"; },
                          [](bool) -> std::string { return "bool"; },
                          [](const std::string&) -> std::string { return "string"; },
                          [](const Value::ListPtr&) -> std::string { return "list"; },
                          [](const FunctionValue&) -> std::string { return "function"; }},
                      v);
}

Value FunctionValue::invoke(const std::vector<Value>& args) const {
    if (isBuiltin) {
        return builtinFn(args);
    }

    const auto& proto = fnAST->getProto();
    const auto& names = proto.getArgs();
    if (args.size() != names.size()) {
        throw std::runtime_error(
            "Function '" + proto.getName() +
            "' expects " + std::to_string(names.size()) +
            " arguments, got " + std::to_string(args.size()));
    }

    g_callStack.push_back(proto.getName());

    auto activationEnv = std::make_shared<Environment>(closure);
    for (size_t i = 0; i < args.size(); ++i) {
        activationEnv->set(names[i], args[i]);
    }

    try {
        fnAST->getBody().eval(*activationEnv);
        g_callStack.pop_back();
        return Value{};
    } catch (const ReturnException& ret) {
        g_callStack.pop_back();
        return ret.value;
    }
}

Value operator+(Value const& a, Value const& b) {
    if (a.isString() && b.isString())
        return Value(a.asString() + b.asString());

    if (a.isList() && b.isList()) {
        auto r = a.asList();
        auto const& rhs = b.asList();
        r.insert(r.end(), rhs.begin(), rhs.end());
        return Value(std::move(r));
    }

    if (a.isBool() && b.isBool())
        return Value(a.asBool() || b.asBool());

    double da = Value::asNumeric(a);
    double db = Value::asNumeric(b);
    return Value(da + db);
}

Value operator-(Value const& a, Value const& b) {
    if (a.isString() && b.isString()) {
        auto s = a.asString(), suf = b.asString();
        if (s.size() >= suf.size() &&
            s.compare(s.size() - suf.size(), suf.size(), suf) == 0)
            s.erase(s.size() - suf.size(), suf.size());
        return Value(std::move(s));
    }

    double da = Value::asNumeric(a);
    double db = Value::asNumeric(b);
    return Value(da - db);
}

Value operator*(Value const& a, Value const& b) {
    if (a.isString() && (b.isNumber() || b.isBool())) {
        std::string res;
        std::string s = a.asString();
        double times = Value::asNumeric(b);
        int full = static_cast<int>(times);
        for (int i = 0; i < full; ++i) res += s;
        double frac = times - full;
        int cut = static_cast<int>(frac * s.size());
        res += s.substr(0, cut);
        return Value(std::move(res));
    }
    if ((a.isNumber() || a.isBool()) && b.isString())
        return operator*(b, a);

    if (a.isList() && (b.isNumber() || b.isBool())) {
        std::vector<Value> res;
        auto lst = a.asList();
        double times = Value::asNumeric(b);
        int full = static_cast<int>(times);
        for (int i = 0; i < full; ++i)
            res.insert(res.end(), lst.begin(), lst.end());
        int cut = static_cast<int>((times - full) * lst.size());
        res.insert(res.end(), lst.begin(), lst.begin() + cut);
        return Value(std::move(res));
    }
    if ((a.isNumber() || a.isBool()) && b.isList())
        return operator*(b, a);

    double da = Value::asNumeric(a);
    double db = Value::asNumeric(b);
    return Value(da * db);
}

Value operator/(Value const& a, Value const& b) {
    double da = Value::asNumeric(a);
    double db = Value::asNumeric(b);
    if (db == 0.0) throw std::runtime_error("Division by zero");
    return Value(da / db);
}

Value operator%(Value const& a, Value const& b) {
    double da = Value::asNumeric(a);
    double db = Value::asNumeric(b);
    if (db == 0.0)
        throw std::runtime_error("Division by zero");
    double rem = std::fmod(da, db);
    return Value(rem);
}

Value operator^(Value const& a, Value const& b) {
    double da = Value::asNumeric(a);
    double db = Value::asNumeric(b);
    return Value(std::pow(da, db));
}

bool operator==(Value const& a, Value const& b) {
    if (a.isNil() || b.isNil())
        return a.isNil() && b.isNil();

    if (a.isString() && b.isString())
        return a.asString() == b.asString();

    if (a.isList() && b.isList())
        return a.asList() == b.asList();

    if ((a.isNumber() || a.isBool()) && (b.isNumber() || b.isBool()))
        return Value::asNumeric(a) == Value::asNumeric(b);

    if (a.isFunc() && b.isFunc())
        return false;

    return false;
}

bool operator!=(Value const& a, Value const& b) { return !(a == b); }

bool operator<(Value const& a, Value const& b) {
    if (a.isNil() || b.isNil()) return false;

    if (a.isString() && b.isString())
        return a.asString() < b.asString();

    if (a.isList() && b.isList())
        return a.asList() < b.asList();

    if ((a.isNumber() || a.isBool()) && (b.isNumber() || b.isBool()))
        return Value::asNumeric(a) < Value::asNumeric(b);

    throw std::runtime_error(
        "Can't compare '" + a.typeName() + "' и '" + b.typeName() + "'");
}

bool operator<=(Value const& a, Value const& b) { return (a < b) || (a == b); }
bool operator>(Value const& a, Value const& b) { return !(a <= b); }
bool operator>=(Value const& a, Value const& b) { return !(a < b); }

Value operator&&(Value const& a, Value const& b) {
    if (!a.isBool() || !b.isBool())
        throw std::runtime_error("&& only applies to bool");
    return Value(a.asBool() && b.asBool());
}
Value operator||(Value const& a, Value const& b) {
    if (!a.isBool() || !b.isBool())
        throw std::runtime_error("|| only applies to bool");
    return Value(a.asBool() || b.asBool());
}
Value operator!(Value const& a) {
    if (!a.isBool()) throw std::runtime_error("! only applies to bool");
    return Value(!a.asBool());
}

Value Value::atIndex(int idx) const {
    if (isString()) {
        auto s = asString();
        int i = Value::normalizeIndex(idx, (int)s.size());
        return Value(std::string(1, s[i]));
    }
    if (isList()) {
        auto lst = asList();
        int i = Value::normalizeIndex(idx, (int)lst.size());
        return lst[i];
    }
    throw std::runtime_error("Type '" + typeName() + "' is not subscriptable");
}

Value Value::slice(std::optional<int> obegin, std::optional<int> oend) const {
    if (isString()) {
        auto s = asString();
        int n = (int)s.size();
        int b = obegin.value_or(0), e = oend.value_or(n);
        if (b < 0) b += n;
        if (e < 0) e += n;
        b = std::clamp(b, 0, n);
        e = std::clamp(e, 0, n);
        if (e < b) e = b;
        return Value(s.substr(b, e - b));
    }

    if (isList()) {
        auto lst = asList();
        int n = (int)lst.size();
        int b = obegin.value_or(0), e = oend.value_or(n);
        if (b < 0) b += n;
        if (e < 0) e += n;
        b = std::clamp(b, 0, n);
        e = std::clamp(e, 0, n);
        if (e < b) e = b;
        RawList out(lst.begin() + b, lst.begin() + e);
        return Value(std::move(out));
    }

    throw std::runtime_error("Type '" + typeName() + "' is not sliceable");
}
