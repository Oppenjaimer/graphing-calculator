#pragma once

#include "plotter.hpp"

struct GUIConfig {
    // Panel
    int panel_x = 10;
    int panel_y = 10;
    int legend_spacing_top = 5;
    size_t legend_label_length = 30;

    // Cursor coordinates
    int coords_size = 20;
    float coords_offset = 25.0f;
    Color coords_color = theme::fg0;
};

class GUI {
public:
    explicit GUI(Plotter& plotter, const GUIConfig& config = GUIConfig());

    void render();
    void display_coords(const Camera2D& camera, bool over_panel);

private:
    GUIConfig config{};

    Plotter& plotter;

    std::string input_str;
};
