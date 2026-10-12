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

## Zensus — Cursor-/Panel-Raeume (wer kann ueberhaupt geschoben werden, wer wird es waehrend der Fahrt)

### Z1 Statisch, alle 206 RDTs (`G2_belege/zensus_objekte_ausschiebung.py` -> `.txt`)

Regel: FUN_8003b558 ueberspringt JEDE Zelle fuer Objekte mit obj+0 & 8 (`8003b668 lw v0,0(s1) / 8003b670 andi
v0,v0,0x8 / 8003b674 bne`), obj+0 = pc[6..7] | 1 (`80040990-9a4`). Ergebnis: 672 Obj_model_set, **99 ohne Bit 3**, davon
30 vom SCD bewegt (Work_set(3,k) + Add_speed/Pos_set im selben Skript):
* **28 Panel-Cursor** — immer dasselbe Objekt: Typ 4, Band 0, `+0 = 0x0101`, Box c(0,-900,0) h(900,900,900), Fahrt
  +-200 je Bild: ROOM1080/1081 (Fahrstuhl-Panel), 10D0/10D1, 1100/1101, 11E0/11E1 (zwei Cursor, obj 1 und 2),
  11F0/11F1 (Generator), 1230/1231, 2040/2041, 2050/2051 (obj 8), 2060/2061 (zwei: obj 3 und 4), 3050/3051 (obj 3),
  30E0, 4020/4021 (Fahrstuhl), 5050/5051. (1081/1101/11E1/11F1/1231 byte-gleich mit ...0, `cmp`; 10D1 nicht.)
* ROOM3091 obj 1 (Typ 4, Band 1, `+0 = 0x0081`, Box h(0,0,0)): Add_speed @0x01E66, Pos_set @0x01786/@0x01E4C.
* Start-Treffer (Objekt beruehrt beim Installieren eine Klasse-2-Zelle seines Bands): nur ROOM2060/2061 obj 2 (der
  Tritt der Spur R, r0[9] @0x00738) und ROOM11A0/11A1 obj 0 im ELSE-Zweig @0x0107E (Ck(5,6)==0 @0x01040 sonst): Bank 5
  Wort 0 wird beim Raumaufbau geloescht (FUN_8003ecec `8003ed74 sw zero,4136(at)` = 0x800b1028), main00 sieht also nie
  5:6 = 1 — der Zweig ist beim Raumeintritt nicht erreichbar. Die Spur-R-Aussage "nur 2060/2061 obj 2 im ersten Bild
  bewegt" bleibt gueltig.
* Die genannten Nachbarn: **ROOM1150/1151 (Hebetisch)** — Tisch/Deckel obj 0..2 tragen `0a 00` -> `+0 = 0x000b`
  (Bit 3), werden nie geschoben; obj 3 (`+0 = 0x0001`) wird vom SCD nicht bewegt und liegt in keiner Zelle.
  **ROOM4000 (Cursor-Raster)** — alle sieben Objekte `0a 00` -> Bit 3, kein Eintrag. **ROOM1080 (Fahrstuhl-Panel)** —
  Panel-Cursor mit Rahmen wie 11F0 (Z3).

### Z2 Port: welche Riegel sehen eine Ruhe-Ausschiebung? (`G2_belege/port_ruhe_zensus.py`, Stand 7b2e56d9)

99 Tests (Regex des Auftrags + 4020|10d0|1100|11e0|1230|2040|2050|2060|3050|30e0|5050|codepanel|espr_11e0|elevator|
fahrstuhl), jeder EINZELN mit eigenem RE15_PUSH_LOG. Bewegungen durch den Ruhe-Zweig gab es nur in:

| Test | Ruhe-Bewegungen | Objekt -> Endlage |
|---|---|---|
| unit_r31_generator (alte Fassung, rot) | 17 | ROOM11F0 obj 0 -> (-19554,29232) |
| unit_r34n_c_generator (alte Fassung, rot) | 31 | ROOM11F0 obj 0 -> (-18554,16068) |
| unit_r27_panel_schalterwerte (gruen) | 8 | ROOM11F0 obj 0 -> (-19554,29232) — Teil E faehrt 40 Bilder HOCH ab 22684 |
| unit_r17_cursor_klick_pin, r31_trefferboxen, unit_r36_dachkiste_zellschub (gruen) | je 1 | ROOM2060 obj 2 -> (-3143,2075) (Tritt, Spur R) |

