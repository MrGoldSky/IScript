#pragma once
#include <memory>
#include <string>
#include <vector>

#include "environment.h"
#include "value.h"
#include "token.h"

static const char* TokenTypeToString(TokenType type);

struct BreakException {};
struct ContinueException {};
struct ReturnException {
    Value value;
    explicit ReturnException(Value v) : value(std::move(v)) {}
};

class ExprAST {
   public:
    virtual ~ExprAST() = default;
    virtual Value eval(Environment& env) const = 0;
};

class NumberExprAST : public ExprAST {
    double Val;

   public:
    NumberExprAST(double V) : Val(V) {}
    Value eval(Environment& env) const override {
        return Value(Val);
    }
};

class VariableExprAST : public ExprAST {
    std::string Name;

   public:
    VariableExprAST(const std::string& N) : Name(N) {}
    Value eval(Environment& env) const override {
        return env.get(Name);
    }
    std::string& getName() {
        return Name;
    }
};

class BinaryExprAST : public ExprAST {
    TokenType Op;
    std::unique_ptr<ExprAST> LHS, RHS;

   public:
    BinaryExprAST(TokenType op,
                  std::unique_ptr<ExprAST> lhs,
                  std::unique_ptr<ExprAST> rhs)
        : Op(op), LHS(std::move(lhs)), RHS(std::move(rhs)) {}

    Value eval(Environment& env) const override {
        Value L = LHS->eval(env);
        Value R = RHS->eval(env);
        switch (Op) {
            // арифметика
            case TokenType::Plus:
                return L + R;
            case TokenType::Minus:
                return L - R;
            case TokenType::Star:
                return L * R;
            case TokenType::Slash:
                return L / R;
            case TokenType::Percent:
                return L % R;
            case TokenType::Caret:
                return L ^ R;

            // сравнения
            case TokenType::Less:
                return L < R;
            case TokenType::LessEqual:
                return L <= R;
            case TokenType::Greater:
                return L > R;
            case TokenType::GreaterEqual:
                return L >= R;
            case TokenType::Equal:
                return L == R;
            case TokenType::NotEqual:
                return L != R;

            // Логические
            case TokenType::And:
                return L && R;
            case TokenType::Or:
                return L || R;

            default:
                throw std::runtime_error(std::string("Unknown binary operator ") + TokenTypeToString(Op));
        }
    }
};

class UnaryExprAST : public ExprAST {
    char Op;
    std::unique_ptr<ExprAST> Operand;

   public:
    UnaryExprAST(char op, std::unique_ptr<ExprAST> operand)
        : Op(op), Operand(std::move(operand)) {}

    Value eval(Environment& env) const override {
        Value V = Operand->eval(env);
        switch (Op) {
            case '+':
                return V;
            case '-':
                return Value(0.0) - V;
            case '!':
                return Value(!V.asBool());
            default:
                throw std::runtime_error(std::string("Unknown unary operator ") + Op);
        }
    }
};

class CallExprAST : public ExprAST {
    std::unique_ptr<ExprAST> CalleeExpr;
    std::vector<std::unique_ptr<ExprAST>> Args;

   public:
    CallExprAST(std::unique_ptr<ExprAST> callee, std::vector<std::unique_ptr<ExprAST>> args)
        : CalleeExpr(std::move(callee)), Args(std::move(args)) {}

    Value eval(Environment& env) const override {
        Value calleeVal = CalleeExpr->eval(env);
        if (!calleeVal.isFunc())
            throw std::runtime_error("Attempt to call a non-function value");

        std::vector<Value> argVals;
        for (auto& arg : Args) {
            argVals.push_back(arg->eval(env));
        }
        return calleeVal.asFunc().invoke(argVals);
    }
};

class PrototypeAST {
    std::string Name;
    std::vector<std::string> Args;

   public:
    PrototypeAST(const std::string& name, std::vector<std::string> args)
        : Name(name), Args(std::move(args)) {}

    const std::string& getName() const { return Name; }
    void setName(const std::string& newName) { Name = newName; }
    const std::vector<std::string>& getArgs() const { return Args; }
};

class FunctionAST {
    std::unique_ptr<PrototypeAST> Proto;
    std::unique_ptr<ExprAST> Body;

   public:
    FunctionAST(std::unique_ptr<PrototypeAST> proto,
                std::unique_ptr<ExprAST> body)
        : Proto(std::move(proto)), Body(std::move(body)) {}

    const PrototypeAST& getProto() const { return *Proto; }
    PrototypeAST& getProto() { return *Proto; }
    ExprAST& getBody() const { return *Body; }
};

class AssignmentExprAST : public ExprAST {
    std::string VarName;
    std::unique_ptr<ExprAST> Expr;

