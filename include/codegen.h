#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>

#include "ast.h"


void codegen_generate(ast_t *root, FILE *out);

#endif /* !CODEGEN_H */
