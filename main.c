#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

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
    Line **lines;
    char *filename;
    bool saved;
    int lineCount;
    int characterCount;
} EditorState;

static struct termios orig_termios; /* In order to restore at exit. */

int enableRawMode(int fd)
{
    setvbuf(stdout, NULL, _IONBF, 0); // Disable output buffering
    struct termios oldattr, newattr;

    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;

    // Set the terminal to raw mode
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    return 0;
}

void initiateNewLine(int lnb, EditorState *e)
{
    Line *newLine = malloc(sizeof(Line));
    CHECK_MEMORY(newLine);
    newLine->lineContent = NULL;
    newLine->line = lnb + 1;
    newLine->position = 0;

    e->lines = SAFE_REALLOC(e->lines, (e->lineCount + 1) * sizeof(Line *));
    e->lines[e->lineCount++] = newLine;
}

void updateScreen(EditorState *e)
{
    write(STDOUT_FILENO, "\x1b[2J", 4); // Clear the screen
    write(STDOUT_FILENO, "\x1b[H", 3);  // Move cursor to top-left position

    for (int i = 0; i < e->lineCount; i++)
    {
        write(STDOUT_FILENO, e->lines[i]->lineContent, strlen(e->lines[i]->lineContent));
    }
}

void insertChar(char addedChar, EditorState *e)
{
    Line *line = e->lines[e->lineCount - 1];

    switch (addedChar)
    {
    case ENTER:
        addCharToBuffer('\n', line);
        initiateNewLine(e->lineCount, e);
        updateScreen(e);
        break;
    case ESCAPE:
        printf("Escape key pressed.\n");
        break;
    case BACKSPACE:
        if (line->position > 0)
        {
            line->lineContent = SAFE_REALLOC(line->lineContent, line->position);
            line->lineContent[--line->position] = '\0';
            updateScreen(e);
        }
        else
        {
            // treat the case where the line is empty
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
        updateScreen(e);
        break;
    }
}

void addCharToBuffer(const char addedChar, Line *line)
{
    line->lineContent = SAFE_REALLOC(line->lineContent, (line->position) + 2); // Increase size for the new character and the null terminator
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

void freeEditor(EditorState e)
{
    for (int i = 0; i < e.lineCount; i++)
    {
        free(e.lines[i]->lineContent);
        free(e.lines[i]);
    }
    free(e.lines);
}

int main()
{
    enableRawMode(STDIN_FILENO);
    system("clear");
    EditorState editor = initiateEditor();
    int lineNb = editor.lineCount;
    SAFE_MALLOC(editor.lines, sizeof(Line *));
    initiateNewLine(lineNb, &editor);

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
                // write(STDOUT_FILENO, &c, 1); // writes a character out
            }
        }
    }
    freeEditor(editor);
    return 0;
}
