-- crawl_force.lua — die Kriech-Kette am ORIGINAL gezielt ausloesen und vermessen.
--
-- Stand (alles belegt, siehe analysis/room1030_crawl_mechanism.md §11-13):
--   flag(3,0x74)[Story] + flag(5,0x20) + flag(5,0x22)[Spieler in AOT-Zone 5]
--     -> Set flag(4,0x0f)  ROOM1030.RDT @0x2198   (einziger Setzer in ganz STAGE1)
--     -> Set flag(5,0x14)  @0x2008                (gegatet auf flag(4,0x0f))
--     -> sub02 laeuft      @0x21a6                (gegatet auf flag(5,0x14))
--     -> Gosub 7 -> +0x1C4 |= 0x1000 -> Zombie legt sich hin und kriecht (+0x94 0x12 -> 0x1A)
--
-- Gemessen war: flag(5,0x22) WIRD gesetzt (AOT funktioniert), flag(5,0x20) und flag(5,0x14) nicht.
-- Ein Debug-Sprung bringt den Story-Zustand nicht mit.
--
-- MINIMALER EINGRIFF, um den MECHANISMUS zu sehen: flag(4,0x0f) selbst setzen. Das ist genau die
-- eine Stelle, an der die Story-Vorbedingung haengt; alles danach laeuft dann original.
--
-- Flag-Adressierung (Helfer @0x8003fdd4-fe74, selbst disassembliert):
--   wort = bank_base + (bit>>5)*4 ;  maske = 0x80000000 >> (bit & 31)   [MSB-ZUERST!]
--   Bank-Basistabelle @0x80074664: bank3 = 0x800B0FF8, bank4 = 0x800B1018, bank5 = 0x800B1028
--   flag(4,0x0f) -> 0x800B1018, maske 0x00010000
--   flag(5,0x14) -> 0x800B1028, maske 0x00000800
--   flag(5,0x20/21/22) -> 0x800B102C, maske 0x80000000 / 0x40000000 / 0x20000000
--   flag(3,0x74) -> 0x800B1004, maske 0x00000800
--
-- ⚠ Rueckruf `return true` — `false` LOESCHT den Haltepunkt. ⚠ Bestaetigen mit QUADRAT (0x80).

local OUT = [[C:\workspace\git\reAi_v2\tools\redux\crawl_force_out.txt]]
local ZIEL_STAGE, ZIEL_IDX = 0, 0x03          -- ROOM1030

local frames, out, spielerAb = 0, nil, -1
local phase, seit = 0, 0
local letzterClip, letzterAot, nzeilen = {}, {}, 0
local oderW0, oderW1, versetzt, erzwungen = 0, 0, false, false
local kriechGesehen = false

local function u8(m, a) return m[bit.band(a, 0x1fffff)] end
local function w8(m, a, v) m[bit.band(a, 0x1fffff)] = v end
local function u16(m, a) local o=bit.band(a,0x1fffff); return m[o]+m[o+1]*256 end
local function u32(m, a)
  local o = bit.band(a, 0x1fffff)
  return m[o] + m[o+1]*256 + m[o+2]*65536 + m[o+3]*16777216
