#include "patches.hpp"
#include "checks.hpp"

namespace patch = exl::patch;
namespace inst = exl::armv8::inst;
namespace reg = exl::armv8::reg;

void changeGameVersion(u64 targetVersion) {
    int addr;

    if (is_version("1.0.0")) {
        EXL_ABORT(false, "Not supported yet!");
    } else if (is_version("1.0.1")) {
        addr = 0xB79F2C;
    } else {
        EXL_ASSERT(false, "Unknown version");
    }

    patch::CodePatcher p(addr);

    if (targetVersion == SCARLET) {
        p.WriteInst(inst::MovRegister(reg::W1, reg::None32));
        p.WriteInst(inst::Movz(reg::W0, 0x32));
    } else if (targetVersion == VIOLET) {
        p.WriteInst(inst::Movz(reg::W1, 1));
        p.WriteInst(inst::Movz(reg::W0, 0x33));
    }

    // TODO: Patch encounter table
    // At some point, patch (*0x042D0658)+0x2C
    // For scarlet, set to 0
    // For violet, set to 1
}

static float speedMultiplier = 1.5;

void patchGameSpeed() {
    int addr;

    if (is_version("1.0.0")) {
        addr = 0x3871798;
    } else if (is_version("1.0.1")) {
        addr = 0x3826ed8;
    } else {
        EXL_ASSERT(false, "Unknown version");
    }

    patch::CodePatcher p(addr);
    p.Write((int)(speedMultiplier * 0x1FCA056));
}

void runCodePatches() {
    // changeGameVersion();
    // TODO: Call this from a menu instead?
    patchGameSpeed();
}
