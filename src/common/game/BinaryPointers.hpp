#pragma once

#include "nn/result.h"
#include "game/checks.hpp"

/// @brief Allows for functions without symbols and static variables to be used/called by our code.
class BinaryPointers {
public:
    static BinaryPointers& instance();
    static void initValues();

    nn::Result (*socketInit)();
    
    bool *isDebugModePtr;

private:

#if IS_VERSION(VERSION_1_0_0)
    static constexpr uintptr_t socketInitOffset = 0x1698800;
    static constexpr uintptr_t isDebugModePtrOffset = 0x41DC078;
#elif IS_VERSION(VERSION_1_0_1)
    static constexpr uintptr_t socketInitOffset = 0x16c1470;
    static constexpr uintptr_t isDebugModePtrOffset = 0x41DC080;
#endif
};
