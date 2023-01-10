#pragma once

#include <vector>
#include <string>
#include <functional>
#include "imgui.h"
#include "exlaunch.hpp"
#include "logger/logger.h"

namespace ui {
    class Drawable {
    protected:
        std::vector<Drawable *> mChildren;
        virtual bool beginDraw() {
            return true;
        }

        virtual void draw() {
            for (auto child: mChildren) {
                child->render();
            }
        }

        virtual void endDraw() {}
    public:
        virtual ~Drawable() = default;

        virtual void render() {
            if (this->beginDraw()) {
                this->draw();
                this->endDraw();
            }
        }

        void addChild(Drawable *child) {
            mChildren.push_back(child);
        }

        void clearChildren() {
            mChildren.clear();
        }
    };

#define ELEMENT(TName, ...) struct TName : public ui::Factory<TName> __VA_OPT__(,) __VA_ARGS__
#define ELEMENT_SUPPORTS_CHILD(TName) struct TName* TName(std::function<void(struct TName&)> initializer) { \
    struct TName* instance = TName::create(initializer);                                                    \
    mChildren.push_back(instance);                                                                          \
    mOwnedChildren.push_back(instance);                                                                     \
    return instance;                                                                                        \
}                                                                                                           \
struct TName* TName() {                                                                                     \
    return TName([](struct TName&){});                                                                      \
}

    template<typename T>
    class Factory : public virtual ui::Drawable {
    protected:
        std::vector<Drawable*> mOwnedChildren;

        virtual bool isValid() {
            return true;
        }

        ~Factory() override {
            for (auto owned : mOwnedChildren) {
                delete owned;
            }
        }

    public:
        static T* create(std::function<void(T &)> initializer) {
            T* instance = IM_NEW(T);
            initializer(*instance);
            if (!instance->isValid()) {
                EXL_ABORT(0, "Type reported invalid after initializing!");
            }
            return instance;
        }

        static T single(std::function<void(T &)> initializer) {
            T instance{};
            initializer(instance);
            if (!instance.isValid()) {
                EXL_ABORT(0, "Type reported invalid after initializing!");
            }
            return instance;
        }
    };
}
