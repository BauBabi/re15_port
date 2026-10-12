# Runde 36 — G2 "generator_ruhezweig": Abnahme 0 (unabhaengig)

Datum 2026-10-12. Baum `.claude/worktrees/r36_generator`, Zweig `r36/generator`, HEAD af3e040e (8 Commits auf
r36/integration 7b2e56d9). Geprueft wurde der committete Endstand; am Code nichts geaendert. Eigene Messskripte und
Rohprotokolle liegen nur im Scratchpad dieser Sitzung (nicht versioniert, Auftrag: nur diesen Bericht committen); die
tragenden Zeilen stehen unten woertlich.

**Urteil: BESTANDEN.** P1 bis P4 sind erfuellt. Die Kernaussage — das ORIGINAL schiebt den Generator-Cursor in jedem Bild
aus den vier Rahmenzellen des Schalterfelds, die beiden roten Riegel hatten eine Port-Annahme ("der Cursor faehrt
unbegrenzt") festgeschrieben — habe ich statisch (eigenes Disassemblat) und dynamisch (eigene GDB-Messung am Original,
eigenes Skript, echte Pad-Eingabe) bestaetigt. Zwei Zaehlfehler im Dossier (Abschnitt 7) aendern am Ergebnis nichts.

## 0. Gegenstand und Diff

`git diff --stat 7b2e56d9..HEAD`: 24 Dateien, **keine** unter `re15_port/engine`, `include` oder `platform` — der
Engine-Code ist der der Integration (Spur R eingeschlossen). Geaendert bzw. neu:

- `tests/unit/r31_generator.c` — D(1): erst UNTEN an den Rahmen (z 16068), dann Schalter 7 und 49 Bilder HOCH, neu
  `dz == 48*200`; D(1b) neu: weiter HOCH bis Stand bei z 29232, >= 15 Bilder Stand, 0 Sperrbilder, nie jenseits des
  Rahmens; vor D(2) 20 Bilder UNTEN (damit "Cursor steht trotz HOCH" nicht schon am Rahmen gilt); Teil G neu (OBEN/UNTEN/
  LINKS/RECHTS mit echter Eingabe, Fahrbilder 33/66/52/101 und Endlagen = Original). Konstanten `ORIG_*` mit Log-Zeile
  und Zellen-/Funktionsadresse, `CURSOR_SCHRITT 200` = sub02 @0x012F6 `2f 02 c8 00`, `ORIG_START_*` = Obj_model_set
  @0x00E54 pc[10..11]/[14..15].
- `tests/unit/probe_r34n_c_generator.c` Teil A: 30 Bilder UNTEN, ab Bild 30 OBEN (eine Richtung allein laeuft ab der
  Zelle von Schalter 7 an den Rahmen); neue Pruefung "Cursor blieb in z 16068..29232".
- `tests/unit/probe_r36_g2_cursor_rahmen.c` + `probes/r36_g2_generator.cmake`: neuer Riegel `unit_r36_g2_cursor_rahmen`
  (Argument `pin`, 91 Pruefungen; SOLL-Tabellen A/B/C/D aus dem Original-Zensus).
- `analysis/befunde_runde36/G2_generator_ruhezweig.md` + `G2_belege/` (Spielstand, GDB-Skripte, Logs, Rueckbau).

Abschluss-Commit `af3e040e fix(r36-generator): Generator-Riegel an den Original-Rahmen des Schalterfelds angepasst
(kein Code-Fix)` ist da; die Message nennt die Belegadressen (@0x8001ce14, @0x80040990-9a4, @0x8003b668-674,
@0x8002bf50, @0x8002bf94, @0x8002c0d0, @0x8003b79c, @0x8003dab0/@0x8003dab8, RDT @0x009DC..0x00A00) und die Summenzeile.

## 1. Bau (selbst)

