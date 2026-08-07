local f = io.open([[C:\workspace\git\reAi_v2\tools\redux\scope_out.txt]], 'w')
if f then
  f:write("CHUNK LAEUFT (Dateiebene)\n")
  f:write("PCSX vorhanden: " .. tostring(PCSX ~= nil) .. "\n")
  f:flush()
  local n = 0
  function DrawImguiFrame()
    n = n + 1
    if n == 30 then f:write("DrawImguiFrame wird gerufen (Bild 30)\n"); f:write("fertig\n"); f:flush() end
  end
end
