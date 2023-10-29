#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "editor.h"
#include "statusbar.h"
#include "file.h"

#define CTRL_X 24

void cleanUp(EditorState *editor, FileMetadata *fd, char **cont, char *file_path, int lines) {
    if (editor) {
        freeEditor(editor);
        free(editor);
    }
    if (cont) {
        for (int i = 0; i < lines; i++) {
            if (cont[i]) free(cont[i]);
        }
        free(cont);
    }
    if (file_path) free(file_path);
    if (fd) {
        if (fd->file) fclose(fd->file);
        if (fd->file_name) free(fd->file_name);
        if (fd->file_path) free(fd->file_path);
        if (fd->file_type) free(fd->file_type);
        free(fd);
    }
}



void fill_lines(char **content, EditorState *editor, int lineNb)
{
    editor->lines = malloc(sizeof(Line *) * lineNb);
    if (!editor->lines)
    {
        perror("Error: can't allocate memory");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < lineNb; i++)
    {
        Line *line = malloc(sizeof(Line));
        if (!line)
        {
            perror("Error: can't allocate memory");
            exit(EXIT_FAILURE);
        }
        line->position = 0;
        line->line = i;
        strncpy(line->lineContent, content[i], 255); // Copy up to 255 characters to lineContent
        line->lineContent[255] = '\0';               // Null-terminate the lineContent
        editor->lines[i] = line;
    }
}

int main(int argc, char const *argv[]) {

    FileMetadata *fd = NULL;
    int lines = 0;
    char *file_path = NULL;
    if (argc < 2) {
        perror("Error");
        exit(EXIT_FAILURE);
    } else {
        file_path = strdup(argv[1]);
        fd = handle_file(file_path);
    }

    enableRawMode(STDIN_FILENO);
    system("clear");

    EditorState *editor = malloc(sizeof(EditorState));
    if (!editor) {
        perror("Couldn't allocate memory");
        cleanUp(editor, fd, NULL, file_path, lines);
        exit(EXIT_FAILURE);
    }
    editor->cursor_x = 0;
    editor->cursor_y = 0;
    editor->lines = NULL;
    editor->lineCount = 0;
    editor->saved = 0;
    editor->characterCount = 0;
    editor->filename = strdup(file_path);

    char **cont = readFileContent(fd->file, &lines);
    if (!cont) {
        cleanUp(editor, fd, NULL, file_path, lines);
        exit(EXIT_FAILURE);
    }

    fill_lines(cont, editor, lines);
    initiateNewLine(editor);

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    int height, width;
    getmaxyx(stdscr, height, width);
    WINDOW *status_bar = newwin(1, width, height - 1, 0);
    refresh();

    wprintw(status_bar, "This is a status bar");
    wrefresh(status_bar);

    getch();

    delwin(status_bar);
    endwin();

    char c;
    while (1) {
        if (read(STDIN_FILENO, &c, 1) == 1) {
            if (c == CTRL_X) {
                break;
            } else {
                insertChar(c, editor);
            }
        }
    }

    cleanUp(editor, fd, cont, file_path, lines);
    return 0;
}
