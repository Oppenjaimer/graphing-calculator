#pragma once

#include "raylib.h"
#include "tinyexpr.h"

#include "theme.hpp"

#include <array>
#include <string_view>
#include <vector>

struct ParsedExpression {
    te_parser parser;
    Color color;
    bool visible;
};

struct PlotterConfig {
    float thickness = 2.0f;
};

class Plotter {
public:
    explicit Plotter(const PlotterConfig& config = PlotterConfig())
        : config(config) {}

    bool parse(std::string_view expression);
    void plot(const Camera2D& camera);

    std::vector<ParsedExpression>& get_expressions() { return parsed_expressions; }

private:
    PlotterConfig config{};

    double x = 0.0;

    std::vector<ParsedExpression> parsed_expressions{};

    std::array<Color, 7> colors = {
        theme::blue, theme::aqua, theme::green, theme::red, theme::yellow, theme::purple, theme::orange
    };
};
