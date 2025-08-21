#pragma once
#include <optional>
#include <iostream>
#include <variant>
#include "core/nodes.hpp"
#include "core/arena.hpp"

class Parser
{
public:
    inline explicit Parser(std::vector<Token> tokens) : m_tokens(std::move(tokens)), m_alloc(1024 * 1024 * 4)
    {
    }

    std::optional<NodeExpr *> parse_primary_expr()
    {
        if (peek().has_value() && peek().value().type == TokenType::_int_lit)
        {
            auto *expr = m_alloc.alloc<NodeExpr>();
            auto *int_lit_node = m_alloc.alloc<NodeExprIntLit>();
            int_lit_node->int_lit = consume();
            expr->var = int_lit_node;
            return expr;
        }
        else if (peek().has_value() && peek().value().type == TokenType::ident)
        {
            auto *expr = m_alloc.alloc<NodeExpr>();
            auto *ident_node = m_alloc.alloc<NodeExprIdent>();
            ident_node->ident = consume();
            expr->var = ident_node;
            return expr;
        }
        else if (peek().has_value() && peek().value().type == TokenType::open_paren)
        {
            consume();
            auto expr = parse_expr();
            if (!peek().has_value() || peek().value().type != TokenType::close_paren)
            {
                std::cerr << "Expected ')'" << std::endl;
                exit(EXIT_FAILURE);
            }
            consume();
            return expr;
        }
        else
        {
            return {};
        }
    }

    std::optional<NodeExpr *> parse_expr()
    {
        auto lhs_opt = parse_primary_expr();
        if (!lhs_opt.has_value())
        {
            return {};
        }
        auto *lhs = lhs_opt.value();

        while (peek().has_value() && peek().value().type == TokenType::plus)
        {
            consume();

            auto rhs_opt = parse_primary_expr();
            if (!rhs_opt.has_value())
            {
                std::cerr << "Expected expression after '+'" << std::endl;
                exit(EXIT_FAILURE);
            }

            auto *bin_expr_node = m_alloc.alloc<BinExpr>();
            auto *add_node = m_alloc.alloc<NodeBinExprAdd>();
            add_node->lh = lhs;
            add_node->rh = rhs_opt.value();
            bin_expr_node->var = add_node;

            auto *new_expr = m_alloc.alloc<NodeExpr>();
            new_expr->var = bin_expr_node;

            lhs = new_expr;
        }

        return lhs;
    }

    std::optional<NodeStmt *> parse_stmt()
    {
        if (peek().has_value() && peek().value().type == TokenType::exit && peek(1).has_value() && peek(1).value().type == TokenType::open_paren)
        {
            consume();
            consume();

            if (auto node_expr_opt = parse_expr())
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
                auto *stmt = m_alloc.alloc<NodeStmt>();
                auto *exit_node = m_alloc.alloc<NodeStmtExit>();
                exit_node->expr = node_expr_opt.value();
                stmt->var = exit_node;
                return stmt;
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
            auto *stmt_let = m_alloc.alloc<NodeStmtLet>();
            stmt_let->ident = consume();
            consume();
            if (auto expr_opt = parse_expr())
            {
                stmt_let->expr = expr_opt.value();
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
            auto *stmt = m_alloc.alloc<NodeStmt>();
            stmt->var = stmt_let;
            return stmt;
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
    ArenaAlloc m_alloc;
};