#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"
#include "misc/cpp/imgui_stdlib.h"
#include "rlImGui.h"
#include "raymath.h"

#include "gui.hpp"
#include "theme.hpp"

namespace ImGui {
    bool SolidCheckbox(const char* label, bool* v) {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        const ImVec2 label_size = CalcTextSize(label, NULL, true);

        // Total bounding box
        const float side = GetFrameHeight();
        const ImVec2 pos = window->DC.CursorPos;
        const ImRect total_bb(
            pos,
            pos + ImVec2(side + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), label_size.y + style.FramePadding.y * 2.0f)
        );

        ItemSize(total_bb, style.FramePadding.y);
        if (!ItemAdd(total_bb, id)) return false;

        // Handle interactions
        bool hovered, held;
        bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);
        if (pressed) {
            *v = !(*v);
            MarkItemEdited(id);
        }

        // Check bounding box
        const ImRect check_bb(pos, pos + ImVec2(side, side));

        RenderNavHighlight(total_bb, id);

        // Empty background frame
        RenderFrame(
            check_bb.Min, check_bb.Max,
            GetColorU32((held && hovered) ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg),
            true, style.FrameRounding
        );

        // Filled inner square if checked
        if (*v) {
            window->DrawList->AddRectFilled(
                check_bb.Min, check_bb.Max,
                GetColorU32(ImGuiCol_CheckMark),
                style.FrameRounding
            );
        }

        // Label text
        if (label_size.x > 0.0f) {
            ImVec2 label_pos = ImVec2(check_bb.Max.x + style.ItemInnerSpacing.x, check_bb.Min.y + style.FramePadding.y);
            RenderText(label_pos, label);
        }

        return pressed;
    }
}

GUI::GUI(Plotter& plotter, const GUIConfig& config) : config(config), plotter(plotter) {
    // Initialize ImGui
    rlImGuiSetup(true);
    theme::apply();

    // Disable imgui.ini
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = NULL;
}

void GUI::render() {
    ImGui::Begin("##panel", NULL, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SetWindowPos(ImVec2(config.panel_x, config.panel_y), ImGuiCond_FirstUseEver);

    // Input expression
    bool submitted = false;
    if (ImGui::InputTextWithHint("##input", "Input expression", &input_str, ImGuiInputTextFlags_EnterReturnsTrue))
        submitted = true;

    ImGui::SameLine();
    if (ImGui::Button("Add"))
        submitted = true;

    if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

    // Add expression to plotter and clear input
    if (submitted) {
        if (!input_str.empty()) plotter.parse(input_str);
        input_str.clear();
    }

    // Separator
    ImGui::InvisibleButton("invisible_separator", ImVec2(1, config.legend_spacing_top));

    if (ImGui::BeginTable("legend_table", 3, ImGuiTableFlags_SizingFixedFit)) {
        int id_counter = 0;

        for (auto& entry : plotter.get_expressions()) {
            // Unique ID for current entry
            ImGui::PushID(id_counter++);
            ImGui::TableNextRow();

            // Column 1: Solid checkbox with visibility toggle
            ImGui::TableNextColumn();
            ImGui::PushStyleColor(ImGuiCol_CheckMark, theme::to_imvec(entry.color));
            ImGui::SolidCheckbox(entry.parser.get_expression().c_str(), &entry.visible);
            if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            ImGui::PopStyleColor();

            // Column 2: Edit button
            ImGui::TableNextColumn();
            ImGui::Button("Edit");
            if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

            // Column 3: Remove button
            ImGui::TableNextColumn();
            ImGui::Button("Remove");
            if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

            ImGui::PopID();
        }

        ImGui::EndTable();
    }

    ImGui::End();
}

void GUI::display_coords(const Camera2D& camera, bool over_panel) {
    if (over_panel) return;

    Vector2 mouse_screen = GetMousePosition();
    Vector2 mouse_world = GetScreenToWorld2D(mouse_screen, camera);

    const char* text = TextFormat("(%.4f, %.4f)", mouse_world.x, mouse_world.y);
    int text_width = MeasureText(text, config.coords_size);
    Vector2 text_pos = Vector2Add(mouse_screen, {-text_width / 2.0f, -config.coords_offset});

    DrawText(text, text_pos.x, text_pos.y, config.coords_size, config.coords_color);
}
