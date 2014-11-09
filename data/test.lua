--quick test/example script
print("This is test.lua running in", _VERSION)
for k, v in pairs(m64p) do print("m64p", k, v) end


local function hexdump(str, base, fmt)
	base = base or 0
	fmt  = fmt  or '%04X'
	for y = 0, #str-1, 16 do
		local buf = {fmt:format(base+y)}

		--print hex
		for x = 0, 15 do
			local a = x + y + 1
			buf[#buf+1] = ((x % 4) == 0) and "|" or " "

			local b = str:byte(a)
			if b then buf[#buf+1] = ('%02X'):format(b)
			else buf[#buf+1] = '..'
			end
		end

		--print ASCII
		buf[#buf+1] = '|'
		for x = 0, 15 do
			local a = x + y + 1
			local b = str:byte(a)
			if b and (b >= 0x20 and b <= 0x7E) then buf[#buf+1] = str:sub(a, a)
			else buf[#buf+1] = '.'
			end
		end

		print(table.concat(buf)) --output the buffer
	end
end


--example function, use the VI callback to implement simple cheats
--you could do more advanced memory manipulation here beyond what's possible
--with a Gameshark code (including e.g. reading from external files or sockets,
--displaying a GUI using lgi, etc)
local vi_count = 0
local function vi_callback()
	if vi_count == 4 then
		print("state:", m64p.state)
		--m64p.state = 'paused'
		m64p.audioVolume = 0.2;
	end

	if vi_count == 0 then
		if m64p.rom.settings.goodname ~= "Mario Kart 64 (U) [!]" then
			m64p:unregisterCallback('vi', vi_callback)
		end

	elseif vi_count >= 4 then
		--hexdump(m64p.memory:read(0x80000000, 256))
		--print("speed:", m64p.memory:read(0x800F6BA4, 'float'))

		--writing too early breaks the game (even though as a Gameshark code
		--on real N64 there's no problem?)
		if m64p.memory:read(0x800F6BA4, 'u8') ~= 0 then
			--change player 1 max speed to ~110km/h (500 units/sec up from ~294)
			--you can always go higher, but the game starts to get unplayable
			--due to your kart lifting off and taking flight on every hill.
			m64p.memory:write(0x800F6BA4, 'float', 500)
		end
	end

	vi_count = vi_count + 1
end

m64p:registerCallback('vi', vi_callback)
