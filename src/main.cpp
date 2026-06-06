#include "raylib.h"

#include "camera.hpp"
#include "config.hpp"

bool set_resource_dir(const char* path) {
    // Check current directory
    if (DirectoryExists(path)) {
        ChangeDirectory(TextFormat("%s/%s", GetWorkingDirectory(), path));
        return true;
    }

    const char* app_dir = GetApplicationDirectory();

    // Check app directory
    const char* dir = TextFormat("%s%s", app_dir, path);
    if (DirectoryExists(dir)) {
        ChangeDirectory(dir);
        return true;
    }

    // Check one level up from app directory
    dir = TextFormat("%s../%s", app_dir, path);
    if (DirectoryExists(dir)) {
        ChangeDirectory(dir);
        return true;
    }

    // Check two levels up from app directory
    dir = TextFormat("%s../../%s", app_dir, path);
    if (DirectoryExists(dir)) {
        ChangeDirectory(dir);
        return true;
    }

    return false;
}

int main() {
    // Initialize raylib
    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(config::screen_width, config::screen_height, config::title);
    SetTargetFPS(config::fps);

    // Set resources directory and app icon
    bool found = set_resource_dir("resources");
    if (found) {
        Image icon = LoadImage("icon.png");
        SetWindowIcon(icon);
        UnloadImage(icon);
    } else {
        TraceLog(LOG_WARNING, "Unable to set resources directory");
    }

    InteractiveCamera camera;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // Update
        camera.update(dt);

        // Draw
        BeginDrawing();
        ClearBackground(theme::bg0);

        // World space
        camera.begin();
        DrawCircle(0, 0, 10, theme::green);
        camera.end();

        // Screen space
        // ...

        EndDrawing();
    }

    // Cleanup
    CloseWindow();

    return 0;
}
