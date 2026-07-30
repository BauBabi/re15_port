-- parity_calib.lua — findet die JUMP-ZEILE und die Cursor-Adresse, indem es das Debug-Menue absucht.
--
-- SEPARATE Datei, absichtlich: parity_trace.lua laedt sie per pcall(dofile), also ist ein Fehler HIER
-- fangbar und landet im Log. Als die Kalibrierung im Haupt-Chunk stand, verhinderte ein Fehler darin
-- das Uebersetzen des GANZEN Chunks — kein Logfile, keine Meldung, nicht von einem toten Emulator zu
-- unterscheiden. Das hat mehrere Runden gekostet.
--
-- WAS HIER GESUCHT WIRD, und warum so:
-- Der Nutzer bestaetigt, dass Select das Debug-Menue oeffnet; nur die Raumauswahl greift nicht. Mein
-- fest verdrahtetes "einmal Down, dann Links" trifft die JUMP-Zeile also nicht. Statt die Zeile zu
-- raten, wird sie gesucht: pro Runde EIN Down, dann N x Links, und danach geprueft, ob irgendein Byte
-- um exakt N gefallen ist. Die Runde, in der das passiert, IST die JUMP-Zeile — und das gefundene
-- Byte ist der Raum-Cursor. Beides faellt in einem einzigen Lauf an, statt in je einem pro Vermutung.
--
-- Der Zwei-Stufen-Test bleibt: ein EINZELNER Diff ueber N Druecke findet auch Bytes, die sich aus
-- anderen Gruenden um N unterscheiden (Timer, Animationszaehler) — er lieferte 15 Kandidaten, alle
-- Rauschen. Deshalb wird JEDE Runde erst gemeldet, wenn sie zweistufig bestaetigt ist.
--
-- Erwartet die Bruecke PT (vom Haupt-Chunk): PT.u8, PT.log, PT.press, PT.release, PT.padL, PT.padD

PT_CAL = { on = 0, nxt = -1, press = 0, dir = nil, A = nil, B = nil,
           lo = tonumber(os.getenv('RE15_PT_LO') or '0x80080000'),
           hi = tonumber(os.getenv('RE15_PT_HI') or '0x80130000') - 1,
           n = 8, row = 0, maxrow = 6 }

local function snap()
  local t = {}
  for a = PT_CAL.lo, PT_CAL.hi do t[a] = PT.u8(a) end
  return t
end

function PT_CAL_START(frame, n)
  PT_CAL.n, PT_CAL.on, PT_CAL.nxt, PT_CAL.row = n or 8, 1, frame + 240, 0
  PT.log:write(string.format("# f%d MENUE-SUCHE: bis zu %d Zeilen, je 2x%d Links\n",
                             frame, PT_CAL.maxrow, PT_CAL.n))
  PT.log:flush()
end

function PT_CAL_TICK(frame)
  local C = PT_CAL
  if C.on == 0 then return false end
  if C.rel and frame >= C.rel then PT.release(); C.rel = nil end
  if frame < C.nxt then return true end

  if C.press > 0 then                      -- laufende Tastenfolge abarbeiten
    C.press = C.press - 1
    PT.release(); PT.press(C.dir)
    -- Haltedauer: 3 Frames reichten dem Menue offenbar nicht. HOLD ist konfigurierbar,
    -- damit "zu kurz gedrueckt" von "Richtung erreicht das Menue gar nicht" trennbar ist.
    C.rel, C.nxt = frame + (PT_HOLD or 12), frame + (PT_GAP or 30)
    return true
  end

  if C.on == 1 then                        -- eine Zeile tiefer, dann Stufe A
    C.row = C.row + 1
    if C.row > C.maxrow then
      PT.log:write(string.format("# f%d MENUE-SUCHE erschoepft: keine Zeile reagiert auf Links\n", frame))
      PT.log:flush(); C.on = 0; PT.release(); return false
    end
    C.dir, C.press = PT.padD, 1            -- ein Down
    C.on, C.nxt = 2, frame + 40
    PT.log:write(string.format("# f%d Zeile %d: Down\n", frame, C.row)); PT.log:flush()
  elseif C.on == 2 then
    C.A = snap()
    C.dir, C.press = PT.padL, C.n
    C.on, C.nxt = 3, frame + 30
  elseif C.on == 3 then
    C.B = snap()
    C.dir, C.press = PT.padL, C.n
    C.on, C.nxt = 4, frame + 30
  else
    local hits, first = 0, nil
    for a = C.lo, C.hi do
      if C.B[a] - C.A[a] == -C.n and PT.u8(a) - C.B[a] == -C.n then
        hits = hits + 1
        first = first or a
        if hits <= 8 then
          PT.log:write(string.format("# CURSOR 0x%08x: %d -> %d -> %d\n",
                                     a, C.A[a], C.B[a], PT.u8(a)))
        end
      end
    end
    PT.log:write(string.format("# f%d Zeile %d: %d beidseitige Treffer\n", frame, C.row, hits))
    PT.log:flush()
    C.A, C.B = nil, nil
    if hits > 0 then
      PT.log:write(string.format("# >>> JUMP-ZEILE = %d Downs, CURSOR = 0x%08x\n", C.row, first))
      PT.log:flush(); C.on = 0; PT.release(); return false
    end
    C.on, C.nxt = 1, frame + 30            -- naechste Zeile probieren
  end
  return true
end
