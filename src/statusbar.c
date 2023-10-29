#include "statusbar.h"

StatusBar *createStatusBar(WINDOW *parent)
{
    StatusBar *statusBar = (StatusBar *)malloc(sizeof(StatusBar));
    if (statusBar == NULL)
    {
        return NULL; // Error handling for memory allocation failure
    }
    statusBar->_parent = parent;
    statusBar->_text = NULL; // Initialize text to NULL
    statusBar->_attributes = 0; // Initialize attributes to 0
    return statusBar;
}

char *getStatusBarText(StatusBar *statusBar)
{
    return statusBar->_text;
}

void setStatusBarText(StatusBar *statusBar, char *text, attr_t attributes)
{
    free(statusBar->_text); // Free the previously allocated text
    statusBar->_text = (char *)malloc((strlen(text) + 1) * sizeof(char)); // Allocate memory for the new text
    if (statusBar->_text == NULL)
    {
        // Error handling for memory allocation failure
        return;
    }
    strcpy(statusBar->_text, text); // Copy the new text
    if (attributes != 0)
    {
        statusBar->_attributes = attributes;
    }
}

attr_t getStatusBarAttributes(StatusBar *statusBar)
{
    return statusBar->_attributes;
}

void setStatusBarAttributes(StatusBar *statusBar, attr_t attributes)
{
    statusBar->_attributes = attributes;
}

void drawStatusBar(StatusBar *statusBar)
{
    int output_row = getmaxy(statusBar->_parent) - 1;
    attron(statusBar->_attributes);
    mvwprintw(statusBar->_parent, output_row, 0, getStatusBarText(statusBar));
    attroff(statusBar->_attributes);
}
