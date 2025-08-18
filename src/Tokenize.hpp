#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <optional>
#include <cctype>

enum class TokenType
{
    exit,
    _int_lit,
    semi
};

struct Token
{
    TokenType type;
    std::optional<std::string> value;
};

class Tokenizer
{
public:
    inline explicit Tokenizer(const std::string &src)
        : m_src(src), m_idx(0)
    {
    }

    inline std::vector<Token> tokenize()
    {
        std::vector<Token> tokens;
        std::string buff;

        while (peak().has_value())
        {
            if (std::isalpha(peak().value()))
            {
                buff.push_back(consume());
                while (peak().has_value() && std::isalnum(peak().value()))
                {
                    buff.push_back(consume());
                }

                if (buff == "exit")
                {
                    tokens.push_back({.type = TokenType::exit});
                }
                buff.clear();
            }
            else if (std::isdigit(peak().value()))
            {
                buff.push_back(consume());

                while (peak().has_value() && std::isdigit(peak().value()))
                {
                    buff.push_back(consume());
                }

                tokens.push_back({.type = TokenType::_int_lit, .value = buff});
                buff.clear();
            }
            else if (peak().value() == ';')
            {
                consume();
                tokens.push_back({.type = TokenType::semi});
            }
            else
            {
                consume();
            }
        }
        m_idx = 0;
        return tokens;
    }

private:
    [[nodiscard]] inline std::optional<char> peak(int ahead = 0) const
    {
        if (m_idx + ahead >= m_src.length())
            return {};
        else
            return m_src.at(m_idx + ahead);
    }

    inline char consume()
    {
        return m_src.at(m_idx++);
    }

    const std::string m_src;
    size_t m_idx = 0;
};