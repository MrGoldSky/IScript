#include "parser.h"

#include <cstdio>
#include <iostream>

static const char* TokenTypeToString(TokenType type) {
    switch (type) {
        // end of input
        case TokenType::EndOfFile:
            return "EndOfFile";

        // Типы
        case TokenType::Identifier:
            return "Identifier";
        case TokenType::Number:
            return "Number";
        case TokenType::Boolean:
            return "Boolean";
        case TokenType::String:
            return "String";

        // Операторы
        case TokenType::Plus:
            return "Plus";  // +
        case TokenType::Minus:
            return "Minus";  // -
        case TokenType::Star:
            return "Star";  // *
        case TokenType::Slash:
            return "Slash";  // /
        case TokenType::Percent:
            return "Percent";  // %
        case TokenType::Caret:
            return "Caret";  // ^

        case TokenType::Assign:
            return "Assign";  // =
        case TokenType::Bang:
            return "Bang";  // !
        case TokenType::Less:
            return "Less";  // <
        case TokenType::Greater:
            return "Greater";  // >

        // Составные операторы
        case TokenType::PlusPlus:
            return "PlusPlus";  // ++
        case TokenType::MinusMinus:
            return "MinusMinus";  // --
        case TokenType::PlusAssign:
            return "PlusAssign";  // +=
        case TokenType::MinusAssign:
            return "MinusAssign";  // -=
        case TokenType::StarAssign:
            return "StarAssign";  // *=
        case TokenType::SlashAssign:
            return "SlashAssign";  // /=
        case TokenType::PercentAssign:
            return "PercentAssign";  // %=
        case TokenType::CaretAssign:
            return "CaretAssign";  // ^=

        case TokenType::Equal:
            return "Equal";  // ==
        case TokenType::NotEqual:
            return "NotEqual";  // !=
        case TokenType::LessEqual:
            return "LessEqual";  // <=
        case TokenType::GreaterEqual:
            return "GreaterEqual";  // >=

        // Разделители
        case TokenType::LParen:
            return "LParen";  // (
        case TokenType::RParen:
            return "RParen";  // )
        case TokenType::LBracket:
            return "LBracket";  // [
        case TokenType::RBracket:
            return "RBracket";  // ]
        case TokenType::Comma:
            return "Comma";  // ,
        case TokenType::Semicolon:
            return "Semicolon";  // ;
        case TokenType::At:
            return "At";  // @

        // Ключевые слова
        case TokenType::If:
            return "If";
        case TokenType::Then:
            return "Then";
        case TokenType::Else:
            return "Else";
        case TokenType::End:
            return "End";

        case TokenType::While:
            return "While";
        case TokenType::For:
            return "For";
        case TokenType::Break:
            return "Break";
        case TokenType::Continue:
            return "Continue";
        case TokenType::In:
            return "In";

        case TokenType::Function:
            return "Function";
        case TokenType::Return:
            return "Return";

        default:
            return "Unknown";
    }
}

std::unique_ptr<ExprAST> Parser::LogError(const char* msg) {
    fprintf(stderr, "Error at line %d: %s\n", CurTok.line, msg);
    return nullptr;
}
std::unique_ptr<PrototypeAST> Parser::LogErrorP(const char* msg) {
    LogError(msg);
    return nullptr;
}

Parser::Parser(Lexer& lex) : Lex(lex) {
    // 0) Булевые операторы
    BinopPrecedence[TokenType::And] = 5;
    BinopPrecedence[TokenType::Or] = 4;

    // 1) Сравнения <, <=, >, >=
    BinopPrecedence[TokenType::Less] = 10;
    BinopPrecedence[TokenType::LessEqual] = 10;
    BinopPrecedence[TokenType::Greater] = 10;
    BinopPrecedence[TokenType::GreaterEqual] = 10;

    // 2) Равенство ==, !=
    BinopPrecedence[TokenType::Equal] = 9;
    BinopPrecedence[TokenType::NotEqual] = 9;

    // 3) Сложение/вычитание +, -
    BinopPrecedence[TokenType::Plus] = 20;
    BinopPrecedence[TokenType::Minus] = 20;

    // 4) Умножение/деление/остаток *, /, %
    BinopPrecedence[TokenType::Star] = 40;
    BinopPrecedence[TokenType::Slash] = 40;
    BinopPrecedence[TokenType::Percent] = 40;

    // 5) Возведение в степень ^
    BinopPrecedence[TokenType::Caret] = 50;

    getNextToken();
}

