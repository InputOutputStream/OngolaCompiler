#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "codegen.h"


typedef struct {
    char **names;
    size_t size;
    size_t cap;
} name_set_t;

static void name_set_init(name_set_t *s)
{
    s->names = NULL;
    s->size = 0;
    s->cap = 0;
}

static int name_set_contains(name_set_t *s, const char *name)
{
    for (size_t i = 0; i < s->size; i++) {
        if (strcmp(s->names[i], name) == 0) return 1;
    }
    return 0;
}

static void name_set_add(name_set_t *s, const char *name)
{
    if (name_set_contains(s, name)) return;

    if (s->size == s->cap) {
        s->cap = s->cap ? s->cap * 2 : 8;
        s->names = realloc(s->names, s->cap * sizeof(char *));
    }
    s->names[s->size++] = strdup(name);
}

static void name_set_free(name_set_t *s)
{
    for (size_t i = 0; i < s->size; i++) free(s->names[i]);
    free(s->names);
}



typedef struct {
    ast_t **items;
    size_t size;
    size_t cap;
} func_list_t;

static void func_list_init(func_list_t *l)
{
    l->items = NULL;
    l->size = 0;
    l->cap = 0;
}

static void func_list_add(func_list_t *l, ast_t *fn)
{
    if (l->size == l->cap) {
        l->cap = l->cap ? l->cap * 2 : 8;
        l->items = realloc(l->items, l->cap * sizeof(ast_t *));
    }
    l->items[l->size++] = fn;
}

static void func_list_free(func_list_t *l)
{
    free(l->items);
}



static void collect_function_defs(ast_t *node, func_list_t *out)
{
    if (node == NULL) return;

    switch (node->type) {
        case AST_FUNCTION_DEFINITION:
            func_list_add(out, node);
            collect_function_defs(node->function_definition_body, out);
            break;

        case AST_COMPOUND:
            for (size_t i = 0; i < node->compound_size; i++)
                collect_function_defs(node->compound_value[i], out);
            break;

        case AST_IF:
            collect_function_defs(node->if_body, out);
            collect_function_defs(node->if_else, out);
            break;

        case AST_WHILE:
            collect_function_defs(node->loop_body, out);
            break;

        case AST_FOR:
            collect_function_defs(node->for_init, out);
            collect_function_defs(node->loop_body, out);
            collect_function_defs(node->for_update, out);
            break;

        default:
            break; /* expressions can't contain function definitions */
    }
}

/* ---- identifier mangling ------------------------------------------- */

static void emit_var_name(FILE *out, const char *name)
{
    fprintf(out, "ev_%s", name);
}

static void emit_func_name(FILE *out, const char *name)
{
    fprintf(out, "ef_%s", name);
}

/* ---- string literal escaping ---------------------------------------- */

static void emit_c_string_literal(FILE *out, const char *s)
{
    fputc('"', out);
    for (const char *p = s; *p; p++) {
        switch (*p) {
            case '"':  fputs("\\\"", out); break;
            case '\\': fputs("\\\\", out); break;
            case '\n': fputs("\\n", out);  break;
            case '\t': fputs("\\t", out);  break;
            case '\r': fputs("\\r", out);  break;
            default:   fputc(*p, out);     break;
        }
    }
    fputc('"', out);
}

/* ---- indentation ------------------------------------------------------ */

static void indent(FILE *out, int level)
{
    for (int i = 0; i < level; i++) fputs("    ", out);
}

/* ---- forward decls ----------------------------------------------------- */

static void emit_expr(FILE *out, ast_t *node);
static void emit_stmt(FILE *out, ast_t *node, int level, name_set_t *declared);

/* ---- variable hoisting pre-pass ---------------------------------------- */

