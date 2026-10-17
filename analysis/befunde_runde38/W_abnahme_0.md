# Runde 38 Spur W "rolltor20a0" — Abnahme 0 (unabhaengig)

Datum 2026-10-17. Baum `.claude/worktrees/r38_rolltor20a0` (Zweig `r38/rolltor20a0`, HEAD 62f35a90, Basis df08d83a).
Pruefer: Abnahme 0 (glaubt nur, was hier selbst am gebauten Stand gemessen ist). Bericht wird fortlaufend geschrieben.

Nutzer-Punkt (AUFTRAG.md Nachtrag 1, woertlich):
> And additionally: in ROOM 20A0 the shutter that you have to open to push the box down is somehow DIRECTLY in the
> camera. I cannot see anything... this is not like this in the original....

## 0. Stand und Bau

* `git log master..HEAD`: 47 Commits (W + gemergte A-Commits r38/cursor bis 10e39eda). `git diff master...HEAD` (Drei-Punkt;
  master hat seit df08d83a nur doc(r38)-Commits) beruehrt im Code: `engine/src/objpoly_r38.c` (neu),
  `include/re15_objpoly_r38.h` (neu), `platform/pc/src/objpoly_r38_pc.c` (neu), `platform/pc/main.c` (Prop-Schleife,
  13+/34-), aus Spur A: `engine/src/nclip_r38.c` + `include/re15_nclip_r38.h` (neu), `platform/pc/src/hebetisch_cursor_pc.c`,
  `engine/src/schluessel_r36.c` + `include/re15_schluessel_r36.h` (rot_x je Schluessel), `engine/src/gen/feuerloescher_r37.inc`
  + `tools/r37_feuerloescher/feuerloescher_modell.py` (neu gewickelt); Tests `tests/unit/{test_r38_rolltor20a0.c,
  test_r38_rolltor20a0_windung.c, probe_r38_rolltor20a0_karte.c, probe_r38_cursor.c}`, `tests/unit/probes/{r38_rolltor20a0,
  r38_cursor}.cmake`, `tests/integration/{test_r38_rolltor20a0.cmake, test_r38_cursor.cmake, r38_rolltor20a0/wp_*.txt}`.
* Bau: `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`, `... build` ->
  `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`. Gegenprobe: alle beruehrten Quellen (juengste
  `tests/unit/test_r38_rolltor20a0_windung.c` 05:35:53) sind aelter als die Binaerdateien (`re15_pc.exe` 05:24:18,
  `test_r38_rolltor20a0_windung.exe` 05:36:00), `git status` leer -> die exe entspricht HEAD.

## 1. Eigene Riegel der Spur (selbst gefahren)

`ctest --test-dir re15_port/build -R "^(unit|integration)_r38_rolltor20a0(_windung)?$" --timeout 1200 --output-on-failure -V`
-> `100% tests passed, 0 tests failed out of 3` (integration 250.77 s). Ausgaben (Auszug):
* `R1 Cut 11 offen: 48 Vierecke wie im Original (NCLIP 20, OTZ<64 16, eingehaengt 12), max |dSXY| 0`
* `R2/R3 Cut 11 offen: Rolladen bedeckt neu 0 Pixel, alt 67728 von 76800 (88.2 %)`
* `R4 Cut 12 y -7200: bedeckt neu 12721, alt 12812` / `R4 Cut 12 y -10800: bedeckt neu 10934, alt 11012`
* `R6 ROOM20A1 = ROOM20A0 (300176 Bytes), Cut 11 offen bedeckt 0 Pixel`
* `R7 ROOM1050 obj 0 Cut 5: NCLIP-Vorzeichen 15/15 wie Original, max |dOPZ| 0, gezeichnet 0`
* `O2: 20 port-eigene Modelle geprueft (9 flach, nicht bewertet), 0 raeumliche nicht in Original-Windung`
* integration: a_leon / b_elza / d_leon_karte je 9 Cut-11-Bilder F700..F1100 Anteil 0.9420..0.9503, `OBJPOLY Cut 11 obj 1:
  450 Bilder, Abweichungen 0`; c_kiste `F1278 room=20A0 p=0 LANDUNG y=-3600 band=2 pos=(12250,-23779)`.

## 2. RE-Gate (selbst disassembliert, `info/Re1.5/PSX.EXE` md5 b55fdaa5..., Stub @0x80026e4c = `08 00 e0 03` = Auslieferung)

