#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

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
    // Add more control characters as needed
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

void enableRawMode()
{
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

Line initiateNewLine(int lnb, int tab)
{
    // char *newLineContent = malloc(sizeof(char));
    // if (newLineContent == NULL)
    // {
    //     perror("Error: Failed to allocate memory");
    //     exit(0);
    // }
    Line newLine = {
        .lineContent = "newLine",
        .line = lnb + 1,
        .position = 0 + tab};

    return newLine;
}

void printLine(Line l)
{
    char *str = l.lineContent;
    int length = strlen(str);
    printf("%s", str);
    write(STDOUT_FILENO, str, length);
}

void printContent(EditorState e)
{
    for (int i = 0; i < e.lineCount; i++)
    {
        printLine(e.lines[i]);
    }
}

void insertChar(char addedChar, EditorState e)
{

    Line line = e.lines[e.lineCount];
    switch (addedChar)
    {
    case ENTER:
        printf("Enter key pressed.\n");
        break;
    case ESCAPE:
        printf("Escape key pressed.\n");
        break;
    case BACKSPACE:
        if (line.position > 0)
        {
            line.lineContent = realloc(line.lineContent, strlen(line.lineContent));
            line.lineContent[--line.position] = '\0';
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
        if (line.lineContent == NULL)
        {
            line.lineContent = malloc(sizeof(char)); // Allocate memory for an empty string
            line.lineContent[0] = '\0';   // Initialize the string
        }
        line.lineContent = realloc(line.lineContent, line.position + 2); // Add 2 for the new character and the null terminator
        line.lineContent[line.position++] = addedChar;
        line.lineContent[line.position] = '\0';

        break;
    }

    printContent(e);
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
    enableRawMode();
    system("clear");
    int lineNb = 0;
    EditorState editor = initiateEditor();
    lineNb = editor.lineCount;
    editor.lines = malloc(sizeof(Line));
    editor.lines[lineNb] = initiateNewLine(lineNb, 0);

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