| Lauf | Ergebnis |
|---|---|
| `bash re15_port/tools/local_build.sh configure` + `build` im Baum (`re15_port/build`) | `=== LOCAL-BUILD-OK (configure)`; `ninja: no work to do.` `=== LOCAL-BUILD-OK (build)` |
| frischer eigener Baum: `RE15_BUILD_DIR=re15_port/build_abn0`, `local_build.sh configure` + `build` | `[1880/1880]`, `=== LOCAL-BUILD-OK (configure)` / `=== LOCAL-BUILD-OK (build)` |

Alle Riegel- und Regressionslaeufe unten laufen aus dem FRISCHEN Baum `build_abn0` (kein Altobjekt moeglich).

## 2. P1 — Port selbst gemessen (RE15_PUSH_LOG, mit und ohne Ruhe-Zweig)

Die ALTEN Riegel (`git show 7b2e56d9:re15_port/tests/unit/...`) habe ich gegen die frisch gebaute Engine gelinkt — einmal
unveraendert (= Integration mit Ruhe-Zweig), einmal mit einer eigenen Kopie von `zellschub_r36.c`, in der
`re15_zellschub_ruhe` sofort zurueckkehrt (= Stand vor Spur R). Objekte/Exes nur im Scratchpad, Arbeitsbaum unberuehrt.

| Riegel (Stand 7b2e56d9) | MIT Ruhe-Zweig | OHNE Ruhe-Zweig |
|---|---|---|
| r31_generator Teil D | rc 1, genau 1 FAIL `nach einem Zwischenschalter bewegt sich der Cursor in jedem Bild (keine Sperre)`; `dz=6548, Stillstandsbilder 15, Sperrbilder 0, Zeiger 20`; 17x RUHE-AUSSCHIEBUNG: 1x `(-19554,29284) -> (-19554,29232)`, 16x `(-19554,29432) -> (-19554,29232)`, alle `ZELLE [33]: x -31700..-7750 z 30150..31150 r=900` | rc 0, `dz=9600, Stillstandsbilder 0` (Cursor endet bei 22684 + 9600 = 32284, jenseits der Rahmenzelle z 30150..31150), 0 RUHE-Zeilen |
| probe_r34n_c_generator Teil A | rc 1, genau 1 FAIL `in JEDEM Bild der Fahrt und der 30 Ruhebilder bewegt sich der Cursor`; `Cursor stand 21-mal, Sperre 0-mal`; 31x RUHE: 1x `(-18554,15884) -> (-18554,16068)`, 30x `(-18554,15868) -> (-18554,16068)`, alle `ZELLE [30]: x -31700..-7750 z 14150..15150` | rc 0, `Cursor stand 0-mal`, 0 RUHE-Zeilen |

ZELLE [33] = Bereich 1 (Start 17) + 16 = r1[16] (Rahmen OBEN), [30] = r1[13] (Rahmen UNTEN). Der Befund des Orchestrators
ist damit exakt reproduziert, die Ursache ist allein der Ruhe-Zweig, die Panel-Sperre war in keinem Bild an. Die Zahlen
decken sich mit Dossier P1. Neuer Riegel mit PUSH_LOG: `r31_generator` (build_abn0) ALLES GRUEN, 172 RUHE-Zeilen, u. a.
`ZELLE [30]` im D-Vorlauf und `ZELLE [33]` in D(1b)/G.

## 3. P2 — Original selbst nachgeprueft

### 3.1 Statisch (eigenes Disassemblat, `re15_disasm.py` auf `info/Re1.5/PSX.EXE`; `bytes 0x80026e4c` = `08 00 e0 03` = Auslieferung)

