#include <stdlib.h>
#include <stdio.h>


#include "ast.h"
#include "lexer.h"


ast_t *init_ast(int type){
    ast_t* ast = calloc(1, sizeof(struct AST_STRUCT));
    if(ast == (void *) 0)
    {
        fprintf(stderr, "Error: AST creation failed\n");
        exit(EXIT_FAILURE);
    }
    ast->type = type;

    /**Scope */ 
    ast->scope = (void *) 0;

    /**Ast Variable definition */
    ast->variable_definition_variable_name = (void *) 0;
    ast->variable_definition_value = (void *) 0;


    /** Function definition */
    ast->funtion_definition_name = (void *) 0;
    ast->function_definition_body = (void *) 0;
    ast->function_definition_args = (void *) 0;
    ast->function_definition_args_size = 0;
    
    /**Ast variable */ 
    ast->variable_name = (void *) 0;

    /**Funtion Call */
    ast->funtion_call_name = (void *) 0;
    ast->funtion_call_arguments_size = 0;
    ast->funtion_call_arguments=(void *) 0;

    /**Ast String */
    ast->string_value = (void *) 0;

    /**Ast Ops */
    
    /**AST Compound*/
    ast->compound_value = (void *) 0;
    ast->compound_size = 0;
    return ast;
}