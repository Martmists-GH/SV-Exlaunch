#include "feat_imgui.h"
#include "helpers/InputHelper.h"
#include "imgui_backend/imgui_impl_nvn.hpp"
#include "nvn/nvn.h"
#include "nvn/nvn_Cpp.h"
#include "hook/trampoline_hook.hpp"
#include "nvn/nvn_CppFuncPtrImpl.h"
#include "nn/fs.h"
#include "checks.hpp"
#include "patch/code_patcher.hpp"


#if __has_include("feat_logging.h")
#include "feat_logging.h"
#endif

#if __has_include("feat_lua.h")
#include "feat_lua.h"
#endif

nvn::Device *nvnDevice;
nvn::Queue *nvnQueue;
nvn::CommandBuffer *nvnCmdBuf;

nvn::DeviceGetProcAddressFunc tempGetProcAddressFuncPtr;

nvn::CommandBufferInitializeFunc tempBufferInitFuncPtr;
nvn::DeviceInitializeFunc tempDeviceInitFuncPtr;
nvn::QueueInitializeFunc tempQueueInitFuncPtr;
nvn::QueuePresentTextureFunc tempPresentTexFunc;
nvn::TextureGetWidthFunc tempTexGetWidthFunc;
nvn::TextureGetHeightFunc tempTexGetHeightFunc;
nvn::WindowBuilderSetTexturesFunc tempWindowBuilderSetTexFunc;
nvn::WindowSetCropFunc tempWindowSetCropFunc;
nvn::CommandBufferSetViewportsFunc tempSetViewportFunc;

bool hasInitImGui = false;

void procDraw();

static void* (*mallocFuncPtr)(size_t size);
static void (*freeFuncPtr)(void *ptr);

bool InitImGui() {
    if (nvnDevice && nvnQueue && nvnCmdBuf) {
#ifdef LOGGING_ENABLED
        Logger::log("Creating ImGui.\n");
#endif
        IMGUI_CHECKVERSION();

        ImGuiMemAllocFunc allocFunc = [](size_t size, void *user_data) {
            return mallocFuncPtr(size);
        };

        ImGuiMemFreeFunc freeFunc = [](void *ptr, void *user_data) {
            freeFuncPtr(ptr);
        };

#ifdef LOGGING_ENABLED
        Logger::log("Setting Allocator Functions.\n");
#endif

        ImGui::SetAllocatorFunctions(allocFunc, freeFunc, nullptr);

#ifdef LOGGING_ENABLED
        Logger::log("Creating ImGui Context.\n");
#endif

        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void) io;

#ifdef LOGGING_ENABLED
        Logger::log("Setting Style to Dark.\n");
#endif

        ImGui::StyleColorsDark();

        ImguiNvnBackend::NvnBackendInitInfo initInfo = {
                .device = nvnDevice,
                .queue = nvnQueue,
                .cmdBuf = nvnCmdBuf
        };

#ifdef LOGGING_ENABLED
        Logger::log("Initializing Backend.\n");
#endif

        ImguiNvnBackend::InitBackend(initInfo);

        InputHelper::initKBM();

        InputHelper::setPort(0); // set input helpers default port to zero

        return true;

    } else {

#ifdef LOGGING_ENABLED
        Logger::log("Unable to create ImGui Renderer!\n");
#endif

        return false;
    }
}


void setViewport(nvn::CommandBuffer *cmdBuf, int first, int count, const float* ranges) {
    tempSetViewportFunc(cmdBuf, first, count, ranges);

//    if (hasInitImGui) {
//        auto &io = ImGui::GetIO();
//
//        int x = ranges[0];
//        int y = ranges[1];
//        int w = ranges[2];
//        int h = ranges[3];
//
//        io.DisplaySize = ImVec2(w - x, h - y);
//
//    }
}

void presentTexture(nvn::Queue *queue, nvn::Window *window, int texIndex) {

    if (hasInitImGui) {
        procDraw();
    }

    tempPresentTexFunc(queue, window, texIndex);
}

void windowBuilderSetTextures(nvn::WindowBuilder *builder, int count, nvn::Texture * const*textures) {
    tempWindowBuilderSetTexFunc(builder, count, textures);

    if (hasInitImGui) {
        int h = tempTexGetHeightFunc(textures[0]);
        int w = tempTexGetWidthFunc(textures[0]);

        Logger::log("WindowBuilderSetTextures: %d, %d\n", w, h);

        ImguiNvnBackend::getBackendData()->viewportSize = ImVec2(w, h);
    }
}

void setCrop(nvn::Window *window, int x, int y, int w, int h) {
    tempWindowSetCropFunc(window, x, y, w, h);

    if (hasInitImGui) {
        ImguiNvnBackend::getBackendData()->viewportSize = ImVec2(w, h);
    }
}

NVNboolean deviceInit(nvn::Device *device, const nvn::DeviceBuilder *builder) {
    NVNboolean result = tempDeviceInitFuncPtr(device, builder);
    nvnDevice = device;
    nvn::nvnLoadCPPProcs(nvnDevice, tempGetProcAddressFuncPtr);
    return result;
}

