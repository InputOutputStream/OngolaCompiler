#include "scope.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


scope_t *init_scope(scope_t *parent)
{
    scope_t *scope = calloc(1, sizeof(struct SCOPE_STRUCT));
    scope->function_definition = (void *)0;
    scope->function_definition_size = 0;
    scope->variable_definitions = (void *)0;
    scope->variable_definitions_size = 0;
    scope->parent = parent;
    return scope;
}

ast_t *scope_get_variable_definition(scope_t *scope, const char *variable_name)
{
    for (scope_t *s = scope; s != NULL; s = s->parent) {
        for (size_t i = 0; i < s->variable_definitions_size; i++) {
            ast_t *variable_def = s->variable_definitions[i];
            if (strcmp(variable_def->variable_definition_variable_name, variable_name) == 0) {
                return variable_def;
            }
        }
    }
    return (void *) 0;
}
ast_t *scope_add_function_definition(scope_t *scope, ast_t *function_def)
{
    scope->function_definition_size ++;
    if(scope->function_definition == (void *)0)
    {
        scope->function_definition = calloc(1,  sizeof(struct AST_STRUCT*));
    }
    else
    {
        scope->function_definition = realloc(scope->function_definition, 
                                            scope->function_definition_size*sizeof(struct AST_STRUCT**)); 
    }

    scope->function_definition[scope->function_definition_size-1] = function_def;
    return function_def;
}


ast_t *scope_get_function_definition(scope_t *scope, const char *function_name)
{
    for (scope_t *s = scope; s != NULL; s = s->parent) {
        for (size_t i = 0; i < s->function_definition_size; i++) {
            ast_t *function_def = s->function_definition[i];
            if (strcmp(function_def->funtion_definition_name, function_name) == 0) {
                return function_def;
            }
        }
    }
    return (void *) 0;
}

ast_t *scope_add_variable_definition(scope_t *scope, ast_t *variable_def)
{
    if (variable_def == NULL)
    {
       perror("NULL AST variable_def ENCOUNTERED BY VARIABLE DEFINITION VISITOR");
       exit(EXIT_FAILURE);
    }

    if(scope->variable_definitions == (void *)0)
    {
        scope->variable_definitions = calloc(1, sizeof(struct AST_STRUC*));
        scope->variable_definitions[0] = variable_def;
        scope->variable_definitions_size++;
    }   
    else
    {
        scope->variable_definitions_size++;
        scope->variable_definitions = realloc(scope->variable_definitions, 
            scope->variable_definitions_size * sizeof(struct AST_STRUCT*));
        if(!scope->variable_definitions)
        {
            fprintf(stderr, "Realloc failed on viditor variable definitions\n");
            for (size_t i = 0; i < scope->variable_definitions_size; i++)
            {
                free(scope->variable_definitions[i]);
            }
            free(scope);
            exit(EXIT_FAILURE);
        }
        scope->variable_definitions[scope->variable_definitions_size-1] = variable_def;
    }
    
    return variable_def;
}


