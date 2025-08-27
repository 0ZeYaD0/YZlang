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
            auto term_int_lit = m_alloc.emplace<NodeTermIntLit>(int_lit.value());
            auto term = m_alloc.emplace<NodeTerm>(term_int_lit);
            return term;
        }
        else if (auto ident = try_consume(TokenType::ident))
        {
            auto expr_ident = m_alloc.emplace<NodeTermIdent>(ident.value());
            auto term = m_alloc.emplace<NodeTerm>(expr_ident);
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

            auto term_paren = m_alloc.emplace<NodeTermParen>(expr.value());
            auto term = m_alloc.emplace<NodeTerm>(term_paren);
            return term;
        }
        else
        {
            return {};
        }
    }

    std::optional<NodeExpr *> parse_expr(int min_prec = 0)
    {
        auto term_lhs_opt = parse_term();
        if (!term_lhs_opt)
            return {};
        auto lhs_expr = m_alloc.emplace<NodeExpr>(term_lhs_opt.value());

        while (true)
        {
            auto curr_tok = peek();
            if (!curr_tok)
                break;

            auto prec = bin_prec(curr_tok->type);
            if (!prec.has_value() || prec.value() < min_prec)
                break;

            Token op = consume();
            int next_min_prec = prec.value() + 1;
            auto rhs_expr_opt = parse_expr(next_min_prec);
            if (!rhs_expr_opt)
            {
                LLOG(RED_TEXT("Unable to parse expression on right-hand side of operator\n"));
                exit(EXIT_FAILURE);
            }

            auto bin_expr = m_alloc.alloc<NodeBinExpr>();
            if (op.type == TokenType::plus)
            {
                auto add = m_alloc.emplace<NodeBinExprAdd>(lhs_expr, rhs_expr_opt.value());
                bin_expr->var = add;
            }
            else if (op.type == TokenType::star)
            {
                auto multi = m_alloc.emplace<NodeBinExprMulti>(lhs_expr, rhs_expr_opt.value());
                bin_expr->var = multi;
            }
            else if (op.type == TokenType::sub)
            {
                auto sub = m_alloc.emplace<NodeBinExprSub>(lhs_expr, rhs_expr_opt.value());
                bin_expr->var = sub;
            }
            else if (op.type == TokenType::div)
            {
                auto div = m_alloc.emplace<NodeBinExprDiv>(lhs_expr, rhs_expr_opt.value());
                bin_expr->var = div;
            }
            auto new_lhs_expr = m_alloc.emplace<NodeExpr>(bin_expr);
            lhs_expr = new_lhs_expr;
        }
        return lhs_expr;
    }

    std::optional<NodeStmtBlock *> parse_scope()
    {
        if (!try_consume(TokenType::open_curly).has_value())
        {
            return {};
        }

        auto block = m_alloc.alloc<NodeStmtBlock>();
        while (true)
        {
            if (!peek().has_value())
            {
                LLOG(RED_TEXT("Unterminated block\n"));
                exit(EXIT_FAILURE);
            }
            if (peek()->type == TokenType::close_curly)
            {
                consume();
                break;
            }
            if (auto inner = parse_stmt())
                block->stmts.push_back(inner.value());
            else
            {
                LLOG(RED_TEXT("Invalid statement inside block\n"));
                exit(EXIT_FAILURE);
            }
        }

        return block;
    }

    std::optional<NodeIfPred *> parse_if_pred()
    {
        if (try_consume(TokenType::elif))
        {
            try_consume(TokenType::open_paren, "Expected '('");
            auto elif = m_alloc.alloc<NodeIfPredElif>();
            if (auto expr = parse_expr())
            {
                elif->expr = expr.value();
            }
            else
            {
                LLOG(RED_TEXT("Expected exprission\n"));
                exit(EXIT_FAILURE);
            }

            try_consume(TokenType::close_paren, "Expected ')'");
            if (auto scope = parse_scope())
            {
                elif->scope = scope.value();
            }
            else
            {
                LLOG(RED_TEXT("Expected scope\n"));
                exit(EXIT_FAILURE);
            }
            elif->pred = parse_if_pred();
            auto pred = m_alloc.emplace<NodeIfPred>(elif);
            return pred;
        }
        if (try_consume(TokenType::else_))
        {
            auto else_ = m_alloc.alloc<NodeIfPredElse>();
            if (auto scope = parse_scope())
            {
                else_->scope = scope.value();
            }
            else
            {
                LLOG(RED_TEXT("Expected scope\n"));
                exit(EXIT_FAILURE);
            }
            auto pred = m_alloc.emplace<NodeIfPred>(else_);
            return pred;
        }
        return {};
    }

    std::optional<NodeStmt *> parse_stmt()
    {
        if (peek().value().type == TokenType::exit &&
            peek(1).has_value() && peek(1).value().type == TokenType::open_paren)
        {
            consume();
            consume();
            auto stmt_exit = m_alloc.alloc<NodeStmtExit>();
            if (auto node_expr = parse_expr())
                stmt_exit->expr = node_expr.value();
            else
            {
                LLOG(RED_TEXT("Invalid Expression\n"));
                exit(EXIT_FAILURE);
            }
            try_consume(TokenType::close_paren, "Expected `)`");
            try_consume(TokenType::semi, "Expected `;`");
            auto stmt = m_alloc.emplace<NodeStmt>(stmt_exit);
            return stmt;
        }
        else if (peek().has_value() && peek().value().type == TokenType::val &&
                 peek(1).has_value() && peek(1).value().type == TokenType::ident &&
                 peek(2).has_value() && peek(2).value().type == TokenType::eq)
        {
            consume();
            auto stmt_let = m_alloc.alloc<NodeStmtLet>();
            stmt_let->ident = consume();
            consume();
            if (auto expr = parse_expr())
                stmt_let->expr = expr.value();
            else
            {
                std::cerr << "Invalid expression" << std::endl;
                exit(EXIT_FAILURE);
            }
            try_consume(TokenType::semi, "Expected `;`");
            auto stmt = m_alloc.emplace<NodeStmt>(stmt_let);
            return stmt;
        }
        else if (peek().has_value() && peek()->type == TokenType::out)
        {
            consume();
            try_consume(TokenType::open_paren, "Expected '(' after print");
            auto expr = parse_expr();
            if (!expr)
            {
                LLOG(RED_TEXT("Invalid expression in print\n"));
                exit(EXIT_FAILURE);
            }
            try_consume(TokenType::close_paren, "Expected ')'");
            try_consume(TokenType::semi, "Expected ';'");
            auto stmt_out = m_alloc.alloc<NodeStmtOut>();
            stmt_out->expr = expr.value();
            auto stmt = m_alloc.emplace<NodeStmt>(stmt_out);
            return stmt;
        }
        else if (auto block = parse_scope())
        {
            auto stmt = m_alloc.emplace<NodeStmt>(block.value());
            return stmt;
        }
        else if (auto if_ = try_consume(TokenType::if_))
        {
            try_consume(TokenType::open_paren, "Expected '('");
            auto stmt_if = m_alloc.alloc<NodeStmtIf>();
            if (auto expr = parse_expr())
            {
                stmt_if->expr = expr.value();
            }
            else
            {
                LLOG(RED_TEXT("Invalid Expression\n"));
                exit(EXIT_FAILURE);
            }
            try_consume(TokenType::close_paren, "Expected ')'");
            if (auto scope = parse_scope())
            {
                stmt_if->scope = scope.value();
            }
            else
            {
                LLOG(RED_TEXT("Invalid scope\n"));
                exit(EXIT_FAILURE);
            }
            stmt_if->pred = parse_if_pred();
            auto stmt = m_alloc.emplace<NodeStmt>(stmt_if);
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
                prog.stmts.push_back(stmt.value());
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
            return {};
        return m_tokens.at(m_idx + offset);
    }

    inline Token consume()
    {
        return m_tokens.at(m_idx++);
    }

    inline Token try_consume(TokenType type, const std::string &err_msg)
    {
        if (peek().has_value() && peek().value().type == type)
            return consume();
        LLOG(RED_TEXT(err_msg), "\n");
        exit(EXIT_FAILURE);
    }

    inline std::optional<Token> try_consume(TokenType type)
    {
        if (peek().has_value() && peek().value().type == type)
            return consume();
        return {};
    }

    const std::vector<Token> m_tokens;
    size_t m_idx = 0;
    ArenaAlloc m_alloc;
};