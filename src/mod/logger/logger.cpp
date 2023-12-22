#include <string>
#include "logger.h"
#include "util.h"

static Logger logger; // do not move inside, something something concurrency
Logger& Logger::instance() {
    return logger;
}

void Logger::log(const std::string& fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log(fmt.c_str(), args);
    va_end(args);
}

void Logger::log(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log(fmt, args);
    va_end(args);
}

void Logger::log(const char *fmt, va_list args) {
    char buffer[0x1000] = {};
    if (nn::util::VSNPrintf(buffer, sizeof(buffer), fmt, args) >= 0) {
        for (const auto& listener : instance().mListeners) {
            listener(buffer);
        }
    }
}

void Logger::addListener(const LogCallback& callback) {
    instance().mListeners.push_back(callback);
}