| Zitat im Diff | selbst gelesen | Urteil |
|---|---|---|
| FUN_80023098 RTPT/NCLIP/`bgez` @0x80023148/@0x80023168/@0x80023190 | `4a280030` RTPT, `4b400006` NCLIP, `swc2 MAC0` `e9980000` @0x80023184, `lw v0,0(sp)`, @0x80023190 `bgez v0,0x800231b8` (sonst `addiu a1,a1,104` + `j 0x80023264` = naechstes Viereck) | stimmt |
| FUN_80023098 RTPS/AVSZ4/OTZ @0x800231f8/@0x8002320c/@0x80023224/28 | `4a180001` RTPS, `4b68002e` AVSZ4, `swc2 OTZ` `e9870000`, `lw v1,4(sp)`, @0x80023224 `srl v0,v1,6`, @0x80023228 `beq v0,zero,0x80023260` (= ueberspringen) | stimmt |
| FUN_80022f0c NCLIP @0x80022fec, `bltz` @0x80023010, AVSZ3 @0x80023020, @0x80023044/48 | `4b400006`, @0x80023010 `bltz v0,0x80023080`, `4b58002d` AVSZ3, `srl v0,v1,6` + `beq v0,zero,0x80023080` | stimmt |
| FUN_800254a0 NCLIP @0x80025580, `bgez` @0x800255a8, @0x80025654/58 | `4b400006`, @0x800255a8 `bgez v0,0x800255d0`, AVSZ3 @0x80025640, @0x80025654 `sra v0,v1,6`, @0x80025658 `beq v0,zero,0x80025690` | stimmt |
| FUN_800256b0 NCLIP @0x80025798, `bgez` @0x800257c0, RTPS @0x80025878, AVSZ4 @0x8002589c, @0x800258b0/b4 | genau so (`sra v0,v0,6`, `beq v0,zero,0x80025918`) | stimmt |
| ZSF3 341 @0x80066c70, ZSF4 256 @0x80066c7c | @0x80066c70 `addiu t0,zero,341` + @0x80066c74 `48c8e800` (ctc2 t0,$29 = ZSF3); @0x80066c7c `addiu t0,zero,256` + @0x80066c80 `48c8f000` (ctc2 t0,$30 = ZSF4) | stimmt |
| SetGeomOffset(0xa0,0x78) @0x8001643c/@0x80016440 | `ori a0,zero,0xa0` / `jal 0x80066d60` / Verzoegerung `ori a1,zero,0x78` | stimmt |
| Dispatch FUN_8002c18c (Attribut & 0x10 -> Flach, Typ 3 -> FUN_8003e64c) | @0x8002c2c8 `jal 0x8003e64c`, @0x8002c320/350/380/3b0 `jal 0x80022f0c/0x80023098/0x800254a0/0x800256b0`; FUN_8003e64c @0x8003e6ac `jal 0x80022f0c`, @0x8003e6dc `jal 0x80023098`; Decompilat RE_15_Quellcode_V2/FUN_8002c18c.c gleichlautend | stimmt |
| RE2 (`info/re2leon/PSX.EXE`, re2_disasm.py): FUN_800363f0 @0x80036610/40/6c/9c, FUN_8002d0e8 NCLIP @0x8002d18c `bgez` @0x8002d1ac, FUN_8002d244 `bgez` @0x8002d314, FUN_80027bec `bgtz` @0x80027cc8, FUN_80027dbc `bgez` @0x80027e84 | alle vier `jal` vorhanden; `4b400006` + `swc2 MAC0` (`ebb80000`) + Verzweigung genau an den genannten Stellen | stimmt (Hinweis: FUN_80027bec verwirft auch MAC0 = 0, Port verwirft nur < 0 — entartete Flaechen, ohne Bildwirkung) |
| psx-spx GPU "Size Restriction" 1023/511, RTPS SX/SY -400h..+3FFh, NCLIP-Formel | `docs/graphicsprocessingunitgpu.md:332-333`, `docs/geometrytransformationenginegte.md:418-419`, `:439` woertlich | stimmt |

Suche nach Rate-Woertern im Code-Diff (`deferred|tunable|interim|for now|faithful|plausibel|TODO`): siehe §5.

