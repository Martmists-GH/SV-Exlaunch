#include "exlaunch.hpp"
#include "patches.hpp"

#if __has_include("feat_logging.h")
#include "feat_logging.h"
#endif

#if __has_include("feat_savecheck.h")
#include "feat_savecheck.h"
#endif

#if __has_include("feat_lua.h")
#include "feat_lua.h"
#endif

#if __has_include("feat_romfs.h")
#include "feat_romfs.h"
#endif

extern "C" void exl_main(void* x0, void* x1) {
    /* Setup hooking enviroment. */
    envSetOwnProcessHandle(exl::util::proc_handle::Get());
    exl::hook::Initialize();

#ifdef LOGGING_ENABLED
    sv::feature::logging::exl_main();
#endif

#ifdef SAVECHECK_ENABLED
    sv::feature::savecheck::exl_main();
#endif

#ifdef LUA_ENABLED
    sv::feature::lua::exl_main();
#endif

#ifdef ROMFS_ENABLED
    sv::feature::romfs::exl_main();
#endif

    runCodePatches();
}

extern "C" NORETURN void exl_exception_entry() {
    /* TODO: exception handling */
    EXL_ABORT(0x420);
}
