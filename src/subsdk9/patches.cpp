#include "patches.hpp"
#include "game/checks.hpp"

namespace patch = exl::patch;
namespace inst = exl::armv8::inst;
namespace reg = exl::armv8::reg;

void stubSocketInit() {
#if IS_VERSION(VERSION_1_0_0)
    patch::CodePatcher p(0xB3BB0C);
    p.WriteInst(inst::Nop());
#elif IS_VERSION(VERSION_1_0_1)
    patch::CodePatcher p(0xB79F2C);
    p.WriteInst(inst::Nop());
#endif
}

void runCodePatches() {
    // stubSocketInit();
}
