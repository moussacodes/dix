#include "file.h"

void save_content_to_file(EditorState *editor, char *file_path)
{
    FILE *file = fopen(file_path, "w");
    if (!file)
    {
        perror("Error: couldn't open file");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < editor->lineCount; i++)
    {
        fprintf(file, "%s", editor->lines[i]->lineContent);

    }

 
    fclose(file);
}