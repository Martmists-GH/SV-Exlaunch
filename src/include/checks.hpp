#pragma once

#include "exlaunch/nx/kernel/svc.h"
#include "oe.h"
#include <cstring>

#define SCARLET 0x0100A3D008C5C000
#define VIOLET  0x01008F6008C5E000

inline bool is_game(u64 game_id) {
    u64 id;
    svcGetInfo(&id, 18, CUR_PROCESS_HANDLE, 0);
    return id == game_id;
}

inline bool is_version(const char* version) {
    nn::oe::DisplayVersion display_version;
    nn::oe::GetDisplayVersion(&display_version);
    return strcmp(display_version.name, version) == 0;
}

inline bool is_game_version(u64 game_id, const char* version) {
    return is_game(game_id) && is_version(version);
}
