#pragma once

#include <vector>
#include "ui/base/element.h"
#include "window.h"
#include "helpers.h"
#include "imgui_backend/imgui_impl_nvn.hpp"

namespace ui {
    ELEMENT(Root) {
        bool beginDraw() override {
            InputHelper::updatePadState();
            if (InputHelper::isInputToggled()) {
                ImguiNvnBackend::newFrame();
                ImGui::NewFrame();
                return true;
            }
            return false;
        }

        void endDraw() override {
            ImGui::Render();
            ImguiNvnBackend::renderDrawData(ImGui::GetDrawData());
        }

        ELEMENT_SUPPORTS_CHILD(Window)
    };
}
