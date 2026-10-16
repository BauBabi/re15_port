-- re2_baby.lua — RE2-Retail-Leon (re2leon.cue) in PCSX-Redux: Neues Spiel, Raumsprung per RAM nach ROOM2010
-- (zwei Adult-Spinnen am Boden, Sce_em_set @Datei 0x1104/0x111A), Spinne 0 per RAM in den TOD schicken
-- (+0x4 = 3, Zeile +0x5 = 1, +0x6 = +0x7 = 0 -> DEATH-Wurzel @0x80103C80, generische Zeile, Babys
-- FUN_80105D38(self,0x2002,(rand&3)+6) @0x80104590-A4) und dann JEDES Bild je Spinne/Baby mitschreiben:
-- Typ, Zustand, Lage x/y/z (+0x38/+0x3C/+0x40), Boden +0x1C2, Modus +0x222, +0x9, +0x106 und die WELT-Lage
-- von Part 0 (Part-Pool +0x198, Part 0 +0x5C t s32 x/y/z = wo das Modell gezeichnet wird).
-- Runde 38 Spur R (Nutzerpunkt 3 "Minispinnen fliegen weitestgehend in der Luft"). Muster:
-- analysis/befunde_runde35/H_raeume/re2_mess/re2_arm_grab.lua (Raumsprung = RE2-Tuer-Handler, dort belegt).
--
-- ENV: R2_OUT, R2_X/R2_Z/R2_YAW/R2_CUT (Leon-Startlage), R2_MAXF (Bilder nach dem Raumsprung),
--      R2_TOD (Bild nach dem Raumsprung, an dem Spinne 0 stirbt).

local ffi = ffi or require("ffi")
local OUT   = os.getenv("R2_OUT") or "C:/Users/mjoedicke/AppData/Local/Temp/re2baby"
local RX    = tonumber(os.getenv("R2_X")   or "-23000")
local RZ    = tonumber(os.getenv("R2_Z")   or "-26000")
local RYAW  = tonumber(os.getenv("R2_YAW") or "2358")
local RCUT  = tonumber(os.getenv("R2_CUT") or "4")
local MAXF  = tonumber(os.getenv("R2_MAXF") or "900")
local TOD   = tonumber(os.getenv("R2_TOD") or "60")

local PL        = 0x800CFBF8
local MODE      = 0x800DF348
local DOORPTR   = 0x800CE550
local FLAGS_DC  = 0x800CFBDC
local STAGE     = 0x800D481C
local ROOM      = 0x800D481E
local CUT       = 0x800CFBF2
local REC       = 0x80010B5C
local ETAB      = 0x800CFE18          -- 33 Entity-Zeiger (Leereintrag 0x800D424C)

local mem
local function u8(a)  return mem[bit.band(a, 0x1fffff)] end
local function u16(a) return u8(a) + u8(a + 1) * 256 end
local function u32(a) return u16(a) + u16(a + 2) * 65536 end
local function s16(a) local v = u16(a); if v >= 32768 then v = v - 65536 end return v end
local function s32(a) local v = u32(a); if v >= 2147483648 then v = v - 4294967296 end return v end
local function w8(a, v)  mem[bit.band(a, 0x1fffff)] = bit.band(v, 0xff) end
local function w16(a, v) w8(a, v); w8(a + 1, bit.rshift(bit.band(v, 0xffff), 8)) end
local function w32(a, v) w16(a, bit.band(v, 0xffff)); w16(a + 2, bit.rshift(v, 16)) end
local function ramptr(p) return p >= 0x80000000 and p < 0x80200000 end

local log
local frame = 0
local phase = "boot"
local t_phase = 0
local pad = PCSX.SIO0.slots[1].pads[1]
local B = PCSX.CONSTS.PAD.BUTTON
local held = {}
local function press(b) if not held[b] then pad.setOverride(b); held[b] = true end end
local function release_all() for b in pairs(held) do pad.clearOverride(b); held[b] = nil end end
local pl_stable, last_px, last_pz = 0, 0, 0
local room_at = -1
local tot_done = false

local function shot(tag)
  local ok, err = pcall(function()
    local ss = PCSX.GPU.takeScreenShot()
    local w, h, bpp = tonumber(ss.width), tonumber(ss.height), tonumber(ss.bpp)
    local data = tostring(ss.data)
    local f = io.open(string.format("%s/shot_%05d_%s.raw", OUT, frame - math.max(room_at, 0), tag), "wb")
    f:write(string.format("%d %d %d\n", w, h, bpp)); f:write(data); f:close()
  end)
  if not ok then log:write("# shot err " .. tostring(err) .. "\n") end
end

