#pragma once

#include <vector>
#include <string>
#include <functional>
#include "ui/base/element.h"

namespace ui {
    ELEMENT(Text) {
        std::string content;

        bool isValid() override {
            return !content.empty();
        }

        void draw() override {
            ImGui::Text("%s", content.c_str());
        }
    };
}
