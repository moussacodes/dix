#ifndef EDITOR_H
#define EDITOR_H

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <ncurses.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>

#define CHECK_MEMORY(ptr)                                                               \
    do                                                                                  \
    {                                                                                   \
        if (!(ptr))                                                                     \
        {                                                                               \
            fprintf(stderr, "Memory allocation failed at %s:%d\n", __FILE__, __LINE__); \
            exit(EXIT_FAILURE);                                                         \
        }                                                                               \
    } while (0)

#define SAFE_MALLOC(ptr, size) \
    do                         \
    {                          \
        ptr = malloc(size);    \
        CHECK_MEMORY(ptr);     \
    } while (0)

#define SAFE_REALLOC(ptr, size)              \
    ({                                       \
        void *temp_ptr = realloc(ptr, size); \
        CHECK_MEMORY(temp_ptr);              \
        temp_ptr;                            \
    })

enum Keypress
{
    ENTER = 10,
    ESCAPE = 27,
    BACKSPACE = 127,
    TAB = 9,
    CTRL_C = 3,
    CTRL_S = 19,
    CTRL_V = 22,
    CTRL_X = 24,
    UP_ARROW = 57,
    DOWN_ARROW = 80,
    RIGHT_ARROW = 77,
    LEFT_ARROW = 75
};

typedef struct
{
    char *lineContent;
    int position;
    int line;
} Line;

typedef struct
{
    int cursor_x, cursor_y;
    Line **lines;
    char *filename;
    char *filextension;
    bool saved;
    int lineCount;
    int characterCount;
} EditorState;

int enableRawMode(int fd);
void initiateNewLine(EditorState *e);
void updateScreen(EditorState *e);
void insertChar(char addedChar, EditorState *e);
void addCharToBuffer(const char addedChar, EditorState *e);
void freeEditor(EditorState *e);
void insert_key_press(int key_press, EditorState *editor);

#endif // EDITOR_H