Alle anderen Tests 0 Ruhe-Bewegungen, rc = 0 (96 von 99; `integration_r36_dachkiste` lief in die 900-s-Grenze des
Zensus-Laeufers — ctest gibt ihm 2700 s, s. P4). Die Riegel der Fahrstuhl-Panels (unit_fahrstuhl_1080_etagen,
probe_elevator_1080, unit_fahrstuhl_4020_etagen), des Hebetischs (unit_r31_hebetisch, unit_r32_hebetisch_faecher,
unit_r34n_b_*) und von 11E0/10D0/1230 fahren ihre Cursor/Objekte NIE an eine Rahmenzelle — darum blieben sie gruen.
Laufprotokolle: `G2_belege/port_ruhe_zensus_vorher.txt` (Zusammenfassung).

### Z3 Panel-Cursor: Port gegen Original, alle Raeume (Fahrt nachgebildet)

Port `probe_r36_g2_cursor_rahmen` (neu), Original `G2_belege/orig_cursor_zensus.py` (GDB). Beide bilden die Fahrt
gleich nach: je Bild +-200 auf obj+0x34/+0x3C NACH der SCD und VOR dem Objekt-Pass (Original: am Haltepunkt
@0x8002bd44; dieselben Felder wie Add_speed `80040f48 lw v1,52(a1) / 80040f54 sw v0,52(a1)`, `80040f70 lw v1,60(a1) /
80040f7c sw v0,60(a1)`), kein Pad (die Schubkette @0x8002bf24 greift nie), je Richtung bis 15 Bilder Stillstand.
Gegenprobe der Nachbildung: ROOM11F0 liefert damit am Original exakt dieselben Zahlen wie die ECHTE SCD-Fahrt
(orig_cursor_lauf1): 33/66/52/101 Bilder, Endlagen 29232/16068/-29782/-9668, gleiche erste Ausschiebungen.

Pruefung je Raum vor der Fahrt (`orig_zensus/log.txt`): EXE-Stub `0800e003`, alle SCA-Zellen im RAM == RDT-Datei bis auf
`u0`/`floor`, die Sca_id_set (0x37) / Sca_floor_set (0x39) beim Raumaufbau umsetzt (z. B. ROOM11E0 Zelle 0/22/44/66/88
u0 0x00 -> 0xff = sub00 @0x017B0..0x017C0 `37 0r 00 ff`; ROOM3050 Zellen 21/112/... u0+floor; der Port setzt dieselben
Opcodes um: scd_vm.c `op_sca_id_set` / `op_sca_floor_set`), Cursor-Pool-Platz = obj: `+0=0101 typ 4 band 0`. Cursor, die
das SCD nur in einem Else-Zweig installiert, mit Spielflag (Bank 3 = 0x800b0ff8, Tabelle @0x80074664[3]) vor dem
Laden — im Port dieselben Flags vor `scd_room_reenter`.

