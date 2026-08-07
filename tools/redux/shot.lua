-- shot.lua — Diagnose: zeigt das Spiel ueberhaupt ein Bild, und wo haengt es?
--
-- BEFUND aus pad_bp.lua: der Schreib-Haltepunkt auf das Pad-Flankenwort feuerte im GANZEN Lauf
-- nur EIN einziges Mal, Spieler blieb (0,0,0), Modus 0 — das Spiel fuehrt seine Hauptschleife
-- nicht aus. Statt weiter zu raten: Bildstatistik + Programmzaehler ansehen.
--
-- ⚠ ss.width/height/bpp und Slice-Groessen sind CDATA — immer durch tonumber().

local OUT = [[C:\workspace\git\reAi_v2\tools\redux\shot_info.txt]]
local frames = 0
local fertig = false
local out = nil

function DrawImguiFrame()
  if fertig then return end
  frames = frames + 1
  if frames % 400 ~= 0 then return end
  if frames > 1600 then
    if out ~= nil then out:write("fertig\n"); out:close() end
    fertig = true
    PCSX.quit(0)
    return
  end

  if out == nil then
    out = io.open(OUT, 'w')
    if out == nil then return end
  end

  local ok, err = pcall(function()
    -- Wo steht die CPU?
    local regs = PCSX.getRegisters()
    out:write(string.format("\n== Bild %d ==\n", frames))
    out:write(string.format("  pc = %08x\n", tonumber(regs.pc)))

    -- Bildstatistik
    local ss = PCSX.GPU.takeScreenShot()
    local w, h, bpp = tonumber(ss.width), tonumber(ss.height), tonumber(ss.bpp)
    local n = tonumber(ss.data:size())
    out:write(string.format("  Bild %dx%d bpp=%d, %d Bytes\n", w, h, bpp, n))
    local hell, gesamt = 0, 0
    local schritt = 997                       -- Primzahl: streut ueber das ganze Bild
    local i = 0
    while i < n do
      local b = tonumber(ss.data:byte(i))
      gesamt = gesamt + 1
      if b > 24 then hell = hell + 1 end
      i = i + schritt
    end
    out:write(string.format("  Stichprobe: %d Punkte, davon %d nicht schwarz (%d%%)\n",
              gesamt, hell, math.floor(hell * 100 / math.max(gesamt, 1))))
  end)
  if not ok then out:write("FEHLER: " .. tostring(err) .. "\n") end
  out:flush()
end
