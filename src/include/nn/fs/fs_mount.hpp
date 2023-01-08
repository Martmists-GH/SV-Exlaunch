#pragma once

#include "fs_types.hpp"

namespace nn::fs {

    /*
        Mount SD card. Must have explicit permission.
        mount: drive to mount to.
    */
    Result MountSdCardForDebug(char const* mount);

    void QueryMountRomCacheSize(uint64_t* out);
    void MountRom(char const* mount, void* cache, uint64_t cache_size);
};