| Raum / obj | Flags | Rahmen | Original (log Zeile) | OBEN | UNTEN | LINKS | RECHTS | Port |
|---|---|---|---|---|---|---|---|---|
| 1080 obj 0 (Fahrstuhl) | — | A | Z.10 | 33 / z 29232 | 66 / z 16068 | 52 / x -29782 | 101 / x -9668 | = |
| 10D0 obj 1 | — | B | Z.18 | 34 / z -16568 | 66 / z -29732 | 52 / x -29782 | 101 / x -9668 | = |
| 10D1 obj 1 (Elza) | — | B | nicht ladbar (Leon-JUMP); SCA + Cursor-Bytes @0x0114E == 10D0 | | | | | = 10D0 |
| 1100 obj 0 | 3:103=1 | A | Z.106 | 33 / 29232 | 66 / 16068 | 52 / -29782 | 101 / -9668 | = |
| 11E0 obj 1 | 3:127=1 (3:139=0) | A | Z.135 | 33 / 29232 | 66 / 16068 | 52 / -29782 | 101 / -9668 | = |
| 11E0 obj 2 | 3:139=0 | C | Z.135 | 33 / z 29232 | 66 / z 16068 | 52 / x 9068 | 101 / x 29182 | = |
| 11F0 obj 0 (Generator) | — | A | Z.1 (+ echte SCD-Fahrt) | 33 / 29232 | 66 / 16068 | 52 / -29782 | 101 / -9668 | = |
| 1230 obj 0 | — | B | Z.34 | 34 / -16568 | 66 / -29732 | 52 / -29782 | 101 / -9668 | = |
| 2040 obj 0 | 3:146=1 | A | Z.115 | 33 / 29232 | 66 / 16068 | 52 / -29782 | 101 / -9668 | = |
| 2050 obj 8 | — | — | Z.178: Pool 8 `+0=0000` | nicht installiert (Ck(5,7)==1 @0x01D34, Bank 5 geleert @0x8003ed74) | | | | = (nicht da) |
| 2060 obj 3 | — | A | Z.48 | 33 / 29232 | 66 / 16068 | 52 / -29782 | 101 / -9668 | = |
| 2060 obj 4 | — | D | Z.48 | 33 / z -15968 | 66 / z -29132 | **51** / x 9418 | 101 / x 29532 | = |
| 3050 obj 3 | 3:178=1 | A | Z.154 | 33 / 29232 | 66 / 16068 | 52 / -29782 | 101 / -9668 | = |
| 30E0 obj 0 | — | A | Z.64 | 33 / 29232 | 66 / 16068 | 52 / -29782 | 101 / -9668 | = |
| 4020 obj 0 (Fahrstuhl) | — | A | Z.72 | 33 / 29232 | 66 / 16068 | 52 / -29782 | 101 / -9668 | = |
| 5050 obj 0 | — | A | Z.80 | 33 / 29232 | 66 / 16068 | 52 / -29782 | 101 / -9668 | = |

Rahmen A: x -31700..-7750 z 14150..31150; B: x -31700..-7750 z -31650..-14650; C: x 7150..31100 z 14150..31150; D: x
7500..31450 z -31050..-14050 (jeweils vier Klasse-2-Zellen Band 0, Zelladressen je Treffer im Log). Je Richtung stimmen
auch die ersten Ausschiebungen ueberein (z. B. 2060 obj 4 LINKS: Original `(9371,-29132) -> (9418,-29132)`, Port dito).
**Ergebnis Zensus: jeder Panel-Cursor des Spiels wird im Original UND im Port beim Erreichen seines Rahmens in jedem Bild
ausgeschoben; die Werte sind bitgleich (15 Cursor, 60 Richtungen). Kein weiterer Raum hat eine Port-Abweichung.** Im
Spielverlauf tritt der Schub genau dann auf, wenn der Spieler den Cursor an den Feldrand faehrt — dort bleibt er stehen
(Original-Verhalten). Hebetisch 1150 und Cursor-Raster 4000 haben keine schiebbaren Objekte (Z1).

## P3 — Riegel mit Original-Beleg angepasst (kein Code-Fix)

