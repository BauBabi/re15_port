-- menu_probe.lua — beide offenen Hypothesen in EINEM Lauf beantworten.
--
-- H1: Kommt die Pad-Uebersteuerung ueberhaupt im Spiel an?
--     -> die vier Pad-Woerter mitlesen. SELECT ist Bit 0x100 im RAW-Wort 0x800AC758.
-- H2: Ist 0x80014444 ueberhaupt die Menue-SCHLEIFE?
--     -> Exec-Zaehler ueber den ganzen Menue-Bereich streuen und sehen, welche Adresse
--        PRO BILD zaehlt. Auf 0x80014444 zeigt weder jal noch Zeiger (selbst gescannt),
--        sie wird aus einem Overlay erreicht — moeglicherweise nur einmal zum Aufbau.
--
-- Aufbau bewusst simpel (ein Zaehler, io im Frame-Haken, alles in pcall): ein Lua-Ladefehler
-- geht hier voellig still verloren, komplexe Skripte haben mich das mehrfach gekostet.

local OUT = [[C:\workspace\git\reAi_v2\tools\redux\probe_out.txt]]
local frames, out, spielerAb = 0, nil, -1

local ADRS = { 0x80014444, 0x80014500, 0x800145f4, 0x80014698, 0x80014760,
               0x80014820, 0x80014900, 0x800149c0, 0x80014a44, 0x80014b10, 0x80014cb0 }
local n = {}

local function u8(m, a) return m[bit.band(a, 0x1fffff)] end
local function u32(m, a)
  local o = bit.band(a, 0x1fffff)
  return m[o] + m[o+1]*256 + m[o+2]*65536 + m[o+3]*16777216
end
local function s32(m, a)
  local v = u32(m, a); if v >= 0x80000000 then v = v - 0x100000000 end; return v
end

function DrawImguiFrame()
  frames = frames + 1

  if out == nil then
    out = io.open(OUT, 'w')
    if out == nil then return end
    out:write("Menue-Sonde: Pad-Ankunft + welche Adresse pro Bild laeuft\n")
    _G.__bps = {}
    for i = 1, #ADRS do
      n[i] = 0
      pcall(function()
        _G.__bps[i] = PCSX.addBreakpoint(ADRS[i], 'Exec', 4, 'a' .. i, function()
          n[i] = n[i] + 1
          return false
        end)
      end)
    end
    out:write(string.format("%d Haltepunkte gesetzt\n", #ADRS))
    out:flush()
  end

  local ok, err = pcall(function()
    local m = PCSX.getMemPtr()
    local px, pz = s32(m, 0x800aca88), s32(m, 0x800aca90)

    if spielerAb < 0 and (px ~= 0 or pz ~= 0) then
      spielerAb = frames
      out:write(string.format("SPIELER ab Bild %d — ab jetzt SELECT dauerhaft\n", frames))
      out:flush()
    end

    -- GEMESSEN: dauerhaft gehalten steht SELECT zwar im Roh- und remappten Wort (roh=0100),
    -- aber die FLANKE 0x800AC76C bleibt 0 — und Menues reagieren auf die Flanke, nicht auf den
    -- Dauerdruck. Also takten: 8 Bilder druecken, 24 loslassen, und JEDES Bild setzen bzw.
    -- loeschen (nur beim Zustandswechsel gesetzt wirkte gar nicht).
    if spielerAb > 0 then
      local t = (frames - spielerAb) % 32
      pcall(function()
        local p = PCSX.SIO0.slots[1].pads[1]
        if t < 8 then p.setOverride(PCSX.CONSTS.PAD.BUTTON.SELECT)
        else p.clearOverride(PCSX.CONSTS.PAD.BUTTON.SELECT) end
      end)
    end

    if frames % 600 == 0 then
      local z = ""
      for i = 1, #ADRS do
        if n[i] > 0 then z = z .. string.format(" %08x:%d", ADRS[i], n[i]) end
      end
      if z == "" then z = " (keine)" end
      out:write(string.format("B%5d | roh=%04x remap=%04x held=%04x edge=%04x | Spieler (%d,%d) |%s\n",
        frames,
        bit.band(u32(m, 0x800AC758), 0xffff), bit.band(u32(m, 0x800AC760), 0xffff),
        bit.band(u32(m, 0x800AC768), 0xffff), bit.band(u32(m, 0x800AC76C), 0xffff),
        px, pz, z))
      out:flush()
    end

    if frames >= 18000 then
      out:write("\nfertig\n")
      out:close()
      PCSX.quit(0)
    end
  end)
  if not ok and out ~= nil then
    out:write("FEHLER: " .. tostring(err) .. "\n")
    out:flush()
  end
end
