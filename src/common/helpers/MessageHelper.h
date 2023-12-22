#pragma once

#include "types.h"

namespace MessageHelper {
    NORETURN void abort(u32 errcode, const char *message, const char* description, ...);
    NORETURN void abort(u32 errcode, const char *message, const char* description, va_list args);
    void popup(u32 errcode, const char *message, const char* description, ...);
    void popup(u32 errcode, const char *message, const char* description, va_list args);
}
