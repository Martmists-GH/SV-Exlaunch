#include "checks.hpp"
#include "exlaunch.hpp"
#include "fs/fs_types.hpp"
#include "fs/fs_files.hpp"
#include "externals.hpp"
#include "helpers/fsHelper.h"
#include "helpers/MessageHelper.h"
#include "logger/logger.h"

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

DEFINE_EXTERNAL(FlatBufferReadResult (*readFileToBuffer)(FlatBufferReadInfo* readInfo, void* buffer, ulong bufferSize));


void NOINLINE fileRedirectionFunc(FlatBufferLoader *loader) {
    if(loader->mFileInfo && loader->field_148 && loader->field_A8) {
        if (FsHelper::isFileExist(loader->mFileInfo->mPath)) {
            long fileSize = FsHelper::getFileSize(loader->mFileInfo->mPath);
            loader->mBufferSize = fileSize;
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
            Logger::log("File accessed: %s\n", thisPtr->mFileInfo->mPath);
            
            if(FsHelper::isFileExist(thisPtr->mFileInfo->mPath)) {
                FlatBufferReadInfo readInfo;
                readInfo.mPosition = 0;

                nn::Result openResult = nn::fs::OpenFile(&readInfo.mHandle, thisPtr->mFileInfo->mPath, nn::fs::OpenMode::OpenMode_Read);
                if(openResult.isSuccess()) {
                    auto result = readFileToBuffer(&readInfo, buffer, bufferSize);

                    nn::fs::CloseFile(readInfo.mHandle);

                    thisPtr->field_180 = 1;
                    thisPtr->mReadPosition = readInfo.mPosition;
                    return result;
                } else {
                    MessageHelper::abort(0, "File Read Failed.", "File Read Failed! Unable to Continue. Path: %s\n", thisPtr->mFileInfo->mPath);
                }
            }
        }

        return Orig(thisPtr, buffer, bufferSize);
    }
};

void exl_romfs_main() {
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
    } else if (is_version("1.2.0")) {
        EXTERNAL_ADDRESS(readFileToBuffer, 0x00bd8b78);

        FlatBufferLoaderHook::InstallAtOffset(0x00bd7ec4);
        FlatBufferCreateFlatBufferHook::InstallAtOffset(0x00bd7d88);
        FlatBufferCreateFlatBuffer2Hook::InstallAtOffset(0x01f12cbc);
        FlatBufferCreateFlatBuffer3Hook::InstallAtOffset(0x01f5f8e0);
    }
}

