-- probe_boot.lua — Werkzeugketten-Test fuer PCSX-Redux.
-- Haken ist DrawImguiFrame(): die GUI ruft es pro gezeichnetem Bild auf (Beleg: das Beispiel
-- src/mips/psyqo/examples/pcsxlua/pcsxlua.lua im Quellbaum benutzt genau diesen Namen).
-- PCSX.nextTick() haengt dagegen an AfterPollingCleanup und feuerte hier nicht.

local OUT = [[C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\55862acd-3b0c-44af-b898-3db9f551f311\scratchpad\redux_out.txt]]
local frames = 0
local done = false

local function u32(mem, addr)
  local o = bit.band(addr, 0x1fffff)
  return mem[o] + mem[o+1]*256 + mem[o+2]*65536 + mem[o+3]*16777216
end
local function s32(mem, addr)
  local v = u32(mem, addr)
  if v >= 0x80000000 then v = v - 0x100000000 end
  return v
end

function DrawImguiFrame()
  if done then return end
  frames = frames + 1
  if frames < 900 then return end          -- ~15 s bei 60 Hz: Boot + Intro abwarten
  done = true
  local ok, err = pcall(function()
    local mem = PCSX.getMemPtr()
    local f = io.open(OUT, 'w')
    f:write(string.format("Bilder: %d\n", frames))
    f:write(string.format("RAM@0x80010000 = %08x\n", u32(mem, 0x80010000)))
    f:write(string.format("Stub@0x80026e4c = %08x   (erwartet 03e00008 = ORIGINAL)\n",
                          u32(mem, 0x80026e4c)))
    f:write(string.format("Spieler-Entity 0x800aca54: x=%d y=%d z=%d\n",
                          s32(mem, 0x800aca88), s32(mem, 0x800aca8c), s32(mem, 0x800aca90)))
    f:write(string.format("Pad held 0x800ac768 = %08x   edge 0x800ac76c = %08x\n",
                          u32(mem, 0x800ac768), u32(mem, 0x800ac76c)))
    f:close()
  end)
  if not ok then
    local f = io.open(OUT, 'w')
    f:write("LUA-FEHLER: " .. tostring(err) .. "\n")
    f:close()
  end
end
