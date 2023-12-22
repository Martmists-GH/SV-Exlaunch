#include "exlaunch.hpp"
#include "fs/fs_mount.hpp"
#include "socket/Socket.h"
#include "logger/logger.h"
#include "oe.h"
#include "ui/ui.h"
#include "features/features.h"
#include "checks.hpp"
#include "helpers/MessageHelper.h"

static Socket gSocket {};

HOOK_DEFINE_TRAMPOLINE(MainInitHook){
    static void Callback(){
        R_ABORT_UNLESS(nn::fs::MountSdCardForDebug("sd"));

        if (gSocket.init(LOGGER_IP, LOGGER_PORT).isFailure()) {
            Logger::log("Failed to connect to logging server!\n");
        } else {
            Logger::addListener([](const char* message) {
                gSocket.sendMessage(message);
            });
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

    if (!is_version("1.0.0") &&
        !is_version("1.0.1") &&
        !is_version("1.1.0") &&
        !is_version("1.2.0")) {

        MessageHelper::abort(0, "Unsupported version detected.", "Only the following versions are supported:\n1.0.0\n1.0.1\n1.1.0\n1.2.0\n\nYour version: %s", get_version().c_str());
    }

    MainInitHook::InstallAtSymbol("nnMain");

    exl_imgui_main();
#ifndef NDEBUG
    exl_debug_menu_main();
    setup_ui();
#endif
    exl_features_main();
}

extern "C" NORETURN void exl_exception_entry() {
    EXL_ABORT(0x420);
}
