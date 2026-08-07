-- wait_title.lua — lange genug laufen lassen, bis das Spiel interaktiv wird.
--
-- KORREKTUR MEINER FEHLDIAGNOSE: alive.lua zeigt 102 verschiedene pc-Werte -> das Spiel LAEUFT.
-- Meine "Haenge-Schleife" war ein Fehlschluss aus zwei Stichproben. Die Brennpunkte
-- (0x8006217c, 0x80062130, 0x80059dfc, 0x8006df90 ...) liegen alle im Bibliotheks-/CD-Bereich,
-- es gibt keinen Spieler, und das Pad wird kaum gelesen: das ist eine laufende FMV-Wiedergabe.
--
-- MESSGROESSE fuer "interaktiv": wie oft pro Abschnitt das Pad-Flankenwort 0x800AC76C
-- geschrieben wird. Im Titelmenue laeuft FUN_80030444 jedes Bild -> die Zahl steigt schnell.
-- Sobald das passiert, protokollieren wir und halten an.

local OUT = [[C:\workspace\git\reAi_v2\tools\redux\title_out.txt]]
local frames = 0
local out = nil
local fertig = false
local padwrites = 0
local letzte = 0

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

function DrawImguiFrame()
  if fertig then return end
  frames = frames + 1
  if frames < 120 then return end

  if out == nil then
    out = io.open(OUT, 'w')
    if out == nil then return end
    out:write("Warten auf einen interaktiven Bildschirm\n")
    out:write("Bild   Pad-Schreibvorgaenge (gesamt / im Abschnitt) | pc | Spieler | Modus\n")
    pcall(function()
      PCSX.addBreakpoint(0x800AC76C, 'Write', 4, 'padcount', function()
        padwrites = padwrites + 1
        return false
      end)
    end)
  end

  if frames % 500 ~= 0 then return end

  local ok, err = pcall(function()
    local m = PCSX.getMemPtr()
    local imAbschnitt = padwrites - letzte
    letzte = padwrites
    out:write(string.format("%6d  %6d / %5d | pc=%08x | Spieler=(%d,%d,%d) | Modus=%d\n",
      frames, padwrites, imAbschnitt, tonumber(PCSX.getRegisters().pc),
      s32(m, 0x800aca88), s32(m, 0x800aca8c), s32(m, 0x800aca90),
      u8(m, 0x800b5359)))
    out:flush()

    -- interaktiv: das Pad wird regelmaessig gelesen
    if imAbschnitt > 200 then
      out:write(string.format(
        "\nINTERAKTIV ab Bild %d — das Pad wird jetzt regelmaessig eingelesen (%d mal in 500 Bildern).\n",
        frames, imAbschnitt))
      out:close()
      fertig = true
      PCSX.quit(0)
    elseif frames >= 20000 then
      out:write("\nnach 20000 Bildern noch nicht interaktiv\n")
      out:close()
      fertig = true
      PCSX.quit(0)
    end
  end)
  if not ok and out ~= nil then
    out:write("FEHLER: " .. tostring(err) .. "\n"); out:flush(); fertig = true
  end
end
