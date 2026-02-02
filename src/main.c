#include <stdio.h>

#include "common.h"
#include "config.h"
#include "draw.h"
#include "update.h"

int main(int argc, char **argv) {
    // Check number of args
    if (argc != 2) {
        fprintf(stderr, "Usage: %s EXPRESSION\n", argv[0]);
        return 1;
    }

    // Set up parser and environment
    Parser parser = parser_init();
    SymbolTable symbol_table = symbol_table_init();
    symbol_table_set(&symbol_table, "x", 1, 0.0);

    // Parse expression
    Node *root = parser_parse(&parser, argv[1]);
    if (root == NULL) return 1;

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
        plot_function(&camera, root, &symbol_table);
        EndMode2D();

        // Screen space
        draw_grid_labels(&camera, dynamic_spacing);

        EndDrawing();
    }

    // Cleanup
    CloseWindow();
    symbol_table_free(&symbol_table);
    parser_free(&parser);

    return 0;
}

// TODO: add coordinates above cursor
// TODO: check whether expression is valid after parsing
// TODO: plot multiple functions at once