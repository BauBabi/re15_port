-- crawl_cycle2.lua — ROOM1030 Kriechzyklus, Lauf 2: das AUFSTEHEN messen.
--
-- Lauf 1 (crawl_cycle.lua -> crawl_cycle_out.txt) hat Hinlegen+Kriechen+Choreografie geliefert,
-- aber KEIN Aufstehen: der Kriecher erreichte den Spieler (der IN Zone 4 stand), wechselte in
-- Verfolgungs-Submodus 3 (DECIDE-Stub — das 0x2000-Gate liegt nur in Submodus 0 FUN_801035f8
-- @0x80103690-c4) und attackierte; bei gf=4825 fror das Spiel ein (Spieler-Kontakt/Grab ->
-- mutmasslich CD-Ladevorgang -> bekannter PCSX-Redux-CD-Deadlock, README 0x80062130-Schleife).
--
-- Aenderungen ggue. Lauf 1:
--   1. SPIELER-TELEPORT: 60 gf nach Kettenstart einmalig nach (-8000,-12000) — weit ausserhalb
--      der Reichweite, aber so, dass der Kriechpfad Zone 4 (x[-14800,-2800] z[-22500,-20300])
--      durchquert -> stamp 4 -> sub05 0x2000 -> Aufstehen in Submodus 0 statt Verfolgung.
--      (Zonen sind GEGNER-maskiert, flags 0x42 — die Spielerposition beeinflusst die Stempel
--      nicht; nur das AI-Targeting. Positions-Poke wie in crawl_watch.lua erprobt.)
--   2. P-Samples JEDEN Spiel-Frame (Savestate-C-Messung: Positions-Delta pro Frame) waehrend
--      Toggle/Kriechen, plus 45 Frames Nachlauf nach dem Aufstehen (Steh-Halt?).
--   3. GF-STALL-Diagnose: steht gf >600 GUI-Bilder, PC der CPU loggen (CD-Deadlock vs. Crash).
-- Rest identisch zu crawl_cycle.lua (Pokes, Eskalation, Messpunkt BP 0x80030444, QUADRAT).

local OUT = [[C:\workspace\git\reAi_v2\tools\redux\crawl_cycle2_out.txt]]
local ZIEL_STAGE, ZIEL_IDX = 0, 0x03          -- ROOM1030

local frames, gf, out, spielerAb = 0, 0, nil, -1
local phase, seit = 0, 0
local versuch = 1
local rj_offen_prev = 0
local settle_bis = 0
local tstart = 0
local FENSTER = 1500

local kette_lebt, kette_gf = false, -1
local teleportiert = false
local flag514_ab = -1
local vollzyklus, ende_bei = false, -1
local nev, NEVMAX = 0, 20000
local letzte, nachlauf = {}, {}
local fprev = nil
local q, qh, qt = {}, 1, 0
local last_gf, last_gf_frame, stall_gemeldet = 0, 0, 0

local function push(s) qt = qt + 1; q[qt] = s end

local function u8(m, a) return m[bit.band(a, 0x1fffff)] end
local function w8(m, a, v) m[bit.band(a, 0x1fffff)] = v end
local function u16(m, a) local o = bit.band(a, 0x1fffff); return m[o] + m[o+1]*256 end
local function u32(m, a)
  local o = bit.band(a, 0x1fffff)
  return m[o] + m[o+1]*256 + m[o+2]*65536 + m[o+3]*16777216
end
local function s32(m, a)
  local v = u32(m, a); if v >= 0x80000000 then v = v - 0x100000000 end; return v
end
local function w32(m, a, v)
  local o = bit.band(a, 0x1fffff)
  if v < 0 then v = v + 4294967296 end
  for b = 0, 3 do m[o+b] = bit.band(bit.rshift(v, b*8), 0xff) end
end
local function hex32(v)
  return string.format("%04x%04x", math.floor(v / 65536) % 65536, v % 65536)
end
local function orbyte(m, a, v)
  local o = bit.band(a, 0x1fffff); m[o] = bit.bor(m[o], v)
end

local function taste(knopf, an)
  pcall(function()
    local p = PCSX.SIO0.slots[1].pads[1]
    if an then p.setOverride(knopf) else p.clearOverride(knopf) end
  end)
end

local function gueltig(p) return p >= 0x80000000 and p < 0x80200000 end
local function torzelle(m, region)
  local base = u32(m, 0x800AC778); if not gueltig(base) then return -1 end
  local tab = u32(m, base + 0x20); if not gueltig(tab) then return -1 end
  local rp = u32(m, tab + (region + 1) * 4); if not gueltig(rp) then return -1 end
  return u8(m, rp + 6 * 12 + 9)
