#include "lib.hpp"
#include "patches.hpp"
#include "nn/err.h"
#include "nn/fs.h"
#include "logger/Logger.hpp"
#include "socket.h"
#include "game/BinaryPointers.hpp"

HOOK_DEFINE_TRAMPOLINE(MainInitHook) {
    static void Callback() {

        R_ABORT_UNLESS(nn::fs::MountSdCardForDebug("sd"));

        R_ABORT_UNLESS(Logger::instance().init(LOGGER_IP, LOGGER_PORT).value);

        Logger::log(MODULE_NAME " Loaded!\n");

        Orig();
    }
};

// currently doesnt do anything, most likely never gets called as nothing needs to
HOOK_DEFINE_REPLACE(DebugPrintHook) {
    static void Callback(void* unk1, void *unk2, const char *fmt, ...) {

        va_list args;
        va_start(args, fmt);
        Logger::log(fmt, args);
        va_end(args);

    }
};

// hooks over the default nn::fs openfile, however it isnt exactly useful for anything inside the data archive.
HOOK_DEFINE_TRAMPOLINE(OpenFileHook) {
    static nn::Result Callback(nn::fs::FileHandle* handleOut, char const* path, int mode) {
        
        Logger::log("Opening File At: %s\n", path);

        return Orig(handleOut, path, mode);
    }
};

// unsure of the contents of this struct, size was purely a guess based loosely off of stack allocations
struct ArcDataStruct {
    char size[0x40];
};

// hooks over what i believe is what the game mainly uses for accessing files within the data archive 
HOOK_DEFINE_TRAMPOLINE(ArcLoadHook) {
    static ArcDataStruct Callback(const char **path) {
        Logger::log("Loading Arc File at: %s\n", path[0]);
        return Orig(path);
    }
};

// Hooks over save data checks
HOOK_DEFINE_TRAMPOLINE(SaveDataCheckHook) {
    static bool Callback(long param_1, u64 param_2, long param_3) {
        Logger::log("Save Data Check for Title ID %jX\n", param_2);
        return true;
    }
};

extern "C" void exl_main(void* x0, void* x1) {
    /* Setup hooking enviroment. */
    envSetOwnProcessHandle(exl::util::proc_handle::Get());
    exl::hook::Initialize();

    BinaryPointers::initValues();

#if IS_VERSION(VERSION_1_0_0)
    MainInitHook::InstallAtOffset(0x1439F40);
    DebugPrintHook::InstallAtOffset(0x2D07104);
    ArcLoadHook::InstallAtOffset(0xC96254);
    SaveDataCheckHook::InstallAtOffset(0x12b6cd4);
#elif IS_VERSION(VERSION_1_0_1)
    MainInitHook::InstallAtOffset(0x1455b20);
    DebugPrintHook::InstallAtOffset(0x2d613c4);
    ArcLoadHook::InstallAtOffset(0xc9b4a8);
    SaveDataCheckHook::InstallAtOffset(0x12e121c);
#endif

    OpenFileHook::InstallAtFuncPtr(nn::fs::OpenFile);

    runCodePatches();
}

extern "C" NORETURN void exl_exception_entry() {
    /* TODO: exception handling */
    EXL_ABORT(0x420);
}
