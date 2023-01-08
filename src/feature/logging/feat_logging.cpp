#include "feat_logging.h"
#include "checks.hpp"
#include "exlaunch.hpp"
#include "fs/fs_files.hpp"
#include "fs/fs_mount.hpp"
#include "nifm.h"
#include "util.h"

extern "C" void nnMain();

Logger& Logger::instance() {
    static Logger instance;
    return instance;
}

void Logger::log(const char *fmt, ...) {
    if(instance().mState != SocketState::CONNECTED)
        return;

    va_list args;
    va_start(args, fmt);

    log(fmt, args);

    va_end(args);
}

void Logger::log(const char *fmt, va_list args) {
    if(instance().mState != SocketState::CONNECTED)
        return;

    char buffer[PACKET_MAX_SIZE] = {};

    if(nn::util::VSNPrintf(buffer, sizeof(buffer), fmt, args) > 0) {
        instance().sendMessage(buffer);
    }
}

namespace sv::feature::logging {
    // Hook into nnMain
    HOOK_DEFINE_TRAMPOLINE(MainInitHook){
        static void Callback(){
            R_ABORT_UNLESS(nn::fs::MountSdCardForDebug("sd"));
            R_ABORT_UNLESS(Logger::instance().init(LOGGER_IP, LOGGER_PORT).value);
            Logger::log(MODULE_NAME " Loaded!\n");

            nn::oe::DisplayVersion display_version;
            nn::oe::GetDisplayVersion(&display_version);
            Logger::log("Detected version: %s\n", display_version.name);

            Orig();
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

    HOOK_DEFINE_TRAMPOLINE(AbortLogger) {
        static void Callback(char const *errCode, char const *func, char const *line, int code, char const *fmt, ...) {
            va_list args;
            va_start(args, fmt);
            char buf[0x100];
            vsnprintf(buf, sizeof(buf), fmt, args);
            va_end(args);
            Logger::log("Abort: %s", buf);
            Orig(errCode, func, line, code, "%s", buf);
        }
    };

    void exl_main() {
        MainInitHook::InstallAtFuncPtr(nnMain);
        AbortLogger::InstallAtSymbol("_ZN2nn4diag6detail9AbortImplEPKcS3_S3_iS3_z");

        if (is_version("1.0.0")) {
            ArcLoadHook::InstallAtOffset(0xC96254);
        } else if (is_version("1.0.1")) {
            ArcLoadHook::InstallAtOffset(0xc9b4a8);
        } else if (is_version("1.1.0")) {
            ArcLoadHook::InstallAtOffset(0xccc65c);
        } else {
            EXL_ABORT(0, "Unknown version");
        }

        OpenFileHook::InstallAtFuncPtr(nn::fs::OpenFile);
    }
}
