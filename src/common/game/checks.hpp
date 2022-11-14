#pragma once

#define SCARLET 0x0100A3D008C5C000
#define VIOLET  0x01008F6008C5E000

#define VERSION_1_0_0 0x010000
#define VERSION_1_0_1 0x010001

#define IS_GAME(game) (TITLE_ID == game)
#define IS_VERSION(version) (GAME_VERSION == version)
#define IS_GAME_VERSION(game, version) (IS_GAME(game) && IS_VERSION(version))
