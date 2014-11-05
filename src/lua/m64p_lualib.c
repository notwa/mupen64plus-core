//Mupen64Plus built-in Lua modules.
#include "lua.h"
#include "debugger/dbg_memory.h"

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


static int mem_meta_index(lua_State *L) {
	if(lua_isinteger(L, 2)) {
		uint32 addr = luaL_checkinteger(L, 2);
		lua_pushinteger(L, read_memory_8(addr));
		return 1;
	}
	lua_pushnil(L);
	return 1;
}


static int mem_meta_newindex(lua_State *L) {
	if(lua_isinteger(L, 2)) {
		uint32 addr = luaL_checkinteger(L, 2);
		uint8  val  = luaL_checkinteger(L, 3);
		write_memory_8(addr, val);
		return 0;
	}
	return 0;
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


	//m64p.memory table
	static const luaL_Reg funcs_mem[] = {
		//{"open",   rom_open},
		{NULL, NULL}
	};
	luaL_newlib(L, funcs_mem); //-1: mem, -2: m64p

	//m64p.memory metatable
	static const luaL_Reg meta_mem[] = {
		{"__index",    mem_meta_index},
		{"__newindex", mem_meta_newindex},
		{NULL, NULL}
	};
	luaL_newlib(L, meta_mem); //-1: meta, -2: mem, -3: m64p
	lua_setmetatable(L, -2); //-1: mem, -2: m64p
	lua_setfield(L, -2, "memory"); //-1: m64p


	lua_setglobal(L, "m64p");
}
