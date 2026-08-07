-- padapi.lua — was bietet die Pad-API dieser PCSX-Redux-Version wirklich?
--
-- ANLASS: 182 SELECT-Zyklen ueber setOverride, aber das Menue oeffnete nicht. Verdacht:
-- PCSX.CONSTS.PAD.BUTTON.SELECT existiert nicht -> setOverride(nil) tut nichts und pcall
-- meldet trotzdem Erfolg. Also erst nachsehen, statt weiter zu druecken.

local OUT = [[C:\workspace\git\reAi_v2\tools\redux\padapi_out.txt]]
local frames = 0
local fertig = false

local function dump(out, t, name, tiefe)
  if tiefe > 2 then return end
  if type(t) ~= 'table' then
    out:write(string.format("%s%s = %s (%s)\n", string.rep("  ", tiefe), name, tostring(t), type(t)))
    return
  end
  out:write(string.format("%s%s = {\n", string.rep("  ", tiefe), name))
  local n = 0
  for k, v in pairs(t) do
    n = n + 1
    if n > 40 then out:write(string.rep("  ", tiefe + 1) .. "...\n"); break end
    if type(v) == 'table' then dump(out, v, tostring(k), tiefe + 1)
    else out:write(string.format("%s%s = %s (%s)\n", string.rep("  ", tiefe + 1), tostring(k), tostring(v), type(v))) end
  end
  out:write(string.rep("  ", tiefe) .. "}\n")
end

function DrawImguiFrame()
  if fertig then return end
  frames = frames + 1
  if frames < 400 then return end
  fertig = true
  local out = io.open(OUT, 'w')
  if out == nil then return end
  local ok, err = pcall(function()
    out:write("PCSX.CONSTS vorhanden: " .. tostring(PCSX.CONSTS ~= nil) .. "\n")
    if PCSX.CONSTS then dump(out, PCSX.CONSTS, "CONSTS", 0) end
    out:write("\nPCSX.SIO0 vorhanden: " .. tostring(PCSX.SIO0 ~= nil) .. "\n")
    if PCSX.SIO0 then
      local p = PCSX.SIO0.slots and PCSX.SIO0.slots[1] and PCSX.SIO0.slots[1].pads
                and PCSX.SIO0.slots[1].pads[1]
      out:write("Pad1 vorhanden: " .. tostring(p ~= nil) .. "\n")
      if p then dump(out, p, "pad1", 0) end
    end
    -- was bietet PCSX selbst?
    out:write("\nPCSX-Felder:\n")
    for k, v in pairs(PCSX) do out:write("  " .. tostring(k) .. " (" .. type(v) .. ")\n") end
  end)
  if not ok then out:write("FEHLER: " .. tostring(err) .. "\n") end
  out:flush(); out:close()
  PCSX.quit(0)
end
