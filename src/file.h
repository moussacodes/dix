#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "editor.h"

typedef struct
{
    char *file_name;
    char *file_path;
    FILE* file;
    unsigned long file_size;
    char *file_type;
    int is_modified;
} FileMetadata;

char **readFileContent(FILE *file, int *numLines);
long getFileSize(FILE *file);
FileMetadata *handle_file(const char *filePath);

#endif // FILE_H