## 3. Eigene Messung am Spiel — Leon, ECHTER Tuerweg 2090 -> 20A0 (nicht der Weg des Bau-Agenten)

Der Bau-Agent hat 20A0 nur per Debug-Sprung vor die Leiter und per synthetischem Spielstand + CONTINUE betreten.
Ich habe den Raum ueber seine einzige Tuer betreten: ROOM2090.RDT @0x009B6 `Door_aot_set` slot 1 (Band 1, Rechteck
x 6750..7750 z -18100..-14600) -> ROOM20A0 (4950,-3600,4100) Blick 1024, Cut 13, Band 2 (Gegenrichtung ROOM20A0.RDT
@0x01746 -> 2090 (6300,-1800,-16250)); Tuer-Ausloesung wie FUN_80042bac = Punkt 620 vor der Blickrichtung
(aot_common.c). Danach zu Fuss ueber Band 2 (Gang x 3250..6700 nach Norden, Gang z -23400..-21000 nach Osten) zur
Leiter, hoch, Schalter, Ja, nach Sueden in Cut 11, Kiste nach Norden durch die Oeffnung.

Werkzeug (Scratchpad `abnW0_rolltor/`): Wegplan `wp_tuer_20a0.txt`, Laeufer `lauf_w0.sh` (Messkopie `re15_pc_abnW0.exe`
bzw. `re15_pc_abnW0alt.exe`), Kontaktbogen `bogen.py`. Befehl (neu):
`bash lauf_w0.sh neu t1_tuer_leon wp_tuer_20a0.txt RE15_PSELECT_AUTO=1 RE15_KEIN_GATOR=1 RE15_DEBUG_JUMP=2090@250
RE15_PLAYER_POS=6300,-16250,2048,1 RE15_FRAMEDUMP=300-3600/10:fd_ RE15_EXIT_AT=3600#20A0` (RE15_WINDOW_SCALE=1 im Laeufer).
Gegenprobe identisch mit der ausgelieferten v0.8.25-exe (`release/re15_port_v0.8.25_win64.zip`, ausgepackt im
Scratchpad, Lauf `t2_tuer_leon_v0825`).

Ablauf (wegplan.txt, beide exe gleich): `F2 room=20A0 spieler=(4950,4100) rot=1024` (Tuer), Leiter `F779 ...
(16500,-20000) rot=3072 mo=210`, Schalter-Frage `There's a shutter switch here. Press it?` (Bild F1100), Rolladen
`F1263 ... obj1=(7942,-10800,-20510)`, Cut-Wechsel (state.txt) `F777 cam=12`, `F1358 cam=11`, `F1610 cam=12`,
`F1662 cam=11`, `F2109 cam=12`; Kiste `F2282 room=20A0 p=0 AUSLOESER wort=0x4206 band=4 pos=(12250,-7200,-21473)`,
`F2301 LANDUNG y=-3600 band=2`, weiter auf Band 1 (`obj0=(12250,-1800,-24918) b1`).

| Messung (Cut-11-Bilder F1360..F2100 je 10, Bild UND Vorbild cam=11) | v0.8.25 | Spurbaum (neu) |
|---|---|---|
| Raumhintergrund-Anteil (`test_r38_rolltor20a0 --bild <ppm> ROOM20A.BSS 11 0`) | 69/69 Bilder **0.1305** | 69/69 Bilder **0.8998..0.9678** (tiefster Wert beim Schieben, Kiste nah vor der Kamera) |
| Bild | ganzes Bild Lamellen des Rolladens, kein Raum, keine Figur (`t2_bogen_cut11.png`) | Plattform, Kiste, Faesser, Regal, Leon (`t1_bogen_cut11.png`) — dieselbe Ansicht wie das Original `W_belege/orig_cut11_offen_buf0.png` |
| RE15_OBJPOLY_LOG Cut 11 obj 1 | (Haken gibt es dort nicht) | **699 von 699** Zeilen `gezeichnet=6 nclip=20 otz=16 gpu=6` = Original-Haltepunkt-Messung (20 NCLIP, 16 OTZ<64) |

