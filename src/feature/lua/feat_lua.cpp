#include "feat_lua.h"
#include "checks.hpp"
#include "exlaunch/hook/replace_hook.hpp"
#include "exlaunch/hook/trampoline_hook.hpp"
#include "lua.hpp"
#include "externals.hpp"

#if __has_include("feat_logging.h")
#include "feat_logging.h"
#endif

int (*lua_rawgeti) (lua_State *L, int idx, lua_Integer n);
void (*lua_pushnil) (lua_State *L);
void (*lua_settop) (lua_State *L, int idx);
const char *(*lua_tolstring) (lua_State *L, int idx, size_t *len);
const char *(*luaL_tolstring) (lua_State *L, int idx, size_t *len);
int (*lua_next) (lua_State *L, int idx);
int (*lua_gettop) (lua_State *L);
int (*luaB_load) (lua_State *L);
int (*lua_pcallk) (lua_State *L, int nargs, int nresults, int errfunc, lua_KContext ctx, lua_KFunction k);
int (*luaL_loadbufferx) (lua_State *L, const char *buff, size_t size, const char *name, const char *mode);
void (*lua_pushcclosure) (lua_State *L, lua_CFunction fn, int n);
void (*lua_createtable) (lua_State *L, int narr, int nrec);
const char *(*luaL_checklstring) (lua_State *L, int arg, size_t *l);
lua_Integer (*luaL_checkinteger) (lua_State *L, int arg);
void *(*lua_newuserdatauv) (lua_State *L, size_t sz, int nuvalue);
void (*lua_setfield) (lua_State *L, int idx, const char *k);
void *(*lua_touserdata) (lua_State *L, int idx);
const char *(*lua_pushstring) (lua_State *L, const char *s);

namespace sv::feature::lua {
    HOOK_DEFINE_TRAMPOLINE(LuaOpenBit32Hook){
        static int Callback(lua_State *L) {
            int result = Orig(L);

            luaL_dostring(L, "print('Hello from Lua Hook!')");

            return result;
        }
    };

    HOOK_DEFINE_TRAMPOLINE(LuaPanicHook) {
        static void Callback(lua_State *L) {
#ifdef LOGGING_ENABLED
            size_t size;
            const char *message = lua_tolstring(L, -1, &size);
            Logger::log("Lua Panic: %.*s\n", size, message);
#endif
            Orig(L);
        }
    };

    HOOK_DEFINE_REPLACE(LuaPrintSocket) {
        static int Callback(lua_State *L) {
#ifdef LOGGING_ENABLED
            int n = lua_gettop(L);
            Logger::log("Lua Print:");
            for (int i = 1; i <= n; i++) {
                size_t size;
                const char *message = lua_tolstring(L, i, &size);
                Logger::log(" %.*s", size, message);
            }
            lua_settop(L, 0);
            Logger::log("\n");
#endif
            return 0;
        }
    };

