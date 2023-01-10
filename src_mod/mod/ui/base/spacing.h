#pragma once

#include "ui/base/element.h"

namespace ui {
    ELEMENT(Spacing) {
        void draw() override {
            ImGui::Spacing();
        }
    };
};
