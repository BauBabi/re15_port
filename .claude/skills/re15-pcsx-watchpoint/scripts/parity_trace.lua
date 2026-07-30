-- parity_trace.lua — FRAME-EXACT input + PER-FRAME state dump from the ORIGINAL, for 1:1 parity.
--
-- WHY PCSX-Redux: the DuckStation route needs a savestate to read state, and obtaining one was the
-- whole problem (SaveStateOnExit stopped firing, no save hotkey ever fired from the virtual pad, and
-- the driver silently copied a STALE resume file — three "parity findings" turned out to be
-- artefacts of that). Here Lua drives the pad deterministically and reads RAM live: no savestate in
-- the loop, and EVERY frame is captured instead of one end state.
--
-- EVERYTHING runs off the vsync counter — boot wait, menu navigation, the debug JUMP and the input
-- script. No wall-clock anywhere, so a 24-frame token really is 24 frames (proven: the log prints
-- "segment R x24" and the next segment starts exactly 24 frames later). Wall-clock holds were what
-- made the DuckStation chain unusable: a nominal 60-frame hold delivered ~3.4 frames of effect.
--
-- LuaJIT (Lua 5.1): NO 5.3 bit operators — bit.band / bit.rshift only.
--
-- ENV
--   RE15_PT_LOG     output path, one line per frame
--   RE15_PT_SCRIPT  frame-exact input, same tokens as the port's RE15_INPUT_SCRIPT but counted in
--                   FRAMES: "W90,R24,U180".  U D L R = d-pad, X = cross, A = square, M = R1, W = wait
--   RE15_PT_LEFT    debug-JUMP steps LEFT from the default room (ROOM1140 = 16, hex 0x114)
--   RE15_PT_START   idle frames after the room is live before the script starts (default 90)

local LOGP   = os.getenv("RE15_PT_LOG")    or "C:/workspace/git/reAi_v2/shots/pcsx_parity.log"
local SCRIPT = os.getenv("RE15_PT_SCRIPT") or "W90"
local START  = tonumber(os.getenv("RE15_PT_START") or "90")
local JLEFT  = tonumber(os.getenv("RE15_PT_LEFT")  or "16")

local PLAYER_BASE = 0x800ACA54
local PLAYER_YAW  = 0x800ACABE   -- base + 0x6a = rot_y (4096 = 360). NOT 0x800ACA74: that is
                                 -- m[0][0] of the entity MATRIX at +0x20, i.e. cos(yaw). Reading it
                                 -- as an angle produced a phantom "spawn yaw -45 vs -96" divergence.
local PLAYER_POS  = 0x800ACA88   -- = MATRIX.t[0..2]  (x at +0, z at +8)
local ENEMY_BASE, ENEMY_STRIDE = 0x800ACC2C, 0x1F4
local ACTIVE_CNT  = 0x800ACA4E

local PAD = { U = 4, R = 5, D = 6, L = 7, X = 14, A = 15, M = 11,
              SELECT = 0, START = 3, TRI = 12, CIRCLE = 13 }

local log = assert(io.open(LOGP, "w"))
log:write("# parity_trace loaded\n"); log:flush()

-- Fetch the RAM pointer FRESH each frame, never once at load time. Caching it was why the trace
-- died at frame ~360 every single run: the chunk runs before the disc boots, and the pointer taken
-- then goes stale when the emulator (re)initialises memory — the callback throws, PCSX drops the
-- listener, and the log simply stops. It looked exactly like "the emulator freezes", which is what
-- I wrongly reported. watch_addr.lua, the script proven to work here, also reads it per call.
local mem
local function u8(a)  return mem[bit.band(a, 0x1fffff)] end
local function u16(a) return u8(a) + u8(a + 1) * 256 end
local function u32(a) return u16(a) + u16(a + 2) * 65536 end
local function s16(a) local v = u16(a); if v >= 32768 then v = v - 65536 end return v end
local function s32(a) local v = u32(a); if v >= 2147483648 then v = v - 4294967296 end return v end

local plan, total = {}, 0
for tok in string.gmatch(SCRIPT, "[^,]+") do
  local letters, n = string.match(tok, "^(%a+)(%d*)$")
  if letters then
    n = tonumber(n) or 1
    table.insert(plan, { letters = string.upper(letters), frames = n })
    total = total + n
  end
end
log:write(string.format("# script=%s frames=%d start=%d jumpleft=%d\n",
                        SCRIPT, total, START, JLEFT)); log:flush()

local pad = PCSX.SIO0.slots[1].pads[1]
local held = {}
local function press(b) if not held[b] then pad.setOverride(b); held[b] = true end end
local function release_all() for b in pairs(held) do pad.clearOverride(b); held[b] = nil end end
local function set_held(letters)
  local want = {}
  for c in string.gmatch(letters, ".") do if PAD[c] then want[PAD[c]] = true end end
  for b in pairs(held) do if not want[b] then pad.clearOverride(b); held[b] = nil end end
  for b in pairs(want) do press(b) end
end

