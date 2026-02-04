#include <math.h>
#include <raylib.h>
#include <raymath.h>

#include "common.h"
#include "draw.h"

// Helper to hold view bounds
typedef struct {
    Vector2 min; // Top-left
    Vector2 max; // Bottom-right
} ViewContext;

static ViewContext get_view_context(Camera2D *camera) {
    // Get screen vertices in world space
    return (ViewContext){
        .min = GetScreenToWorld2D((Vector2){0, 0}, *camera),
        .max = GetScreenToWorld2D((Vector2){WIDTH, HEIGHT}, *camera),
    };
}

static Vector2 get_grid_range(float min, float max, float spacing) {
    // Compute start and end points for grid lines
    float start = floorf(min / spacing) * spacing;
    float end = ceilf(max / spacing) * spacing;

    return (Vector2){start, end};
}

static float pixels_to_math(float px) {
    // Convert to math units
    return px / GRID_INITIAL_SPACING * GRID_UNITS_PER_SPACE;
}

static float math_to_pixels(float math) {
    // Convert to pixels
    return math / GRID_UNITS_PER_SPACE * GRID_INITIAL_SPACING;
}

void draw_grid(Camera2D *camera, float dynamic_spacing) {
    ViewContext ctx = get_view_context(camera);
    Vector2 x_range = get_grid_range(ctx.min.x, ctx.max.x, dynamic_spacing);
    Vector2 y_range = get_grid_range(ctx.min.y, ctx.max.y, dynamic_spacing);

    // Apply grid render buffer to coordinates so that a few lines are rendered beyond the screen
    x_range.x -= GRID_RENDER_BUFFER * dynamic_spacing;
    x_range.y += GRID_RENDER_BUFFER * dynamic_spacing;
    y_range.x -= GRID_RENDER_BUFFER * dynamic_spacing;
    y_range.y += GRID_RENDER_BUFFER * dynamic_spacing;

    // Draw vertical lines
    for (float x = x_range.x; x <= x_range.y; x += dynamic_spacing) {
        // Make major lines brighter
        Color color = COLOR_BRIGHT_WHITE;
        int line_index = (int)lroundf(x / dynamic_spacing);
        color.a = (line_index % GRID_MAJOR_STEP == 0) ? GRID_MAJOR_OPACITY : GRID_MINOR_OPACITY;
        if (fabsf(x) < GRID_AXIS_THRESHOLD) color.a = 255;

        DrawLineV((Vector2){x, ctx.min.y}, (Vector2){x, ctx.max.y}, color);
    }

    // Draw horizontal lines
    for (float y = y_range.x; y <= y_range.y; y += dynamic_spacing) {
        // Make major lines brighter
        Color color = COLOR_BRIGHT_WHITE;
        int line_index = (int)lroundf(y / dynamic_spacing);
        color.a = (line_index % GRID_MAJOR_STEP == 0) ? GRID_MAJOR_OPACITY : GRID_MINOR_OPACITY;
        if (fabsf(y) < GRID_AXIS_THRESHOLD) color.a = 255;

        DrawLineV((Vector2){ctx.min.x, y}, (Vector2){ctx.max.x, y}, color);
    }
}

