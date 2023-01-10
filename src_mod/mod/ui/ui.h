#pragma once

#include "ui/base/element.h"
#include "ui/base/button.h"
#include "ui/base/collapsing_header.h"
#include "ui/base/combo.h"
#include "ui/base/menu.h"
#include "ui/base/menu_bar.h"
#include "ui/base/menu_item.h"
#include "ui/base/root_element.h"
#include "ui/base/selectable.h"
#include "ui/base/slider.h"
#include "ui/base/spacing.h"
#include "ui/base/text.h"
#include "ui/base/window.h"
#include "ui/components/file_selector.h"
#include "ui/components/lua_console_window.h"

ui::Root* getRootElement();
void exl_imgui_main();
void setup_ui();
void ui_lua_print(const char* str);

#define ROOT getRootElement()
