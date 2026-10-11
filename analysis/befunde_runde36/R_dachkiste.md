# Runde 36 — Spur R "dachkiste" (Dossier, fortlaufend)

Baum: `.claude/worktrees/r36_dachkiste`, Zweig `r36/dachkiste`, Basis r36/integration 99398dc7
(elf integrierte Spuren + G/H/M; Spur A objfall_r36.c integriert). Diff-/Log-Bezuege gegen 99398dc7.

## Auftrag (Orchestrator, aus dem Befund der Spur H, Abschnitte 2.2/2.4 und OFFEN)

Die Kiste obj 1 auf dem DACH von ROOM1090/1091 ist die einzige Stufe zum Tor nach ROOM1100 (Tuer-Record
slot 1 @0x0213A verlangt Band 6 = Kistenoberkante y -10800; aus ROOM1100 fuehrt keine Tuer zurueck). Seit
Spur A (Objekt-Fall FUN_8002c444 in engine/src/objfall_r36.c) kann die Kiste vom Dach fallen — dann waere
das Tor unerreichbar (Softlock). Nutzer-Wortlaut der Spur A: "Das macht ein Fortschritt im Spiel unmoeglich"
— ein vom Port erzeugter Softlock ist ausgeschlossen.

1. An der exe messen, ob Leon (und Elza in ROOM1091) die Dach-Kiste im echten Spiel bis zum Fall-Ausloeser
   schieben kann (echter Weg: aus ROOM10F0 ueber den Tritt, Tuer nach 1090, Kiste in jede moegliche Richtung).
2. Am ORIGINAL messen (DuckStation, Savestate stage_saves/room1090_orig.sav): laesst das Original die Kiste
   bis zur Absprungzelle schieben (Schubpruefung FUN_8003b558 je Schritt @0x8003b634-660, Objekt-Pass
   @0x8002c0fc-c14c), oder blockt es vorher?
3. Blockt das Original -> der Port blockt an derselben Stelle mit demselben Mechanismus (Beleg @0x..., Pin).
   Laesst das Original die Kiste fallen -> byte-true belassen, Softlock als Grenze benennen, Pin auf den
   Original-Weg.
4. Dieselbe Pruefung fuer ROOM3050/3051 obj 1 und ROOM2050/2051 obj 6: haengt dort ein Fortschritt an der Kiste?

## Stand (Arbeitsprotokoll)

* 2026-10-11 Start: Baum sauber auf 99398dc7; AUFTRAG/VERTRAG/H 2.x/A Nachbesserung 3/objfall_r36.c/
  probe_r36_objfall.c/aot_common.c re15_prop_push_tick/re15_collision.c gelesen. Dossier angelegt.

## Punkt 1 — Messung an der exe (Leon ROOM1090, Elza ROOM1091)

(folgt)

## Punkt 2 — Messung am Original (DuckStation, room1090_orig.sav)

(folgt)

## RE-Belege

(folgt)

## Punkt 3 — Umsetzung

(folgt)

## Punkt 4 — ROOM3050/3051 obj 1, ROOM2050/2051 obj 6

(folgt)

## Tests

(folgt)

## OFFEN

(folgt)

## Fuer den Nutzer

(folgt)
