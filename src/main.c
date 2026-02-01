#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdio.h>

#include "common.h"

/* -------------------------------------------------------------------------- */
/*                                   CONFIG                                   */
/* -------------------------------------------------------------------------- */

void custom_trace_log(int msgType, const char *text, va_list args) {
    // Prepend message type
    switch (msgType) {
        case LOG_INFO:    printf("[INFO]    "); break;
        case LOG_ERROR:   printf("[ERROR]   "); break;
        case LOG_WARNING: printf("[WARNING] "); break;
        case LOG_DEBUG:   printf("[DEBUG]   "); break;
        default: break;
    }

    // Print log message
    vprintf(text, args);
    printf("\n");
}

bool set_resource_dir(const char* folder_name) {
    // Check current directory
    if (DirectoryExists(folder_name)) {
        ChangeDirectory(TextFormat("%s/%s", GetWorkingDirectory(), folder_name));
        return true;
    }

    const char* app_dir = GetApplicationDirectory();

    // Check app directory
    const char* dir = TextFormat("%s%s", app_dir, folder_name);
    if (DirectoryExists(dir)) {
        ChangeDirectory(dir);
        return true;
    }

    // Check one level up from app directory
    dir = TextFormat("%s../%s", app_dir, folder_name);
    if (DirectoryExists(dir)) {
        ChangeDirectory(dir);
        return true;
    }

    // Check two levels up from app directory
    dir = TextFormat("%s../../%s", app_dir, folder_name);
    if (DirectoryExists(dir)) {
        ChangeDirectory(dir);
        return true;
    }

    return false;
}

/* -------------------------------------------------------------------------- */
/*                                   UPDATE                                   */
/* -------------------------------------------------------------------------- */

void translate(Camera2D *camera) {
    // Left click to translate
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        Vector2 delta = GetMouseDelta();
        delta = Vector2Scale(delta, -DRAG_SENSITIVITY/camera->zoom);
        camera->target = Vector2Add(camera->target, delta);
    }
}

void zoom(Camera2D *camera) {
    // Mouse wheel to zoom
    float wheel = GetMouseWheelMove();
    if (wheel == 0) return;

    // Make cursor point match in screen space and world space
    Vector2 mouse_world_pos = GetScreenToWorld2D(GetMousePosition(), *camera);
    camera->offset = GetMousePosition();
    camera->target = mouse_world_pos;

    // Zoom in/out with log scaling
    float scale = ZOOM_SENSITIVITY * wheel;
    camera->zoom = Clamp(expf(logf(camera->zoom) + scale), ZOOM_MIN_LIMIT, ZOOM_MAX_LIMIT);
}

/* -------------------------------------------------------------------------- */
/*                                    DRAW                                    */
/* -------------------------------------------------------------------------- */

void draw_grid(Camera2D *camera) {
    // Get screen vertices in world space
    Vector2 screen_top_left = GetScreenToWorld2D((Vector2){0, 0}, *camera);
    Vector2 screen_bottom_right = GetScreenToWorld2D((Vector2){WIDTH, HEIGHT}, *camera);

    // Compute start and end points for grid lines
    float start_x = floorf(screen_top_left.x / GRID_SPACING) * GRID_SPACING;
    float end_x = ceilf(screen_bottom_right.x / GRID_SPACING) * GRID_SPACING;
    float start_y = floorf(screen_top_left.y / GRID_SPACING) * GRID_SPACING;
    float end_y = ceilf(screen_bottom_right.y / GRID_SPACING) * GRID_SPACING;

    // Draw vertical lines
    for (float x = start_x; x <= end_x; x += GRID_SPACING) {
        bool is_axis = (x == 0);
        DrawLine(x, screen_top_left.y, x, screen_bottom_right.y, is_axis ? COLOR_BRIGHT_WHITE : GRID_COLOR);
    }

    // Draw horizontal lines
    for (float y = start_y; y <= end_y; y += GRID_SPACING) {
        bool is_axis = (y == 0);
        DrawLine(screen_top_left.x, y, screen_bottom_right.x, y, is_axis ? COLOR_BRIGHT_WHITE : GRID_COLOR);
    }
}

/* -------------------------------------------------------------------------- */
/*                                    MAIN                                    */
/* -------------------------------------------------------------------------- */

int main() {
    // Initialization
    SetTraceLogCallback(custom_trace_log);
    InitWindow(WIDTH, HEIGHT, "Graphing Calculator");
    SetTargetFPS(FPS);

    // Set resources directory
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
    camera.zoom = CAMERA_INITIAL_ZOOM;
    camera.rotation = CAMERA_INITIAL_ROTATION;
    camera.target = CAMERA_INITIAL_TARGET;
    camera.offset = CAMERA_OFFSET_CENTER;

    // Main loop
    while (!WindowShouldClose()) {
        /* --------------------------------- Update --------------------------------- */
        translate(&camera);
        zoom(&camera);

        /* ---------------------------------- Draw ---------------------------------- */
        BeginDrawing();
        ClearBackground(COLOR_BLACK);

        // World space
        BeginMode2D(camera);
        draw_grid(&camera);
        DrawCircle(0, 0, 10, COLOR_RED);
        EndMode2D();

        // Screen space
        // ...

        EndDrawing();
    }

    // Cleanup
    CloseWindow();
    return 0;
}