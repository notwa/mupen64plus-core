//Mupen64Plus built-in Lua modules.
#include "lua.h"
#include "debugger/dbg_memory.h"

enum {
	ROM_FIELD_HEADER,
	ROM_FIELD_SETTINGS,
	NUM_ROM_FIELDS
};
static const char *romFieldName[] = {"header", "settings", NULL};


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
	lua_getfield(L, LUA_REGISTRYINDEX, "rom_fields"); //-1: fields
	lua_pushvalue(L, 2); //-1: key, -2: fields
	lua_gettable(L, -2); //-1: val, -2: fields
	int field = luaL_optinteger(L, -1, -1);
	lua_pop(L, 2);

	switch(field) {
		case ROM_FIELD_HEADER: {
			m64p_rom_header header;
			if(CoreDoCommand(M64CMD_ROM_GET_HEADER, sizeof(header), &header)) {
				lua_pushnil(L);
				break;
			}

			lua_createtable(L, 0, 16); //-1: tbl
			LUA_SET_FIELD(L, -1, "init_PI_BSB_DOM1_LAT_REG", integer,
				header.init_PI_BSB_DOM1_LAT_REG);
			LUA_SET_FIELD(L, -1, "init_PI_BSB_DOM1_PGS_REG", integer,
				header.init_PI_BSB_DOM1_PGS_REG);
			LUA_SET_FIELD(L, -1, "init_PI_BSB_DOM1_PWD_REG", integer,
				header.init_PI_BSB_DOM1_PWD_REG);
			LUA_SET_FIELD(L, -1, "init_PI_BSB_DOM1_PGS_REG2", integer,
				header.init_PI_BSB_DOM1_PGS_REG2);
			LUA_SET_FIELD(L, -1, "ClockRate", integer, header.ClockRate);
			LUA_SET_FIELD(L, -1, "PC",        integer, header.PC);
			LUA_SET_FIELD(L, -1, "Release",   integer, header.Release);
			LUA_SET_FIELD(L, -1, "CRC1",      integer, header.CRC1);
			LUA_SET_FIELD(L, -1, "CRC2",      integer, header.CRC2);
			LUA_SET_FIELD(L, -1, "_x18",      integer, header.Unknown[0]);
			LUA_SET_FIELD(L, -1, "_x1C",      integer, header.Unknown[1]);

			lua_pushlstring(L, (char*)header.Name, sizeof(header.Name));
			lua_setfield(L, -2, "Name");

			LUA_SET_FIELD(L, -1, "_x34", integer, header.unknown);
			LUA_SET_FIELD(L, -1, "Manufacturer_ID", integer,
				header.Manufacturer_ID);
			LUA_SET_FIELD(L, -1, "Cartridge_ID", integer, header.Cartridge_ID);
			LUA_SET_FIELD(L, -1, "Country_code", integer, header.Country_code);
			break;
		}

		case ROM_FIELD_SETTINGS: {
			m64p_rom_settings settings;
			if(CoreDoCommand(M64CMD_ROM_GET_SETTINGS,
			  sizeof(settings), &settings)) {
				lua_pushnil(L);
				break;
			}

			lua_createtable(L, 0, 6); //-1: tbl
			lua_pushlstring(L, settings.goodname, sizeof(settings.goodname));
			lua_setfield(L, -2, "goodname");

			lua_pushlstring(L, settings.MD5, sizeof(settings.MD5));
			lua_setfield(L, -2, "MD5");

			LUA_SET_FIELD(L, -1, "savetype", integer, settings.savetype);
			LUA_SET_FIELD(L, -1, "status",   integer, settings.status);
			LUA_SET_FIELD(L, -1, "players",  integer, settings.players);
			LUA_SET_FIELD(L, -1, "rumble",   boolean, settings.rumble);
			break;
		}

		default:
			lua_pushnil(L);
	}

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
	//table of ROM field names
	lua_createtable(L, 0, NUM_ROM_FIELDS);
	int i; for(i=0; romFieldName[i]; i++) {
		lua_pushinteger(L, i);
		lua_setfield(L, -2, romFieldName[i]);
	}
	lua_setfield(L, LUA_REGISTRYINDEX, "rom_fields");


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
