#ifndef HIGHLIGHT_H
#define HIGHLIGHT_H

#include "editor.h"

#define IS_OP(CH) ( \
    (CH) == '(' || (CH) == ')' || (CH) == '[' || (CH) == ']' || (CH) == '{' || (CH) == '}' || (CH) == ',' || (CH) == '|' || (CH) == '_' || (CH) == ';' || (CH) == ':') || (CH) == '+' || (CH) == '-' || (CH) == '*' || (CH) == '/' || (CH) == '%') || (CH) == '=' || (CH) == '<' || (CH) == '>' || (CH) == '!')

char **ret_words(char *content, int *words);

#endif // HIGHLIGHT_H
