#pragma once

#include <string>
#include "ui/base/element.h"
#include "menu.h"

namespace ui {
    ELEMENT(MenuBar) {
        bool beginDraw()
        override{
                return ImGui::BeginMenuBar();
        }

        void endDraw()
        override{
                return ImGui::EndMenuBar();
        }

        ELEMENT_SUPPORTS_CHILD(Menu)
    };
}