int Parser::GetTokPrecedence() {
    auto it = BinopPrecedence.find(CurTok.type);
    if (it == BinopPrecedence.end())
        return -1;
    return it->second;
}

std::unique_ptr<ExprAST> Parser::ParseNumberExpr() {
    auto Result = std::make_unique<NumberExprAST>(std::get<double>(CurTok.literal));
    getNextToken();
    return Result;
}

std::unique_ptr<ExprAST> Parser::ParseParenExpr() {
    getNextToken();
    auto V = ParseExpression();
    if (!V) return nullptr;
    if (CurTok.type != TokenType::RParen)
        return LogError("expected ')'");
    getNextToken();
    return V;
}

std::unique_ptr<ExprAST> Parser::ParseIdentifierExpr() {
    std::string IdName = CurTok.lexeme;
    getNextToken();
    return std::make_unique<VariableExprAST>(IdName);
}

std::unique_ptr<ExprAST> Parser::ParseCallExpr(std::unique_ptr<ExprAST> Callee) {
    getNextToken();

    std::vector<std::unique_ptr<ExprAST>> Args;
    if (CurTok.type != TokenType::RParen) {
        while (true) {
            auto Arg = ParseExpression();
            if (!Arg) return nullptr;
            Args.push_back(std::move(Arg));

            if (CurTok.type == TokenType::Comma) {
                getNextToken();
                if (CurTok.type == TokenType::RParen)
                    break;
                else
                    continue;
            }
            break;
        }
    }

    if (CurTok.type != TokenType::RParen)
        return LogError("Expected ')' in function call");
    getNextToken();

    return std::make_unique<CallExprAST>(std::move(Callee), std::move(Args));
}

std::unique_ptr<ExprAST> Parser::ParsePrimary() {
    std::unique_ptr<ExprAST> E;
    switch (CurTok.type) {
        case TokenType::If:
            E = ParseIfExpr();
            break;
        case TokenType::While:
            E = ParseWhileExpr();
            break;
        case TokenType::For:
            E = ParseForExpr();
            break;
        case TokenType::In:
            E = ParseInExpr(std::move(E));
            break;
        case TokenType::Break:
            E = ParseBreakExpr();
            break;
        case TokenType::Return:
            E = ParseReturnExpr();
            break;
        case TokenType::Function:
            E = ParseFunctionExpr();
            break;
        case TokenType::Continue:
            E = ParseContinueExpr();
            break;
        case TokenType::Identifier:
            E = ParseIdentifierExpr();
            break;
        case TokenType::Number:
            E = ParseNumberExpr();
            break;
        case TokenType::LParen:
            E = ParseParenExpr();
            break;
        case TokenType::LBracket:
            E = ParseListExpr();
            break;
        case TokenType::RParen:
            return LogError("unexpected ')'");
        case TokenType::Boolean:
            E = ParseBooleanExpr();
            break;
        case TokenType::String:
            E = ParseStringExpr();
            break;
        case TokenType::EndOfFile:
            return nullptr;
        case TokenType::Nil:
            E = ParseNilExpr();
            break;
        default: {
            std::cerr << "[Debug] Parser::ParsePrimary - unexpected token '"
                      << CurTok.lexeme << "' of type "
                      << TokenTypeToString(CurTok.type) << std::endl;
            return LogError("unknown token when expecting an expression");
        }
    }

    while (true) {
        if (CurTok.type == TokenType::LParen) {
            // преобразуем E в вызов E(...)
            E = ParseCallExpr(std::move(E));
        } else if (CurTok.type == TokenType::LBracket) {
            // универсальный сабскрипт / слайс E[...]
            E = ParseStringSlice(std::move(E));
        } else if (CurTok.type == TokenType::In) {
            getNextToken();
            auto RHS = ParseExpression();
            if (!RHS) return nullptr;
            E = std::make_unique<InExprAST>(std::move(E), std::move(RHS));
        } else {
            break;
        }
    }

    while (CurTok.type == TokenType::PlusPlus || CurTok.type == TokenType::MinusMinus) {
        bool inc = CurTok.type == TokenType::PlusPlus;
        getNextToken();
        E = std::make_unique<PostfixExprAST>(inc, std::move(E));
    }
    return E;
}

