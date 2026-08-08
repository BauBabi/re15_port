-- jump_menu.lua — ueber das Debug-Menue nach ROOM1030 springen und das Kriechen messen.
--
-- Alles hier belegt, nichts geraten (FUN_8001443c, RE_15_Quellcode_V2/FUN_8001443c.c):
--   @0x80014440 lhu DAT_800ac762 / @0x8001444c andi 0x100 / @0x80014450 beq  -> SELECT (ROHE Flanke)
--   @0x80014460 lbu DAT_800bbe5c / @0x80014480 xori 1 / @0x80014488 sb       -> Offen-Kennung, TOGGLE
--   Zeile 29/32: HOCH 0x1000 / RUNTER 0x4000 auf DAT_800bbe5d (max 2)        -> Auswahlzeile
--   Zeile 15:    (&DAT_800bbe5f)[DAT_800b0fe0] = DAT_800b0fe2                -> Index = aktueller Raum
-- Raumnummer = (stage+1)<<8 | idx  ->  ROOM1030 = Stage 0, Index 0x03.
--
-- ⛔ BESTAETIGT WIRD MIT QUADRAT (0x80), NICHT MIT KREUZ. Ich hatte Zeile 36
--   `if ((DAT_800ac762 & 0x40) == 0)` als „Kreuz bestaetigt" gelesen — das ist aber nur das Gatter
--   fuer die Navigation. Die Ausfuehrung steht in Zeile 82 und disassembliert so:
--     80014a30  lhu  v0,-0x389e(v0) => DAT_800ac762     ; rohe Flanke
--     80014a38  andi v0,v0,0x80                         ; QUADRAT
--     80014a3c  beq  v0,zero,LAB_80014ab4
--     80014a48  sb   v0, DAT_800b5359                   ; Modus 1 = Raumsprung
--     80014a50  sw   zero, DAT_800ac9a8
--     80014a58  sb   zero, DAT_800bbe5c                 ; Menue zu
--   Passt zur projektweiten Regel: RE1.5 bestaetigt game-weit mit QUADRAT.
--
-- ⛔ Der aktuelle Raum steht in Stage 0x800B0FE0 / Index 0x800B0FE2. 0x800B0FE6 ist der
--   VORHERIGE Raum (@0x8001d644) — danach zu vergleichen war falsch.
--
-- ⚠ Der Toggle ist der Grund, warum man NICHT dauernd SELECT takten darf: jede weitere Flanke
--   schliesst das Menue wieder. Also: druecken bis 0x800BBE5C==1, dann SOFORT aufhoeren.
-- ⚠ Im Haltepunkt-Rueckruf `return true` — `false` LOESCHT den Haltepunkt (gemessen, siehe Skill).

local OUT = [[C:\workspace\git\reAi_v2\tools\redux\jump_menu_out.txt]]
local ZIEL_STAGE, ZIEL_IDX = 0, 0x03          -- ROOM1030

local frames, out, spielerAb = 0, nil, -1
local phase, seit = 0, 0                       -- 0=SELECT  1=Ziel setzen  2=bestaetigen  3=messen
local naufruf = 0

local function u8(m, a) return m[bit.band(a, 0x1fffff)] end
local function w8(m, a, v) m[bit.band(a, 0x1fffff)] = v end
local function u16(m, a) local o=bit.band(a,0x1fffff); return m[o]+m[o+1]*256 end
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

