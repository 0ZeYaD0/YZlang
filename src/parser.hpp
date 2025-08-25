#pragma once
#include <optional>
#include <iostream>
#include <variant>
#include "YLogger/logger.h"
#include "core/nodes.hpp"
#include "core/arena.hpp"

class Parser
{
public:
    inline explicit Parser(std::vector<Token> tokens)
        : m_tokens(std::move(tokens)), m_alloc(1024 * 1024 * 4)
    {
    }

    std::optional<NodeTerm *> parse_term()
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
        else if (try_consume(TokenType::open_paren))
        {
            auto expr = parse_expr();
            if (!expr.has_value())
            {
                LLOG(RED_TEXT("Expected Expression inside parentheses\n"));
                exit(EXIT_FAILURE);
            }
            try_consume(TokenType::close_paren, "Expected ')' after expression");

            auto term_paren = m_alloc.alloc<NodeTermParen>();
            term_paren->expr = expr.value();

            auto term = m_alloc.alloc<NodeTerm>();
            term->var = term_paren;
            return term;
        }
        else
        {
            return {};
        }
    }

    std::optional<NodeExpr *> parse_expr(int min_prec = 0)
    {
        std::optional<NodeTerm *> term_lhs_opt = parse_term();
        if (!term_lhs_opt.has_value())
        {
            return {};
        }
        auto lhs_expr = m_alloc.alloc<NodeExpr>();
        lhs_expr->var = term_lhs_opt.value();

        while (true)
        {
            std::optional<Token> curr_tok = peek();
            if (!curr_tok.has_value())
            {
                break;
            }

            std::optional<int> prec = bin_prec(curr_tok->type);
            if (!prec.has_value() || prec.value() < min_prec)
            {
                break;
            }

            Token op = consume();
            int next_min_prec = prec.value() + 1;
            auto rhs_expr_opt = parse_expr(next_min_prec);
            if (!rhs_expr_opt.has_value())
            {
                LLOG(RED_TEXT("Unable to parse expression on right-hand side of operator\n"));
                exit(EXIT_FAILURE);
            }

            auto bin_expr = m_alloc.alloc<NodeBinExpr>();
            if (op.type == TokenType::plus)
            {
                auto add = m_alloc.alloc<NodeBinExprAdd>();
                add->lhs = lhs_expr;
                add->rhs = rhs_expr_opt.value();
                bin_expr->var = add;
            }
            else if (op.type == TokenType::star)
            {
                auto multi = m_alloc.alloc<NodeBinExprMulti>();
                multi->lhs = lhs_expr;
                multi->rhs = rhs_expr_opt.value();
                bin_expr->var = multi;
            }
            else if (op.type == TokenType::sub)
            {
                auto sub = m_alloc.alloc<NodeBinExprSub>();
                sub->lhs = lhs_expr;
                sub->rhs = rhs_expr_opt.value();
                bin_expr->var = sub;
            }
            else if (op.type == TokenType::div)
            {
                auto div = m_alloc.alloc<NodeBinExprDiv>();
                div->lhs = lhs_expr;
                div->rhs = rhs_expr_opt.value();
                bin_expr->var = div;
            }

            auto new_lhs_expr = m_alloc.alloc<NodeExpr>();
            new_lhs_expr->var = bin_expr;
            lhs_expr = new_lhs_expr;
        }
        return lhs_expr;
    }

    std::optional<NodeStmt *> parse_stmt()
    {
        if (peek().value().type == TokenType::exit && peek(1).has_value() && peek(1).value().type == TokenType::open_paren)
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
        else if (peek().value().type == TokenType::out && peek(1).has_value() && peek(1).value().type == TokenType::open_paren)
        {
            consume();
            consume();
            auto stmt_out = m_alloc.alloc<NodeStmtOut>();

            if (auto node_expr = parse_expr())
            {
                stmt_out->expr = node_expr.value();
            }
            else
            {
                LLOG(RED_TEXT("Invalid Expression\n"));
                exit(EXIT_FAILURE);
            }

            try_consume(TokenType::close_paren, "Expected `)`");
            try_consume(TokenType::semi, "Expected `;`");

            auto stmt = m_alloc.alloc<NodeStmt>();
            stmt->var = stmt_out;

            return stmt;
        }
        else if (peek().value().type == TokenType::open_curly)
        {
            consume();
            auto stmt_scope = m_alloc.alloc<NodeStmtScope>();
            
            while (peek().has_value() && peek().value().type != TokenType::close_curly)
            {
                if (auto stmt = parse_stmt())
                {
                    stmt_scope->stmts.push_back(stmt.value());
                }
                else
                {
                    LLOG(RED_TEXT("Invalid Statement in scope\n"));
                    exit(EXIT_FAILURE);
                }
            }

            try_consume(TokenType::close_curly, "Expected `}`");

            auto stmt = m_alloc.alloc<NodeStmt>();
            stmt->var = stmt_scope;

            return stmt;
        }
        else if (
            peek().has_value() && peek().value().type == TokenType::val && peek(1).has_value() && peek(1).value().type == TokenType::ident && peek(2).has_value() && peek(2).value().type == TokenType::eq)
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

    inline Token try_consume(TokenType type, const std::string &err_msg)
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