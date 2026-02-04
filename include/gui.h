#ifndef GUI_H
#define GUI_H

#include <raylib.h>

#include "parser.h"

// Helper to hold parsed expressions and their plot color
typedef struct {
    const char *text;
    Node *root;
    Color color;
    bool visible;
} ParsedExpression;

/**
 * Display legend of plotted functions.
 * Returns whether the cursor is hovering over the legend box.
 */
bool display_legend(ParsedExpression *expressions, int count);

#endif