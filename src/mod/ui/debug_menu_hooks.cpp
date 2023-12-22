#include "exlaunch.hpp"
#include "checks.hpp"


static float multiplier = 1.0f;

void setExpMultiplier(float m) {
    multiplier = m;
}

struct CalcResult {
    u32 exp;
    bool byGakusyuSouti;
    bool isContainBonus;
};

// Patch Dpr::Battle::Logic::Exp::CalcExp
HOOK_DEFINE_TRAMPOLINE(ExpMultiplierHook) {
    static void Callback(void* calcParam, CalcResult* result) {
        Orig(calcParam, result);
        result->exp = (u32)(result->exp * multiplier);
    }
};

void exl_debug_menu_main() {
    if (is_version("1.0.0")) {
        ExpMultiplierHook::InstallAtOffset(0x011269b8);
    } else if (is_version("1.0.1")) {
        ExpMultiplierHook::InstallAtOffset(0x010bace8);
    } else if (is_version("1.1.0")) {
        ExpMultiplierHook::InstallAtOffset(0x010ee724);
    } else if (is_version("1.2.0")) {
        ExpMultiplierHook::InstallAtOffset(0x0107d4b4);
    }
}
