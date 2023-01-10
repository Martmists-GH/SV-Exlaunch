#pragma once

#include "ui/base/element.h"
#include "ui/base/button.h"
#include "ui/base/checkbox.h"
#include "ui/base/combo.h"
#include "ui/base/menu_bar.h"
#include "ui/base/text.h"
#include "ui/base/slider.h"
#include "ui/base/spacing.h"
#include "ui/components/file_selector.h"

namespace ui {
    ELEMENT(CollapsingHeader) {
        std::string label;
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;

        bool isValid() override {
            return !label.empty();
        }

        bool beginDraw() override {
            return ImGui::CollapsingHeader(label.c_str());
        };

        ELEMENT_SUPPORTS_CHILD(Button)
        ELEMENT_SUPPORTS_CHILD(Checkbox)
        ELEMENT_SUPPORTS_CHILD(Combo)
        ELEMENT_SUPPORTS_CHILD(MenuBar)
        ELEMENT_SUPPORTS_CHILD(Text)
        ELEMENT_SUPPORTS_CHILD(SliderFloat)
        ELEMENT_SUPPORTS_CHILD(Spacing)
        ELEMENT_SUPPORTS_CHILD(FileSelector)
    };
}