std::unique_ptr<ExprAST> Parser::ParseUnary() {
    if (CurTok.type == TokenType::Not) {
        getNextToken();
        auto operand = ParseUnary();
        return std::make_unique<UnaryExprAST>('!',
                                              std::move(operand));
    }

    // Префиксный ++ и --
    if (CurTok.type == TokenType::PlusPlus || CurTok.type == TokenType::MinusMinus) {
        bool inc = CurTok.type == TokenType::PlusPlus;
        getNextToken();  // съели ++/--
        auto operand = ParseUnary();
        if (!operand) return nullptr;
        return std::make_unique<PrefixExprAST>(inc, std::move(operand));
    }
    // Унарные + и -
    if (CurTok.type == TokenType::Plus || CurTok.type == TokenType::Minus) {
        char op = CurTok.lexeme[0];
        getNextToken();
        auto operand = ParseUnary();
        if (!operand) return nullptr;
        return std::make_unique<UnaryExprAST>(op, std::move(operand));
    }
    return ParsePrimary();
}

std::unique_ptr<ExprAST> Parser::ParseExpression() {
    auto LHS = ParseUnary();
    if (!LHS) return nullptr;

    if (CurTok.type == TokenType::Assign || CurTok.type == TokenType::PlusAssign || CurTok.type == TokenType::MinusAssign || CurTok.type == TokenType::StarAssign || CurTok.type == TokenType::SlashAssign || CurTok.type == TokenType::PercentAssign || CurTok.type == TokenType::CaretAssign) {
        TokenType op = CurTok.type;
        int assignLine = CurTok.line;
        getNextToken();

        if (CurTok.type == TokenType::EndOfFile || CurTok.type == TokenType::RBracket || CurTok.type == TokenType::RParen || CurTok.type == TokenType::Comma || CurTok.type == TokenType::Semicolon) {
            return LogError("Expected expression after '=' on the same line");
        }

        if (CurTok.line > assignLine) {
            return LogError("Expected expression after '=' on the same line");
        }

        auto RHS = ParseExpression();
        if (!RHS) return nullptr;

        auto* var = dynamic_cast<VariableExprAST*>(LHS.get());
        if (!var)
            return LogError("left side of assignment must be a variable");

        std::string varName = var->getName();
        if (auto* funcLit = dynamic_cast<FunctionLiteralExprAST*>(RHS.get())) {
            funcLit->getFunctionAST()->getProto().setName(varName);
        }

        if (op == TokenType::Assign) {
            return std::make_unique<AssignmentExprAST>(
                var->getName(), std::move(RHS));
        } else {
            return std::make_unique<CompoundAssignmentExprAST>(
                op, var->getName(), std::move(RHS));
        }
    }

    return ParseBinOpRHS(0, std::move(LHS));
}

std::unique_ptr<ExprAST> Parser::ParseBinOpRHS(int exprPrec, std::unique_ptr<ExprAST> LHS) {
    while (true) {
        int tokPrec = GetTokPrecedence();
        if (tokPrec < exprPrec) return LHS;

        TokenType binOp = CurTok.type;
        getNextToken();

        auto RHS = ParsePrimary();
        if (!RHS) return nullptr;

        int nextPrec = GetTokPrecedence();
        if (tokPrec < nextPrec) {
            RHS = ParseBinOpRHS(tokPrec + 1, std::move(RHS));
            if (!RHS) return nullptr;
        }

        LHS = std::make_unique<BinaryExprAST>(binOp, std::move(LHS), std::move(RHS));
    }
}

std::unique_ptr<PrototypeAST> Parser::ParsePrototype() {
    if (CurTok.type != TokenType::Identifier)
        return LogErrorP("Expected function name in prototype");
    std::string FnName = CurTok.lexeme;
    getNextToken();

    if (CurTok.type != TokenType::LParen)
        return LogErrorP("Expected '(' in prototype");
    getNextToken();

    std::vector<std::string> ArgNames;
    while (CurTok.type == TokenType::Identifier) {
        ArgNames.push_back(CurTok.lexeme);
        getNextToken();
        if (CurTok.type == TokenType::Comma)
            getNextToken();
        else
            break;
    }

    if (CurTok.type != TokenType::RParen)
        return LogErrorP("Expected ')' in prototype");
    getNextToken();

    return std::make_unique<PrototypeAST>(FnName, std::move(ArgNames));
}

std::unique_ptr<FunctionAST> Parser::ParseDefinition() {
    getNextToken();
    auto Proto = ParsePrototype();
    if (!Proto) return nullptr;

    auto BlockBody = ParseBlockUntil(TokenType::End, TokenType::Function);
    if (!BlockBody) return nullptr;

    return std::make_unique<FunctionAST>(std::move(Proto), std::move(BlockBody));
}

