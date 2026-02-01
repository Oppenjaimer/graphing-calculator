#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>

#include "common.h"

void custom_trace_log(int msgType, const char *text, va_list args) {
    switch (msgType) {
        case LOG_INFO:    printf("[INFO]    "); break;
        case LOG_ERROR:   printf("[ERROR]   "); break;
        case LOG_WARNING: printf("[WARNING] "); break;
        case LOG_DEBUG:   printf("[DEBUG]   "); break;
        default: break;
    }

    vprintf(text, args);
    printf("\n");
}

bool set_resource_dir(const char* folder_name) {
    if (DirectoryExists(folder_name)) {
        ChangeDirectory(TextFormat("%s/%s", GetWorkingDirectory(), folder_name));
        return true;
    }

    const char* app_dir = GetApplicationDirectory();

    const char* dir = TextFormat("%s%s", app_dir, folder_name);
    if (DirectoryExists(dir)) {
        ChangeDirectory(dir);
        return true;
    }

    dir = TextFormat("%s../%s", app_dir, folder_name);
    if (DirectoryExists(dir)) {
        ChangeDirectory(dir);
        return true;
    }

    dir = TextFormat("%s../../%s", app_dir, folder_name);
    if (DirectoryExists(dir)) {
        ChangeDirectory(dir);
        return true;
    }

    return false;
}

int main() {
    SetTraceLogCallback(custom_trace_log);
    InitWindow(WIDTH, HEIGHT, "Graphing Calculator");
    SetTargetFPS(FPS);

    char *resource_dir = "resource";
    bool found = set_resource_dir(resource_dir);
    if (found) {
        Image icon = LoadImage("icon.png");
        SetWindowIcon(icon);
    } else {
        TraceLog(LOG_WARNING, "Unable to find '%s' directory", resource_dir);
    }

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(COLOR_BLACK);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}