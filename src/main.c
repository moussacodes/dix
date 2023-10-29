

#include "editor.h"
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

int main(int argc, char const *argv[])
{
    char *file_path;
    char *file_name;
    FILE *fd;
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

    init_status_bar(file_name);
    char c;
    while (1)
    {
        if (read(STDIN_FILENO, &c, 1) == 1)
        {
            if (c == CTRL_X)
            {
                break;
            }
            else
            {
                insertChar(c, &editor);
            }
        }
    }
    freeEditor(&editor);
    free(file_path);

    return 0;
}

