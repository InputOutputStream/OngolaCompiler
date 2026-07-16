
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <math.h>

#include "lexer.h"


lexer_t *init_lexer(char *contents){
    if(contents == NULL)
    {
        return NULL;   
    }

    lexer_t *lexer = calloc(1, sizeof(struct LEXER_STRUCT));
    if(lexer == NULL)
    {
        fprintf(stderr, "Error: Lexer creation failed\n");
        exit(EXIT_FAILURE);
    }
    lexer->contents = contents;
    lexer->i = 0;
    lexer->c = contents[lexer->i];

    return lexer;
}

void lexer_advance(lexer_t *lexer){
    if(lexer == NULL)
    {
        return;   
    }

    if(lexer->i < strlen(lexer->contents) - 1)
    {
        lexer->i += 1;
        lexer->c = lexer->contents[lexer->i];
    }
    else
    {
        lexer->c = '\0';
    }
}


void lexer_skip_carriage(lexer_t *lexer)
{
    if(lexer == NULL)
        return;

    while(lexer->c == '\r' || lexer->c == 13)
    {
        lexer_advance(lexer);
        if(lexer->c == '\0')
            break;
    }   
}
void lexer_skip_white_space(lexer_t *lexer){
    if(lexer == NULL)
        return;

    while(lexer->c == ' ' || lexer->c == 10)
    {
        lexer_advance(lexer);
        if(lexer->c == '\0')
            break;
    } 
}

token_t* lexer_peek_next_token(lexer_t *lexer){
    if (lexer == NULL) return NULL;

    lexer_t temp = *lexer;              
    return lexer_get_next_token(&temp); 
}

token_t *lexer_get_next_token(lexer_t *lexer)
{
    while(lexer->c != '\0' && lexer->i < strlen(lexer->contents))
    {
        while (lexer->c == ' ' || lexer->c == 10 || lexer->c == 13 || lexer->c == '\r') {
            lexer_skip_white_space(lexer);
            lexer_skip_carriage(lexer);
        }

        if (lexer->c == '\0')
            break;

        if(isdigit(lexer->c) || lexer->c == '.')
            return lexer_collect_number(lexer); 

        if(lexer->c == '"')
            return lexer_collect_string(lexer); 

        if(isalnum(lexer->c) || lexer->c == '_')
            return lexer_collect_id(lexer);

        switch(lexer->c)
        {
            case ',':
                return _lexer_advance_with_token(lexer, init_token(TOKEN_COMMA, lexer_get_current_char_as_string(lexer)));
                break;
            case ';':
                return _lexer_advance_with_token(lexer, init_token(TOKEN_SEMI, lexer_get_current_char_as_string(lexer)));
                break;
            case '(':
                return _lexer_advance_with_token(lexer, init_token(TOKEN_LPAREN, lexer_get_current_char_as_string(lexer)));
                break;
            case ')':
                return _lexer_advance_with_token(lexer, init_token(TOKEN_RPAREN, lexer_get_current_char_as_string(lexer)));
                break;
            case '{':
                return _lexer_advance_with_token(lexer, init_token(TOKEN_LBRACE, lexer_get_current_char_as_string(lexer)));
                break;
            case '}':
                return _lexer_advance_with_token(lexer, init_token(TOKEN_RBRACE, lexer_get_current_char_as_string(lexer)));
            case '[':
                return _lexer_advance_with_token(lexer, init_token(TOKEN_LSBRACE, lexer_get_current_char_as_string(lexer)));
                break;
            case ']':
                return _lexer_advance_with_token(lexer, init_token(TOKEN_RSBRACE, lexer_get_current_char_as_string(lexer)));
                break;
            case ':':
                return _lexer_advance_with_token(lexer, init_token(TOKEN_COLON, lexer_get_current_char_as_string(lexer)));
                break;
            case '+':
                return _lexer_advance_with_token(lexer, init_token(TOKEN_PLUS, lexer_get_current_char_as_string(lexer)));
                break;
            case '-':
                return _lexer_advance_with_token(lexer, init_token(TOKEN_MINUS, lexer_get_current_char_as_string(lexer)));
                break;
            case '/':
                lexer_advance(lexer);
                if (lexer->c == '/') {
                    while (lexer->c != '\n' && lexer->c != '\0') {
                        lexer_advance(lexer);
                    }
                    continue;
                }
                if (lexer->c == '*') {
                    lexer_advance(lexer);
                    while (lexer->c != '\0') {
                        if (lexer->c == '*') {
                            lexer_advance(lexer);
                            if (lexer->c == '/') {
                                lexer_advance(lexer);
                                break;
                            }
                            continue;
                        }
                        lexer_advance(lexer);
                    }
                    continue;
                }
                return init_token(TOKEN_SLASH, "/");
            case '*':
                return _lexer_advance_with_token(lexer, init_token(TOKEN_STAR, lexer_get_current_char_as_string(lexer)));
                break;

            case '<':                    
                lexer_advance(lexer);
                if (lexer->c == '=') {
                    lexer_advance(lexer);
                    return init_token(TOKEN_LTEQ, "<=");
                }
                return _lexer_advance_with_token(lexer, init_token(TOKEN_LT, lexer_get_current_char_as_string(lexer)));
                break;
            case '>':
                lexer_advance(lexer);
                if (lexer->c == '=') {
                    lexer_advance(lexer);
                    return init_token(TOKEN_GTEQ, ">=");
                }
                return _lexer_advance_with_token(lexer, init_token(TOKEN_GT, lexer_get_current_char_as_string(lexer)));
                break;
            case '!':
                lexer_advance(lexer);
                if (lexer->c == '=') {
                    lexer_advance(lexer);
                    return init_token(TOKEN_NEQ, "!=");
                }
                return _lexer_advance_with_token(lexer, init_token(TOKEN_NOT, lexer_get_current_char_as_string(lexer)));
                break;
            case '=':
                lexer_advance(lexer);
                if (lexer->c == '=') {
                    lexer_advance(lexer);
                    return init_token(TOKEN_EQEQ, "==");
                }
        
                return _lexer_advance_with_token(lexer, init_token(TOKEN_EQUALS, lexer_get_current_char_as_string(lexer)));
                break;
            default:
                fprintf(stderr, "Lexer: unrecognized character '%c' (code %d) at position %zu\n",
                      isprint((unsigned char)lexer->c) ? lexer->c : '?', (int)(unsigned char)lexer->c, lexer->i);
                lexer_advance(lexer); // always make forward progress, even on unknown input
                break;        }
    }

    return init_token(TOKEN_EOF, "\0");
}


