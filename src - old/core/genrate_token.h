#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "Token.h"

TokenLit *generate_number(char cur, FILE *file)
{
    TokenLit *token = malloc(sizeof(TokenLit));
    if (!token)
    {
        fprintf(stderr, "Error: Memory allocation failed for TokenLit\n");
        return NULL;
    }

    token->type = INT;

    char *value = malloc(sizeof(char) * 8); // Temporary buffer for the number as a string
    if (!value)
    {
        fprintf(stderr, "Error: Memory allocation failed for number buffer\n");
        free(token);
        return NULL;
    }

    int value_index = 0;

    while (isdigit(cur) && cur != EOF)
    {
        if (value_index >= 7) // Prevent buffer overflow
            break;

        value[value_index] = cur; // Store the digit
        value_index++;
        cur = fgetc(file);
    }
    value[value_index] = '\0';

    // Put the non-digit character back into the input stream
    if (cur != EOF)
    {
        ungetc(cur, file);
    }

    token->value = atoi(value);
    free(value);
    return token;
}

TokenKeyword *generate_keyword(char cur, FILE *file)
{
    TokenKeyword *token = malloc(sizeof(TokenKeyword));
    if (!token)
    {
        fprintf(stderr, "Error: Memory allocation failed for TokenKeyword\n");
        return NULL;
    }

    char *keyword = malloc(sizeof(char) * 32); // Allocate memory for the keyword (max length: 31)
    if (!keyword)
    {
        fprintf(stderr, "Error: Memory allocation failed for keyword buffer\n");
        free(token);
        return NULL;
    }

    int keyword_idx = 0;

    // Read the keyword
    while (isalpha(cur) && cur != EOF)
    {
        if (keyword_idx >= 31)
        { // Prevent buffer overflow
            fprintf(stderr, "Warning: Keyword exceeds maximum length, truncating\n");
            break;
        }

        keyword[keyword_idx] = cur;
        keyword_idx++;
        cur = fgetc(file);
    }
    keyword[keyword_idx] = '\0'; // Null-terminate the string

    // Put the non-alpha character back into the input stream
    if (cur != EOF)
    {
        ungetc(cur, file);
    }

    // Check if the keyword matches known keywords
    if (strcmp(keyword, "exit") == 0)
    {
        token->type = EXIT;
    }
    else
    {
        token->type = -1; // Assign an invalid type for unknown keywords
    }

    free(keyword); // Free the temporary keyword buffer
    return token;
}

TokenSeparator *generate_separator(char cur)
{
    TokenSeparator *token = malloc(sizeof(TokenSeparator));
    if (!token)
    {
        fprintf(stderr, "Error: Memory allocation failed for TokenSeparator\n");
        return NULL;
    }

    // Assign the type based on the character
    if (cur == ';')
    {
        token->type = SEMI;
    }
    else if (cur == '(')
    {
        token->type = OPEN_PAR;
    }
    else if (cur == ')')
    {
        token->type = CLOSE_PAR;
    }
    else
    {
        fprintf(stderr, "Error: Unknown separator character: '%c'\n", cur);
        free(token);
        return NULL;
    }

    return token;
}

TokenOperator *generate_operator(char cur)
{
    TokenOperator *token = malloc(sizeof(TokenOperator));
    if (!token)
    {
        fprintf(stderr, "Error: Memory allocation failed for TokenOperator\n");
        return NULL;
    }

    switch (cur)
    {
    case '+':
        token->type = PLUS;
        break;
    case '-':
        token->type = MINUS;
        break;
    case '*':
        token->type = MULTIPLY;
        break;
    case '/':
        token->type = DIVIDE;
        break;
    case '=':
        token->type = ASSIGN;
        break;
    default:
        fprintf(stderr, "Error: Unknown operator character: '%c'\n", cur);
        free(token);
        return NULL;
    }

    return token;
}
