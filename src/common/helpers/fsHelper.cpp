#include "fsHelper.h"
#include "diag/assert.hpp"
#include "init.h"

#if __has_include("feat_logging.h")
#include "feat_logging.h"
#endif

namespace FsHelper {
    nn::Result writeFileToPath(void *buf, size_t size, const char *path) {
        nn::fs::FileHandle handle;

        if (isFileExist(path)) {
#ifdef LOGGING_ENABLED
            Logger::log("Removing Previous File.\n");
#endif
            nn::fs::DeleteFile(path); // remove previous file
        }

        if (nn::fs::CreateFile(path, size)) {
#ifdef LOGGING_ENABLED
            Logger::log("Failed to Create File.\n");
#endif
            return 1;
        }

        if (nn::fs::OpenFile(&handle, path, nn::fs::OpenMode_Write)) {
#ifdef LOGGING_ENABLED
            Logger::log("Failed to Open File.\n");
#endif
            return 1;
        }

        if (nn::fs::WriteFile(handle, 0, buf, size, nn::fs::WriteOption::CreateOption(nn::fs::WriteOptionFlag_Flush))) {
#ifdef LOGGING_ENABLED
            Logger::log("Failed to Write to File.\n");
#endif
            return 1;
        }

#ifdef LOGGING_ENABLED
        Logger::log("Successfully wrote file to: %s!\n", path);
#endif

        nn::fs::CloseFile(handle);

        return 0;
    }

    // make sure to free buffer after usage is done
    void loadFileFromPath(LoadData &loadData) {

        nn::fs::FileHandle handle;

        EXL_ASSERT(FsHelper::isFileExist(loadData.path), "Failed to Find File: %s", loadData.path);

        R_ABORT_UNLESS(nn::fs::OpenFile(&handle, loadData.path, nn::fs::OpenMode_Read))

        long size = 0;
        nn::fs::GetFileSize(&size, handle);
        long alignedSize = ALIGN_UP(size, loadData.alignment);
        Logger::log("File Size: %ld, Aligned Size: %ld\n", size, alignedSize);
        loadData.buffer = malloc(alignedSize);
        loadData.bufSize = alignedSize;

        EXL_ASSERT(loadData.buffer, "Failed to Allocate Buffer! File Size: %ld", size);

        R_ABORT_UNLESS(nn::fs::ReadFile(handle, 0, loadData.buffer, size))

        nn::fs::CloseFile(handle);
    }


    long getFileSize(const char *path) {
        nn::fs::FileHandle handle;
        long result = -1;

        nn::Result openResult = nn::fs::OpenFile(&handle, path, nn::fs::OpenMode::OpenMode_Read);

        if (openResult.isSuccess()) {
            nn::fs::GetFileSize(&result, handle);
            nn::fs::CloseFile(handle);
        }

        return result;
    }

    bool isFileExist(const char *path) {
        nn::fs::DirectoryEntryType type;
        nn::Result result = nn::fs::GetEntryType(&type, path);

        return result.isSuccess() && type == nn::fs::DirectoryEntryType_File;

    }
}
