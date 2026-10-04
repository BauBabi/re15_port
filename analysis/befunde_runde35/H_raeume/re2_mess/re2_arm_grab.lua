-- re2_arm_grab.lua — RE2-Retail-Leon (SLUS-00421, re2leon.cue) in PCSX-Redux: Neues Spiel, Raumsprung
-- per RAM nach ROOM2050 (Gitterarme EM2D), Leon an eine Stelle neben die Arme, Griff abwarten und
-- JEDES Bild RAM (PL, Leons Parts, haltender Arm, dessen Parts) + Bildschirm mitschreiben.
-- Runde 35 Spur H, Nachbesserung 3. LuaJIT (5.1): nur bit.band/bit.bor/bit.rshift.
--
-- Raumsprung = genau das, was der RE2-Tuer-AOT-Handler tut (info/re2leon/PSX.EXE, selbst disassembliert):
--   800516cc addiu v1,zero,1 / 800516d4 sb v1,-3256(at)   -> 0x800DF348 = 1 (Hauptschleifen-Modus Tuer)
--   800516e8 sw s0,-6832(at)                               -> 0x800CE550 = Zeiger auf den Tuer-Satz
--   800516e0..f4                                           -> 0x800CFBDC |= 0xFF000000
-- FUN_80026b7c (einziger Aufrufer @0x80025a70) liest den Satz: s16 x/y/z/yaw an +0/+2/+4/+6,
-- Stage = byte +8 % 9, Raum = byte +9, Cut = byte +10, Ebene = byte +11.
-- Der Satz liegt auf dem unreferenzierten Debug-String "d:/bio2/room/data/tetubox1.tim" @0x80010B5C
-- (ghidra_re2_Leon.txt:73647, keine XREF).
--
-- ENV: R2_OUT (Ordner), R2_X/R2_Z/R2_YAW (Leon-Startlage), R2_CUT, R2_MAXF (Bilder nach dem Raumsprung),
--      R2_SHOTS (1 = Bildschirm waehrend des Haltens), R2_STATE_SAVE (Pfad: Spielstand nach "im Spiel").

local ffi = ffi or require("ffi")
local OUT   = os.getenv("R2_OUT") or "C:/Users/mjoedicke/AppData/Local/Temp/re2run"
local RX    = tonumber(os.getenv("R2_X")   or "-26384")
local RZ    = tonumber(os.getenv("R2_Z")   or "-12350")
local RYAW  = tonumber(os.getenv("R2_YAW") or "2048")
local RCUT  = tonumber(os.getenv("R2_CUT") or "9")
local MAXF  = tonumber(os.getenv("R2_MAXF") or "2400")
local SHOTS = (os.getenv("R2_SHOTS") or "1") == "1"
local SAVEP = os.getenv("R2_STATE_SAVE")

local PL        = 0x800CFBF8
local HOLDER    = 0x800CFDAC          -- PL+0x1B4 (B4 P0 @0x80100c30 sw s0,-596(at))
local MODE      = 0x800DF348
local DOORPTR   = 0x800CE550
local FLAGS_DC  = 0x800CFBDC
local STAGE     = 0x800D481C
local ROOM      = 0x800D481E
local CUT       = 0x800CFBF2
local REC       = 0x80010B5C
local ETAB      = 0x800CFE18          -- 33 Entity-Zeiger (Raumlader FUN_80049e48: Leereintrag 0x800D424C)
local PART_N, PART_SZ, ENT_SZ = 16, 0xAC, 0x248

local mem
local function u8(a)  return mem[bit.band(a, 0x1fffff)] end
local function u16(a) return u8(a) + u8(a + 1) * 256 end
local function u32(a) return u16(a) + u16(a + 2) * 65536 end
local function s16(a) local v = u16(a); if v >= 32768 then v = v - 65536 end return v end
local function s32(a) local v = u32(a); if v >= 2147483648 then v = v - 4294967296 end return v end
local function w8(a, v)  mem[bit.band(a, 0x1fffff)] = bit.band(v, 0xff) end
local function w16(a, v) w8(a, v); w8(a + 1, bit.rshift(bit.band(v, 0xffff), 8)) end
local function w32(a, v) w16(a, bit.band(v, 0xffff)); w16(a + 2, bit.rshift(v, 16)) end
local function blob(a, n) return ffi.string(mem + bit.band(a, 0x1fffff), n) end
local function ramptr(p) return p >= 0x80000000 and p < 0x80200000 end

