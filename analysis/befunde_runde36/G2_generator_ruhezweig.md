# Runde 36 — G2 "generator_ruhezweig" (Dossier, fortlaufend)

Baum `.claude/worktrees/r36_generator`, Zweig `r36/generator` (abgezweigt von r36/integration 7b2e56d9 = alle Spuren
der Runde 36 inkl. Spur R). Alle Disassemblate selbst mit `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` aus
`info/Re1.5/PSX.EXE` (Auslieferung: @0x80026e4c `08 00 e0 03`, Datei-Offset 0x1764c selbst gelesen).

## Auftrag (Orchestrator)

Nach dem Merge der Spur R (Ruhe-Zweig des Objekt-Passes: LAB_8002c0b8 = FUN_8003b558(obj,2) @0x8002c0d0 +
Ausschiebung FUN_8003da78 @0x8003b79c je Bild fuer jedes aktive Objekt ausser dem Stand-Objekt; Code
`engine/src/zellschub_r36.c`) sind zwei Riegel rot, die vorher gruen waren:
- `unit_r31_generator` Teil D: "Zwischenschalter 7, Fahrt + Ruhe (49 Bilder) HOCH gehalten: dz=6548,
  Stillstandsbilder 15, Sperrbilder 0, Zeiger 20" -> [FAIL] "nach einem Zwischenschalter bewegt sich der Cursor in
  jedem Bild (keine Sperre)".
- `unit_r34n_c_generator` Teil A: [FAIL] "in JEDEM Bild der Fahrt und der 30 Ruhebilder bewegt sich der Cursor".
FRAGE: Schiebt das ORIGINAL den Generator-Cursor auch aus Zellen (dann pinnten die Riegel eine Port-Annahme) oder nicht
(dann fehlt dem Port eine Bedingung)? Weitere Cursor-/Panel-Raeume pruefen (Zensus).

## Kurzantwort

**Das Original schiebt den Cursor genauso.** ROOM11F0 hat in JEDEM der fuenf SCA-Bereiche vier Klasse-2-Zellen, die
einen RAHMEN um das Schalterfeld bilden (Band 0, `u0 0xff`): unten z 14150..15150, links x -31700..-30700, rechts
x -8750..-7750, oben z 30150..31150 (RDT @0x00910..0x00934 fuer Bereich 0; Bereich 1 @0x009DC..0x00A00). Der Cursor
obj 0 (Obj_model_set @0x00E54, `+0 = 0x0101`: Bit 3 NICHT gesetzt) wird vom Ruhe-Zweig in jedem Bild aus diesen Zellen
geschoben und steht an der Rahmenkante — am Original gemessen (DuckStation + GDB, `G2_belege/orig_cursor_lauf1/log.txt`):
OBEN z 29232, UNTEN z 16068, LINKS x -29782, RECHTS x -9668; der Port (nach Spur R) liefert dieselben Zahlen
(z 29232 in `unit_r31_generator` D, z 16068 in `unit_r34n_c_generator` A). Die beiden Riegel pinnten die Port-Annahme
"der Cursor faehrt unbegrenzt" — die galt nur, solange dem Port der Ruhe-Zweig fehlte (vor Spur R lief der Cursor
ueber den Rahmen hinaus aus dem Schalterfeld). Folge: Riegel mit Original-Beleg anpassen, kein Code-Fix.

## Stand (Arbeitsprotokoll)

- [x] P1 Messung im Port (RE15_PUSH_LOG an beiden roten Riegeln)
- [x] P2 Original: statisch (FUN_8002bd44 / Installer / FUN_8003b558) + dynamisch (GDB, vier Richtungen)
- [ ] Zensus der Cursor-/Panel-Raeume (statisch alle RDTs + Port-Laeufe mit RE15_PUSH_LOG)
- [ ] P3 Riegel anpassen (mit Original-Beleg), Rueckbau-Nachweis
- [ ] P4 Regression gezielt, LOCAL-BUILD-OK

## P1 — Messung im Port

Bau: `local_build.sh configure` + `build` im Baum (`=== LOCAL-BUILD-OK (build)`), Stand 7b2e56d9.
Lauf: `RE15_PUSH_LOG=<datei> tests/unit/r31_generator.exe` bzw. `probe_r34n_c_generator.exe` (Messschrieb
`dachkiste_r36.c re15_dachkiste_ruhe_log`, eine Zeile je Bewegung durch den Ruhe-Zweig; `F0` = die Sonde zaehlt
`g_engine.frame_count` nicht).