static void collect_var_names(ast_t *node, name_set_t *set)
{
    if (node == NULL) return;

    switch (node->type) {
        case AST_VARIABLE_DEFINITION:
            name_set_add(set, node->variable_definition_variable_name);
            collect_var_names(node->variable_definition_value, set);
            break;

        case AST_COMPOUND:
            for (size_t i = 0; i < node->compound_size; i++)
                collect_var_names(node->compound_value[i], set);
            break;

        case AST_IF:
            collect_var_names(node->if_condition, set);
            collect_var_names(node->if_body, set);
            collect_var_names(node->if_else, set);
            break;

        case AST_WHILE:
            collect_var_names(node->loop_condition, set);
            collect_var_names(node->loop_body, set);
            break;

        case AST_FOR:
            collect_var_names(node->for_init, set);
            collect_var_names(node->for_condition, set);
            collect_var_names(node->for_update, set);
            collect_var_names(node->loop_body, set);
            break;

        case AST_ASSIGNMENT:
            collect_var_names(node->variable_definition_value, set);
            break;

        case AST_RETURN:
            collect_var_names(node->return_value, set);
            break;

        case AST_BINOP:
            collect_var_names(node->left_value, set);
            collect_var_names(node->right_value, set);
            break;

        case AST_UNOP:
            collect_var_names(node->left_value, set);
            break;

        case AST_FUNCTION_CALL:
            for (size_t i = 0; i < node->funtion_call_arguments_size; i++)
                collect_var_names(node->funtion_call_arguments[i], set);
            break;

        case AST_FUNCTION_DEFINITION:
        case AST_VARIABLE:
        case AST_STRING:
        case AST_NUMBER:
        case AST_NOOP:
        default:
            break;
    }
}

/* ---- expressions --------------------------------------------------- */

static void emit_expr(FILE *out, ast_t *node)
{
    if (node == NULL) {
        fprintf(out, "edu_nil()");
        return;
    }

    switch (node->type) {
        case AST_NUMBER:
            fprintf(out, "edu_num(%.17g)", node->number_value);
            break;

        case AST_STRING:
            fprintf(out, "edu_str(");
            emit_c_string_literal(out, node->string_value);
            fprintf(out, ")");
            break;

        case AST_VARIABLE:
            emit_var_name(out, node->variable_name);
            break;

        case AST_UNOP:
            fprintf(out, "edu_neg(");
            emit_expr(out, node->left_value);
            fprintf(out, ")");
            break;

        case AST_BINOP: {
            const char *fn = "edu_add";
            const char *op = node->operator_str ? node->operator_str : "+";
            if      (strcmp(op, "+") == 0)  fn = "edu_add";
            else if (strcmp(op, "-") == 0)  fn = "edu_sub";
            else if (strcmp(op, "*") == 0)  fn = "edu_mul";
            else if (strcmp(op, "/") == 0)  fn = "edu_div";
            else if (strcmp(op, "<") == 0)  fn = "edu_lt";
            else if (strcmp(op, ">") == 0)  fn = "edu_gt";
            else if (strcmp(op, "<=") == 0) fn = "edu_lteq";
            else if (strcmp(op, ">=") == 0) fn = "edu_gteq";
            else if (strcmp(op, "==") == 0) fn = "edu_eq";
            else if (strcmp(op, "!=") == 0) fn = "edu_neq";

            fprintf(out, "%s(", fn);
            emit_expr(out, node->left_value);
            fprintf(out, ", ");
            emit_expr(out, node->right_value);
            fprintf(out, ")");
            break;
        }

        case AST_FUNCTION_CALL:
            if (strcmp(node->funtion_call_name, "ribate") == 0) {
                /* print() returns nothing meaningful in the interpreter
                 * either (AST_NOOP); as an expression, yield nil. */
                fprintf(out, "(");
                for (size_t i = 0; i < node->funtion_call_arguments_size; i++) {
                    fprintf(out, "edu_print(");
                    emit_expr(out, node->funtion_call_arguments[i]);
                    fprintf(out, "), ");
                }
                fprintf(out, "edu_nil())");
            } else if (strcmp(node->funtion_call_name, "riba") == 0) {
                if (node->funtion_call_arguments_size != 0) {
                    fprintf(stderr, "codegen: `riba` (read) takes no arguments\n");
                    exit(EXIT_FAILURE);
                }
                fprintf(out, "edu_read()");
            } else if (strcmp(node->funtion_call_name, "ricane") == 0) {
                fprintf(out, "(");
                for (size_t i = 0; i < node->funtion_call_arguments_size; i++) {
                    fprintf(out, "edu_write(");
                    emit_expr(out, node->funtion_call_arguments[i]);
                    fprintf(out, "), ");
                }
                fprintf(out, "edu_nil())");
            } else {
                emit_func_name(out, node->funtion_call_name);
                fprintf(out, "(");
                for (size_t i = 0; i < node->funtion_call_arguments_size; i++) {
                    if (i > 0) fprintf(out, ", ");
                    emit_expr(out, node->funtion_call_arguments[i]);
                }
                fprintf(out, ")");
            }
            break;

        case AST_NOOP:
            fprintf(out, "edu_nil()");
            break;

        default:
            fprintf(stderr, "codegen: unsupported node in expression context (type %d)\n", node->type);
            exit(EXIT_FAILURE);
    }
}

