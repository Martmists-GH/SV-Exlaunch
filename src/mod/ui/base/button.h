#pragma once

#include <vector>
#include <string>
#include <functional>
#include "ui/base/element.h"

namespace ui {
    ELEMENT(Button) {
        std::string label;
        std::function<void()> onClick;

        bool isValid()
        override{
                return !label.empty() && onClick;
        }

        void draw()
        override{
                if (ImGui::Button(label.c_str())) {
                    onClick();
                }
        }
    };
}
