#pragma once

#include "camera.hpp"
#include "grid.hpp"
#include "plotter.hpp"
#include "gui.hpp"

struct AppConfig {
    // Window
    int fps = 60;
    int screen_width = 1280;
    int screen_height = 720;
    const char* title = "Graphing Calculator";

    // Colors
    Color bg_color = theme::bg0;
};

class App {
public:
    explicit App(const AppConfig& config = AppConfig());
    ~App();

    void run(int argc, char** argv);

private:
    AppConfig config{};

    InteractiveCamera camera{};
    AdaptiveGrid grid{};
    Plotter plotter{};
    GUI gui{plotter};

    ImGuiIO io{};

    bool running = true;

    void update(float dt);
    void draw();

    bool set_resource_dir(const char* path);
};
