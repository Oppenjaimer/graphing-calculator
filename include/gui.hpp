#pragma once

#include "plotter.hpp"

struct GUIConfig {
    int legend_x = 10;
    int legend_y = 10;
};

class GUI {
public:
    explicit GUI(Plotter& plotter, const GUIConfig& config = GUIConfig());

    void render();

private:
    GUIConfig config{};

    Plotter& plotter;
};