    void exl_main() {
        if (is_version("1.0.0")) {
            LuaOpenBit32Hook::InstallAtOffset(0x4f430);
            LuaPanicHook::InstallAtOffset(0x2374ce4);
            LuaPrintSocket::InstallAtOffset(0x45f80);

            EXTERNAL_ADDRESS(lua_rawgeti, 0x99c9e0);
            EXTERNAL_ADDRESS(lua_pushnil, 0x9e3ad0);
            EXTERNAL_ADDRESS(lua_settop, 0x99df80);
            EXTERNAL_ADDRESS(lua_tolstring, 0x9aeb10);
            EXTERNAL_ADDRESS(luaL_tolstring, 0x24c10);
            EXTERNAL_ADDRESS(lua_next, 0xaefae0);
            EXTERNAL_ADDRESS(lua_gettop, 0x99df60);
            EXTERNAL_ADDRESS(luaB_load, 0x45c60);
            EXTERNAL_ADDRESS(lua_pcallk, 0x99e3b0);
            EXTERNAL_ADDRESS(luaL_loadbufferx, 0xd69e70);
            EXTERNAL_ADDRESS(lua_pushcclosure, 0x99e1c0);
            EXTERNAL_ADDRESS(lua_createtable, 0xa26520);
            EXTERNAL_ADDRESS(luaL_checklstring, 0x240e0);
            EXTERNAL_ADDRESS(luaL_checkinteger, 0x24330);
            EXTERNAL_ADDRESS(lua_newuserdatauv, 0x99e700);
            EXTERNAL_ADDRESS(lua_setfield, 0xd37df0);
            EXTERNAL_ADDRESS(lua_touserdata, 0x989980);
            EXTERNAL_ADDRESS(lua_pushstring, 0x9c8c50);
        } else if (is_version("1.0.1")) {
            LuaOpenBit32Hook::InstallAtOffset(0x4f5e0);
            LuaPanicHook::InstallAtOffset(0x23d69e8);
            LuaPrintSocket::InstallAtOffset(0x46130);

            EXTERNAL_ADDRESS(lua_rawgeti, 0x9bc0e0);
            EXTERNAL_ADDRESS(lua_pushnil, 0xa008f0);
            EXTERNAL_ADDRESS(lua_settop, 0x9b9290);
            EXTERNAL_ADDRESS(lua_tolstring, 0x9ca610);
            EXTERNAL_ADDRESS(luaL_tolstring, 0x24c20);
            EXTERNAL_ADDRESS(lua_next, 0xb21d00);
            EXTERNAL_ADDRESS(lua_gettop, 0x9b9270);
            EXTERNAL_ADDRESS(luaB_load, 0x45e10);
            EXTERNAL_ADDRESS(lua_pcallk, 0x9b96c0);
            EXTERNAL_ADDRESS(luaL_loadbufferx, 0xd9eef0);
            EXTERNAL_ADDRESS(lua_pushcclosure, 0x9b94d0);
            EXTERNAL_ADDRESS(lua_createtable, 0xa5f0c0);
            EXTERNAL_ADDRESS(luaL_checklstring, 0x240f0);
            EXTERNAL_ADDRESS(luaL_checkinteger, 0x24340);
            EXTERNAL_ADDRESS(lua_newuserdatauv, 0x9b9a10);
            EXTERNAL_ADDRESS(lua_setfield, 0xd70870);
            EXTERNAL_ADDRESS(lua_touserdata, 0x9a4a70);
            EXTERNAL_ADDRESS(lua_pushstring, 0x9e48e0);
        } else if (is_version("1.1.0")) {
            LuaOpenBit32Hook::InstallAtOffset(0x4f5e0);
            LuaPanicHook::InstallAtOffset(0x27a9384);
            LuaPrintSocket::InstallAtOffset(0x46130);

            EXTERNAL_ADDRESS(lua_rawgeti, 0x9cb5b0);
            EXTERNAL_ADDRESS(lua_pushnil, 0xa10b90);
            EXTERNAL_ADDRESS(lua_settop, 0x9c86f0);
            EXTERNAL_ADDRESS(lua_tolstring, 0x9d99a0);
            EXTERNAL_ADDRESS(luaL_tolstring, 0x24c20);
            EXTERNAL_ADDRESS(lua_next, 0xb34fe0);
            EXTERNAL_ADDRESS(lua_gettop, 0x9c86d0);
            EXTERNAL_ADDRESS(luaB_load, 0x45e10);
            EXTERNAL_ADDRESS(lua_pcallk, 0x9c8b20);
            EXTERNAL_ADDRESS(luaL_loadbufferx, 0xdc6a80);
            EXTERNAL_ADDRESS(lua_pushcclosure, 0x9c8930);
            EXTERNAL_ADDRESS(lua_createtable, 0xa71400);
            EXTERNAL_ADDRESS(luaL_checklstring, 0x240f0);
            EXTERNAL_ADDRESS(luaL_checkinteger, 0x24340);
            EXTERNAL_ADDRESS(lua_newuserdatauv, 0x9c8e70);
            EXTERNAL_ADDRESS(lua_setfield, 0xd92ed0);
            EXTERNAL_ADDRESS(lua_touserdata, 0x9b2950);
            EXTERNAL_ADDRESS(lua_pushstring, 0x9f46f0);
        } else {
            EXL_ASSERT(false, "Unknown version");
        }
#undef EXTERNAL_ADDRESS
    }
}
