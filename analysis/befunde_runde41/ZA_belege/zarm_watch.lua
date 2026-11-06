-- ZA zombiearm: wer loescht die Spawn-Varianten-Bits 0x400|0x80 im Teil-Wort?
-- Sprung ROOM1140 (Stage 0, Index 0x14) ueber das Debug-Menue (Ablauf aus crawl_1030.lua).
-- Exec-Haltepunkte: INIT 0x80100688, Varianten 0x80101008/58/a8/13c/18c, Teil-Aufbau 0x8001e5b0,
-- Teil-Reset 0x8001ee68. Bei jeder Variante: Schreib-Haltepunkte auf die betroffenen Teil-Woerter.
-- LuaJIT: bit.band etc.; Rueckrufe geben IMMER true zurueck (false loescht den Haltepunkt).

local OUT = [[C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\za\zarm_out.txt]]
local frames, out, phase, wartet = 0, nil, 0, 0
local menuehits, spielerAb, menueAb = 0, -1, -1
local zielgesetzt = 0
local bestaetige = false
_G.__keep = {}
local beobachtet = {}

local function u8(m, a) return m[bit.band(a, 0x1fffff)] end
local function u32(m, a)
  local o = bit.band(a, 0x1fffff)
  return m[o] + m[o+1]*256 + m[o+2]*65536 + m[o+3]*16777216
end
local function u16(m, a) local o = bit.band(a, 0x1fffff); return m[o] + m[o+1]*256 end
local function s32(m, a)
  local v = u32(m, a); if v >= 0x80000000 then v = v - 0x100000000 end; return v
end
local function w8(m, a, v) m[bit.band(a, 0x1fffff)] = v end
local function w16(m, a, v)
  local o = bit.band(a, 0x1fffff)
  m[o] = bit.band(v, 0xff); m[o+1] = bit.band(bit.rshift(v, 8), 0xff)
end
local function log(s) if out then out:write(s .. "\n"); out:flush() end end

local VARIANTE = {
  [0x80101058] = { 12 }, [0x80101008] = { 2 }, [0x801010a8] = { 8, 9 },
  [0x8010113c] = { 5 },  [0x8010118c] = { 4, 5 },
}

