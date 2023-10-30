#include "editor.h"

static struct termios orig_termios; /* In order to restore at exit. */

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

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

void initiateNewLine(EditorState *e)
{
    e->lines = SAFE_REALLOC(e->lines, (e->lineCount + 1) * sizeof(Line *));
    e->lines[e->lineCount] = malloc(sizeof(Line));
    if (!e->lines[e->lineCount])
    {
        fprintf(stderr, "Memory allocation failed at %s:%d\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }
    Line *newLine = e->lines[e->lineCount];
    newLine->lineContent = NULL;
    newLine->line = e->lineCount + 1;
    newLine->position = 0;
    e->lineCount++;
    e->cursor_x = 0;
    e->cursor_y += 1;
}

// fix segmentation fault here:

void updateScreen(EditorState *e) {
    write(STDOUT_FILENO, "\x1b[2J", 4); // Clear the screen
    write(STDOUT_FILENO, "\x1b[H", 3);  // Move cursor to top-left position
    char **words;
    int word_count = 0;

    for (int i = 0; i < e->lineCount; i++) {
        if (e->lines[i]->lineContent != NULL) {
            words = ret_words(e->lines[i]->lineContent, &word_count);
            if (words) {
                for (int j = 0; j < word_count; j++) {
                    if (is_c_keyword(words[j])) {
                        write(STDOUT_FILENO, ANSI_COLOR_RED, strlen(ANSI_COLOR_RED));
                        write(STDOUT_FILENO, words[j], strlen(words[j]));
                        write(STDOUT_FILENO, ANSI_COLOR_RESET, strlen(ANSI_COLOR_RESET));
                    } else {
                        write(STDOUT_FILENO, words[j], strlen(words[j]));
                    }
                    free(words[j]); // Free the allocated memory for each word
                }
                free(words); // Free the allocated memory for the words list
            }
        }
    }
}

void insert_key_press(int key_press, EditorState *editor)
{

    switch (key_press)
    {

    case UP_ARROW:

        editor->cursor_y = (editor->cursor_y > 0) ? (editor->cursor_y - 1) : (editor->cursor_x = 0);
        break;
    case DOWN_ARROW:
        if (editor->cursor_y == editor->lineCount)
        {
            editor->cursor_x = strlen(editor->lines[editor->lineCount]->lineContent);
        }
        else
        {
            editor->cursor_y += 1;
        }
        updateScreen(editor);
        break;
    case RIGHT_ARROW:
        if (editor->cursor_x == strlen(editor->lines[editor->cursor_y]->lineContent))
        {
            if (editor->cursor_y < editor->lineCount)
            {
                editor->cursor_y += 1;
            }
        }
        else
        {
            editor->cursor_x += 1;
        }
        updateScreen(editor);
        break;
    case LEFT_ARROW:
        if (editor->cursor_x == 0)
        {
            if (editor->cursor_y > 0)
            {
                editor->cursor_y -= 1;
            }
        }
        else
        {
            editor->cursor_x -= 1;
        }

        updateScreen(editor);
        break;
    }
}

void insertChar(char addedChar, EditorState *e)
{
    Line *line = e->lines[e->lineCount - 1];
    Line *tempLine; // will be used to delete lines

    switch (addedChar)
    {
    case ENTER:
        addCharToBuffer('\n', line);
        initiateNewLine(e);
        updateScreen(e);
        break;
    case ESCAPE:
        printf("Escape key pressed.\n");
        break;
    case BACKSPACE:
        if (line->position > 0)
        {
            line->lineContent[--line->position] = '\0';
            e->cursor_x -= 1;
            updateScreen(e);
        }
        else
        {
            if (e->lineCount > 1)
            {

                free(line->lineContent);
                e->lineCount -= 1;
                e->cursor_y -= 1;
                line = e->lines[e->lineCount - 1];
                e->cursor_x -= 1;
                line->position = strlen(line->lineContent);
                if (line->position > 0)
                {
                    size_t newSize = line->position * sizeof(char); // Calculate the new size in bytes
                    line->lineContent = realloc(line->lineContent, newSize);
                }
                else
                {

                    line->lineContent = NULL;
                    updateScreen(e);
                }
            }
            else
            {
                updateScreen(e);
            }
        }
        break;

    case TAB:

        for (int i = 0; i < 4; i++)
        {
            addCharToBuffer(' ', line);
        }
        e->cursor_x += 4;
        // maybe i'll replace it with addCharToBuffer('\t', line); later on
        updateScreen(e);
        break;

    default:
        addCharToBuffer(addedChar, line);
        e->cursor_x += 1;
        updateScreen(e);
        break;
    }
}

void addCharToBuffer(const char addedChar, Line *line)
{
    if (line->lineContent == NULL)
    {
        line->lineContent = malloc(2 * sizeof(char)); // Allocate initial memory for one character and the null terminator
        CHECK_MEMORY(line->lineContent);
        line->lineContent[0] = addedChar;
        line->lineContent[1] = '\0';
        line->position = 1; // Update the position
    }
    else
    {
        char *temp_ptr = realloc(line->lineContent, (line->position) + 2); // Increase size for the new character and the null terminator
        if (temp_ptr == NULL)
        {
            fprintf(stderr, "Memory reallocation failed at %s:%d\n", __FILE__, __LINE__);
            exit(EXIT_FAILURE);
        }
        line->lineContent = temp_ptr;
        line->lineContent[line->position] = addedChar;
        line->lineContent[line->position + 1] = '\0'; // Update the null terminator
        line->position++;                             // Increment the position
    }
}

void freeEditor(EditorState *e)
{
    for (int i = 0; i < e->lineCount; i++)
    {
        free(e->lines[i]->lineContent);
        free(e->lines[i]);
    }
    free(e->lines);
}
