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
    let,
    eq,
    plus,
    star
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

struct NodeBinExprAdd
{
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct NodeBinExprMulti {
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct NodeBinExpr
{
    NodeBinExprAdd* add;
};

struct NodeTerm
{
    std::variant<NodeTermIntLit*, NodeTermIdent*> var;
};

struct NodeExpr
{
    std::variant<NodeTerm*, NodeBinExpr*> var;
};

struct NodeStmtExit
{
    NodeExpr* expr;
};

struct NodeStmtLet
{
    Token ident;
    NodeExpr* expr;
};

struct NodeStmt
{
    std::variant<NodeStmtExit*, NodeStmtLet*> var;
};

struct NodeProg
{
    std::vector<NodeStmt*> stmts;
};