local function gegner(m)
  local l = {}
  for i = 0, 31 do
    local e = 0x800acc2c + i * 0x1F4
    local typ = u8(m, e + 8)
    if bit.band(u32(m, e), 0x8000) == 0 and typ ~= 0 and typ < 0x80 then
      l[#l+1] = { slot = i, e = e, typ = typ }
    end
  end
  return l
end

function DrawImguiFrame()
  frames = frames + 1

  if out == nil then
    out = io.open(OUT, 'w')
    if out == nil then return end
    out:write("Sprung nach ROOM1030 ueber das Debug-Menue\n")
    _G.__bps = {}
    pcall(function()
      _G.__bps[1] = PCSX.addBreakpoint(0x8001443c, 'Exec', 4, 'menuein', function()
        naufruf = naufruf + 1
        return true
      end)
    end)
    out:flush()
  end

  local ok, err = pcall(function()
    local m = PCSX.getMemPtr()
    local px, pz = s32(m, 0x800aca88), s32(m, 0x800aca90)
    local offen = u8(m, 0x800BBE5C)
    local SEL, BESTAETIGE = PCSX.CONSTS.PAD.BUTTON.SELECT, PCSX.CONSTS.PAD.BUTTON.SQUARE

    if spielerAb < 0 and (px ~= 0 or pz ~= 0) then
      spielerAb = frames
      out:write(string.format("SPIELER ab Bild %d\n", frames)); out:flush()
    end

    if phase == 0 then
      -- SELECT takten, bis die Offen-Kennung steht
      if offen ~= 0 then
        taste(SEL, false)
        phase, seit = 1, frames
        out:write(string.format("MENUE OFFEN ab Bild %d — SELECT aus, Ziel eintragen\n", frames))
        out:flush()
      elseif spielerAb > 0 then
        local t = (frames - spielerAb) % 32
        taste(SEL, t < 8)
      end

    elseif phase == 1 then
      -- Zeile = JUMP (1), Stage + Index auf das Ziel. Jedes Bild schreiben, das Menue
      -- ueberschreibt den Index sonst beim naechsten Oeffnen wieder mit DAT_800b0fe2.
      w8(m, 0x800BBE5D, 1)
      w8(m, 0x800BBE5E, ZIEL_STAGE)
      w8(m, 0x800BBE5F + ZIEL_STAGE, ZIEL_IDX)
      if frames - seit > 60 then
        phase, seit = 2, frames
        out:write(string.format("Ziel steht (Zeile=%d Stage=%d Idx=%02x) — jetzt QUADRAT\n",
          u8(m, 0x800BBE5D), u8(m, 0x800BBE5E), u8(m, 0x800BBE5F))); out:flush()
      end

    elseif phase == 2 then
      -- Ziel weiter halten und QUADRAT takten (8 an / 24 aus) fuer die rohe Flanke 0x80
      w8(m, 0x800BBE5D, 1)
      w8(m, 0x800BBE5E, ZIEL_STAGE)
      w8(m, 0x800BBE5F + ZIEL_STAGE, ZIEL_IDX)
      local t = (frames - seit) % 32
      taste(BESTAETIGE, t < 8)
      -- Raumwechsel erkennen: 0x800b0fe2 = aktueller Raumindex
      if u8(m, 0x800B0FE2) == ZIEL_IDX and u8(m, 0x800B0FE0) == ZIEL_STAGE then
        taste(BESTAETIGE, false)
        phase, seit = 3, frames
        out:write(string.format("\n*** IM ZIELRAUM ab Bild %d ***\n", frames)); out:flush()
      end
      if frames - seit > 2400 then
        out:write("QUADRAT blieb wirkungslos — Abbruch der Bestaetigung\n"); out:flush()
        phase, seit = 3, frames
      end

    else
      if (frames - seit) % 300 ~= 0 then return end
      local g = gegner(m)
      -- Die Kriech-Kette ist in analysis/room1030_crawl_mechanism.md byte-belegt. Live pruefbar
      -- sind genau die Felder, die der Sub-Modus-0x10-Toggle FUN_80104f80 schreibt:
      --   @0x80104fd4 sb +0x93 |= 1     @0x80104fec sb +0x94 = 0x12   (Kriech-Clip)
      --   @0x80104ffc sb +0x95 = rng&3  @0x8010501c sb +0x8F = 0x0F
      --   @0x80105050 sb +0x9F = 1 NUR wenn (+0x09 & 0x80) CLEAR
      -- Freigabe kommt von sub07 als +0x1C4 |= 0x1000 (Glied 7/8).
      -- ROOM1030 spawnt 20x Typ 0x16 (@0x8011e8a0-a4 sw v0,11268(at) -> 0x80072c04).
      local nkriech = 0
      for i = 1, #g do if u8(m, g[i].e + 0x94) == 0x12 then nkriech = nkriech + 1 end end
      out:write(string.format("\n== Bild %d  Raum St %d Idx %02x  Spieler (%d,%d)  Gegner %d  davon Clip 0x12: %d ==\n",
        frames, u8(m, 0x800B0FE0), u8(m, 0x800B0FE2), px, pz, #g, nkriech))
      for i = 1, math.min(#g, 10) do
        local e = g[i].e
        out:write(string.format(
          "   Slot %2d Typ %02x grid %02x  %d/%d/%d  Clip %02x Bild %02x  +93=%02x +8F=%02x +9F=%02x  1C4=%08x  (%d,%d)\n",
          g[i].slot, g[i].typ, u8(m, e + 9), u8(m, e + 4), u8(m, e + 5), u8(m, e + 6),
          u8(m, e + 0x94), u8(m, e + 0x95),
          u8(m, e + 0x93), u8(m, e + 0x8F), u8(m, e + 0x9F), u32(m, e + 0x1C4),
          s32(m, e + 0x34), s32(m, e + 0x3c)))
      end
      out:flush()
    end

    if frames % 1500 == 0 then
      out:write(string.format("B%5d | Phase %d | offen=%d Zeile=%d Stage=%d Idx=%02x | Raum St %d Idx %02x | n=%d | Spieler (%d,%d)\n",
        frames, phase, offen, u8(m, 0x800BBE5D), u8(m, 0x800BBE5E), u8(m, 0x800BBE5F),
        u8(m, 0x800B0FE0), u8(m, 0x800B0FE2), naufruf, px, pz))
      out:flush()
    end

    if frames >= 20000 then
      out:write("\nfertig\n"); out:close(); PCSX.quit(0)
    end
  end)
  if not ok and out ~= nil then out:write("FEHLER: " .. tostring(err) .. "\n"); out:flush() end
end
