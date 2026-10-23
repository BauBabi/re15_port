# Runde 40 — UEBERGABE (laufend; neueste Einträge oben)

Für jede nächste Sitzung geschrieben. Pflichtlektüre: `CLAUDE.md`, `AUFTRAG.md` (Wortlaut). Release-Ziel **v0.8.28**.
Zeiten immer aus der Uhr (date), nie schätzen.

## ✅ Fortschritt (neueste zuerst)

* 2026-10-23 09:58: ROOM1120/1121 Text-Plätze 5/6/7 -> msg 1/3/4 (Modul `engine/src/untersuchen_r40.c`,
  `include/re15_untersuchen_r40.h`, Haken am Ende des Install-Blocks in `scd_room_setup.c`). Satzform = Telefon
  ROOM1120 main00 @0x00CF8; Orte aus der Rückprojektion (analysis/untersuchungstexte_2026-10-23). Riegel
  `unit_r40_untersuchen1120` grün (39 Prüfungen, beide Räume, Aktion vor Bild/Fenster/Schrank öffnet msg 1/3/4
  mit RDT-Wortlaut; Telefon bleibt Save-Punkt; ROOM1130 ohne Plätze). Gezielt 17/17 (1120|1121|aot|savepoint).

## Befund ROOM11D0 (Hundenamen) — nur Frage, kein Code

* Cut 12 = Nahaufnahme des Zwinger-Schaltpults: sechs Knöpfe BOCKER, COURIER, GUTS, ROCKEY, DOGMEAT, JoJo mit roten
  Lämpchen, rechts ein Kasten mit Steckplatz. Kamera Cut 12 P(-3637,-2842,-21815) -> T(-599,-474,-14143), also direkt
  vor dem Pult-Platz 2 (main00 @0x01248, Rechteck (-3600,-20400,800,800), msg 0 "A panel to control the cells of the
  dogs. It does not work.").
* Nichts schaltet auf Cut 12: keine RVD-Zone hat Cut 12 als Ziel (37 Zonen, Ziele 0-4, 8-11, 13, 14), kein `29 0c` /
  Cut-Befehl in main00/sub00..03 von 11D0 oder 11D1 (einzig `29 0d` @0x01760 in sub02 = Zwinger offen).
* Kein Text mit Hundenamen (5 Texte, alle verdrahtet: Plätze 2..6), kein Flag, kein Item für ein Namensrätsel.
  Die Hunde kommen über die Falle: Item 0x05 (15 Stück, Bit (9,0x9B)) im Lager, sub03 @0x01892 -> sub02 @0x01728
  (Member_set 0x0C = 0x42 für alle fünf, Cut 13).
* Ergebnis: vom Rätsel existiert nur das Bild und der Kamerasatz.
