#pragma once

#include <string>
#include <functional>
#include "ui/base/element.h"

namespace ui {
    ELEMENT(Selectable) {
        std::string label;
        std::function<void()> onSelected;
        ImGuiSelectableFlags flags = ImGuiSelectableFlags_None;

        bool isValid() override{
            return !label.empty();
        }

        void draw() override {
            if (ImGui::Selectable(label.c_str(), false, flags)) {
                if (onSelected) {
                    onSelected();
                }
            }
        }
    };
}
