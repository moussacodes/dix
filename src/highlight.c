#include "highlight.h"

char **ret_words(char *content, int *words)
{
    char **words_list = malloc(sizeof(char *));
    char buffer[256];
    int index = 0;
    int word_count = 0;

    for (int i = 0; content[i] != '\0'; i++)
    {
        if (content[i] == ' ')
        {
            buffer[index] = '\0';
            words_list[word_count] = malloc(strlen(buffer) + 1);
            strcpy(words_list[word_count], buffer);
            words_list = realloc(words_list, (word_count + 2) * sizeof(char *));
            word_count++;
            index = 0;
        }
        else
        {
            buffer[index++] = content[i];
        }
    }

    buffer[index] = '\0';
    words_list[word_count] = malloc(strlen(buffer) + 1);
    strcpy(words_list[word_count], buffer);
    word_count++;
    words_list[word_count] = NULL;
    *words = word_count;

    return words_list;
}

bool is_c_keyword(char *word) {
    char *keywords[44] = {
        "auto", "double", "int", "struct",
        "break", "else", "long", "switch",
        "case", "enum", "register", "typedef",
        "char", "extern", "return", "union",
        "const", "float", "short", "unsigned",
        "continue", "for", "signed", "void",
        "default", "goto", "sizeof", "volatile",
        "do", "if", "static", "while",
        "FILE", "stdin", "stdout", "stderr",
        "fopen", "fclose", "printf", "scanf",
        "malloc", "calloc", "free", "exit"
    };

    for (int i = 0; i < 44; i++) {
        if (strcmp(word, keywords[i]) == 0) {
            return true;
        }
    }
    return false;
}
