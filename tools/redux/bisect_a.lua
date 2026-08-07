-- bisect_a.lua — Schnitt 1: alle Helfer aus jump_1030.lua, aber nur eine Markierung im 1. Bild.
-- Erscheint die Datei, sind die Helfer in Ordnung und der Fehler steckt in der Sprung-Logik.

local OUT = [[C:\workspace\git\reAi_v2\tools\redux\bisect_a_out.txt]]
local frames = 0
local out = nil

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

function DrawImguiFrame()
  frames = frames + 1
  if out ~= nil then return end
  out = io.open(OUT, 'w')
  if out == nil then return end
  out:write("Schnitt A: Helfer geladen, Frame-Haken laeuft\n")
  local ok, err = pcall(function()
    local m = PCSX.getMemPtr()
    out:write(string.format("u32-Test 0x80010000 = %08x\n", u32(m, 0x80010000)))
    out:write(string.format("s32-Test Spieler-x = %d\n", s32(m, 0x800aca88)))
    out:write(string.format("u16-Test Stage = %d  Index = %d\n",
              u16(m, 0x800B0FE6), u16(m, 0x800B0FE2)))
    out:write(string.format("u8-Test Modus = %d\n", u8(m, 0x800b5359)))
  end)
  if not ok then out:write("LESE-FEHLER: " .. tostring(err) .. "\n") end
  out:flush()
  out:close()
  out = nil
  frames = -1000000   -- nicht nochmal
end
