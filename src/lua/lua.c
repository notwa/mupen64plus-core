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

		m64p_lua_load_libs(g_luaState);
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


int m64p_lua_return_errcode(lua_State *L, m64p_error err) {
	const char *str = "Unknown error";
	switch(err) {
		case M64ERR_SUCCESS:
			lua_pushboolean(L, 1);
			return 1;

		case M64ERR_NOT_INIT:
			str = "Not initialized";
			break;
		case M64ERR_ALREADY_INIT:
			str = "Already initialized";
			break;
		case M64ERR_INCOMPATIBLE:
			str = "Incompatible API version";
			break;
		case M64ERR_INPUT_ASSERT:
			str = "Invalid parameter";
			break;
		case M64ERR_INPUT_INVALID:
			str = "Invalid parameter value";
			break;
		case M64ERR_INPUT_NOT_FOUND:
			str = "Item not found";
			break;
		case M64ERR_NO_MEMORY:
			str = "Out of memory";
			break;
		case M64ERR_FILES:
			str = "I/O error";
			break;
		case M64ERR_INTERNAL:
			str = "Internal error";
			break;
		case M64ERR_INVALID_STATE:
			str = "Operation not valid in current state";
			break;
		case M64ERR_PLUGIN_FAIL:
			str = "Plugin operation failed";
			break;
		case M64ERR_SYSTEM_FAIL:
			str = "System operation failed";
			break;
		case M64ERR_UNSUPPORTED:
			str = "Operation not supported";
			break;
		case M64ERR_WRONG_TYPE:
			str = "Incorrect parameter type";
			break;
	}

	lua_pushnil(L);
	lua_pushstring(L, str);
	lua_pushinteger(L, err);
	return 3;
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


static void run_callbacks(const char *name, lua_State *L) {
	//printf("calling Lua callback '%s'\n", name);
	lua_getfield(L, LUA_REGISTRYINDEX, name);
	int i=0;
	while(1) {
		lua_rawgeti(L, -1, ++i);
		if(lua_isfunction(L, -1)) {
			//printf("  calling #%d\n", i);
			int err = lua_pcall(L, 0, 0, 0);
			if(err) {
				DebugMessage(M64MSG_ERROR, "Lua %s: %s",
					name, lua_tostring(L, -1));
				lua_pop(L, 1);
			}
		}
		else if(lua_isnil(L, -1)) {
			lua_pop(L, 1);
			break;
		}
		else lua_pop(L, 1);
	}
	lua_pop(L, 1); //remove callbacks table
	//printf("callbacks done\n");
}


void m64p_lua_render_callback() {
	if(g_luaState) run_callbacks("render_callbacks", g_luaState);
}


void m64p_lua_vi_callback() {
	if(g_luaState) run_callbacks("vi_callbacks", g_luaState);
}
