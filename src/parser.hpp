#pragma once
#include <optional>
#include <iostream>
#include <variant>
#include "YLogger/logger.h"
#include "core/nodes.hpp"
#include "core/arena.hpp"

class Parser {
public:
    inline explicit Parser(std::vector<Token> tokens)
        : m_tokens(std::move(tokens)), m_alloc(1024 * 1024 * 4)
    {
    }

    std::optional<NodeTerm*> parse_term()
    {
        if (auto int_lit = try_consume(TokenType::_int_lit))
        {
            auto term_int_lit = m_alloc.alloc<NodeTermIntLit>();
            term_int_lit->int_lit = int_lit.value();
            auto term = m_alloc.alloc<NodeTerm>();
            term->var = term_int_lit;
            return term;
        }
        else if (auto ident = try_consume(TokenType::ident))
        {
            auto expr_ident = m_alloc.alloc<NodeTermIdent>();
            expr_ident->ident = ident.value();
            auto term = m_alloc.alloc<NodeTerm>();
            term->var = expr_ident;
            return term;
        }
        else
        {
            return {};
        }
    }

    std::optional<NodeExpr*> parse_expr()
    {
        if (auto term = parse_term())
        {
            if (try_consume(TokenType::plus).has_value())
            {
                auto bin_expr = m_alloc.alloc<NodeBinExpr>();
                auto bin_expr_add = m_alloc.alloc<NodeBinExprAdd>();
                auto lhs_expr = m_alloc.alloc<NodeExpr>();
                lhs_expr->var = term.value();
                bin_expr_add->lhs = lhs_expr;

                if (auto rhs = parse_expr())
                {
                    bin_expr_add->rhs = rhs.value();
                    bin_expr->add = bin_expr_add;
                    auto expr = m_alloc.alloc<NodeExpr>();
                    expr->var = bin_expr;
                    return expr;
                }
                else
                {
                    LLOG(RED_TEXT("Expected expression\n"));
                    exit(EXIT_FAILURE);
                }

            }
            else 
            {
                auto expr = m_alloc.alloc<NodeExpr>();
                expr->var = term.value();
                return expr;
            }
        }
        else
        {
            return {};
        }
    }

    std::optional<NodeStmt*> parse_stmt()
    {
        if (peek().value().type == TokenType::exit && peek(1).has_value()
            && peek(1).value().type == TokenType::open_paren)
        {
            consume();
            consume();
            auto stmt_exit = m_alloc.alloc<NodeStmtExit>();

            if (auto node_expr = parse_expr())
            {
                stmt_exit->expr = node_expr.value();
            }
            else
            {
                LLOG(RED_TEXT("Invalid Expression\n"));
                exit(EXIT_FAILURE);
            }

            try_consume(TokenType::close_paren, "Expected `)`");
            try_consume(TokenType::semi, "Expected `;`");

            auto stmt = m_alloc.alloc<NodeStmt>();
            stmt->var = stmt_exit;

            return stmt;
        }
        else if( // ayo wth is this condition
            peek().has_value() && peek().value().type == TokenType::let && peek(1).has_value()
            && peek(1).value().type == TokenType::ident && peek(2).has_value()
            && peek(2).value().type == TokenType::eq)
        {
            consume();
            auto stmt_let = m_alloc.alloc<NodeStmtLet>();
            stmt_let->ident = consume();
            consume();
            if (auto expr = parse_expr())
            {
                stmt_let->expr = expr.value();
            }
            else
            {
                std::cerr << "Invalid expression" << std::endl;
                exit(EXIT_FAILURE);
            }

            try_consume(TokenType::semi, "Expected `;`");
            auto stmt = m_alloc.alloc<NodeStmt>();
            stmt->var = stmt_let;

            return stmt;
        }
        else
        {
            return {};
        }
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
                LLOG(RED_TEXT("Invalid Statement\n"));
                exit(EXIT_FAILURE);
            }
        }

        return prog;
    }

private:
    [[nodiscard]] inline std::optional<Token> peek(int offset = 0) const
    {
        if (m_idx + offset >= m_tokens.size())
        {
            return {};
        }
        else
        {
            return m_tokens.at(m_idx + offset);
        }
    }

    inline Token consume()
    {
        return m_tokens.at(m_idx++);
    }

    inline Token try_consume(TokenType type, const std::string& err_msg)
    {
        if (peek().has_value() && peek().value().type == type)
        {
            return consume();
        }
        else
        {
            LLOG(RED_TEXT(err_msg), "\n");
            exit(EXIT_FAILURE);
        }
    }

    inline std::optional<Token> try_consume(TokenType type)
    {
        if (peek().has_value() && peek().value().type == type)
        {
            return consume();
        }
        else
        {
            return {};
        }
    }

    const std::vector<Token> m_tokens;
    size_t m_idx = 0;
    ArenaAlloc m_alloc;
};