#pragma once
#include "Tokenize.hpp"
#include <optional>
#include <iostream>
#include <variant>

struct NodeExprIntLit
{
    Token int_lit;
};

struct NodeExprIdent
{
    Token ident;
};

struct NodeExpr
{
    std::variant<NodeExprIntLit, NodeExprIdent> var;
};

struct NodeStmtExit
{
    NodeExpr expr;
};

struct NodeStmtLet
{
    Token ident;
    NodeExpr expr;
};

struct NodeStmt
{
    std::variant<NodeStmtExit, NodeStmtLet> var;
};

struct NodeProg
{
    std::vector<NodeStmt> stmts;
};

class Parser
{
public:
    inline explicit Parser(std::vector<Token> tokens) : m_tokens(std::move(tokens))
    {
    }

    std::optional<NodeExpr> parse_expr()
    {
        if (peek().has_value() && peek().value().type == TokenType::_int_lit)
            return NodeExpr{.var = NodeExprIntLit{.int_lit = consume()}};

        else if (peek().has_value() && peek().value().type == TokenType::ident)
            return NodeExpr{.var = NodeExprIdent{.ident = consume()}}; // Fixed: use NodeExprIdent

        else
            return {};
    }

    std::optional<NodeStmt> parse_stmt()
    {
        if (peek().has_value() && peek().value().type == TokenType::exit && peek(1).has_value() && peek(1).value().type == TokenType::open_paren)
        {
            consume();
            consume();

            if (auto node_expr = parse_expr())
            {
                if (peek().has_value() && peek().value().type == TokenType::close_paren)
                {
                    consume();
                }
                else
                {
                    std::cerr << "Expected ')'" << std::endl;
                    exit(EXIT_FAILURE);
                }

                if (peek().has_value() && peek().value().type == TokenType::semi)
                {
                    consume();
                }
                else
                {
                    std::cerr << "Expected ';'" << std::endl;
                    exit(EXIT_FAILURE);
                }
                return NodeStmt{.var = NodeStmtExit{.expr = node_expr.value()}};
            }
            else
            {
                std::cerr << "Invalid expression" << std::endl;
                exit(EXIT_FAILURE);
            }
        }

        else if (peek().has_value() && peek().value().type == TokenType::let && peek(1).has_value() && peek(1).value().type == TokenType::ident && peek(2).has_value() && peek(2).value().type == TokenType::eq)
        {
            consume();
            auto stmt_let = NodeStmtLet{.ident = consume()};
            consume();
            if (auto expr = parse_expr())
            {
                stmt_let.expr = expr.value();
            }
            else
            {
                std::cerr << "Invalid expression" << std::endl;
                exit(EXIT_FAILURE);
            }
            if (peek().has_value() && peek().value().type == TokenType::semi)
            {
                consume();
            }
            else
            {
                std::cerr << "Expected ';'" << std::endl;
                exit(EXIT_FAILURE);
            }
            return NodeStmt{.var = stmt_let};
        }

        return {};
    }

    std::optional<NodeProg> parse_prog()
    {
        NodeProg prog;
        while (peek().has_value())
        {
            if (auto stmt = parse_stmt())
            {
                prog.stmts.push_back(stmt.value());
            }
            else
            {
                std::cerr << "Invalid statement" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        return prog;
    }

private:
    [[nodiscard]] inline std::optional<Token>
    peek(int offset = 0) const
    {
        if (m_idx + offset >= m_tokens.size())
            return {};
        else
            return m_tokens.at(m_idx + offset);
    }

    inline Token consume()
    {
        return m_tokens.at(m_idx++);
    }

    size_t m_idx = 0;
    const std::vector<Token> m_tokens;
};