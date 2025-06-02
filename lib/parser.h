#pragma once
#include <map>

#include "AST.h"
#include "lexer.h"

class Parser {
    Lexer& Lex;
    Token CurTok;
    std::map<TokenType, int> BinopPrecedence;

    void getNextToken() { CurTok = Lex.nextToken(); }

    std::unique_ptr<ExprAST> LogError(const char* msg);
    std::unique_ptr<PrototypeAST> LogErrorP(const char* msg);

    std::unique_ptr<ExprAST> ParsePrimary();
    std::unique_ptr<ExprAST> ParseUnary();
    std::unique_ptr<ExprAST> ParseExpression();
    std::unique_ptr<ExprAST> ParseBinOpRHS(int exprPrec, std::unique_ptr<ExprAST> LHS);
    std::unique_ptr<ExprAST> ParseCallExpr(std::unique_ptr<ExprAST> Callee);

    std::unique_ptr<PrototypeAST> ParsePrototype();
    std::unique_ptr<FunctionAST> ParseDefinition();
    std::unique_ptr<PrototypeAST> ParseExtern();
    std::unique_ptr<FunctionAST> ParseTopLevelExpr();

    std::unique_ptr<ExprAST> ParseNumberExpr();
    std::unique_ptr<ExprAST> ParseParenExpr();
    std::unique_ptr<ExprAST> ParseIdentifierExpr();
    std::unique_ptr<ExprAST> ParseStringExpr();
    std::unique_ptr<ExprAST> ParseBooleanExpr();
    std::unique_ptr<ExprAST> ParseListExpr();
    std::unique_ptr<ExprAST> ParseStringSlice(std::unique_ptr<ExprAST> StrExpr);
    std::unique_ptr<ExprAST> ParseFunctionExpr();
    std::unique_ptr<ExprAST> ParseNilExpr();

    std::unique_ptr<ExprAST> ParseBlockUntil(TokenType endKeyword, TokenType requiredSuffix);
    std::unique_ptr<ExprAST> ParseIfExpr();
    std::unique_ptr<ExprAST> ParseInExpr(std::unique_ptr<ExprAST> LHS);
    std::unique_ptr<ExprAST> ParseWhileExpr();
    std::unique_ptr<ExprAST> ParseForExpr();
    std::unique_ptr<ExprAST> ParseBreakExpr();
    std::unique_ptr<ReturnExprAST> ParseReturnExpr();

    std::unique_ptr<ExprAST> ParseContinueExpr();

    int GetTokPrecedence();

   public:
    Parser(Lexer& lex);

    bool parseModule(std::vector<std::unique_ptr<FunctionAST>>& Out);
};