Uebersicht aller Cuts des Laufs (Bilder alle 100, `t1_bogen_alle1.png`, `t1_bogen_alle2.png`): Cut 13/14/15/1/2 (Weg),
Cut 12 (Schalter, Rolladen von vorn zu -> offen), Cut 11, Cut 12 nach dem Kistenfall — in KEINEM Bild steht der
Rolladen vor der Kamera. Der Nutzer-Befund ist am echten Tuerweg reproduziert (v0.8.25) und behoben (neu).

## 4. Elza — ECHTER Tuerweg 2091 -> 20A1

ROOM2091.RDT == ROOM2090.RDT (cmp), Tuersatz @0x009B6 wie oben; die Elza-Variante waehlt die Engine (Debug-Sprung
`room_id >> 4`, debug_menu_common.c:332). Wegplan `wp_tuer_20a1_elza.txt` (= Leon-Plan mit RAUM 2091 / RAUM 20A1),
Lauf `t5_tuer_elza`: `RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 RE15_KEIN_GATOR=1 RE15_DEBUG_JUMP=2091@250
RE15_PLAYER_POS=6300,-16250,2048,1 RE15_FRAMEDUMP=300-3600/10:fd_ RE15_EXIT_AT=3600#20A1`.
* debug.log `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`; wegplan `F1 room=2091 ... RAUM 2091 geladen`,
  `F46 room=20A1 spieler=(4950,4100) ... RAUM 20A1 geladen` (durch die Tuer), `F1263 ... obj1=(7942,-10800,-20510)`.
* Cut 11 F1358..F1609 und F1662..F2111: **70/70 Bilder Raumhintergrund-Anteil 0.9026..0.9684**; OBJPOLY Cut 11 obj 1
  **702/702 `gezeichnet=6 nclip=20 otz=16 gpu=6`**; Bild `t5_bogen_cut11.png` (Plattform, Kiste, Faesser, Regal, Elza;
  Cut 12 mit offenem Rolladen von vorn).
* Kiste: `F2288 room=20A1 p=0 AUSLOESER wort=0x4206 band=4`, `F2307 LANDUNG y=-3600 band=2`, `F2309 AUSLOESER ... band=2`.

## 5. Nebenwirkungen (selbst geprueft)

### 5.1 Was der Zensus des Bau-Agenten NICHT prueft
`W_belege/zensus_objpoly.py` meldet nur Faelle, in denen der neue Weg WENIGER zeichnet (`pa - pb > 1536 ...`). Der neue
Weg zeichnet aber auch Polygone, die der alte Weg ganz verwarf (ein Scheitel `vz < 64` -> altes Polygon weg; neu mit
gesaettigten SXY, wenn NCLIP >= 0, OTZ >= 64 und ein GPU-Dreieck besteht). Meine Kopie `abnW0_rolltor/zensus_mehr.py`
(gleicher Zensus + Ausgabe `NEU > ALT + 300 px`) findet **3 Faelle**:
* `MEHR ROOM1170 obj 3 Cut 0 @(-1161,-12296,1991): ALT 0 px -> NEU 11338 px (7 Poly)` — Rotor des Hubschraubers im
  New-Game-Intro; Kamera Cut 0 (-1170,-11682,4050) haengt 614 unter der Rotorebene. Detail (`mehr_detail.py`): 7 Dreiecke
  mit zwei Scheiteln hinter der Kamera (vz -1578..-1843 -> SZ 0 -> SX 1023), OTZ 337..904, NCLIP > 0, GPU-Grenze ok.
* `MEHR ROOM5060/5061 obj 4 Cut 4 @(-19548,-3780,-23346): ALT 0 -> NEU 1849 px (1 Poly)` — ein Viereck mit Scheitel 0 bei
  vz 21 (SXY (-1024,1023)); Dreieck (0,1,2) zu gross, (1,2,3) besteht -> Halbviereck.
Beide folgen derselben, am Original belegten Regel (RTPS-Saettigung, NCLIP, OTZ, GPU-Grenze je Dreieck) — kein neuer
Mechanismus; im Dossier aber nicht erwaehnt (siehe Maengel).

