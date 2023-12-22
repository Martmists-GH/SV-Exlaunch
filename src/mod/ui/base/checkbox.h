#pragma once

#include <vector>
#include <string>
#include <functional>
#include "ui/base/element.h"

namespace ui {
    ELEMENT(Checkbox) {
        std::string label;
        bool enabled = false;
        std::function<void(bool)> onChange;

        bool isValid() override {
            return !label.empty();
        }

        void draw() override{
            if (ImGui::Checkbox(label.c_str(), &enabled)) {
                if (onChange) {
                    onChange(enabled);
                }
            }
        }
    };
}
