#include "imgui.h"
#include "rlImGui.h"
#include "raylib.h"

#include "app.hpp"

App::App(const AppConfig& config) : config(config) {
    // Initialize raylib
    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(config.screen_width, config.screen_height, config.title);
    SetTargetFPS(config.fps);

    // Set resources directory and app icon
    bool found = set_resource_dir("resources");
    if (found) {
        Image icon = LoadImage("icon.png");
        SetWindowIcon(icon);
        UnloadImage(icon);
    } else {
        TraceLog(LOG_WARNING, "Unable to set resources directory");
    }
}

App::~App() {
    rlImGuiShutdown();
    CloseWindow();
}

void App::run(int argc, char** argv) {
    // Parse expressions from arguments
    for (int i = 1; i < argc; i++) {
        if (!plotter.parse(argv[i])) {
            // TODO: support invalid expressions appearing in legend with a warning and being editable
            TraceLog(LOG_WARNING, "Invalid expression: '%s'", argv[i]);
        }
    }

    while (running) {
        float dt = GetFrameTime();
        update(dt);
        draw();
    }
}

void App::update(float dt) {
    running = !WindowShouldClose();

    io = ImGui::GetIO();
    bool gui_focus = io.WantCaptureMouse || io.WantCaptureKeyboard;

    camera.update(dt, !gui_focus);
    grid.update(camera.get_camera());
}

void App::draw() {
    Camera2D cam = camera.get_camera();

    BeginDrawing();
    ClearBackground(config.bg_color);

    // World space
    camera.begin();
    grid.draw_grid(cam);
    plotter.plot(cam);
    camera.end();

    // Screen space
    grid.draw_labels(cam);
    gui.display_coords(cam, io.WantCaptureMouse);

    // GUI
    rlImGuiBegin();
    gui.render();
    rlImGuiEnd();

    EndDrawing();
}

bool App::set_resource_dir(const char* path) {
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
