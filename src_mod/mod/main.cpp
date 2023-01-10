#include "exlaunch.hpp"
#include "fs/fs_mount.hpp"
#include "socket/Socket.h"
#include "logger/logger.h"
#include "oe.h"
#include "ui/ui.h"
#include "mlua/lua.h"
#include "romfs/redirect.h"

static Socket gSocket {};
void logToSocket(const char* message) {
    gSocket.sendMessage(message);
}

HOOK_DEFINE_TRAMPOLINE(MainInitHook){
    static void Callback(){
        R_ABORT_UNLESS(nn::fs::MountSdCardForDebug("sd"));

        if (gSocket.init(LOGGER_IP, LOGGER_PORT).isFailure()) {
            Logger::log("Failed to connect to logging server!\n");
        } else {
            Logger::addListener(&logToSocket);
            Logger::log("Connected to logging server!\n");
        }
        Logger::log(MODULE_NAME " Loaded!\n");

        nn::oe::DisplayVersion display_version{};
        nn::oe::GetDisplayVersion(&display_version);
        Logger::log("Detected version: %s\n", display_version.name);

        Orig();
    }
};

extern "C" void exl_main(void* x0, void* x1) {
    envSetOwnProcessHandle(exl::util::proc_handle::Get());
    exl::hook::Initialize();

    MainInitHook::InstallAtSymbol("nnMain");

    exl_romfs_main();
    exl_lua_main();
    exl_imgui_main();
    setup_ui();
}

extern "C" NORETURN void exl_exception_entry() {
    EXL_ABORT(0x420);
}
