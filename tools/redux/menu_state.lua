-- menu_state.lua — oeffnet SELECT das Debug-Menue? Mit dem EINDEUTIGEN Nachweis.
--
-- Alle bisherigen Aussagen dazu beruhten auf Ausfuehrungs-Zaehlern, die durch `return false`
-- nach dem ersten Treffer tot waren (siehe Skill, Falle 1). Deshalb hier KEINE Zaehler als
-- Beleg, sondern die Offen-Kennung, die das Spiel selbst fuehrt:
--
--   FUN_8001443c @0x80014460  lbu v0,0(v1) => DAT_800bbe5c
--                @0x80014480  xori v0,v0,0x1        <- Umschalten
--                @0x80014488  sb   v0,0(v1)
--   => 0x800BBE5C == 1  heisst: Menue offen. Punkt.
--
-- Der Einstieg prueft @0x8001444c `andi v0,v0,0x100` auf die ROHE Flanke 0x800AC762 (low half
-- von 0x800AC75C). SELECT steht NIE in den virtuellen Woertern 0x800AC768/76C — das ist Design
-- (FUN_80030444 baut die aus der Belegungstabelle, SELECT ist keine belegbare Spielfunktion).
--
-- Gemessen wird deshalb:
--   * 0x800BBE5C  jedes Bild            -> Menue offen?
--   * 0x800AC762  ODER ueber alle Bilder -> kommt die ROHE SELECT-Flanke ueberhaupt zustande?
--   * Aufrufzaehler auf 0x8001443c mit `return true` -> laeuft der Menue-Handler ueberhaupt?

local OUT = [[C:\workspace\git\reAi_v2\tools\redux\menu_state_out.txt]]
local frames, out, spielerAb = 0, nil, -1
local naufruf, oder_flanke, n_select = 0, 0, 0
local offenAb, maxzeile = -1, 0

local function u8(m, a) return m[bit.band(a, 0x1fffff)] end
local function u16(m, a)
  local o = bit.band(a, 0x1fffff)
  return m[o] + m[o+1]*256
end
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
    out:write("Debug-Menue: Offen-Kennung 0x800BBE5C als Beleg\n")
    _G.__bps = {}
    pcall(function()
      _G.__bps[1] = PCSX.addBreakpoint(0x8001443c, 'Exec', 4, 'menuein', function()
        naufruf = naufruf + 1
        return true   -- false wuerde den Haltepunkt LOESCHEN
      end)
    end)
    out:flush()
  end

  local ok, err = pcall(function()
    local m = PCSX.getMemPtr()
    local px, pz = s32(m, 0x800aca88), s32(m, 0x800aca90)

    if spielerAb < 0 and (px ~= 0 or pz ~= 0) then
      spielerAb = frames
      out:write(string.format("SPIELER ab Bild %d — ab jetzt SELECT getaktet\n", frames))
      out:flush()
    end

    -- rohe Flanke jedes Bild akkumulieren (sie steht nur EIN Bild an)
    local fl = u16(m, 0x800AC762)
    oder_flanke = bit.bor(oder_flanke, fl)
    if bit.band(fl, 0x100) ~= 0 then n_select = n_select + 1 end

    local offen = u8(m, 0x800BBE5C)
    if offen ~= 0 then
      if offenAb < 0 then
        offenAb = frames
        out:write(string.format("\n*** MENUE OFFEN ab Bild %d (0x800BBE5C=%d) ***\n", frames, offen))
        out:flush()
      end
      local z = u8(m, 0x800BBE5D)
      if z > maxzeile then maxzeile = z end
    end

    if spielerAb > 0 then
      local t = (frames - spielerAb) % 32
      pcall(function()
        local p = PCSX.SIO0.slots[1].pads[1]
        if t < 8 then p.setOverride(PCSX.CONSTS.PAD.BUTTON.SELECT)
        else p.clearOverride(PCSX.CONSTS.PAD.BUTTON.SELECT) end
      end)
    end

    if frames % 1500 == 0 then
      out:write(string.format(
        "B%5d | offen=%d (ab %d) Zeile=%d/%d Stage=%d Idx=%02x | FUN_8001443c n=%d | rohe Flanke oder=%04x SELECT-Flanken=%d | Spieler (%d,%d)\n",
        frames, u8(m, 0x800BBE5C), offenAb, u8(m, 0x800BBE5D), maxzeile,
        u8(m, 0x800BBE5E), u8(m, 0x800BBE5F),
        naufruf, oder_flanke, n_select, px, pz))
      out:flush()
    end

    if frames >= 18000 then
      out:write("\nfertig\n"); out:close(); PCSX.quit(0)
    end
  end)
  if not ok and out ~= nil then out:write("FEHLER: " .. tostring(err) .. "\n"); out:flush() end
end
