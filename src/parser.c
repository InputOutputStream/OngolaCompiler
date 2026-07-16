#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdlib.h>

#include "parser.h"
#include "ast.h"
#include "scope.h"



parser_t *init_parser(lexer_t *lexer){
    if (lexer == NULL){
        fprintf(stderr, "ERROR: NULL Lexer Encounted\n");
        exit(EXIT_FAILURE);
    }

    parser_t *parser = calloc(1, sizeof(struct PARSER_STRUCT));
    if(parser == NULL)
    {
        fprintf(stderr, "Error: Parser creation failed\n");
        exit(EXIT_FAILURE);
    }
    parser->lexer = lexer;
    parser->current_token = lexer_get_next_token(lexer);
    parser->previous_token = parser->current_token;

    parser->scope = init_scope(NULL);

    return parser;
}

void parser_eat(parser_t *parser, int token_type)
{
    if ((int)parser->current_token->type == token_type)
    {
        parser->previous_token = parser->current_token;
        parser->current_token = lexer_get_next_token(parser->lexer);
    }
    else{
        fprintf(stderr, "Unexpected token %s with type %d \n", parser->current_token->value, parser->current_token->type );
        exit(EXIT_FAILURE);
    }
}

ast_t *parser_parse(parser_t *parser, scope_t *scope){
    
    if(parser == NULL)
    {
        fprintf(stderr, "Error: NULL Parser encountered\n");
        exit(EXIT_FAILURE);
    }

    return parser_parse_statements(parser, scope);
}

ast_t *parser_parse_statement(parser_t *parser, scope_t *scope){
    if(parser == NULL)
    {
        fprintf(stderr, "Error: NULL Parser encountered\n");
        exit(EXIT_FAILURE);
    }

    switch (parser->current_token->type)
    {
        case TOKEN_ID:
            return parser_parse_id(parser, scope);
        case TOKEN_IF:
            return parser_parse_if_cond(parser, scope);
        case TOKEN_WHILE:
            return parser_parse_while_loop(parser, scope);
        case TOKEN_FOR:
            return parser_parse_for_loop(parser, scope);
        case TOKEN_RETURN:
            return parser_parse_function_return(parser, scope);
        case TOKEN_FUNC:
            return parser_parse_function_definition(parser, scope);
        case TOKEN_SEMI:
            return init_ast(AST_NOOP);
        default:
            fprintf(stderr, "Unexpected token in statement: '%s' (type %d)\n",
                    parser->current_token->value, parser->current_token->type);
            exit(EXIT_FAILURE);
    }

    return init_ast(AST_NOOP);
}

ast_t *parser_parse_statements(parser_t *parser, scope_t *scope){

    if(parser == NULL || parser->current_token == NULL)
    {
        fprintf(stderr, "Error: NULL TOKEN ENCOUNTERED!\n");
        exit(EXIT_FAILURE);
    }

    ast_t *compound = init_ast(AST_COMPOUND);
    compound->scope = scope;
    ast_t *ast_statement = parser_parse_statement(parser, scope);
    ast_statement->scope = scope;

    compound->compound_value = calloc(1, sizeof(struct AST_STRUCT));
    compound->compound_value[0] = ast_statement;
    compound->compound_size++;

    while (parser->current_token->type != TOKEN_EOF &&
           parser->current_token->type != TOKEN_RBRACE)
    {
            ast_t *ast_statement = parser_parse_statement(parser, scope);
            ast_statement->scope = scope;

            compound->compound_size++;
            compound->compound_value = realloc(
                compound->compound_value,
                compound->compound_size * sizeof(struct AST_STRUCT*)
            );
            compound->compound_value[compound->compound_size - 1] = ast_statement;

            if (parser->current_token->type == TOKEN_SEMI) {
                parser_eat(parser, TOKEN_SEMI);
            }
    }

    return compound;
}


