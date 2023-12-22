#pragma once

#include <string>
#include "ui/base/element.h"
#include "ui/base/button.h"
#include "ui/base/checkbox.h"
#include "ui/base/collapsing_header.h"
#include "ui/base/combo.h"
#include "ui/base/menu_bar.h"
#include "ui/base/slider.h"
#include "ui/base/spacing.h"
#include "ui/base/text.h"
#include "ui/base/text_unformatted.h"
#include "ui/components/file_selector.h"
#include "ui/components/string_view.h"
#include "menu_item.h"

namespace ui {
    ELEMENT(Child) {
        std::string id;
        ImVec2 size = ImVec2(0, 0);
        bool border = false;
        ImGuiWindowFlags flags = 0;

        bool beginDraw() override{
            return ImGui::BeginChild(id.c_str(), size, border, flags);
        }

        void endDraw() override{
            ImGui::EndChild();
        }

        COMMON_ELEMENTS();
    };
}
