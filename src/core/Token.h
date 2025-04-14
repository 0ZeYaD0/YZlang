#ifndef TOKEN_H
#define TOKEN_H

typedef enum
{
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    ASSIGN,
} TypeOperator;

typedef struct
{
    TypeOperator type;
} TokenOperator;

typedef enum
{
    SEMI,
    OPEN_PAR,
    CLOSE_PAR
} TypeSeparator;

typedef enum
{
    EXIT
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

#endif