| Stelle | selbst gelesen | Bedeutung fuer den Cursor |
|---|---|---|
| Hauptschleife | `8001cdec jal 0x8003f038` (SCD) ... `8001ce04 jal 0x8001a50c`, `8001ce0c jal 0x80031c44`, `8001ce14 jal 0x8002bd44`; der Block ab 0x8001cd00 endet in jedem Zweig bei 0x8001cdec (`8001cd08/cd20/cda4/cdb8` springen direkt hin, `cd38/cd40/cd74` ueber cd64/cd94 dorthin) | Objekt-Pass laeuft NACH der SCD (Add_speed); dass er im Raetsel jedes Bild laeuft, zeigt die GDB-Messung (3.3) |
| FUN_8002bd44 Schleife | `8002bdcc lw v0,0(s2) / andi v0,v0,0x1 / beq -> 0x8002c150` | Cursor aktiv (0x0101 & 1) |
| Typ-Handler | `8002bde0 lbu v0,8(s2)` -> Tabelle @0x80073c70[4] = 0x8002c40c: `8002c414 lbu v1,4(a0) / 8002c41c beq v1,zero,0x8002c434` (nur obj+4 == 1 ruft FUN_8002c444) | beim Cursor (obj+4 = 0) nichts |
| Szenen-Bit | `8002bec0 andi v0,v0,0x200 / beq -> 0x8002beec` | 0x0101 & 0x200 = 0 |
| Stand-Objekt | `8002bef0 lw v0,0(s6)` (s6 = 0x800aca3c - 692 = DAT_800ac788) `/ 8002bef8 bne s2,v0,0x8002bf14` | Cursor ist nie das Stand-Objekt |
| Schubkette | `8002bf14 lbu v1,8(s2)` Typ 4 -> `8002bf24 lw v0,-32(s6)` (0x800ac768) `andi 0x1 / beq -> 0x8002bf74`; sonst `8002bf38/3c` Band, `8002bf50 jal 0x8002d1e8` (Reichweite) `/ 8002bf58 beq v0,zero,0x8002bf74`; `8002bf74 sb zero,140(s2)` | ohne HOCH oder ausser Reichweite: Zaehler obj+0x8C = 0 |
| Abzweig | `8002bf80 andi v0,v0,0x80` (0); `8002bf8c lbu v1,140(s2) / ori v0,zero,0x9 / 8002bf94 bne v1,v0,0x8002c0b8` | Zaehler != 9 -> LAB_8002c0b8 (Schub-Pfad @0x8002bf9c..bfb0 nur bei Zaehler 9) |
| Ruhe-Zweig | `8002c0b8 ori a1,zero,0x2`, `8002c0c4 sw v0,52(s2)`, **`8002c0d0 jal 0x8003b558`**, `8002c0d4 sw v0,60(s2)` | FUN_8003b558(Cursor, 2) jedes Bild |
| Installer LAB_80040914 | `80040978 lw v0,0(a1) / 80040980 beq v0,zero,0x80040990`; `80040990 lhu v0,6(a2) / 80040998 ori v0,v0,0x1 / 800409a4 sw v0,0(a1)`; Box `800409f4 addiu v0,a2,22 / 800409f8 sw v0,120(a1)` | obj+0 = pc[6..7] \| 1 |
| ROOM11F0.RDT @0x0E54 | `2d 00 04 00 00 00 00 01 00 00 9e b3 00 00 9c 58 ...`, @0x0E6A Box `00 00 7c fc 00 00 84 03 84 03 84 03` (identisch in `shared_assets/PSX`, `cmp`) | Typ 4, Band 0, pc[6..7] = 0x0100 -> **obj+0 = 0x0101 (Bit 3 frei)**, Start (-19554,22684), Box c(0,-900,0) h(900,900,900) |
| FUN_8003b558 | Band `8003b634 lhu v0,8(s0) / sll 16 / sra 28` == `8003b638 lbu v1,130(s1)`; Klasse `8003b64c lhu v0,6(s0) / sll 16 / sra 24 / and v0,s6,v0 / beq`; **`8003b668 lw v0,0(s1) / 8003b670 andi v0,v0,0x8 / 8003b674 bne v0,zero,0x8003b7a8`**; `8003b684/688 lhu 6/10(a3) / 8003b690 beq` (hx == hz -> r = hx); halboffene Tests @0x8003b738-794; **`8003b79c jal 0x8003da78`** (a0 = Zelle im Delay-Slot @0x8003b798, a1 = s1 im Delay-Slot @0x8003b7a0) | NUR Bit 3 nimmt ein Objekt von allen Zellen aus; Cursor wird geprueft |
| Rahmenzellen ROOM11F0 (Bereich 1, RDT @0x09DC/0x09E8/0x09F4/0x0A00, je 12 B) | `8e5d e803 2c84 4637 01ff 0003` = x -31700..-7750 z 14150..15150; `e803 6842 2c84 4637 01ff 0003` = x -31700..-30700 z 14150..31150; `e803 6842 d2dd 4637 01ff 0003` = x -8750..-7750 z 14150..31150; `8e5d e803 2c84 c675 01ff 0003` = x -31700..-7750 z 30150..31150 | `u0 0xff` -> (s8)-1 & 2 != 0 (Klasse 2), `fl 0x03` -> Bandnibble 0 = Cursor-Band |
| FUN_8003da78 | `8003da98-dab0` t0 = (x0+w+r) - (cx+x) + 18, `8003daa4/dab4-dab8` a0 = (x0-r) - (cx+x) - 18, `8003dabc-dacc` kleinerer Betrag; z dito `8003dad0-db10`; Lagecode `8003db14-db38` aus Spiegel obj+0x40/+0x44; Code 2 `8003db40 beq v1,v0,0x8003dba0`, `8003dbac-dbb8` (\|a3\|+400)&0xffff < 0x321, `8003dbbc/dbc4 sw v0,60(a1)` | nachgerechnet fuer B033: Zelle r1[16], r 900, Lage z 29284, Spiegel z 29084: a3 = min((31150+900)-29284+18, (30150-900)-29284-18) = **-52**, Code 2, 452 < 801 -> **z = 29232**; fuer z 29432: a3 = -200 -> 29232 |