-- A tap is expressed in FRAMES, not seconds: hold for `on` frames, then stay off for `off`. The
-- DuckStation driver needed 0.30-0.60s gaps and was still unreliable; here the menu sees an exact
-- number of frames, which is why short taps suffice and cannot auto-repeat by accident.
local queue = {}   -- list of {btn=<or nil>, frames=n}
local function q_tap(btn, on, off) table.insert(queue, {btn=btn, frames=on or 3})
                                   table.insert(queue, {btn=nil,  frames=off or 12}) end
local function q_wait(n)           table.insert(queue, {btn=nil,  frames=n}) end

-- BOOT -> NEW GAME -> (ROOM1240) -> DEBUG JUMP -> ROOM1140.
-- Reaching a room via the debug menu is what the DuckStation capture skill does; going through the
-- intro instead is what made the previous run end with live=0 (it never got to gameplay).
q_wait(60 * 130)                                   -- boot to title (interpreter is slow; be generous)
for _ = 1, 3 do q_tap(PAD.U, 3, 20) end            -- title cursor up to NEW GAME
q_tap(PAD.X, 3, 60 * 12)                           -- NEW GAME
q_tap(PAD.X, 3, 60 * 4)                            -- character confirm (Leon)
q_tap(PAD.X, 3, 60 * 20)                           -- into the room
for _ = 1, 8 do q_tap(PAD.X, 3, 60 * 2) end        -- skip intro dialog/movies
q_wait(60 * 5)
q_tap(PAD.SELECT, 4, 90)                           -- DEBUG MENU
q_tap(PAD.D, 3, 60)                                -- -> JUMP line
for _ = 1, JLEFT do q_tap(PAD.L, 3, 24) end        -- step rooms (JUMP numbers are HEX; 16 = 0x114)
q_tap(PAD.A, 4, 60 * 12)                           -- Square = LOAD room

local qi, qleft = 0, 0
local frame, live, live_at = 0, false, -1
local seg, seg_left, script_done = 0, 0, false

-- The callback body runs inside pcall and any error is written to the LOG. Without this an error
-- silently kills the listener: the emulator keeps running perfectly (user-confirmed) while the trace
-- just stops, which looks exactly like an emulator freeze. I misdiagnosed that three times — as a
-- stale savestate, as a stalled emulator, and as parallel instances. A harness must report its own
-- death.
local function on_vsync()
  mem = PCSX.getMemPtr()          -- fresh every frame, see the note above
  frame = frame + 1
  if not live and u8(ACTIVE_CNT) > 0 then
    live = true; live_at = frame
    release_all()
    log:write(string.format("# f%d ROOM LIVE (act=%d)\n", frame, u8(ACTIVE_CNT))); log:flush()
  end

  if not live then
    -- navigation phase: consume the queue, one frame per entry-frame
    if qleft <= 0 then
      qi = qi + 1
      if queue[qi] then
        qleft = queue[qi].frames
        release_all()
        if queue[qi].btn then press(queue[qi].btn) end
      end
    end
    if qleft > 0 then qleft = qleft - 1 end
  else
    -- script phase: starts START frames after the room went live
    local f = frame - live_at
    if f > START and not script_done then
      if seg_left <= 0 then
        seg = seg + 1
        if plan[seg] then
          seg_left = plan[seg].frames
          set_held(plan[seg].letters)
          log:write(string.format("# f%d segment %s x%d\n", frame, plan[seg].letters, seg_left))
          log:flush()
        else
          set_held(""); script_done = true
          log:write(string.format("# f%d script done\n", frame)); log:flush()
        end
      end
      if seg_left > 0 then seg_left = seg_left - 1 end
    end
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
  -- During the BOOT/NAVIGATION phase nothing here is evidence yet, and writing plus flushing every
  -- single vsync throttled the run badly (~8 vsync/s, so the 7800-frame boot wait would take the
  -- better part of an hour). Sample it instead; from ROOM LIVE onward every frame is written and
  -- flushed, because that is the part that has to survive the emulator being killed.
  if live then
    log:write(line .. "\n"); log:flush()
  elseif frame % 120 == 0 then
    log:write(line .. "\n"); log:flush()
  end
end

-- KEEP THE LISTENER OBJECT ALIVE. This is the documented pitfall of this skill ("_G.__keep = bp —
-- MUSS am Leben gehalten werden, GC entfernt sonst den Breakpoint") and I walked straight into it:
-- I stored `true` in the global instead of the listener, so the first GC cycle collected it. The
-- trace then stopped at frame ~360 while the emulator kept running perfectly — which is exactly the
-- "the emulator freezes" I wrongly reported.
local err_seen = false
_G.__parity_listener = PCSX.Events.createEventListener("GPU::Vsync", function()
  local ok, err = pcall(on_vsync)
  if not ok and not err_seen then
    err_seen = true
    log:write(string.format("# CALLBACK ERROR at frame %d: %s\n", frame, tostring(err)))
    log:flush()
  end
end)

_G.__parity_keep = true
print("[parity_trace] armed")
