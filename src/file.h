#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "editor.h"
 


void save_content_to_file(EditorState* editor, char* file_path);

#endif // FILE_H
