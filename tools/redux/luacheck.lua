-- luacheck.lua — den Lua-SYNTAXFEHLER eines Sonden-Skripts sichtbar machen.
--
-- ANLASS: padflow.lua erzeugte seine Ausgabedatei nie. Ein Lua-LADEFEHLER geht in PCSX-Redux
-- voellig still verloren: der Treiber leitet stdout nach DEVNULL, und -stdout hilft nicht, weil
-- pcsx-redux eine GUI-Anwendung ist und die Umleitung gar nicht bedient (gemessen: Logdatei
-- 0 Byte). Bisher habe ich solche Fehler durch Halbierung gesucht — das kostet pro Schritt
-- einen ~6-Minuten-Lauf.
--
-- STATTDESSEN: dieses Skript laeuft (es ist bewusst nach dem Muster eines nachweislich
-- ladbaren Skripts gebaut) und ruft loadfile() auf den Kandidaten auf. loadfile KOMPILIERT nur,
-- fuehrt nichts aus — die Fehlermeldung samt Zeilennummer landet in der Ausgabedatei.
--
-- Aufruf:  PCSX_LUA=...\luacheck.lua  und der Kandidat in ZIEL (unten).
-- Dauer: ein paar Sekunden, kein Booten noetig.

local ZIEL = [[C:\workspace\git\reAi_v2\tools\redux\crawl_cycle2.lua]]
local OUT  = [[C:\workspace\git\reAi_v2\tools\redux\luacheck_out.txt]]

local frames, fertig = 0, false

function DrawImguiFrame()
  if fertig then return end
  frames = frames + 1
  if frames < 30 then return end
  fertig = true

  local out = io.open(OUT, 'w')
  if out == nil then return end
  out:write("Syntaxpruefung: " .. ZIEL .. "\n")

  local ok, err = pcall(function()
    local chunk, lerr = loadfile(ZIEL)
    if chunk ~= nil then
      out:write("OK — laedt sauber (Kompilat: " .. tostring(chunk) .. ")\n")
      out:write("Der Fehler liegt also NICHT in der Syntax, sondern zur Laufzeit.\n")
    else
      out:write("LADEFEHLER:\n  " .. tostring(lerr) .. "\n")
    end
  end)
  if not ok then out:write("pcall-Fehler: " .. tostring(err) .. "\n") end

  out:write("fertig\n")
  out:flush(); out:close()
  PCSX.quit(0)
end
