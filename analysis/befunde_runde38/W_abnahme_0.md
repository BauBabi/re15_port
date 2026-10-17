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

(weitere Abschnitte folgen: Elza, Nebenwirkungen, Vertrag, Urteil)
