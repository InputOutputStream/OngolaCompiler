#ifndef VISITOR_H
#define VISITOR_H

#include "ast.h"

typedef struct VISITOR_STRUCT
{
    int is_returning;        // set to 1 when a return statement
    ast_t *return_value;     // the value being propagated up
} visitor_t;

visitor_t *init_visitor(void);

ast_t *visitor_visit(visitor_t *visitor, ast_t *node);
ast_t *visitor_visit_function_call(visitor_t *visitor, ast_t *node);
ast_t *visitor_visit_function_definition(visitor_t *visitor, ast_t* node);
ast_t *visitor_visit_variable_definition(visitor_t *visitor, ast_t *node);
ast_t *visitor_visit_string(visitor_t *visitor, ast_t *node);
ast_t *visitor_visit_compound(visitor_t *visitor, ast_t *node);
ast_t *visitor_visit_variable(visitor_t *visitor, ast_t *node);
ast_t *visitor_visit_unop(visitor_t *visitor, ast_t *node);
ast_t *visitor_visit_binop(visitor_t *visitor, ast_t *node);
ast_t *visitor_visit_assignment(visitor_t *visitor, ast_t *node);
ast_t *visitor_visit_if(visitor_t *visitor, ast_t *node);
ast_t *visitor_visit_while(visitor_t *visitor, ast_t *node);
ast_t *visitor_visit_for(visitor_t *visitor, ast_t *node);
ast_t *visitor_visit_return(visitor_t *visitor, ast_t *node);
ast_t *visitor_visit_return(visitor_t *visitor, ast_t *node);

#endif // !VISITOR_H