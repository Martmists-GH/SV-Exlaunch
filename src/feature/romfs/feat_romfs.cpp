#include "feat_romfs.h"
#include "checks.hpp"
#include "exlaunch.hpp"
#include "fs/fs_types.hpp"
#include "fs/fs_files.hpp"
#include "externals.hpp"

#if __has_include("feat_logging.h")
#include "feat_logging.h"
#endif


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
DEFINE_EXTERNAL(FlatBufferReadResult (*readFileToBuffer)(FlatBufferReadInfo* readInfo, void* buffer, ulong bufferSize));


namespace sv::feature::romfs {
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

    void exl_main() {
        if (is_version("1.0.0")) {
            EXTERNAL_ADDRESS(readFileToBuffer, 0xBEC958);

            FlatBufferLoaderHook::InstallAtOffset(0xBEBCD4);
            FlatBufferCreateFlatBufferHook::InstallAtOffset(0xBEBB90);
            FlatBufferCreateFlatBuffer2Hook::InstallAtOffset(0x1DD78A4);
            FlatBufferCreateFlatBuffer3Hook::InstallAtOffset(0x1E211B0);

        } else if (is_version("1.0.1")) {
            EXTERNAL_ADDRESS(readFileToBuffer, 0xC2F8B8);

            FlatBufferLoaderHook::InstallAtOffset(0xc2ec30);
            FlatBufferCreateFlatBufferHook::InstallAtOffset(0xc2eaec);
            FlatBufferCreateFlatBuffer2Hook::InstallAtOffset(0x1e27e00);
            FlatBufferCreateFlatBuffer3Hook::InstallAtOffset(0x1e72770);
        } else if (is_version("1.1.0")) {
            EXTERNAL_ADDRESS(readFileToBuffer, 0xC50058);

            FlatBufferLoaderHook::InstallAtOffset(0xC4F3A8);
            FlatBufferCreateFlatBufferHook::InstallAtOffset(0xC4F264);
            FlatBufferCreateFlatBuffer2Hook::InstallAtOffset(0x1E9E1E4);
            FlatBufferCreateFlatBuffer3Hook::InstallAtOffset(0x1EEB2E4);
        } else {
            EXL_ASSERT(false, "Unknown version.");
        }
    }
}
