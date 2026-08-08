-- crawl_watch.lua — das Kriechen unter dem Rolltor in ROOM1030 im Original MESSEN.
--
-- Vorlauf (jump_menu.lua) hat den Sprung geloest und ROOM1030 erreicht: 6 Gegner Typ 0x16,
-- exakt so viele wie der byte-belegte Spawn-Cap `24 12 06 00` @0x1de2 = work_vars[0x12] = 6.
-- ABER: dort wurde alle 300 Bilder abgetastet, und ein Kriechvorgang dauert weniger. Slot 0 sprang
-- zwischen zwei Stichproben von z=-27878 auf z=-25004 — genau die Strecke unter dem Tor. Der
-- Vorgang lag also ZWISCHEN den Abtastpunkten.
--
-- Deshalb hier EREIGNISGESTEUERT: jede Aenderung von +0x94 (Clip) wird sofort protokolliert.
-- Laut analysis/room1030_crawl_mechanism.md ist  +0x94 = 0x12 der UEBERGANG, 0x1A das KRIECHEN.
-- (Im Vorlauf habe ich nur auf 0x12 geachtet — 0x1A gar nicht gesucht.)
--
-- Mitgeschrieben werden die Felder der belegten Kette:
--   +0x0B  AOT-Stempel (Glied 1, @0x80042f5c / @0x80042fc4 sb v0,11(s1))
--   +0x06  Sub-Modus-Phase (Dispatch @0x80104f90 lbu v1,6(a0))
--   +0x93  @0x80104fd4 |= 1      +0x8F  @0x8010501c = 0x0F
--   +0x9F  @0x80105050 (nur wenn +0x09 & 0x80 CLEAR)
--   +0x1C4 low half — Freigabe-Bit 0x1000 aus sub07 (Glied 7/8)
--
-- ⛔ NACHTRAG nach der ersten Messung: der Trigger ist ZOMBIE-getrieben, nicht spieler-getrieben.
-- sub06 (Glied 6) testet `3e 00 0f 00 05 00` = member15 == 5, also den AOT-Stempel auf
-- entity+0x0B (Glied 1, @0x80042f5c / @0x80042fc4 sb v0,11(s1)); erst dann `18 07` Gosub 7 ->
-- +0x1C4 |= 0x1000. Gemessen stand +0x0B bei allen sechs Zombies auf 0x00 (Zone 0), nie auf 4/5/6
-- — die Kette konnte also gar nicht anlaufen. Den Spieler blind laufen zu lassen war zudem
-- falsch: er lief bei konstantem z=-22303 von x=-8200 auf x=+484, WEG von den Zombies.
-- Deshalb jetzt: Spieler steht still, und protokolliert wird jede Aenderung von +0x0B UND +0x94,
-- dazu regelmaessig die Positionen und die Flag-Bank 5 (Wort 0 = 0x800B1028 persistente
-- Pro-Gegner-Latches, Wort 1 = 0x800B102C Ein-Frame-Signale, gewischt @0x8003ec1c).
--
-- ⚠ Rueckruf `return true` — `false` LOESCHT den Haltepunkt. ⚠ Bestaetigen mit QUADRAT (0x80).

local OUT = [[C:\workspace\git\reAi_v2\tools\redux\crawl_watch_out.txt]]
local ZIEL_STAGE, ZIEL_IDX = 0, 0x03          -- ROOM1030

local frames, out, spielerAb = 0, nil, -1
local phase, seit = 0, 0
local letzterClip, letzterAot, nzeilen = {}, {}, 0
local oderW0, oderW1, versetzt = 0, 0, false

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