end

local function messe(m)
  if versuch >= 1 then orbyte(m, 0x800B102C + 3, 0x80) end   -- flag(5,0x20)
  if versuch >= 2 then orbyte(m, 0x800B1018 + 2, 0x01) end   -- flag(4,0x0f)
  if versuch >= 3 then orbyte(m, 0x800B1028 + 1, 0x08) end   -- flag(5,0x14)

  if gf < settle_bis then return end

  -- Spieler einmalig aus der Reichweite nehmen (siehe Kopf, Punkt 1)
  if kette_lebt and not teleportiert and gf >= kette_gf + 60 then
    teleportiert = true
    w32(m, 0x800aca88, -8000)    -- Spieler +0x34 (X)
    w32(m, 0x800aca90, -12000)   -- Spieler +0x3C (Z)
    push(string.format(">>> SPIELER teleportiert nach (-8000,-12000) bei gf=%d\n", gf))
  end

  local px, pz = s32(m, 0x800aca88), s32(m, 0x800aca90)

  local b5w0, b5w1 = u32(m, 0x800B1028), u32(m, 0x800B102C)
  local b4w0, b3w3 = u32(m, 0x800B1018), u32(m, 0x800B0FF8 + 12)
  local t2, t3 = torzelle(m, 2), torzelle(m, 3)
  local wv7 = u16(m, 0x800B0FD0 + 14)
  local ftup = hex32(b5w0) .. hex32(b5w1) .. hex32(b4w0) .. hex32(b3w3) .. t2 .. "/" .. t3 .. wv7
  if ftup ~= fprev then
    fprev = ftup
    push(string.format("F gf=%6d B5W0=%s B5W1=%s B4W0=%s B3W3=%s tor2=%02x tor3=%02x wv7=%d\n",
      gf, hex32(b5w0), hex32(b5w1), hex32(b4w0), hex32(b3w3), t2 % 256, t3 % 256, wv7))
    if flag514_ab < 0 and bit.band(b5w0, 0x800) ~= 0 then
      flag514_ab = gf
      push(string.format(">>> flag(5,0x14) GESETZT ab gf=%d (Versuch %d)\n", gf, versuch))
    end
  end

  for i = 0, 7 do
    local e = 0x800acc2c + i * 0x1F4
    if bit.band(u32(m, e), 0x8000) == 0 and u8(m, e + 8) == 0x16 then
      local c   = u8(m, e + 0x94)
      local f95 = u8(m, e + 0x95)
      local s4, s5, s6 = u8(m, e + 4), u8(m, e + 5), u8(m, e + 6)
      local g9  = u8(m, e + 0x09)
      local b0b = u8(m, e + 0x0B)
      local c1c4 = u16(m, e + 0x1C4)
      local d7  = u8(m, e + 0x1D7)
      local tup = string.format("%02x%02x%02x%02x%02x%02x%04x%02x", c, s4, s5, s6, g9, b0b, c1c4, d7)
      local alt = letzte[i]
      if alt ~= tup then
        letzte[i] = tup
        if nev < NEVMAX then
          nev = nev + 1
          push(string.format(
            "E gf=%6d S%d 94=%02x 95=%02x  04/05/06=%02x/%02x/%02x 09=%02x 0B=%02x 1C4=%04x 1D7=%02x  9F=%02x 8C=%04x 8F=%02x 93=%02x 1D8=%02x pos=(%d,%d) P=(%d,%d)\n",
            gf, i, c, f95, s4, s5, s6, g9, b0b, c1c4, d7,
            u8(m, e + 0x9F), u16(m, e + 0x8C), u8(m, e + 0x8F), u8(m, e + 0x93), u8(m, e + 0x1D8),
            s32(m, e + 0x34), s32(m, e + 0x3c), px, pz))
        elseif nev == NEVMAX then
          nev = nev + 1; push("!! Ereignis-Limit erreicht\n")
        end
        if not kette_lebt and (bit.band(c1c4, 0x1000) ~= 0 or c == 0x12 or c == 0x1a) then
          kette_lebt, kette_gf = true, gf
          push(string.format(">>> KETTE LEBT ab gf=%d (Slot %d, Versuch %d)\n", gf, i, versuch))
        end
        if alt ~= nil and not vollzyklus then
          local altg9 = tonumber(string.sub(alt, 9, 10), 16)
          if altg9 and bit.band(altg9, 0x80) ~= 0 and bit.band(g9, 0x80) == 0 and d7 == 4 then
            vollzyklus = true
            push(string.format(">>> VOLLER ZYKLUS: Slot %d wieder aufrecht ab gf=%d\n", i, gf))
          end
        end
      end
      -- P-Samples JEDEN Spiel-Frame in Toggle/Kriechphase + 45 Frames Nachlauf
      local aktiv = (c == 0x12 or s5 == 0x10 or bit.band(g9, 0x80) ~= 0)
      if aktiv then
        nachlauf[i] = 45
      elseif (nachlauf[i] or 0) > 0 then
        nachlauf[i] = nachlauf[i] - 1; aktiv = true
      end
      if aktiv then
        push(string.format("P gf=%6d S%d 94=%02x 95=%02x 05=%02x 06=%02x 8C=%04x 9F=%02x 1C4=%04x 09=%02x 1D7=%02x pos=(%d,%d)\n",
          gf, i, c, f95, s5, s6, u16(m, e + 0x8C), u8(m, e + 0x9F), c1c4, g9, d7,
          s32(m, e + 0x34), s32(m, e + 0x3c)))
      end
    end
  end

  if gf % 400 == 0 then
    push(string.format("S gf=%6d B%d V%d | B5W0=%s B5W1=%s B4W0=%s | tor2=%02x tor3=%02x | wv4=%d wv7=%d | St%d/Idx%02x | P=(%d,%d)\n",
      gf, frames, versuch, hex32(b5w0), hex32(b5w1), hex32(b4w0), t2 % 256, t3 % 256,
      u16(m, 0x800B0FD0 + 8), wv7, u8(m, 0x800B0FE0), u8(m, 0x800B0FE2), px, pz))
    for k = 3, 6 do
      local p = u32(m, 0x800AC9B0 + 4 * k)
      if gueltig(p) then
        push(string.format("   aot%d sce=%02x flags=%02x\n", k, u8(m, p), u8(m, p + 1)))
      end
    end
    for k = 0, 7 do
      local e2 = 0x800acc2c + k * 0x1F4
      if bit.band(u32(m, e2), 0x8000) == 0 and u8(m, e2 + 8) == 0x16 then
        push(string.format("   S%d 94=%02x 95=%02x 04/05/06=%02x/%02x/%02x 09=%02x 0B=%02x 1C4=%04x 1D7=%02x 9F=%02x 8C=%04x pos=(%d,%d)\n",
          k, u8(m, e2 + 0x94), u8(m, e2 + 0x95), u8(m, e2 + 4), u8(m, e2 + 5), u8(m, e2 + 6),
          u8(m, e2 + 0x09), u8(m, e2 + 0x0B), u16(m, e2 + 0x1C4), u8(m, e2 + 0x1D7),
          u8(m, e2 + 0x9F), u16(m, e2 + 0x8C), s32(m, e2 + 0x34), s32(m, e2 + 0x3c)))
      end
    end
  end
