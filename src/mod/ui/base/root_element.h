#pragma once

#include <vector>
#include "ui/base/element.h"
#include "ui/base/window.h"
#include "helpers/InputHelper.h"
#include "imgui_backend/imgui_impl_nvn.hpp"

namespace ui {
    ELEMENT(Root) {
        bool beginDraw() override {
            InputHelper::updatePadState();
            if (showing || InputHelper::isInputToggled()) {
                showing = true;
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

        ELEMENT_SUPPORTS_CHILD(Window);

    private:
        bool showing = false;
    };
}
