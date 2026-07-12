#include <stdio.h>

#include <string.h>
#include <stdlib.h>

#include "io.h"

static char *_get_file_extension(const char* path)
{
    if(path == NULL)
    {
        return NULL;
    }

    char* last_dot = strrchr(path, '.');

    if(last_dot == NULL || last_dot == path)
    {
        return NULL;
    }

    if( *(last_dot + 1) == '\0')
    {
        return NULL;
    }

    return last_dot + 1;
}

static char* get_extension(const char *path)
{
    char *ext = _get_file_extension(path);
    if(ext == NULL)
        return NULL;
    
    return strdup(ext);
}

char *get_file(const char *file_path)
{
    char *buffer = 0;
    long length;

    char *extension = get_extension(file_path);

    if(extension == NULL)
    {
        fprintf(stderr, "ERROR: Could Not Open File\n");
        exit(EXIT_FAILURE);
    }

    if(strcmp(extension, "edu") != 0)
    {
        fprintf(stderr, "ERROR: Unkown File Type, The Correct Type is \".edu\"\n");
        exit(EXIT_FAILURE);
    }

    FILE *f = fopen(file_path, "rb");
    if(f==NULL)
    {
        fprintf(stderr, "Error reading %s\n", file_path);
        exit(EXIT_FAILURE);
    }

    fseek(f, 0, SEEK_END);
    length = ftell(f);
    fseek(f, 0, SEEK_SET);

    buffer = calloc(1, length + 1);
    
    if(buffer != NULL)
        fread(buffer, 1, length, f); 

    fclose(f);
    return buffer;
}