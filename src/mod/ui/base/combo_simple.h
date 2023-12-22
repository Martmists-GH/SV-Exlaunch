#pragma once

#include <string>
#include "ui/base/element.h"
#include "selectable.h"

namespace ui {
    ELEMENT(ComboSimple) {
        std::string label;
        const char* const* items;
        int items_count;
        int selected = 0;
        std::function<void(int)> onChange;
        ImGuiComboFlags flags = ImGuiComboFlags_None;

        bool isValid() override{
            return !label.empty() && items != nullptr && items_count > 0;
        }

        void draw() override {
            if (ImGui::Combo(label.c_str(), &selected, items, items_count)) {
                if (onChange) {
                    onChange(selected);
                }
            }
        }
    };
}
