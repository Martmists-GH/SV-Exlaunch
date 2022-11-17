#pragma once

#include "common.hpp"

#define EXL_MODULE_NAME MODULE_NAME
#define EXL_MODULE_NAME_LEN MODULE_NAME_LEN

#define EXL_DEBUG

#define EXL_USE_FAKEHEAP

/*
#define EXL_SUPPORTS_REBOOTPAYLOAD
*/

namespace exl::setting {
    /* How large the fake .bss heap will be. */
    constexpr size_t HeapSize = 0x10000;
}
