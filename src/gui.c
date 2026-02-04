#include <raylib.h>

#include "common.h"
#include "gui.h"

void display_legend(ParsedExpression *expressions, int count) {
    int max_text_width = 0;
    int non_null = 0;

    // Get maximum text width and non-null expressions
    for (int i = 0; i < count; i++) {
        if (expressions[i].root == NULL) continue;
        non_null++;

        int text_width = MeasureText(expressions[i].text, LEGEND_ELEM_SIZE);
        if (text_width > max_text_width) max_text_width = text_width;
    }

    // Calculate legend rectangle size
    int width = 3 * LEGEND_SPACING + LEGEND_ELEM_SIZE + max_text_width;
    int height = (non_null + 1) * LEGEND_SPACING + non_null * LEGEND_ELEM_SIZE;

    // Draw legend rectangle
    Color color = COLOR_BRIGHT_BLACK;
    color.a = LEGEND_OPACITY;
    DrawRectangle(LEGEND_SPACING, LEGEND_SPACING, width, height, color);

    // Draw legend entries
    int non_null_i = 0;
    for (int i = 0; i < count; i++) {
        if (expressions[i].root == NULL) continue;

        // Draw entry color square and text
        int color_x = 2 * LEGEND_SPACING;
        int text_x = 3 * LEGEND_SPACING + LEGEND_ELEM_SIZE;
        int y = 2 * LEGEND_SPACING + non_null_i * (LEGEND_ELEM_SIZE + LEGEND_SPACING);
        DrawRectangle(color_x, y, LEGEND_ELEM_SIZE, LEGEND_ELEM_SIZE, expressions[i].color);
        DrawText(expressions[i].text, text_x, y, LEGEND_ELEM_SIZE, COLOR_BRIGHT_WHITE);

        non_null_i++;
    }
}