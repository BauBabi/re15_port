-- parity_calib.lua — findet den Ziel-Raum im Debug-JUMP durch AUSPROBIEREN mit objektivem Abbruch.
--
-- SEPARATE Datei, absichtlich: parity_trace.lua laedt sie per pcall(dofile), ein Fehler HIER ist
-- daher fangbar und landet im Log. Als das im Haupt-Chunk stand, verhinderte ein Fehler darin das
-- Uebersetzen des GANZEN Chunks — kein Logfile, keine Meldung, nicht von einem toten Emulator zu
-- unterscheiden. (Die vorige Adress-Suche liegt als parity_calib_addrsearch.lua.bak daneben.)
--
-- WARUM AUSPROBIEREN STATT ADRESSE:
-- Die Cursor-Adresse zu bestimmen kostete mehrere Runden und drei falsche Detektoren: "delta == -N"
-- konnte nie anschlagen, weil die JUMP-Liste eine Liste von RAUM-IDs ist und keine Zaehlreihe;
-- "monoton fallend" lieferte 60 auslaufende Puffer; verschaerft blieben 19 Kandidaten. Und selbst
-- mit der richtigen Adresse fehlte noch der Zielwert, den ich nicht kenne.
-- Die Adresse wird gar nicht gebraucht: ob der richtige Raum geladen wurde, sagt das ROSTER.
-- ROOM1140 (BRIEFING) hat GENAU 5 Gegner der Typen 0x16,0x10,0x10,0x11,0x11 — eine objektive
-- Abbruchbedingung, unabhaengig von jeder Annahme ueber die Menue-Kodierung.
--
-- ABLAUF je Versuch: Select -> Down (JUMP-Zeile, bestaetigt = 1x) -> N x Links -> Square -> warten
-- -> Roster pruefen. Trifft es nicht, naechstes N. EIN Emulator-Start fuer ALLE Versuche.
--
-- Erwartet PT: PT.u8, PT.log, PT.press, PT.release, PT.padL, PT.padD, PT.padSel, PT.padSq, PT.roster

PT_CAL = { on = 0, nxt = -1, press = 0, dir = nil, n = 0, try = 0, rel = nil,
           steps = { 4, 8, 12, 16, 20, 24, 28, 32 } }

function PT_CAL_START(frame)
  PT_CAL.on, PT_CAL.try, PT_CAL.nxt = 1, 0, frame + 200
  PT.log:write(string.format("# f%d RAUM-SUCHE: %d Schrittzahlen werden probiert\n",
                             frame, #PT_CAL.steps))
  PT.log:flush()
end

function PT_CAL_TICK(frame)
  local C = PT_CAL
  if C.on == 0 then return false end
  if C.rel and frame >= C.rel then PT.release(); C.rel = nil end
  if frame < C.nxt then return true end

  if C.press > 0 then
    C.press = C.press - 1
    PT.release(); PT.press(C.dir)
    C.rel, C.nxt = frame + (PT_HOLD or 12), frame + (PT_GAP or 30)
    return true
  end

  if C.on == 1 then
    C.try = C.try + 1
    if not C.steps[C.try] then
      PT.log:write(string.format("# f%d RAUM-SUCHE erschoepft\n", frame)); PT.log:flush()
      C.on = 0; PT.release(); return false
    end
    C.n = C.steps[C.try]
    C.dir, C.press = PT.padSel, 1
    C.on, C.nxt = 2, frame + 90
    PT.log:write(string.format("# f%d Versuch %d: %d Schritte\n", frame, C.try, C.n)); PT.log:flush()
  elseif C.on == 2 then
    C.dir, C.press = PT.padD, 1
    C.on, C.nxt = 3, frame + 60
  elseif C.on == 3 then
    C.dir, C.press = PT.padL, C.n
    C.on, C.nxt = 4, frame + 40
  elseif C.on == 4 then
    C.dir, C.press = PT.padSq, 1
    C.on, C.nxt = 5, frame + 60 * 12
  else
    local n, sig = PT.roster()
    PT.log:write(string.format("# f%d Versuch %d (%d Schritte): %d Gegner [%s]\n",
                               frame, C.try, C.n, n, sig)); PT.log:flush()
    if sig == "16,10,10,11,11" then
      PT.log:write(string.format("# >>> ROOM1140 GEFUNDEN mit %d Links-Schritten\n", C.n))
      PT.log:flush(); C.on = 0; PT.release(); return false
    end
    C.on, C.nxt = 1, frame + 60
  end
  return true
end
