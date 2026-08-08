-- crawl_cycle.lua — ROOM1030: den VOLLEN Kriechzyklus am ORIGINAL messen (Hardware-Ground-Truth).
--
-- Warum dieses Skript: der PC-Port kriecht jetzt, aber (1) "komische Bewegungen VOR dem Tor"
-- vor dem Hinlegen, (2) Aufstehen "ganz abrupt". Wir brauchen den tick-genauen SOLL-Trace.
--
-- §11 des Dossiers (analysis/room1030_crawl_mechanism.md): der Debug-Sprung zeigt das Kriechen
-- NIE, weil flag(4,0x0f)/flag(5,0x14) fehlen. Deshalb hier ESKALIERENDE Pokes:
--   Versuch 1: flag(5,0x20) poken (0x800B102C |= 0x80000000; Maske MSB-zuerst, §13, Helfer
--              @0x8003fe68 srlv v1,t0,a1 mit t0=0x80000000). Das ist die EINZIGE fehlende
--              Aussenbedingung des sub01-Gatters @0x2180 (Ck(3,0x74) setzt sub00 selbst,
--              flag(5,0x22) feuert organisch — gemessen, §13). sub01 setzt dann flag(4,0x0f)
--              und exec't sub08 -> Tor-Zellen + flag(5,0x14) LIVE = der Story-Pfad.
--   Versuch 2: flag(4,0x0f) poken (0x800B1018 |= 0x00010000 = 0x80000000>>15, §13) und den
--              Raum per Debug-Menue ERNEUT betreten, damit sub00 sein Ck(4,0x0f)-Gate nimmt
--              (Set(5,0x14) @0x2008 + Zellen 37 02/03 06 f7 @0x2000/0x2004).
--   Versuch 3: flag(5,0x14) direkt poken (0x800B1028 |= 0x00000800 = 0x80000000>>20, §13)
--              + nochmal Raum betreten. (Dann bleiben die Tor-Zellen ggf. solid — dokumentieren.)
--
-- MESSPUNKT: Exec-Haltepunkt auf 0x80030444 (Pad-Aufbereiter; main.c ruft ihn 1x pro Bild,
-- VOR der SCD-VM). Dort ist entity+0x0B der STABILE Stempel des Vor-Scans — nicht der
-- 0xFF-Wisch, den GUI-Zeit-Samples erwischen (VM-Ende @0x8003f194 -> FUN_8003ec28).
-- gf = echter Spiel-Frame-Zaehler. ⚠ Rueckruf IMMER `return true` (false LOESCHT den BP).
--
-- Gemessene Felder (Belege im Dossier §1/§2):
--   +0x94 Clip / +0x95 Anim-Frame     +0x04/05/06 State-Wort-Bytes (Toggle-Phase in +0x06)
--   +0x09 grid (0x81 = Kriech-Commit @0x801050d4)   +0x0B AOT-Stempel (@0x80042fc4)
--   +0x1C4 lhu (0x1000 sub07 / 0x2000 sub05)        +0x1D7 SCA-Maske (8 @0x801050f4)
--   +0x9F Richtungs-Latch (@0x80105050)  +0x8C Kriech-Timer (sh 0x1E @0x801036fc)
--   +0x8F Blend / +0x93 Busy / +0x1D8   Position +0x34/+0x3C
-- Tor-Zellen: *( *( *(0x800AC778) + 0x20 ) + (region+1)*4 ) + index*12, Byte +9
--   (Sca_id_set-Handler @0x8004175c-a8 selbst gelesen: lw 0x20(v0) / (rgn+1)<<2 / idx*12 / sb 9)
--
-- Bestaetigt wird im Debug-Menue mit QUADRAT (@0x80014a38 andi 0x80). SELECT-Toggle:
-- druecken bis 0x800BBE5C==1, dann SOFORT aufhoeren (jede weitere Flanke schliesst wieder).

local OUT = [[C:\workspace\git\reAi_v2\tools\redux\crawl_cycle_out.txt]]
local ZIEL_STAGE, ZIEL_IDX = 0, 0x03          -- ROOM1030

local frames, gf, out, spielerAb = 0, 0, nil, -1
local phase, seit = 0, 0        -- 0 SELECT / 1 Ziel / 2 QUADRAT / 3 MESSEN / 4-6 Re-Jump
local versuch = 1               -- Eskalationsstufe der Pokes
local rj_offen_prev = 0
local settle_bis = 0            -- gf, ab dem nach Re-Entry wieder geloggt wird
local tstart = 0                -- GUI-Frame des aktuellen Messfenster-Starts
local FENSTER = 1500            -- GUI-Frames ohne Kettenleben -> eskalieren

local kette_lebt, kette_gf = false, -1
local flag514_ab = -1
local vollzyklus, ende_bei = false, -1
local nev, NEVMAX = 0, 20000
local letzte = {}               -- pro Slot: Tupel-Cache
local psample = {}              -- pro Slot: letzter P-Sample-gf
local fprev = nil               -- Flag-Tupel-Cache
local q, qh, qt = {}, 1, 0

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
local function hex32(v)
  return string.format("%04x%04x", math.floor(v / 65536) % 65536, v % 65536)
end
-- Byte-weises OR — vermeidet LuaJIT-32bit-Vorzeichenfallen komplett.
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

