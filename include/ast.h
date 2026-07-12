#ifndef AST_H
#define AST_H

#include <stdlib.h>

typedef struct AST_STRUCT
{
    enum{
        AST_FUNCTION_CALL,
        AST_FUNCTION_DEFINITION,
        AST_VARIABLE_DEFINITION,
        AST_ASSIGNMENT,
        AST_VARIABLE,     
        AST_STRING,
        AST_COMPOUND, // List of multiple statements[nodes]
        AST_NOOP,
        AST_IF, 
        AST_WHILE, 
        AST_FOR,
        AST_BINOP,
        AST_UNOP,
        AST_NUMBER,
        AST_RETURN
    }type;

    /**Scope  */
    struct SCOPE_STRUCT *scope;

    /**Ast Variable definition */
    char *variable_definition_variable_name;
    struct AST_STRUCT* variable_definition_value;

    /**AST function definition */
    char *funtion_definition_name;
    struct AST_STRUCT *function_definition_body;
    struct AST_STRUCT **function_definition_args;
    size_t function_definition_args_size;
    
    /**Ast variable */
    char *variable_name;

    /**Funtion Call */
    char *funtion_call_name;
    struct  AST_STRUCT** funtion_call_arguments;
    size_t funtion_call_arguments_size;

    /**Condition  */
    struct  AST_STRUCT* if_condition;   
    struct  AST_STRUCT* if_body; 
    struct  AST_STRUCT* if_else;
    size_t if_block_size;

    /**loop  */
    struct  AST_STRUCT* loop_condition;   
    struct  AST_STRUCT* loop_body; 
    size_t loop_size;

    /* For loop */
    struct AST_STRUCT *for_init;
    struct AST_STRUCT *for_condition;
    struct AST_STRUCT *for_update;

    /**Ast String */
    char *string_value;

    /**AST Compound*/
    struct AST_STRUCT **compound_value;
    size_t compound_size;
    
    /**Ast Bin Op */
    char *op_name;  
    struct AST_STRUCT *left_value;
    struct AST_STRUCT *right_value;
    char *operator_str;  

    /* AST_NUMBER */
    double number_value;

    /* Return Value*/
    struct AST_STRUCT *return_value;


}ast_t;


ast_t *init_ast(int type);

#endif
