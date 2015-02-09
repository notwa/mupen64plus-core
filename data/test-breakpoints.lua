local function printf(...) io.write(string.format(...)) end

local vi_count = 0
local function vi_callback()
	vi_count = vi_count + 1
	if vi_count < 5 then return end

	if vi_count == 5 then
		print("m64p.debug=", m64p.debug)
		for k, v in pairs(m64p.debug) do print("", k, v) end
		print("state=", m64p.debug.runState)
		printf("prevPC = 0x%08X\n", m64p.debug.prevPC or -1)
	end
end

m64p:registerCallback('vi', vi_callback)
