#pragma once

#include <string>
#include "ui/base/element.h"
#include "ui/base/button.h"
#include "ui/base/checkbox.h"
#include "ui/base/child.h"
#include "ui/base/collapsing_header.h"
#include "ui/base/combo.h"
#include "ui/base/combo_simple.h"
#include "ui/base/input_int.h"
#include "ui/base/menu_bar.h"
#include "ui/base/slider.h"
#include "ui/base/spacing.h"
#include "ui/base/text.h"
#include "ui/base/text_unformatted.h"
#include "ui/components/file_selector.h"
#include "ui/components/string_view.h"
#include "fs/fs_files.hpp"
#include "features/lua.h"

namespace ui {
    ELEMENT(LuaConsoleWindow, Window) {
        struct FileSelector luaFileSelector = FileSelector::single([](struct FileSelector &_) {
            _.path = "sd:/scripts/";
        });
        struct Text luaConsoleOutput = Text::single([](struct Text &_) {
            _.content = "Output will appear here.";
        });
        bool running = false;

        LuaConsoleWindow() {
            this->Text([](struct Text &_) {
                _.content = "Run a script by placing it in sd:/scripts\n and selecting it below.";
            });
            this->Spacing();
            addChild(&luaFileSelector);
            addChild(&luaConsoleOutput);
            this->Button([this](struct Button &_) {
                _.label = "Run lua file";
                _.onClick = [this]() {
                    const char *file = luaFileSelector.selectedFile;

                    if (strcmp(file, "Select a file") == 0) {
                        return;
                    }

                    nn::fs::FileHandle fp{};
                    nn::fs::OpenFile(&fp, file, nn::fs::OpenMode_Read);
                    long size;
                    nn::fs::GetFileSize(&size, fp);
                    char buf[size];
                    nn::fs::ReadFile(fp, 0, buf, size);
                    nn::fs::CloseFile(fp);

                    running = true;
                    luaConsoleOutput.content = "";

                    if (luaL_dostring(L, buf) != LUA_OK) {
                        luaConsoleOutput.content.append(lua_tostring(L, -1));
                    } else {
                        luaConsoleOutput.content.append("Script ran successfully!");
                    }
                    running = false;
                };
            });
        }

        using Factory<LuaConsoleWindow>::create;
        using Factory<LuaConsoleWindow>::isValid;
        using Factory<LuaConsoleWindow>::single;
    };
}
