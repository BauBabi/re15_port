local OUT = [[C:\workspace\git\reAi_v2\tools\redux\mini_out.txt]]
local frames = 0
local getan = false
function DrawImguiFrame()
  if getan then return end
  frames = frames + 1
  if frames < 30 then return end
  getan = true
  local out = io.open(OUT, 'w')
  if out == nil then return end
  out:write("A: Datei offen, Bild " .. tostring(frames) .. "\n"); out:flush()
  out:write("B: loadfile = " .. tostring(loadfile) .. "\n"); out:flush()
  local ok, chunk, lerr = pcall(loadfile, [[C:\workspace\git\reAi_v2\tools\redux\padflow.lua]])
  out:write("C: pcall=" .. tostring(ok) .. " chunk=" .. tostring(chunk) .. "\n"); out:flush()
  out:write("D: fehler=" .. tostring(lerr) .. "\n"); out:flush()
  out:write("fertig\n"); out:close()
end
