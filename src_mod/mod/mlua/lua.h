#pragma once

#include "lua.hpp"
#include <cstring>

#define LUA_ENABLED 1

extern int (*lua_rawgeti) (lua_State *L_, int idx, lua_Integer n);
extern void (*lua_pushnil) (lua_State *L_);
extern void (*lua_settop) (lua_State *L_, int idx);
extern const char *(*lua_tolstring) (lua_State *L_, int idx, size_t *len);
extern const char *(*luaL_tolstring) (lua_State *L_, int idx, size_t *len);
extern int (*lua_next) (lua_State *L_, int idx);
extern int (*lua_gettop) (lua_State *L_);
extern int (*luaB_load) (lua_State *L_);
extern int (*lua_pcallk) (lua_State *L_, int nargs, int nresults, int errfunc, lua_KContext ctx, lua_KFunction k);
extern int (*luaL_loadbufferx) (lua_State *L_, const char *buff, size_t size, const char *name, const char *mode);
extern void (*lua_pushcclosure) (lua_State *L_, lua_CFunction fn, int n);
extern void (*lua_createtable) (lua_State *L_, int narr, int nrec);
extern const char *(*luaL_checklstring) (lua_State *L_, int arg, size_t *L2_);
extern lua_Integer (*luaL_checkinteger) (lua_State *L_, int arg);
extern void *(*lua_newuserdatauv) (lua_State *L_, size_t sz, int nuvalue);
extern void (*lua_setfield) (lua_State *L_, int idx, const char *k);
extern void *(*lua_touserdata) (lua_State *L_, int idx);
extern const char *(*lua_pushstring) (lua_State *L_, const char *s);

inline int luaL_loadstring (lua_State *L_, const char *s) {
    return luaL_loadbuffer(L_, s, strlen(s), s);
}

void exl_lua_main();
lua_State* get_lua_state();

#define L get_lua_state()