local log, bin
local frame = 0
local phase = "boot"       -- boot -> ingame -> jump -> room -> done
local t_phase = 0
local pad = PCSX.SIO0.slots[1].pads[1]
local B = PCSX.CONSTS.PAD.BUTTON
local held = {}
local function press(b) if not held[b] then pad.setOverride(b); held[b] = true end end
local function release_all() for b in pairs(held) do pad.clearOverride(b); held[b] = nil end end
local pl_stable, last_px, last_pz = 0, 0, 0
local room_at, grabs, held_frames, last_r5 = -1, 0, 0, false
local shotn = 0

local function shot(tag)
  local ok, err = pcall(function()
    local ss = PCSX.GPU.takeScreenShot()
    local w, h, bpp = tonumber(ss.width), tonumber(ss.height), tonumber(ss.bpp)
    local data = tostring(ss.data)
    local f = io.open(string.format("%s/shot_%05d_%s.raw", OUT, frame, tag), "wb")
    f:write(string.format("%d %d %d\n", w, h, bpp)); f:write(data); f:close()
    shotn = shotn + 1
  end)
  if not ok then log:write("# shot err " .. tostring(err) .. "\n") end
end

local function arms_line()
  local s = ""
  for k = 0, 32 do
    local p = u32(ETAB + 4 * k)
    if ramptr(p) and p ~= 0x800D424C and u8(p + 8) == 0x2D then
      s = s .. string.format(" [%d %08x w4=%08x x=%d y=%d z=%d yaw=%d cw=%08x f10e=%04x w0=%08x]", k, p,
        u32(p + 4), s32(p + 0x38), s32(p + 0x3C), s32(p + 0x40), s16(p + 0x76), u32(p + 0x14C), u16(p + 0x10E), u32(p))
    end
  end
  return s
end

local function dump_bin()
  -- Satz: 'R2F1' frame | PL 0x248 | PL-Parts 16*0xAC | Halter-Zeiger | Halter 0x248 | Halter-Parts 16*0xAC
  local parts = u32(PL + 0x198)
  local h = u32(HOLDER)
  bin:write("R2F1", ffi.string(ffi.new("uint32_t[1]", frame), 4))
  bin:write(blob(PL, ENT_SZ))
  if ramptr(parts) then bin:write(blob(parts, PART_N * PART_SZ)) else bin:write(string.rep("\0", PART_N * PART_SZ)) end
  bin:write(ffi.string(ffi.new("uint32_t[1]", h), 4))
  if ramptr(h) then
    bin:write(blob(h, ENT_SZ))
    local hp = u32(h + 0x198)
    if ramptr(hp) then bin:write(blob(hp, PART_N * PART_SZ)) else bin:write(string.rep("\0", PART_N * PART_SZ)) end
  else
    bin:write(string.rep("\0", ENT_SZ + PART_N * PART_SZ))
  end
end

