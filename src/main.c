#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "parser.h"
#include "visitor.h"
#include "io.h"



static void run_file(const char *path){
    printf("\n========================================\n");
    printf("RUNNING: %s\n", path);
    printf("========================================\n");

    char *contents = get_file(path); 
    lexer_t *lexer = init_lexer(contents);
    parser_t *parser = init_parser(lexer);

    ast_t *root = parser_parse(parser, parser->scope);
    visitor_t *visitor = init_visitor();

    visitor_visit(visitor, root);
}

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        fprintf(stderr, "Error: No Input File!\n");
        fprintf(stderr, "Usage: ./Ocomp <file1.edu> <file2.edu> ...\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < argc; i++) {
        run_file(argv[i]);
    }

    printf("\n========================================\n");
    printf("DONE: %d file(s) run\n", argc - 1);
    printf("========================================\n");

    return 0;
}