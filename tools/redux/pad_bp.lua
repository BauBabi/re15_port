-- pad_bp.lua — Pad-Eingabe per HALTEPUNKT statt Frame-Haken.
--
-- BEFUND aus pad_drive.lua: Schreiben in die Pad-Woerter aus DrawImguiFrame() greift zwar
-- (die Werte stehen drin), kommt aber ZU SPAET — das Spiel schreibt sie jedes Bild neu, bevor es
-- sie benutzt. Nach der Treibphase standen sie wieder auf 0, das Spiel reagierte nie.
--
-- LOESUNG: ein SCHREIB-Haltepunkt auf das Flanken-Wort 0x800AC76C. Es ist das letzte der vier,
-- das FUN_80030444 setzt (@0x8003057c `sw v0,0x800ac76c`, nach dem gehaltenen Wort @0x8003051c
-- und dem remappten @0x80030564). Feuert er, hat das Spiel gerade fertig eingelesen — genau dann
-- ueberschreiben wir alle vier Woerter, und der Verbraucher sieht unsere Eingabe.
-- Der Rueckruf liefert false: der Emulator laeuft weiter, es wird nicht angehalten.
--
-- Pad-Maske 0x8f0 = START|Quadrat|Kreuz|Kreis|Dreieck; der Titel nimmt jeden davon (@0x80102c14).

local OUT  = [[C:\workspace\git\reAi_v2\tools\redux\padbp_out.txt]]
local MASK = 0x8f0
local frames = 0
local out = nil
local fertig = false
local treibe = false
local bp = nil
local treffer = 0

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
    "%5d %-6s Treffer=%d | roh=%04x remap=%04x held=%04x edge=%04x | Spieler=(%d,%d,%d) Gegner=%d Modus=%d\n",
    frames, tag, treffer,
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
    out:write("Bild  Phase  Haltepunkt-Treffer, Pad-Woerter, Spielzustand\n")
    -- Haltepunkt einmalig setzen
    local ok, err = pcall(function()
      bp = PCSX.addBreakpoint(0x800AC76C, 'Write', 4, 'padinject', function()
        if treibe then
          local m = PCSX.getMemPtr()
          treffer = treffer + 1
          w32(m, 0x800AC758, MASK)
          w32(m, 0x800AC760, MASK)
          w32(m, 0x800AC768, MASK)
          w32(m, 0x800AC76C, MASK)
        end
        return false          -- nicht anhalten
      end)
    end)
    out:write(ok and "Haltepunkt gesetzt\n" or ("Haltepunkt FEHLER: " .. tostring(err) .. "\n"))
    out:flush()
  end

  local ok2, err2 = pcall(function()
    local m = PCSX.getMemPtr()
    if frames == 600 then treibe = true; zeile(m, "START") end
    if frames == 1200 then treibe = false; zeile(m, "STOP") end
    if frames % 150 == 0 then zeile(m, treibe and "TREIB" or "beob.") end
    if frames >= 1900 then
      zeile(m, "ENDE")
      out:write("\nfertig\n")
      out:close()
      fertig = true
      PCSX.quit(0)
    end
  end)
  if not ok2 and out ~= nil then
    out:write("FEHLER (Bild " .. tostring(frames) .. "): " .. tostring(err2) .. "\n")
    out:flush()
    fertig = true
  end
end