local function on_vsync()
  mem = PCSX.getMemPtr()
  frame = frame + 1
  if not log then
    log = assert(io.open(OUT .. "/log.txt", "w"))
    log:write(string.format("# re2_arm_grab: X=%d Z=%d YAW=%d CUT=%d MAXF=%d\n", RX, RZ, RYAW, RCUT, MAXF))
    log:flush()
  end
  local px, pz = s32(PL + 0x38), s32(PL + 0x40)
  if px == last_px and pz == last_pz and (px ~= 0 or pz ~= 0) then pl_stable = pl_stable + 1 else pl_stable = 0 end
  last_px, last_pz = px, pz

  if phase == "boot" then
    if frame == 60 then
      -- EXE-Abgleich: der Tuerwechsel FUN_80026b7c muss im RAM die Bytes der Datei tragen
      log:write(string.format("# f%d RAM@80026b7c = %08x %08x (Datei: 27bdffe0 ...)\n", frame,
        u32(0x80026b7c), u32(0x80026b80))); log:flush()
    end
    -- Titel/Menue/Filme: START und KREUZ im Wechsel, bis Leon steht
    local t = frame % 90
    if frame > 300 then
      if t < 4 then press(B.START) elseif t >= 45 and t < 49 then press(B.CROSS) else release_all() end
    end
    if frame % 600 == 0 then
      log:write(string.format("# f%d boot PL(%d,%d) st=%d room=%d mode=%d\n", frame, px, pz, s16(STAGE), s16(ROOM), u8(MODE)))
      log:flush(); shot("boot")
    end
    if pl_stable >= 30 then
      release_all(); phase = "ingame"; t_phase = frame
      log:write(string.format("# f%d IM SPIEL PL(%d,%d,%d) yaw=%d stage=%d room=%d mode=%d\n", frame,
        px, s32(PL + 0x3C), pz, s16(PL + 0x76), s16(STAGE), s16(ROOM), u8(MODE))); log:flush()
      shot("ingame")
    end
  elseif phase == "ingame" then
    release_all()
    if frame == t_phase + 150 then
      if SAVEP then
        local ok, err = pcall(function()
          local st = PCSX.createSaveState()
          local f = io.open(SAVEP, "wb"); f:write(tostring(st)); f:close()
        end)
        log:write(string.format("# f%d Spielstand -> %s ok=%s %s\n", frame, SAVEP, tostring(ok), tostring(err)))
      end
      shot("vorsprung")
      if u8(MODE) ~= 0 then
        log:write(string.format("# f%d WARNUNG mode=%d vor dem Sprung\n", frame, u8(MODE)))
      end
      -- Tuer-Satz schreiben und den Handler nachbilden
      w16(REC + 0, RX); w16(REC + 2, 0); w16(REC + 4, RZ); w16(REC + 6, RYAW)
      w8(REC + 8, 1); w8(REC + 9, 5); w8(REC + 10, RCUT); w8(REC + 11, 0)
      w32(DOORPTR, REC)
      w32(FLAGS_DC, bit.bor(u32(FLAGS_DC), 0xFF000000))
      w8(MODE, 1)
      phase = "jump"; t_phase = frame
      log:write(string.format("# f%d SPRUNG geschrieben: Satz %s\n", frame,
        string.format("%02x", u8(REC)) .. "..")); log:flush()
    end
  elseif phase == "jump" then
    if s16(STAGE) == 1 and s16(ROOM) == 5 and px == RX and pz == RZ then
      phase = "room"; room_at = frame
      bin = assert(io.open(OUT .. "/frames.bin", "wb"))
      log:write(string.format("# f%d ROOM2050 geladen PL(%d,%d,%d) yaw=%d cut=%d\n", frame, px,
        s32(PL + 0x3C), pz, s16(PL + 0x76), u8(CUT))); log:flush()
      shot("raum")
    elseif frame > t_phase + 1800 then
      log:write(string.format("# f%d SPRUNG fehlgeschlagen stage=%d room=%d PL(%d,%d) mode=%d\n", frame,
        s16(STAGE), s16(ROOM), px, pz, u8(MODE))); log:flush()
      shot("fehl"); phase = "done"
    end
  elseif phase == "room" then
    local r = u8(PL + 4)
    local h = u32(HOLDER)
    log:write(string.format("f%d PL(%d,%d,%d) yaw=%d w4=%08x cw=%08x cut=%d H=%08x%s\n", frame - room_at,
      px, s32(PL + 0x3C), pz, s16(PL + 0x76), u32(PL + 4), u32(PL + 0x14C), u8(CUT), h, arms_line()))
    dump_bin()
    if r == 5 then
      held_frames = held_frames + 1
      if not last_r5 then grabs = grabs + 1; log:write(string.format("# GRIFF %d ab f%d\n", grabs, frame - room_at)) end
      if SHOTS then shot(string.format("g%d", grabs)) end
    elseif (frame - room_at) % 60 == 0 then
      shot("r")
    end
    last_r5 = (r == 5)
    if frame - room_at >= MAXF then phase = "done" end
  end
  if phase == "done" then
    log:write(string.format("# fertig f%d griffe=%d haltebilder=%d shots=%d\n", frame, grabs, held_frames, shotn))
    log:close(); if bin then bin:close() end
    PCSX.quit(0)
  end
end

_G.__r2_listener = PCSX.Events.createEventListener("GPU::Vsync", function()
  local ok, err = pcall(on_vsync)
  if not ok then
    if log then log:write("# FEHLER " .. tostring(err) .. "\n"); log:flush() end
  end
end)