/* ---- statements ------------------------------------------------------ */

static void emit_stmt(FILE *out, ast_t *node, int level, name_set_t *declared)
{
    if (node == NULL) return;

    switch (node->type) {
        case AST_COMPOUND:
            for (size_t i = 0; i < node->compound_size; i++)
                emit_stmt(out, node->compound_value[i], level, declared);
            break;

        case AST_VARIABLE_DEFINITION:
        case AST_ASSIGNMENT:
            indent(out, level);
            emit_var_name(out, node->variable_definition_variable_name);
            fprintf(out, " = ");
            emit_expr(out, node->variable_definition_value);
            fprintf(out, ";\n");
            break;

        case AST_FUNCTION_CALL:
            indent(out, level);
            emit_expr(out, node);
            fprintf(out, ";\n");
            break;

        case AST_IF:
            indent(out, level);
            fprintf(out, "if (edu_truthy(");
            emit_expr(out, node->if_condition);
            fprintf(out, ")) {\n");
            emit_stmt(out, node->if_body, level + 1, declared);
            indent(out, level);
            if (node->if_else != NULL) {
                fprintf(out, "} else {\n");
                emit_stmt(out, node->if_else, level + 1, declared);
                indent(out, level);
                fprintf(out, "}\n");
            } else {
                fprintf(out, "}\n");
            }
            break;

        case AST_WHILE:
            indent(out, level);
            fprintf(out, "while (edu_truthy(");
            emit_expr(out, node->loop_condition);
            fprintf(out, ")) {\n");
            emit_stmt(out, node->loop_body, level + 1, declared);
            indent(out, level);
            fprintf(out, "}\n");
            break;

        case AST_FOR:
            indent(out, level);
            fprintf(out, "{\n");
            emit_stmt(out, node->for_init, level + 1, declared);
            indent(out, level + 1);
            fprintf(out, "while (edu_truthy(");
            emit_expr(out, node->for_condition);
            fprintf(out, ")) {\n");
            emit_stmt(out, node->loop_body, level + 2, declared);
            emit_stmt(out, node->for_update, level + 2, declared);
            indent(out, level + 1);
            fprintf(out, "}\n");
            indent(out, level);
            fprintf(out, "}\n");
            break;

        case AST_RETURN:
            indent(out, level);
            fprintf(out, "return ");
            emit_expr(out, node->return_value);
            fprintf(out, ";\n");
            break;

        case AST_FUNCTION_DEFINITION:
            break;

        case AST_NOOP:
            break;

        default:
            fprintf(stderr, "codegen: unsupported node in statement context (type %d)\n", node->type);
            exit(EXIT_FAILURE);
    }
}