local function spinnen_zeile()
  local s = ""
  for k = 0, 32 do
    local p = u32(ETAB + 4 * k)
    if ramptr(p) and p ~= 0x800D424C and (u8(p + 8) == 0x25 or u8(p + 8) == 0x26) then
      local parts = u32(p + 0x198)
      local tx, ty, tz = 0, 0, 0
      if ramptr(parts) then tx, ty, tz = s32(parts + 0x5C), s32(parts + 0x60), s32(parts + 0x64) end
      s = s .. string.format(" [%d t=%02x w0=%08x st=%02x.%02x.%02x.%02x g9=%02x @(%d,%d,%d) yaw=%d y1c2=%d m=%d f106=%d f10e=%04x hp=%d P0(%d,%d,%d)]",
        k, u8(p + 8), u32(p), u8(p + 4), u8(p + 5), u8(p + 6), u8(p + 7), u8(p + 9),
        s32(p + 0x38), s32(p + 0x3C), s32(p + 0x40), s16(p + 0x76), s16(p + 0x1C2), u8(p + 0x222), u8(p + 0x106),
        u16(p + 0x10E), s16(p + 0x156), tx, ty, tz)
    end
  end
  return s
end

local function on_vsync()
  mem = PCSX.getMemPtr()
  frame = frame + 1
  if not log then
    log = assert(io.open(OUT .. "/log.txt", "w"))
    log:write(string.format("# re2_baby: X=%d Z=%d YAW=%d CUT=%d MAXF=%d TOD=%d\n", RX, RZ, RYAW, RCUT, MAXF, TOD))
    log:flush()
  end
  local px, pz = s32(PL + 0x38), s32(PL + 0x40)
  if px == last_px and pz == last_pz and (px ~= 0 or pz ~= 0) then pl_stable = pl_stable + 1 else pl_stable = 0 end
  last_px, last_pz = px, pz

  if phase == "boot" then
    local t = frame % 90
    if frame > 300 then
      if t < 4 then press(B.START) elseif t >= 45 and t < 49 then press(B.CROSS) else release_all() end
    end
    if pl_stable >= 30 then
      release_all(); phase = "ingame"; t_phase = frame
      log:write(string.format("# f%d IM SPIEL PL(%d,%d,%d) stage=%d room=%d\n", frame, px, s32(PL + 0x3C), pz,
        s16(STAGE), s16(ROOM))); log:flush()
    end
  elseif phase == "ingame" then
    release_all()
    if frame == t_phase + 150 then
      w16(REC + 0, RX); w16(REC + 2, 0); w16(REC + 4, RZ); w16(REC + 6, RYAW)
      w8(REC + 8, 1); w8(REC + 9, 1); w8(REC + 10, RCUT); w8(REC + 11, 0)
      w32(DOORPTR, REC)
      w32(FLAGS_DC, bit.bor(u32(FLAGS_DC), 0xFF000000))
      w8(MODE, 1)
      phase = "jump"; t_phase = frame
      log:write(string.format("# f%d SPRUNG ROOM2010 geschrieben\n", frame)); log:flush()
    end
  elseif phase == "jump" then
    if s16(STAGE) == 1 and s16(ROOM) == 1 and px == RX and pz == RZ then
      phase = "room"; room_at = frame
      log:write(string.format("# f%d ROOM2010 geladen PL(%d,%d,%d) cut=%d\n", frame, px, s32(PL + 0x3C), pz, u8(CUT)))
      log:flush(); shot("raum")
    elseif frame > t_phase + 1800 then
      log:write(string.format("# f%d SPRUNG fehlgeschlagen stage=%d room=%d\n", frame, s16(STAGE), s16(ROOM)))
      log:flush(); phase = "done"
    end
  elseif phase == "room" then
    local n = frame - room_at
    if n == TOD and not tot_done then
      for k = 0, 32 do
        local p = u32(ETAB + 4 * k)
        if ramptr(p) and p ~= 0x800D424C and u8(p + 8) == 0x25 then
          log:write(string.format("# f%d TOD Spinne Satz %d @%08x vorher st=%02x.%02x.%02x.%02x\n", n, k, p,
            u8(p + 4), u8(p + 5), u8(p + 6), u8(p + 7)))
          w8(p + 4, 3); w8(p + 5, 1); w8(p + 6, 0); w8(p + 7, 0)
          tot_done = true
          break
        end
      end
      log:flush()
    end
    log:write(string.format("f%d PL(%d,%d,%d) hp=%d cut=%d%s\n", n, px, s32(PL + 0x3C), pz, s16(PL + 0x156),
      u8(CUT), spinnen_zeile()))
    if n % 15 == 0 then shot("r") end
    if n >= MAXF then phase = "done" end
  end
  if phase == "done" then
    log:write(string.format("# fertig f%d\n", frame))
    log:close()
    PCSX.quit(0)
  end
end

_G.__r2_listener = PCSX.Events.createEventListener("GPU::Vsync", function()
  local ok, err = pcall(on_vsync)
  if not ok then
    if log then log:write("# FEHLER " .. tostring(err) .. "\n"); log:flush() end
  end
end)
