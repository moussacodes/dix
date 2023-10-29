#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    WINDOW *_parent;
    char *_text;
    attr_t _attributes;
} StatusBar;

StatusBar *createStatusBar(WINDOW *parent);

char *getStatusBarText(StatusBar *statusBar);

void setStatusBarText(StatusBar *statusBar, char *text, attr_t attributes);

attr_t getStatusBarAttributes(StatusBar *statusBar);

void setStatusBarAttributes(StatusBar *statusBar, attr_t attributes);

void drawStatusBar(StatusBar *statusBar);

#endif /* STATUSBAR_H */
