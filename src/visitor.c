#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "visitor.h"
#include "scope.h"

static ast_t* _builtin_print(visitor_t *visitor, struct AST_STRUCT** args, size_t args_size)
{
    for (size_t i = 0; i < args_size; i++)
    {
        ast_t *visited_ast = visitor_visit(visitor,  args[i]);
        switch (visited_ast->type)
        {
            case AST_STRING:
                fprintf(stderr, "%s\n", visited_ast->string_value);
                break;
            case AST_NUMBER:
                printf("%g\n", visited_ast->number_value);
                break;  
            default:
                printf("<unprintable value, type %d>\n", visited_ast->type);
                break;
        } 

    }

    return init_ast(AST_NOOP);
}

visitor_t *init_visitor(void)
{
    visitor_t *visitor = calloc(1, sizeof(struct VISITOR_STRUCT));

    visitor->is_returning = 0;
    visitor->return_value = NULL;
    return visitor;
}



ast_t *visitor_visit(visitor_t *visitor, ast_t *node){
    switch (node->type)
    {
        case AST_FUNCTION_DEFINITION:
            return visitor_visit_function_definition(visitor, node);
        case AST_VARIABLE_DEFINITION:
            return visitor_visit_variable_definition(visitor, node);
        case AST_VARIABLE:
            return visitor_visit_variable(visitor, node);
        case AST_FUNCTION_CALL:
            return visitor_visit_function_call(visitor, node);
        case AST_STRING:
            return visitor_visit_string(visitor, node);
        case AST_COMPOUND:
            return visitor_visit_compound(visitor, node);
        case AST_NOOP:
            return node;

        case AST_NUMBER:
            return node; 

        case AST_BINOP:
            return visitor_visit_binop(visitor, node);

        case AST_ASSIGNMENT:
            return visitor_visit_assignment(visitor, node);

        case AST_IF:
            return visitor_visit_if(visitor, node);

        case AST_WHILE:
            return visitor_visit_while(visitor, node);

        case AST_FOR:
            return visitor_visit_for(visitor, node);

        case AST_RETURN:
            return visitor_visit_return(visitor, node);
    }

    printf("Uncaught statement of type %d\n", node->type);
    exit(EXIT_FAILURE);
    return init_ast(AST_NOOP);
}

ast_t *visitor_visit_binop(visitor_t *visitor, ast_t *node){
    ast_t *left = visitor_visit(visitor, node->left_value);
    ast_t *right = visitor_visit(visitor, node->right_value);

    if (left->type != AST_NUMBER || right->type != AST_NUMBER) {
        fprintf(stderr, "Binop operands must evaluate to numbers\n");
        exit(EXIT_FAILURE);
    }

    double l = left->number_value;
    double r = right->number_value;
    double result = 0;

    if (node->operator_str) {
        if (strcmp(node->operator_str, "<") == 0)  result = (l < r);
        else if (strcmp(node->operator_str, ">") == 0)  result = (l > r);
        else if (strcmp(node->operator_str, "==") == 0) result = (l == r);
        else if (strcmp(node->operator_str, "!=") == 0) result = (l != r);
        else if (strcmp(node->operator_str, "<=") == 0) result = (l <= r);
        else if (strcmp(node->operator_str, ">=") == 0) result = (l >= r);
        else if (strcmp(node->operator_str, "+") == 0)  result = l + r;
        else if (strcmp(node->operator_str, "-") == 0)  result = l - r;
        else if (strcmp(node->operator_str, "*") == 0)  result = l * r;
        else if (strcmp(node->operator_str, "/") == 0)  result = l / r;
    }

    ast_t *num = init_ast(AST_NUMBER);
    num->number_value = result;
    return num;
}

ast_t *visitor_visit_assignment(visitor_t *visitor, ast_t *node){
    ast_t *existing = scope_get_variable_definition(node->scope, node->variable_definition_variable_name);
    if (existing == NULL) {
        fprintf(stderr, "Assignment to undeclared variable `%s`\n", node->variable_definition_variable_name);
        exit(EXIT_FAILURE);
    }
    ast_t *new_value = visitor_visit(visitor, node->variable_definition_value);
    existing->variable_definition_value = new_value;
    return new_value;
}


ast_t *visitor_visit_for(visitor_t *visitor, ast_t *node){
    visitor_visit(visitor, node->for_init);

    while (1) {
        ast_t *cond_result = visitor_visit(visitor, node->for_condition);
        if (cond_result->type != AST_NUMBER) {
            fprintf(stderr, "For condition must evaluate to a number\n");
            exit(EXIT_FAILURE);
        }
        if (cond_result->number_value == 0) break;
        
        visitor_visit(visitor, node->loop_body);
        if (visitor->is_returning) break;  

        visitor_visit(visitor, node->for_update);
    }
    return init_ast(AST_NOOP);
}

