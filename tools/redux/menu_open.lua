-- menu_open.lua — Debug-Menue oeffnen, mit HARTEM Nachweis.
--
-- NUTZER: "ab dem Zeitpunkt nach der Spielerauswahl kannst du JEDERZEIT SELECT druecken."
-- Mein Zeitpunkt war also richtig — die Taste kam nicht an. Wahrscheinlich ist der
-- vgamepad-Knopf BACK in PCSX-Redux gar nicht auf SELECT gemappt.
--
-- LOESUNG: die deterministische Lua-Pad-API aus dem Skill statt des physischen Pads:
--   PCSX.SIO0.slots[1].pads[1].setOverride(PCSX.CONSTS.PAD.BUTTON.SELECT)  -- SELECT = 0
--   ... clearOverride(...) zum Loslassen
--
-- ABNAHMEKRITERIUM (aus dem Skill): der Exec-Haltepunkt auf der Menuefunktion 0x80014444 muss
-- DREISTELLIG zaehlen. Ein einzelner Treffer ist KEIN offenes Menue — genau daran habe ich mich
-- schon einmal selbst getaeuscht.

local OUT = [[C:\workspace\git\reAi_v2\tools\redux\menu_out.txt]]
local frames, out = 0, nil
local menuehits, spielerAb = 0, -1
local druecke, phase, zyklus = false, 0, 0

local function u8(m, a) return m[bit.band(a, 0x1fffff)] end
local function u32(m, a)
  local o = bit.band(a, 0x1fffff)
  return m[o] + m[o+1]*256 + m[o+2]*65536 + m[o+3]*16777216
end
local function s32(m, a)
  local v = u32(m, a); if v >= 0x80000000 then v = v - 0x100000000 end; return v
end

local function pad(gedrueckt)
  local ok = pcall(function()
    local p = PCSX.SIO0.slots[1].pads[1]
    if gedrueckt then p.setOverride(PCSX.CONSTS.PAD.BUTTON.SELECT)
    else p.clearOverride(PCSX.CONSTS.PAD.BUTTON.SELECT) end
  end)
  return ok
end

function DrawImguiFrame()
  frames = frames + 1
  if out == nil then
    out = io.open(OUT, 'w')
    if out == nil then return end
    out:write("Debug-Menue oeffnen (Lua-Pad-Override)\n")
    local ok = pcall(function()
      _G.__menu = PCSX.addBreakpoint(0x80014444, 'Exec', 4, 'dbgmenu', function()
        menuehits = menuehits + 1
        return true   -- MESSUNG: false LOESCHT den Haltepunkt (1 Treffer, dann tot)
      end)
    end)
    out:write("Menue-Haltepunkt: " .. tostring(ok) .. "\n")
    -- Pad-API einmal testen und das Ergebnis protokollieren
    out:write("Pad-API erreichbar: " .. tostring(pad(false)) .. "\n")
    out:flush()
  end

  local ok, err = pcall(function()
    local m = PCSX.getMemPtr()
    local px, pz = s32(m, 0x800aca88), s32(m, 0x800aca90)

    if spielerAb < 0 and (px ~= 0 or pz ~= 0) then
      spielerAb = frames
      out:write(string.format("\nSPIELER ab Bild %d: (%d,%d) — ab jetzt SELECT\n", frames, px, pz))
      out:flush()
    end

    -- ab Spieler-Existenz: SELECT im Takt druecken/loslassen (20 Bilder an, 40 aus)
    -- JEDES BILD setzen, nicht nur beim Wechsel: falls der Emulator die Uebersteuerung pro Bild
    -- zuruecknimmt, dauerte ein "Druck" sonst genau ein Bild. Das war der vorige Fehlversuch
    -- (182 Zyklen, kein einziger zusaetzlicher Menue-Aufruf).
    if spielerAb > 0 and menuehits < 100 then
      local t = (frames - spielerAb) % 60
      local soll = (t < 20)
      pad(soll)
      if soll and not druecke then zyklus = zyklus + 1 end
      druecke = soll
    elseif druecke then
      druecke = false; pad(false)
    end

    if frames % 300 == 0 then
      -- Pad-Woerter mitlesen: kommt die Uebersteuerung ueberhaupt im Spiel an?
      -- SELECT ist Bit 0x100 im RAW-Wort (libetc-Layout).
      out:write(string.format(
        "B%5d | Menue %5d | Zyklen %3d | druecke=%s | roh=%04x remap=%04x held=%04x edge=%04x | Spieler (%d,%d)\n",
        frames, menuehits, zyklus, tostring(druecke),
        bit.band(u32(m, 0x800AC758), 0xffff), bit.band(u32(m, 0x800AC760), 0xffff),
        bit.band(u32(m, 0x800AC768), 0xffff), bit.band(u32(m, 0x800AC76C), 0xffff),
        px, pz))
      out:flush()
      if menuehits >= 100 then
        out:write(string.format("\nMENUE IST OFFEN — %d Aufrufe (Kriterium: dreistellig)\n", menuehits))
        out:write("fertig\n"); out:close(); PCSX.quit(0)
      end
    end
    if frames >= 20000 then
      out:write(string.format("\nnach %d Bildern nur %d Menue-Aufrufe — nicht offen\n", frames, menuehits))
      out:write("fertig\n"); out:close(); PCSX.quit(0)
    end
  end)
  if not ok and out ~= nil then out:write("FEHLER: " .. tostring(err) .. "\n"); out:flush() end
end
