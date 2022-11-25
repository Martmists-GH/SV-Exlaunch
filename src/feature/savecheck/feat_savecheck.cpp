#include "feat_savecheck.h"
#include "checks.hpp"
#include "hook/replace_hook.hpp"

#if __has_include("feat_logging.h")
#include "feat_logging.h"
#endif

namespace sv::feature::savecheck {
    // Hooks over save data checks
    HOOK_DEFINE_REPLACE(SaveDataCheckHook) {
        static bool Callback(long param_1, u64 param_2, long param_3) {
#ifdef LOGGING_ENABLED
            Logger::log("Save Data Check for Title ID %jX\n", param_2);
#endif
            return true;
        }
    };

    void exl_main() {
        if (is_version("1.0.0")) {
            SaveDataCheckHook::InstallAtOffset(0x12b6cd4);
        } else if (is_version("1.0.1")) {
            SaveDataCheckHook::InstallAtOffset(0x12e121c);
        } else {
            EXL_ASSERT(false, "Unknown version");
        }
    }
}
