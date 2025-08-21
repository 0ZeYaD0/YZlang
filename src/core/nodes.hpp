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


struct NodeExpr;
struct BinExpr;

struct NodeExprIntLit
{
    Token int_lit;
};

struct NodeExprIdent
{
    Token ident;
};

struct NodeBinExprAdd
{
    NodeExpr *lh;
    NodeExpr *rh;
};

struct NodeBinExprMulti
{
    NodeExpr *lh;
    NodeExpr *rh;
};

struct BinExpr
{
    std::variant<NodeBinExprAdd *, NodeBinExprMulti *> var;
};

struct NodeExpr
{
    std::variant<NodeExprIntLit *, NodeExprIdent *, BinExpr *> var;
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

struct NodeStmt
{
    std::variant<NodeStmtExit *, NodeStmtLet *> var;
};

struct NodeProg
{
    std::vector<NodeStmt *> stmts;
};