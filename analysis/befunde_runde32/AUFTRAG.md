# Runde 32 (2026-09-29) — Auftrag des Nutzers, wörtlich

Stand vor der Runde: master eb10ceba (v0.8.17, Suite 411). Der Nutzer hat mit der neu gebauten exe
des Hauptverzeichnisses getestet.

> Ok, ein paar findings:
> - Die Granate und die Sicherung die im hochfahrenden Modell in ROOM 1170 rein soll, liegt aktuell
>   oben drauf. Aber sie sollen unten, in den hochfahrenden Fach liegen - ein item links ein item rechts.
> - Die Türen haben eine komische Verzerrung beim öffnen in den Türsequenzen. Das ist noch falsch
> - Das von uns erstellte Tor in ROOM 1170 sieht gut aus, aber ist zu dunkel.

## Lesart und erste Belege (Leiter, vor dem Bau)

- **H — Hebetisch (ROOM1150/1151, wie Runde 30/31).** Das hochfahrende Modell (Prop 0) hat UNTER der
  Tischplatte zwei offene Fächer mit Trennwand in der Mitte (sichtbar ab der Hubfahrt,
  `analysis/befunde_2026-09-20/irons-mittelmodell/szene_mit_anhaenger.png` Bilder f_000180/f_000200).
  Runde 31 hat Granate und Sicherung oben in die Kuppel zwischen die Papierstapel gesetzt = "oben drauf".
  Ziel: Granate im LINKEN, Sicherung im RECHTEN unteren Fach (aus Sicht der Szenenkamera Cut 4).
- **V — Verzerrung der Türsequenzen.** RE2-Türzeichner FUN_8001468c: `@0x80014a28 lhu v0,324(s7)`
  (Objekt-Flags), `@0x80014a30 andi v0,v0,0x20`, `@0x80014a34 beq v0,zero,0x80014ac0`, sonst
  `@0x80014a90 jal 0x8008ebf4` (DivideGT3, DIVPOLYGON3 bei [0x800c3a80]+12). Türblätter tragen das Bit
  (DOOR13/DOOR00 obj 0 mesh 0 flags 0x0aa0); der Port (`platform/pc/src/door_scene_pc.c`) setzt Flag 0x20
  nicht um -> zwei grosse affin texturierte Dreiecke, Knick an der Diagonale (Kontaktbogen
  build/r31_tueren/t4/kontaktbogen_01.png, Spalte "Sequenz Mitte").
- **T — Tor ROOM1170 zu dunkel.** Tor-Skripte nach DOOR2E (Blatt flags 0x0a80, BK 68); Tortextur aus den
  beleuchteten Hintergrundpixeln -> doppelt abgedunkelt. RE2 kennt Flag 0x1000 (BK 136, @0x800142b4),
  benutzt nur in DOOR2B (0x1a80). Mechanismus der Korrektur nach Messung (Helligkeit Sequenz gegen das
  gemalte Tor im Raum).