token_t *_lexer_advance_with_token(lexer_t *lexer, token_t *token)
{
    if(lexer == NULL)
        return NULL;

    lexer_advance(lexer);
    return token;
}

token_t *lexer_collect_string(lexer_t *lexer){
    if(lexer == NULL)
        return NULL;

    lexer_advance(lexer);
    char *value = calloc(1, sizeof(char));
    if(value == NULL)
    {
        fprintf(stderr, "Error: Lexer Advance Allocation object creation failed\n");
        exit(EXIT_FAILURE);
    }

    value[0] = '\0';
    while(lexer->c != '"' && lexer->c != '\0')
    {
        char *s = lexer_get_current_char_as_string(lexer);
        value = realloc(value, (strlen(value) + strlen(s) + 1) * sizeof(char));
        strcat(value, s);
        lexer_advance(lexer);
    }
    
    if(lexer->c != '"')
    {
        fprintf(stderr, "ERROR: Unterminated string literal!\n");
        free(value);
        return NULL;
    }
    
    lexer_advance(lexer);

    return init_token(TOKEN_STRING, value);
}


token_t *lexer_collect_id(lexer_t *lexer)
{
    if(lexer == NULL)
        return NULL;

    char *value = calloc(1, sizeof(char));
    value[0] = '\0';

    while(isalnum(lexer->c) || lexer->c == '_')
    {
        char *s = lexer_get_current_char_as_string(lexer);
        value = realloc(value, (strlen(value) + strlen(s) + 1) * sizeof(char));
        strcat(value, s);
        lexer_advance(lexer);
    }

    if(strcmp(value, "de")==0)
       return init_token(TOKEN_IF, value);
    else if(strcmp(value, "nensole")==0)
       return init_token(TOKEN_ELSE, value);
    else if(strcmp(value, "tsiamenke")==0)
       return init_token(TOKEN_WHILE, value);
    else if(strcmp(value, "bisou")==0)
       return init_token(TOKEN_FOR, value);
    else if(strcmp(value, "delorene")==0 || strcmp(value, "resoucte")==0)
       return init_token(TOKEN_RETURN, value);
    else if(strcmp(value, "kesa") == 0)
        return init_token(TOKEN_FUNC, value);

    return init_token(TOKEN_ID, value);
}


int is_decimal(const char *str) {
    if (str == NULL || *str == '\0') return 0;

    // Skip leading whitespace manually if desired, or let strtod handle it
    while (isspace((unsigned char)*str)) str++;
    if (*str == '\0') return 0;

    char *endptr;
    errno = 0;
    double val = strtod(str, &endptr);

    // Check range errors
    if (errno == ERANGE) return 0;

    // Check if conversion happened and consumed the whole string (ignoring trailing whitespace)
    if (endptr == str) return 0; 
    
    // Skip trailing whitespace
    while (isspace((unsigned char)*endptr)) endptr++;
    if (*endptr != '\0') return 0;

    // Reject special values (Inf, NaN) if strictly finite decimals are required
    if (!isfinite(val)) return 0;

    // (strtod accepts 'e' or 'E', so we scan the original string for them)
    if (strchr(str, 'e') || strchr(str, 'E')) return 0;

    return 1;
}



token_t *lexer_collect_number(lexer_t *lexer)
{
    if(lexer == NULL)
        return NULL;

    char *value = calloc(1, sizeof(char));
    if(value == NULL)
    {
        fprintf(stderr, "Error: Lexer Advance Allocation object creation failed\n");
        exit(EXIT_FAILURE);
    }

    value[0] = '\0';
    while(isalnum(lexer->c))
    {
        char *s = lexer_get_current_char_as_string(lexer);
        value = realloc(value, (strlen(value) + strlen(s) + 1) * sizeof(char));
        strcat(value, s);
        lexer_advance(lexer);
    }
    if(!is_decimal(value))
    {
        fprintf(stderr, "Error: Lexer Invalid Number %s\n", value);
        exit(EXIT_FAILURE);
    }
    
    return init_token(TOKEN_NUMBER, value);
}


char *lexer_get_current_char_as_string(lexer_t *lexer){
    if(lexer == NULL)
        return NULL;

    static char str[2];
    str[0] = lexer->c;
    str[1] = '\0';

    return str;
}

