--quick test/example script
print("This is test.lua running in", _VERSION)
for k, v in pairs(m64p) do print("m64p", k, v) end

local vi_count = 0
local function vi_callback()
	if vi_count == 0 then
		if m64p.rom.settings.goodname ~= "Mario Kart 64 (U) [!]" then
			m64p.unregisterCallback('vi', vi_callback)
		end

	elseif vi_count >= 4 then
		--writing too early breaks the game (even though as a Gameshark code
		--on real N64 there's no problem?)
		if m64p.memory:readu8(0x800F6BA4) ~= 0 then
			--change player 1 max speed (second byte of a float)
			m64p.memory[0x800F6BA5] = 0xFF
		end
	end

	vi_count = vi_count + 1
end

m64p.registerCallback('vi', vi_callback)
