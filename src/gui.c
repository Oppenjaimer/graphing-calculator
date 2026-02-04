#include <raylib.h>

#include "common.h"
#include "gui.h"

bool display_legend(ParsedExpression *expressions, int count) {
    int max_text_width = 0;
    int non_null = 0;

    // Get maximum text width and non-null expressions
    for (int i = 0; i < count; i++) {
        if (expressions[i].root == NULL) continue;
        non_null++;

        int text_width = MeasureText(expressions[i].text, LEGEND_ELEM_SIZE);
        if (text_width > max_text_width) max_text_width = text_width;
    }

    // Calculate legend box size
    int width = 3 * LEGEND_SPACING + LEGEND_ELEM_SIZE + max_text_width;
    int height = (non_null + 1) * LEGEND_SPACING + non_null * LEGEND_ELEM_SIZE;

    // Draw legend box
    Color color = COLOR_BRIGHT_BLACK;
    color.a = LEGEND_OPACITY;
    Rectangle box = {LEGEND_SPACING, LEGEND_SPACING, width, height};
    DrawRectangleRec(box, color);

    // Change cursor on legend box hover
    if (CheckCollisionPointRec(GetMousePosition(), box)) SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    else SetMouseCursor(MOUSE_CURSOR_CROSSHAIR);

    // Draw legend entries
    int non_null_i = 0;
    for (int i = 0; i < count; i++) {
        if (expressions[i].root == NULL) continue;

        // Calculate entry elements coordinates
        int color_x = 2 * LEGEND_SPACING;
        int text_x = 3 * LEGEND_SPACING + LEGEND_ELEM_SIZE;
        int y = 2 * LEGEND_SPACING + non_null_i * (LEGEND_ELEM_SIZE + LEGEND_SPACING);

        // Check for click on color square
        Rectangle toggle = {color_x, y, LEGEND_ELEM_SIZE, LEGEND_ELEM_SIZE};
        if (CheckCollisionPointRec(GetMousePosition(), toggle)) {
            // Toggle visibility
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                expressions[i].visible = !expressions[i].visible;
        }

        DrawText(expressions[i].text, text_x, y, LEGEND_ELEM_SIZE, COLOR_BRIGHT_WHITE);
        if (expressions[i].visible) DrawRectangleRec(toggle, expressions[i].color);
        else DrawRectangleLinesEx(toggle, LEGEND_RECT_THICKNESS, expressions[i].color);

        non_null_i++;
    }

    return CheckCollisionPointRec(GetMousePosition(), box);
}
