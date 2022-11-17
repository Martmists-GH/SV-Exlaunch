#pragma once

#include "../../include/common.hpp"
#include "armv8/register.hpp"
#include "util/math/bitset.hpp"

namespace exl::armv8 {

    using InstType = uint;

    template<InstType... Args>
    using InstMask = util::Mask<InstType, Args...>;

    using InstBitSet = util::BitSet<InstType>;
}

#include "armv8/instructions.hpp"
