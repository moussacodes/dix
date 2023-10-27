#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>

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
    Line *lines;
    char *filename;
    bool saved;
    int lineCount;
    int characterCount;
} EditorState;
static struct termios orig_termios; /* In order to restore at exit.*/

int enableRawMode(int fd)
{
    setvbuf(stdout, (char *)NULL, _IONBF, 0);
    struct termios oldattr, newattr;

    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;

    // Set the terminal to raw mode
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
}

Line initiateNewLine(int lnb)
{
    char *lineContentText = malloc(sizeof(char)); // Make sure to allocate enough memory
    strcpy(lineContentText, "");
    Line newLine = {
        .lineContent = lineContentText,
        .line = lnb + 1,
        .position = 0};

    return newLine;
}

void updateScreen(EditorState *e)
{
    write(STDOUT_FILENO, "\x1b[2J", 4); // Clear the screen
    write(STDOUT_FILENO, "\x1b[H", 3);  // Move cursor to top-left position

    for (int i = 0; i < e->lineCount; i++)
    {
        write(STDOUT_FILENO, e->lines[i].lineContent, strlen(e->lines[i].lineContent));
        write(STDOUT_FILENO, "\n", 1);
    }
}

// void updateScreen(Ed)
// {
//     write(STDOUT_FILENO, "\x1b[2J", 4); // Clear the screen
//     write(STDOUT_FILENO, "\x1b[H", 3);  // Move cursor to top-left position

//     for (int i = 0; i < e->lineCount; i++) {
//         write(STDOUT_FILENO, e->lines[i].lineContent, strlen(e->lines[i].lineContent));
//         write(STDOUT_FILENO, "\r\n", 2); // Add a newline after each line
//     }

//     write(STDOUT_FILENO, line, strlen(line));
// }

void insertChar(char addedChar, EditorState e)
{
    Line *line = &(e.lines[e.lineCount]);

    switch (addedChar)
    {
    case ENTER:
        addCharToBuffer('\n', line);
        Line l = initiateNewLine(e.lineCount);
        e.lines[e.lineCount++] = l;
        updateScreen(&e);
        break;
    case ESCAPE:
        printf("Escape key pressed.\n");
        break;
    case BACKSPACE:
        if (line->position > 0)
        {
            line->lineContent = SAFE_REALLOC(line->lineContent, line->position);
            line->lineContent[--line->position] = '\0';
            updateScreen(&e);
        }
        break;
    case TAB:
        printf("Tab key pressed.\n");
        break;
    case CTRL_C:
        printf("Ctrl + C key pressed.\n");
        break;
    case CTRL_S:
        printf("Ctrl + S key pressed.\n");
        break;
    case CTRL_V:
        printf("Ctrl + V key pressed.\n");
        break;
    case CTRL_X:
        printf("Ctrl + X key pressed.\n");
        break;
    default:
        addCharToBuffer(addedChar, line);

        updateScreen(&e);
        break;
    }
}

void addCharToBuffer(const char addedChar, Line *line)
{
    line->lineContent = realloc(line->lineContent, (line->position) + 2); // Increase size for the new character and the null terminator
    line->lineContent[line->position++] = addedChar;
    line->lineContent[line->position] = '\0';
}

EditorState initiateEditor()
{
    EditorState editor = {
        .cursor_x = 0,
        .cursor_y = 0,
        .filename = "empty",
        .lines = NULL,
        .lineCount = 0,
        .characterCount = 0,
        .saved = false};
    return editor;
}

int main()
{
    enableRawMode(STDIN_FILENO);
    system("clear");
    int lineNb = 0;
    EditorState editor = initiateEditor();
    lineNb = editor.lineCount;
    SAFE_MALLOC(editor.lines, 1);
    editor.lines[lineNb] = initiateNewLine(lineNb);

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
                insertChar(c, editor);
                // write(STDOUT_FILENO, &c, 1); // writes a character out
            }
        }
    }
    freeEditor(editor);
    return 0;
}

void freeEditor(EditorState e)
{
    for (int i = 0; i < e.lineCount; i++)
    {
        free(e.lines[i].lineContent);
    }
    free(e.lines);
}

// fix error when character is pressed