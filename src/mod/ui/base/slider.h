#pragma once

#include <vector>
#include <string>
#include <functional>
#include "ui/base/element.h"

namespace ui {
    ELEMENT(SliderFloat) {
        std::string label;
        float min, max, value;
        std::function<void(float)> onChange;

        bool isValid() override {
            return !label.empty() && (min <= value) && (value <= max) && onChange != nullptr;
        }

        void render() override {
            if (ImGui::SliderFloat(label.c_str(), &value, min, max)) {
                onChange(value);
            }
        }
    };

    ELEMENT(SliderInt) {
        std::string label;
        int min, max, value;
        std::function<void(int)> onChange;

        bool isValid() override {
            return !label.empty() && (min <= value) && (value <= max) && onChange != nullptr;
        }

        void render() override {
            if (ImGui::SliderInt(label.c_str(), &value, min, max)) {
                onChange(value);
            }
        }
    };
}
