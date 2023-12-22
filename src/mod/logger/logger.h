#pragma once

#include <functional>
#include <cstdarg>
#include <string>

typedef std::function<void(const char*)> LogCallback;

class Logger {
private:
    std::vector<LogCallback> mListeners;

    static Logger& instance();

public:
    static void log(const std::string& fmt, ...);
    static void log(const char *fmt, ...);
    static void log(const char *fmt, va_list args);
    static void addListener(const LogCallback& callback);
    [[deprecated]] Logger() = default;
};
