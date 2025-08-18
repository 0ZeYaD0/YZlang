#pragma once
#include "parser.hpp"
#include <iostream>
#include <sstream> // Added: missing include for std::stringstream

class Generator
{
public:
    inline Generator(NodeExit root)
        : m_root(std::move(root))
    {
    }
    [[nodiscard]] std::string genrate() const
    {
        std::stringstream output;

        output << ".section .text\n";
        output << ".global main\n";
        output << "\n";
        output << "main:\n";
        output << "    movl $" << m_root.expr.int_lit.value.value() << ", %eax\n";
        output << "    ret\n";
        return output.str();
    }

private:
    NodeExit m_root;
};