Der Port-Code (zellschub_r36.c, aot_common.c, game_step_common.c) bleibt unveraendert: er tut, was das Original tut.
Angepasst sind die Riegel, die "der Cursor faehrt unbegrenzt" angenommen hatten; ihre eigentliche Aussage (nach einem
Zwischenschalter KEINE Eingabesperre — Nutzer-Vorgabe Runde 34 Nacht "man soll sich frei bewegen koennen Ausser ganz am
Ende") bleibt erhalten und wird jetzt im Schalterfeld geprueft:

* `tests/unit/r31_generator.c`
  - Konstanten `ORIG_RAHMEN_*`, `ORIG_FAHRT_*`, `ORIG_START_*`, `CURSOR_SCHRITT` als Zahlen mit Log-Zeile / Byte-Offset
    (nicht aus dem Port gerechnet — Lehre Runde 31, selbstbestaetigende Metrik).
  - Teil D(1): vorher UNTEN an den Rahmen (Original z 16068), dann Schalter 7 und 49 Bilder HOCH: jedes Bild +200,
    dz = 9600, 0 Sperrbilder, Zeiger 20.
  - Teil D(1b) neu: weiter HOCH bis zum Stand: z = 29232 (Original B033), >= 15 Bilder Stand, 0 Sperrbilder, kein Bild
    jenseits des Rahmens. Danach 20 Bilder UNTEN, damit "Cursor steht trotz HOCH" in D(2) nicht am Rahmen gilt.
  - Teil G neu: dieselbe Fahrt wie die Original-Messung mit der ECHTEN Eingabe (sub02..05): OBEN 33 / UNTEN 66 / LINKS 52 /
    RECHTS 101 Fahrbilder, Endlagen wie das Original, keine Sperre, kein Schalter umgelegt.
* `tests/unit/probe_r34n_c_generator.c` Teil A: 30 Bilder UNTEN, ab Bild 30 OBEN (ab der Zelle von Schalter 7 reicht eine
  Richtung nur 42 bzw. 23 Bilder bis zum Rahmen); neu: Cursor bleibt in z 16068..29232.
* `tests/unit/probe_r36_g2_cursor_rahmen.c` + `probes/r36_g2_generator.cmake` neu: ctest `unit_r36_g2_cursor_rahmen`
  (Argument `pin`) — Tabelle Z3 als Riegel (91 Pruefungen).

Messung nachher (Bau `LOCAL-BUILD-OK`): `r31_generator` ALLES GRUEN — D: `Vorlauf UNTEN: Cursor bei (-19554,16068)`,
`dz=9600, Stillstandsbilder 0, Sperrbilder 0, Zeiger 20`, `weiter HOCH: Cursor steht bei z=29232 (15 Bilder Stillstand),
Sperrbilder 0, ueber dem Rahmen 0`; G: `OBEN start (-19554,22684) bewegt 33 ende (-19554,29232) Sperrbilder 0 | Original:
bewegt 33 ende (-19554,29232)` (UNTEN/LINKS/RECHTS dito). `probe_r34n_c_generator` ALLES GRUEN — A: `Schalterbit im Bild
15, Zeiger 20 im Bild 34; 51 Fahrt+Ruhe-Bilder: Cursor stand 0-mal, Sperre 0-mal`. `probe_r36_g2_cursor_rahmen pin`
ALLES GRUEN (91 OK).

**Rueckbau-Nachweis** (`G2_belege/mutation.sh`, je Mutation `local_build.sh build`, drei Riegel gefahren, Datei per
`git checkout` zurueck, am Ende sauberer Bau `LOCAL-BUILD-OK`; Protokoll `G2_belege/mutation.log`):

| Mutation | r31_generator | probe_r34n_c_generator | unit_r36_g2_cursor_rahmen |
|---|---|---|---|
| g1 Ruhe-Zweig aus (`re15_zellschub_ruhe` kehrt sofort zurueck = Stand vor Spur R) | 7 FAIL (D-Vorlauf, D(1b) x2, G x4) | 0 FAIL (A bleibt im Feld, prueft die Sperre) | 60 FAIL (alle Richtungen aller Cursor) |
| g2 alte Runde-31-Zwischensperre wieder an | 3 FAIL (D(1): Cursor steht, dz, Sperrbilder) | 7 FAIL (A, B, C, D, G) | 0 FAIL |
| g3 Endsperre aus (`sperrt()` = 0) | 4 FAIL (D(2) inkl. "Cursor steht trotz HOCH", B k+31) | 12 FAIL (B 11F0/11F1) | 0 FAIL |

Jeder Riegel faellt genau auf seine Aussage: der Rahmen (g1) nur in den neuen Rahmen-Pruefungen, die Zwischensperre (g2)
und die Endsperre (g3) in den Sperr-Pruefungen; D(2) "Cursor steht trotz HOCH" bleibt wirksam (g3 faellt dort), weil
der Cursor vor D(2) 20 Bilder vom Rahmen weggefahren wird.
