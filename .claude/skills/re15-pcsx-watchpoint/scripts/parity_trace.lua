-- parity_trace.lua — FRAME-EXACT input + PER-FRAME state dump from the ORIGINAL, for 1:1 parity.
--
-- WHY PCSX-Redux instead of DuckStation: the DuckStation route needs a savestate to read state, and
-- getting one turned out to be the whole problem — SaveStateOnExit stopped firing, the save hotkey
-- never fired from the virtual pad (measured on trigger axes AND shoulder buttons, running AND
-- paused), and the driver silently copied a STALE resume file, which produced three parity
-- "findings" that were artefacts. Here none of that exists: Lua drives the pad deterministically
-- (setOverride/clearOverride) and reads RAM live, so there is no savestate in the loop at all — and
-- instead of one end-state we get EVERY frame, which is what "1:1" actually needs.
--
-- LuaJIT (Lua 5.1): NO 5.3 bit operators. bit.band / bit.rshift / bit.bor only.
--
-- ENV:
--   RE15_PT_LOG    output path (one line per frame)
--   RE15_PT_SCRIPT frame-exact input, same token language as the port's RE15_INPUT_SCRIPT but the
--                  counts are FRAMES: "W90,R24,U180". U/D/L/R = d-pad, X = cross, A = square,
--                  M = R1, W = wait.
--   RE15_PT_START  frames to wait after the room is live before the script starts (default 90)

local LOGP   = os.getenv("RE15_PT_LOG")    or "C:/workspace/git/reAi_v2/shots/pcsx_parity.log"
local SCRIPT = os.getenv("RE15_PT_SCRIPT") or "W90"
local START  = tonumber(os.getenv("RE15_PT_START") or "90")

local PLAYER_BASE = 0x800ACA54
local PLAYER_YAW  = 0x800ACABE            -- player base + 0x6a = rot_y (4096 = 360). NOT 0x800ACA74:
                                          -- that is m[0][0] of the entity MATRIX at +0x20, i.e.
                                          -- cos(yaw). Reading it as an angle produced a phantom
                                          -- "spawn yaw -45 vs -96" divergence that does not exist.
local PLAYER_POS  = 0x800ACA88            -- = MATRIX.t[0..2] (x at +0, z at +8)
local ENEMY_BASE, ENEMY_STRIDE = 0x800ACC2C, 0x1F4
local ACTIVE_CNT  = 0x800ACA4E

local BTN = { U = 4, R = 5, D = 6, L = 7, X = 14, A = 15, M = 11 }  -- A = SQUARE(15), M = R1(11)

local log = assert(io.open(LOGP, "w"))
-- LINE buffering, immediately. The first run came back with a ZERO-BYTE log: the header had been
-- written but never flushed, and the emulator is force-killed at the end, so the buffer was lost.
-- A capture whose evidence dies with the process is useless — so nothing here is buffered.
-- NO setvbuf: PCSX-Redux's sandboxed io may not expose it, and a missing method kills the whole
-- chunk right after io.open — which is exactly the failure mode seen (log file created, 0 bytes).
-- watch_addr.lua, the one Lua script proven to work here, does not use it either. Flush explicitly.
log:write("# parity_trace loaded\n"); log:flush()
local mem = PCSX.getMemPtr()

local function u8(a)  return mem[bit.band(a, 0x1fffff)] end
local function u16(a) return u8(a) + u8(a + 1) * 256 end
local function u32(a) return u16(a) + u16(a + 2) * 65536 end
local function s16(a) local v = u16(a); if v >= 32768 then v = v - 65536 end return v end
local function s32(a) local v = u32(a); if v >= 2147483648 then v = v - 4294967296 end return v end

-- parse "W90,R24" -> { {letters, frames}, ... }
local plan, total = {}, 0
for tok in string.gmatch(SCRIPT, "[^,]+") do
  local letters, n = string.match(tok, "^(%a+)(%d*)$")
  if letters then
    n = tonumber(n) or 1
    table.insert(plan, { letters = string.upper(letters), frames = n })
    total = total + n
  end