ast_t *parser_parse_factor(parser_t *parser, scope_t *scope){

    if (parser->current_token->type == TOKEN_MINUS) {
        parser_eat(parser, TOKEN_MINUS);
        ast_t *unop = init_ast(AST_UNOP);
        unop->left_value = parser_parse_factor(parser, scope); 
        return unop;
    }
    else if (parser->current_token->type == TOKEN_NUMBER) {
        ast_t *num = init_ast(AST_NUMBER);
        num->number_value = strtod(parser->current_token->value, NULL); 
        parser_eat(parser, TOKEN_NUMBER);
        return num;
    }
    else if (parser->current_token->type == TOKEN_LPAREN) {
        parser_eat(parser, TOKEN_LPAREN);
        ast_t *inner = parser_parse_expression(parser, scope);
        parser_eat(parser, TOKEN_RPAREN);
        return inner;
    }
    else if (parser->current_token->type == TOKEN_STRING) {
        return parser_parse_string(parser, scope);
    }
    else if (parser->current_token->type == TOKEN_ID) {
        return parser_parse_id(parser, scope); 
    }

    fprintf(stderr, "Unexpected token in factor: %s\n", parser->current_token->value);
    exit(EXIT_FAILURE);
}

ast_t *parser_parse_term(parser_t *parser, scope_t *scope){
    ast_t *left = parser_parse_factor(parser, scope);

    while (parser->current_token->type == TOKEN_STAR ||
           parser->current_token->type == TOKEN_SLASH) {
        char *op = (parser->current_token->type == TOKEN_STAR) ? "*" : "/";
        parser_eat(parser, parser->current_token->type);

        ast_t *right = parser_parse_factor(parser, scope);

        ast_t *binop = init_ast(AST_BINOP);
        binop->left_value = left;
        binop->right_value = right;
        binop->operator_str = op;   
        binop->scope = scope;

        left = binop; 
    }

    return left;
}

ast_t *parser_parse_addition(parser_t *parser, scope_t *scope){
    ast_t *left = parser_parse_term(parser, scope);

    while (parser->current_token->type == TOKEN_PLUS ||
           parser->current_token->type == TOKEN_MINUS) {
        char *op = (parser->current_token->type == TOKEN_PLUS) ? "+" : "-";
        parser_eat(parser, parser->current_token->type);

        ast_t *right = parser_parse_term(parser, scope);

        ast_t *binop = init_ast(AST_BINOP);
        binop->left_value = left;
        binop->right_value = right;
        binop->operator_str = op;
        binop->scope = scope;

        left = binop;
    }

    return left;
}

ast_t *parser_parse_comparison(parser_t *parser, scope_t *scope){
    ast_t *left = parser_parse_addition(parser, scope);

    if (parser->current_token->type == TOKEN_LT ||
        parser->current_token->type == TOKEN_GT ||
        parser->current_token->type == TOKEN_EQEQ ||
        parser->current_token->type == TOKEN_NEQ ||
        parser->current_token->type == TOKEN_LTEQ ||
        parser->current_token->type == TOKEN_GTEQ) {

        int tok_type = parser->current_token->type;
        char *op_str = (tok_type == TOKEN_LT) ? "<" :
                        (tok_type == TOKEN_GT) ? ">" :
                        (tok_type == TOKEN_EQEQ) ? "==" :
                        (tok_type == TOKEN_NEQ) ? "!=" :
                        (tok_type == TOKEN_LTEQ) ? "<=" : ">=";
        parser_eat(parser, tok_type);

        ast_t *right = parser_parse_addition(parser, scope);

        ast_t *binop = init_ast(AST_BINOP);
        binop->left_value = left;
        binop->right_value = right;
        binop->operator_str = op_str; 
        binop->scope = scope;

        return binop;
    }

    return left;
}

ast_t *parser_parse_expression(parser_t *parser, scope_t *scope){
    if (parser == NULL) {
        fprintf(stderr, "Error: NULL Parser encountered\n");
        exit(EXIT_FAILURE);
    }
    return parser_parse_comparison(parser, scope);
}


ast_t *parser_parse_function_call(parser_t *parser, scope_t *scope){
    ast_t *function_call = init_ast(AST_FUNCTION_CALL);
    function_call->funtion_call_name = parser->previous_token->value;
    parser_eat(parser, TOKEN_LPAREN);

    function_call->funtion_call_arguments = NULL;
    function_call->funtion_call_arguments_size = 0;

    if (parser->current_token->type != TOKEN_RPAREN) {  
        function_call->funtion_call_arguments = calloc(1, sizeof(struct AST_STRUCT*));
        ast_t *ast_expression = parser_parse_expression(parser, scope);
        function_call->funtion_call_arguments[0] = ast_expression;
        function_call->funtion_call_arguments_size++;

        while (parser->current_token->type == TOKEN_COMMA) {
            parser_eat(parser, TOKEN_COMMA);
            ast_t *ast_expression = parser_parse_expression(parser, scope);
            function_call->funtion_call_arguments_size++;
            function_call->funtion_call_arguments = realloc(
                function_call->funtion_call_arguments,
                function_call->funtion_call_arguments_size * sizeof(struct AST_STRUCT*)
            );
            function_call->funtion_call_arguments[function_call->funtion_call_arguments_size - 1] = ast_expression;
        }
    }
    parser_eat(parser, TOKEN_RPAREN);
    function_call->scope = scope;
    return function_call;
}



