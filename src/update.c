#include <raylib.h>
#include <stdbool.h>

#include "common.h"
#include "update.h"

static void pan_delta(Camera2D *camera, Vector2 delta, bool shortcut) {
    // Pan by given delta vector
    float sensitivity = shortcut ? PAN_SHORTCUT_SENSITIVITY : PAN_SENSITIVITY;
    delta = Vector2Scale(delta, -sensitivity / camera->zoom);
    camera->target = Vector2Add(camera->target, delta);
}

static void zoom_log_scaling(Camera2D *camera, float value, bool shortcut) {
    // Zoom in/out with log scaling
    float sensitivity = shortcut ? ZOOM_SHORTCUT_SENSITIVITY : ZOOM_SENSITIVITY;
    float scale = sensitivity * value;
    camera->zoom = Clamp(expf(logf(camera->zoom) + scale), ZOOM_MIN_LIMIT, ZOOM_MAX_LIMIT);
}

void camera_reset(Camera2D *camera) {
    camera->zoom = CAMERA_INITIAL_ZOOM;
    camera->rotation = CAMERA_INITIAL_ROTATION;
    camera->target = CAMERA_INITIAL_TARGET;
    camera->offset = CAMERA_OFFSET_CENTER;
}

void pan(Camera2D *camera) {
    // Left click to pan
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        pan_delta(camera, GetMouseDelta(), false);
    }
}

void zoom(Camera2D *camera) {
    // Mouse wheel to zoom
    float wheel = GetMouseWheelMove();
    if (wheel == 0) return;

    // Make cursor point match in screen space and world space
    Vector2 mouse_world = GetScreenToWorld2D(GetMousePosition(), *camera);
    camera->offset = GetMousePosition();
    camera->target = mouse_world;

    zoom_log_scaling(camera, wheel, false);
}

void shortcuts(Camera2D *camera) {
    /* ---------------------------------- Pan ----------------------------------- */
    Vector2 delta = {0, 0};
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))    delta.y = 1;  // Arrow up / W
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))  delta.y = -1; // Arrow down / S
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))  delta.x = 1;  // Arrow left / A
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) delta.x = -1; // Arrow right / D

    pan_delta(camera, delta, true);

    /* ---------------------------------- Zoom ---------------------------------- */
    Vector2 center_screen = {WIDTH / 2.0f, HEIGHT / 2.0f};
    Vector2 center_world = GetScreenToWorld2D(center_screen, *camera);
    camera->offset = center_screen;
    camera->target = center_world;

    float zoom = 0.0f;
    if (IsKeyDown(KEY_SLASH))         zoom = -1.0f; // Minus (-)
    if (IsKeyDown(KEY_RIGHT_BRACKET)) zoom = 1.0f;  // Plus (+)

    zoom_log_scaling(camera, zoom, true);

    /* --------------------------------- Reset ---------------------------------- */
    if (IsKeyPressed(KEY_SPACE)) {
        camera_reset(camera);
    }
}