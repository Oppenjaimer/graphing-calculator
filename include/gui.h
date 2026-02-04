#ifndef GUI_H
#define GUI_H

#include <raylib.h>

#include "parser.h"

// Helper to hold parsed expressions and their plot color
typedef struct {
    const char *text;
    Node *root;
    Color color;
} ParsedExpression;

/**
 * Display legend of plotted functions.
 */
void display_legend(ParsedExpression *expressions, int count);

#endif