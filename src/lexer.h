#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
void lexer(FILE *file)
{
    char cur = fgetc(file);
    while (cur != EOF)
    {
        if (cur == ';')
            printf("Found Semicoln: %c\n", cur);
        else if (cur == '(')
            printf("Found open parc: %c\n", cur);
        else if (cur == ')')
            printf("Found close parc: %c\n", cur);
        else if (isdigit(cur))
            printf("Found digit: %d\n", cur - '0');
        else if (isalpha(cur))
            printf("Found char: %c\n", cur);
        cur = fgetc(file);
    }
}