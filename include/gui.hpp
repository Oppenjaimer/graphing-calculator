#pragma once

struct GUIConfig {
    int legend_x = 10;
    int legend_y = 10;
};

class GUI {
public:
    explicit GUI(const GUIConfig& config = GUIConfig());

    void update();
    void draw() const;

private:
    GUIConfig config{};
};
