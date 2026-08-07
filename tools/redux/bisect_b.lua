-- bisect_b.lua — Schnitt 2: Helfer + die Zustands-Ausgabe (zustand) + ein Schreibzugriff.
-- Damit ist eingegrenzt, ob der Fehler in zustand() oder in der Phasenmaschine sitzt.

local OUT = [[C:\workspace\git\reAi_v2\tools\redux\bisect_b_out.txt]]
local frames = 0
local out = nil
local fertig = false

local function u8(m, a) return m[bit.band(a, 0x1fffff)] end
local function u16(m, a)
  local o = bit.band(a, 0x1fffff)
  return m[o] + m[o+1] * 256
end
local function u32(m, a)
  local o = bit.band(a, 0x1fffff)
  return m[o] + m[o+1]*256 + m[o+2]*65536 + m[o+3]*16777216
end
local function s32(m, a)
  local v = u32(m, a)
  if v >= 0x80000000 then v = v - 0x100000000 end
  return v
end
local function w8(m, a, v) m[bit.band(a, 0x1fffff)] = v end
local function w16(m, a, v)
  local o = bit.band(a, 0x1fffff)
  m[o] = bit.band(v, 0xff)
  m[o+1] = bit.band(bit.rshift(v, 8), 0xff)
end
local function w32(m, a, v)
  local o = bit.band(a, 0x1fffff)
  for i = 0, 3 do m[o+i] = bit.band(bit.rshift(v, i*8), 0xff) end
end

local function zustand(m, titel)
  local stage = u16(m, 0x800B0FE6)
  local idx   = u16(m, 0x800B0FE2)
  out:write(string.format("\n== %s (Bild %d) ==\n", titel, frames))
  out:write(string.format("  Stage=%d Index=0x%02x -> Raum 0x%03x\n",
            stage, idx, bit.bor(bit.lshift(stage + 1, 8), idx)))
  out:write(string.format("  Modus 0x800b5359=%d  Datensatz 0x800ac9a8=%08x\n",
            u8(m, 0x800b5359), u32(m, 0x800ac9a8)))
  out:write(string.format("  Spieler=(%d,%d,%d)\n",
            s32(m, 0x800aca88), s32(m, 0x800aca8c), s32(m, 0x800aca90)))
  local n = 0
  local liste = ""
  for i = 0, 31 do
    local e = 0x800acc2c + i * 0x1F4
    local typ = u8(m, e + 8)
    if bit.band(u32(m, e), 0x8000) == 0 and typ ~= 0 and typ < 0x80 then
      n = n + 1
      liste = liste .. string.format(" s%02d:t%02x/g%02x", i, typ, u8(m, e + 9))
    end
  end
  out:write(string.format("  aktive Gegner: %d%s\n", n, liste))
  out:flush()
end

function DrawImguiFrame()
  if fertig then return end
  frames = frames + 1
  if frames < 900 then return end
  fertig = true
  out = io.open(OUT, 'w')
  if out == nil then return end
  out:write("Schnitt B laeuft\n")
  local ok, err = pcall(function()
    local m = PCSX.getMemPtr()
    zustand(m, "Zustand")
    w8(m, 0x800bbe5c, 0)                      -- ein einzelner Schreibzugriff als Test
    out:write("Schreibzugriff ok\n")
  end)
  if not ok then out:write("FEHLER: " .. tostring(err) .. "\n") end
  out:flush()
  out:close()
end
