-- bpret.lua — feuern Haltepunkte nur EINMAL, weil der Rueckgabewert false sie entfernt?
-- Beide Varianten am SELBEN, jedes Bild laufenden Code (Pad-Aufbereiter FUN_80030444).
local OUT = [[C:\workspace\git\reAi_v2\tools\redux\bpret_out.txt]]
local frames, out = 0, nil
local nfalse, ntrue, nnil = 0, 0, 0
function DrawImguiFrame()
  frames = frames + 1
  if out == nil then
    out = io.open(OUT, 'w')
    if out == nil then return end
    _G.__k = {}
    pcall(function() _G.__k[1] = PCSX.addBreakpoint(0x80030444,'Exec',4,'rf',function() nfalse=nfalse+1; return false end) end)
    pcall(function() _G.__k[2] = PCSX.addBreakpoint(0x80030448,'Exec',4,'rt',function() ntrue =ntrue +1; return true  end) end)
    pcall(function() _G.__k[3] = PCSX.addBreakpoint(0x8003044c,'Exec',4,'rn',function() nnil  =nnil  +1 end) end)
    out:write("Haltepunkte auf FUN_80030444 (laeuft jedes Bild)\n"); out:flush()
  end
  if frames % 900 ~= 0 then return end
  out:write(string.format("B%5d  return false=%d | return true=%d | ohne return=%d\n",
            frames, nfalse, ntrue, nnil)); out:flush()
  if frames >= 5400 then out:write("fertig\n"); out:close(); PCSX.quit(0) end
end
