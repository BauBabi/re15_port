-- pathcount.lua — welche Code-Pfade laufen beim Debug-Menue tatsaechlich?
--
-- ANLASS: Mein Exec-Haltepunkt auf 0x8001d630 (angeblich der JUMP-Ausfuehrer) feuerte NIE,
-- obwohl Raumwechsel stattfanden. Meine Annahme ueber den Pfad war also falsch. Statt weiter
-- zu raten: Zaehler auf alle Kandidaten setzen und nachsehen, was wirklich ausgefuehrt wird.

local OUT = [[C:\workspace\git\reAi_v2\tools\redux\path_out.txt]]
local frames, out = 0, nil
local n = {}

local KAND = {
  { 0x80014444, "Menuefunktion" },
  { 0x80014a44, "JUMP bestaetigt (sb 1 -> Modus)" },
  { 0x80014a50, "JUMP: Datensatz-Zeiger NULL" },
  { 0x8001d630, "Raumwahl-Kette (lbu Menue-Stage)" },
  { 0x8001d660, "Raumwahl: neuer Raum -> 0x800b0fe2" },
  { 0x800396fc, "Raumlader FUN_800396fc" },
  { 0x80025c00, "Karten-/Menue-FSM FUN_80025c00" },
}

local function u8(m, a) return m[bit.band(a, 0x1fffff)] end
local function u32(m, a)
  local o = bit.band(a, 0x1fffff)
  return m[o] + m[o+1]*256 + m[o+2]*65536 + m[o+3]*16777216
end
local function s32(m, a)
  local v = u32(m, a); if v >= 0x80000000 then v = v - 0x100000000 end; return v
end

function DrawImguiFrame()
  frames = frames + 1
  if out == nil then
    out = io.open(OUT, 'w')
    if out == nil then return end
    _G.__bps = {}
    for i, k in ipairs(KAND) do
      n[i] = 0
      local ok = pcall(function()
        _G.__bps[i] = PCSX.addBreakpoint(k[1], 'Exec', 4, 'p' .. i, function()
          n[i] = n[i] + 1
          return true   -- MESSUNG: false LOESCHT den Haltepunkt (1 Treffer, dann tot)
        end)
      end)
      out:write(string.format("Haltepunkt %d @0x%08x %-38s %s\n", i, k[1], k[2], tostring(ok)))
    end
    out:flush()
  end

  if frames % 600 ~= 0 then return end
  local ok, err = pcall(function()
    local m = PCSX.getMemPtr()
    local z = {}
    for i = 1, #KAND do z[#z+1] = string.format("%d:%d", i, n[i]) end
    out:write(string.format("B%5d | %s | Spieler (%d,%d) Modus %d | Menue-Bytes %d/%d/%02x\n",
      frames, table.concat(z, " "),
      s32(m, 0x800aca88), s32(m, 0x800aca90), u8(m, 0x800b5359),
      u8(m, 0x800BBE5D), u8(m, 0x800BBE5E), u8(m, 0x800BBE5F)))
    out:flush()
    if frames >= 18000 then
      out:write("\nfertig\n"); out:close(); PCSX.quit(0)
    end
  end)
  if not ok and out ~= nil then out:write("FEHLER: " .. tostring(err) .. "\n"); out:flush() end
end