function DrawImguiFrame()
  frames = frames + 1

  if out == nil then
    out = io.open(OUT, 'w')
    if out == nil then return end
    out:write("ROOM1030 — Clip-Wechsel aller Gegner, ereignisgesteuert\n")
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
        out:write(string.format("\n*** IM ZIELRAUM ab Bild %d — ab hier jeder Clip-Wechsel ***\n", frames))
        out:flush()
      end
      if frames - seit > 2400 then
        out:write("QUADRAT wirkungslos\n"); out:flush(); phase, seit = 3, frames
      end

    else
      -- JEDES Bild alle Gegner prüfen und NUR bei Clip-Wechsel schreiben.
      -- Nur die ersten 8 Slots: ROOM1030 haelt 6 Gegner (Cap work_vars[0x12] = 6, `24 12 06 00`
      -- @0x1de2). 32 Slots x ~12 Byte JEDES Bild ueber die Lua-Speicherschnittstelle kosten
      -- messbar Emulatorleistung.
      for i = 0, 7 do
        local e = 0x800acc2c + i * 0x1F4
        local typ = u8(m, e + 8)
        if bit.band(u32(m, e), 0x8000) == 0 and typ ~= 0 and typ < 0x80 then
          local c = u8(m, e + 0x94)
          local a = u8(m, e + 0x0B)
          if letzterClip[i] ~= c or letzterAot[i] ~= a then
            if letzterClip[i] ~= nil and nzeilen < 400 then
              nzeilen = nzeilen + 1
              out:write(string.format(
                "B%6d Slot %2d Typ %02x  Clip %02x -> %02x  Zust %d/%d/%d  +0B=%02x +06=%02x +93=%02x +8F=%02x +9F=%02x 1C4lo=%04x  (%d,%d)  Spieler (%d,%d)\n",
                frames, i, typ, letzterClip[i], c,
                u8(m, e + 4), u8(m, e + 5), u8(m, e + 6),
                u8(m, e + 0x0B), u8(m, e + 0x06), u8(m, e + 0x93),
                u8(m, e + 0x8F), u8(m, e + 0x9F), u16(m, e + 0x1C4),
                s32(m, e + 0x34), s32(m, e + 0x3c), px, pz))
              out:flush()
            end
            letzterClip[i] = c
            letzterAot[i] = a
          end
        end
      end

      -- Flag-Bank 5 JEDES Bild ODER-akkumulieren. Wort 1 wird am Ende JEDES VM-Laufs gewischt
      -- (@0x8003ec1c sw zero,4140(at) = 0x800B102C) — es sind Ein-Bild-Signale. Alle 600 Bilder
      -- abzutasten konnte sie gar nicht sehen; das war ein Messfehler der Vorrunde.
      oderW0 = bit.bor(oderW0, u32(m, 0x800B1028))
      oderW1 = bit.bor(oderW1, u32(m, 0x800B102C))

      -- HYPOTHESE, die hier geprueft wird: die Zombies stehen mit +0x0B = 5 IN der HINWEG-Zone,
      -- aber sub06 ist auf `21 05 22 01` = Ck(bank5, bit 0x22, ==1) gegatet. Dieses Bit setzt der
      -- sce-4-Handler von AOT-Slot 5 — also der SPIELER beim Betreten derselben Zone. Er stand
      -- bei z=-22303, die Zombies bei z=-24838. Deshalb EINMAL dorthin versetzen und beobachten.
      -- (Positionsschreiben ist hier legitim: der AOT-Scan ist ein Punkt-Test pro Bild.)
      if not versetzt and frames - seit > 1200 then
        versetzt = true
        local o = bit.band(0x800aca90, 0x1fffff)
        local zneu = -24300
        local u = zneu + 4294967296
        for b = 0, 3 do m[o+b] = bit.band(bit.rshift(u, b*8), 0xff) end
        out:write(string.format("\n*** SPIELER versetzt nach z=%d bei Bild %d ***\n", zneu, frames))
        out:flush()
      end

      -- Regelmaessige Vollaufnahme: ohne sie verliert man die Positionen komplett, sobald die
      -- Clips aufhoeren zu wechseln (genau das ist in der Vormessung passiert).
      if (frames - seit) % 600 == 0 then
        out:write(string.format("-- B%6d  Flag5 W0=%08x W1=%08x  ODER W0=%08x W1=%08x  Spieler (%d,%d)\n",
          frames, u32(m, 0x800B1028), u32(m, 0x800B102C), oderW0, oderW1, px, pz))
        for k = 0, 7 do
          local e2 = 0x800acc2c + k * 0x1F4
          if bit.band(u32(m, e2), 0x8000) == 0 and u8(m, e2 + 8) == 0x16 then
            out:write(string.format("     Slot %d  +0B=%02x Clip %02x Bild %02x  Zust %d/%d/%d  1C4lo=%04x  (%d,%d)\n",
              k, u8(m, e2 + 0x0B), u8(m, e2 + 0x94), u8(m, e2 + 0x95),
              u8(m, e2 + 4), u8(m, e2 + 5), u8(m, e2 + 6), u16(m, e2 + 0x1C4),
              s32(m, e2 + 0x34), s32(m, e2 + 0x3c)))
          end
        end
        out:flush()
      end
    end

    if frames % 1500 == 0 then
      out:write(string.format("B%5d | Phase %d | Raum St %d Idx %02x | Spieler (%d,%d) | Zeilen %d\n",
        frames, phase, u8(m, 0x800B0FE0), u8(m, 0x800B0FE2), px, pz, nzeilen))
      out:flush()
    end

    if frames >= 20000 then out:write("\nfertig\n"); out:close(); PCSX.quit(0) end
  end)
  if not ok and out ~= nil then out:write("FEHLER: " .. tostring(err) .. "\n"); out:flush() end
end
