#include "common.h"

int main() {
    InitWindow(WIDTH, HEIGHT, "Graphing Calculator");
    SetTargetFPS(FPS);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(COLOR_BLACK);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}