-- crawl_1030.lua — Debug-Menue NACHWEISLICH erkennen, nach ROOM1030 springen, Kriechen messen.
--
-- Das Menue oeffnet sich mit SELECT waehrend des INTROS, also NACH der Spielerauswahl
-- (Nutzer-Hinweis). Zu frueh gedrueckt stoert es die Auswahl — deshalb wartet der Treiber,
-- bis der SPIELER EXISTIERT, und drueckt erst dann.
--
-- NACHWEIS statt Vermutung: ein AUSFUEHRUNGS-Haltepunkt auf die Menuefunktion @0x80014444.
-- Feuert er, ist das Menue offen — vorher schreiben wir NICHTS in die Menue-Bytes.
--
-- MENUE-ZUSTAND (debug_menu_common.c):
--   0x800BBE5D Auswahlzeile (1 = JUMP), 0x800BBE5E Stage, 0x800BBE5F+stage Raumindex
--   Bestaetigen = Bit 0x80 im Halbwort 0x800AC762 (`lhu` @0x80014a30, `andi 0x80` @0x80014a38)
-- Raumnummer = (stage+1)<<8 | idx  ->  ROOM1030 = Stage 0, Index 0x03.
--
-- ⚠ LuaJIT (5.1): bit.band/bit.bor/bit.rshift. ⚠ _G.__keep haelt Haltepunkte am Leben.
-- ⚠ Pad aus DrawImguiFrame zu schreiben kommt zu spaet -> Schreib-Haltepunkt auf 0x800AC76C.

local OUT = [[C:\workspace\git\reAi_v2\tools\redux\crawl_out.txt]]
local frames, out, phase, wartet = 0, nil, 0, 0
local menuehits, spielerAb, menueAb = 0, -1, -1
local bestaetige = false

local function u8(m, a) return m[bit.band(a, 0x1fffff)] end
local function u32(m, a)
  local o = bit.band(a, 0x1fffff)
  return m[o] + m[o+1]*256 + m[o+2]*65536 + m[o+3]*16777216
end
local function s32(m, a)
  local v = u32(m, a); if v >= 0x80000000 then v = v - 0x100000000 end; return v
end
local function w8(m, a, v) m[bit.band(a, 0x1fffff)] = v end
local function w16(m, a, v)
  local o = bit.band(a, 0x1fffff)
  m[o] = bit.band(v, 0xff); m[o+1] = bit.band(bit.rshift(v, 8), 0xff)
end

local function gegner(m)
  local liste = {}
  for i = 0, 31 do
    local e = 0x800acc2c + i * 0x1F4
    local typ = u8(m, e + 8)
    if bit.band(u32(m, e), 0x8000) == 0 and typ ~= 0 and typ < 0x80 then
      liste[#liste+1] = { slot = i, e = e, typ = typ }
    end
  end
  return liste
end

function DrawImguiFrame()
  if phase == 9 then return end
  frames = frames + 1

  if out == nil then
    out = io.open(OUT, 'w')
    if out == nil then return end
    out:write("ROOM1030 ueber das Debug-Menue (mit Nachweis)\n")
    local ok1 = pcall(function()
      _G.__menu = PCSX.addBreakpoint(0x80014444, 'Exec', 4, 'dbgmenu', function()
        menuehits = menuehits + 1
        return false
      end)
    end)
    local ok2 = pcall(function()
      _G.__pad = PCSX.addBreakpoint(0x800AC76C, 'Write', 4, 'confirm', function()
        if bestaetige then w16(PCSX.getMemPtr(), 0x800AC762, 0x80) end
        return false
      end)
    end)
    out:write(string.format("Haltepunkte: Menue=%s Pad=%s\n", tostring(ok1), tostring(ok2)))
    out:flush()
  end

  local ok, err = pcall(function()
    local m = PCSX.getMemPtr()
    local px, pz = s32(m, 0x800aca88), s32(m, 0x800aca90)

    if spielerAb < 0 and (px ~= 0 or pz ~= 0) then
      spielerAb = frames
      out:write(string.format("\nSPIELER existiert ab Bild %d: (%d,%d) — jetzt darf SELECT kommen\n",
                frames, px, pz)); out:flush()
    end
    if menueAb < 0 and menuehits > 0 then
      menueAb = frames
      out:write(string.format("MENUE offen ab Bild %d (%d Aufrufe)\n", frames, menuehits)); out:flush()
    end

    -- Solange das Menue laeuft: Ziel eintragen und bestaetigen
    if menuehits > 0 and phase == 0 then
      w8(m, 0x800BBE5D, 1)        -- Zeile = JUMP
      w8(m, 0x800BBE5E, 0)        -- Stage 0
      w8(m, 0x800BBE5F, 0x03)     -- Index 3 -> ROOM1030
      wartet = wartet + 1
      if wartet > 120 then
        bestaetige = true
        if wartet > 400 then phase, wartet = 1, 0 end
      end
    end

    if frames % 600 == 0 then
      out:write(string.format("Bild %5d | Menue-Aufrufe %5d | Spieler=(%d,%d) | Modus %d | Gegner %d\n",
        frames, menuehits, px, pz, u8(m, 0x800b5359), #gegner(m)))
      out:flush()
    end

    if phase == 1 then
      wartet = wartet + 1
      if wartet % 300 ~= 0 then return end
      bestaetige = false
      local g = gegner(m)
      out:write(string.format("\n== Bild %d  Spieler=(%d,%d) Modus=%d Gegner=%d ==\n",
        frames, px, pz, u8(m, 0x800b5359), #g))
      for i = 1, math.min(#g, 10) do
        local e = g[i].e
        out:write(string.format("   Slot %2d Typ %02x grid %02x  %d/%d/%d  Clip %02x Bild %02x  (%d,%d)\n",
          g[i].slot, g[i].typ, u8(m, e + 9), u8(m, e + 4), u8(m, e + 5), u8(m, e + 6),
          u8(m, e + 0x94), u8(m, e + 0x95), s32(m, e + 0x34), s32(m, e + 0x3c)))
      end
      out:flush()
      if wartet >= 6000 then
        out:write("\nfertig\n"); out:close(); phase = 9; PCSX.quit(0)
      end
    end
  end)
  if not ok and out ~= nil then
    out:write("FEHLER: " .. tostring(err) .. "\n"); out:flush(); phase = 9
  end
end
