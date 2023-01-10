#pragma once

#include <functional>
#include <cstdarg>

typedef void(*LogCallback)(const char*);

class Logger {
private:
    std::vector<LogCallback> mListeners;

    static Logger& instance();

public:
    static void log(const char *fmt, ...);
    static void log(const char *fmt, va_list args);
    static void addListener(LogCallback callback);
    [[deprecated]] Logger() = default;
};
