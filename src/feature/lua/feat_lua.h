#pragma once

#include "lapi.h"
#include "lauxlib.h"
#include "lua.h"
#include <cstring>

#define LUA_ENABLED 1

static int (*lua_rawgeti) (lua_State *L, int idx, lua_Integer n);
static void (*lua_pushnil) (lua_State *L);
static void (*lua_settop) (lua_State *L, int idx);
static const char *(*lua_tolstring) (lua_State *L, int idx, size_t *len);
static const char *(*luaL_tolstring) (lua_State *L, int idx, size_t *len);
static int (*lua_next) (lua_State *L, int idx);
static int (*lua_gettop) (lua_State *L);
static int (*luaB_load) (lua_State *L);
static int (*lua_pcallk) (lua_State *L, int nargs, int nresults, int errfunc, lua_KContext ctx, lua_KFunction k);
static int (*luaL_loadbufferx) (lua_State *L, const char *buff, size_t size, const char *name, const char *mode);
static void (*lua_pushcclosure) (lua_State *L, lua_CFunction fn, int n);
static void (*lua_createtable) (lua_State *L, int narr, int nrec);
static const char *(*luaL_checklstring) (lua_State *L, int arg, size_t *l);
static lua_Integer (*luaL_checkinteger) (lua_State *L, int arg);
static void *(*lua_newuserdatauv) (lua_State *L, size_t sz, int nuvalue);
static void (*lua_setfield) (lua_State *L, int idx, const char *k);
static void *(*lua_touserdata) (lua_State *L, int idx);
static const char *(*lua_pushstring) (lua_State *L, const char *s);

inline int luaL_loadstring (lua_State *L, const char *s) {
    return luaL_loadbuffer(L, s, strlen(s), s);
}

namespace sv::feature::lua {
    void exl_main();
}