std::unique_ptr<FunctionAST> Parser::ParseTopLevelExpr() {
    auto E = ParseExpression();
    if (!E) return nullptr;
    auto Proto = std::make_unique<PrototypeAST>("__anon_expr", std::vector<std::string>{});
    return std::make_unique<FunctionAST>(std::move(Proto), std::move(E));
}

bool Parser::parseModule(
    std::vector<std::unique_ptr<FunctionAST>>& Out) {
    while (CurTok.type != TokenType::EndOfFile) {
        if (CurTok.type == TokenType::Function) {
            if (auto Fn = ParseDefinition())
                Out.push_back(std::move(Fn));
            else
                return false;
        } else {
            if (auto Fn = ParseTopLevelExpr())
                Out.push_back(std::move(Fn));
            else
                return false;
        }
    }
    return true;
}

std::unique_ptr<ExprAST> Parser::ParseStringExpr() {
    auto Val = std::get<std::string>(CurTok.literal);
    getNextToken();
    std::unique_ptr<ExprAST> Expr = std::make_unique<StringExprAST>(Val);
    return Expr;
}

std::unique_ptr<ExprAST> Parser::ParseBooleanExpr() {
    bool Val = std::get<bool>(CurTok.literal);
    getNextToken();
    return std::make_unique<BooleanExprAST>(Val);
}

std::unique_ptr<ExprAST> Parser::ParseListExpr() {
    getNextToken();

    std::vector<std::unique_ptr<ExprAST>> Elements;

    if (CurTok.type != TokenType::RBracket) {
        while (true) {
            auto Element = ParseExpression();
            if (!Element) return nullptr;
            Elements.push_back(std::move(Element));

            // Trailing comma
            if (CurTok.type == TokenType::Comma) {
                getNextToken();

                if (CurTok.type == TokenType::RBracket) {
                    break;
                }
                continue;
            } else {
                break;
            }
        }
    }

    if (CurTok.type != TokenType::RBracket)
        return LogError("Expected ']' after list literal");
    getNextToken();

    return std::make_unique<ListExprAST>(std::move(Elements));
}

std::unique_ptr<ExprAST> Parser::ParseStringSlice(std::unique_ptr<ExprAST> StrExpr) {
    bool sawColon = false;
    getNextToken();

    std::unique_ptr<ExprAST> StartExpr;
    if (CurTok.type != TokenType::Colon) {
        if (!(StartExpr = ParseExpression()))
            return nullptr;
    }

    std::unique_ptr<ExprAST> EndExpr;
    if (CurTok.type == TokenType::Colon) {
        sawColon = true;
        getNextToken();
        if (CurTok.type != TokenType::RBracket) {
            if (!(EndExpr = ParseExpression()))
                return nullptr;
        }
    }

    if (CurTok.type != TokenType::RBracket)
        return LogError("Expected ']' after index or slice");
    getNextToken();

    if (!sawColon) {
        // Нет ':' — это просто один индекс
        return std::make_unique<IndexExprAST>(
            std::move(StrExpr),
            std::move(StartExpr));
    } else {
        // Есть ':' — это срез
        return std::make_unique<SliceExprAST>(
            std::move(StrExpr),
            std::move(StartExpr),
            std::move(EndExpr));
    }
}

std::unique_ptr<ExprAST> Parser::ParseFunctionExpr() {
    getNextToken();
    if (CurTok.type != TokenType::LParen)
        return LogError("Expected '(' after 'function'");
    getNextToken();

    std::vector<std::string> ArgNames;
    while (CurTok.type == TokenType::Identifier) {
        ArgNames.push_back(CurTok.lexeme);
        getNextToken();
        if (CurTok.type == TokenType::Comma)
            getNextToken();
        else
            break;
    }
    if (CurTok.type != TokenType::RParen)
        return LogError("Expected ')' after function arguments");
    getNextToken();
    auto BlockBody = ParseBlockUntil(TokenType::End, TokenType::Function);
    if (!BlockBody) return nullptr;
    return std::make_unique<FunctionLiteralExprAST>(
        std::move(ArgNames), std::move(BlockBody));
}

std::unique_ptr<ExprAST> Parser::ParseNilExpr() {
    getNextToken();
    return std::make_unique<NilExprAST>();
}

