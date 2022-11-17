#include "patches.hpp"
#include "checks.hpp"

namespace patch = exl::patch;
namespace inst = exl::armv8::inst;
namespace reg = exl::armv8::reg;

void stubSocketInit() {
#if 0
    if (is_version("1.0.0")) {
        patch::CodePatcher p(0xB3BB0C);
        p.WriteInst(inst::Nop());
    } else if (is_version("1.0.1")) {
        patch::CodePatcher p(0xB79F2C);
        p.WriteInst(inst::Nop());
    } else {
        EXL_ASSERT(false, "Unknown version");
    }
#endif
}

void runCodePatches() {
    // stubSocketInit();
}