### 3.2 Spielstand selbst geprueft (eigenes Skript `ss_check.py`, re15_ss.Ram)

`G2_belege/room11f0_orig.sav` (Kopie, sha1 1476ef32... gleich): EXE-Stub `0800e003`; Hauptschleife, FUN_8002bd44,
FUN_8003b558, FUN_8003da78, Installer im RAM == PSX.EXE; ganze Strecke 0x80010000..0x80070000: nur 9 Bytes anders ab
0x80013b7c — die patcht das Original selbst (`80013144 sw t0,0x1c(t1)=>LAB_80013b7c`, `8001314c sw zero,0x20(t1)=>
LAB_80013b80`, FUN_8001311c), kein Mod. RDT-Basis 0x801219b0, Obj_model_set @0x0E54 im RAM == Datei, alle 85 SCA-Zellen
== Datei, r1[13..16] @0x8012238c/398/3a4/3b0 wie oben; Cursor Pool 0 `+0=00000101 typ 4 band 0 (-19554,22684)`, Box
= RDT+0x0E6A; Stand-Objekt 0; Spieler (250,250) Band 0; Bank 5 Wort 0 = 0.

### 3.3 Dynamisch — EIGENE GDB-Messung (eigenes Skript `abn0_orig_messung.py`; nur der Transport `Gdb/ini_set/pad_bind_alle/pids` aus `R_belege/orig_1090_push.py`)

DuckStation (MZD-Disc), Spielstand-Kopie, `SaveStateOnExit` fuer den Lauf aus, settings.ini vorher gesichert und danach
zurueckgeschrieben (Schluss unten). Vor jedem Lauf: Stub `0800e003`, Code 0x8002bd44..c18c / 0x8003b558..7f0 /
0x8003da78..dcc0 im RAM == PSX.EXE (`True`).

