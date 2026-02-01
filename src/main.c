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

void draw_grid(Camera2D *camera, float dynamic_spacing) {
    // Get screen vertices in world space
    Vector2 screen_top_left = GetScreenToWorld2D((Vector2){0, 0}, *camera);
    Vector2 screen_bottom_right = GetScreenToWorld2D((Vector2){WIDTH, HEIGHT}, *camera);

    // Compute start and end points for grid lines
    float start_x = floorf(screen_top_left.x / dynamic_spacing) * dynamic_spacing;
    float end_x   = ceilf(screen_bottom_right.x / dynamic_spacing) * dynamic_spacing;
    float start_y = floorf(screen_top_left.y / dynamic_spacing) * dynamic_spacing;
    float end_y   = ceilf(screen_bottom_right.y / dynamic_spacing) * dynamic_spacing;

    // Apply grid render buffer to coordinates
    start_x -= GRID_RENDER_BUFFER * dynamic_spacing;
    end_x   += GRID_RENDER_BUFFER * dynamic_spacing;
    start_y -= GRID_RENDER_BUFFER * dynamic_spacing;
    end_y   += GRID_RENDER_BUFFER * dynamic_spacing;

    // Draw vertical lines
    for (float x = start_x; x <= end_x; x += dynamic_spacing) {
        // Make major lines brighter
        Color color = COLOR_BRIGHT_WHITE;
        int line_index = (int)lroundf(x / dynamic_spacing);
        color.a = (line_index % GRID_MAJOR_STEP == 0) ? GRID_MAJOR_OPACITY : GRID_MINOR_OPACITY;

        // Draw axis or generic vertical line
        bool is_axis = fabsf(x) < GRID_AXIS_THRESHOLD;
        DrawLineV((Vector2){x, screen_top_left.y}, (Vector2){x, screen_bottom_right.y}, is_axis ? COLOR_BRIGHT_WHITE : color);
    }

    // Draw horizontal lines
    for (float y = start_y; y <= end_y; y += dynamic_spacing) {
        // Make major lines brighter
        Color color = COLOR_BRIGHT_WHITE;
        int line_index = (int)lroundf(y / dynamic_spacing);
        color.a = (line_index % GRID_MAJOR_STEP == 0) ? GRID_MAJOR_OPACITY : GRID_MINOR_OPACITY;

        // Draw axis or generic horizontal line
        bool is_axis = fabsf(y) < GRID_AXIS_THRESHOLD;
        DrawLineV((Vector2){screen_top_left.x, y}, (Vector2){screen_bottom_right.x, y}, is_axis ? COLOR_BRIGHT_WHITE : color);
    }
}

void draw_grid_labels(Camera2D *camera, float dynamic_spacing) {
    // Get screen vertices in world space
    Vector2 screen_top_left = GetScreenToWorld2D((Vector2){0, 0}, *camera);
    Vector2 screen_bottom_right = GetScreenToWorld2D((Vector2){WIDTH, HEIGHT}, *camera);

    // Compute start and end points for grid lines
    float start_x = floorf(screen_top_left.x / dynamic_spacing) * dynamic_spacing;
    float end_x   = ceilf(screen_bottom_right.x / dynamic_spacing) * dynamic_spacing;
    float start_y = floorf(screen_top_left.y / dynamic_spacing) * dynamic_spacing;
    float end_y   = ceilf(screen_bottom_right.y / dynamic_spacing) * dynamic_spacing;

    // Get origin position in screen space
    Vector2 origin_pos = GetWorldToScreen2D((Vector2){0, 0}, *camera);

    // Calculate origin label text and its width
    const char *zero_text = "0";
    int zero_width = MeasureText(zero_text, GRID_LABEL_SIZE);

    // Clamp X and Y positions so origin label stays visible at screen edges
    float zero_x = Clamp(origin_pos.x, GRID_LABEL_CLAMP_OFFSET + zero_width + GRID_LABEL_OFFSET,
                         (float)WIDTH + GRID_LABEL_OFFSET - GRID_LABEL_CLAMP_OFFSET);
    float zero_y = Clamp(origin_pos.y, GRID_LABEL_CLAMP_OFFSET - GRID_LABEL_OFFSET,
                         (float)HEIGHT - GRID_LABEL_SIZE - GRID_LABEL_CLAMP_OFFSET);

    // Draw origin label at clamped X and Y positions
    DrawText(zero_text, (int)zero_x - zero_width - (int)GRID_LABEL_OFFSET, (int)zero_y + (int)GRID_LABEL_OFFSET,
        GRID_LABEL_SIZE, COLOR_BRIGHT_WHITE);

    // Draw X-axis labels
    for (float x = start_x; x <= end_x; x += dynamic_spacing) {
        // Check for major grid lines
        int line_index = (int)lroundf(x / dynamic_spacing);
        if (line_index % GRID_MAJOR_STEP != 0 || line_index == 0) continue;

        // Get X position in screen space
        Vector2 world_pos = {x, 0};
        Vector2 screen_pos = GetWorldToScreen2D(world_pos, *camera);

        // Calculate text and its width
        float math_value = x / GRID_INITIAL_SPACING * GRID_UNITS_PER_SPACE;
        const char* text = TextFormat("%g", math_value);
        int text_width = MeasureText(text, GRID_LABEL_SIZE);

        // Clamp Y position so labels stay visible at screen edges
        float label_y = Clamp(origin_pos.y, GRID_LABEL_CLAMP_OFFSET - GRID_LABEL_OFFSET,
                              (float)HEIGHT - GRID_LABEL_SIZE - GRID_LABEL_CLAMP_OFFSET);

        // Draw labels at line X position and clamped Y position
        DrawText(text, (int)screen_pos.x - text_width / 2, (int)label_y + (int)GRID_LABEL_OFFSET,
                 GRID_LABEL_SIZE, COLOR_BRIGHT_WHITE);
    }

    // Draw Y-axis labels
    for (float y = start_y; y <= end_y; y += dynamic_spacing) {
        // Check for major grid lines
        int line_index = (int)lroundf(y / dynamic_spacing);
        if (line_index % GRID_MAJOR_STEP != 0 || line_index == 0) continue;

        // Get Y position in screen space
        Vector2 world_pos = {0, y};
        Vector2 screen_pos = GetWorldToScreen2D(world_pos, *camera);

        // Calculate text and its width
        float math_value = -y / GRID_INITIAL_SPACING * GRID_UNITS_PER_SPACE;
        const char* text = TextFormat("%g", math_value);
        int text_width = MeasureText(text, GRID_LABEL_SIZE);

        // Clamp X  position so labels stay visible at screen edges
        float label_x = Clamp(origin_pos.x, GRID_LABEL_CLAMP_OFFSET + text_width + GRID_LABEL_OFFSET,
                              (float)WIDTH + GRID_LABEL_OFFSET - GRID_LABEL_CLAMP_OFFSET);

        // Draw labels at clamped X position and line Y position
        DrawText(text, (int)label_x - text_width - (int)GRID_LABEL_OFFSET, (int)screen_pos.y - GRID_LABEL_SIZE / 2,
                 GRID_LABEL_SIZE, COLOR_BRIGHT_WHITE);
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
    SetMouseCursor(MOUSE_CURSOR_CROSSHAIR);

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
        EndMode2D();

        // Screen space
        draw_grid_labels(&camera, dynamic_spacing);

        EndDrawing();
    }

    // Cleanup
    CloseWindow();
    return 0;
}