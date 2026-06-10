#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"
#include "misc/cpp/imgui_stdlib.h"
#include "rlImGui.h"
#include "raymath.h"

#include "gui.hpp"
#include "theme.hpp"

namespace ImGui {
    bool SolidCheckbox(const char* label, bool* v, bool valid, ImVec4 color) {
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
        bool value_changed = false;

        if (pressed && valid) {
            *v = !(*v);
            MarkItemEdited(id);
            value_changed = true;
        }

        // Check bounding box
        const ImRect check_bb(pos, pos + ImVec2(side, side));

        RenderNavHighlight(total_bb, id);

        // Empty background frame
        ImU32 frame_color = GetColorU32(
            (!valid) ? ImGuiCol_FrameBg :
            (held && hovered) ? ImGuiCol_FrameBgActive :
            hovered ? ImGuiCol_FrameBgHovered :
            ImGuiCol_FrameBg
        );

        RenderFrame(check_bb.Min, check_bb.Max, frame_color, true, style.FrameRounding);

        // Filled inner square if checked
        if (*v)
            window->DrawList->AddRectFilled(check_bb.Min, check_bb.Max, ImGui::ColorConvertFloat4ToU32(color), style.FrameRounding);

        // Invalid indicator (!)
        if (!valid) {
            const char* mark_text = "!";

            ImFont* font = ImGui::GetFont();
            float font_size = g.FontSize * 1.5f;

            ImVec2 mark_size = font->CalcTextSizeA(font_size, FLT_MAX, 0.0f, mark_text);
            ImVec2 mark_pos = ImVec2(
                check_bb.Min.x + (side - mark_size.x) / 2.0f,
                check_bb.Min.y + (side - mark_size.y) / 2.0f
            );

            ImU32 color = ImGui::ColorConvertFloat4ToU32(theme::to_imvec(theme::bright_red));
            window->DrawList->AddText(font, font_size, mark_pos, color, mark_text);
        }

        // Label text
        if (label_size.x > 0.0f) {
            ImVec2 label_pos = ImVec2(check_bb.Max.x + style.ItemInnerSpacing.x, check_bb.Min.y + style.FramePadding.y);
            RenderText(label_pos, label);
        }

        return value_changed;
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

    // Compute input width
    const char* button_text = edit_idx >= 0 ? "Save" : "Add";
    float button_width = ImGui::CalcTextSize(button_text).x + ImGui::GetStyle().FramePadding.x * 2.0f;
    float input_width = ImGui::GetContentRegionAvail().x - button_width - ImGui::GetStyle().ItemSpacing.x;
    ImGui::SetNextItemWidth(input_width >= config.input_initial_width ? input_width : config.input_initial_width);

    // Input expression
    static bool focus_input = false;
    if (focus_input) {
        ImGui::SetKeyboardFocusHere();
        focus_input = false;
    }

    bool submitted = ImGui::InputTextWithHint("##input", "Expression...", &input_str, ImGuiInputTextFlags_EnterReturnsTrue);

    ImGui::SameLine();
    submitted |= ImGui::Button(button_text);
    if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

    // Add expression to plotter and clear input
    if (submitted) {
        if (!input_str.empty()) {
            if (edit_idx >= 0) {
                // Edit mode
                plotter.update(edit_idx, input_str);
                edit_idx = -1;
            } else {
                // Normal mode
                plotter.parse(input_str);
            }
        }

        input_str.clear();
        focus_input = true;
    }

    // Separator
    auto& entries = plotter.get_expressions();
    if (!entries.empty()) ImGui::InvisibleButton("invisible_separator", ImVec2(1, config.legend_spacing_top));

    if (ImGui::BeginTable("legend_table", 3, ImGuiTableFlags_SizingFixedFit)) {
        int id_counter = 0;

        for (size_t i = 0; i < entries.size(); i++) {
            auto& entry = entries[i];

            // Unique ID for current entry
            ImGui::PushID(id_counter++);
            ImGui::TableNextRow();

            // Column 1: Solid checkbox with visibility toggle
            ImGui::TableNextColumn();

            // Truncate label if necessary
            std::string full_label = entry.expression;
            std::string truncated_label = full_label;
            if (truncated_label.length() > config.legend_label_length)
                truncated_label = truncated_label.substr(0, config.legend_label_length) + "...";

            ImGui::SolidCheckbox(truncated_label.c_str(), &entry.visible, entry.valid, theme::to_imvec(entry.color));
            if (ImGui::IsItemHovered()) {
                ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
                if (full_label.length() > config.legend_label_length)
                    ImGui::SetTooltip("%s", full_label.c_str());
            }

            // Column 2: Edit button
            ImGui::TableNextColumn();
            if (ImGui::Button("Edit")) {
                input_str = entry.expression;
                edit_idx = static_cast<int>(i);
                focus_input = true;
            }
            if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

            // Column 3: Remove button
            ImGui::TableNextColumn();
            if (ImGui::Button("Remove")) entries.erase(entries.begin() + i);
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
