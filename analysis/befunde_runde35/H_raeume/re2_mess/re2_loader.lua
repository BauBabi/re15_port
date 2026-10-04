-- re2_loader.lua — laedt re2_arm_grab.lua sichtbar: Lade-/Laufzeitfehler eines -dofile-Skripts gehen in
-- PCSX-Redux sonst still verloren (Skill re15-pcsx-watchpoint, Falle 10). Schreibt <R2_OUT>/loader.txt.
local frames, done = 0, false
function DrawImguiFrame()
  if done then return end
  frames = frames + 1
  if frames < 20 then return end
  done = true
  local outdir = os.getenv("R2_OUT") or "C:/Users/mjoedicke/AppData/Local/Temp/re2run"
  local f = io.open(outdir .. "/loader.txt", "w")
  local ziel = os.getenv("R2_LUA") or ""
  if f then f:write("R2_OUT=" .. tostring(os.getenv("R2_OUT")) .. "\nR2_LUA=" .. ziel .. "\n") end
  local chunk, lerr = loadfile(ziel)
  if not chunk then
    if f then f:write("LADEFEHLER: " .. tostring(lerr) .. "\n"); f:close() end
    return
  end
  local ok, err = pcall(chunk)
  if f then f:write(ok and "geladen ok\n" or ("LAUFZEITFEHLER: " .. tostring(err) .. "\n")); f:close() end
end
