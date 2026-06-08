#pragma once

#include "raylib.h"

#include "theme.hpp"

struct GridConfig {
    // Dimensions
    float spacing = 100.0f;
    float axis_thickness = 2.0f;
    float major_thickness = 1.5f;
    float minor_thickness = 1.0f;

    // Colors
    Color axis_color = theme::fg0;
    Color major_color = theme::fg2;
    Color minor_color = theme::fg4;
};

class AdaptiveGrid {
public:
    explicit AdaptiveGrid(const GridConfig& config = GridConfig())
        : config(config) {}

    void draw(const Camera2D& camera) const;

private:
    GridConfig config{};
};
