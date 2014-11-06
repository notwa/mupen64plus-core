--quick test/example script
print("This is test.lua running in", _VERSION)
for k, v in pairs(m64p) do print("m64p", k, v) end

local function vi_callback()
	print("VI callback.")
	print("ROM name is:", m64p.rom.header.Name)
	m64p.unregisterCallback('vi', vi_callback)
end

m64p.registerCallback('vi', vi_callback)
