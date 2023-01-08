#include "patches.hpp"
#include "checks.hpp"

namespace patch = exl::patch;
namespace inst = exl::armv8::inst;
namespace reg = exl::armv8::reg;

void changeGameVersion(u64 targetVersion) {
    int addr;

    if (is_version("1.0.0")) {
        EXL_ABORT(0, "Not supported yet!");
    } else if (is_version("1.0.1")) {
        addr = 0xB79F2C;
    } else {
        EXL_ABORT(0, "Unknown version");
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

void runCodePatches() {
    // changeGameVersion();
}
