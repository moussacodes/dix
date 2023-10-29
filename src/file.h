#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "editor.h"
 


void save_content_to_file(EditorState* editor, char* file_path);
bool file_exists(char *filename);
char *get_file_part(char separator, char *file_path);
FILE *create_file(char *file_path);
void fill_lines(char **file_content, EditorState *editor, int lines);
bool check_if_file_empty(char *file_path);
char **readFileContent(const char *file_path, int *num_lines);
void freeFileContent(char **content, int num_lines);

#endif // FILE_H
