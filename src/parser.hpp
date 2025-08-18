#pragma once
#include "Tokenize.hpp"
#include <optional>
#include <iostream>

struct NodeExpr
{
    Token int_lit;
};

struct NodeExit
{
    NodeExpr expr;
};

class Parser
{
public:
    inline explicit Parser(std::vector<Token> tokens) : m_tokens(std::move(tokens))
    {
    }

    std::optional<NodeExpr> parse_expr()
    {
        if (peak().has_value() && peak().value().type == TokenType::_int_lit) // Fixed: .has_value and .type
            return NodeExpr{.int_lit = consume()};
        else
            return {};
    }

    std::optional<NodeExit> parse_exit()
    {
        if (peak().has_value() && peak().value().type == TokenType::exit) // Fixed: added if condition
        {
            consume();
            if (auto node_expr = parse_expr())
            {
                if (peak().has_value() && peak().value().type == TokenType::semi) // Fixed: && instead of ||
                {
                    consume();                                  // Fixed: consume the semicolon
                    return NodeExit{.expr = node_expr.value()}; // Fixed: added return
                }
                else
                {
                    std::cerr << "Expected semicolon" << std::endl;
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                std::cerr << "Invalid expression" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        return {};
    }

private:
    [[nodiscard]] inline std::optional<Token> peak(int ahead = 0) const
    {
        if (m_idx + ahead >= m_tokens.size())
            return {};
        else
            return m_tokens.at(m_idx + ahead);
    }

    inline Token consume()
    {
        return m_tokens.at(m_idx++);
    }

    size_t m_idx = 0;
    const std::vector<Token> m_tokens;
};