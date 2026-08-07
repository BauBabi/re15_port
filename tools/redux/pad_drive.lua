-- pad_drive.lua — Schritt 1 zum autonomen Raumzugang: per RAM-Schreibzugriff durch den Titel.
--
-- Zuerst BEOBACHTEN, dann treiben (lesson.txt: messen, nicht modellieren):
--   Bild  300-599  nur mitschreiben  -> was legt das Spiel selbst in die Pad-Woerter?
--   Bild  600-1100 Bestaetigen-Maske in alle vier Woerter schreiben
--   Bild 1100-1700 wieder nur mitschreiben -> hat es gegriffen?
--
-- Pad-Woerter (aus debug_menu_common.c, alle mit @0x-Beleg):
--   0x800AC758  roh (Quelle fuer den Remap in FUN_80030444, geschrieben @0x80030564)
--   0x800AC760  remappt (das Debug-Menue liest dieses)
--   0x800AC768  gehalten     0x800AC76C  Flanke
-- Titel nimmt jeden Face-Button: Maske 0x8f0 @0x80102c14
-- (0x800 START, 0x80 Quadrat, 0x40 Kreuz, 0x20 Kreis, 0x10 Dreieck).
--
-- ⚠ Aufbau bewusst wie bisect_b.lua (verifiziert): EIN Zaehler, Datei erst im Frame-Aufruf,
-- alles in pcall. Ein Lua-Ladefehler geht hier sonst voellig still verloren.

local OUT  = [[C:\workspace\git\reAi_v2\tools\redux\pad_out.txt]]
local MASK = 0x8f0
local frames = 0
local out = nil
local fertig = false

local function u8(m, a) return m[bit.band(a, 0x1fffff)] end
local function u32(m, a)
  local o = bit.band(a, 0x1fffff)
  return m[o] + m[o+1]*256 + m[o+2]*65536 + m[o+3]*16777216
end
local function s32(m, a)
  local v = u32(m, a)
  if v >= 0x80000000 then v = v - 0x100000000 end
  return v
end
local function w32(m, a, v)
  local o = bit.band(a, 0x1fffff)
  for i = 0, 3 do m[o+i] = bit.band(bit.rshift(v, i*8), 0xff) end
end

local function zeile(m, tag)
  local n = 0
  for i = 0, 31 do
    local e = 0x800acc2c + i * 0x1F4
    local typ = u8(m, e + 8)
    if bit.band(u32(m, e), 0x8000) == 0 and typ ~= 0 and typ < 0x80 then n = n + 1 end
  end
  out:write(string.format(
    "%5d %-6s pad roh=%04x remap=%04x held=%04x edge=%04x | Spieler=(%d,%d,%d) Gegner=%d Modus=%d\n",
    frames, tag,
    bit.band(u32(m, 0x800AC758), 0xffff), bit.band(u32(m, 0x800AC760), 0xffff),
    bit.band(u32(m, 0x800AC768), 0xffff), bit.band(u32(m, 0x800AC76C), 0xffff),
    s32(m, 0x800aca88), s32(m, 0x800aca8c), s32(m, 0x800aca90), n,
    u8(m, 0x800b5359)))
  out:flush()
end

function DrawImguiFrame()
  if fertig then return end
  frames = frames + 1
  if frames < 300 then return end

  if out == nil then
    out = io.open(OUT, 'w')
    if out == nil then return end
    out:write("Bild  Phase  Pad-Woerter und Spielzustand\n")
  end

  local ok, err = pcall(function()
    local m = PCSX.getMemPtr()

    if frames >= 600 and frames < 1100 then
      -- treiben: Bestaetigen-Maske in alle vier Woerter
      w32(m, 0x800AC758, MASK)
      w32(m, 0x800AC760, MASK)
      w32(m, 0x800AC768, MASK)
      w32(m, 0x800AC76C, MASK)
      if frames % 100 == 0 then zeile(m, "TREIB") end
    elseif frames % 100 == 0 then
      zeile(m, (frames < 600) and "beob." or "nach")
    end

    if frames >= 1700 then
      zeile(m, "ENDE")
      out:write("\nfertig\n")
      out:close()
      fertig = true
      PCSX.quit(0)
    end
  end)

  if not ok and out ~= nil then
    out:write("FEHLER (Bild " .. tostring(frames) .. "): " .. tostring(err) .. "\n")
    out:flush()
    fertig = true
  end
end