void draw_grid_labels(Camera2D *camera, float dynamic_spacing) {
    ViewContext ctx = get_view_context(camera);
    Vector2 x_range = get_grid_range(ctx.min.x, ctx.max.x, dynamic_spacing);
    Vector2 y_range = get_grid_range(ctx.min.y, ctx.max.y, dynamic_spacing);
    Vector2 origin_screen = GetWorldToScreen2D((Vector2){0, 0}, *camera);

    // Calculate text width
    const char *zero_text = "0";
    int zero_width = MeasureText(zero_text, GRID_LABEL_SIZE);

    // Clamp X and Y positions so origin label stays visible at screen edges
    float zero_x = Clamp(origin_screen.x, GRID_LABEL_CLAMP_OFFSET + zero_width + GRID_LABEL_OFFSET,
                         (float)WIDTH + GRID_LABEL_OFFSET - GRID_LABEL_CLAMP_OFFSET);
    float zero_y = Clamp(origin_screen.y, GRID_LABEL_CLAMP_OFFSET - GRID_LABEL_OFFSET,
                         (float)HEIGHT - GRID_LABEL_SIZE - GRID_LABEL_CLAMP_OFFSET);

    // Draw origin label at clamped X and Y positions
    DrawText(zero_text, (int)zero_x - zero_width - (int)GRID_LABEL_OFFSET, (int)zero_y + (int)GRID_LABEL_OFFSET,
        GRID_LABEL_SIZE, COLOR_BRIGHT_WHITE);

    // Draw X labels
    for (float x = x_range.x; x <= x_range.y; x += dynamic_spacing) {
        // Skip if zero or not a major grid line
        int line_index = (int)lroundf(x / dynamic_spacing);
        if (line_index % GRID_MAJOR_STEP != 0 || line_index == 0) continue;

        // Get X position in screen space
        Vector2 pos_world = {x, 0};
        Vector2 pos_screen = GetWorldToScreen2D(pos_world, *camera);

        // Calculate text width
        float math_value = pixels_to_math(x);
        const char* text = TextFormat("%g", math_value);
        int text_width = MeasureText(text, GRID_LABEL_SIZE);

        // Clamp Y position so labels stay visible at screen edges
        float label_y = Clamp(origin_screen.y, GRID_LABEL_CLAMP_OFFSET - GRID_LABEL_OFFSET,
                              (float)HEIGHT - GRID_LABEL_SIZE - GRID_LABEL_CLAMP_OFFSET);

        // Draw labels at line X position and clamped Y position
        DrawText(text, (int)pos_screen.x - text_width / 2, (int)label_y + (int)GRID_LABEL_OFFSET,
                 GRID_LABEL_SIZE, COLOR_BRIGHT_WHITE);
    }

    // Draw Y labels
    for (float y = y_range.x; y <= y_range.y; y += dynamic_spacing) {
        // Skip if zero or not a major grid line
        int line_index = (int)lroundf(y / dynamic_spacing);
        if (line_index % GRID_MAJOR_STEP != 0 || line_index == 0) continue;

        // Get Y position in screen space
        Vector2 pos_world = {0, y};
        Vector2 pos_screen = GetWorldToScreen2D(pos_world, *camera);

        // Calculate text width
        float math_value = pixels_to_math(-y);
        const char* text = TextFormat("%g", math_value);
        int text_width = MeasureText(text, GRID_LABEL_SIZE);

        // Clamp X position so labels stay visible at screen edges
        float label_x = Clamp(origin_screen.x, GRID_LABEL_CLAMP_OFFSET + text_width + GRID_LABEL_OFFSET,
                              (float)WIDTH + GRID_LABEL_OFFSET - GRID_LABEL_CLAMP_OFFSET);

        // Draw labels at clamped X position and line Y position
        DrawText(text, (int)label_x - text_width - (int)GRID_LABEL_OFFSET, (int)pos_screen.y - GRID_LABEL_SIZE / 2,
                 GRID_LABEL_SIZE, COLOR_BRIGHT_WHITE);
    }
}

void plot_function(Camera2D *camera, Node *expression, SymbolTable *symbol_table, Color color) {
    ViewContext ctx = get_view_context(camera);

    // Get sampling pixel step
    int samples = WIDTH;
    float pixel_step = (ctx.max.x - ctx.min.x) / samples;

    Vector2 previous_point = {0};
    bool has_previous = false;

    for (int i = 0; i <= samples; i++) {
        // Get X coordinate and update symbol table
        float x_world = ctx.min.x + (i * pixel_step);
        float x_math = pixels_to_math(x_world);
        symbol_table_set(symbol_table, "x", 1, x_math);

        // Evaluate expression
        float y_math = (float)env_evaluate(expression, symbol_table);
        if (isnan(y_math) || isinf(y_math)) {
            has_previous = false;
            continue;
        }

        float y_world = math_to_pixels(-y_math);
        Vector2 current_point = {x_world, y_world};

        if (has_previous) {
            // Connect previous and current point unless an asymptote is present
            if (fabsf(current_point.y - previous_point.y) < ASYMPTOTE_THRESHOLD)
                DrawLineEx(previous_point, current_point, LINE_THICKNESS / camera->zoom, color);
        }

        previous_point = current_point;
        has_previous = true;
    }
}

void display_coords(Camera2D *camera, bool over_legend) {
    // Skip if hovering over legend box
    if (over_legend) return;

    // Get mouse position in math units
    Vector2 mouse_screen = GetMousePosition();
    Vector2 mouse_world = GetScreenToWorld2D(mouse_screen, *camera);
    Vector2 mouse_math = {pixels_to_math(mouse_world.x), pixels_to_math(-mouse_world.y)};

    // Compute text size and position
    const char *text = TextFormat("(%.4f, %.4f)", mouse_math.x, mouse_math.y);
    int text_width = MeasureText(text, COORDS_DISPLAY_SIZE);
    Vector2 text_pos = Vector2Add(mouse_screen, (Vector2){-text_width / 2.0f, -COORDS_DISPLAY_OFFSET});

    DrawTextEx(GetFontDefault(), text, text_pos, COORDS_DISPLAY_SIZE, COORDS_DISPLAY_SPACING, COLOR_BRIGHT_WHITE);
}