### 5.2 Am Spiel gemessen: New-Game-Intro ROOM1170 (Leon), neu gegen v0.8.25
Laeufe `t3_intro1170_neu` / `t4_intro1170_alt`: `RE15_PSELECT_AUTO=1 RE15_FRAMEDUMP=0-1400/5:fd_ RE15_EXIT_AT=1400#1170`.
Beide exe identischer Ablauf (state.txt: 1170 `F1 cam=7` Erzaehler, `F445 cam=0`, `F479 cam=2`, `F982 cam=1`,
`F1043 cam=2`, `F1230 cam=0`, `F1324 cam=1`, `F1360 cam=2`, `F1392 cam=3`). Pixelvergleich (`diff_dumps.py`, max|d| > 24):
146 von 281 Bildern verschieden, **hoechstens 96 Pixel je Bild (0,12 %)**; Cut 0 F450..F470 nur 6..9 Pixel, Cut 0
F1230..F1320 0 Pixel. Bild `intro_vergleich.png`: nur Kantenpixel (Rotorblatt-Kanten oben, Naehte) — die 11338-px-
Zahl des statischen Zensus (Rotor in Ruhelage ry 0) tritt im Spiel nicht auf (der Rotor dreht, Pool 3 ry 2048/2560/
12800/5632 in den Original-Spielstaenden lamp_1170_run2/3, orig_1170_gp/gp2).

### 5.2b Am Spiel gemessen: ROOM5060 Cut 4 (Zensus-Fall MEHR, Halbtransparenz-Objekt) — NEUE sichtbare Aenderung
Laeufe `t7_5060_cut4_neu` / `t8_5060_cut4_alt`: `RE15_PSELECT_AUTO=1 RE15_DEBUG_JUMP=5060@250 RE15_FORCE_CUT=4
RE15_FRAMEDUMP=100-300/50:fd_ RE15_EXIT_AT=300#5060` (debug.log `JUMP -> 506 P-4 LABORATORY (ROOM5060)`, state cam=4).
* OBJPOLY neu: `room=5060 cut=4 obj=4 gezeichnet=1 nclip=1 otz=0 gpu=0` (299 Bilder).
* Pixelvergleich neu gegen v0.8.25: **1193 px in jedem Bild** (`diff_dumps.py`); Bild `r5060_cut4_vergleich.png`: das
  linke Fenster, in dem v0.8.25 den Raumhintergrund zeigt (Streifen; Hintergrund-Anteil tol 24: alt 0.9541, neu 0.9377
  gegen ROOM506.BSS Cut 4), ist im Spurbaum von einem **deckend dunklen Dreieck** zugedeckt.
* Ursache (selbst nachgerechnet, `mehr_detail.py`): obj 4 (`Obj_model_set` @0x02D72, Typ 0, **Attribut 0x0012**,
  (-19548,-3780,-23346)), Viereck P01 mit Scheitel 0 bei vz 21 -> SXY (-1024,1023); Dreieck (0,1,2) zu gross, (1,2,3)
  = (-666,509),(-20,20),(36,142) besteht; NCLIP 156982, OTZ 422 -> der neue Weg zeichnet das Halbviereck (alter Weg:
  `vz < 64` -> weg). Das entspricht der Original-Geometrie-Regel. ABER: Attribut-Bit 0x02 = ABE; FUN_8002c18c reicht es
  als 4. Parameter an den Flach-Viereck-Zeichner, der den Primitivcode baut: @0x800230ac `sll a3,a3,1`, @0x800230b0
  `ori a3,a3,0x3c`, @0x800230d0 `sb a3,3(s0)` -> Code **0x3E = halbtransparent**; Texturseite des Modells 0x0080 ->
  ABR 0 (0,5·B + 0,5·F). Der PC-Prop-Weg setzt fuer Raum-Objekte KEIN ABE (main.c Prop-Schleife ohne
  `re15_render_pc_set_tri_blend`; vorbestehende Luecke: 77 Obj_model_set-Saetze mit Attribut & 2 in 24 Raeumen,
  `abe_zensus.py`, u. a. alle Typ-3-Wasser/Gitter der Stage 2 mit 0x001a, ROOM1040, 11B0, 5030, 5060).
  Das Original zeigt dort also das Fenster zu 50 % abgedunkelt, v0.8.25 zeigte es unverdeckt, der Spurbaum zeigt es
  deckend dunkel. Der Abstand zum Original ist vorher wie nachher 0,5·|BG - Textur| — aber das Bild ist sichtbar
  anders, im Spiel erreichbar (RVD-Zonen 3->4 x -22101..-19900 z -27000..-24700 und 7->4) und im Dossier nicht erwaehnt.