**A — Poke ohne Eingabe, Raetsel NICHT scharf** (Bank 5 = 0): Cursor z := 29500 (in der um r erweiterten Zelle r1[16]),
Haltepunkte @0x8002bd44 / @0x8002c0d0 / @0x8003b79c / @0x8002bfb0 / @0x8002c158:
```
Bild 1 EINTRITT Objekt-Pass: Cursor x -19554 z 29500 spiegel (-19554,22684)
  @0x8002c0d0 Ruhe-Zweig jal 0x8003b558: a0 = 800b3f98 (Cursor) a1 = 2
  @0x8003b79c jal 0x8003da78: Zelle a0 = 801223b0 x -31700..-7750 z 30150..31150 u0 ff fl 03, s1 = Cursor, r a2 = 900
Bild 1 AUSTRITT: Cursor x -19554 z 29232 spiegel (-19554,29232)
Bild 2/3: Ruhe-Zweig mit a0 = Cursor jedes Bild, kein Zellentreffer mehr, z bleibt 29232
```
(Nachgerechnet: a3 = 30150-900-29500-18 = -268, dz = +6816 -> Code 2, 668 < 801 -> 29232.) Der Schub-Pfad @0x8002bfb0
wurde fuer den Cursor nie erreicht. (Erster Lauf hatte am Haltepunkt @0x8003b79c a1 abgefragt — a1 wird erst im
Delay-Slot @0x8003b7a0 gesetzt; im zweiten Lauf auf s1 korrigiert. Ergebnis sonst identisch.)

**B — echte Eingabe** (vgamepad, Raetsel scharf wie sub16 @0x015C2..0x015F2 = 13x `22 05 bb 01` = Set(5,0..12,1) ->
Bank 5 Wort 0 = 0xFFF80000), je Richtung 5 s halten, dann bei weiter gehaltener Taste je zwei Bilder mit Haltepunkten
(beide Bilder identisch):
```
OBEN   5 s: (-19554,22684) -> (-19554,29232)  pad 0x800ac768 = 0011 | nach SCD (-19554,29432) -> Zelle 801223b0 -> nach Pass (-19554,29232)
UNTEN  5 s: (-19554,29232) -> (-19554,16068)  pad 0024           | nach SCD (-19554,15868) -> Zelle 8012238c -> nach Pass (-19554,16068)
LINKS  5 s: (-19554,16068) -> (-29782,16068)  pad 1008           | nach SCD (-29982,16068) -> Zelle 80122398 -> nach Pass (-29782,16068)
RECHTS 5 s: (-29782,16068) -> (-9668,16068)   pad 2002           | nach SCD (-9468,16068)  -> Zelle 801222d8 -> nach Pass (-9668,16068)
```
Endlagen und Zellen = Dossier P2.2 (29232 / 16068 / -29782 / -9668; r1[16] / r1[13] / r1[14] / r0[15]) und = Port
(r31 G). Die SCD faehrt den Cursor jedes Bild +-200, der Objekt-Pass schiebt ihn im SELBEN Bild an die Rahmenkante
zurueck. **Antwort auf die Frage des Auftrags: Ja, das Original schiebt den Generator-Cursor aus Zellen.**

settings.ini: nach dem Lauf aus der Sicherung zurueck, `EnableGDBServer = false` (Z. 507), `diff` gegen meine
Sicherung VOR der Abnahme: identisch.

## 4. Zensus der Cursor-/Panel-Raeume nachgeprueft

- **SOLL-Tabellen des neuen Riegels = Original-Log:** `orig_zensus/log.txt` programmatisch gegen SOLL_A/B/C/D der Probe
  verglichen (Fahrbilder, Endlage, erste Ausschiebung vorher/nachher): **56/56 Richtungen gleich**, 14 verschiedene
  Cursor am Original gemessen. ROOM10D1 traegt SOLL_B ohne eigene Messung (offen gelegt, Abschnitt 7).
- **Hebetisch ROOM1150/1151** (eigener SCD-Dump `scd_dump_room.py`): obj 0..2 `2d 0k 00 00 01 (00|c0) 0a 00` -> obj+0 =
  0x000b (Bit 3) -> FUN_8003b558 ueberspringt jede Zelle; nur sie werden per `2e 03 0k` + Speed_set/Add_speed/Pos_set
  bewegt; obj 3 (`00 00` -> 0x0001) wird nie per Work_set(3,3) angefasst. **ROOM4000**: alle sieben Obj_model_set
  `... 01 00 0a 00` -> Bit 3. Kein Ruhe-Zweig-Schub moeglich — bestaetigt.
