# Runde 40 — UEBERGABE (laufend; neueste Einträge oben)

Für jede nächste Sitzung geschrieben. Pflichtlektüre: `CLAUDE.md`, `AUFTRAG.md` (Wortlaut). Release-Ziel **v0.8.28**.
Zeiten immer aus der Uhr (date), nie schätzen.

## ✅ Fortschritt (neueste zuerst)

* 2026-10-23 10:18: Nutzer: "GIbt es noch weitere Dinge in STAGE1 mit son einer Nahaufnahme ... Wenn ja, baue die ruhig
  mit ein." Zensus "Kamera ohne Ausloeser" (keine RVD-Zielzone, kein Cut_chg/Cut_replace, kein Tuer-Eintritt aus irgendeinem
  Raum): 54 Eintraege / 34 Bilder in STAGE1. Nahaufnahmen vor einer passenden Untersuchungsstelle -> eingebaut
  (`engine/src/nahaufnahmen_r40.c`, `include/re15_nahaufnahmen_r40.h`, Programme aus Original-Befehlen, Vorbilder
  ROOM1051 sub03 @0x00DA4.. / ROOM11D0 sub02 @0x01884.. / ROOM1100 sub02+sub08):
  - 11D0/11D1 Cut 12 Zwinger-Pult (Hundenamen) bei Platz 2 msg 0 (Platz -> Ereignis 31).
  - 1110/1111 Cut 11 Waffenspinde bei Platz 16 msg 4 (-> Ereignis 31); Cut 12 Spind "EXPLOSIVE" um die Zeitbomben-
    Aufnahme (Ereignis 3 = sub03 @0x00D24..@0x00D47 bytegleich umhuellt).
  - 1100/1101 VUS ohne Geraet ((3,0x67)=0): Cut 7 SYSTEM READY -> 8 SAY HELLO -> 9 INVALID -> 10 TRY AGAIN, dazu msg 1
    "A type of voice device is required", zurueck Cut 6 + Cut_auto (sub02 @0x00CC0..@0x00CC6). Cut 9/10 nie geschaltet.
  - 11F0/11F1 Cut 12 Notstrom-Pult an der Wand waehrend msg 0/1 (Ereignis 16 = sub16 umhuellt; Cut_old vor Cut_chg 10,
    damit sub17s Cut_old wie im Original auf den Raum-Cut zurueckfuehrt).
  - Nicht eingebaut (keine Stelle/kein Text): 1010 Cut 2/3 Messlatte, 1090 Cut 14 Fahrzeug, 10E0 Cut 10 Bett,
    1171 Cut 12 Schild, 1190 Cut 10 Bahnfenster. 1050 Cut 7/8 nutzt der Port schon (rolltor_1050.c), 1050 Cut 9 = Elzas
    1051 sub03 (Original).
  Riegel `unit_r40_nahaufnahmen` 53/53 (Leon+Elza, angezeigter Cut je Bild, Rueckweg, Original-Teile == RDT).
  Spieler im Bild (Projektion): 11D0/11F0 unterhalb des Bildrands, 1110 Cut 12 links ausserhalb, 1110 Cut 11 klein
  vor den Spinden.
* 2026-10-23 10:05: Windows unit_/prop_/probe_ 914/915; rot nur `unit_pri_hashes` wegen UNVERSIONIERTER Nutzer-Masken
  `pri/STAGE1/11A0/01,03,06.png` (22.10.) - nicht im Release (Bau kopiert die git-Liste); Rueckfrage an den Nutzer.
  Neue Nutzer-Aufnahmen (19.10., 45 Dateien) unveraendert committet (1d1ce988).

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
