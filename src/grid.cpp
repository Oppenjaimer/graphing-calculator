#include "grid.hpp"

#include <algorithm>
#include <cmath>

void AdaptiveGrid::update(const Camera2D& camera) {
    float screen_width = static_cast<float>(GetScreenWidth());
    float screen_height = static_cast<float>(GetScreenHeight());

    Vector2 top_left = GetScreenToWorld2D({0.0f, 0.0f}, camera);
    Vector2 bottom_right = GetScreenToWorld2D({screen_width, screen_height}, camera);

    float world_spacing = config.spacing / camera.zoom;

    // Snap to nearest base-10 order of magnitude
    float order = std::floor(std::log10(world_spacing));
    float magnitude = std::pow(10.0f, order);

    float major_step = magnitude * 10.0f;
    float minor_step = major_step / static_cast<float>(config.subdivisions);

    // Smooth fading between 1.0 (approaching next level) and 10.0 (just reached current level)
    float transition = world_spacing / minor_step;
    float fade = std::clamp(1.0f - std::log10(transition), 0.0f, 1.0f);

    metrics = {top_left, bottom_right, minor_step, major_step, fade};
}

void AdaptiveGrid::draw_grid(const Camera2D& camera) const {
    Color minor_color = config.minor_color;
    minor_color.a = static_cast<unsigned char>(255.0f * metrics.fade);

    float axis_thickness = config.axis_thickness / camera.zoom;
    float major_thickness = config.major_thickness / camera.zoom;
    float minor_thickness = config.minor_thickness / camera.zoom;

    int start_x = static_cast<int>(std::floor(metrics.top_left.x / metrics.minor_step));
    int end_x = static_cast<int>(std::ceil(metrics.bottom_right.x / metrics.minor_step));
    int start_y = static_cast<int>(std::floor(metrics.top_left.y / metrics.minor_step));
    int end_y = static_cast<int>(std::ceil(metrics.bottom_right.y / metrics.minor_step));

    // Helper lambda for drawing lines
    auto draw_lines = [&](int start, int end, bool is_vertical, float thickness, Color color, auto condition) {
        for (int i = start; i <= end; i++) {
            if (!condition(i)) continue;

            float pos = i * metrics.minor_step;
            if (is_vertical) DrawLineEx({pos, metrics.top_left.y}, {pos, metrics.bottom_right.y}, thickness, color);
            else DrawLineEx({metrics.top_left.x, pos}, {metrics.bottom_right.x, pos}, thickness, color);
        }
    };

    // Draw lines in order (minor, major, axis)
    auto is_minor = [&](int i) { return i % config.subdivisions != 0; };
    draw_lines(start_x, end_x, true, minor_thickness, minor_color, is_minor);
    draw_lines(start_y, end_y, false, minor_thickness, minor_color, is_minor);

    auto is_major = [&](int i) { return i % config.subdivisions == 0 && i != 0; };
    draw_lines(start_x, end_x, true, major_thickness, config.major_color, is_major);
    draw_lines(start_y, end_y, false, major_thickness, config.major_color, is_major);

    auto is_axis = [](int i) { return i == 0; };
    draw_lines(start_x, end_x, true, axis_thickness, config.axis_color, is_axis);
    draw_lines(start_y, end_y, false, axis_thickness, config.axis_color, is_axis);
}

void AdaptiveGrid::draw_labels(const Camera2D& camera) const {
    float screen_width = static_cast<float>(GetScreenWidth());
    float screen_height = static_cast<float>(GetScreenHeight());
    Vector2 origin_screen = GetWorldToScreen2D({0.0f, 0.0f}, camera);

    // Origin label
    const char* zero_text = "0";
    int zero_width = MeasureText(zero_text, config.label_size);

    float clamped_zero_x = std::clamp(origin_screen.x, zero_width + 2 * config.label_padding, screen_width - config.label_padding);
    float clamped_zero_y = std::clamp(origin_screen.y, config.label_padding, screen_height - config.label_size - config.label_padding);

    DrawText(
        zero_text,
        static_cast<int>(clamped_zero_x - zero_width - config.label_padding),
        static_cast<int>(clamped_zero_y + config.label_padding),
        config.label_size,
        config.axis_color
    );

    // X-axis labels
    int start_x = static_cast<int>(std::floor(metrics.top_left.x / metrics.major_step));
    int end_x = static_cast<int>(std::ceil(metrics.bottom_right.x / metrics.major_step));
    float label_y = std::clamp(origin_screen.y, config.label_padding, screen_height - config.label_size - config.label_padding);

    for (int i = start_x; i <= end_x; i++) {
        if (i == 0) continue;

        float x_world = i * metrics.major_step;
        Vector2 pos_screen = GetWorldToScreen2D({x_world, 0.0f}, camera);

        const char* text = TextFormat("%g", x_world);
        int text_width = MeasureText(text, config.label_size);

        DrawText(
            text,
            static_cast<int>(pos_screen.x - text_width / 2.0f),
            static_cast<int>(label_y + config.label_padding),
            config.label_size,
            config.axis_color
        );
    }

    // Y-axis labels
    int start_y = static_cast<int>(std::floor(metrics.top_left.y / metrics.major_step));
    int end_y = static_cast<int>(std::ceil(metrics.bottom_right.y / metrics.major_step));

    for (int i = start_y; i <= end_y; i++) {
        if (i == 0) continue;

        float y_world = i * metrics.major_step;
        Vector2 pos_screen = GetWorldToScreen2D({0.0f, y_world}, camera);

        const char* text = TextFormat("%g", -y_world);
        int text_width = MeasureText(text, config.label_size);

        float clamped_x = std::clamp(origin_screen.x, text_width + 2 * config.label_padding, screen_width - config.label_padding);

        DrawText(
            text,
            static_cast<int>(clamped_x - text_width - config.label_padding),
            static_cast<int>(pos_screen.y - config.label_size / 2.0f),
            config.label_size,
            config.axis_color
        );
    }
}
