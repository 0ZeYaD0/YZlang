#pragma once

#include "YLogger/logger.h"
#include "parser.hpp"
#include <cassert>
#include <map>
#include <sstream>

#include "core/defines.h"

class Generator {
public:
    inline explicit Generator(NodeProg prog)
        : m_prog(std::move(prog))
    {
    }

    void gen_term(const NodeTerm* term)
    {
        struct TermVisitor
        {
            Generator* gen;
            void operator()(const NodeTermIntLit* term_int_lit) const
            {

            #if defined(IPLATFORM_LINUX)
                gen->m_output << "    mov rax, " << term_int_lit->int_lit.value.value() << "\n";
                gen->push("rax");
            #elif defined(IPLATFORM_WINDOWS)
                gen->m_output << "    movl $" << term_int_lit->int_lit.value.value() << ", %eax\n";
                gen->push("rax");
            #endif

            }


            void operator()(const NodeTermIdent* term_ident) const
            {
                if (!gen->m_vars.count(term_ident->ident.value.value()))
                {
                    LLOG(RED_TEXT("Undeclared identifier: "), term_ident->ident.value.value(), "\n");
                    exit(EXIT_FAILURE);
                }

            #if defined(IPLATFORM_LINUX)
                const auto& var = gen->m_vars.at(term_ident->ident.value.value());
                std::stringstream offset;
                offset << "QWORD [rsp + " << (gen->m_stack_size - var.stack_loc - 1) * 8 << "]\n";
                gen->push(offset.str());
            #elif defined(IPLATFORM_WINDOWS)
                const auto& var = gen->m_vars.at(term_ident->ident.value.value());
                gen->m_output << "    movl -" << (var.stack_loc + 1) * 8 << "(%rbp), %eax\n";
                gen->push("rax");
   
            #endif

            }
        };

        TermVisitor visitor({.gen = this});
        std::visit(visitor, term->var);
    }

    void gen_expr(const NodeExpr* expr)
    {
        struct ExprVisitor
        {
            Generator* gen;
            void operator()(const NodeTerm* term) const
            {
                gen->gen_term(term);
            }

            void operator()(const NodeBinExpr* bin_expr) const
            {

            #if defined(IPLATFORM_LINUX)
                gen->gen_expr(bin_expr->add->lhs);
                gen->gen_expr(bin_expr->add->rhs);
                gen->pop("rax");
                gen->pop("rbx");
                gen->m_output << "    add rax, rbx\n";
                gen->push("rax");
            #elif defined(IPLATFORM_WINDOWS)
                gen->gen_expr(bin_expr->add->lhs);
                gen->gen_expr(bin_expr->add->rhs);
                gen->pop("rax");
                gen->pop("rcx");
                gen->m_output << "    addl %eax, %ecx\n";
                gen->push("rcx");
            #endif

            }
        };

        ExprVisitor visitor { .gen = this };
        std::visit(visitor, expr->var);
    }

    void gen_stmt(const NodeStmt* stmt)
    {
        struct StmtVisitor 
        {
            Generator* gen;
            void operator()(const NodeStmtExit* stmt_exit) const
            {

            #if defined(IPLATFORM_LINUX)
                gen->gen_expr(stmt_exit->expr);
                gen->m_output << "    mov rax, 60\n";
                gen->pop("rdi");
                gen->m_output << "    syscall\n";
            #elif defined(IPLATFORM_WINDOWS)
                gen->gen_expr(stmt_exit->expr);
                gen->pop("rax");
                gen->m_output << "    movq %rbp, %rsp\n";
                gen->m_output << "    popq %rbp\n";
                gen->m_output << "    ret\n";
            #endif

            }


            void operator()(const NodeStmtLet* stmt_let) const
            {
                if (gen->m_vars.count(stmt_let->ident.value.value()))
                {
                    LLOG(RED_TEXT("Identifier already used: "), stmt_let->ident.value.value(), "\n");
                    exit(EXIT_FAILURE);
                }

                gen->m_vars.insert({ stmt_let->ident.value.value(), Var { .stack_loc = gen->m_stack_size } });
                gen->gen_expr(stmt_let->expr);
            }
        };

        StmtVisitor visitor { .gen = this };
        std::visit(visitor, stmt->var);
    }

    [[nodiscard]] std::string generate()
    {

    #if defined(IPLATFORM_LINUX)
        m_output << "global _start\n_start:\n";
    #elif defined(IPLATFORM_WINDOWS)
        m_output << ".section .text\n";
        m_output << ".global main\n";
        m_output << "\n";
        m_output << "main:\n";
        m_output << "    pushq %rbp\n";
        m_output << "    movq %rsp, %rbp\n";
    #endif

        for (const NodeStmt* stmt : m_prog.stmts)
            gen_stmt(stmt);

    #if defined(IPLATFORM_LINUX)
        m_output << "    mov rax, 60\n";
        m_output << "    mov rdi, 0\n";
        m_output << "    syscall\n";
    #elif defined(IPLATFORM_WINDOWS)
        m_output << "    movl $0, %eax\n";
        m_output << "    movq %rbp, %rsp\n";
        m_output << "    popq %rbp\n";
        m_output << "    ret\n";
    #endif

        return m_output.str();

    }

private:

    void push(const std::string& reg)
    {

    #if defined(IPLATFORM_LINUX)
        m_output << "    push " << reg << "\n";
    #elif defined(IPLATFORM_WINDOWS)
        m_output << "    pushq %" << reg << "\n";
    #endif

        m_stack_size++;
    }

    void pop(const std::string& reg)
    {

    #if defined(IPLATFORM_LINUX)
        m_output << "    pop " << reg << "\n";
    #elif defined(IPLATFORM_WINDOWS)
        m_output << "    popq %" << reg << "\n";
    #endif
        m_stack_size--;

    }

    struct Var
    {
        size_t stack_loc;
    };

    const NodeProg m_prog;
    std::stringstream m_output;
    size_t m_stack_size = 0;
    std::map<std::string, Var> m_vars {};
};