   public:
    AssignmentExprAST(const std::string& name,
                      std::unique_ptr<ExprAST> expr)
        : VarName(name), Expr(std::move(expr)) {}

    Value eval(Environment& env) const override {
        Value v = Expr->eval(env);

        env.set(VarName, v);
        if (v.isFunc()) {
            FunctionValue fv = v.asFunc();
            fv.closure->set(VarName, v);
        }
        return v;
    }
};

class StringExprAST : public ExprAST {
    std::string Val;

   public:
    StringExprAST(const std::string& V) : Val(V) {}
    Value eval(Environment& env) const override {
        return Value(Val);
    }
};

class BooleanExprAST : public ExprAST {
    bool Val;

   public:
    BooleanExprAST(bool V) : Val(V) {}
    Value eval(Environment& env) const override {
        return Value(Val);
    }
};

class ListExprAST : public ExprAST {
    std::vector<std::unique_ptr<ExprAST>> Elements;

   public:
    ListExprAST(std::vector<std::unique_ptr<ExprAST>> Elems)
        : Elements(std::move(Elems)) {}
    Value eval(Environment& env) const override {
        Value::RawList vals;
        for (auto& E : Elements)
            vals.push_back(E->eval(env));
        return Value(vals);
    }
};

class FunctionLiteralExprAST : public ExprAST {
    std::unique_ptr<FunctionAST> FnAST;

   public:
    FunctionLiteralExprAST(std::vector<std::string> A,
                           std::unique_ptr<ExprAST> B)
        : FnAST(std::make_unique<FunctionAST>(
              std::make_unique<PrototypeAST>("", std::move(A)),
              std::move(B))) {}

    Value eval(Environment& env) const override {
        auto newEnv = std::make_shared<Environment>(env);
        return Value(FunctionValue{FnAST.get(), std::move(newEnv)});
    }
    FunctionAST* getFunctionAST() const { return FnAST.get(); }
};

// Префиксный ++x или --x
class PrefixExprAST : public ExprAST {
    bool IsIncrement;
    std::unique_ptr<ExprAST> Operand;

   public:
    PrefixExprAST(bool inc, std::unique_ptr<ExprAST> op)
        : IsIncrement(inc), Operand(std::move(op)) {}

    Value eval(Environment& env) const override {
        auto* var = dynamic_cast<VariableExprAST*>(Operand.get());
        if (!var) throw std::runtime_error("Operand of prefix ++/-- must be a variable");
        Value v = env.get(var->getName());
        double d = Value::asNumeric(v);
        d += IsIncrement ? 1 : -1;
        env.set(var->getName(), Value(d));
        return Value(d);
    }
};

// Постфиксный x++ или x--
class PostfixExprAST : public ExprAST {
    bool IsIncrement;
    std::unique_ptr<ExprAST> Operand;

   public:
    PostfixExprAST(bool inc, std::unique_ptr<ExprAST> op)
        : IsIncrement(inc), Operand(std::move(op)) {}

    Value eval(Environment& env) const override {
        auto* var = dynamic_cast<VariableExprAST*>(Operand.get());
        if (!var) throw std::runtime_error("Operand of postfix ++/-- must be a variable");
        Value old = env.get(var->getName());
        double d = Value::asNumeric(old);
        d += IsIncrement ? 1 : -1;
        env.set(var->getName(), Value(d));
        return old;
    }
};

class CompoundAssignmentExprAST : public ExprAST {
    TokenType Op;
    std::string VarName;
    std::unique_ptr<ExprAST> RHS;

   public:
    CompoundAssignmentExprAST(TokenType op,
                              std::string name,
                              std::unique_ptr<ExprAST> rhs)
        : Op(op), VarName(std::move(name)), RHS(std::move(rhs)) {}

    Value eval(Environment& env) const override {
        Value old = env.get(VarName);
        Value right = RHS->eval(env);
        Value result;
        switch (Op) {
            case TokenType::PlusAssign:
                result = old + right;
                break;
            case TokenType::MinusAssign:
                result = old - right;
                break;
            case TokenType::StarAssign:
                result = old * right;
                break;
            case TokenType::SlashAssign:
                result = old / right;
                break;
            case TokenType::PercentAssign: {
                double a = Value::asNumeric(old),
                       b = Value::asNumeric(right);
                result = Value(std::fmod(a, b));
                break;
            }
            case TokenType::CaretAssign:
                result = old ^ right;
                break;
            default:
                throw std::runtime_error("Unknown compound assignment operator");
        }
        env.set(VarName, result);
        return result;
    }
};

class IndexExprAST : public ExprAST {
    std::unique_ptr<ExprAST> Base, Index;