-- ================= MESSUNG: laeuft im Exec-BP auf 0x80030444, 1x pro SPIEL-Frame ============
local function messe(m)
  -- Pokes (eskaliert). Byte-Lage little-endian: 0x800B102C+3 = Bit 0x80000000 usw.
  if versuch >= 1 then orbyte(m, 0x800B102C + 3, 0x80) end   -- flag(5,0x20)
  if versuch >= 2 then orbyte(m, 0x800B1018 + 2, 0x01) end   -- flag(4,0x0f) = 0x00010000
  if versuch >= 3 then orbyte(m, 0x800B1028 + 1, 0x08) end   -- flag(5,0x14) = 0x00000800

  if gf < settle_bis then return end

  local px, pz = s32(m, 0x800aca88), s32(m, 0x800aca90)

  -- Flag-/Zellen-Tupel (F-Zeile nur bei Aenderung)
  local b5w0, b5w1 = u32(m, 0x800B1028), u32(m, 0x800B102C)
  local b4w0, b3w3 = u32(m, 0x800B1018), u32(m, 0x800B0FF8 + 12)
  local t2, t3 = torzelle(m, 2), torzelle(m, 3)
  local wv7 = u16(m, 0x800B0FD0 + 14)
  local ftup = hex32(b5w0) .. hex32(b5w1) .. hex32(b4w0) .. hex32(b3w3) .. t2 .. "/" .. t3
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
        -- Voller Zyklus: Slot war Kriecher (grid-Bit 0x80) und ist wieder aufrecht (1D7==4)
        if alt ~= nil and not vollzyklus then
          local altg9 = tonumber(string.sub(alt, 9, 10), 16)
          if altg9 and bit.band(altg9, 0x80) ~= 0 and bit.band(g9, 0x80) == 0 and d7 == 4 then
            vollzyklus = true
            push(string.format(">>> VOLLER ZYKLUS: Slot %d wieder aufrecht ab gf=%d\n", i, gf))
          end
        end
      end
      -- P-Samples: waehrend Uebergang/Kriechen/Toggle alle 3 Spiel-Frames Frame+Position
      if c == 0x12 or c == 0x1a or s5 == 0x10 or bit.band(g9, 0x80) ~= 0 then
        if (psample[i] or -10) <= gf - 3 then
          psample[i] = gf
          push(string.format("P gf=%6d S%d 94=%02x 95=%02x 06=%02x 8C=%04x 9F=%02x 1C4=%04x 09=%02x 1D7=%02x pos=(%d,%d)\n",
            gf, i, c, f95, s6, u16(m, e + 0x8C), u8(m, e + 0x9F), c1c4, g9, d7,
            s32(m, e + 0x34), s32(m, e + 0x3c)))
        end
      end
    end
  end

  -- Status-Block alle 400 Spiel-Frames
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

-- ================= ABLAUF (GUI-Frames): Boot -> Menue -> Sprung -> Messen -> ggf. Re-Jump ====
function DrawImguiFrame()
  frames = frames + 1

  if out == nil then
    out = io.open(OUT, 'w')
    if out == nil then return end
    out:write("ROOM1030 Kriechzyklus — Hardware-Ground-Truth (gf = Spiel-Frame via BP 0x80030444)\n")
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
        out:write(string.format("\n*** IM ZIELRAUM ab Bild %d gf=%d — Versuch 1: poke flag(5,0x20) ***\n", frames, gf))
        out:flush()
      end
      if frames - seit > 2400 then
        out:write("QUADRAT wirkungslos beim Erst-Sprung\n"); out:flush()
        phase, seit, tstart = 3, frames, frames
      end

    elseif phase == 3 then
      -- Messung laeuft im BP. Hier nur: Eskalation + Ende.
      if not kette_lebt and frames - tstart > FENSTER and versuch < 3 then
        versuch = versuch + 1
        out:write(string.format("\n*** ESKALATION auf Versuch %d bei Bild %d gf=%d (Kette lebt nicht) ***\n",
          versuch, frames, gf))
        out:flush()
        phase, seit, rj_offen_prev = 4, frames, offen
      elseif not kette_lebt and frames - tstart > FENSTER * 2 and versuch >= 3 then
        out:write(string.format("\n*** AUCH VERSUCH 3 OHNE KETTE (Bild %d gf=%d) — messe weiter bis Budget ***\n",
          frames, gf))
        out:flush()
        tstart = frames   -- nicht nochmal melden
      end
      if vollzyklus and ende_bei < 0 then
        ende_bei = frames + 1800   -- noch ~700 gf fuer wartende Zombies mitnehmen
        out:write(string.format("*** Endphase: noch 1800 GUI-Bilder ab Bild %d ***\n", frames)); out:flush()
      end

    elseif phase == 4 then
      -- Re-Jump Schritt 1: Menue oeffnen
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
      -- Bestaetigt = Menue schliesst sich (sb zero @0x80014a58)
      if rj_offen_prev ~= 0 and offen == 0 then
        taste(BESTAETIGE, false)
        settle_bis = gf + 90       -- Reload abwarten, Tupel frisch aufbauen
        letzte, psample, fprev = {}, {}, nil
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

    -- Warteschlange in die Datei leeren (max 400 Zeilen pro GUI-Bild)
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