NVNboolean queueInit(nvn::Queue *queue, const nvn::QueueBuilder *builder) {
    NVNboolean result = tempQueueInitFuncPtr(queue, builder);
    nvnQueue = queue;
    return result;
}

NVNboolean cmdBufInit(nvn::CommandBuffer *buffer, nvn::Device *device) {
    NVNboolean result = tempBufferInitFuncPtr(buffer, device);
    nvnCmdBuf = buffer;

    if (!hasInitImGui) {
        Logger::log("Initializing ImGui.\n");
        hasInitImGui = InitImGui();
    }

    return result;
}

nvn::GenericFuncPtrFunc getProc(nvn::Device *device, const char *procName) {

    nvn::GenericFuncPtrFunc ptr = tempGetProcAddressFuncPtr(nvnDevice, procName);

    if (strcmp(procName, "nvnQueueInitialize") == 0) {
        tempQueueInitFuncPtr = (nvn::QueueInitializeFunc) ptr;
        return (nvn::GenericFuncPtrFunc) &queueInit;
    } else if (strcmp(procName, "nvnCommandBufferInitialize") == 0) {
        tempBufferInitFuncPtr = (nvn::CommandBufferInitializeFunc) ptr;
        return (nvn::GenericFuncPtrFunc) &cmdBufInit;
    } else if (strcmp(procName, "nvnCommandBufferSetViewports") == 0) {
        tempSetViewportFunc = (nvn::CommandBufferSetViewportsFunc) ptr;
        return (nvn::GenericFuncPtrFunc) &setViewport;
    } else if (strcmp(procName, "nvnQueuePresentTexture") == 0) {
        tempPresentTexFunc = (nvn::QueuePresentTextureFunc) ptr;
        return (nvn::GenericFuncPtrFunc) &presentTexture;
    } else if (strcmp(procName, "nvnWindowBuilderSetTextures") == 0) {
        tempWindowBuilderSetTexFunc = (nvn::WindowBuilderSetTexturesFunc) ptr;
        return (nvn::GenericFuncPtrFunc) &windowBuilderSetTextures;
    } else if (strcmp(procName, "nvnTextureGetWidth") == 0) {
        tempTexGetWidthFunc = (nvn::TextureGetWidthFunc) ptr;
    } else if (strcmp(procName, "nvnTextureGetHeight") == 0) {
        tempTexGetHeightFunc = (nvn::TextureGetHeightFunc) ptr;
    } else if (strcmp(procName, "nvnWindowSetCrop") == 0) {
        tempWindowSetCropFunc = (nvn::WindowSetCropFunc) ptr;
        return (nvn::GenericFuncPtrFunc) &setCrop;
    }

    return ptr;
}

HOOK_DEFINE_TRAMPOLINE(NvnBootstrapHook) {
    static void *Callback(const char *funcName) {

    void *result = Orig(funcName);

#ifdef LOGGING_ENABLED
    Logger::log("Installing nvn Hooks.\n");
#endif

    if (strcmp(funcName, "nvnDeviceInitialize") == 0) {
        tempDeviceInitFuncPtr = (nvn::DeviceInitializeFunc) result;
        return (void *) &deviceInit;
    }
    if (strcmp(funcName, "nvnDeviceGetProcAddress") == 0) {
        tempGetProcAddressFuncPtr = (nvn::DeviceGetProcAddressFunc) result;
        return (void *) &getProc;
    }

    return result;
    }
};

#define INPUT_HOOK(type) \
HOOK_DEFINE_TRAMPOLINE(Disable##type) { \
    static int Callback(int *unkInt, nn::hid::Npad##type *state, int count, uint const &port) { \
        int result = Orig(unkInt, state, count, port); \
        if (!InputHelper::isReadInputs()) { \
            if(InputHelper::isInputToggled()) { \
                *state = nn::hid::Npad##type(); \
            } \
        } \
        return result; \
    } \
};                 \

INPUT_HOOK(FullKeyState);
INPUT_HOOK(HandheldState);
INPUT_HOOK(JoyDualState);
INPUT_HOOK(JoyLeftState);
INPUT_HOOK(JoyRightState);

static lua_State *L = nullptr;
typedef struct {
    bool visible, running;
    std::string output;
} LuaWindow;
static LuaWindow luaConsole = {
    .visible = false,
    .running = false,
    .output = "",
};
static char selectedLuaFile[0x100] = {"Select a Lua file"};
static float gameSpeed = 1.0f;

