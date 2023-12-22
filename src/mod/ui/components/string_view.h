#pragma once

#include "ui/base/element.h"
#include "ui/base/function_element.h"
#include "ui/base/slider.h"
#include "logger/logger.h"
#include "os.h"

namespace ui {
    ELEMENT(StringView) {
        std::string buffer;
        nn::os::MutexType mutex;
        int maxLines = 100;

        StringView() {
            nn::os::InitializeMutex(&mutex, true, 0);

            addChild(SliderInt::create([this](SliderInt &s) {
                s.label = "Max lines";
                s.min = 10;
                s.max = 1000;
                s.value = maxLines;
                s.onChange = [this](int value) {
                    maxLines = value;
                };
            }));

            addChild(FunctionElement::create([this](FunctionElement &f) {
                f.callback = [this]() {
                    ImGui::TextUnformatted(buffer.c_str());
                };
            }));
        }

        void append(const char* str) {
            nn::os::LockMutex(&mutex);

//            buffer += str;

            // trim to last N lines
            int lines = 0;
            for (long i = buffer.size() - 1; i >= 0; i--) {
                if (buffer[i] == '\n') {
                    lines++;
                    if (lines > maxLines) {
                        memcpy(buffer.data(), buffer.data() + i, buffer.size() - i);
                        buffer.resize(buffer.size() - i);
                        break;
                    }
                }
            }

            nn::os::UnlockMutex(&mutex);
        }
    };
}
