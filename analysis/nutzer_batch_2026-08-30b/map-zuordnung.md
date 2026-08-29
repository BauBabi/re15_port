# Karten-Zuordnung Raum → Rect (RE2-Kartensystem im Port)

Nutzer-Auftrag 2026-08-30: das RE2-Retail-Kartensystem (aktueller Raum ROT, besuchte
GRÜN, unbesuchte SCHWARZ, Türen/Treppen eingezeichnet) „bezogen auf unsere Map".
Dieses Dokument ist die Ableitung der generierten Tabelle
`re15_port/engine/src/re15_map_rooms.h`. Das RE2-Vorbild (Mechanismus, @0x…-Belege)
steht in `map-re2-system.md`; die RE1.5-Map-Datenlage in `map-port-und-daten.md`.

## 1. Was zugeordnet werden muss

Der RE1.5-MAP-Screen zeichnet pro Seite eine feste Rect-Liste (Pair-Table
@0x80076840 `{u16 count, u32 ptr}`, Einträge Stride 12 `{x,y,w,h,u@+8,v@+10}`) —
**ohne** jede Verbindung Rect↔Raum. Das RE2-System braucht genau diese Verbindung
(welches Rect färbt sich, wenn Raum X aktuell/besucht ist). RE1.5 liefert nur einen
indirekten Anker: die **Marker-Zeile je Raum-Slot** (@0x800768B0 + 8·Slot,
`{s16 xoff, u16 yoff, u16 xscl, u16 zscl}`, Slot = Stage-Basis {0,38,50,65,77,98} +
Raum-Index), durch die die Spielerposition per FUN_800473f8 (@0x8004741c-528) auf
Seiten-Pixel projiziert wird.

## 2. Methode (nur belegbare Zuordnungen)

Je Basis-Raum (Szenario-Varianten 0xSRR0/0xSRR1 gepoolt — Seite/Slot hängen NUR am
Raum-Index, FUN_8004b568 liest 0x800b0fe2):

1. Alle AOT-Zentren des Raums (Census über beide Varianten-RDTs) durch die
   **EXE-eigene Marker-Zeile des Raums** projizieren.
2. Voting: kleinstes enthaltendes Rect je Punkt; Annahme bei
   - `seed`: Top ≥ 60 % aller Treffer und ≥ 2× Zweitplatzierter, oder
   - `seed-med`: Top ≥ 40 % und der **Median-Punkt** des Raums landet im Top-Rect.
3. `forced` (kam nicht vor): genau 1 offener Raum + 1 freies Rect auf einer Seite.

**Verworfene Methoden** (Negativ-Befunde, nicht wiederholen):
- Least-Squares-Seiten-Fit: reproduzierte nur 21/76 der eigenen Kalibrierung.
- Seiten-Mittel-Projektion (Kalibrierung von Nachbar-Räumen leihen): Kontrolle auf
  der EIGENEN Seite nur 2/8 — die Zeilen sind pro Raum handgesetzt, nicht affin.
- **Tür-Graph-Propagation auf freie Rects: erzeugte 31 Varianten-Divergenzen**
  (gleicher physischer Raum auf verschiedenen Rects, z. B. 0x1140→r3 vs. 0x1141→r0)
  → komplett verworfen. Ein falsch rot gefärbter Raum ist schlechter als neutral.

## 3. Ergebnis

**28 Basis-Räume zugeordnet (×2 Varianten = 56 Einträge), 0 Widersprüche.**
Geteilte Rects sind legitim (Phasen-/Flur-Teilung): (7,0)=3000+3080,
(9,3)=5030+5110, (9,4)=5040+5120.

Warum nicht mehr: **65 der 120 Basis-Räume tragen im Auslieferungsstand nur die
Platzhalter-Zeile `{0,0,1,1}`** — dort ist bereits der Stock-Positionsmarker defekt
(projiziert alles auf ~(0,0)); es existiert schlicht keine Kalibrierung, gegen die
man voten könnte. Dazu: 13 Räume ohne Map-Seite (Stage-Init verwirft den
Raum-Index), 2 Räume auf der defekten Seite 13 (CD-Datei-Id-Überlauf @0x80074c4c),
9 unentschiedene Votings (dokumentiert im Skript-Log), 3 ohne verwertbare Samples.

Unzugeordnete Räume/Rects behalten im Port den **Stock-Neutralton** (128er-
Modulation) — sichtbar „wie bisher", nie falsch gefärbt.

## 4. Port-Implementierung

- `re15_map_visited.c` — 240-Bit-Besucht-Speicher (Index = re15_room_ids[]),
  markiert am einzigen Raum-Lade-Choke-Point `scd_room_reenter`; Reset bei NEW GAME;
  Save v6 (`visited[32]` vor der Checksumme, v5→v6-Upgrade validiert das alte
  Checksum-Extent). Rect-Zustand: `re15_map_rect_state(page, rect)` =
  Max(aktuell > besucht > unbesucht) über alle zugeordneten Räume des Rects.
- `re15_inv_screen.c` (MAP-Draw): RE2-Logik FUN_8006e120 @0x8006e4c4-72c —
  unbesucht → **kein Prim** (schwarz = Hintergrund), besucht → grün, aktueller Raum
  → stetiges Highlight (Basis+1-Analog @0x8006e60c; im Map-Tab blinkt bei RE2 nur
  der Spieler-Pfeil, nicht der Raum). RE2 schaltet CLUT-Zeilen um, deren RGB in
  RE2-CD-Datei 170 liegen (lokal nicht vorhanden) — die Tint-Werte des Ports
  (grün 40/144/40, rot 192/24/24) sind deshalb dokumentierte PORT-WAHL nach
  Nutzer-Spezifikation; nur die Logik ist RE2-byte-belegt.
- Türen/Treppen: bei RE1.5 sind die Durchgänge in die Rect-Grafik der Map-Seiten
  eingemalt (wie RE2s gelbe Türpunkte, map-re2-system.md §4.1) und färben mit dem
  Rect mit. RE2s 14 separate Blink-Marker (@0x800a9b1c) haben in RE1.5 kein
  Daten-Gegenstück (keine Marker-Tabelle, keine Icon-Zeile v≥0xf0 in den
  MAP*.PIX-Seiten) — nicht nachgebaut.
- Pin: `tests/unit/test_map_re2_system.c` (Choke-Point-Wache, Zustands-Prioritäten,
  Save-Roundtrip + v5→v6, Negativ-Kontrolle: ohne die Markierung fällt der Pin —
  verifiziert). FSM-Fixture `test_inv_fsm.c` (5) auf 2+6/2+7 gehoben.

## 5. Offene Wege für mehr Abdeckung (später)

1. Live-Autorisierung: Raum betreten (Debug-JUMP), MAP öffnen, sehen welches Rect
   der Nutzer erwartet — pro Sitzung ~Dutzend Räume.
2. Die 9 Unentschiedenen mit gefilterten Samples (nur Tür-AOTs, keine geparkten
   Ziel-Koordinaten) erneut voten.
3. Für Platzhalter-Räume: Rect-Geometrie gegen die Raum-Kollisionsbox statt gegen
   Marker-Projektion matchen (erfordert Seiten-Layout-Verständnis je Gebäude).
