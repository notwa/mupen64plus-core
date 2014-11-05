#include "lua.h"

lua_State *g_luaState = NULL;

int m64p_lua_init() {
	DebugMessage(M64MSG_INFO, "Lua support enabled");
	g_luaState = luaL_newstate();
	luaL_openlibs(g_luaState);
	if(luaL_dostring(g_luaState, "return _VERSION") == 0) {
		DebugMessage(M64MSG_INFO, "Lua version is: %s",
			lua_tostring(g_luaState, -1));
		lua_pop(g_luaState, 1);
		return 1;
	}
	else {
		DebugMessage(M64MSG_ERROR, "Lua execution error: %s",
			lua_tostring(g_luaState, -1));
		lua_close(g_luaState);
		g_luaState = NULL;
		return 0;
	}
}


m64p_error m64p_lua_load_script(const char *path) {
	if(!g_luaState) return M64ERR_INVALID_STATE;

	//get debug.traceback from Lua globals table.
	lua_getglobal(g_luaState, "debug"); //-1: debug
	lua_getfield(g_luaState, -1, "traceback"); //-1: trace, -2: debug
	lua_remove(g_luaState, -2); //-1: trace

	//load the script
	int res = luaL_loadfile(g_luaState, path);
	if(res == LUA_OK) {
		//loaded OK, now execute it
		res = lua_pcall(g_luaState, 0, 0, -2);
		if(res == LUA_OK) return M64ERR_SUCCESS;
	}

	//failed to load script. output an explanation why
	DebugMessage(M64MSG_ERROR, "Failed to load Lua script '%s': %s",
		path, lua_tostring(g_luaState, -1));
	lua_pop(g_luaState, 2); //remove error and traceback function

	//return an error code
	switch(res) {
		case LUA_ERRFILE:   return M64ERR_INPUT_NOT_FOUND;
		case LUA_ERRSYNTAX: return M64ERR_INPUT_INVALID;
		case LUA_ERRMEM:    return M64ERR_NO_MEMORY;
		case LUA_ERRGCMM:   return M64ERR_NO_MEMORY;
		default:
			DebugMessage(M64MSG_ERROR,
				"Unrecognized error code %d from luaL_loadfile", res);
			return M64ERR_INTERNAL;
	}
}
