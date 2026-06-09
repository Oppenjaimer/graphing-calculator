#include "plotter.hpp"

#include <cmath>

bool Plotter::parse(std::string_view expression) {
    te_parser parser;
    parser.set_variables_and_functions({{"x", &x}});

    if (parser.compile(expression)) {
        Color color = colors[parsed_expressions.size() % colors.size()];
        parsed_expressions.push_back({std::move(parser), color});

        return true;
    }

    return false;
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
                // Avoid asymptotes
                if (std::abs(current_point.y - previous_point.y) >= asymptote_threshold)
                    has_previous = false;
                else
                    DrawLineEx(previous_point, current_point, thickness, expr.color);
            }

            previous_point = current_point;
            has_previous = true;
        }
    }
}