void procDraw() {
    InputHelper::updatePadState(); // update input helper

    if (InputHelper::isInputToggled()) {
        ImguiNvnBackend::newFrame();
        ImGui::NewFrame();

        if (ImGui::Begin("SV Toolbox", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar)) {
            ImGui::Text("Press ZL+ZR to close all ImGui windows.");
            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("Windows")) {
                    ImGui::MenuItem("Lua Console", nullptr, &luaConsole.visible);
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            if (ImGui::SliderFloat("Game Speed", &gameSpeed, 0.1f, 3.0f)) {
                int addr;

                if (is_version("1.0.0")) {
                    addr = 0x3871798;
                } else if (is_version("1.0.1")) {
                    addr = 0x3826ed8;
                } else if (is_version("1.1.0")) {
                    addr = 0x38d0b58;
                } else {
                    EXL_ABORT(0, "Unknown version");
                }

                exl::patch::CodePatcher p(addr);
                p.Write((int)(gameSpeed * 0x1FCA056));
            }

            ImGui::End();
        }

        if (luaConsole.visible) {
            ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
            if (ImGui::Begin("Lua Console", &luaConsole.visible)) {
                nn::fs::DirectoryHandle handle {};
                nn::Result res = nn::fs::OpenDirectory(&handle, "sd:/scripts/", nn::fs::OpenDirectoryMode_All);
                long numItems;
                if (res.isSuccess()) {
                    nn::fs::GetDirectoryEntryCount(&numItems, handle);
                } else {
                    numItems = 0;
                }

                if (ImGui::BeginCombo(" ", selectedLuaFile)) {
                    if (numItems > 0) {
                        nn::fs::DirectoryEntry entry[numItems];
                        nn::fs::ReadDirectory(&numItems, entry, handle, numItems);
                        for (int i = 0; i < numItems; i++) {
                            if (ImGui::Selectable(entry[i].m_Name)) {
                                snprintf(selectedLuaFile, 0x100, "sd:/scripts/%s", entry[i].m_Name);
                            }
                        }
                    }
                    ImGui::EndCombo();
                }

                if (res.isSuccess()) {
                    nn::fs::CloseDirectory(handle);
                }

                if (strcmp(selectedLuaFile, "Select a Lua file") != 0 && ImGui::Button("Run Lua file")) {
                    luaConsole.output = "";
                    luaConsole.running = true;

                    nn::fs::FileHandle fp;
                    nn::fs::OpenFile(&fp, selectedLuaFile, nn::fs::OpenMode_Read);
                    long size;
                    nn::fs::GetFileSize(&size, fp);
                    char *buf = (char *) malloc(size);
                    nn::fs::ReadFile(fp, 0, buf, size);
                    nn::fs::CloseFile(fp);

                    if (luaL_dostring(L, buf) != LUA_OK) {
                        luaConsole.output.append(lua_tostring(L, -1));
                    } else {
                        luaConsole.output.append("Lua script ran successfully.");
                    }
                    luaConsole.running = false;
                }
                ImGui::Text("%s", luaConsole.output.c_str());
                ImGui::End();
            }
        }

        ImGui::Render();
        ImguiNvnBackend::renderDrawData(ImGui::GetDrawData());
    }
}

HOOK_DEFINE_TRAMPOLINE(GetLuaState) {
    static lua_State* Callback(void* p1, void* p2) {
        L = Orig(p1, p2);
        return L;
    }
};

HOOK_DEFINE_TRAMPOLINE(LuaPrintConsole) {
    static int Callback(lua_State *L_) {
        if (luaConsole.running) {
            int n = lua_gettop(L_);
            for (int i = 1; i <= n; i++) {
                luaConsole.output.append(lua_tostring(L_, i));
                luaConsole.output.append(" ");
            }
            lua_settop(L_, 0);
            luaConsole.output.append("\n");
        }
        return Orig(L_);
    }
};

namespace sv::feature::imgui {
    void exl_main() {
        // Required for imgui to work
        nn::ro::LookupSymbol(reinterpret_cast<uintptr_t *>(&mallocFuncPtr), "malloc");
        nn::ro::LookupSymbol(reinterpret_cast<uintptr_t *>(&freeFuncPtr), "free");
        NvnBootstrapHook::InstallAtSymbol("nvnBootstrapLoader");
        DisableFullKeyState::InstallAtSymbol("_ZN2nn3hid6detail13GetNpadStatesEPiPNS0_16NpadFullKeyStateEiRKj");
        DisableHandheldState::InstallAtSymbol("_ZN2nn3hid6detail13GetNpadStatesEPiPNS0_17NpadHandheldStateEiRKj");
        DisableJoyDualState::InstallAtSymbol("_ZN2nn3hid6detail13GetNpadStatesEPiPNS0_16NpadJoyDualStateEiRKj");
        DisableJoyLeftState::InstallAtSymbol("_ZN2nn3hid6detail13GetNpadStatesEPiPNS0_16NpadJoyLeftStateEiRKj");
        DisableJoyRightState::InstallAtSymbol("_ZN2nn3hid6detail13GetNpadStatesEPiPNS0_17NpadJoyRightStateEiRKj");

        // Custom hooks for UI
        ptrdiff_t newStateOffset, printOffset;
        if (is_version("1.0.0")) {
            newStateOffset = 0x00adae00;
            printOffset = 0x45f80;
        } else if (is_version("1.0.1")) {
            newStateOffset = 0x0003b420;
            printOffset = 0x46130;
        } else if (is_version("1.1.0")) {
            newStateOffset = 0x0003b420;
            printOffset = 0x46130;
        } else {
            EXL_ABORT(0, "Unsupported version");
        }
        GetLuaState::InstallAtOffset(newStateOffset);
        LuaPrintConsole::InstallAtOffset(printOffset);
    }
}
