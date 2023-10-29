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

bool file_exists(char *filename)
{
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

char *get_file_name(char *file_path)
{

    char *file_name;
    int index = 0;
    for (int i = strlen(file_path) - 1; i >= 0; i--)
    {
        if (file_path[i] == '/')
        {
            index = i;
            break;
        }
    }
    file_name = malloc(sizeof(char) * (strlen(file_path) - index));
    if (!file_name)
    {
        // Handle error, if necessary
        return NULL;
    }
    for (int i = index + 1, j = 0; i < strlen(file_path); i++, j++)
    {
        file_name[j] = file_path[i];
    }
    file_name[strlen(file_path) - index - 1] = '\0'; // Add the null-terminator
    return file_name;
}

FILE *create_file(char *file_path)
{
    FILE *fd;
    fd = fopen(file_path, "w");
    if (fd == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    return fd;
}

void fill_lines(char **file_content, EditorState *editor, int lines)
{
    editor->lines = malloc(sizeof(Line *) * lines);
    if (editor->lines == NULL)
    {
        perror("Memory allocation failed for lines");
        exit(EXIT_FAILURE);
    }

    editor->lineCount = 0; // Initialize lineCount

    for (int i = 0; i < lines; i++)
    {
        editor->lines[i] = malloc(sizeof(Line));
        if (editor->lines[i] == NULL)
        {
            perror("Memory allocation failed for line");
            exit(EXIT_FAILURE);
        }

        editor->lines[i]->line = i;
        editor->lines[i]->position = strlen(file_content[i]);

        // Allocate memory for lineContent
        editor->lines[i]->lineContent = malloc(sizeof(char) * (editor->lines[i]->position + 1));
        if (editor->lines[i]->lineContent == NULL)
        {
            perror("Memory allocation failed for lineContent");
            exit(EXIT_FAILURE);
        }

        // Copy the content and ensure null-termination
        strcpy(editor->lines[i]->lineContent, file_content[i]);

        // Increment the line count
        editor->lineCount++;
    }
}

// check if file is empty

bool check_if_file_empty(char *file_path)
{
    FILE *file = fopen(file_path, "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return 1;
    }

    fseek(file, 0, SEEK_END); // Move the file pointer to the end of the file

    return ftell(file) == 0;

    fclose(file);
}
