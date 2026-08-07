-- jump_1030.lua — Raumsprung nach ROOM1030 ohne Navigation, per RAM-Schreibzugriff.
--
-- MECHANISMUS (selbst disassembliert): Der Debug-JUMP des Originals baut KEINEN Tuer-Datensatz.
--   @0x80014a44/48  sb 1    -> 0x800b5359   Modus 1 = Raumwechsel
--   @0x80014a4c/50  sw zero -> 0x800ac9a8   Datensatz-Zeiger = NULL
--   @0x80014a54/58  sb zero -> 0x800bbe5c
-- Das Ziel steht in den Raum-Globals, die der JUMP-Ausfuehrer direkt schreibt:
--   @0x8001D644  sh -> 0x800B0FE6 = Stage      @0x8001D660  sh -> 0x800B0FE2 = Index
-- Raumnummer = (stage+1)<<8 | idx  ->  ROOM1030 = Stage 0, Index 0x03.
-- (Formel-Beleg: BRIEFING ROOM = Index 0x14 -> 0x114 -> Port-ROOM1140.)
--
-- ⚠ AUFBAU NICHT UMSTELLEN. Eine frühere Fassung mit Phasen-Variable lud kommentarlos nicht
-- (DrawImguiFrame wurde nie definiert, es entstand keinerlei Ausgabe — ein Lua-Ladefehler geht
-- hier still verloren). Diese Fassung ist aus der per Bisektion verifizierten bisect_b.lua
-- aufgebaut: EIN Wahrheitswert als Ablaufsteuerung, Datei erst im Frame-Aufruf oeffnen,
-- alles Weitere in pcall.

local OUT = [[C:\workspace\git\reAi_v2\tools\redux\jump_out.txt]]
local frames = 0
local out = nil
local gesprungen = false
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

  if not gesprungen then
    if frames < 900 then return end
    gesprungen = true
    out = io.open(OUT, 'w')
    if out == nil then return end
    out:write("ROOM1030-Sprung\n")
    local ok, err = pcall(function()
      local m = PCSX.getMemPtr()
      zustand(m, "VOR dem Sprung")
      w16(m, 0x800B0FE6, 0)       -- Stage 0
      w16(m, 0x800B0FE2, 0x03)    -- Index 3 -> Raum 0x103 -> ROOM1030
      w32(m, 0x800ac9a8, 0)       -- Datensatz-Zeiger NULL
      w8(m, 0x800bbe5c, 0)
      w8(m, 0x800b5359, 1)        -- Modus 1 = Raumwechsel
      out:write("\n  -> Ziel gesetzt, Modus 1 ausgeloest\n")
    end)
    if not ok then out:write("FEHLER beim Sprung: " .. tostring(err) .. "\n") end
    out:flush()
    return
  end

  if frames < 1600 then return end
  fertig = true
  local ok2, err2 = pcall(function()
    zustand(PCSX.getMemPtr(), "NACH dem Sprung")
  end)
  if not ok2 then out:write("FEHLER danach: " .. tostring(err2) .. "\n") end
  out:write("\nfertig\n")
  out:flush()
  out:close()
end
