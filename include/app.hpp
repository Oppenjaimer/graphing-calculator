#pragma once

#include "camera.hpp"
#include "grid.hpp"

struct AppConfig {
    // Window
    int fps = 60;
    int screen_width = 1280;
    int screen_height = 720;
    const char* title = "Graphing Calculator";

    // Colors
    Color bg_color = theme::bg0;
};

struct AppState {
    bool running = true;
};

class App {
public:
    explicit App(const AppConfig& config = AppConfig());
    ~App();

    void run();

private:
    AppConfig config{};
    AppState state{};
    InteractiveCamera camera{};
    AdaptiveGrid grid{};

    void update(float dt);
    void draw() const;

    bool set_resource_dir(const char* path);
};
