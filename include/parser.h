#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"
#include "scope.h"

typedef struct PARSER_STRUCT
{
    lexer_t *lexer;
    token_t *current_token;
    token_t *previous_token;
    scope_t *scope;
}parser_t;



parser_t *init_parser(lexer_t *lexer);
void parser_eat(parser_t *parser, int token_type);

ast_t *parser_parse(parser_t *parser, scope_t *scope);
ast_t *parser_parse_statement(parser_t *parser, scope_t *scope);
ast_t *parser_parse_statements(parser_t *parser, scope_t *scope);
ast_t *parser_parse_expression(parser_t *parser, scope_t *scope);
ast_t *parser_parse_factor(parser_t *parser, scope_t *scope);
ast_t *parser_parse_term(parser_t *parser, scope_t *scope);
ast_t *parser_parse_function_call(parser_t *parser, scope_t *scope);
ast_t *parser_parse_variable(parser_t *parser, scope_t *scope);
ast_t *parser_parse_variable_definition(parser_t *parser, scope_t *scope);
ast_t *parser_parse_function_definition(parser_t *parser, scope_t *scope);
ast_t *parser_parse_string(parser_t *parser, scope_t *scope);
ast_t *parser_parse_id(parser_t *parser, scope_t *scope);



ast_t *parser_parse_number(parser_t *parser, scope_t *scope);
ast_t *parser_parse_while_loop(parser_t *parser, scope_t *scope);
ast_t *parser_parse_if_cond(parser_t *parser, scope_t *scope);
ast_t *parser_parse_function_return(parser_t *parser, scope_t *scope);
ast_t *parser_parse_for_loop(parser_t *parser, scope_t *scope);
ast_t *parser_parse_assignment(parser_t *parser, scope_t *scope);
ast_t *parser_parse_for_loop_parameters(parser_t *parser, scope_t *scope);

#endif
