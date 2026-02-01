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

void draw_grid() {
    // Grid lines half-length
    float limit = (GRID_LINES / 2.0f) * GRID_SPACING;

    for (int i = -GRID_LINES / 2; i <= GRID_LINES / 2; i++) {
        float pos = i * GRID_SPACING;

        // Vertical and horizontal lines
        DrawLine(pos, -limit, pos, limit, GRID_COLOR);
        DrawLine(-limit, pos, limit, pos, GRID_COLOR);
    }
}

void draw_axes() {
    // Draw X and Y axes
    DrawLine(-AXIS_LENGTH, 0, AXIS_LENGTH, 0, COLOR_BRIGHT_WHITE);
    DrawLine(0, -AXIS_LENGTH, 0, AXIS_LENGTH, COLOR_BRIGHT_WHITE);
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
        draw_grid();
        draw_axes();
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