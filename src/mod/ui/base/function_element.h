#pragma once

#include <vector>
#include <string>
#include <functional>
#include "ui/base/element.h"

namespace ui {
    ELEMENT(FunctionElement) {
        std::function<void()> callback;

        bool isValid() override {
            return callback != nullptr;
        }

        void draw() override {
            callback();
        }
    };
}
