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
            return !label.empty() && onChange;
        }

        void draw() override{
            if (ImGui::Checkbox(label.c_str(), &enabled)) {
                onChange(enabled);
            }
        }
    };
}
