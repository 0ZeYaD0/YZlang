#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"

typedef enum
{
    SEMI,
    OPEN_PAR,
    CLOSE_PAR
} TypeSeparator;

typedef enum
{
    EXIT,
} TypeKeyword;

typedef enum
{
    INT
} TypeLit;

typedef struct
{
    TypeSeparator type;
} TokenSeparator;

typedef struct
{
    TypeKeyword type;
} TokenKeyword;

typedef struct
{
    TypeLit type;
    int value;
} TokenLit;

int main(void)
{
    FILE *file = fopen("D:\\Codes\\Zlang\\test\\main.zl", "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return 1;
    }

    lexer(file);

    return 0;
}