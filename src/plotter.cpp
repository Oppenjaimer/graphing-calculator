#include "plotter.hpp"

#include <cmath>

void Plotter::parse(std::string_view expression) {
    te_parser parser;
    parser.set_variables_and_functions({{"x", &x}});

    bool valid = parser.compile(expression);
    Color color = colors[parsed_expressions.size() % colors.size()];

    parsed_expressions.push_back({std::move(parser), color, valid, valid});
}

void Plotter::plot(const Camera2D& camera) {
    float screen_width = static_cast<float>(GetScreenWidth());
    float screen_height = static_cast<float>(GetScreenHeight());

    float x_min = GetScreenToWorld2D({0.0f, 0.0f}, camera).x;
    float x_max = GetScreenToWorld2D({screen_width, 0.0f}, camera).x;

    double step = 1.0 / camera.zoom; // Step of 1 px width in world space
    float thickness = config.thickness / camera.zoom;
    float asymptote_threshold = screen_height / camera.zoom;

    for (auto& expr : parsed_expressions) {
        if (!expr.visible) continue;

        bool has_previous = false;
        Vector2 previous_point = {0.0f, 0.0f};

        for (double x = x_min; x <= x_max; x += step) {
            this->x = x;
            double y = expr.parser.evaluate();

            // Check division by zero, complex numbers, etc.
            if (std::isnan(y) || std::isinf(y)) {
                has_previous = false;
                continue;
            }

            Vector2 current_point = {static_cast<float>(x), static_cast<float>(-y)};

            if (has_previous) {
                float y_diff = std::abs(current_point.y - previous_point.y);

                if (y_diff < asymptote_threshold) {
                    // Normal continuous line
                    DrawLineEx(previous_point, current_point, thickness, expr.color);
                } else {
                    // Distinguish between steep line and asymptote by checking midpoint
                    double mid_x = (previous_point.x + x) / 2.0;
                    this->x = mid_x;
                    double mid_y = expr.parser.evaluate();

                    // Check if midpoint has a singularity
                    if (std::isnan(mid_y) || std::isinf(mid_y)) {
                        has_previous = false;
                        continue;
                    }

                    float mid_y_actual = static_cast<float>(-mid_y);
                    float mid_y_expected = (previous_point.y + current_point.y) / 2.0f;

                    // If midpoint is close to the expected value, draw the line
                    if (std::abs(mid_y_actual - mid_y_expected) < asymptote_threshold)
                        DrawLineEx(previous_point, current_point, thickness, expr.color);
                    else
                        has_previous = false;
                }
            }

            previous_point = current_point;
            has_previous = true;
        }
    }
}