`unit_r31_generator` Teil D (Schalter 7, dann 49 Bilder HOCH ab der Startlage (-19554,22684)):
```
F0 room=11F0 p=1 obj=0 RUHE-AUSSCHIEBUNG lage=(-19554,29284) -> (-19554,29232) band=0 y=0
  ZELLE [33]: x -31700..-7750 z 30150..31150 r=900 -> lage (-19554,29232)
F0 room=11F0 p=1 obj=0 RUHE-AUSSCHIEBUNG lage=(-19554,29432) -> (-19554,29232) band=0 y=0
  ZELLE [33]: x -31700..-7750 z 30150..31150 r=900 -> lage (-19554,29232)
... (insgesamt 16 Zeilen: 1x 29284 -> 29232, 15x 29432 -> 29232)
```
ZELLE [33] = Bereich 1 (Start 17) + 16 = r1[16] = Rahmen OBEN. Ausgabe der Sonde: `dz=6548` = 29232 - 22684,
`Stillstandsbilder 15`, `Sperrbilder 0` (die Panel-Sperre war in keinem Bild an — der Cursor steht am Rahmen, nicht
an einer Sperre).

`unit_r34n_c_generator` Teil A (Cursor in die Zelle von Schalter 7 (-18554,24484), Quadrat, dann UNTEN gehalten):
31 Zeilen `RUHE-AUSSCHIEBUNG`, 1x `lage=(-18554,15884) -> (-18554,16068)`, 30x `(-18554,15868) -> (-18554,16068)`
(Rahmen UNTEN r1[13] z 14150..15150); Sonde: `51 Fahrt+Ruhe-Bilder: Cursor stand 21-mal, Sperre 0-mal`.

Ohne Ruhe-Zweig (Stand vor dem Merge der Spur R, Integration 645/645): beide Riegel gruen, d. h. der Cursor fuhr in
allen 49 bzw. 51 Bildern weiter — in D bis z = 22684 + 48*200 = 32284, also UEBER die Rahmenzelle r1[16]
(z 30150..31150) hinaus aus dem Schalterfeld. (Rueckbau-Nachweis mit Zahlen: Abschnitt P3.)

## P2 — Original

### P2.1 Statisch: nichts im Original nimmt den Cursor vom Ruhe-Zweig aus

* **Hauptschleife:** `8001cdec jal 0x8003f038` (SCD, dort sub01/sub02 = Cursor-Fahrt) ... `8001ce04 jal 0x8001a50c`
  (Gegner) / `8001ce0c jal 0x80031c44` (Spieler) / **`8001ce14 jal 0x8002bd44`** (Objekt-Pass) — unbedingt (die
  Verzweigungen @0x8001cd08/@0x8001cd20/@0x8001cd38 laufen alle auf 0x8001cdec), keine Pausen-Abfrage davor.
  Die Spieler-/KI-Pause, die sub01 im Raetsel setzt (@0x012AC `22 02 00 01` / @0x012B0 `22 02 02 01`), wird im
  Objekt-Pass nicht gelesen.
* **Objekt-Pass FUN_8002bd44** (`dis 0x8002bd44 280`): Schleife `8002bdcc lw v0,0(s2) / andi v0,v0,0x1 / beq ->
  0x8002c150` (nur aktiv). Typ-Handler `8002bde0 lbu v0,8(s2)` -> Tabelle @0x80073c70 (`table`): [4] = 0x8002c40c
  (`lbu v1,4(a0)` = Fall-Zustand, beim Cursor 0 -> nichts). Szenen-Bit `8002bec0 andi v0,v0,0x200` (Cursor: 0).
  Stand-Objekt `8002bef0 lw v0,0(s6)` / `8002bef8 bne s2,v0` (DAT_800ac788; Spielstand: 0). Schubkette
  `8002bf14 lbu v1,8(s2)` Typ 4 -> `8002bf24 lw v0,-32(s6)` (0x800ac768 & 1 = HOCH gehalten — im Raetsel JA) ->
  `8002bf38 lbu v1,846(s6) / lbu v0,130(s2) / bne` (Spieler-Band 0 = Cursor-Band 0) -> `8002bf50 jal 0x8002d1e8`
  Reichweite 900 um den Punkt 563 vor dem Spieler: der Spieler steht bei (250,250) bzw. am Panel x >= -1900, der Cursor
  bei x <= -9668 -> 0 -> `8002bf74 sb zero,140(s2)` -> `8002bf8c lbu v1,140(s2) / ori v0,zero,0x9 / bne v1,v0,
  0x8002c0b8` -> **LAB_8002c0b8**: `8002c0b8 ori a1,zero,0x2` / `8002c0c4 sw v0,52(s2)` / **`8002c0d0 jal 0x8003b558`**
  / `8002c0d4 sw v0,60(s2)`. Danach Spiegel `8002c0ec..8002c10c` (lhu 52/56/60 -> sh 64/66/68).