### 5.2c Typ-3-Objekte (vom Zensus des Bau-Agenten ausgenommen) und grosse WENIGER-Faelle am Spiel
* `zensus_typ3.py` (Zensus nur Typ 3, beide Richtungen): keine MEHR-Faelle; WENIGER u. a. ROOM2050 obj 4 Cut 11
  (statisch 12674 -> 0 px, Rueckseiten von unten) und ROOM20A0 obj 6 Cut 3 (1906 -> 0). Am Spiel (`RE15_FORCE_CUT`):
  ROOM2050 Cut 11 neu gegen v0.8.25 **82..100 px** (nur Kanten, `r2050_cut11_vergleich.png`; das Skript setzt obj 4
  je Cut um), ROOM20A0 Cut 3 **0 px**.
* ROOM10D0 Cut 12 (Dossier §8, Zensus ALT 76800 px): `RE15_DEBUG_JUMP=10D0@250 RE15_FORCE_CUT=12` — v0.8.25 zeigt
  das **ganze Bild voller Rolladen-Lamellen** (derselbe Fehler wie der Nutzer-Befund), der Spurbaum Marvin vor dem
  Fenster (`r10d0_cut12_vergleich.png`, 24643..24744 px Unterschied); OBJPOLY `gezeichnet=8 nclip=0 otz=2 gpu=5` — hier
  entscheiden GPU-Grenze und OTZ, nicht NCLIP. Die GPU-Grenze ist am Original-Bild belegt: Q35 des 20A0-Rolladens ist
  eingehaengt (OTZ 122, OPZ > 0), seine Dreiecke (804,-76),(481,231),(-114,-344) / (481,231),(-114,-344),(105,395)
  laegen quer ueber dem Bild (dy 575 / 739 > 511), und `orig_cut11_offen_buf0.png` zeigt nichts davon.

### 5.3 ROOM20A0 selbst (alle Cuts des Tuerwegs)
OBJPOLY im Tuerweg (Auszug): Cut 13/14 obj 1 `otz=48` (Rolladen hinter der Kamera — vorher ebenso weg durch `_vz < 64`),
Cut 15 obj 4 `gezeichnet=9 otz=6 gpu=11`, Cut 2 obj 4 `gezeichnet=18 otz=6 gpu=2`; die Bilder dieser Cuts
(`t1_bogen_alle1.png`) zeigen Gaenge und Rolllaeden ohne Loecher oder Riesendreiecke.

### 5.3b Erklaert der Fix den Befund?
`abnW0_rolltor/rolladen_aufschluesselung.py` (Zensus-Rechnung, ROOM20A0 obj 1 offen, Cut 11): vom alten Weg gezeichnet
wurden nur Vierecke mit allen Scheiteln vz >= 64; davon nimmt die Original-Schranke
`NCLIP: 17 Vierecke, 68220 ALT-Pixel` (Summe je Viereck), `GPU: 3 Vierecke, 0 Pixel`, `gezeichnet: 5 Vierecke, 0 Pixel`
(ausserhalb des Bilds). Die Lamellen vor der Kamera sind also die RUECKSEITEN des offenen Rolladens, die der Port ohne
NCLIP zeichnete; das Original verwirft genau diese an `bgez v0` @0x80023190 (Haltepunkt-Messung 20/48 OPZ < 0). Die
OTZ-, Saettigungs- und GPU-Schranken tragen zu diesem Bild nichts bei; sie sind weitere Original-Regeln desselben
Zeichners (Wirkung §5.1/5.2). Der Fix erklaert den Befund.

Unit-Riegel-Tabelle `k_orig` (test_r38_rolltor20a0.c) gegen `W_belege/orig1_log.txt` maschinell verglichen: 48/48
Vierecke gleich (OPZ, OTZ, eingehaengt, SXY). Echtheit des Logs: der Port trifft die GDB-SXY auf 0 Pixel, die
Python-Nachrechnung mit exakter Wurzel (psxsim.py) nur auf 3 Pixel — passt zu einem Original mit SquareRoot0, nicht zu
einem aus psxsim erzeugten Log. ROOM20A0.RDT @0x01932 selbst gelesen: `2d 01 00 00 05 00 98 00 10 00 06 1f e0 e3 e2 af
00 00 00 04` = obj 1, Typ 0, Attribut 0x0010 (Flach-Zeichner), (7942,-7200,-20510), ry 1024; ROOM1050.RDT @0x00C36 =
obj 0, Attribut 0, (15432,0,-10424), ry 2048.

