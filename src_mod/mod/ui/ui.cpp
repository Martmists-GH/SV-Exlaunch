#include "ui/ui.h"
#include "checks.hpp"

using namespace ui;

static LuaConsoleWindow luaConsoleWindow = LuaConsoleWindow::single([](LuaConsoleWindow& _){
    _.title = "Lua Console";
    _.initialPos = ImVec2(900, 50);
    _.initialSize = ImVec2(350, 500);
});

void ui_lua_print(const char* str) {
    if (luaConsoleWindow.running) {
        luaConsoleWindow.luaConsoleOutput.content.append(str);
    }
}

static Window mainWindow = Window::single([](Window& _){
    _.title = "SV Toolbox";
    _.toggleable = false;
    _.flags |= ImGuiWindowFlags_MenuBar;
    _.initialPos = ImVec2(50, 50);
    _.initialSize = ImVec2(300, 500);

    _.MenuBar([](MenuBar& _){
        _.Menu([](Menu& _) {
            _.label = "Tools";
            _.MenuItem([](MenuItem& _) {
                _.label = "Lua Console";
                _.checked = &luaConsoleWindow.open;
            });
        });
    });

    _.Text([](Text& _){
        _.content = "Press ZL+ZR to toggle all menus.";
    });

    _.Spacing();

    _.CollapsingHeader([](CollapsingHeader& _){
        _.label = "Gameplay Tweaks";

        _.SliderFloat([](SliderFloat& _){
            _.label = "Game Speed";
            _.min = 0.1f;
            _.max = 3.0f;
            _.value = 1.0f;
            _.onChange = [](float value) {
                int addr;

                if (is_version("1.0.0")) {
                    addr = 0x3871798;
                } else if (is_version("1.0.1")) {
                    addr = 0x3826ed8;
                } else if (is_version("1.1.0")) {
                    addr = 0x38d0b58;
                } else {
                    EXL_ABORT(0, "Unknown version");
                }

                exl::patch::CodePatcher p(addr);
                p.Write((int)(value * 0x1FCA056));
            };
        });

        _.Checkbox([](Checkbox&_){
            _.label = "Save data present for all games";
            _.onChange = [](bool checked) {
                int addr;

                if (is_version("1.0.0")) {
                    addr = 0x12b6cd4;
                } else if (is_version("1.0.1")) {
                    addr = 0x12e121c;
                } else if (is_version("1.1.0")) {
                    addr = 0x131eadc;
                } else {
                    EXL_ABORT(0, "Unknown version");
                }

                exl::patch::CodePatcher p(addr);
                if (checked) {
                    p.WriteInst(exl::armv8::inst::Movz(exl::armv8::reg::X0, 1));
                    p.WriteInst(exl::armv8::inst::Ret());
                } else {
                    p.WriteInst(exl::armv8::inst::SubImmediate(exl::armv8::reg::SP, exl::armv8::reg::SP, 0x50));
                    p.Write(0xa9037bfd);  // stp x29, x30,[sp, #0x20]
                }
            };
        });
    });
});

void setup_ui() {
    ROOT->addChild(&mainWindow);
    ROOT->addChild(&luaConsoleWindow);
}
