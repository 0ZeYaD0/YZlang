#pragma once
#include <variant>
#include <vector>
#include <optional>
#include <string>

enum class TokenType
{
    exit,
    _int_lit,
    semi,
    open_paren,
    close_paren,
    ident,
    val,
    eq,
    plus,
    star,
    sub,
    div,
    open_curly,
    close_curly,
    out
};

struct Token
{
    TokenType type;
    std::optional<std::string> value;
};

struct NodeTermIntLit
{
    Token int_lit;
};

struct NodeTermIdent
{
    Token ident;
};

struct NodeExpr;

struct NodeTermParen
{
    NodeExpr *expr;
};

struct NodeBinExprAdd
{
    NodeExpr *lhs;
    NodeExpr *rhs;
};

struct NodeBinExprMulti
{
    NodeExpr *lhs;
    NodeExpr *rhs;
};

struct NodeBinExprSub
{
    NodeExpr *lhs;
    NodeExpr *rhs;
};

struct NodeBinExprDiv
{
    NodeExpr *lhs;
    NodeExpr *rhs;
};

struct NodeBinExpr
{
    std::variant<NodeBinExprAdd *, NodeBinExprMulti *, NodeBinExprDiv *, NodeBinExprSub *> var;
};

struct NodeTerm
{
    std::variant<NodeTermIntLit *, NodeTermIdent *, NodeTermParen *> var;
};

struct NodeExpr
{
    std::variant<NodeTerm *, NodeBinExpr *> var;
};

struct NodeStmtExit
{
    NodeExpr *expr;
};

struct NodeStmtLet
{
    Token ident;
    NodeExpr *expr;
};

struct NodeStmtOut
{
    NodeExpr *expr;
};

struct NodeStmt;

struct NodeStmtScope
{
    std::vector<NodeStmt *> stmts;
};

struct NodeStmt
{
    std::variant<NodeStmtExit *, NodeStmtLet *, NodeStmtOut *, NodeStmtScope *> var;
};

struct NodeProg
{
    std::vector<NodeStmt *> stmts;
};

std::optional<int> bin_prec(TokenType type)
{
    switch (type)
    {
    case TokenType::sub:
    case TokenType::plus:
        return 0;
    case TokenType::div:
    case TokenType::star:
        return 1;
    default:
        return {};
    }
}