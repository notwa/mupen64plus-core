//Mupen64Plus built-in Lua modules.
#include "lua.h"

static int emu_run(lua_State *L) {
	return m64p_lua_return_errcode(L,
		CoreDoCommand(M64CMD_EXECUTE, 0, NULL));
}

static int emu_stop(lua_State *L) {
	return m64p_lua_return_errcode(L,
		CoreDoCommand(M64CMD_STOP, 0, NULL));
}

static int emu_pause(lua_State *L) {
	return m64p_lua_return_errcode(L,
		CoreDoCommand(M64CMD_PAUSE, 0, NULL));
}

static int emu_resume(lua_State *L) {
	return m64p_lua_return_errcode(L,
		CoreDoCommand(M64CMD_RESUME, 0, NULL));
}

static int emu_register_callback(lua_State *L) {
	luaL_checktype(L, 2, LUA_TFUNCTION);

	char tname[256];
	const char *name = luaL_checkstring(L, 1);
	snprintf(tname, sizeof(tname), "%s_callbacks", name);

	lua_getfield(L, LUA_REGISTRYINDEX, tname);
	if(!lua_istable(L, -1)) return luaL_error(L, "Invalid callback ID");
	//-1: callbacks tbl

	//scan this table until we find an empty slot
	int i=0, done=0;
	while(!done) {
		lua_rawgeti(L, -1, ++i); //-1: slot, -2: tbl
		if(!lua_isfunction(L, -1)) done = 1;
		lua_pop(L, 1); //-1: tbl
	}

	lua_pushvalue(L, 2); //-1: func, -2: tbl
	lua_rawseti(L, -2, i); //-1: tbl
	lua_pop(L, 1); //remove tbl

	lua_pushboolean(L, 1);
	return 1;
}

static int emu_unregister_callback(lua_State *L) {
	luaL_checktype(L, 2, LUA_TFUNCTION);

	char tname[256];
	const char *name = luaL_checkstring(L, 1);
	snprintf(tname, sizeof(tname), "%s_callbacks", name);

	lua_getfield(L, LUA_REGISTRYINDEX, tname);
	if(!lua_istable(L, -1)) return luaL_error(L, "Invalid callback ID");
	//-1: callbacks tbl

	//scan this table until we find the given function.
	int i=0, done=0, found=0;
	while(!done) {
		lua_rawgeti(L, -1, ++i); //-1: slot, -2: tbl
		if(lua_isnil(L, -1)) done = 1;
		else if(lua_compare(L, -1, 2, LUA_OPEQ)) { //value == arg2
			//replace with a non-function, non-nil value.
			lua_pushboolean(L, 0); //-1: false, -2: slot, -3: tbl
			lua_rawseti(L, -3, i); //-1: slot, -2: tbl
			found = 1; done = 1;
		}
		lua_pop(L, 1); //-1: tbl
	}

	lua_pop(L, 1); //remove tbl
	lua_pushboolean(L, found);
	return 1;
}


void m64p_lua_load_libs(lua_State *L) {
	//create callback tables
	lua_newtable(L);
	lua_setfield(L, LUA_REGISTRYINDEX, "vi_callbacks");
	lua_newtable(L);
	lua_setfield(L, LUA_REGISTRYINDEX, "render_callbacks");


	//global m64p table
	static const luaL_Reg funcs_m64p[] = {
		{"run",                emu_run},
		{"stop",               emu_stop},
		{"pause",              emu_pause},
		{"resume",             emu_resume},
		{"registerCallback",   emu_register_callback},
		{"unregisterCallback", emu_unregister_callback},
		{NULL, NULL}
	};
	luaL_newlib(L, funcs_m64p); //-1: m64p


	//load m64p submodules into m64p table
	m64p_lua_load_memlib(L); lua_setfield(L, -2, "memory");
	m64p_lua_load_romlib(L); lua_setfield(L, -2, "rom");


	//install m64p table as global variable
	lua_setglobal(L, "m64p");
}
