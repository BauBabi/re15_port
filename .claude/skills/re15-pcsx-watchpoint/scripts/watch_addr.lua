-- re15-pcsx-watchpoint : trace every WRITE to a PSX RAM address, log PC + value + cause.
-- Catches CPU stores AND indirect/DMA writes (address-based breakpoint) — the thing a static
-- disasm scan can miss (e.g. a store through a base register the disassembler didn't resolve).
--
-- PCSX-Redux runs LuaJIT (Lua 5.1): NO 5.3 bit operators (& | >> <<) — use the `bit` library.
-- Config via env: PCSX_WATCH_ADDR (hex, e.g. 0x800aca3c), PCSX_WATCH_LOG (path),
--                 PCSX_WATCH_BITMASK (hex, optional — flag "<== SETS" when old&mask==0 and new&mask!=0).
local band, brshift = bit.band, bit.rshift

local ADDR = tonumber(os.getenv("PCSX_WATCH_ADDR") or "0x800aca3c")
local LOGP = os.getenv("PCSX_WATCH_LOG") or [[C:\workspace\git\reAi_v2\shots\pcsx_watch.log]]
local MASK = tonumber(os.getenv("PCSX_WATCH_BITMASK") or "0")

local log = io.open(LOGP, "w")
log:write(string.format("== watch 0x%08x (mask 0x%x) ==\n", ADDR, MASK)); log:flush()

local mem = PCSX.getMemPtr()                       -- uint8_t* to 2MB RAM
local function r32(a)
  local o = band(a, 0x1fffff)
  return mem[o] + mem[o+1]*256 + mem[o+2]*65536 + mem[o+3]*16777216
end

local count, found = 0, false
_G.__watch_bp = PCSX.addBreakpoint(ADDR, 'Write', 4, 'watch', function(address, width, cause)
  local ok, err = pcall(function()
    local regs = PCSX.getRegisters()
    local pc    = regs.pc
    local instr = r32(pc)                          -- probable store instruction word
    local rt    = band(brshift(instr, 16), 0x1f)   -- store source reg (sw/sh/sb: rt)
    local val   = regs.GPR.r[rt] or 0              -- value being written (sw)
    local old   = r32(ADDR)
    count = count + 1
    local sets = (MASK ~= 0) and (band(old, MASK) == 0) and (band(val, MASK) ~= 0)
    log:write(string.format("#%d pc=%08x instr=%08x rt=%d old=%08x newval=%08x cause=%s%s\n",
        count, pc, instr, rt, old, val, tostring(cause), sets and "  <== SETS mask!" or ""))
    log:flush()
    if sets and not found then
      found = true
      log:write(string.format(">>> SETTER: pc=%08x instr=%08x <<<\n", pc, instr)); log:flush()
    end
    if count >= 800 then _G.__watch_bp = nil end   -- allow GC -> removes the breakpoint
  end)
  if not ok then log:write("CB ERROR: "..tostring(err).."\n"); log:flush() end
end)
print("watch breakpoint installed @ 0x"..string.format("%08x", ADDR))
