#ifndef LEXER_H
#define LEXER_H

#include "token.h"
#include <stdlib.h>

typedef struct LEXER_STRUCT
{
	char c;
	size_t i;
	char *contents;
	
}lexer_t;

lexer_t *init_lexer(char *contents);
void lexer_advance(lexer_t *lexer);
void lexer_skip_white_space(lexer_t *lexer);
void lexer_skip_carriage(lexer_t *lexer);
token_t *lexer_get_next_token(lexer_t *lexer);
token_t *lexer_collect_string(lexer_t *lexer);
token_t *lexer_collect_id(lexer_t *lexer);
char *lexer_get_current_char_as_string(lexer_t *lexer);
token_t *_lexer_advance_with_token(lexer_t *lexer, token_t *token);

token_t *lexer_collect_number(lexer_t *lexer);
token_t *lexer_peek_next_token(lexer_t *lexer);
int is_decimal(const char *str);

#endif
