#include "ui.h"
#include "helpers.h"
#include "imgui_backend/imgui_impl_nvn.hpp"
#include "nvn/nvn.h"
#include "nvn/nvn_Cpp.h"
#include "exlaunch.hpp"
#include "nvn/nvn_CppFuncPtrImpl.h"
#include "nn/fs.h"
#include "checks.hpp"
#include "logger/logger.h"

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
nvn::CommandBufferSetTexturePoolFunc tempCommandSetTexturePoolFunc;
nvn::CommandBufferSetSamplerPoolFunc tempCommandSetSamplerPoolFunc;


bool hasInitImGui = false;

static ui::Root root = ui::Root::single([](ui::Root& it){});

void procDraw() {
    root.render();
}

ui::Root* getRootElement() {
    return &root;
}

static void* (*mallocFuncPtr)(size_t size);
static void (*freeFuncPtr)(void *ptr);

bool InitImGui() {
    if (nvnDevice && nvnQueue && nvnCmdBuf) {
        Logger::log("Creating ImGui.\n");
        IMGUI_CHECKVERSION();

        ImGuiMemAllocFunc allocFunc = [](size_t size, void *user_data) {
            return mallocFuncPtr(size);
        };

        ImGuiMemFreeFunc freeFunc = [](void *ptr, void *user_data) {
            freeFuncPtr(ptr);
        };

        Logger::log("Setting Allocator Functions.\n");
        ImGui::SetAllocatorFunctions(allocFunc, freeFunc, nullptr);
        Logger::log("Creating ImGui Context.\n");

        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void) io;

        Logger::log("Setting Style to Dark.\n");
        ImGui::StyleColorsDark();

        ImguiNvnBackend::NvnBackendInitInfo initInfo = {
                .device = nvnDevice,
                .queue = nvnQueue,
                .cmdBuf = nvnCmdBuf
        };

        Logger::log("Initializing Backend.\n");
        ImguiNvnBackend::InitBackend(initInfo);
        InputHelper::initKBM();
        InputHelper::setPort(0); // set input helpers default port to zero

        return true;

    } else {
        Logger::log("Unable to create ImGui Renderer!\n");

        return false;
    }
}

static nvn::CommandBuffer* __cmdBuf = nullptr;
static const nvn::TexturePool* __texturePool = nullptr;
static const nvn::SamplerPool* __samplerPool = nullptr;

void setTexturePool(nvn::CommandBuffer* cmdBuf, const nvn::TexturePool* pool) {
    __cmdBuf = cmdBuf;
    __texturePool = pool;

    tempCommandSetTexturePoolFunc(cmdBuf, pool);
}

void setSamplerPool(nvn::CommandBuffer* cmdBuf, const nvn::SamplerPool* pool) {
    __samplerPool = pool;

    tempCommandSetSamplerPoolFunc(cmdBuf, pool);
}

void presentTexture(nvn::Queue *queue, nvn::Window *window, int texIndex) {
    auto* buf = __cmdBuf;
    auto* pool = __texturePool;
    auto* samplerPool = __samplerPool;

    if (hasInitImGui) {
        procDraw();
    }

    buf->BeginRecording();
    setTexturePool(buf, pool);
    setSamplerPool(buf, samplerPool);
    auto handle = buf->EndRecording();
    queue->SubmitCommands(1, &handle);
    tempPresentTexFunc(queue, window, texIndex);
}

void windowBuilderSetTextures(nvn::WindowBuilder *builder, int count, nvn::Texture * const*textures) {
    tempWindowBuilderSetTexFunc(builder, count, textures);

    if (hasInitImGui) {
        int h = tempTexGetHeightFunc(textures[0]);
        int w = tempTexGetWidthFunc(textures[0]);

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
    } else if (strcmp(procName, "nvnCommandBufferSetTexturePool") == 0) {
        tempCommandSetTexturePoolFunc = (nvn::CommandBufferSetTexturePoolFunc) ptr;
        return (nvn::GenericFuncPtrFunc) &setTexturePool;
    } else if (strcmp(procName, "nvnCommandBufferSetSamplerPool") == 0) {
        tempCommandSetSamplerPoolFunc = (nvn::CommandBufferSetSamplerPoolFunc ) ptr;
        return (nvn::GenericFuncPtrFunc) &setSamplerPool;
    } else if (strcmp(procName, "nvnDeviceInitialize") == 0) {  // Duplicated from NvnBootstrapHook to make porting to e.g. BDSP easier
        tempDeviceInitFuncPtr = (nvn::DeviceInitializeFunc) ptr;
        return (nvn::GenericFuncPtrFunc) &deviceInit;
    } else if (strcmp(procName, "nvnDeviceGetProcAddress") == 0) {
        tempGetProcAddressFuncPtr = (nvn::DeviceGetProcAddressFunc) ptr;
        return (nvn::GenericFuncPtrFunc) &getProc;
    }

    return ptr;
}

HOOK_DEFINE_TRAMPOLINE(NvnBootstrapHook) {
    static void *Callback(const char *funcName) {
        void *result = Orig(funcName);

        Logger::log("Installing nvn Hooks.\n");

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
};

INPUT_HOOK(FullKeyState);
INPUT_HOOK(HandheldState);
INPUT_HOOK(JoyDualState);
INPUT_HOOK(JoyLeftState);
INPUT_HOOK(JoyRightState);

void exl_imgui_main() {
    nn::ro::LookupSymbol(reinterpret_cast<uintptr_t *>(&mallocFuncPtr), "malloc");
    nn::ro::LookupSymbol(reinterpret_cast<uintptr_t *>(&freeFuncPtr), "free");
    NvnBootstrapHook::InstallAtSymbol("nvnBootstrapLoader");

#ifndef NDEBUG
    DisableFullKeyState::InstallAtSymbol("_ZN2nn3hid6detail13GetNpadStatesEPiPNS0_16NpadFullKeyStateEiRKj");
    DisableHandheldState::InstallAtSymbol("_ZN2nn3hid6detail13GetNpadStatesEPiPNS0_17NpadHandheldStateEiRKj");
    DisableJoyDualState::InstallAtSymbol("_ZN2nn3hid6detail13GetNpadStatesEPiPNS0_16NpadJoyDualStateEiRKj");
    DisableJoyLeftState::InstallAtSymbol("_ZN2nn3hid6detail13GetNpadStatesEPiPNS0_16NpadJoyLeftStateEiRKj");
    DisableJoyRightState::InstallAtSymbol("_ZN2nn3hid6detail13GetNpadStatesEPiPNS0_17NpadJoyRightStateEiRKj");
#endif
}
