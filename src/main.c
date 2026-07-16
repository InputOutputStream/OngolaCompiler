#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "lexer.h"
#include "parser.h"
#include "codegen.h"
#include "io.h"

static char *stem_of(const char *path)
{
    const char *base = strrchr(path, '/');
    base = base ? base + 1 : path;

    const char *dot = strrchr(base, '.');
    size_t len = dot ? (size_t)(dot - base) : strlen(base);

    char *stem = calloc(len + 1, sizeof(char));
    memcpy(stem, base, len);
    stem[len] = '\0';
    return stem;
}

static void compile_file(
    const char *path,
    const char *include_dir,
    const char *runtime_src,
    int show_ir)
{
    printf("\n========================================\n");
    printf("COMPILING: %s\n", path);
    printf("========================================\n");

    char *contents = get_file(path);
    lexer_t *lexer = init_lexer(contents);
    parser_t *parser = init_parser(lexer);

    ast_t *root = parser_parse(parser, parser->scope);

    char *stem = stem_of(path);

    char c_path[1024];
    if (show_ir) {
        snprintf(c_path, sizeof(c_path), "%s.c", stem);
    } else {
        snprintf(c_path, sizeof(c_path), "/tmp/.educ_ir_%s_%d.c", stem, (int)getpid());
    }

    FILE *out = fopen(c_path, "w");
    if (out == NULL) {
        fprintf(stderr, "Error: could not open %s for writing\n", c_path);
        exit(EXIT_FAILURE);
    }
    codegen_generate(root, out);
    fclose(out);

    if (show_ir) {
        printf("Generated C (intermediate representation): %s\n", c_path);
    }

    pid_t pid = fork();
    if (pid == 0) {
        execlp("cc", "cc",
            "-O2",
            "-I", include_dir,
            c_path,
            runtime_src,
            "-lm",
            "-o", stem,
            (char *)NULL);
        perror("execlp cc");
        _exit(EXIT_FAILURE);
    }
    int status;
    waitpid(pid, &status, 0);

    if (!show_ir) {
        remove(c_path);
    }

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        fprintf(stderr, "ERROR: C compilation failed for %s\n", c_path);
        exit(EXIT_FAILURE);
    }

    printf("Built executable: ./%s\n", stem);
    free(stem);
}

int main(int argc, char **argv)
{
    int show_ir = 0;
    const char *files[argc];
    int file_count = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0) {
            show_ir = 1;
        } else {
            files[file_count++] = argv[i];
        }
    }

    if (file_count == 0) {
        fprintf(stderr, "Error: No Input File!\n");
        fprintf(stderr, "Usage: ./educ [-i] <file1.edu> <file2.edu> ...\n");
        fprintf(stderr, "  -i   keep the generated C (the intermediate representation) as <name>.c\n");
        exit(EXIT_FAILURE);
    }

    const char *include_dir = EDU_RUNTIME_INCLUDE_DIR;
    const char *runtime_src = EDU_RUNTIME_SRC;

    for (int i = 0; i < file_count; i++) {
        compile_file(files[i], include_dir, runtime_src, show_ir);
    }

    printf("\n========================================\n");
    printf("DONE: %d file(s) compiled\n", file_count);
    printf("========================================\n");

    return 0;
}