   public:
    IndexExprAST(std::unique_ptr<ExprAST> B,
                 std::unique_ptr<ExprAST> I)
        : Base(std::move(B)), Index(std::move(I)) {}

    Value eval(Environment& env) const override {
        Value V = Base->eval(env);
        int i = static_cast<int>(Index->eval(env).asNumber());
        return V.atIndex(i);
    }
};

class SliceExprAST : public ExprAST {
    std::unique_ptr<ExprAST> Base, Start, End;

   public:
    SliceExprAST(std::unique_ptr<ExprAST> B,
                 std::unique_ptr<ExprAST> S,
                 std::unique_ptr<ExprAST> E)
        : Base(std::move(B)), Start(std::move(S)), End(std::move(E)) {}

    Value eval(Environment& env) const override {
        Value V = Base->eval(env);
        std::optional<int> b, e;
        if (Start) b = static_cast<int>(Start->eval(env).asNumber());
        if (End) e = static_cast<int>(End->eval(env).asNumber());
        return V.slice(b, e);
    }
};

class NilExprAST : public ExprAST {
   public:
    NilExprAST() {}
    Value eval(Environment& env) const override {
        return Value();
    }
};

class IfExprAST : public ExprAST {
    std::unique_ptr<ExprAST> Cond, Then, Else;

   public:
    IfExprAST(std::unique_ptr<ExprAST> Cond,
              std::unique_ptr<ExprAST> Then,
              std::unique_ptr<ExprAST> Else)
        : Cond(std::move(Cond)),
          Then(std::move(Then)),
          Else(std::move(Else)) {}

    void setElse(std::unique_ptr<ExprAST> E) { Else = std::move(E); }
    ExprAST* getElse() const { return Else.get(); }

    Value eval(Environment& env) const override {
        bool c = Cond->eval(env).asBool();
        if (c)
            return Then->eval(env);
        else if (Else)
            return Else->eval(env);
        return Value();
    }
};

class WhileExprAST : public ExprAST {
    std::unique_ptr<ExprAST> Cond, Body;

   public:
    WhileExprAST(std::unique_ptr<ExprAST> cond,
                 std::unique_ptr<ExprAST> body)
        : Cond(std::move(cond)), Body(std::move(body)) {}
    Value eval(Environment& env) const override {
        Value result;
        while (Cond->eval(env).asBool()) {
            try {
                result = Body->eval(env);
            } catch (const ContinueException&) {
                continue;
            } catch (const BreakException&) {
                break;
            }
        }
        return result;
    }
};

class ForExprAST : public ExprAST {
    std::string VarName;
    std::unique_ptr<ExprAST> SeqExpr, Body;

   public:
    ForExprAST(std::string var,
               std::unique_ptr<ExprAST> seq,
               std::unique_ptr<ExprAST> body)
        : VarName(std::move(var)),
          SeqExpr(std::move(seq)),
          Body(std::move(body)) {}
    Value eval(Environment& env) const override {
        Value seqV = SeqExpr->eval(env);
        if (!seqV.isList())
            throw std::runtime_error("For: ожидается список в выражении 'in'");
        auto list = seqV.asList();
        Value result;
        for (auto& el : list) {
            env.set(VarName, el);
            try {
                result = Body->eval(env);
            } catch (const ContinueException&) {
                continue;
            } catch (const BreakException&) {
                break;
            }
        }
        return result;
    }
};

class BreakExprAST : public ExprAST {
   public:
    Value eval(Environment&) const override {
        throw BreakException();
    }
};

class ContinueExprAST : public ExprAST {
   public:
    Value eval(Environment&) const override {
        throw ContinueException();
    }
};

class InExprAST : public ExprAST {
    std::unique_ptr<ExprAST> L, R;

   public:
    InExprAST(std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs)
        : L(std::move(lhs)), R(std::move(rhs)) {}

    Value eval(Environment& env) const override {
        std::string hay = L->eval(env).asString();
        std::string needle = R->eval(env).asString();
        return Value(hay.find(needle) != std::string::npos);
    }
};

class BlockExprAST : public ExprAST {
    std::vector<std::unique_ptr<ExprAST>> Stmts;

   public:
    BlockExprAST(std::vector<std::unique_ptr<ExprAST>> stmts)
        : Stmts(std::move(stmts)) {}
    Value eval(Environment& env) const override {
        Value last;
        for (auto& s : Stmts)
            last = s->eval(env);
        return last;
    }
};

class ReturnExprAST : public ExprAST {
    std::unique_ptr<ExprAST> Expr;

   public:
    explicit ReturnExprAST(std::unique_ptr<ExprAST> expr)
        : Expr(std::move(expr)) {}

    Value eval(Environment& env) const override {
        Value v = Expr->eval(env);
        throw ReturnException(v);
    }
};
