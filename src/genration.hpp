#pragma once
#include "parser.hpp"
#include <iostream>
#include <sstream>
#include <variant>
#include <vector>
#include <map>

class Generator
{
public:
    inline Generator(NodeProg prog)
        : m_prog(std::move(prog))
    {
    }

    void gen_expr(const NodeExpr &expr)
    {
        struct ExprVisitor
        {
            Generator *gen;
            void operator()(const NodeExprIntLit &expr_int_lit) const
            {
                gen->m_output << "    movl $" << expr_int_lit.int_lit.value.value() << ", %eax\n";
            }
            void operator()(const NodeExprIdent &expr_ident) const
            {
                auto it = gen->m_vars.find(expr_ident.ident.value.value());
                if (it == gen->m_vars.end())
                {
                    std::cerr << "Undeclared identifier: " << expr_ident.ident.value.value() << std::endl;
                    exit(EXIT_FAILURE);
                }
                gen->m_output << "    movl " << it->second << "(%rbp), %eax\n";
            }
        };
        std::visit(ExprVisitor{.gen = this}, expr.var);
    }

    void gen_stmt(const NodeStmt &stmt)
    {
        struct StmtVisitor
        {
            Generator *gen;
            void operator()(const NodeStmtExit &stmt_exit) const
            {
                gen->gen_expr(stmt_exit.expr);
                gen->m_output << "    movq %rbp, %rsp\n";
                gen->m_output << "    popq %rbp\n";
                gen->m_output << "    ret\n";
            }
            void operator()(const NodeStmtLet &stmt_let) const
            {
                if (gen->m_vars.count(stmt_let.ident.value.value()))
                {
                    std::cerr << "Identifier already declared: " << stmt_let.ident.value.value() << std::endl;
                    exit(EXIT_FAILURE);
                }
                gen->gen_expr(stmt_let.expr);
                gen->m_output << "    pushq %rax\n";
                gen->m_stack_size += 8;
                gen->m_vars[stmt_let.ident.value.value()] = -gen->m_stack_size;
            }
        };
        std::visit(StmtVisitor{.gen = this}, stmt.var);
    }

    [[nodiscard]] std::string genrate()
    {
        m_output << ".section .text\n";
        m_output << ".global main\n";
        m_output << "\n";
        m_output << "main:\n";
        m_output << "    pushq %rbp\n";
        m_output << "    movq %rsp, %rbp\n";

        for (const auto &stmt : m_prog.stmts)
        {
            gen_stmt(stmt);
        }

        m_output << "    movl $0, %eax\n";
        m_output << "    movq %rbp, %rsp\n";
        m_output << "    popq %rbp\n";
        m_output << "    ret\n";
        return m_output.str();
    }

private:
    const NodeProg m_prog;
    std::stringstream m_output;
    std::map<std::string, int> m_vars;
    int m_stack_size = 0;
};