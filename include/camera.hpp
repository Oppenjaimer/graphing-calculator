#pragma once

#include "raylib.h"

struct CameraConfig {
    // Defaults
    Vector2 initial_target = {0.0f, 0.0f};
    float initial_rotation = 0.0f;
    float initial_zoom = 1.0f;

    // Sensitivities & speeds
    float pan_sensitivity_mouse = 1.0f;
    float pan_speed_keyboard = 400.0f;
    float zoom_sensitivity_mouse = 0.15f;
    float zoom_speed_keyboard = 4.0f;

    // Limits
    float zoom_min = 0.01f;
    float zoom_max = 100.0f;

    // Keymap
    int key_reset = KEY_SPACE;
    int key_pan_up = KEY_W;
    int key_pan_down = KEY_S;
    int key_pan_left = KEY_A;
    int key_pan_right = KEY_D;
    int key_zoom_in = KEY_E;
    int key_zoom_out = KEY_Q;
};

class InteractiveCamera {
public:
    explicit InteractiveCamera(const CameraConfig& config = CameraConfig());

    void reset();
    void update(float dt, bool has_focus = true);

    void begin() const { BeginMode2D(camera); }
    void end() const { EndMode2D(); };

    Camera2D get_camera() const { return camera; };

private:
    CameraConfig config{};
    Camera2D camera{};

    void pan_delta(Vector2 delta, bool is_keyboard);
    void zoom_log_scaling(float value, bool is_keyboard);

    void handle_reset();
    void handle_pan(float dt);
    void handle_zoom(float dt);
};