end
log:write(string.format("# script=%s total_frames=%d start=%d\n", SCRIPT, total, START))

local pad = PCSX.SIO0.slots[1].pads[1]
local frame, seg, seg_left, live = 0, 0, 0, false
local held = {}

local function set_held(letters)
  local want = {}
  for c in string.gmatch(letters, ".") do if BTN[c] then want[BTN[c]] = true end end
  for b in pairs(held) do if not want[b] then pad.clearOverride(b); held[b] = nil end end
  for b in pairs(want) do if not held[b] then pad.setOverride(b); held[b] = true end end
end

-- The room is "live" once the enemy roster is populated: this is the ONLY honest ready-signal, and
-- checking it is exactly what a previous attempt lacked — it captured an empty room and the result
-- read like a broken port.
local function room_live() return u8(ACTIVE_CNT) > 0 end

-- The event name is the one thing here I cannot verify statically, so it is probed rather than
-- assumed: if the registration fails, the LOG says so (stdout from the emulator does not reach the
-- driver's captured stream, as the first run showed).
local ok_evt, err_evt = pcall(function()
PCSX.Events.createEventListener("GPU::Vsync", function()
  -- Log from the FIRST vsync, never gated. The previous version only started once the enemy roster
  -- was populated and produced a 0-byte log — which is indistinguishable from "the Lua is broken".
  -- A trace that stays silent when its precondition is unmet cannot tell you WHY, so the readiness
  -- is now DATA in the log (live=, act=) instead of a gate around it.
  frame = frame + 1
  if not live and room_live() then live = true end

  if frame > START then
    if seg_left <= 0 then
      seg = seg + 1
      if plan[seg] then
        seg_left = plan[seg].frames
        set_held(plan[seg].letters)
        log:write(string.format("# f%d segment %s x%d\n", frame, plan[seg].letters, seg_left))
      elseif seg == #plan + 1 then
        set_held("")
        log:write(string.format("# f%d script done\n", frame))
        log:flush()
      end
    end
    if seg_left > 0 then seg_left = seg_left - 1 end
  end

  local line = string.format("F%d live=%d act=%d PL(%d,%d,rot=%d,hp=%d)",
      frame, live and 1 or 0, u8(ACTIVE_CNT),
      s32(PLAYER_POS), s32(PLAYER_POS + 8), s16(PLAYER_YAW), s16(PLAYER_BASE + 0x1ba))
  for i = 0, 7 do
    local b = ENEMY_BASE + i * ENEMY_STRIDE
    if bit.band(u32(b), 1) ~= 0 then
      line = line .. string.format(" [%d t=%02x st=%d ss1=%d ss2=%d ss3=%d g=%02x mo=%d af=%d d=%d @(%d,%d,r%d)]",
        i, u8(b + 8), u8(b + 4), u8(b + 5), u8(b + 6), u8(b + 7), u8(b + 9),
        u8(b + 0x94), u8(b + 0x95), u32(b + 0x1d0),
        s32(b + 0x34), s32(b + 0x3c), s16(b + 0x6a))
    end
  end
  log:write(line .. "\n"); log:flush()
end)
end)
log:write(string.format("# createEventListener ok=%s err=%s\n",
                       tostring(ok_evt), tostring(err_evt)))
if not ok_evt then
  -- Try the alternative spellings before giving up, and record which one took.
  for _, name in ipairs({ "GPU::VSync", "Vsync", "Frame", "GPU::Frame" }) do
    local ok2 = pcall(function() PCSX.Events.createEventListener(name, function() end) end)
    log:write(string.format("# probe '%s' ok=%s\n", name, tostring(ok2)))
  end
end
_G.__parity_keep = true
print("[parity_trace] armed: " .. SCRIPT .. " -> " .. LOGP)
