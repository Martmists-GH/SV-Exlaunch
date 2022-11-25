# SV-Exlaunch

This repository is intended to be a template for creating your Scarlet/Violet mod. While this repository is licensed under BSD-3, it contains code from GPL projects, as outlined in the [NOTICE](NOTICE) file.

The repository builds two mods by default; Your own mod (called TestMod in config.cmake by default) and a RomFS Redirection mod. 
The latter is written entirely by CraftyBoss and only included here to have it in one place, and ensure it does not conflict with other mods, as each subsdk1 file built from this repository would be identical and interchangeable.

## Building

### Prerequisites

- devkitPro
- devkitA64
- cmake

### Configuring

See ["A guide on ExeFS modding for Scarlet/Violet"](https://gamebanana.com/tuts/15485)
I recommend removing the Logger feature in release builds.

## Credits

- [CraftyBoss](https://github.com/CraftyBoss) 
  - [Original repo](https://github.com/CraftyBoss/Scarlet-Exlaunch-Base)
  - RomFS Redirection impl for 1.0.0
- [Shadow](https://github.com/shadowninja108)
  - [Exlaunch](https://github.com/shadowninja108/exlaunch)