ast_t *parser_parse_variable(parser_t *parser, scope_t *scope){

    if(parser == NULL)
    {
        fprintf(stderr, "Error: NULL Parser encountered\n");
        exit(EXIT_FAILURE);
    }

    char *token_value = parser->current_token->value;
    parser_eat(parser, TOKEN_ID); // variable name of function call

    if(parser->current_token->type == TOKEN_LPAREN)
        return parser_parse_function_call(parser, scope);

    ast_t *ast_variable = init_ast(AST_VARIABLE);
    ast_variable->variable_name = token_value;

    ast_variable->scope = scope;

    return ast_variable;
}

ast_t *parser_parse_variable_definition(parser_t *parser, scope_t *scope){
    parser_eat(parser, TOKEN_ID); // ripare, detsinfe
    char *var_definition_var_name  = parser->current_token->value;

    parser_eat(parser, TOKEN_ID); // Variable name
    parser_eat(parser, TOKEN_EQUALS); 
    

    ast_t *var_definition_value = parser_parse_expression(parser, scope);
    
    
    ast_t *var_definition = init_ast(AST_VARIABLE_DEFINITION);

    var_definition->variable_definition_variable_name = var_definition_var_name;
    var_definition->variable_definition_value = var_definition_value;

    var_definition->scope=scope;
    return var_definition;
    
}

ast_t *parser_parse_function_definition(parser_t *parser, scope_t *scope)
{
    ast_t *ast = init_ast(AST_FUNCTION_DEFINITION);
    parser_eat(parser, TOKEN_FUNC); // function keyword

    char *function_name = parser->current_token->value;
    ast->funtion_definition_name = calloc(strlen(function_name)+1, sizeof(char));
    strcpy(ast->funtion_definition_name, function_name);

    parser_eat(parser, TOKEN_ID); //function name
    parser_eat(parser, TOKEN_LPAREN);

    //Function arguments
    ast->function_definition_args = NULL;
    ast->function_definition_args_size = 0;

    if (parser->current_token->type != TOKEN_RPAREN) {
        ast->function_definition_args = calloc(1, sizeof(struct AST_STRUCT*));
        ast_t *function_args = parser_parse_variable(parser, scope);
        ast->function_definition_args_size++;
        ast->function_definition_args[ast->function_definition_args_size - 1] = function_args;

        while (parser->current_token->type == TOKEN_COMMA) {
            parser_eat(parser, TOKEN_COMMA);
            ast->function_definition_args_size++;
            ast->function_definition_args = realloc(
                ast->function_definition_args,
                sizeof(struct AST_STRUCT*) * ast->function_definition_args_size
            );
            ast_t *arg = parser_parse_variable(parser, scope);
            ast->function_definition_args[ast->function_definition_args_size - 1] = arg;
        }
    }  
    //End
    parser_eat(parser, TOKEN_RPAREN);
    parser_eat(parser, TOKEN_LBRACE);

    // Function definition
    ast -> function_definition_body = parser_parse_statements(parser, scope);

    parser_eat(parser, TOKEN_RBRACE);   

    ast->scope = scope;
     
    return ast; 
}

ast_t *parser_parse_string(parser_t *parser, scope_t *scope){
    ast_t *ast_string = init_ast(AST_STRING);
    ast_string->string_value = parser->current_token->value;
    parser_eat(parser, TOKEN_STRING);

    ast_string->scope = scope;
    return ast_string;
}


