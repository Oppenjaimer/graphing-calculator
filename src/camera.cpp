#include "camera.hpp"
#include "raymath.h" // Include after raylib.h

#include <algorithm>
#include <cmath>

InteractiveCamera::InteractiveCamera(const CameraConfig& config) : config(config) {
    reset();
}

void InteractiveCamera::reset() {
    camera.target = config.initial_target;
    camera.rotation = config.initial_rotation;
    camera.zoom = config.initial_zoom;
    camera.offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
}

void InteractiveCamera::update(float dt, bool has_focus) {
    if (!has_focus) return;

    // Always center camera offset in case of window resizing
    camera.offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};

    handle_reset();
    handle_pan(dt);
    handle_zoom(dt);
}

void InteractiveCamera::pan_delta(Vector2 delta, bool is_keyboard) {
    // Mouse drag needs inversion, keyboard does not
    float sign = is_keyboard ? 1.0f : -1.0f;
    float sensitivity = is_keyboard ? config.pan_speed_keyboard : config.pan_sensitivity_mouse;
    delta = Vector2Scale(delta, sign * sensitivity / camera.zoom);
    camera.target = Vector2Add(camera.target, delta);
}

void InteractiveCamera::zoom_log_scaling(float value, bool is_keyboard) {
    float sensitivity = is_keyboard ? config.zoom_speed_keyboard : config.zoom_sensitivity_mouse;
    float zoom = std::exp(std::log(camera.zoom) + sensitivity * value);
    camera.zoom = std::clamp(zoom, config.zoom_min, config.zoom_max);
}

void InteractiveCamera::handle_reset() {
    if (IsKeyPressed(config.key_reset))
        reset();
}

void InteractiveCamera::handle_pan(float dt) {
    // Mouse
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        pan_delta(GetMouseDelta(), false);

    // Keyboard
    Vector2 delta = {0.0f, 0.0f};
    if (IsKeyDown(config.key_pan_up))    delta.y -= 1.0f;
    if (IsKeyDown(config.key_pan_down))  delta.y += 1.0f;
    if (IsKeyDown(config.key_pan_left))  delta.x -= 1.0f;
    if (IsKeyDown(config.key_pan_right)) delta.x += 1.0f;

    if (delta.x != 0.0f || delta.y != 0.0f) {
        delta = Vector2Normalize(delta);
        delta = Vector2Scale(delta, dt);
        pan_delta(delta, true);
    }
}

void InteractiveCamera::handle_zoom(float dt) {
    // Mouse
    float wheel = GetMouseWheelMove();
    if (wheel != 0.0f) {
        Vector2 mouse_world_before = GetScreenToWorld2D(GetMousePosition(), camera);
        zoom_log_scaling(wheel, false);
        Vector2 mouse_world_after = GetScreenToWorld2D(GetMousePosition(), camera);

        // Adjust camera target to keep mouse point fixed
        Vector2 delta = Vector2Subtract(mouse_world_before, mouse_world_after);
        camera.target = Vector2Add(camera.target, delta);
    }

    // Keyboard
    float value = 0.0f;
    if (IsKeyDown(config.key_zoom_out)) value -= 1.0f;
    if (IsKeyDown(config.key_zoom_in))  value += 1.0f;

    if (value != 0.0f)
        zoom_log_scaling(value * dt, true);
}
