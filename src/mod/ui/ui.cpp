#include "ui/ui.h"
#include "logger/logger.h"
#include "checks.hpp"
#include "helpers/MessageHelper.h"

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

static Window debugLogWindow = Window::single([](Window &_) {
    _.title = "Debug Log";
    _.sticky = true;

    auto view = _.StringView();

    Logger::addListener([view](const char *message) {
        view->append(message);
    });
});

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
                _.label = "Debug Log";
                _.checked = &debugLogWindow.open;
            });
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
                    addr = 0x03871798;
                } else if (is_version("1.0.1")) {
                    addr = 0x03826ed8;
                } else if (is_version("1.1.0")) {
                    addr = 0x038d0b58;
                } else if (is_version("1.2.0")) {
                    addr = 0x039d8ec0;
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
                    addr = 0x012b6cd4;
                } else if (is_version("1.0.1")) {
                    addr = 0x012e121c;
                } else if (is_version("1.1.0")) {
                    addr = 0x0131eadc;
                } else if (is_version("1.2.0")) {
                    addr = 0x01ebc048;
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

        _.Checkbox([](Checkbox& _){
            _.label = "Enable EXP Share";
            _.enabled = true;
            _.onChange = [](bool checked) {
                int addr;

                if (is_version("1.0.0")) {
                    addr = 0x010f9660;
                } else if (is_version("1.0.1")) {
                    addr = 0x010aadf4;
                } else if (is_version("1.1.0")) {
                    addr = 0x010dedc0;
                } else if (is_version("1.2.0")) {
                    addr = 0x0106d5e0;
                }

                exl::patch::CodePatcher p(addr);
                if (checked) {
                    p.Write(0xa9ba7bfd);  // stp x29, x30, [sp, #-0x60]!
                } else {
                    p.WriteInst(exl::armv8::inst::Ret());
                }
            };
        });

        _.SliderFloat([](SliderFloat&_){
            _.min = 0.0f;
            _.max = 64.0f;
            _.value = 1.0f;
            _.label = "EXP Multiplier";
            _.onChange = [](float value) {
                setExpMultiplier(value);
            };
        });

        _.Checkbox([](Checkbox& _){
            _.label = "100% Catch Rate";
            _.enabled = true;
            _.onChange = [](bool checked) {
                int addr;

                if (is_version("1.1.0")) {
                    addr = 0x017262ec;
                } else if (is_version("1.2.0")) {
                    addr = 0x01854d94;
                } else {
                    MessageHelper::popup(0, "Unsupported version", "This feature is not supported on this version of the game. Only 1.1.0 and 1.2.0 are supported.");
                    return;
                }

                exl::patch::CodePatcher p(addr);
                if (checked) {
                    p.WriteInst(exl::armv8::inst::Movz(exl::armv8::reg::W8, 0x1));
                } else {
                    p.Write(0x3940a028);  // ldrb w8, [x1, #0x28]
                }
            };
        });
    });

    _.CollapsingHeader([](CollapsingHeader& _){
        _.label = "Miscellaneous";

        _.Button([](Button& _){
            _.label = "Show game info";
            _.onClick = []() {
                MessageHelper::popup(0, "Game Info", "Game version: %s\n", get_version().c_str());
            };
        });
    });
});

void setup_ui() {
    ROOT->addChild(&mainWindow);
    ROOT->addChild(&luaConsoleWindow);
    ROOT->addChild(&debugLogWindow);
}
