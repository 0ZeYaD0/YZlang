#pragma once

#include "YLogger/logger.h"
#include "parser.hpp"
#include <cassert>
#include <sstream>
#include <vector>
#include <algorithm>
#include "core/defines.h"

class Generator
{
public:
    inline explicit Generator(NodeProg prog)
        : m_prog(std::move(prog))
    {
    }

    void gen_term(const NodeTerm *term)
    {
        struct TermVisitor
        {
            Generator &gen;
            void operator()(const NodeTermIntLit *term_int_lit) const
            {
#if defined(IPLATFORM_LINUX)
                gen.m_output << "    mov rax, " << term_int_lit->int_lit.value.value() << "\n";
                gen.push("rax");
#elif defined(IPLATFORM_WINDOWS)
                gen.m_output << "    movl $" << term_int_lit->int_lit.value.value() << ", %eax\n";
                gen.push("rax");
#endif
            }

            void operator()(const NodeTermIdent *term_ident) const
            {
                const auto &name = term_ident->ident.value.value();
                size_t offset;
                const Var *var = gen.find_var(name, &offset);
                if (!var)
                {
                    LLOG(RED_TEXT("Undeclared identifier: "), name, "\n");
                    exit(EXIT_FAILURE);
                }
#if defined(IPLATFORM_WINDOWS)
                gen.m_output << "    movl -" << offset << "(%rbp), %eax\n";
                gen.push("rax");
#elif defined(IPLATFORM_LINUX)
                gen.m_output << "    mov rax, QWORD [rsp + " << offset << "]\n";
                gen.push("rax");
#endif
            }

            void operator()(const NodeTermParen *term_paren) const
            {
                gen.gen_expr(term_paren->expr);
            }
        };

        TermVisitor visitor({.gen = *this});
        std::visit(visitor, term->var);
    }

    void gen_bin_expr(const NodeBinExpr *bin_expr)
    {
        struct BinExprVisitor
        {
            Generator &gen;
            void operator()(const NodeBinExprAdd *add)
            {
                gen.gen_expr(add->rhs);
                gen.gen_expr(add->lhs);
#if defined(IPLATFORM_LINUX)
                gen.pop("rbx");
                gen.pop("rax");
                gen.m_output << "    add rax, rbx\n";
                gen.push("rax");
#elif defined(IPLATFORM_WINDOWS)
                gen.pop("rcx");
                gen.pop("rax");
                gen.m_output << "    addl %eax, %ecx\n";
                gen.push("rcx");
#endif
            }
            void operator()(const NodeBinExprSub *sub)
            {
                gen.gen_expr(sub->rhs);
                gen.gen_expr(sub->lhs);
#if defined(IPLATFORM_LINUX)
                gen.pop("rbx");
                gen.pop("rax");
                gen.m_output << "    sub rbx, rax\n";
                gen.push("rbx");
#elif defined(IPLATFORM_WINDOWS)
                gen.pop("rax");                          // LEFT operand
                gen.pop("rcx");                          // RIGHT operand
                gen.m_output << "    subl %ecx, %eax\n"; // rax = rax - rcx
                gen.push("rax");
#endif
            }
            void operator()(const NodeBinExprMulti *multi)
            {
                gen.gen_expr(multi->rhs);
                gen.gen_expr(multi->lhs);
#if defined(IPLATFORM_LINUX)
                gen.pop("rbx");
                gen.pop("rax");
                gen.m_output << "    mul rbx\n";
                gen.push("rax");
#elif defined(IPLATFORM_WINDOWS)
                gen.pop("rcx");
                gen.pop("rax");
                gen.m_output << "    imul %ecx, %eax\n";
                gen.push("rax");
#endif
            }
            void operator()(const NodeBinExprDiv *div)
            {
                gen.gen_expr(div->rhs);
                gen.gen_expr(div->lhs);
#if defined(IPLATFORM_LINUX)
                gen.pop("rbx");
                gen.pop("rax");
                gen.m_output << "    xor rdx, rdx\n";
                gen.m_output << "    idiv rbx\n";
                gen.push("rax");
#elif defined(IPLATFORM_WINDOWS)
                gen.pop("rax"); // LEFT operand (dividend)
                gen.pop("rcx"); // RIGHT operand (divisor)
                gen.m_output << "    xorl %edx, %edx\n";
                gen.m_output << "    idivl %ecx\n";
                gen.push("rax");
#endif
            }
        };
        BinExprVisitor visitor{.gen = *this};
        std::visit(visitor, bin_expr->var);
    }

    void gen_expr(const NodeExpr *expr)
    {
        struct ExprVisitor
        {
            Generator &gen;
            void operator()(const NodeTerm *term) const
            {
                gen.gen_term(term);
            }

            void operator()(const NodeBinExpr *bin_expr) const
            {
                gen.gen_bin_expr(bin_expr);
            }
        };

        ExprVisitor visitor{.gen = *this};
        std::visit(visitor, expr->var);
    }