- **Fahrstuhl 1080** und die uebrigen Panel-Cursor: dasselbe Objekt (Typ 4, Band 0, 0x0101, h900) mit Rahmen; Original-
  Zensus und Port-Riegel gleich (s. o.).
- **Spur-R-Aussage "nur 2060/2061 obj 2 im ersten Bild bewegt"**: `test_r36_dachkiste_zellschub.exe diag` (build_abn0)
  -> `BEWEGT ROOM2060 p=2 obj=2 ... (-3125,2075) -> (-3143,2075)`, dito ROOM2061, `ZENSUS: 206 Raeume, 523 aktive
  Objekte, 2 vom Ruhe-Zweig im ersten Bild bewegt` — gueltig. (Im Spur-R-Baum r36_dachkiste 946314e0 liefert dieselbe
  Sonde `513 aktive Objekte, 2 ... bewegt`; die +10 Objekte stammen aus der Integrationsbasis, G2 aendert keinen
  Engine-Code.) ROOM11A0 obj 0 im Else-Zweig: `0x01040 Ck 21 05 06 00` + Bank-5-Loeschung `8003ed74 sw zero,4136(at)`
  (= 0x800b1028) selbst gelesen -> beim Raumeintritt nicht erreichbar.

## 5. P3 — Riegel mit Original-Beleg (kein Code-Fix)

- Richtig entschieden: das Original verhaelt sich wie der Port nach Spur R; ein Code-Fix waere eine Abweichung vom
  Original gewesen.
- Die eigentliche Aussage der Riegel bleibt erhalten: nach einem Zwischenschalter KEINE Eingabesperre (Nutzer-Vorgabe
  Runde 34 Nacht), Endsperre bis zur Abnahme bei Zeiger 80 (D(2), r34n B). D(1) prueft jetzt sogar schaerfer
  (`dz == 48*200`).
- **Rueckbau selbst gefahren** (eigene Ruhe-Zweig-aus-Kopie, gegen die NEUEN Riegel): `r31_generator` rc 1, **7 FAIL**
  (Vorlauf, D(1b) x2, G x4), `probe_r34n_c_generator` 0 FAIL (A prueft die Sperre, nicht den Rahmen — gewollt),
  `probe_r36_g2_cursor_rahmen pin` rc 1, **60 FAIL / 31 OK** — deckungsgleich mit `G2_belege/mutation.log` g1. Die
  Mutationen g2/g3 (Zwischen-/Endsperre) habe ich nicht erneut gefahren; ihr Protokoll ist schluessig (g2: r31 3 / r34n 7
  FAIL, g3: r31 4 / r34n 12 FAIL).
- Spur-R-Riegel gruen (Abschnitt 6), Zensus-Aussage gueltig (Abschnitt 4).

## 6. P4 — Regression gezielt (selbst, frischer Baum build_abn0)

`PATH="/c/msys64/mingw64/bin:$PATH" SDL_ASSERT=always_ignore ctest --test-dir re15_port/build_abn0 -R "generator|11f0|
panel|hebetisch|r34n_|r31_|r36_dachkiste|zellschub|klettern|objfall|cursor|1080|4000|push|prop_|collision" --timeout 900
-j 4` (87 Tests):

```
100% tests passed, 0 tests failed out of 87
Total Test time (real) = 1473.02 sec
```

Darunter (Auszug): unit_r31_generator 0.58 s, unit_r34n_c_generator 1.03 s, unit_r36_g2_cursor_rahmen 0.52 s,
unit_r27_panel_schalterwerte, unit_gen_11f0_switches, unit_gen_11f0_cursor_view, probe_elevator_1080,
unit_fahrstuhl_1080_etagen, unit_r31_hebetisch, unit_r32_hebetisch_faecher, unit_r34n_b_* (8) + integration_r34n_b_cursor
99.97 s, **unit_r36_dachkiste_zensus 7.88 s, unit_r36_dachkiste_zellschub 0.03 s, integration_r36_dachkiste 1457.93 s**,
integration_r36_klettern 529.44 s, unit_r36_objfall 0.09 s — alle `Passed`; keine `Failed`/`Timeout`/`Not Run`-Zeile.
Deckt sich mit der Summenzeile im Dossier (87/87, 1481.63 s). Keine volle Suite gefahren (wie vorgegeben).

