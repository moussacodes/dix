

#include "editor.h"
#include <ncurses.h>

typedef struct
{
    WINDOW *_parent;
    char *_text;
    attr_t _attributes;
} StatusBar;

int main(int argc, char const *argv[])
{
    char *file_path;
    if (argc < 2)
    {
        perror("Error: not enough arguments");
        exit(EXIT_FAILURE);
    }
    else
    {
        file_path = malloc(sizeof(char) * sizeof(argv[1]));
        strcpy(file_path, argv[1]);
    }

    enableRawMode(STDIN_FILENO);
    system("clear");

    EditorState editor = {
        .cursor_x = 0,
        .cursor_y = 0,
        .filename = "empty",
        .lines = NULL,
        .lineCount = 0,
        .characterCount = 0,
        .saved = false};

    initiateNewLine(&editor);

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    int height, width;
    getmaxyx(stdscr, height, width);
    WINDOW *status_bar = newwin(1, width, height - 1, 0);
    refresh();

    wprintw(status_bar, file_path);
    wrefresh(status_bar);

    getch();

    delwin(status_bar);
    endwin();

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
