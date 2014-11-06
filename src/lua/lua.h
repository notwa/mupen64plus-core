#ifndef __M64P_LUA_H__
#define __M64P_LUA_H__

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <string.h>

#define M64P_CORE_PROTOTYPES 1
#include "api/lua.h"
#include "api/m64p_frontend.h"

void m64p_lua_load_libs(lua_State *L);

#define LUA_PUSH(L, type, val) lua_push ##type((L), (val))
#define LUA_SET_FIELD(L, idx, name, type, val) do { \
	lua_push ##type((L), (val)); \
	lua_setfield((L), (idx) < 0 ? ((idx)-1) : (idx), (name)); \
} while(0)

#endif //__M64P_LUA_H__
