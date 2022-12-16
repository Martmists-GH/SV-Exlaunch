#include "exlaunch.hpp"
#include "patches.hpp"
#include "externals.hpp"

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

#ifdef LUA_ENABLED
#ifdef LOGGING_ENABLED
#define CUSTOM_ENABLED 1

static bool (*lua_toboolean)(void *L, int idx);

#define Lua_String_Arg_Hook(N, boolean) \
HOOK_DEFINE_TRAMPOLINE(Hook_##N) { \
    static void* Callback(lua_State *L) { \
        const char* param = lua_tolstring(L, 2, nullptr); \
        if (boolean) { \
            bool b = lua_toboolean(L, 3); \
            Logger::log("Lua Hook #%08x String Arg: %s | %d\n", N, param, b); \
        } else {                        \
            Logger::log("Lua Hook #%08x String Arg: %s\n", N, param); \
        } \
        return Orig(L); \
    } \
};

Lua_String_Arg_Hook(0x165d8c0, false)
Lua_String_Arg_Hook(0x179bdd0, true)
Lua_String_Arg_Hook(0x23e0780, true)
Lua_String_Arg_Hook(0x2411d30, true)
Lua_String_Arg_Hook(0x2421560, false)
Lua_String_Arg_Hook(0x24264b0, true)

HOOK_DEFINE_TRAMPOLINE(StringsFuncHook) {
    static void* Callback(void* p1, void* p2, char* p3, char* p4) {
        Logger::log("Lua Hook Strings: %s, %s\n", p3, p4);
        return Orig(p1, p2, p3, p4);
    }
};

#endif
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
    // sv::feature::romfs::exl_main();
#endif

#ifdef CUSTOM_ENABLED
    EXTERNAL_ADDRESS(lua_toboolean, 0xa44d90);
    Hook_0x165d8c0::InstallAtOffset(0x165d8c0);
    Hook_0x179bdd0::InstallAtOffset(0x179bdd0);
    Hook_0x23e0780::InstallAtOffset(0x23e0780);
    Hook_0x2411d30::InstallAtOffset(0x2411d30);
    Hook_0x2421560::InstallAtOffset(0x2421560);
    Hook_0x24264b0::InstallAtOffset(0x24264b0);

    StringsFuncHook::InstallAtOffset(0x1312744);
#endif

    runCodePatches();
}

extern "C" NORETURN void exl_exception_entry() {
    /* TODO: exception handling */
    EXL_ABORT(0x420);
}
