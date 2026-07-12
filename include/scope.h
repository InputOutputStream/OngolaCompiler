#ifndef SCOPE_H
#define SCOPE_H
#include "ast.h"

typedef struct SCOPE_STRUCT
{
    ast_t **function_definition;
    size_t function_definition_size;

    ast_t ** variable_definitions;
    size_t variable_definitions_size;
}scope_t;

scope_t *init_scope();
ast_t *scope_add_function_definition(scope_t *scope, ast_t *function_def);
ast_t *scope_get_function_definition(scope_t *scope, const char *function_name);

ast_t *scope_add_variable_definition(scope_t *scope, ast_t *variable_def);
ast_t *scope_get_variable_definition(scope_t *scope, const char *variable_name);

#endif // !SCOPE_H