std::unique_ptr<ExprAST> Parser::ParseIfExpr() {
    getNextToken();

    auto Cond = ParseExpression();
    if (!Cond) return nullptr;

    if (CurTok.type != TokenType::Then)
        return LogError("Expected 'then' after condition");
    getNextToken();

    auto Then = ParseExpression();
    if (!Then) return nullptr;

    struct Elif {
        std::unique_ptr<ExprAST> Cond, Then;
    };
    std::vector<Elif> elifs;
    std::unique_ptr<ExprAST> Else;

    while (CurTok.type == TokenType::Else) {
        getNextToken();
        if (CurTok.type == TokenType::If) {
            getNextToken();
            auto eCond = ParseExpression();
            if (!eCond) return nullptr;
            if (CurTok.type != TokenType::Then)
                return LogError("Expected 'then' after else if");
            getNextToken();
            auto eThen = ParseExpression();
            if (!eThen) return nullptr;
            elifs.push_back({std::move(eCond), std::move(eThen)});
        } else {
            Else = ParseExpression();
            if (!Else) return nullptr;
            break;
        }
    }

    if (CurTok.type != TokenType::End)
        return LogError("Expected 'end' after if");
    getNextToken();
    if (CurTok.type != TokenType::If)
        return LogError("Expected 'if' after 'end'");
    getNextToken();

    auto Root = std::make_unique<IfExprAST>(std::move(Cond), std::move(Then), nullptr);
    IfExprAST* Curr = Root.get();

    for (auto& ei : elifs) {
        auto Next = std::make_unique<IfExprAST>(
            std::move(ei.Cond),
            std::move(ei.Then),
            nullptr);
        Curr->setElse(std::move(Next));
        Curr = static_cast<IfExprAST*>(Curr->getElse());
    }

    Curr->setElse(std::move(Else));

    return Root;
}

std::unique_ptr<ExprAST> Parser::ParseBlockUntil(TokenType endKeyword, TokenType requiredSuffix) {
    std::vector<std::unique_ptr<ExprAST>> stmts;
    while (CurTok.type != TokenType::End && CurTok.type != TokenType::EndOfFile) {
        auto stmt = ParseExpression();
        if (!stmt) return nullptr;
        stmts.push_back(std::move(stmt));
    }
    if (CurTok.type != TokenType::End)
        return LogError("Expected 'end' to close block");
    getNextToken();
    if (CurTok.type != requiredSuffix)
        return LogError("Expected matching keyword after 'end'");
    getNextToken();
    return std::make_unique<BlockExprAST>(std::move(stmts));
}

std::unique_ptr<ExprAST> Parser::ParseWhileExpr() {
    getNextToken();
    auto Cond = ParseExpression();
    if (!Cond) return nullptr;

    auto Body = ParseBlockUntil(TokenType::End, TokenType::While);
    if (!Body) return nullptr;

    return std::make_unique<WhileExprAST>(std::move(Cond), std::move(Body));
}

std::unique_ptr<ExprAST> Parser::ParseForExpr() {
    getNextToken();

    if (CurTok.type != TokenType::Identifier)
        return LogError("Expected identifier after 'for'");
    std::string VarName = CurTok.lexeme;
    getNextToken();

    while (CurTok.type == TokenType::Semicolon) {
        getNextToken();
    }

    if (CurTok.type != TokenType::In)
        return LogError("Expected 'in' after for variable");
    getNextToken();

    auto SeqExpr = ParseExpression();
    if (!SeqExpr) return nullptr;

    auto Body = ParseBlockUntil(TokenType::End, TokenType::For);
    if (!Body) return nullptr;

    return std::make_unique<ForExprAST>(VarName, std::move(SeqExpr), std::move(Body));
}

std::unique_ptr<ExprAST> Parser::ParseInExpr(std::unique_ptr<ExprAST> LHS) {
    getNextToken();
    auto RHS = ParseExpression();
    if (!RHS) return nullptr;
    return std::make_unique<InExprAST>(std::move(LHS), std::move(RHS));
}

std::unique_ptr<ExprAST> Parser::ParseBreakExpr() {
    getNextToken();
    return std::make_unique<BreakExprAST>();
}

std::unique_ptr<ReturnExprAST> Parser::ParseReturnExpr() {
    getNextToken();
    auto retExpr = ParseExpression();
    if (!retExpr) return nullptr;
    return std::make_unique<ReturnExprAST>(std::move(retExpr));
}

std::unique_ptr<ExprAST> Parser::ParseContinueExpr() {
    getNextToken();
    return std::make_unique<ContinueExprAST>();
}
