#include "MessageHelper.h"
#include "nn/err.h"
#include "nx/kernel/svc.h"
#include "util.h"

namespace MessageHelper {
    NORETURN void abort(u32 errcode, const char *message, const char* description, ...) {
        va_list args;
        va_start(args, description);
        abort(errcode, message, description, args);
        va_end(args);
    }

    NORETURN void abort(u32 errcode, const char *message, const char* description, va_list args) {
        popup(errcode, message, description, args);
        svcSleepThread(1000000000L);
        svcExitProcess();
    }

    void popup(u32 errcode, const char *message, const char* description, ...) {
        va_list args;
        va_start(args, description);
        popup(errcode, message, description, args);
        va_end(args);
    }

    void popup(u32 errcode, const char *message, const char* description, va_list args) {
        char buffer[0x2000] = {};
        if (nn::util::VSNPrintf(buffer, sizeof(buffer), description, args) >= 0) {
            nn::err::ApplicationErrorArg arg;
            arg.SetApplicationErrorCodeNumber(errcode);
            arg.SetDialogMessage(message);
            arg.SetFullScreenMessage(buffer);
            nn::err::ShowApplicationError(arg);
        }
    }
}
