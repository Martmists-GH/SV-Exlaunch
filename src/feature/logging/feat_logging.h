#pragma once

#include "socket/Socket.h"

#define LOGGING_ENABLED 1

class Logger : public Socket {
public:
    Logger() = default;

    static Logger& instance();

    static void log(const char *fmt, ...);
    static void log(const char *fmt, va_list args);
};

namespace sv::feature::logging {
    void exl_main();
}