ast_t *visitor_visit_while(visitor_t *visitor, ast_t *node){
    while (1) {
        ast_t *cond_result = visitor_visit(visitor, node->loop_condition);
        if (cond_result->type != AST_NUMBER) {
            fprintf(stderr, "While condition must evaluate to a number\n");
            exit(EXIT_FAILURE);
        }
        if (cond_result->number_value == 0) break;

        visitor_visit(visitor, node->loop_body);        
        if (visitor->is_returning) break;  
    }
    return init_ast(AST_NOOP);
}

ast_t *visitor_visit_if(visitor_t *visitor, ast_t *node){
    ast_t *cond_result = visitor_visit(visitor, node->if_condition);
    if (cond_result->type != AST_NUMBER) {
        fprintf(stderr, "If condition must evaluate to a number\n");
        exit(EXIT_FAILURE);
    }

    if (cond_result->number_value != 0) {
        return visitor_visit(visitor, node->if_body);
    } else if (node->if_else != NULL) {
        return visitor_visit(visitor, node->if_else);
    }

    return init_ast(AST_NOOP);
}

ast_t *visitor_visit_function_call(visitor_t *visitor, ast_t *node){
    if (node == NULL)
    {
       perror("NULL AST NODE ENCOUNTERED FUNCTION CALL VSISITOR");
       exit(EXIT_FAILURE );
    }

    if(strcmp(node->funtion_call_name, "print")==0)
    {
        return _builtin_print(visitor, node->funtion_call_arguments, node->funtion_call_arguments_size);
    }

    ast_t *function_def = scope_get_function_definition(node->scope, node->funtion_call_name);

    if(function_def == (void *)0){
        fprintf(stderr, "Undefined method: %s\n", node->funtion_call_name);
        exit(1);
    }

    for(size_t i = 0; i<function_def->function_definition_args_size; i++)
    {
        ast_t *function_def_arg = function_def->function_definition_args[i];
        ast_t *ast_function_call_args_value = node->funtion_call_arguments[i];

        //Create variable definition
        ast_t *variable_def = init_ast(AST_VARIABLE_DEFINITION);

        //Copy to variable definition the corresponding function definition parameter name
        variable_def->variable_definition_variable_name = calloc(strlen(function_def_arg->variable_name)+1, sizeof(char));
        strcpy(variable_def->variable_definition_variable_name, function_def_arg->variable_name);

        //Assign to function definition variable the function call value given
        variable_def->variable_definition_value = ast_function_call_args_value;

        //Add the created variable to function scope
        scope_add_variable_definition(function_def->function_definition_body->scope,
                                        variable_def);
    }

    int saved_is_returning = visitor->is_returning;
    visitor->is_returning = 0;                       

    ast_t *result = visitor_visit(visitor, function_def->function_definition_body);

    ast_t *return_val = visitor->is_returning ? visitor->return_value : result;

    visitor->is_returning = saved_is_returning;    
    return return_val;
}

ast_t *visitor_visit_function_definition(visitor_t __attribute_maybe_unused__ *visitor, ast_t* node)
{
    scope_add_function_definition(node->scope, node);
    return node;
}


ast_t *visitor_visit_variable_definition(visitor_t __attribute_maybe_unused__ *visitor, ast_t *node){
    scope_add_variable_definition(node->scope, node);
    return node;
}


ast_t *visitor_visit_string(visitor_t __attribute_maybe_unused__ *visitor, ast_t *node){
    if (node == NULL)
    {
       perror("NULL AST NODE ENCOUNTERED BY STRING");
       exit(EXIT_FAILURE);
    }

    return node;
}

ast_t *visitor_visit_compound(visitor_t *visitor, ast_t *node){
    for(size_t i=0; i<node->compound_size; i++)
    {
        visitor_visit(visitor, node->compound_value[i]);
        if (visitor->is_returning) {
            return visitor->return_value; 
        }
    }

    return init_ast(AST_NOOP);     
}

ast_t *visitor_visit_variable(visitor_t *visitor, ast_t *node){
    if (node == NULL)
    {
       perror("NULL AST NODE ENCOUNTERED BY VARIABLE VISITOR");
       exit(EXIT_FAILURE);
    }
    if (visitor == NULL)
    {
       perror("NULL AST VISITOR ENCOUNTERED");
       exit(EXIT_FAILURE);
    }

   ast_t *variable_definition = scope_get_variable_definition(node->scope, node->variable_name);


    if(variable_definition == (void *)0)
    {
        fprintf(stderr,"Undefinied variable `%s`\n", node->variable_name);
        exit(EXIT_FAILURE);
    }

    return visitor_visit(visitor, variable_definition->variable_definition_value);

}

ast_t *visitor_visit_return(visitor_t *visitor, ast_t *node){
    ast_t *val = visitor_visit(visitor, node->return_value);
    visitor->is_returning = 1;
    visitor->return_value = val;
    return val;
}

ast_t *visitor_visit_unop(visitor_t *visitor, ast_t *node){
    ast_t *operand = visitor_visit(visitor, node->left_value);
    ast_t *result = init_ast(AST_NUMBER);
    result->number_value = -operand->number_value;
    return result;
}