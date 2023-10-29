

#include "editor.h"
#include "file.h"
#include <ncurses.h>
#include <sys/stat.h>
#include <stdbool.h>

typedef struct
{
    WINDOW *_parent;
    char *_text;
    attr_t _attributes;
} StatusBar;

// TODO: handle relative paths

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

void init_status_bar(char *file_name)
{

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    int height, width;
    getmaxyx(stdscr, height, width);
    WINDOW *status_bar = newwin(1, width, height - 1, 0);
    refresh();

    mvwprintw(status_bar, 0, 0, file_name);
    wrefresh(status_bar);

    move(0, 0); // Move the cursor to the top-left corner
    refresh();  // Refresh the screen to show the cursor at the top-left corner

    getch();

    delwin(status_bar);
    endwin();
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

int main(int argc, char const *argv[])
{
    char *file_path;
    char *file_name;
    FILE *fd;
    int lines = 0;
    if (argc < 2)
    {
        perror("Error: not enough arguments");
        exit(EXIT_FAILURE);
    }
    else
    {
        file_path = malloc(strlen(argv[1]) + 1); // +1 for the null-terminator
        if (!file_path)
        {
            perror("Error: couldn't allocate memory");
            exit(EXIT_FAILURE);
        }
        strcpy(file_path, argv[1]);
        file_name = strdup(get_file_name(file_path));
        if (!file_exists(argv[1]))
        {
            fd = create_file(file_path);
        }
    }

    enableRawMode(STDIN_FILENO);
    system("clear");

    EditorState editor = {
        .cursor_x = 0,
        .cursor_y = 0,
        .filename = file_name,
        .lines = NULL,
        .lineCount = 0,
        .characterCount = 0,
        .saved = false};

    initiateNewLine(&editor);

    char **cont = readFileContent(file_path, &lines);
    fill_lines(cont, &editor, lines);
    updateScreen(&editor);

    // init_status_bar(file_name);
    char c;
    while (1)
    {
        if (read(STDIN_FILENO, &c, 1) == 1)
        {
            if (c == CTRL_X)
            {
                break;
            }
            else if (c == 27)
            {
                if (read(STDIN_FILENO, &c, 1) == 1 && c == 91)
                {
                    if (read(STDIN_FILENO, &c, 1) == 1)
                    {
                        if (c == 'A')
                        {
                            insert_key_press(57, &editor);
                        }
                        else if (c == 'B')
                        {
                            insert_key_press(80, &editor);
                        }
                        else if (c == 'C')
                        {
                            insert_key_press(77, &editor);
                        }
                        else if (c == 'D')
                        {
                            insert_key_press(75, &editor);
                        }
                    }

                    else
                    {
                        lseek(STDIN_FILENO, -1, SEEK_CUR);
                    }
                }
            }
            else if (c == 19) // CTRL_S: to save a file
            {
                save_content_to_file(&editor, file_path);
            }
            else
            {
                insertChar(c, &editor);
            }
        }
    }

    freeEditor(&editor);
    free(file_path);
    freeFileContent(cont, lines);
    return 0;
}
