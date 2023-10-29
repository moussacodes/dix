#include "file.h"

void save_content_to_file(EditorState *editor, char *file_path) {
    FILE *file = fopen(file_path, "w");
    if (!file) {
        perror("Error: couldn't open file");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < editor->lineCount; i++) {
        if (editor->lines[i] && editor->lines[i]->lineContent) {
            if (strlen(editor->lines[i]->lineContent) == 0) {
                fprintf(file, "%c", '\n');
            } else {
                fprintf(file, "%s", editor->lines[i]->lineContent);
            }
        } else {
            fprintf(file, "%c", '\n');  // handle null case by printing new line
        }
    }
    editor->saved = true;
    fclose(file);
}



bool file_exists(char *filename)
{
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

char *get_file_part(char separator, char *file_path)
{

    char *p;
    int index = 0;
    for (int i = strlen(file_path) - 1; i >= 0; i--)
    {
        if (file_path[i] == separator)
        {
            index = i;
            break;
        }
    }
    p = malloc(sizeof(char) * (strlen(file_path) - index));
    if (!p)
    {
        // Handle error, if necessary
        return NULL;
    }
    for (int i = index + 1, j = 0; i < strlen(file_path); i++, j++)
    {
        p[j] = file_path[i];
    }
    p[strlen(file_path) - index - 1] = '\0'; // Add the null-terminator
    return p;
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

void freeFileContent(char **content, int num_lines)
{
    if (content == NULL)
    {
        return;
    }
    for (int i = 0; i < num_lines; i++)
    {
        free(content[i]);
    }
    free(content);
}

char **readFileContent(const char *file_path, int *num_lines)
{
    FILE *file = fopen(file_path, "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return NULL;
    }

    int size = 10;
    int lines = 0;
    char **content = (char **)malloc(size * sizeof(char *));
    if (content == NULL)
    {
        perror("Memory allocation failed");
        return NULL;
    }

    char *buffer = NULL;
    size_t length = 0;
    ssize_t read;
    while ((read = getline(&buffer, &length, file)) != -1)
    {
        if (lines >= size)
        {
            size *= 2;
            content = (char **)realloc(content, size * sizeof(char *));
            if (content == NULL)
            {
                perror("Memory reallocation failed");
                return NULL;
            }
        }
        content[lines] = strdup(buffer);
        lines++;
    }

    free(buffer);
    *num_lines = lines;
    fclose(file);
    return content;
}