    void gen_stmt(const NodeStmt *stmt)
    {
        struct StmtVisitor
        {
            Generator &gen;
            void operator()(const NodeStmtExit *stmt_exit) const
            {
                gen.gen_expr(stmt_exit->expr);
                gen.pop("rax");

#if defined(IPLATFORM_WINDOWS)
                gen.m_output << "    movl %eax, %ecx\n";
                gen.m_output << "    subq $32, %rsp\n";
                gen.m_output << "    call exit\n";
                gen.m_output << "    addq $32, %rsp\n";
#elif defined(IPLATFORM_LINUX)
                gen.m_output << "    mov rdi, rax\n";
                gen.m_output << "    mov rax, 60\n";
                gen.m_output << "    syscall\n";
#endif
                gen.m_has_explicit_exit = true;
            }

            void operator()(const NodeStmtLet *stmt_let) const
            {
                auto it = std::find_if(gen.m_vars.cbegin(), gen.m_vars.cend(), [&](const Var &var)
                                       {
                    bool in_current_scope = false;
                    if (!gen.m_scopes.empty()) {
                        size_t current_scope_start_index = gen.m_scopes.back();
                        size_t var_index = &var - &gen.m_vars[0];
                        if (var_index >= current_scope_start_index) {
                            in_current_scope = true;
                        }
                    } else { 
                        in_current_scope = true;
                    }
                    return in_current_scope && var.name == stmt_let->ident.value.value(); });

                if (it != gen.m_vars.cend())
                {
                    LLOG(RED_TEXT("Identifier already used in this scope: "), stmt_let->ident.value.value(), "\n");
                    exit(EXIT_FAILURE);
                }

                gen.gen_expr(stmt_let->expr);
                gen.m_vars.push_back({.name = stmt_let->ident.value.value(), .stack_loc = gen.m_stack_size - 1});
            }
            void operator()(const NodeStmtOut *stmt_out) const
            {
                for (const auto *expr : stmt_out->exprs)
                {
                    gen.gen_expr(expr);
                    gen.pop("rax");

#if defined(IPLATFORM_WINDOWS)
                    bool is_stack_misaligned = (gen.m_vars.size() * 8) % 16 != 0;

                    if (is_stack_misaligned)
                        gen.m_output << "    subq $40, %rsp\n";
                    else
                        gen.m_output << "    subq $32, %rsp\n";

                    gen.m_output << "    leaq .LC_fmt_int(%rip), %rcx\n";
                    gen.m_output << "    movl %eax, %edx\n";
                    gen.m_output << "    xorl %eax, %eax\n";
                    gen.m_output << "    call printf\n";

                    if (is_stack_misaligned)
                        gen.m_output << "    addq $40, %rsp\n";
                    else
                        gen.m_output << "    addq $32, %rsp\n";
#endif
                }
            }

            void operator()(const NodeStmtAssign *stmt_assign) const
            {
                gen.gen_expr(stmt_assign->expr);

                const auto &name = stmt_assign->ident.value.value();
                auto it = std::find_if(gen.m_vars.begin(), gen.m_vars.end(),
                                       [&](const Var &var)
                                       { return var.name == name; });

                if (it == gen.m_vars.end())
                {
                    LLOG(RED_TEXT("Undeclared identifier in assignment: "), name, "\n");
                    exit(EXIT_FAILURE);
                }

                gen.pop("rax");

#if defined(IPLATFORM_WINDOWS)
                size_t offset = (&(*it) - &gen.m_vars[0] + 1) * 8;
                gen.m_output << "    movl %eax, -" << offset << "(%rbp)\n";
#elif defined(IPLATFORM_LINUX)
                size_t offset = (gen.m_stack_size - it->stack_loc - 1) * 8;
                gen.m_output << "    mov QWORD [rsp + " << offset << "], rax\n";
#endif
            }

            void operator()(const NodeStmtBlock *block) const
            {
                gen.gen_stmt_block(block);
            }

            void operator()(const NodeStmtIf *stmt_if) const
            {
                gen.gen_expr(stmt_if->expr);
                gen.pop("rax");

                static size_t label_count = 0;
                std::string else_label = "if_else_" + std::to_string(label_count);
                std::string end_label = "if_end_" + std::to_string(label_count++);

#if defined(IPLATFORM_WINDOWS)
                gen.m_output << "    testl %eax, %eax\n";
                gen.m_output << "    jz " << else_label << "\n";
#elif defined(IPLATFORM_LINUX)
                gen.m_output << "    test eax, eax\n";
                gen.m_output << "    jz " << else_label << "\n";
#endif

                gen.gen_stmt_block(stmt_if->scope);
                gen.m_output << "    jmp " << end_label << "\n";
                gen.m_output << else_label << ":\n";

                if (stmt_if->pred.has_value())
                {
                    gen.gen_if_pred(stmt_if->pred.value(), end_label);
                }

                gen.m_output << end_label << ":\n";
            }
        };

        StmtVisitor visitor{.gen = *this};
        std::visit(visitor, stmt->var);
    }

