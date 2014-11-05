#ifndef __M64P_LUA_H__
#define __M64P_LUA_H__

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#define M64P_CORE_PROTOTYPES 1
#include "api/lua.h"
#include "api/m64p_frontend.h"

void m64p_lua_load_libs(lua_State *L);

#endif //__M64P_LUA_H__
