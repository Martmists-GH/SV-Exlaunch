#pragma once

#include "element.h"

namespace ui {
    ELEMENT(InputInt) {
        std::string label;
        int value;
        int min = 0;
        int max = 100;
        int step = 1;
        int stepFast = 10;
        std::function<void(int)> onValueChanged;

        bool isValid() override {
            return !label.empty();
        }

        void draw() override {
            if (ImGui::InputInt(label.c_str(), &value, step, stepFast, ImGuiInputTextFlags_None)) {
                if (value < min) {
                    value = min;
                } else if (value > max) {
                    value = max;
                }

                if (onValueChanged) {
                    onValueChanged(value);
                }
            }
        }
    };
}