end
local function w32(m, a, v)
  local o = bit.band(a, 0x1fffff)
  for b = 0, 3 do m[o+b] = bit.band(bit.rshift(v, b*8), 0xff) end
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
    out:write("ROOM1030 — Kriech-Kette gezielt ausgeloest (flag(4,0x0f) gesetzt)\n")
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
      if offen ~= 0 then
        taste(SEL, false); phase, seit = 1, frames
        out:write(string.format("MENUE OFFEN ab Bild %d\n", frames)); out:flush()
      elseif spielerAb > 0 then
        taste(SEL, ((frames - spielerAb) % 32) < 8)
      end

    elseif phase == 1 then
      w8(m, 0x800BBE5D, 1); w8(m, 0x800BBE5E, ZIEL_STAGE)
      w8(m, 0x800BBE5F + ZIEL_STAGE, ZIEL_IDX)
      if frames - seit > 60 then phase, seit = 2, frames
        out:write("Ziel steht — QUADRAT\n"); out:flush() end

    elseif phase == 2 then
      w8(m, 0x800BBE5D, 1); w8(m, 0x800BBE5E, ZIEL_STAGE)
      w8(m, 0x800BBE5F + ZIEL_STAGE, ZIEL_IDX)
      taste(BESTAETIGE, ((frames - seit) % 32) < 8)
      if u8(m, 0x800B0FE2) == ZIEL_IDX and u8(m, 0x800B0FE0) == ZIEL_STAGE then
        taste(BESTAETIGE, false); phase, seit = 3, frames
        out:write(string.format("\n*** IM ZIELRAUM ab Bild %d ***\n", frames)); out:flush()
      end
      if frames - seit > 2400 then
        out:write("QUADRAT wirkungslos\n"); out:flush(); phase, seit = 3, frames
      end

    else
      oderW0 = bit.bor(oderW0, u32(m, 0x800B1028))
      oderW1 = bit.bor(oderW1, u32(m, 0x800B102C))

      -- 1) Spieler in die Zombie-Zone versetzen -> AOT-Slot 5 setzt flag(5,0x22) (nachgewiesen)
      if not versetzt and frames - seit > 900 then
        versetzt = true
        w32(m, 0x800aca90, -24300 + 4294967296)
        out:write(string.format("\n*** SPIELER versetzt (z=-24300) bei Bild %d ***\n", frames))
        out:flush()
      end

      -- 2) flag(4,0x0f) setzen — die Stelle, an der die Story-Vorbedingung haengt.
      --    JEDES Bild, damit ein etwaiger Clear die Messung nicht wieder zumacht.
      if frames - seit > 1500 then
        if not erzwungen then
          erzwungen = true
          out:write(string.format("*** flag(4,0x0f) gesetzt ab Bild %d (0x800B1018 |= 0x00010000) ***\n", frames))
          out:flush()
        end
        w32(m, 0x800B1018, bit.bor(u32(m, 0x800B1018), 0x00010000))
      end

      -- 3) jede Aenderung von Clip oder AOT-Stempel protokollieren
      for i = 0, 7 do
        local e = 0x800acc2c + i * 0x1F4
        local typ = u8(m, e + 8)
        if bit.band(u32(m, e), 0x8000) == 0 and typ ~= 0 and typ < 0x80 then
          local c = u8(m, e + 0x94)
          local a = u8(m, e + 0x0B)
          if c == 0x12 or c == 0x1a then kriechGesehen = true end
          if letzterClip[i] ~= c or letzterAot[i] ~= a then
            if letzterClip[i] ~= nil and nzeilen < 400 then
              nzeilen = nzeilen + 1
              out:write(string.format(
                "B%6d Slot %2d  Clip %02x -> %02x  Zust %d/%d/%d  +0B=%02x +06=%02x +8F=%02x +9F=%02x 1C4lo=%04x  (%d,%d)\n",
                frames, i, letzterClip[i], c,
                u8(m, e + 4), u8(m, e + 5), u8(m, e + 6),
                a, u8(m, e + 0x06), u8(m, e + 0x8F), u8(m, e + 0x9F), u16(m, e + 0x1C4),
                s32(m, e + 0x34), s32(m, e + 0x3c)))
              out:flush()
            end
            letzterClip[i] = c
            letzterAot[i] = a
          end
        end
      end

      if (frames - seit) % 600 == 0 then
        out:write(string.format(
          "-- B%6d  b3w3=%08x b4w0=%08x b5w0=%08x b5w1=%08x | ODER W0=%08x W1=%08x | flag(5,0x14)=%d flag(3,0x74)=%d | Kriechen=%s\n",
          frames, u32(m, 0x800B1004), u32(m, 0x800B1018),
          u32(m, 0x800B1028), u32(m, 0x800B102C), oderW0, oderW1,
          (bit.band(oderW0, 0x00000800) ~= 0) and 1 or 0,
          (bit.band(u32(m, 0x800B1004), 0x00000800) ~= 0) and 1 or 0,
          tostring(kriechGesehen)))
        out:flush()
      end
    end

    if frames % 2000 == 0 then
      out:write(string.format("B%5d | Phase %d | Raum St %d Idx %02x | Spieler (%d,%d)\n",
        frames, phase, u8(m, 0x800B0FE0), u8(m, 0x800B0FE2), px, pz))
      out:flush()
    end

    if frames >= 20000 then
      out:write(string.format("\nERGEBNIS: Kriechen gesehen = %s | ODER W0=%08x W1=%08x\n",
        tostring(kriechGesehen), oderW0, oderW1))
      out:write("fertig\n"); out:close(); PCSX.quit(0)
    end
  end)
  if not ok and out ~= nil then out:write("FEHLER: " .. tostring(err) .. "\n"); out:flush() end
end
