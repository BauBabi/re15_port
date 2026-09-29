# Runde 33, Thema T, Stufe 2 — alle restlichen Tueren bauen

Nutzer-Auftrag (woertlich): "Ich möchte das du - für alle Türen die jetzt noch fehlen mit der
Türanimation die Türen baust und Animationen hinzufügst, das wir da komplett sind."

Arbeitsbaum: `.claude/worktrees/r33_tueren` (Zweig `r33/tueren`). Laufend fortgeschrieben.
Vorgaenger: `tueren_rest_plan.md` (Plan, Stufe 1), `tueren_rest_pilot.md` (Pilot G1/G4/G6).

⛔ PORT-WAHL, KEINE Original-Adresse: RE1.5 waehlt kein Tuerarchiv (Payload+12/+13 = 0). Jede Textur,
jede Archivwahl und jede Tabellenzeile hier ist eine Bauentscheidung mit gemessener Herleitung.
Belegt sind die Rechenwege: gezeigt = Texel * c / 128 (psx-spx GPU:1438-1446), Blattfarbe c = 73
(BK 68 @0x800142e8, L @0x8009a470, LCM 1600 @0x8009a490, RGBC 0x808080 @0x80014b58; tor_helligkeit.md).

## 0. Stand beim Start (gelesen)

- 100 von 144 physischen Tueren spielen eine Sequenz (83 Runde 31 + 17 Pilot: G1 12, G4 2, G6 3).
- Port-Archive P07G, P1DG, P16M; Generator `tools/tueren/tuer_archiv_bauen.py`, Tabelle
  `engine/src/gen/re15_tuer_eigen.inc`, Riegel `probes/r33_tueren.cmake` (archive, zuordnung).
- Paket-Gate `release/make_package.sh` prueft bereits JEDE Datei `shared_assets/RE15DOOR/*.DO2`
  (Schleife ueber den Quellbaum: vorhanden, nicht leer, `cmp` gleich) - neue Archive sind damit gedeckt.
- `shared_assets/RE2/DOOR` enthaelt 24 RE2-Archive; fuer die Stufe fehlen DOOR0C (P0CD), DOOR14 (P14A)
  und DOOR36 (G12) - der Riegel "archive" vergleicht jedes Port-Archiv mit seiner Basisdatei dort.

## 1. Vorgehen

(wird gefuellt)
