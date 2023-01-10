/*
 * Copyright (c) Atmosphère-NX
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "abort.hpp"

#include <cstring>
#include <algorithm>
#include <atomic>

namespace exl::diag {

    void NORETURN NOINLINE AbortImpl(const AbortCtx & ctx) {
        #ifdef EXL_SUPPORTS_REBOOTPAYLOAD
        /* Ensure abort handler doesn't recursively abort. */
        static std::atomic<bool> recurse_guard;
        auto recursing = recurse_guard.exchange(true);

        if(!recursing && util::IsSocErista()) {
            /* Reboot to abort payload.*/
            AbortToPayload(ctx);
        } else 
        #endif
        {
            /* We have no capability of chainloading payloads on mariko. */
            /* Don't have a great solution for this at the moment, just data abort. */
            /* TODO: maybe write to a file? custom fatal program? */
            register u64 x21 __asm__("x21") = ctx.m_Result;
            register u64 x22 __asm__("x22") = reinterpret_cast<u64>(ctx.file);
            register u64 x23 __asm__("x23") = ctx.line;
            register u64 x24 __asm__("x24") = reinterpret_cast<u64>(ctx.func);
            register u64 x25 __asm__("x25") = reinterpret_cast<u64>(ctx.expr);
            register u64 x26 __asm__("x26") = reinterpret_cast<u64>(ctx.message);
            register u64 addr __asm__("x27") = 0x6969696969696969;
            register u64 val __asm__("x28")  = ctx.m_Result;
            while (true) {
                __asm__ __volatile__ (
                    "mov x21, %[x21]\n"
                    "mov x22, %[x22]\n"
                    "mov x23, %[x23]\n"
                    "mov x24, %[x24]\n"
                    "mov x25, %[x25]\n"
                    "mov x26, %[x26]\n"
                    "str %[val], [%[addr]]\n"  // Segfault to halt execution.
                    :
                    : [val]"r"(val), [addr]"r"(addr), [x21]"r"(x21), [x22]"r"(x22), [x23]"r"(x23), [x24]"r"(x24), [x25]"r"(x25), [x26]"r"(x26)
                );
            }
        }

        UNREACHABLE;
    }

    #define ABORT_WITH_VALUE(v)                             \
    {                                                       \
        exl::diag::AbortCtx ctx {.m_Result = (Result)v};    \
        AbortImpl(ctx);                                     \
    }

    #define ABORT_FULL(file, line, func, expr, value)        \
    {                                                       \
        exl::diag::AbortCtx ctx {                           \
            .file = (char *)file,                           \
            .line = (u32)line,                              \
            .func = (char *)func,                           \
            .expr = (char *)expr,                           \
            .m_Result = (Result)value                       \
        };                                                  \
        AbortImpl(ctx);                                     \
    }

    #define ABORT_FULL_MESSAGE(file, line, func, expr, value, format) \
    {                                                                 \
        char message[0x100]; \
        va_list args; \
        va_start(args, format); \
        std::vsnprintf(message, sizeof(message), format, args); \
        va_end(args); \
        exl::diag::AbortCtx ctx {                                     \
            .file = (char *)file,                           \
            .line = (u32)line,                              \
            .func = (char *)func,                           \
            .expr = (char *)expr,                           \
            .m_Result = (Result)value,                      \
            .message = (char *)message                       \
        };                                                  \
        AbortImpl(ctx);                                     \
    }

    /* TODO: better assert/abort support. */
    void NORETURN NOINLINE AssertionFailureImpl(const char *file, int line, const char *func, const char *expr, u64 value, const char *format, ...) ABORT_FULL_MESSAGE(file, line, func, expr, value, format)
    void NORETURN NOINLINE AssertionFailureImpl(const char *file, int line, const char *func, const char *expr, u64 value)                          ABORT_FULL(file, line, func, expr, value)
    void NORETURN NOINLINE AbortImpl(const char *file, int line, const char *func, const char *expr, u64 value, const char *format, ...)            ABORT_FULL_MESSAGE(file, line, func, expr, value, format)
    void NORETURN NOINLINE AbortImpl(const char *file, int line, const char *func, const char *expr, u64 value)                                     ABORT_FULL(file, line, func, expr, value)

};

/* C shim for libnx */
extern "C" NORETURN void exl_abort(Result r) 
    ABORT_WITH_VALUE(r)