    [[nodiscard]] std::string generate()
    {
#if defined(IPLATFORM_WINDOWS)
        m_output << ".section .rodata\n";
        m_output << ".LC_fmt_int:\n    .string \"%d\\n\"\n";
        m_output << ".section .text\n";
        m_output << ".extern printf\n";
        m_output << ".extern exit\n";
        m_output << ".global main\n";
        m_output << "main:\n";
        m_output << "    pushq %rbp\n";
        m_output << "    movq %rsp, %rbp\n";
#elif defined(IPLATFORM_LINUX)
        m_output << "global _start\n_start:\n";
#endif
        for (const NodeStmt *s : m_prog.stmts)
            gen_stmt(s);

#if defined(IPLATFORM_WINDOWS)
        if (!m_has_explicit_exit)
            m_output << "    movl $0, %eax\n";
        m_output << "    movq %rbp, %rsp\n";
        m_output << "    popq %rbp\n";
        m_output << "    ret\n";
#elif defined(IPLATFORM_LINUX)
        m_output << "    mov rax, 60\n";
        if (m_has_explicit_exit)
        {
            m_output << "    pop rdi\n";
        }
        else
        {
            m_output << "    mov rdi, 0\n";
        }
        m_output << "    syscall\n";
#endif
        return m_output.str();
    }

    void gen_if_pred(const NodeIfPred *pred, const std::string &end_label)
    {
        struct IfPredVisitor
        {
            Generator &gen;
            const std::string &end_label;

            void operator()(const NodeIfPredElif *elif) const
            {
                gen.gen_expr(elif->expr);
                gen.pop("rax");

                static size_t elif_label_count = 0;
                std::string next_label = "elif_next_" + std::to_string(elif_label_count++);

#if defined(IPLATFORM_WINDOWS)
                gen.m_output << "    testl %eax, %eax\n";
                gen.m_output << "    jz " << next_label << "\n";
#elif defined(IPLATFORM_LINUX)
                gen.m_output << "    test eax, eax\n";
                gen.m_output << "    jz " << next_label << "\n";
#endif

                gen.gen_stmt_block(elif->scope);
                gen.m_output << "    jmp " << end_label << "\n";
                gen.m_output << next_label << ":\n";

                if (elif->pred.has_value())
                {
                    gen.gen_if_pred(elif->pred.value(), end_label);
                }
            }

            void operator()(const NodeIfPredElse *else_pred) const
            {
                gen.gen_stmt_block(else_pred->scope);
            }
        };

        IfPredVisitor visitor{.gen = *this, .end_label = end_label};
        std::visit(visitor, pred->var);
    }

private:
    void push(const std::string &reg)
    {
#if defined(IPLATFORM_LINUX)
        m_output << "    push " << reg << "\n";
#elif defined(IPLATFORM_WINDOWS)
        m_output << "    pushq %" << reg << "\n";
#endif
        m_stack_size++;
    }

    void pop(const std::string &reg)
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
        std::string name;
        size_t stack_loc;
    };

    std::vector<Var> m_vars{};
    std::vector<size_t> m_scopes{};

    void push_scope()
    {
        m_scopes.push_back(m_vars.size());
    }

    void pop_scope()
    {
        size_t pop_count = m_vars.size() - m_scopes.back();
        if (pop_count > 0)
        {
#if defined(IPLATFORM_WINDOWS)
            m_output << "    addq $" << (pop_count * 8) << ", %rsp\n";
#elif defined(IPLATFORM_LINUX)
            m_output << "    add rsp, " << (pop_count * 8) << "\n";
#endif
            m_stack_size -= pop_count;
            for (size_t i = 0; i < pop_count; i++)
            {
                m_vars.pop_back();
            }
        }
        m_scopes.pop_back();
    }

    const Var *find_var(const std::string &name, size_t *out_offset) const
    {
        auto it = std::find_if(m_vars.crbegin(), m_vars.crend(), [&](const Var &var)
                               { return var.name == name; });

        if (it == m_vars.crend())
        {
            return nullptr;
        }

        const Var &var = *it;
#if defined(IPLATFORM_WINDOWS)
        *out_offset = (&var - &m_vars[0] + 1) * 8;
#elif defined(IPLATFORM_LINUX)
        *out_offset = (m_stack_size - var.stack_loc - 1) * 8;
#endif
        return &var;
    }

    void gen_stmt_block(const NodeStmtBlock *block)
    {
        push_scope();
        for (auto *s : block->stmts)
            gen_stmt(s);
        pop_scope();
    }

    const NodeProg m_prog;
    std::stringstream m_output;
    size_t m_stack_size = 0;
    bool m_has_explicit_exit = false;
};