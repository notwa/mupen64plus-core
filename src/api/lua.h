#if !defined(API_LUA_H)
#define API_LUA_H

#include "m64p_types.h"
#include "callbacks.h"

extern int m64p_lua_init();
extern m64p_error m64p_lua_load_script(const char *path);
extern int m64p_lua_return_errcode(lua_State *L, m64p_error err);
extern lua_State *g_luaState;

#endif //API_LUA_H
