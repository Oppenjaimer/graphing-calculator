#include "imgui.h"
#include "rlImGui.h"

#include "gui.hpp"
#include "theme.hpp"

GUI::GUI(const GUIConfig& config) : config(config) {
    // Initialize ImGui
    rlImGuiSetup(true);
    theme::apply();

    // Disable imgui.ini
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = NULL;
}

void GUI::update() {
    // ...
}

void GUI::draw() const {
    ImGui::Begin("Legend", NULL, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SetWindowPos(ImVec2(config.legend_x, config.legend_y), ImGuiCond_FirstUseEver);

    ImGui::Text("FPS: %d", GetFPS());

    ImGui::End();
}