### 5.4 Bildgenau um die Cut-Wechsel (prueft die Bildwahl e4c0cb9e des exe-Riegels)
Der exe-Riegel wertet seit e4c0cb9e nur Bilder, deren Zustand UND Vorbild cam=11 tragen (Umschaltbild F650 =
"Messartefakt"). Gegenprobe ohne diese Auswahl: Lauf `t6_tuer_leon_jedes` (Tuerweg Leon, `RE15_FRAMEDUMP=1350-1670/1`),
JEDES Bild gegen beide Hintergruende gemessen (`bg_jedes.txt`): kleinstes max(BG11, BG12) = **0.8249 (F1358)**, kein
Bild unter 0.80. An den Wechseln zeigt das Dumpbild noch den alten Schnitt: `F1358 cam=11 bg11=0.0042 bg12=0.8249`,
`F1359 cam=11 bg11=0.9389`; `F1610 cam=12 bg11=0.9435 bg12=0.0020`, `F1611 cam=12 bg12=0.8264`; `F1662 cam=11
bg12=0.8288`, `F1663 bg11=0.9414`. Die Erklaerung des Bau-Agenten (Zustandslog ein Bild vor dem Bild) stimmt; in
keinem einzigen Bild steht der Rolladen vor der Kamera.

### 5.5 ROOM20A0 neu gegen v0.8.25, ganzer Tuerweg
`diff_dumps.py t1_tuer_leon t2_tuer_leon_v0825` (331 Bilder, max|d| > 24) je Cut: Cut 15 2 px, Cut 1 27 px, Cut 2
104..107 px (Rueckseiten des Typ-3-Rolladens obj 4 oben links, Bild `tuer_vergleich_cut2_12.png`), Cut 12 7..71 px
(Kanten von Kiste/Rolladen; Ausnahme F1610 = Umschaltbild mit Cut-11-Inhalt), **Cut 11 32629..36499 px** (der Fix).
Keine weitere sichtbare Aenderung im Raum.

## 6. Vertrag (VERTRAG.md)

* Pfade (`git diff master...HEAD --name-only`): keine `release/`, `platform/android/`, `shared_assets/PSX/`, keine
  Test-CMakeLists; Registrierung ueber `tests/unit/probes/r38_rolltor20a0.cmake` (Muster "Sonden je Thema").
* Bank-9-Bits / Nachrichten-Ids / Items / Ereignisse: keine belegt (grep `game_flag_set|MSG_ID|k_port` im Diff leer).
* Gemeinsame Datei `platform/pc/main.c`: W-Haken = 1 Include + 1 Messschienen-Aufruf + je Dreieck/Viereck der Aufruf
  von objpoly_r38 (mit Kommentar "Runde 38 Spur W"), dafuer 34 Zeilen alte Inline-Projektion entfernt; die Logik liegt in
  eigenen Dateien (objpoly_r38.c / objpoly_r38_pc.c). Mehr als "1-5 Zeilen", aber reine Ersetzung des Port-Ausschnitts
  durch den Modulaufruf — vertragsgemaess im Sinn (Hinweis H2).
* Gemergte Spur-A-Aenderungen (Auftrag des Orchestrators 2026-10-17 05:11) beruehren K-Dateien (`schluessel_r36.c/.h`,
  `gen/feuerloescher_r37.inc`). Lesender Merge-Test `git merge-tree <base> r38/integration HEAD` (Integrationsbaum
  97490964 mit K): Konflikte NUR in `schluessel_r36.c` (Lage-Zeilen A gegen K), `re15_schluessel_r36.h` (A-Konstanten
  gegen K's Entfernung) und `main.c` (nur die Include-Zeilen, beide behalten). Die Prop-Schleife merget konfliktfrei.
  Aufloesungsregel steht in A_cursor.md §3c ("K's Seite nehmen") und W §9.6.
* Riegel je Punkt: unit_r38_rolltor20a0 (R1..R7, Original-Tabelle), unit_r38_rolltor20a0_windung, integration_r38_
  rolltor20a0 (A Leon Sprung, B Elza Karte, D Leon Karte, C Kiste) — messend, gruen (§1).

(weitere Abschnitte folgen: Testgruppen, Urteil)
