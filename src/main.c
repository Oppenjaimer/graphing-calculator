#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "config.h"
#include "draw.h"
#include "gui.h"
#include "update.h"

int main(int argc, char **argv) {
    // Check number of args
    if (argc < 2) {
        fprintf(stderr, "Usage: %s EXPRESSION [EXPRESSION...]\n", argv[0]);
        return 1;
    }

    // Set up parser and environment
    Parser parser = parser_init();
    SymbolTable symbol_table = symbol_table_init();
    symbol_table_set(&symbol_table, "x", 1, 0.0);

    // Allocate array of parsed expressions and color pool
    ParsedExpression *parsed = malloc(sizeof(ParsedExpression) * (argc - 1));
    Color colors[] = {COLOR_BLUE, COLOR_CYAN, COLOR_GREEN, COLOR_RED, COLOR_YELLOW, COLOR_PURPLE};

    // Store parsed expressions
    for (int i = 1; i < argc; i++) {
        Node *root = parser_parse(&parser, argv[i]);
        Color color = colors[(i - 1) % (sizeof(colors) / sizeof(Color))];
        parsed[i - 1] = (ParsedExpression){argv[i], root, color};
    }

    // Initialization
    SetTraceLogCallback(custom_trace_log);
    InitWindow(WIDTH, HEIGHT, "Graphing Calculator");
    SetTargetFPS(FPS);
    SetMouseCursor(MOUSE_CURSOR_CROSSHAIR);

    // Set resources directory and app icon
    char *resource_dir = "resources";
    bool found = set_resource_dir(resource_dir);
    if (found) {
        Image icon = LoadImage("icon.png");
        SetWindowIcon(icon);
    } else {
        TraceLog(LOG_WARNING, "Unable to find '%s' directory", resource_dir);
    }

    // Create camera
    Camera2D camera = {0};
    camera_reset(&camera);

    // Main loop
    while (!WindowShouldClose()) {
        /* --------------------------------- Update --------------------------------- */
        pan(&camera);
        zoom(&camera);
        shortcuts(&camera);

        // Increase/Decrease adaptive spacing when zooming out/in
        float dynamic_spacing = GRID_INITIAL_SPACING;
        while (dynamic_spacing * camera.zoom < GRID_MIN_SPACING) dynamic_spacing *= 2.0f;
        while (dynamic_spacing * camera.zoom > GRID_MAX_SPACING) dynamic_spacing /= 2.0f;

        /* ---------------------------------- Draw ---------------------------------- */
        BeginDrawing();
        ClearBackground(COLOR_BLACK);

        // World space
        BeginMode2D(camera);
        draw_grid(&camera, dynamic_spacing);

        for (int i = 0; i < argc - 1; i++) {
            if (parsed[i].root == NULL) continue;
            plot_function(&camera, parsed[i].root, &symbol_table, parsed[i].color);
        }

        EndMode2D();

        // Screen space
        draw_grid_labels(&camera, dynamic_spacing);
        display_legend(parsed, argc - 1);
        display_coords(&camera);

        EndDrawing();
    }

    // Cleanup
    CloseWindow();
    symbol_table_free(&symbol_table);
    parser_free(&parser);
    free(parsed);

    return 0;
}
