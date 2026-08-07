-- padflow.lua — WOHIN kommt SELECT wirklich? Und warum laeuft das Menue nur einen Durchlauf?
--
-- FEHLER DER VORMESSUNG: ich habe die Pad-Woerter nur alle 600 Bilder abgetastet. Die FLANKE
-- 0x800AC76C steht aber genau EIN Bild. "edge=0000" in der Stichprobe war deshalb kein Beweis,
-- sondern ein Abtast-Artefakt. Also: jedes Bild ODER-akkumulieren.
--
-- Gemessen wird:
--   (1) oder_*  = ODER ueber ALLE Bilder je Pad-Wort -> erreicht Bit 0x100 held/edge ueberhaupt?
--   (2) n_*     = in wie vielen Bildern das Wort Bit 0x100 trug
--   (3) je Menue-Adresse: Zahl der Treffer + Bild des ERSTEN und des LETZTEN Treffers
--       -> feuert eine Adresse nur einmal ganz frueh, gehoert sie zum Aufbau, nicht zur Schleife.
--
-- ⚠ LuaJIT (5.1): bit.band/bit.bor. ⚠ _G.__bps haelt die Haltepunkte am Leben.

local OUT = [[C:\workspace\git\reAi_v2\tools\redux\padflow_out.txt]]
local frames, out, spielerAb = 0, nil, -1

local ADRS = { 0x80014444, 0x80014500, 0x800145f4, 0x80014698, 0x80014760,
               0x80014820, 0x80014900, 0x800149c0, 0x80014a44, 0x80014b10, 0x80014cb0 }
local n, erst, letzt = {}, {}, {}

-- Pad-Woerter: roh / remappt / gehalten / Flanke
local PW = { 0x800AC758, 0x800AC760, 0x800AC768, 0x800AC76C }
local PN = { "roh", "remap", "held", "flanke" }
local oder, ntreffer = { 0, 0, 0, 0 }, { 0, 0, 0, 0 }

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
    out:write("Pad-Fluss + Menue-Durchlaeufe (jedes Bild akkumuliert)\n")
    _G.__bps = {}
    for i = 1, #ADRS do
      n[i], erst[i], letzt[i] = 0, -1, -1
      local idx = i
      pcall(function()
        _G.__bps[idx] = PCSX.addBreakpoint(ADRS[idx], 'Exec', 4, 'a' .. idx, function()
          n[idx] = n[idx] + 1
          if erst[idx] < 0 then erst[idx] = frames end
          letzt[idx] = frames
          return true   -- MESSUNG: false LOESCHT den Haltepunkt (1 Treffer, dann tot)
        end)
      end)
    end
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

    -- JEDES Bild akkumulieren, nicht stichprobenartig
    for i = 1, 4 do
      local v = bit.band(u32(m, PW[i]), 0xffff)
      oder[i] = bit.bor(oder[i], v)
      if bit.band(v, 0x100) ~= 0 then ntreffer[i] = ntreffer[i] + 1 end
    end

    if spielerAb > 0 then
      local t = (frames - spielerAb) % 32
      pcall(function()
        local p = PCSX.SIO0.slots[1].pads[1]
        if t < 8 then p.setOverride(PCSX.CONSTS.PAD.BUTTON.SELECT)
        else p.clearOverride(PCSX.CONSTS.PAD.BUTTON.SELECT) end
      end)
    end

    if frames % 3000 == 0 then
      local p = {}
      for i = 1, 4 do p[#p+1] = string.format("%s oder=%04x n0x100=%d", PN[i], oder[i], ntreffer[i]) end
      out:write(string.format("\nB%d  Spieler (%d,%d)\n  %s\n", frames, px, pz, table.concat(p, " | ")))
      for i = 1, #ADRS do
        if n[i] > 0 then
          out:write(string.format("  %08x  n=%-5d erst=%-6d letzt=%-6d\n", ADRS[i], n[i], erst[i], letzt[i]))
        end
      end
      out:flush()
    end

    if frames >= 18000 then
      out:write("\nfertig\n"); out:close(); PCSX.quit(0)
    end
  end)
  if not ok and out ~= nil then out:write("FEHLER: " .. tostring(err) .. "\n"); out:flush() end
end