/* ---- function emission -------------------------------------------------- */

static void emit_function_prototype(FILE *out, ast_t *fn)
{
    fprintf(out, "Value ");
    emit_func_name(out, fn->funtion_definition_name);
    fprintf(out, "(");
    if (fn->function_definition_args_size == 0) {
        fprintf(out, "void");
    } else {
        for (size_t i = 0; i < fn->function_definition_args_size; i++) {
            if (i > 0) fprintf(out, ", ");
            fprintf(out, "Value ");
            emit_var_name(out, fn->function_definition_args[i]->variable_name);
        }
    }
    fprintf(out, ")");
}

static void emit_function_definition(FILE *out, ast_t *fn)
{
    emit_function_prototype(out, fn);
    fprintf(out, "\n{\n");

    name_set_t declared;
    name_set_init(&declared);
    for (size_t i = 0; i < fn->function_definition_args_size; i++)
        name_set_add(&declared, fn->function_definition_args[i]->variable_name);

    name_set_t hoisted;
    name_set_init(&hoisted);
    collect_var_names(fn->function_definition_body, &hoisted);

    for (size_t i = 0; i < hoisted.size; i++) {
        if (name_set_contains(&declared, hoisted.names[i])) continue;
        indent(out, 1);
        fprintf(out, "Value ");
        emit_var_name(out, hoisted.names[i]);
        fprintf(out, " = edu_nil();\n");
        name_set_add(&declared, hoisted.names[i]);
    }

    emit_stmt(out, fn->function_definition_body, 1, &declared);

    indent(out, 1);
    fprintf(out, "return edu_nil();\n");
    fprintf(out, "}\n\n");

    name_set_free(&hoisted);
    name_set_free(&declared);
}

/* ---- top-level driver ---------------------------------------------------- */

void codegen_generate(ast_t *root, FILE *out)
{
    if (root == NULL || root->type != AST_COMPOUND) {
        fprintf(stderr, "codegen: expected top-level compound AST node\n");
        exit(EXIT_FAILURE);
    }

    fprintf(out, "/* Generated by the edu compiler. Do not edit by hand. */\n");
    fprintf(out, "#include \"edu_runtime.h\"\n\n");

    func_list_t funcs;
    func_list_init(&funcs);
    collect_function_defs(root, &funcs);

    for (size_t i = 0; i < funcs.size; i++) {
        emit_function_prototype(out, funcs.items[i]);
        fprintf(out, ";\n");
    }
    fprintf(out, "\n");

    /* Pass 2: emit full definitions for each. */
    for (size_t i = 0; i < funcs.size; i++) {
        emit_function_definition(out, funcs.items[i]);
    }

    func_list_free(&funcs);

    /* Pass 3: everything else (top-level statements) becomes main(). */
    fprintf(out, "int main(void)\n{\n");

    name_set_t declared;
    name_set_init(&declared);
    for (size_t i = 0; i < root->compound_size; i++) {
        ast_t *node = root->compound_value[i];
        if (node->type != AST_FUNCTION_DEFINITION)
            collect_var_names(node, &declared);
    }
    for (size_t i = 0; i < declared.size; i++) {
        indent(out, 1);
        fprintf(out, "Value ");
        emit_var_name(out, declared.names[i]);
        fprintf(out, " = edu_nil();\n");
    }

    for (size_t i = 0; i < root->compound_size; i++) {
        ast_t *node = root->compound_value[i];
        if (node->type == AST_FUNCTION_DEFINITION) continue;
        if (node->type == AST_RETURN) {
            indent(out, 1);
            fprintf(out, "return 0; /* top-level return */\n");
            continue;
        }
        emit_stmt(out, node, 1, &declared);
    }

    indent(out, 1);
    fprintf(out, "return 0;\n");
    fprintf(out, "}\n");

    name_set_free(&declared);
}