end

function DrawImguiFrame()
  frames = frames + 1

  if out == nil then
    out = io.open(OUT, 'w')
    if out == nil then return end
    out:write("ROOM1030 Kriechzyklus Lauf 2 — Aufstehen (gf = Spiel-Frame via BP 0x80030444)\n")
    out:flush()
    _G.__bps = {}
    pcall(function()
      _G.__bps[1] = PCSX.addBreakpoint(0x80030444, 'Exec', 4, 'gframe', function()
        gf = gf + 1
        if phase >= 3 then pcall(function() messe(PCSX.getMemPtr()) end) end
        return true               -- ⛔ false wuerde den Haltepunkt LOESCHEN
      end)
    end)
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
        taste(BESTAETIGE, false); phase, seit, tstart = 3, frames, frames
        settle_bis = gf + 10
        last_gf, last_gf_frame = gf, frames
        out:write(string.format("\n*** IM ZIELRAUM ab Bild %d gf=%d — Versuch 1: poke flag(5,0x20) ***\n", frames, gf))
        out:flush()
      end
      if frames - seit > 2400 then
        out:write("QUADRAT wirkungslos beim Erst-Sprung\n"); out:flush()
        phase, seit, tstart = 3, frames, frames
      end

    elseif phase == 3 then
      -- GF-Stall-Diagnose (CD-Deadlock 0x80062130/0x8006217c vs. Crash)
      if gf ~= last_gf then last_gf, last_gf_frame = gf, frames end
      if frames - last_gf_frame > 600 and frames - stall_gemeldet > 1500 then
        stall_gemeldet = frames
        local pc = 0
        pcall(function() pc = PCSX.getRegisters().pc end)
        out:write(string.format("!! GF STALL seit Bild %d (gf=%d) CPU pc=%s\n",
          last_gf_frame, gf, hex32(tonumber(pc) or 0))); out:flush()
      end
      if not kette_lebt and frames - tstart > FENSTER and versuch < 3 then
        versuch = versuch + 1
        out:write(string.format("\n*** ESKALATION auf Versuch %d bei Bild %d gf=%d ***\n", versuch, frames, gf))
        out:flush()
        phase, seit, rj_offen_prev = 4, frames, offen
      elseif not kette_lebt and frames - tstart > FENSTER * 2 and versuch >= 3 then
        out:write(string.format("\n*** AUCH VERSUCH 3 OHNE KETTE (Bild %d gf=%d) — messe weiter ***\n", frames, gf))
        out:flush()
        tstart = frames
      end
      if vollzyklus and ende_bei < 0 then
        ende_bei = frames + 1800
        out:write(string.format("*** Endphase: noch 1800 GUI-Bilder ab Bild %d ***\n", frames)); out:flush()
      end

    elseif phase == 4 then
      if offen ~= 0 then
        taste(SEL, false); phase, seit = 5, frames
        out:write(string.format("RE-JUMP: Menue offen ab Bild %d\n", frames)); out:flush()
      else
        taste(SEL, ((frames - seit) % 32) < 8)
        if frames - seit > 2400 then
          out:write("RE-JUMP: Menue oeffnet nicht — zurueck zum Messen\n"); out:flush()
          taste(SEL, false); phase, tstart = 3, frames
        end
      end

    elseif phase == 5 then
      w8(m, 0x800BBE5D, 1); w8(m, 0x800BBE5E, ZIEL_STAGE)
      w8(m, 0x800BBE5F + ZIEL_STAGE, ZIEL_IDX)
      if frames - seit > 60 then phase, seit, rj_offen_prev = 6, frames, 1
        out:write("RE-JUMP: Ziel steht — QUADRAT\n"); out:flush() end

    elseif phase == 6 then
      w8(m, 0x800BBE5D, 1); w8(m, 0x800BBE5E, ZIEL_STAGE)
      w8(m, 0x800BBE5F + ZIEL_STAGE, ZIEL_IDX)
      taste(BESTAETIGE, ((frames - seit) % 32) < 8)
      if rj_offen_prev ~= 0 and offen == 0 then
        taste(BESTAETIGE, false)
        settle_bis = gf + 90
        letzte, nachlauf, fprev = {}, {}, nil
        phase, tstart = 3, frames
        out:write(string.format("\n*** RE-ENTRY bestaetigt bei Bild %d gf=%d (Versuch %d) ***\n",
          frames, gf, versuch))
        out:flush()
      end
      rj_offen_prev = offen
      if frames - seit > 2400 then
        taste(BESTAETIGE, false)
        out:write("RE-JUMP: QUADRAT wirkungslos — zurueck zum Messen\n"); out:flush()
        phase, tstart = 3, frames
      end
    end

    if out ~= nil and qh <= qt then
      local n = 0
      while qh <= qt and n < 400 do
        out:write(q[qh]); q[qh] = nil; qh = qh + 1; n = n + 1
      end
      out:flush()
    end

    if frames % 1500 == 0 then
      out:write(string.format("B%5d gf=%6d | Phase %d Versuch %d | Kette=%s 514ab=%d | Raum St%d Idx%02x | P=(%d,%d)\n",
        frames, gf, phase, versuch, tostring(kette_lebt), flag514_ab,
        u8(m, 0x800B0FE0), u8(m, 0x800B0FE2), px, pz))
      out:flush()
    end

    if frames >= 40000 or (ende_bei > 0 and frames >= ende_bei) then
      while qh <= qt do out:write(q[qh]); q[qh] = nil; qh = qh + 1 end
      out:write(string.format("\nfertig (Bild %d, gf %d, Kette=%s, Vollzyklus=%s)\n",
        frames, gf, tostring(kette_lebt), tostring(vollzyklus)))
      out:close(); out = nil
      PCSX.quit(0)
    end
  end)
  if not ok and out ~= nil then out:write("FEHLER: " .. tostring(err) .. "\n"); out:flush() end
end