## 7. Maengel / Hinweise (nicht blockierend)

1. **Zaehlfehler Zensus Original:** Dossier/Meldung "alle 15 Cursor ... (13 RDTs) am Original vermessen ... 60/60
   Richtungen". Gemessen sind **14 verschiedene Cursor in 12 RDTs = 56 Richtungen** (`orig_zensus/log.txt`: 15
   Messbloecke, ROOM11E0 obj 1 zweimal). Die 60/60 des Port-Riegels enthalten ROOM10D1 (4 Richtungen) mit den 10D0-Werten
   — das ist im OFFEN-Abschnitt korrekt als nicht gemessen ausgewiesen, die Ueberschrift-Zahlen stimmen aber nicht.
2. **Zaehlfehler statischer Zensus:** Dossier Z1 "28 Panel-Cursor in 20 RDTs"; `zensus_objekte_ausschiebung.txt` hat
   **29 Panel-Cursor-Saetze in 25 RDTs** (Typ 4, Band 0, 0x0101, vom SCD bewegt) + ROOM3091 obj 1 = die 30 bewegten.
   Inhaltlich (alle derselbe Objekttyp) richtig.
3. `RE15_MIN_TESTS` nicht angehoben (+1 `unit_r36_g2_cursor_rahmen`) — offen gelegt, Sache der Integration.
4. 13 der 14 Original-Cursor sind per Nachbildung (+-200 je Bild am Haltepunkt @0x8002bd44) gefahren, nicht ueber die
   raumeigenen Tastenpfade — offen gelegt; die Nachbildung ist an 11F0 gegen die echte SCD-Fahrt gegengeprueft, und
   meine eigene Messung B (echte Eingabe) liefert dieselben Endlagen.
5. ROOM10D1 (Elza) am Original nicht gefahren, ROOM2050 obj 8 spaeter nicht untersucht — offen gelegt, nicht Auftrag.

Empfehlung: Punkte 1/2 bei der Integration im Dossier korrigieren (reine Zahlen), Punkt 3 mit dem naechsten
Zaehlerstand nachziehen.

## 8. Urteil je Punkt

| Punkt | Urteil | Beleg |
|---|---|---|
| P1 MESSEN | erfuellt | alte Riegel mit/ohne Ruhe-Zweig selbst gefahren: FAIL genau wie gemeldet + 17/31 RUHE-Zeilen (ZELLE [33]/[30]) bzw. gruen ohne Zweig (dz 9600) |
| P2 ORIGINAL | erfuellt | Disassemblat selbst gelesen (Hauptschleife, FUN_8002bd44, Installer, FUN_8003b558, FUN_8003da78, RDT-Bytes); eigene GDB-Messung: Ruhe-Zweig @0x8002c0d0 mit a0 = Cursor, Treffer @0x8003b79c Zelle 0x801223b0 r 900, 29500 -> 29232; echte Eingabe: 29232 / 16068 / -29782 / -9668 |
| P3 FIX | erfuellt | Riegel mit Original-Beleg angepasst, Absicht erhalten; Rueckbau ohne Ruhe-Zweig 7/0/60 FAIL selbst reproduziert; Spur-R-Riegel gruen, Zensus "2 bewegt (2060/2061 obj 2)" gueltig |
| P4 REGRESSION | erfuellt | frischer Bau LOCAL-BUILD-OK (1880 Schritte); ctest -R (Auftrags-Regex) selbst: `100% tests passed, 0 tests failed out of 87` (1473.02 s) inkl. aller Spur-R-Riegel; Summenzeile im Dossier; Abschluss-Commit af3e040e |
