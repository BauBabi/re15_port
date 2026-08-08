-- menu_close.lua — WER setzt die Offen-Kennung des Debug-Menues zurueck?
--
-- Belegt: 0x800BBE5C == 1 heisst „Menue offen" (FUN_8001443c: @0x80014460 lbu v0,0(v1) /
-- @0x80014480 xori v0,v0,0x1 / @0x80014488 sb v0,0(v1) — die Kennung ist ein TOGGLE).
-- Gemessen: das Menue oeffnet bei Bild ~9560, ist aber ~900 Bilder spaeter wieder zu, obwohl
-- ab dem Oeffnen KEIN SELECT mehr gedrueckt wird. Wer sie zurueckschreibt, ist unbekannt.
--
-- Das ist exakt der Zweck dieses Werkzeugs: ein SCHREIB-Haltepunkt auf die Adresse liefert den
-- literalen PC — auch fuer indirekte Stores ueber ein Basisregister, die ein statischer Scan
-- verfehlt (hier sehr wahrscheinlich, denn FUN_8001443c laedt die Adresse selbst erst per
-- `lui v1,0x800c` / `addiu v1,v1,-0x41a4` in ein Register).
--
-- ⚠ Im Rueckruf `return true` — `false` LOESCHT den Haltepunkt (gemessen: 1 vs. 2075 Treffer).
-- ⚠ PCSX.getRegisters().pc ist CDATA -> tonumber().

local OUT = [[C:\workspace\git\reAi_v2\tools\redux\menu_close_out.txt]]
local ADR = 0x800BBE5C

local frames, out, spielerAb = 0, nil, -1
local offen_seit, drueckt = -1, true
local hist, ereignis = {}, {}
local ntreffer = 0

local function u8(m, a) return m[bit.band(a, 0x1fffff)] end
local function u32(m, a)
  local o = bit.band(a, 0x1fffff)
  return m[o] + m[o+1]*256 + m[o+2]*65536 + m[o+3]*16777216
end
local function s32(m, a)
  local v = u32(m, a); if v >= 0x80000000 then v = v - 0x100000000 end; return v
end

local function taste(knopf, an)
  pcall(function()
    local p = PCSX.SIO0.slots[1].pads[1]
    if an then p.setOverride(knopf) else p.clearOverride(knopf) end
  end)
end

function DrawImguiFrame()
  frames = frames + 1

  if out == nil then
    out = io.open(OUT, 'w')
    if out == nil then return end
    out:write(string.format("Schreib-Haltepunkt auf 0x%08x (Offen-Kennung Debug-Menue)\n", ADR))
    _G.__bps = {}
    local ok, err = pcall(function()
      _G.__bps[1] = PCSX.addBreakpoint(ADR, 'Write', 1, 'menuflag', function()
        ntreffer = ntreffer + 1
        local pc, vor = 0, -1
        pcall(function()
          pc  = tonumber(PCSX.getRegisters().pc)
          vor = PCSX.getMemPtr()[bit.band(ADR, 0x1fffff)]
        end)
        hist[pc] = (hist[pc] or 0) + 1
        if #ereignis < 40 then
          ereignis[#ereignis+1] = string.format("   Bild %6d  PC=0x%08x  Wert vorher=%d", frames, pc, vor)
        end
        return true
      end)
    end)
    out:write(ok and "Haltepunkt gesetzt\n" or ("FEHLER: " .. tostring(err) .. "\n"))
    out:flush()
  end

  local ok, err = pcall(function()
    local m = PCSX.getMemPtr()
    local px, pz = s32(m, 0x800aca88), s32(m, 0x800aca90)
    local offen = u8(m, ADR)

    if spielerAb < 0 and (px ~= 0 or pz ~= 0) then
      spielerAb = frames
      out:write(string.format("SPIELER ab Bild %d\n", frames)); out:flush()
    end

    -- SELECT nur takten, bis die Kennung STEHT. Danach nie wieder — jede weitere Flanke
    -- wuerde sie selbst zuruecksetzen (Toggle) und die Messung wertlos machen.
    if drueckt then
      if offen ~= 0 then
        drueckt = false
        taste(PCSX.CONSTS.PAD.BUTTON.SELECT, false)
        offen_seit = frames
        out:write(string.format("\n*** OFFEN ab Bild %d — ab hier KEIN SELECT mehr ***\n", frames))
        out:flush()
      elseif spielerAb > 0 then
        local t = (frames - spielerAb) % 32
        taste(PCSX.CONSTS.PAD.BUTTON.SELECT, t < 8)
      end
    elseif offen == 0 and offen_seit > 0 then
      out:write(string.format("*** WIEDER ZU bei Bild %d (%d Bilder offen) ***\n",
                frames, frames - offen_seit))
      out:flush()
      offen_seit = -1
    end

    if frames % 3000 == 0 then
      out:write(string.format("\nB%5d offen=%d Treffer=%d  Spieler (%d,%d)\n",
                frames, offen, ntreffer, px, pz))
      local l = {}
      for pc, n in pairs(hist) do l[#l+1] = { pc = pc, n = n } end
      table.sort(l, function(a, b) return a.n > b.n end)
      for i = 1, math.min(#l, 12) do
        out:write(string.format("   PC 0x%08x  %d Schreibvorgaenge\n", l[i].pc, l[i].n))
      end
      out:flush()
    end

    if frames >= 18000 then
      out:write("\n=== die ersten Schreibvorgaenge im Detail ===\n")
      for i = 1, #ereignis do out:write(ereignis[i] .. "\n") end
      out:write("\nfertig\n"); out:close(); PCSX.quit(0)
    end
  end)
  if not ok and out ~= nil then out:write("FEHLER: " .. tostring(err) .. "\n"); out:flush() end
end