ast_t *parser_parse_id(parser_t *parser, scope_t *scope)
{
    if(strcmp(parser->current_token->value, "ripare")==0)  /* Declarer */
    {
        return parser_parse_variable_definition(parser, scope);
    }
    else if(strcmp(parser->current_token->value, "detsinfe")==0)  /* Variable */
    {
        return parser_parse_variable_definition(parser, scope);
    }
    else if(parser->current_token->type == TOKEN_ID)
    {
        token_t *next = lexer_peek_next_token(parser->lexer);
        if (next->type == TOKEN_EQUALS) {
            return parser_parse_assignment(parser, scope);
        }
        return parser_parse_variable(parser, scope);
    }
    else{
        return parser_parse_variable(parser, scope);
    }
}

ast_t *parser_parse_assignment(parser_t *parser, scope_t *scope){
    char *name = parser->current_token->value;
    parser_eat(parser, TOKEN_ID);
    parser_eat(parser, TOKEN_EQUALS);

    ast_t *value = parser_parse_expression(parser, scope);

    ast_t *assign = init_ast(AST_ASSIGNMENT); 
    assign->variable_definition_variable_name = name;
    assign->variable_definition_value = value;
    assign->scope = scope;
    return assign;
}

ast_t *parser_parse_if_cond(parser_t *parser, scope_t *scope){

    ast_t *cond = init_ast(AST_IF);
    parser_eat(parser, TOKEN_IF);
    parser_eat(parser, TOKEN_LPAREN);
    cond->if_condition = parser_parse_expression(parser, scope); /* ast->if_condition */
    parser_eat(parser, TOKEN_RPAREN);
    parser_eat(parser, TOKEN_LBRACE);
    cond->if_body = parser_parse_statements(parser, scope); /* ast->if_then */
    parser_eat(parser, TOKEN_RBRACE);
    if (parser->current_token->type == TOKEN_ELSE) {

        parser_eat(parser, TOKEN_ELSE);
        if (parser->current_token->type == TOKEN_IF) {
            cond->if_else = parser_parse_if_cond(parser, scope); /* ast->if_else_if */
            return cond;  
        }

        parser_eat(parser, TOKEN_LBRACE);
        cond->if_else = parser_parse_statements(parser, scope); /* ast->if_else */
        parser_eat(parser, TOKEN_RBRACE);
    }
    return cond;
}

ast_t *parser_parse_while_loop(parser_t *parser, scope_t *scope){
    
    ast_t *loop = init_ast(AST_WHILE);
    parser_eat(parser, TOKEN_WHILE);
    parser_eat(parser, TOKEN_LPAREN);
    loop->loop_condition = parser_parse_expression(parser, scope); /* ast->while_condition */
    parser_eat(parser, TOKEN_RPAREN);
    parser_eat(parser, TOKEN_LBRACE);
    loop->loop_body = parser_parse_statements(parser, scope); /* ast->if_then */
    parser_eat(parser, TOKEN_RBRACE);
    return loop;
}

ast_t *parser_parse_function_return(parser_t *parser, scope_t *scope){
    ast_t *ret = init_ast(AST_RETURN); 
    parser_eat(parser, TOKEN_RETURN);
    ret->return_value = parser_parse_expression(parser, scope);
    ret->scope = scope;
    return ret;
}

ast_t *parser_parse_for_loop(parser_t *parser, scope_t *scope){
    ast_t *loop = init_ast(AST_FOR);
    parser_eat(parser, TOKEN_FOR);
    parser_eat(parser, TOKEN_LPAREN);

    ast_t *params = parser_parse_for_loop_parameters(parser, scope);
    loop->for_init = params->for_init;
    loop->for_condition = params->for_condition;
    loop->for_update = params->for_update;

    parser_eat(parser, TOKEN_RPAREN);
    parser_eat(parser, TOKEN_LBRACE);
    loop->loop_body = parser_parse_statements(parser, scope);
    parser_eat(parser, TOKEN_RBRACE);
    return loop;
}


ast_t *parser_parse_for_loop_parameters(parser_t *parser, scope_t *scope){
    ast_t *params = init_ast(AST_NOOP); 

    ast_t *init_stmt = parser_parse_statement(parser, scope); 
    parser_eat(parser, TOKEN_SEMI);

    ast_t *cond_expr = parser_parse_expression(parser, scope); 
    parser_eat(parser, TOKEN_SEMI);

    ast_t *update_stmt = parser_parse_statement(parser, scope); 

    params->for_init = init_stmt;
    params->for_condition = cond_expr;
    params->for_update = update_stmt;

    return params;
}



