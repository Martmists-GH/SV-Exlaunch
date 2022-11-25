#include "exlaunch.hpp"
#include "fs/fs_types.hpp"
#include "fs/fs_files.hpp"
#include "checks.hpp"
// Source: https://github.com/CraftyBoss/Scarlet-Exlaunch-Base/commit/fe9bdb44f90261308f389b81bc26964ec6455e8a

struct FlatBufferReadResult {
    u64 unk1;
    u64 unk2;
};

struct FlatBufferReadInfo {
    long mSize;
    long mPosition;
    nn::fs::FileHandle mHandle;
};

struct FlatBufferLoaderInfo
{
    char gap_0[56];
    const char *mPath;
};

struct FlatBufferLoader
{
    void *__vftable;
    void *field_8;
    char gap_10[48];
    FlatBufferLoaderInfo *mFileInfo;
    char gap_48[88];
    int field_A0;
    int field_A4;
    bool field_A8;
    FlatBufferReadInfo mReadInfo;
    char gap_C8[128];
    void *field_148;
    void *field_150;
    void *field_158;
    void *field_160;
    void *mAllocator1;
    char gap_170[16];
    int field_180;
    void *struct_188;
    void *field_190;
    char gap_188[64];
    long mBufferSize;
    long mReadPosition;
    void *mFlatBuffer;
    void *mAllocator2;
    void *field_1F8;
    void *field_200;
};

static FlatBufferReadInfo newReadInfo;
static nn::Result readResult = -1;
static FlatBufferReadResult (*readFileToBuffer)(FlatBufferReadInfo* readInfo, void* buffer, ulong bufferSize);

void NOINLINE fileRedirectionFunc(FlatBufferLoader *loader) {
    if(loader->mFileInfo && loader->field_148 && loader->field_A8) {
        readResult = nn::fs::OpenFile(&newReadInfo.mHandle, loader->mFileInfo->mPath, nn::fs::OpenMode::OpenMode_Read);

        if(readResult.isSuccess()) {
            nn::fs::GetFileSize(&newReadInfo.mSize, newReadInfo.mHandle);
            loader->mBufferSize = newReadInfo.mSize;
        }
    }
}

HOOK_DEFINE_TRAMPOLINE(FlatBufferCreateFlatBufferHook) {
    static FlatBufferReadResult Callback(FlatBufferLoader *thisPtr) {
        fileRedirectionFunc(thisPtr);
        return Orig(thisPtr);
    }
};

HOOK_DEFINE_TRAMPOLINE(FlatBufferCreateFlatBuffer2Hook) {
    static FlatBufferReadResult Callback(FlatBufferLoader *thisPtr) {
        fileRedirectionFunc(thisPtr);
        return Orig(thisPtr);
    }
};

HOOK_DEFINE_TRAMPOLINE(FlatBufferCreateFlatBuffer3Hook) {
    static FlatBufferReadResult Callback(FlatBufferLoader *thisPtr) {
        fileRedirectionFunc(thisPtr);
        return Orig(thisPtr);
    }
};

HOOK_DEFINE_TRAMPOLINE(FlatBufferLoaderHook) {
    static FlatBufferReadResult Callback(FlatBufferLoader *thisPtr, void *buffer, ulong bufferSize) {

        if(thisPtr->mFileInfo && thisPtr->field_148 && thisPtr->field_A8) {

            if(readResult.isSuccess()) {

                newReadInfo.mPosition = 0;

                auto result = readFileToBuffer(&newReadInfo, buffer, bufferSize);

                thisPtr->field_180 = 1;
                thisPtr->mReadPosition = newReadInfo.mPosition;

                return result;
            }
        }

        return Orig(thisPtr, buffer, bufferSize);
    }
};

extern "C" void exl_main(void* x0, void* x1) {
    /* Setup hooking enviroment. */
    envSetOwnProcessHandle(exl::util::proc_handle::Get());
    exl::hook::Initialize();

    if (is_version("1.0.0")) {
        readFileToBuffer = reinterpret_cast<FlatBufferReadResult (*)(FlatBufferReadInfo *, void *, ulong)>(exl::util::modules::GetTargetOffset(0xBEC958));

        FlatBufferLoaderHook::InstallAtOffset(0xBEBCD4);
        FlatBufferCreateFlatBufferHook::InstallAtOffset(0xBEBB90);
        FlatBufferCreateFlatBuffer2Hook::InstallAtOffset(0x1DD78A4);
        FlatBufferCreateFlatBuffer3Hook::InstallAtOffset(0x1E211B0);

    } else if (is_version("1.0.1")) {
        readFileToBuffer = reinterpret_cast<FlatBufferReadResult (*)(FlatBufferReadInfo *, void *, ulong)>(exl::util::modules::GetTargetOffset(0xc2f8b8));

        FlatBufferLoaderHook::InstallAtOffset(0xc2ec30);
        FlatBufferCreateFlatBufferHook::InstallAtOffset(0xc2eaec);
        FlatBufferCreateFlatBuffer2Hook::InstallAtOffset(0x1e27e00);
        FlatBufferCreateFlatBuffer3Hook::InstallAtOffset(0x1e72770);
    } else {
        EXL_ASSERT(false, "Unknown version.");
    }
}

extern "C" NORETURN void exl_exception_entry() {
    /* TODO: exception handling */
    EXL_ABORT(0x420);
}