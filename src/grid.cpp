#include "grid.hpp"

#include <cmath>

void AdaptiveGrid::draw(const Camera2D& camera) const {
    float screen_width = static_cast<float>(GetScreenWidth());
    float screen_height = static_cast<float>(GetScreenHeight());

    Vector2 top_left = GetScreenToWorld2D({0.0f, 0.0f}, camera);
    Vector2 bottom_right = GetScreenToWorld2D({screen_width, screen_height}, camera);

    float world_spacing = config.spacing / camera.zoom;

    // Snap to nearest base-10 order of magnitude
    float order = std::floor(std::log10(world_spacing));
    float minor_step = std::pow(10.0f, order);

    // Smooth fading between 1.0 (approaching next level) and 10.0 (just reached current level)
    float transition = world_spacing / minor_step;
    float fade = 1.0f - std::log10(transition);

    Color minor_color = config.minor_color;
    minor_color.a = static_cast<unsigned char>(255 * fade);

    float axis_thickness = config.axis_thickness / camera.zoom;
    float major_thickness = config.major_thickness / camera.zoom;
    float minor_thickness = config.minor_thickness / camera.zoom;

    int start_x = static_cast<int>(std::floor(top_left.x / minor_step));
    int end_x   = static_cast<int>(std::ceil(bottom_right.x / minor_step));
    int start_y = static_cast<int>(std::floor(top_left.y / minor_step));
    int end_y   = static_cast<int>(std::ceil(bottom_right.y / minor_step));

    // Helper lambda for drawing lines
    auto draw_lines = [&](int start, int end, bool is_vertical, float thickness, Color color, auto condition) {
        for (int i = start; i <= end; i++) {
            if (!condition(i)) continue;

            float pos = i * minor_step;
            if (is_vertical) DrawLineEx({pos, top_left.y}, {pos, bottom_right.y}, thickness, color);
            else DrawLineEx({top_left.x, pos}, {bottom_right.x, pos}, thickness, color);
        }
    };

    // Draw lines in order (minor, major, axis)
    auto is_minor = [](int i) { return i % 10 != 0; };
    draw_lines(start_x, end_x, true, minor_thickness, minor_color, is_minor);
    draw_lines(start_y, end_y, false, minor_thickness, minor_color, is_minor);

    auto is_major = [](int i) { return i % 10 == 0 && i != 0; };
    draw_lines(start_x, end_x, true, major_thickness, config.major_color, is_major);
    draw_lines(start_y, end_y, false, major_thickness, config.major_color, is_major);

    auto is_axis = [](int i) { return i == 0; };
    draw_lines(start_x, end_x, true, axis_thickness, config.axis_color, is_axis);
    draw_lines(start_y, end_y, false, axis_thickness, config.axis_color, is_axis);
}
