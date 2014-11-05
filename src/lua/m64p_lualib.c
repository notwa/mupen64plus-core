//Mupen64Plus built-in Lua modules.
#include "lua.h"


static int rom_open(lua_State *L) {
	const char *path = luaL_checkstring(L, 1);
	return m64p_lua_return_errcode(L,
		CoreDoCommand(M64CMD_ROM_OPEN, 0, (void*)path));
}

static int rom_close(lua_State *L) {
	return m64p_lua_return_errcode(L,
		CoreDoCommand(M64CMD_ROM_CLOSE, 0, NULL));
}

static int rom_meta_index(lua_State *L) {
	//XXX
	printf("rom __index\n");
	lua_pushnil(L);
	return 1;
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


void m64p_lua_load_libs(lua_State *L) {
	//global m64p table
	static const luaL_Reg funcs_m64p[] = {
		{"run",    emu_run},
		{"stop",   emu_stop},
		{"pause",  emu_pause},
		{"resume", emu_resume},
		{NULL, NULL}
	};
	luaL_newlib(L, funcs_m64p); //-1: m64p


	//m64p.rom table
	static const luaL_Reg funcs_rom[] = {
		{"open",   rom_open},
		{"close",  rom_close},
		{NULL, NULL}
	};
	luaL_newlib(L, funcs_rom); //-1: rom, -2: m64p

	//m64p.rom metatable
	static const luaL_Reg meta_rom[] = {
		{"__index", rom_meta_index},
		{NULL, NULL}
	};
	luaL_newlib(L, meta_rom); //-1: meta, -2: rom, -3: m64p
	lua_setmetatable(L, -2); //-1: rom, -2: m64p
	lua_setfield(L, -2, "rom"); //-1: m64p


	lua_setglobal(L, "m64p");
}
