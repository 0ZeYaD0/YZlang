#pragma once
#include <vector>
#include <string>
#include <optional>
#include <cctype>
#include "core/defines.h"
#include "core/nodes.hpp"
#include "YLogger/logger.h"

class Tokenizer {

public:
    inline explicit Tokenizer(const std::string &src)
        : m_src(src), m_idx(0)
    {
    }

    inline std::vector<Token> tokenize()
    {
        std::vector<Token> tokens;
        std::string buff;

        while (peek().has_value())
        {
            if (std::isalpha(peek().value()))
            {
                buff.push_back(consume());
                while (peek().has_value() && std::isalnum(peek().value()))
                {
                    buff.push_back(consume());
                }

                if (buff == "exit")
                {
                    tokens.push_back({.type = TokenType::exit});
                }
                else if (buff == "val")
                {
                    tokens.push_back({.type = TokenType::let});
                }
                else
                {
                    tokens.push_back({.type = TokenType::ident, .value = buff});
                }
                buff.clear();
            }
            else if (std::isdigit(peek().value()))
            {
                buff.push_back(consume());

                while (peek().has_value() && std::isdigit(peek().value()))
                {
                    buff.push_back(consume());
                }

                tokens.push_back({.type = TokenType::_int_lit, .value = buff});
                buff.clear();
            }
            else if (peek().value() == '(')
            {
                consume();
                tokens.push_back({.type = TokenType::open_paren});
            }
            else if (peek().value() == ')')
            {
                consume();
                tokens.push_back({.type = TokenType::close_paren});
            }
            else if (peek().value() == ';')
            {
                consume();
                tokens.push_back({.type = TokenType::semi});
            }
            else if (peek().value() == '=')
            {
                consume();
                tokens.push_back({.type = TokenType::eq});
            }
            else if (peek().value() == '+')
            {
                consume();
                tokens.push_back({.type = TokenType::plus});
            }
            else if(peek().value() == '*')
            {
                consume();
                tokens.push_back({.type = TokenType::star});
            }
            else if(std::isspace(peek().value()))
            {
                consume();
                continue;
            } 
            else 
            {
                LLOG(RED_TEXT("Token Type not in known token list....\n"));
                exit(EXIT_FAILURE);
            }
        }
        m_idx = 0;
        return tokens;
    }

private:
    [[nodiscard]] inline std::optional<char> peek(int offset = 0) const
    {
        if (m_idx + offset >= m_src.length())
            return {};
        else
            return m_src.at(m_idx + offset);
    }

    inline char consume()
    {
        return m_src.at(m_idx++);
    }

    const std::string m_src;
    size_t m_idx = 0;
};