local function wache_wort(addr, tag)
  if beobachtet[addr] then return end
  beobachtet[addr] = true
  local bp = PCSX.addBreakpoint(addr, 'Write', 4, 'teilwort', function(address, width, cause)
    local ok, err = pcall(function()
      local m = PCSX.getMemPtr()
      local regs = PCSX.getRegisters()
      local pc = regs.pc
      local instr = u32(m, pc)
      local rt = bit.band(bit.rshift(instr, 16), 0x1f)
      local val = regs.GPR.r[rt] or 0
      log(string.format("  WRITE B%d %s @%08x pc=%08x instr=%08x val=%08x alt=%08x ra=%08x cause=%s",
          frames, tag, addr, pc, instr, val, u32(m, addr), regs.GPR.r[31] or 0, tostring(cause)))
    end)
    if not ok then log("CB-FEHLER write: " .. tostring(err)) end
    return true
  end)
  _G.__keep[#_G.__keep + 1] = bp
end

local function exec_bp(addr, name, fn)
  local bp = PCSX.addBreakpoint(addr, 'Exec', 4, name, function()
    local ok, err = pcall(fn)
    if not ok then log("CB-FEHLER " .. name .. ": " .. tostring(err)) end
    return true
  end)
  _G.__keep[#_G.__keep + 1] = bp
end

local function stage1(m)
  return u32(m, 0x801008b0) == 0x0c006bc8 and u32(m, 0x8011f7ec) == 0x80101058
end
local function zombie_ent(e) return e >= 0x800acc2c and e < 0x800acc2c + 16 * 0x1f4 end

local function installiere()
  exec_bp(0x80100688, 'init', function()
    local m = PCSX.getMemPtr()
    if not stage1(m) then return end
    local e = u32(m, 0x800ac784)
    log(string.format("INIT B%d ent=%08x slot=%d typ=%02x grid=%02x teile=%08x n=%d",
        frames, e, math.floor((e - 0x800acc2c) / 0x1f4), u8(m, e + 8), u8(m, e + 9), u32(m, e + 0x188), u8(m, e + 0x83)))
  end)
  for fa, teile in pairs(VARIANTE) do
    exec_bp(fa, string.format('var%08x', fa), function()
      local m = PCSX.getMemPtr()
      if not stage1(m) then return end
      local e = u32(m, 0x800ac784)
      if not zombie_ent(e) then return end
      local p = u32(m, e + 0x188)
      local s = string.format("VARIANTE %08x B%d ent=%08x slot=%d typ=%02x teile=%08x", fa, frames, e,
          math.floor((e - 0x800acc2c) / 0x1f4), u8(m, e + 8), p)
      for _, k in ipairs(teile) do
        s = s .. string.format(" | k%d wort=%08x", k, u32(m, p + k * 0xac))
        wache_wort(p + k * 0xac, string.format("slot%d.k%d", math.floor((e - 0x800acc2c) / 0x1f4), k))
      end
      log(s)
    end)
  end
  exec_bp(0x8001e5b0, 'e5b0', function()
    local regs = PCSX.getRegisters()
    log(string.format("E5B0 B%d a0=%08x ra=%08x", frames, regs.GPR.r[4] or 0, regs.GPR.r[31] or 0))
  end)
  exec_bp(0x8001ee68, 'ee68', function()
    local regs = PCSX.getRegisters()
    log(string.format("EE68 B%d a0=%08x ra=%08x", frames, regs.GPR.r[4] or 0, regs.GPR.r[31] or 0))
  end)
end

local function zombies(m)
  for i = 0, 15 do
    local e = 0x800acc2c + i * 0x1F4
    local typ = u8(m, e + 8)
    if bit.band(u32(m, e), 1) == 1 and typ >= 0x10 and typ <= 0x1f then
      local p = u32(m, e + 0x188)
      local s = string.format("   slot %2d typ %02x st %d/%d teile=%08x", i, typ, u8(m, e + 4), u8(m, e + 5), p)
      if p >= 0x80000000 and p < 0x80200000 then
        for k = 0, 14 do
          local w = u32(m, p + k * 0xac)
          if w ~= 1 or u16(m, p + k * 0xac + 0x88) == 0x5f4 or u16(m, p + k * 0xac + 0x88) == 0x7e8 then
            s = s .. string.format(" k%d=%08x s%04x", k, w, u16(m, p + k * 0xac + 0x88))
          end
        end
      end
      log(s)
    end
  end
end

function DrawImguiFrame()
  if phase == 9 then return end
  frames = frames + 1

  if out == nil then
    out = io.open(OUT, 'w')
    if out == nil then return end
    log("ZA zombiearm: ROOM1140 ueber das Debug-Menue")
    local ok1 = pcall(function()
      _G.__menu = PCSX.addBreakpoint(0x80014444, 'Exec', 4, 'dbgmenu', function()
        menuehits = menuehits + 1
        return true
      end)
    end)
    local ok3 = pcall(function()
      _G.__jump = PCSX.addBreakpoint(0x8001d630, 'Exec', 4, 'jumptarget', function()
        local m = PCSX.getMemPtr()
        w8(m, 0x800BBE5E, 0)        -- Stage 0
        w8(m, 0x800BBE5F, 0x14)     -- Index 0x14 = BRIEFING -> Raum 0x114 -> ROOM1140
        zielgesetzt = zielgesetzt + 1
        return true
      end)
    end)
    local ok2 = pcall(function()
      _G.__pad = PCSX.addBreakpoint(0x800AC76C, 'Write', 4, 'confirm', function()
        local m = PCSX.getMemPtr()
        if menuehits > 0 then
          w8(m, 0x800BBE5D, 1)
          w8(m, 0x800BBE5E, 0)
          w8(m, 0x800BBE5F, 0x14)
        end
        if bestaetige and (frames % 16) < 2 then w16(m, 0x800AC762, 0x80) end
        return true
      end)
    end)
    local ok4, e4 = pcall(installiere)
    log(string.format("Haltepunkte: Menue=%s Pad=%s Ziel=%s Wachen=%s %s", tostring(ok1), tostring(ok2), tostring(ok3), tostring(ok4), tostring(e4)))
  end

  local ok, err = pcall(function()
    local m = PCSX.getMemPtr()
    local px, pz = s32(m, 0x800aca88), s32(m, 0x800aca90)
    if spielerAb < 0 and (px ~= 0 or pz ~= 0) then
      spielerAb = frames
      log(string.format("SPIELER existiert ab Bild %d: (%d,%d)", frames, px, pz))
    end
    if menueAb < 0 and menuehits > 0 then
      menueAb = frames
      log(string.format("MENUE offen ab Bild %d (%d Aufrufe)", frames, menuehits))
    end
    if menuehits > 0 and phase == 0 then
      w8(m, 0x800BBE5D, 1); w8(m, 0x800BBE5E, 0); w8(m, 0x800BBE5F, 0x14)
      wartet = wartet + 1
      if wartet > 120 then
        bestaetige = true
        if zielgesetzt > 0 or wartet > 1500 then phase, wartet = 1, 0 end
      end
    end
    if frames % 300 == 0 then
      log(string.format("B%d Menue %d Ziel %d Idx %02x Phase %d", frames, menuehits, zielgesetzt, u8(m, 0x800B0FE2), phase))
    end
    if phase == 1 then
      wartet = wartet + 1
      if wartet % 300 == 0 then
        if zielgesetzt > 0 then bestaetige = false end
        log(string.format("== Bild %d Raum-Idx %02x Spieler (%d,%d) ==", frames, u8(m, 0x800B0FE2), px, pz))
        zombies(m)
      end
      if wartet >= 3000 then
        log("fertig"); out:close(); out = nil; phase = 9; PCSX.quit(0)
      end
    end
  end)
  if not ok and out ~= nil then log("FEHLER: " .. tostring(err)); phase = 9 end
end
