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

        bool isValid()
        override{
                return !label.empty() && (min <= value) && (value <= max) && onChange;
        }

        void render()
        override{
                if (ImGui::SliderFloat(label.c_str(), &value, min, max)) {
                    onChange(value);
                }
        }
    };
}
