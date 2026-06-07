#pragma once

#include "camera.hpp"

struct AppConfig {
    int fps = 60;
    int screen_width = 1280;
    int screen_height = 720;
    const char* title = "Graphing Calculator";
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

    void update(float dt);
    void draw() const;

    bool set_resource_dir(const char* path);
};
