#pragma once

#include "raylib.h"

#include "theme.hpp"

struct GridConfig {
    // Dimensions
    float spacing = 80.0f;
    float axis_thickness = 1.0f;
    float major_thickness = 0.5f;
    float minor_thickness = 0.5f;
    float label_padding = 10.0f;
    int label_size = 20;
    int subdivisions = 10;

    // Colors
    Color axis_color = theme::fg0;
    Color major_color = theme::fg2;
    Color minor_color = theme::fg4;
};

struct GridMetrics {
    Vector2 top_left;
    Vector2 bottom_right;
    float minor_step;
    float major_step;
    float fade;
};

class AdaptiveGrid {
public:
    explicit AdaptiveGrid(const GridConfig& config = GridConfig())
        : config(config) {}

    void update(const Camera2D& camera);
    void draw_grid(const Camera2D& camera) const;
    void draw_labels(const Camera2D& camera) const;

private:
    GridConfig config{};
    GridMetrics metrics{};
};