* **Installer LAB_80040914** (`dis 0x80040914 115`): `80040990 lhu v0,6(a2) / 80040998 ori v0,v0,0x1 /
  800409a4 sw v0,0(a1)` -> obj+0 = pc[6..7] | 1. Cursor @0x00E54 `2d 00 04 00 00 00 00 01 ...`: pc[6..7] = `00 01` =
  0x0100 -> **obj+0 = 0x0101**. Typ `8004095c sb a0,8(a1)` = pc[2] = 4, Band `80040974 sb v0,130(a1)` = pc[4] = 0,
  Box `800409f4 addiu v0,a2,22 / 800409f8 sw v0,120(a1)` = pc+22 = `00 00 7c fc 00 00 84 03 84 03 84 03` =
  c(0,-900,0) h(900,900,900).
* **FUN_8003b558** (`dis 0x8003b558 105`): `8003b668 lw v0,0(s1) / 8003b670 andi v0,v0,0x8 / 8003b674 bne v0,zero,
  0x8003b7a8` — NUR Bit 3 nimmt ein Objekt von allen Zellen aus. Cursor 0x0101: Bit 3 = 0 -> Zellen werden geprueft.
  (Gegenprobe im selben Raum: die zehn Schalter obj 2..11 und obj 1 tragen pc[6..7] = `0a 00` -> obj+0 = 0x000b,
  Bit 3 gesetzt -> nie ausgeschoben. Das Original unterscheidet also bewusst: Hebel ausgenommen, Cursor nicht.)
  Band `8003b634 lhu v0,8(s0) / sll 16 / sra 28` == `8003b638 lbu v1,130(s1)`: Rahmenzellen `fl 0x03` -> Nibble 0 ==
  Cursor-Band 0. Klasse `8003b64c lhu v0,6(s0) / sll 16 / sra 24 / and s6` (a1 = 2): `u0 0xff` -> (s8)-1 & 2 = 2.
  Quadrat (hx == hz = 900) -> r = hx (`8003b690 beq v1,v0,0x8003b738`). Treffer -> `8003b79c jal 0x8003da78`.
* **FUN_8003da78** (Belege R_dachkiste.md, Nachbesserung 3): Rahmen OBEN r1[16] z 30150..31150, Cursor z 29284:
  a3 = min-Betrag((31150+900)-29284+18, (30150-900)-29284-18) = -52; t0 (x) = 12722; Lagecode dx = 0 -> Bit0 = 0,
  dz = +200 -> a3^dz < 0 -> Bit1 = 1 -> Code 2 (`8003db40 beq v1,v0,0x8003dba0`), |a3|+400 = 452 < 0x321
  (`8003dbac-b8`) -> `sw v0,60(a1)` z = 29284 - 52 = **29232**.

### P2.2 Dynamisch: DuckStation + GDB (vier Richtungen)

Spielstand `G2_belege/room11f0_orig.sav` — erzeugt mit `G2_belege/orig_11f0_spielstand.py`: mzd_debugmenu.sav laden,
GDB-Stopp, Menue-Zustand gesetzt (Funktion @0x80014444: Zeile 0x800BBE5D = 1 JUMP, Stage 0x800BBE5E = 0, Raumindex
0x800BBE5F := 0x1F; vorher 0x24 = JUMP 124), Quadrat, Pruefung ueber GDB (SCA-Zellen im RAM == ROOM11F0.RDT), geordnet
geschlossen (SaveStateOnExit). Werkzeug-Befund: re15_quickload.py `--left 5` landete zweimal in ROOM1240 (der virtuelle
Pad wurde SDL-1, Spur R `pad_bind_alle`; und 0,3 s Links = -3 Raeume per Auto-Repeat, gemessen `diag`).
Pruefung `G2_belege/savestate_11f0_pruefen.txt`: EXE-Stub `0800e003`, RDT-Basis 0x801219b0, **SCD 0x00CC4..0x01810
und alle 85 SCA-Zellen im RAM == ROOM11F0.RDT**, Pool 0 = Cursor `+0=0101 typ 4 band 0 lage (-19554,0,22684) box@
RDT+0x00E6A (0,-900,0,900,900,900)`, obj 1..11 `+0=000b`, 4:238 = 0, Stand-Objekt DAT_800ac788 = 0.

