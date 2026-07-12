#include <stdlib.h>
#include <stdio.h>

#include "token.h"



token_t *init_token(int type, char *value)
{
    token_t *token = calloc(1, sizeof(struct TOKEN_STRUCT));
    if(token == NULL)
    {
        fprintf(stderr, "Error: Token reader creation failed\n");
        exit(EXIT_FAILURE);
    }
    token->type = type;
    token->value = value;

    return token;
}