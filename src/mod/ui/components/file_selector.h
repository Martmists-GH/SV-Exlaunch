#pragma once

#include "ui/base/element.h"
#include "ui/base/combo.h"
#include "ui/base/selectable.h"
#include "nn/fs/fs_directories.hpp"
#include "nn/result.h"
#include "logger/logger.h"

namespace ui {
    ELEMENT(FileSelector) {
        std::string path;
        int ticker = -1;
        nn::fs::DirectoryEntry *entries = nullptr;
        long numEntries = 0;
        char selectedFile[0x400]{};
        std::vector<Selectable*> options;
        Combo combo = Combo::single([](Combo &c) {
            c.label = " ";
            c.placeholder = "Select a file";
        });

        FileSelector() {
            addChild(&combo);
        }

        bool isValid() override {
                return !path.empty();
        }

        bool beginDraw() override {
            ticker = (ticker + 1) % 600;
            if (ticker == 0 || entries == nullptr) {
                nn::fs::DirectoryHandle handle{};
                nn::Result res = nn::fs::OpenDirectory(&handle, "sd:/scripts/", nn::fs::OpenDirectoryMode_All);
                if (res.isFailure()) {
                    return false;
                } else {
                    nn::fs::GetDirectoryEntryCount(&numEntries, handle);
                }

                if (numEntries == 0) {
                    return false;
                }

                delete[] entries;
                entries = new nn::fs::DirectoryEntry[numEntries];
                nn::fs::ReadDirectory(&numEntries, entries, handle, numEntries);
                nn::fs::CloseDirectory(handle);

                combo.clearChildren();
                for (auto item : options) {
                    IM_DELETE(item);
                }
                options.clear();

                for (int i = 0; i < numEntries; i++) {
                    Selectable* item = Selectable::create([this, i](Selectable &_) {
                        _.label = entries[i].m_Name;
                        _.onSelected = [this, i]() {
                            snprintf(selectedFile, sizeof(selectedFile), "sd:/scripts/%s", entries[i].m_Name);
                            combo.placeholder = entries[i].m_Name;
                        };
                    });
                    combo.addChild(item);
                    options.push_back(item);
                }
            }

            return true;
        }
    };
}