Messung `G2_belege/orig_11f0_cursor.py` -> `G2_belege/orig_cursor_lauf1/log.txt`: Raetsel scharf wie sub16
@0x015C2..0x015F2 (Bank 5 = 0x800b1028 |= 0xFFF80000; Tabelle @0x80074664[5], Set-Handler @0x8003fdd0-fe0c), dann je
Richtung die Taste halten; Haltepunkte @0x8002bd44 (Eintritt Objekt-Pass = Lage nach der SCD), @0x8003da78
(Ausschiebung, a1 = Cursor), @0x8002c158 (Austritt). Auszug OBEN:
```
B032 OBEN pad=0011 spieler=(250,250) nach-SCD (-19554,29084) -> nach-Pass (-19554,29084) spiegel (-19554,29084)
B033 OBEN pad=0011 spieler=(250,250) nach-SCD (-19554,29284) -> nach-Pass (-19554,29232) spiegel (-19554,29232)
      TREFFER Zelle@801223b0 x -31700..-7750 z 30150..31150 u0 0xff fl 0x03 r=900 vor (-19554,29284) spiegel (-19554,29084)
B034 OBEN pad=0011 spieler=(250,250) nach-SCD (-19554,29432) -> nach-Pass (-19554,29232) spiegel (-19554,29232)
      TREFFER Zelle@801223b0 x -31700..-7750 z 30150..31150 u0 0xff fl 0x03 r=900 vor (-19554,29432) spiegel (-19554,29232)
... B035..B048 identisch (15 Bilder Stillstand an der Rahmenkante)
```
Zelle@0x801223b0 = SCA-Bereich 1 (Zeiger 0x801222f0) + 16*12 = r1[16]. Die SCD faehrt den Cursor in jedem Bild +200
(B001 22884 ... B033 29284), ab B034 schiebt FUN_8003da78 jedes Bild 29432 -> 29232 zurueck. Der Spieler steht die
ganze Zeit bei (250,250) (Pause aus sub01). **33 Bilder Fahrt + 15 Bilder Stillstand = dieselbe Bilanz wie der Port-
Riegel D (`dz=6548` = 29232-22684, `Stillstandsbilder 15`).**

| Richtung | erster Treffer | Zelle (RAM) | Endlage Original | Port (nach Spur R) |
|---|---|---|---|---|
| OBEN   | B033 nach-SCD z 29284 | @0x801223b0 r1[16] x -31700..-7750 z 30150..31150 | (-19554, **29232**) | 29232 (r31 D) |
| UNTEN  | B114 nach-SCD z 16032 | @0x8012238c r1[13] x -31700..-7750 z 14150..15150 | (-19554, **16068**) | 16068 (r34n A) |
| LINKS  | B181 nach-SCD x -29954 | @0x80122398 r1[14] x -31700..-30700 z 14150..31150 | (**-29782**, 16068) | s. Zensus |
| RECHTS | B297 nach-SCD x -9582 | @0x801222d8 r0[15] x -8750..-7750 z 14150..31150 | (**-9668**, 16068) | s. Zensus |

(RECHTS liegt jenseits ceil_x -9648 im Quadranten 0 — FUN_8003b068 `8003b5ec jal` — dort dieselben Rahmenzellen.)
64 TREFFER insgesamt, alle mit `a1 = 0x800b3f98` (Cursor), r = 900. settings.ini danach zurueck (`EnableGDBServer =
false`, Datei-Inhalt verglichen).

**Ergebnis P2:** Das Original schiebt den Generator-Cursor in jedem Bild aus den vier Rahmenzellen; er steht an der
Rahmenkante. Die roten Riegel pinnten eine Port-Annahme.
