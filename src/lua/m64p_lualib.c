//Mupen64Plus built-in Lua modules.
#include "lua.h"
#include "main/main.h"

enum {
	EMU_FIELD_STATE,
	NUM_EMU_FIELDS
};
static const char *emuFieldName[] = {"state", NULL};


static int emu_meta_index(lua_State *L) {
	lua_getfield(L, LUA_REGISTRYINDEX, "emu_fields"); //-1: fields
	lua_pushvalue(L, 2); //-1: key, -2: fields
	lua_gettable(L, -2); //-1: val, -2: fields
	int field = luaL_optinteger(L, -1, -1);
	lua_pop(L, 2);

	switch(field) {
		case EMU_FIELD_STATE: {
			int state = 0;
			main_core_state_query(M64CORE_EMU_STATE, &state);
			lua_getfield(L, LUA_REGISTRYINDEX, "emu_states"); //-1: states
			lua_rawgeti(L, -1, state); //-1: val, -2: states
			lua_remove(L, -2); //-1: val
			break;
		}

		default:
			lua_pushnil(L);
	}

	return 1;
}


static int emu_meta_newindex(lua_State *L) {
	lua_getfield(L, LUA_REGISTRYINDEX, "emu_fields"); //-1: fields
	lua_pushvalue(L, 2); //-1: key, -2: fields
	lua_gettable(L, -2); //-1: val, -2: fields
	int field = luaL_optinteger(L, -1, -1);
	lua_pop(L, 2);

	switch(field) {
		case EMU_FIELD_STATE: {
			int state = 0, isnum = 0;
			lua_getfield(L, LUA_REGISTRYINDEX, "emu_states"); //-1: states
			lua_pushvalue(L, 3); //-1: key, -2: states
			lua_rawget(L, -2); //-1: state, -2: states
			state = lua_tointegerx(L, -1, &isnum);
			lua_pop(L, 2);

			if(isnum) {
				int err = main_core_state_set(M64CORE_EMU_STATE, state);
				if(err) return luaL_error(L, "%s",m64p_lua_get_err_string(err));
			}

			break;
		}

		default:
			if(lua_tostring(L, 2))
				return luaL_error(L, "Cannot assign to field '%s' in emu",
					lua_tostring(L, 2));
			else return luaL_error(L, "Cannot assign to field (%s) in emu",
				luaL_typename(L, 2));
	}

	return 0;
}


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
	luaL_checktype(L, 3, LUA_TFUNCTION);

	char tname[256];
	const char *name = luaL_checkstring(L, 2);
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

	lua_pushvalue(L, 3); //-1: func, -2: tbl
	lua_rawseti(L, -2, i); //-1: tbl
	lua_pop(L, 1); //remove tbl

	lua_pushboolean(L, 1);
	return 1;
}


static int emu_unregister_callback(lua_State *L) {
	luaL_checktype(L, 3, LUA_TFUNCTION);

	char tname[256];
	const char *name = luaL_checkstring(L, 2);
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
	//emulator state table
	//these don't start at 0 for some reason so we don't use an array of names
	lua_createtable(L, 0, 6);
	LUA_SET_FIELD(L, -1, "stopped", integer, M64EMU_STOPPED);
	LUA_SET_FIELD(L, -1, "running", integer, M64EMU_RUNNING);
	LUA_SET_FIELD(L, -1, "paused",  integer, M64EMU_PAUSED);
	lua_pushstring(L, "stopped"); lua_rawseti(L, -2, M64EMU_STOPPED);
	lua_pushstring(L, "running"); lua_rawseti(L, -2, M64EMU_RUNNING);
	lua_pushstring(L, "paused");  lua_rawseti(L, -2, M64EMU_PAUSED);
	lua_setfield(L, LUA_REGISTRYINDEX, "emu_states");


	//table of emu field names
	lua_createtable(L, 0, NUM_EMU_FIELDS);
	int i; for(i=0; emuFieldName[i]; i++) {
		lua_pushinteger(L, i);
		lua_setfield(L, -2, emuFieldName[i]);
	}
	lua_setfield(L, LUA_REGISTRYINDEX, "emu_fields");


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


	//m64p metatable
	static const luaL_Reg meta_m64p[] = {
		{"__index",    emu_meta_index},
		{"__newindex", emu_meta_newindex},
		{NULL, NULL}
	};
	luaL_newlib(L, meta_m64p); //-1: meta, -2: m64p
	lua_setmetatable(L, -2); //-1: m64p


	//install m64p table as global variable
	lua_setglobal(L, "m64p");
}
