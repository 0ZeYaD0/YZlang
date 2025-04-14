#include "core/genrate_token.h"

void lexer(FILE *file)
{
    char cur = fgetc(file);
    int token_count = 0;

    printf("LEXICAL ANALYSIS\n");
    printf("----------------\n");

    while (cur != EOF)
    {
        if (isspace(cur))
        {
            cur = fgetc(file);
            continue;
        }

        token_count++;

        if (cur == ';' || cur == '(' || cur == ')')
        {
            TokenSeparator *test_token = generate_separator(cur);

            const char *sep_type;
            if (cur == ';')
                sep_type = "SEMICOLON";
            else if (cur == '(')
                sep_type = "OPEN_PARENTHESIS";
            else
                sep_type = "CLOSE_PARENTHESIS";

            printf("SEPARATOR: %c (%s)\n", cur, sep_type);
            free(test_token);
        }
        else if (isdigit(cur))
        {
            TokenLit *test_token = generate_number(cur, file);
            printf("NUMBER: %d\n", test_token->value);
            free(test_token);
        }
        else if (isalpha(cur))
        {
            TokenKeyword *test_token = generate_keyword(cur, file);
            if (test_token->type != -1)
            {
                printf("KEYWORD: exit\n");
            }
            else
            {
                printf("UNKNOWN: identifier\n");
            }
            free(test_token);
        }
        else if (cur == '+' || cur == '-' || cur == '*' || cur == '/' || cur == '=')
        {
            TokenOperator *test_token = generate_operator(cur);

            const char *op_type;
            switch (cur)
            {
            case '+':
                op_type = "PLUS";
                break;
            case '-':
                op_type = "MINUS";
                break;
            case '*':
                op_type = "MULTIPLY";
                break;
            case '/':
                op_type = "DIVIDE";
                break;
            case '=':
                op_type = "ASSIGN";
                break;
            }

            printf("OPERATOR: %c (%s)\n", cur, op_type);
            free(test_token);
        }

        cur = fgetc(file);
    }

    printf("----------------\n");
    printf("TOTAL TOKENS: %d\n", token_count);
}