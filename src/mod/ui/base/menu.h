#pragma once

#include <string>
#include "ui/base/element.h"
#include "menu_item.h"

namespace ui {
    ELEMENT(Menu) {
        std::string label;
        bool enabled = true;

        bool isValid()
        override{
                return !label.empty();
        }

        bool beginDraw()
        override{
                return ImGui::BeginMenu(label.c_str(), enabled);
        }

        void endDraw()
        override{
                ImGui::EndMenu();
        }

        ELEMENT_SUPPORTS_CHILD(MenuItem)
    };
}
