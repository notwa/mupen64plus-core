--quick test/example script
print("This is test.lua running in", _VERSION)
for k, v in pairs(m64p) do print("m64p", k, v) end


local function hexdump(str, base, fmt)
	base = base or 0
	fmt  = fmt  or '%04X'
	for y = 0, #str-1, 16 do
		local buf = {fmt:format(base+y)}
		for x = 0, 15 do
			local a = x + y + 1
			buf[#buf+1] = ((x % 4) == 0) and "|" or " "

			local b = str:byte(a)
			if b then buf[#buf+1] = ('%02X'):format(b)
			else buf[#buf+1] = '..'
			end
		end

		buf[#buf+1] = '|'
		for x = 0, 15 do
			local a = x + y + 1
			local b = str:byte(a)
			if b and (b >= 0x20 and b <= 0x7E) then buf[#buf+1] = str:sub(a, a)
			else buf[#buf+1] = '.'
			end
		end

		print(table.concat(buf))
	end
end


local vi_count = 0
local function vi_callback()
	if vi_count == 0 then
		if m64p.rom.settings.goodname ~= "Mario Kart 64 (U) [!]" then
			m64p.unregisterCallback('vi', vi_callback)
		end

	elseif vi_count >= 4 then
		hexdump(m64p.memory:read(0x80000000, 256))

		--writing too early breaks the game (even though as a Gameshark code
		--on real N64 there's no problem?)
		if m64p.memory:read(0x800F6BA4, 'u8') ~= 0 then
			--change player 1 max speed (second byte of a float)
			m64p.memory[0x800F6BA5] = 0xFF
		end
	end

	vi_count = vi_count + 1
end

m64p.registerCallback('vi', vi_callback)
