-- parity_calib.lua — findet die JUMP-Cursor-Adresse per ZWEISTUFIGEM RAM-Diff.
--
-- SEPARATE Datei, absichtlich. parity_trace.lua laedt sie per pcall(dofile): ein Syntaxfehler HIER
-- ist damit fangbar und wird ins Log geschrieben, waehrend der Haupt-Harness weiterlaeuft. Als die
-- Kalibrierung noch im Haupt-Chunk stand, hat ein Fehler darin das GANZE Chunk am Uebersetzen
-- gehindert — kein Logfile, keine Meldung, nicht unterscheidbar von einem toten Emulator. Genau das
-- hat mehrere Runden gekostet.
--
-- WARUM ZWEISTUFIG: ein einzelner Diff ueber N Tastendruecke findet auch Bytes, die sich aus anderen
-- Gruenden um N unterscheiden (Timer, Animationszaehler). Das lieferte 15 Kandidaten; der von mir
-- gewaehlte war Rauschen (Werte kreisten 44/60/124/252). Nur ein Byte, das in BEIDEN Intervallen um
-- exakt N faellt, ist der Cursor.
--
-- Erwartet die Bruecke PT (vom Haupt-Chunk gesetzt): PT.u8, PT.log, PT.press, PT.release, PT.padL

PT_CAL = { on = 0, nxt = -1, press = 0, A = nil, B = nil,
           lo = 0x80090000, hi = 0x800BFFFF, n = 8 }

function PT_CAL_START(frame, n)
  PT_CAL.n   = n or 8
  PT_CAL.on  = 1
  PT_CAL.nxt = frame + 260
  PT.log:write(string.format("# f%d KALIBRIERUNG: 2 Stufen a %d Schritte\n", frame, PT_CAL.n))
  PT.log:flush()
end

-- Gibt true zurueck, solange die Kalibrierung laeuft (der Haupt-Chunk pausiert dann seine Queue).
function PT_CAL_TICK(frame)
  local C = PT_CAL
  if C.on == 0 then return false end
  if frame < C.nxt then return true end

  if C.press > 0 then
    C.press = C.press - 1
    PT.release(); PT.press(PT.padL)
    C.nxt = frame + 27
    C.rel = frame + 3
  elseif C.on == 1 then
    C.A = {}
    for a = C.lo, C.hi do C.A[a] = PT.u8(a) end
    C.on, C.press, C.nxt = 2, C.n, frame + 30
    PT.log:write(string.format("# f%d Snapshot A\n", frame)); PT.log:flush()
  elseif C.on == 2 then
    C.B = {}
    for a = C.lo, C.hi do C.B[a] = PT.u8(a) end
    C.on, C.press, C.nxt = 3, C.n, frame + 30
    PT.log:write(string.format("# f%d Snapshot B\n", frame)); PT.log:flush()
  else
    local hits = 0
    for a = C.lo, C.hi do
      if C.B[a] - C.A[a] == -C.n and PT.u8(a) - C.B[a] == -C.n then
        hits = hits + 1
        PT.log:write(string.format("# CURSOR 0x%08x: %d -> %d -> %d\n",
                                   a, C.A[a], C.B[a], PT.u8(a)))
      end
    end
    PT.log:write(string.format("# f%d Kalibrierung fertig: %d beidseitige Treffer\n", frame, hits))
    PT.log:flush()
    C.on, C.A, C.B = 0, nil, nil
    PT.release()
    return false
  end
  if C.rel and frame >= C.rel then PT.release(); C.rel = nil end
  return true
end
