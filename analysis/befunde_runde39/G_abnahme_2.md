# Runde 39 Spur G "karten" - Abnahme 2 (unabhaengig, 2026-10-18/19)

Baum: .claude/worktrees/r39_karten (Zweig r39/karten, Kopf 59ed4de3, Merge-Basis 22abaed9). Gegenstand: Nachbesserung 2
(Commits c6e0b801..59ed4de3) auf den Mangel N1 der Abnahme 1 (ROOM1210 Suedwest-Kammer) - und erneut beide Nutzer-Punkte
gegen den Wortlaut. Der Pruefer glaubt nur, was er selbst am gebauten Stand gemessen hat. Fortlaufend geschrieben
(Sitzungslimit). Arbeitsordner der Messungen (nicht im Baum): `<scratchpad>/G39abn2/`.

## 0. Stand / Bau
- `git log master..HEAD`: 35 Commits, davon 9 der Nachbesserung 2 (c6e0b801, a6ba2ff0, 8dcd780d, d29ee63b, 7ec823df,
  6ea1cec7, a8e630e5, 6027bf4d, 59ed4de3). `git diff 6ea1cec7 HEAD`: nur Dossier, Bild, Werkzeugkopien (nb2/) - kein Code.
- Code der Nachbesserung 2 (`git diff 0c1ded3c HEAD -- re15_port`): karte_r39.c (+66), re15_inv_screen.c (+1 Haken),
  re15_map_zones.h (+23), re15_karte_r39.h (+6), test_r39_karten.c/.cmake, probes/r39_karten.cmake.
- Bau: `bash re15_port/tools/local_build.sh configure && bash re15_port/tools/local_build.sh build` ->
  `=== LOCAL-BUILD-OK (build)`, "ninja: no work to do" (exe 23:03; seit 6ea1cec7 kein Code geaendert).
- Gezielter Lauf: s. Abschnitt 7.

## 1. Punkt 1a - Blaetter im Vollzustand (Sweep der echten exe, eigene Messung)
Werkzeuge: Abnahme-1-Skripte (G39abn1/, Dateizeiten 21:03..22:06 = vor Beginn der Nachbesserung 2 ~22:30, vom Bau
unberuehrt) nach G39abn2/ kopiert (reg.py, vgl2.py, struktur.py, ring.py; lauf.sh nur mit eigenem exe-Praefix
`re15_pc_g39abn2_<id>.exe`). Lauf: `lauf.sh <exe> neu_S 1190 300 1190 3174 0 -11182 0 -- RE15_MAP_SHOT_SWEEP=s`
(Spielstand per probe_r36_karte karte, LOAD GAME, RE15_WINDOW_SCALE=1, alle 13 Blaetter).
- **Gegen den Sweep der Abnahme 1** (G39abn1/neu_S, exe b1da8c01; vgl_sweep.py): s00, s02..s12 je **0** Pixel verschieden;
  s01 (B2) **249** Pixel, alle im Kasten x186..201 y113..129 (= Kammer + Durchgang). Kein anderes Blatt beruehrt.
- **Gegen die rechten Vorlagen-Haelften** (vgl2.py/vgl_box.py, Klassen): B2 249 Abweichungen, **alle 249 im Kammer-Kasten**,
  ausserhalb **0** (von 56 207 verglichenen Kartenpixeln); 1F **0**; 2F **0**; 3F 54 = die zwei begruendeten
  Treppenkaesten der Abnahme 1 (H3, unveraendert).
- **Strukturpruefung** (struktur.py, Kartenfeld x40..300 y40..210) - Zahlen identisch zur Abnahme 1:
  Doppelwand-Pixel B1/B2/1F/2F/3F = 0/0/0/0/0; Tueren nicht auf Wand 1/0/0/0/0 (B1: der Spieler-Ring in der 1190-Halle
  ueber dem Tuerbalken (126..128,115), wie Abnahme 1); Treppenpixel an Wand 0/6/12/6/1 (Sprossenenden wie Vorlage).
- Die Kammer im Klassenbild (W Wand, B Koerper, . Grund; NEU s01):
  ```
       x186   x201
  y112 WWWWWWWWWWWWWWWW      (Unterkante r5 = Nordlinie der Kammer)
  y113 WBBBBBBBBBBBBBBW
  y116 WBBBBBBBBBBBBBBW
  y117 WBBBBBBBBBBBBBBB      <- Durchgang x201 y117..119 (Koerper)
  y119 WBBBBBBBBBBBBBBB
  y120 WBBBBBBBBBBBBBBW
  y128 WBBBBBBBBBBBBBBW
  y129 WWWWWWWWWWWWWWWW...   (setzt die Flur-Unterkante y129 fort)
  ```
  x186 = Ostwand von r9 (10A0) bis y121, darunter dieselbe Linie; keine zweite Linie auf x200; vorher (Abnahme 1) war
  x186..200 y113..128 Grund.

## 2. Mangel N1 (Punkt 1b) - ROOM1210 Suedwest-Kammer an der echten exe (eigene Messung)
### 2a. Der Weg der Abnahme 1 (echtes Laufen durch die Oeffnung)
`lauf.sh <exe> w1210 1210 300 1210 -19500 0 -21700 2048 besucht:11E0:0:0 besucht:1210:-19500:-21700 --
RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT=W1,U4,W5 RE15_INV_OPEN_AT=200#1210 RE15_INV_FB_SHOT=karte.bmp
RE15_INV_FB_SHOT_AT=60 RE15_EXIT_AT=400#1210` (eigener Lauf, nicht integration_r39_karten):
debug.log `[walk] F390 ... pl pos=(-25500,0,-21700)` (wie Abnahme 1), ring.py `RING (191, 117)` - **vorher im Blau links der
Flurwand x201, jetzt im ROT der Kammer** (Klassenbild w1210/karte.bmp: Kammer x187..200 y113..128 rot, Flur gruen,
Durchgang x201 y117..119 Koerper, Ring 5x5 vollstaendig auf Rot):
```
y112 ......WWWWWWWWWWWWWWWWBBBBBBBBBBW      (x180..212; R = aktuell, B = besucht, o = Ring)
y115 ......WRRRoooRRRRRRRRWBBBBBBBBBBW
y117 ......WRRoRoRoRRRRRRRBBBBBBBBBBBW
y119 ......WRRRoooRRRRRRRRBBBBBBBBBBBW
y129 ......WWWWWWWWWWWWWWWWWWWWWWWWWWW
```
### 2b. Weitere Lagen (Spielstand an der Lage, LOAD GAME, Kartenschirm; zst.sh) - Ring der exe, Sonde, Handrechnung
Handrechnung mit der selbst disassemblierten Formel FUN_800473f8 (Abschnitt 5) und der Zeile @0x800769B8 (177,140,2496,2250):

| Lage | Zustand | exe-Ring | Sonde (= Ring + (1,1)) | von Hand | Zone |
|---|---|---|---|---|---|
| (-23300,-26000) Kammer SO-Ecke | Kammer aktuell | (197,126) im Rot | (198,127) | (198,127) | zid 112 |
| (-26600,-22300) Kammer West | Kammer aktuell | (189,118) im Rot | (190,119) | (190,119) | zid 112 |
| (-22900,-22000) Kammer am Durchgang | Kammer aktuell | (198,118) im Rot | (199,119) | (199,119) | zid 112 |
| (-21700,-21700) im Durchgang | Flur aktuell, Kammer besucht (gruen) | (201,117) | (202,118) Index 1 | (202,118) | zid 34 |
| (-19500,-21700) Flur, Kammer nie betreten | Flur aktuell | (206,117) | (207,118) | - | zid 34 |

- Flur allein (z_a_flur): Kammer **nicht** gezeichnet, Flur-Westwand x201 y117..119 **Wand** (geschlossen).
- Flur + B2-Plan (flag:9:77, z_b_plan): Kammer als **Umriss** (Linien x186, y112, y129; kein Koerper), x201 y117..119 Wand.
- Flur aktuell + Kammer besucht (z_p4): Kammer gruen, Durchgang Koerper in der Farbe des Flurs (rot), Ring im Durchgang.
- ROOM1211 (Elza-Raumvariante, Spielstand mit Leon - die Kartenlogik haengt nur an der Raumnummer; LOAD GAME, gleicher
  Weg W1,U4,W5): ohne (4,0xEC)/(4,0xFC) bleibt der Spieler bei
  `pl pos=(-20824,0,-21700)` an der von sub00 geschlossenen Zelle 15 stehen, Ring (203,117) im Flur, Kammer ungezeichnet;
  mit `flag:4:236 flag:4:252` laeuft er bis `pos=(-25500,0,-21700)`, Ring (191,117) im Rot der Kammer.
  (debug.log `[boot] room RDT: STAGE1/ROOM1211.RDT`.)
### 2c. Flaeche (Sonde markerflaeche an JEDEM begehbaren Rasterpunkt, Dichte 1; Sonde = exe an 5 Lagen oben)
```
ROOM1210 Band 0 Saat (-25800,-2200):  10387 begehbare Rasterpunkte, 10387 geprueft, Marker x 189..224 y 74..128, ausserhalb 0
ROOM1210 Band 0 Saat (-25500,-21700): 10387 ... ausserhalb 0                     (vorher, Abnahme 1: 1693)
ROOM1211 Saat Flur (Kammer zu):        8783 ... Marker x 191..224 y 74..128, ausserhalb 0
ROOM1211 Saat Kammer (Kammer zu):      2022 ... Marker x 189..199 y 115..128, ausserhalb 0
```
=> Der Befund N1 (Ring 10 px neben der Zeichnung, 1693 Punkte) ist an der echten exe behoben; der Fix erklaert ihn:
die Kammer ist eine eigene Zone (kleinste Weltbox gewinnt, re15_map_zones.c zone_index_at) mit DERSELBEN Zeile wie der
Flur - die Lage des Rings aendert sich nicht ((191,117) vorher wie nachher), nur steht jetzt die Zeichnung darunter.
### 2d. Querbalken-Befund (184 Punkte) - VORHER-exe gegen NEU-exe an derselben Lage
Lage im suedlichsten begehbaren Streifen des Querbalkens (Mitte z -3440 > -3470 = Innenseite @0x01BD4 z -3920 + 450),
Spielstand an der Lage, LOAD GAME, Kartenschirm; VORHER = exe der Merge-Basis 22abaed9 (G39abn0/vorher_build, mit
RE15_ASSET_ROOT auf den Spurbaum), NEU = Baum:

| Lage | VORHER Ring | NEU Ring | Handrechnung FUN_800473f8 (Zeile @0x800769B8) |
|---|---|---|---|
| (-26328,0,-3440) West | (190,78) = Marker (191,79) UNTER der Balkenlinie y78 | (190,77) = Marker (191,78) | px 191, py 79 -> Fenster y <= 78 |
| (-12300,0,-3440) Ost | (223,78) = Marker (224,79) | (223,77) = Marker (224,78) | px 224, py 79 -> y 78 |

MAP02.PIX r3 uv(96,48) selbst gelesen (pix.py): Balken = Linie y70 (Index 4, x187..226) und y78 (Index 4 ausser der
T-Oeffnung x202..211, dort Index 1), y79 ausserhalb der T-Oeffnung Index 0. Die Wandmitten @0x01B8C (z 466) und
@0x01BD4 (z -4994) projizieren auf py 70 und 82 (von Hand nachgerechnet) - der Balken ist gestaucht gemalt. Der Befund
ist echt, und das Fenster (nur ausserhalb der Flurspanne x -21090 @0x01BC8 .. -17696 @0x01C04, nur z > -4084) erklaert
und behebt ihn: der Marker bleibt auf der Balkenzeile y78 statt eine Zeile darunter im fremden ROOM1220.

## 3. Punkt 2 - ROOM1090 Leons Marker im aktuellen Raum (Nachbesserung 2 beruehrt 1090 nicht; neu gemessen)
- Nutzerpunkt (R1090 pos=(-12528,-9000,959), nutzer/befund_marken_2026-10-17.txt), Sweep neu_M: debug.log
  `pl pos=(-12528,-9000,959)`, Blatt 2F Ring **(183,72)** im Rot (Querband des Gammas); s03 gegen Abnahme 1 (G39abn1/neu_M)
  **0** Pixel verschieden (einzige Abweichung des ganzen Sweeps: B2-Kammer, 249 Pixel).
- ECHTER WEG Leon (Muster test_r36_klettern.cmake Lauf A: RE15_DEBUG_JUMP=10f0@250, (9,71)=1, Tritt schieben, klettern,
  Tuer von oben; Kartenschirm 40 Bilder nach Ankunft; lauf_env.sh mit eigener exe-Kopie): `[room] PC loaded room1090.rdt`,
  climb.txt `T1227 room=1090 pos=(-13600,-9000,1300) band=5`, Ring **(182,72)** im Rot; Abzug gegen Abnahme 1
  (G39abn1/echt_1090_leon) **0** Pixel verschieden.
- ECHTER WEG Elza (Lauf B: RE15_TITLE_SHOT_CHAR=1 RE15_GOTO_ROOM=10F1, Weg durch den Raum, Tritt, Tuer; Karte 20 Bilder
  nach Ankunft): `[pl] Spieler-Familie PL04`, `PC loaded room1091.rdt`, `T1243 room=1091 pos=(-13600,-9000,1300) band=5`,
  Ring **(182,72)** im Rot; gegen den Leon-Abzug nur 13 Pixel im Ring-Kasten (180..184,70..74) = Blinkphase.
- Sonde (unit_r39_karten_1090 im gezielten Lauf gruen; Flaechenzahlen der Abnahme 1 unveraendert, Code von 1090 seit
  Abnahme 1 nicht geaendert: `git diff 0c1ded3c HEAD -- re15_port/engine` beruehrt nur 1210/1211-Eintraege).

## 4. Nebenwirkungen Tuer-Uebergaenge (Gesamtschiene)
`RE15_UEBERGANG_LISTE=1 tests/integration/test_map_uebergang.exe` (Bau des Baums): `[Sprung] 151 Uebergaenge auf gleichem
Blatt: Median 6 px, Mittel 17 px, schlimmster 91 px, ueber 8 px: 62`; je Tuerpaar gegen die Liste der Abnahme 1
(G39abn1/uebergang_neu.txt, ueb_vgl2.py): **132 Paare, 132 gleich, 0 verschieden** (Lagen und Spruenge).

## 5. RE-Gate (selbst disassembliert / gelesen, eigene Werkzeuge G39abn2/tools/)
| Behauptung (Diff/Dossier) | Werkzeug | Befund |
|---|---|---|
| Zeile ROOM1210 @0x800769B8 = (177,140,2496,2250) | `re15_disasm.py bytes 0x800769B8 8` (info/Re1.5/PSX.EXE) | `b1 00 8c 00 c0 09 ca 08` - **stimmt** |
| Formel FUN_800473f8 | `re15_disasm.py dis 0x800473f8 120` | `addiu v0,v0,32000` @0x80047428, x10 @0x8004742c-48, `lhu` sx @0x80047444 (0x800768b4+8i), `mult`/`sra t0,t0,20` @0x8004745c, `addiu t0,t0,5` @0x80047460, /10 per `0x66666667` @0x80047450/54 + `sra a1,a1,2`/Vorzeichen @0x800474d0-d8, `lh` ox @0x800474e8; z: `lhu` sy @0x8004747c, `sra v1,v1,20` @0x800474a8, `+5` @0x800474ac, `subu v1,zero,v1` @0x800474b0, `lhu` oy @0x80047518, `sh` @0x80047528 - **stimmt**. Damit von Hand: px(-22128)=200, px(-22127)=201 (Ost-Grenze der Kammer-Zone); py(-20746)=116, py(-22520)=120 (Durchgang y117..119); Wandmitten Querbalken z 466 -> py 70, z -4994 -> py 82; Suedstreifen z -3440 -> py 79 - **alle wie behauptet** |
| Seiten-Tabelle B2 @0x800762F4 (12 B je Eintrag), r3 @0x80076318 = (187,70) 64x80 uv(96,48); West-Zellen r4 (187,78), r5 (187,95), Ost r6/r7/r8 (212,78/95/112), r9 (170,101); keine Zelle bei (187,112) | `re15_disasm.py bytes 0x800762F4 120` | **stimmt** (10 Eintraege dekodiert) |
| ROOM1210.RDT RVD @0x002DC Schnitt 5->6 in der Oeffnung, @0x002F0/@0x00304/@0x00318/@0x0032C/@0x00340 | eigenes rdt1210.py (RDT+0x28 = 0x188, 20 B je Eintrag) | @0x002DC `cut 5->6 x -22000..-21300 z -23500..-19500`; @0x002F0 `6->0 x -28600..-17300 z -27800..-17200`; @0x00304 `6->5`; @0x00318 `6->7 z -24260..-23419`; @0x0032C `7->0`; @0x00340 `7->6` - **stimmt**: die Kammer hat im Original die Schnitte 6 und 7 |
| SCA-Zellen (RDT+0x20 = 0x1B68, 24 B Kopf, 12 B je Zelle {u16 w,d; s16 x,z; type,u0,u1,floor}) | rdt1210.py | @0x01BEC x -28908..-27358 z -27578..-17930; @0x01BF8 x -27992..-21368 z -19898..-17948; @0x01BE0 x -28928..-16328 z -28496..-26696; @0x01C40 x -22397..-21185 z -27560..-22520; @0x01BC8 x -22150..-21090 z -20746..-4084; @0x01C04 x -17696..-16676; @0x01B8C z -434..1366; @0x01BD4 z -6068..-3920; Hindernisse @0x01C64/@0x01C58/Kreis @0x01C4C (Typ 3) - **stimmt** (Werte auf die Einheit) |
| ROOM1210 main00: Door_aot_set nur @0x01CC6..@0x01D66 (11E0 + fuenf 1220-Zellen), Obj_model_set @0x01E4E Kollisionsbox 0, kein 0x37/0x38 | eigenes scd1210.py (Laengen = scd_vm.c s_opcode_sizes/scd_opcode_size_at) | sechs 0x3B: @0x01CC6 Ziel-Raum 0x1E (11E0), @0x01CE6/@0x01D06/@0x01D26/@0x01D46/@0x01D66 Ziel 0x22 (1220) Schnitte 0/2/4/6/8; @0x01E4E `2d 00 00 00 01 00 0a 00 10 00 0a ab 00 00 e8 ae 00 00 c0 04 00..00` = (-21750,0,-20760) rot 1216, Bytes 22..33 = 0; kein 0x37/0x38 in main00/sub00..02 - **stimmt** |
| ROOM1211 sub00 schliesst die Kammer bis (4,0xEC)+(4,0xFC): Sca_id_set @0x01E0E/@0x01E5A auf Zelle 15 @0x01C34 | scd1210.py + rdt1210.py ROOM1211 | @0x01DE6 If, @0x01DEA `21 04 ec 00` Ck(4,0xEC)=0 -> @0x01E0E `37 00 0f ff` (Zelle 15 u0 := ff = fest); Else If @0x01E1A `21 04 fc 00` -> @0x01E5A `37 00 0f ff`; sonst @0x01E62 `37 00 13 ff` (Zelle 19); Zelle 15 in ROOM1211 @0x01C34 x -22224..-21292 z -22928..-20330 u0 00 (deckt die Oeffnung). 0x37 = op_sca_id_set (scd_vm.c:4762, u0 := pc[3]) - **stimmt**; an der exe bestaetigt (Abschnitt 2b: ohne Flags Halt bei x -20824) |
| Gemalte Linien MAP02.PIX: r3 Querbalken y70/y78 (T-Oeffnung x202..211), Flur-Westwand x201 y79..128, Unterkante y129 ab x201; r9 Ostwand x186 y101..121; r5 Unterkante y112 x187..201 | eigenes pix.py (4 bpp, 128 B je Zeile, u/v der Seiten-Tabelle) | **stimmt** - die Kammer-Linien x186/y112/y129 liegen auf gemalten Linien, x201 ist die gemalte Flurwand |

- Suche im Code-Diff der Nachbesserung 2 (`git diff 0c1ded3c HEAD -- re15_port | grep '^+'`) nach deferred/tunable/interim/
  for now/faithful/plausibel/TODO/FIXME/geschaetzt/approx/getenv: **kein Treffer**. Ganzer Zweig: nur der Kommentar
  "GEMESSEN statt geschaetzt" (karte_r39.c:17). Kein neuer getenv.
- Konstanten der Nachbesserung 2 je mit Beleg: s_korr {1,3,14,47,1,3,1,R39_SCHEMA_KOERPER(12)} (= x201 y117..119; @0x01BC8/
  @0x01C40, @0x800769B8, FUN_800473f8), s_kanten {1,186,112,15,18,3,3} (Zelle der Kammer, Traeger r3), s_fenster (Flurspanne
  @0x01BC8/@0x01C04, Nordende @0x01BC8 z -4084, Balken r3 Spalten 0..39/Zeilen 0..8; +-40000 = "unbegrenzt" = PORT-WAHL der
  Tabellenform), Zone 1 {0x1210/0x1211, -25858,-25196,-23628,-21398, ..., zid 112, Zeile @0x800769B8, synth 12} (Weltbox =
  Innenseiten @0x01BEC/@0x01BF8/@0x01BE0 und px-Grenze -22128, je um ZONE_SLACK 1500 eingezogen: -27358+1500, -19898-1500,
  -26696+1500, -22128-1500 - nachgerechnet), s_map_synth synth 12 / Zelle 21 (186,112,15,18) (Linien aus den Waenden,
  nachgerechnet). **Gate haelt.**
- Erklaert der Fix den Befund? **Ja** (Abschnitte 2a-2d): gleiche Zeile, gleiche Ringlage (191,117) wie vorher, jetzt mit
  Zeichnung darunter; der Querbalken-Rest ist ein eigener, an VORHER/NEU-exe gemessener Mechanismus (py 79 -> 78).

- Nachrechnung der Formel als Skript (Vorzeichenbehandlung wie disassembliert: `sra v1,v1,31` @0x800474fc, `subu v0,v0,v1`
  @0x80047508): px(-22128)=200, px(-22127)=201; py(-20746)=116, py(-22520)=120; py(466)=70, py(-4994)=82; py(-3440)=79;
  Lagen p1..p4/Flur (198,127), (190,119), (199,119), (202,118), (207,118) = Sonde = exe-Ring + (1,1); Waende West 184..188,
  Nord 114..110, Sued 132..129, Ost 200..203. Einzige Ungenauigkeit (nur Dossier, nicht Code): "py 79 ab z -3337" - richtig
  ist ab z -3339.

## 6. Teilzustaende: jeder Raum der Polizeistation ALLEIN besucht (echte exe, neu in dieser Abnahme)
Lauf allein_alle.sh: 46 Lagen = Ankunftslagen der Tueren (eigener Dekoder ankunft.py: alle Door_aot_set der STAGE1-RDTs,
next_x/y/z), je Spielstand mit NUR diesem Ort besucht (`besucht:<raum>:<x>:<z>:<band>`), LOAD GAME, Kartenschirm,
FB-Abzug; Pruefung tuer2.py (Tuerbalken, dessen Ende auf Grund liegt oder neben dem quer eine Wand steht) und struktur.py.
- 42 von 46 Lagen auswertbar (1050 erst im zweiten Anlauf, dazu die Lage (20600,0,12350); 10F0, 1170, 11C0, 1240 oeffnen
  den Kartenschirm wegen ihrer Eintritts-Szene nicht - durch Vollzustand/Sonde abgedeckt). **Doppelwand in allen 43
  Abzuegen: 0.** Treppen: nur Sprossenenden an Waenden
  wie in den Vorlagen (1060/10A0).
- Tuerbalken NICHT auf einer Wand:
  | Zustand | Balken | Befund | erreichbar? |
  |---|---|---|---|
  | **1050 allein** (al_1050x, Lage (14700,0,6100)) | Tuer 1030<->1050 x186 y150..154 | Enden Grund, 5 Wandpixel quer daneben (1050-Westwand x187) - der Balken schwebt LINKS neben der Wand im Leeren | **ja, normaler Weg** (s.u.) |
  | **1040 allein** (al_1040, Lage (-21008,0,-13134) = Ankunft aus 1060) | Tuer 1030<->1040 x119 y127..131 | Enden Grund, 5 Wandpixel quer daneben (1040-Ostwand x118) - schwebt RECHTS neben der Wand | 1060->1040 ist gesperrt, solange Leon nicht in ROOM1150 war (re15_tuer1060.h: gesperrt <=> (9,73)=0 und ((9,71)=1 oder (3,94)=0); Lauf: `[tuer1060] ROOM1060 Slot 2 -> Text-Platz msg 1`); danach (und ueber den Aufzug 1080) vor 1030 moeglich, nicht als Weg gemessen |
  | 1210 allein | Tuer 11E0<->1210 x186 y72..76 | Enden Grund | nein: ROOM1210 ist nur aus 11E0 erreichbar (11E0-Ostwand x186 dann gezeichnet) |
  | 1090 obere Ebene allein | Tuer x197..201 y114 | belegt das Eckpixel der Suedwand (liegt auf der Wandlinie) | - kein Schweben |
  | 1130 allein | (153,128) | Rest eines vom Ring ueberdeckten Balkens auf der Wand x153 | - Messartefakt |
- **ECHTER WEG zum 1050-Zustand** (echt_1090_1050): Spielstand im Hof ROOM1090 (Band 1) vor der Tuer nach 1050 (Tuer slot 0
  @0x0211A x -12430 z 2260 w 4700 d 1600, floor 1, Ziel 0x05 = ROOM1050; Ankunft (19850,0,-22300)), 10F0 + 1090 besucht,
  `RE15_INPUT_SCRIPT=W1,U0.3,W0.3,A0.1,W8`: debug.log `[aot] DOOR FIRE slot=0 ... spawn=(19850,0,-22300)`,
  `[room] PC loaded room1050.rdt`, Kartenschirm 40 Bilder spaeter: **Tuer 1030<->1050 schwebt x186 y150..154 neben der
  Wand x187** (tuer2.py: Enden [0,0], 5 Wandpixel quer). Ausschnitt x6: G39abn2/echt_1090_1050/tuer_1030_1050_x6.png
  (gelber Balken ausserhalb der grauen Westwand im Blau); 1040: G39abn2/al_1040/tuer_1030_1040_x6.png.
- Warum das der NORMALE erste Stand auf 1F ist: der 1F-Plan liegt in ROOM1030 selbst (gen/kartenitems_r36.inc
  k_r36k_lage[2] = `{ 0x1030, 2, 89, ... }` "1F Mitte"), vor dem Betreten von 1030 ist 1030 also weder besucht noch als
  Umriss gezeichnet; ein normaler Weg vom Dach (Start ROOM1240) nach 1F fuehrt ueber 10F0 -> Klettern (Fortschritts-
  pflicht, Runde 36 Spur A) -> 1090 -> Hof -> 1050 (Tuer 1060->1040 bis zum Besuch von ROOM1150 gesperrt, s.o.). Die Marke ist `{ 2, 4, 186, 152, 3, 4, 6, 1 }` (re15_map_zones.h:753; zid 4 = 1030,
  zid2 6 = 1050, auf_partner 1 = schon sichtbar, wenn nur 1050 besucht ist); 1040: `{ 2, 4, 119, 129, 3, 4, 5, 1 }`
  (re15_map_zones.h:730). Seit Runde 39 ist im Vollzustand die 1030-Linie x186 bzw. x119 die EINE Wand (Korrektur
  s_korr 1F r0 Spalte 7 bzw. r2 Spalte 16 mit Traeger r4 = 1030); ohne gezeichnetes 1030 bleibt die eigene Linie x187
  bzw. x118 stehen (richtig, Abnahme 0 M3) - die Tuer bleibt auf der 1030-Linie und schwebt.
- VORHER: identisch (Merge-Basis-exe G39abn0/al_1050_alt und al_1040_alt: Balken x186 bzw. x119 neben x187 bzw. x118) -
  **keine Regression der Runde 39**, aber auch nicht behoben. In der Abnahme 1 als Hinweis H1 gefuehrt (nur 1050, ohne
  Erreichbarkeit); neu sind hier der echte Weg, die Erreichbarkeit vor dem 1F-Plan und der zweite Fall 1040.
- Original: RE1.5 malt immer alle Rechtecke (re15_map_owned.h, Schleife 0x80047128..0x800471FC) und kennt keine
  Teilzustaende; an diesen Stellen tragen die Kacheln KEIN Tuersymbol (MAP03.PIX selbst gelesen: r4 = 1030 Ostlinie x186
  y144..157 und Westlinie x119 y125..134 reine Wandlinie Index 4). Die gelben Tuerbalken (s_map_marks) und die
  Teilzustaende (Besucht/Plan, RE2-Vorbild) sind Port-Logik - Massstab ist damit der Wortlaut. Der Nutzer hat ein
  schwebendes Tuersymbol schon einmal gemeldet ("This door is flying", 2026-09-05, zitiert in re15_map_zones.h:652-653).

## 7. Tests (gezielt, KEINE volle Suite im Spurbaum)
- Regex = die 99 Tests des Baus (alle Karten-/Hinweis-/Inventar-Gruppen, die der Diff beruehrt: karte_r39.c, map_hint_common.c,
  re15_inv_screen.c, re15_map_zones.h, re15_karte_r39.h, + unit_1210_ fuer den Raum); mit `ctest -N` gegen alle Namen mit
  karte/map/inv/hint/hinweis/tagebuch/1210/zone abgeglichen - nicht enthalten sind nur Biss-/Griff-Riegel ("hinten"),
  prop_zone_dispatch, unit_maggot_zone_leap, unit_scd_zone2_pauseflags, unit_stage6_hintergrund, unit_r35_affen_zonensprung
  (kein Kartenbezug). Lauf 23:18:51..23:26:54, `-j 4 --timeout 900 --output-on-failure`, parallel zu meinen exe-Laeufen:
  **100% tests passed, 0 tests failed out of 99** (483 s). Log: G39abn2/ctest_ziel.log.
- Riegel selbst nachgefahren (Ausgabe gelesen): `probe_r39_karten pruef teil` (e) sechs Kammer-Zustaende PASS mit denselben
  Zahlen, die ich an der echten exe sehe (Flur allein: Kammer 0 Pixel, x201 3/3 Wand; Flur+Kammer: Linien 65/65, Durchgang
  3/3, Innenraum 224/224, x200 0; Kammer allein: x200 18/18; Plan: Umriss 34/34, Durchgang Wand 3/3); `pruef marker`:
  ROOM1210 0/10387, ROOM1211 offen 0/10391, zu 0/8783 + 0/2022.
- Messen die Riegel? Gegenprobe ohne Code-Eingriff: die Pruefungen von Lauf K (integration_r39_karten) auf den VORHER-Abzug
  der Abnahme 1 (G39abn1/w1210b/karte.bmp, gleicher Weg) angewandt: `MARKER 191 117`, `FARBEN ... rot=0 besucht=0` ->
  "kammer-ring-nicht-im-rot" = **ROT**; auf meinen NEU-Abzug (w1210): `rot=36 besucht=0` -> gruen. Die Flaechenzahl des
  Marker-Riegels (markerflaeche 1210, Dichte 1) lag VORHER bei 1693 (Abnahme 1, gleiche Funktion, gleiche Saat) -> ROT.
  (Die Gegenproben des Baus mit voruebergehend umgestellten Zonen habe ich nicht wiederholt.)
- Neue Laeufe parallel-/Linux-fest: Lauf K nutzt die vorhandene exe-Kopie/den Laufordner von integration_r39_karten
  (`cmake_minimum_required` im Skript, eigener Arbeitsordner `${_basis_wd}_K`, kein geerbtes Env); `list(SUBLIST)` statt
  Index-Arithmetik. Kein neues add_test ausserhalb probes/r39_karten.cmake.

## 8. Vertrag / Pfade / Nebenwirkungen
- Keine Bank-9-Bits, keine Nachrichten-Ids, keine Slots/Ereignisse (Diff gelesen). Das neue Besucht-Bit der Kammer ist das
  Zonen-Bit 2n+1 von ROOM1210 im Kartenfeld s_visited (re15_map_zones.c zone_bit) - kein Bank-9-Bit; ROOM1210 hatte vorher
  nur Zone 0 (zones-Tabelle gelesen), unit_map_zone_bits gruen. Alte Spielstaende: Bit 0 = Kammer unbesucht.
- Pfade (`git diff master...HEAD --name-only`, ohne analysis/): engine/src/karte_r39.c, map_hint_common.c, re15_inv_screen.c,
  re15_map_zones.h, include/re15_karte_r39.h, Tests unter tests/unit + tests/integration (*.cmake-Laufskripte), Sonde
  probes/r39_karten.cmake (per GLOB tests/unit/CMakeLists.txt:4060 eingebunden). KEINE release/, platform/android/,
  shared_assets/PSX/, keine CMakeLists.txt.
- Gemeinsame Dateien: re15_inv_screen.c in der Nachbesserung 2 genau EINE Zeile (Haken re15_karte_r39_fenster, Kommentar
  "Runde 39 Spur G"); karte_r39.c/re15_map_zones.h = Karten-Code der Spur.
- Code gelesen: `schema_gezeichnet(..., mit_umriss=1)` verhaelt sich fuer alle alten R39_SCHEMA-Traeger wie vorher (Ziel ->
  1, Plan -> 1); nur der neue Traeger R39_SCHEMA_KOERPER zaehlt keinen Umriss. `re15_karte_r39_fenster` wirkt nur fuer
  0x1210/0x1211 Zone 0. Andere Raeume unberuehrt - bestaetigt durch Sweep (12 Blaetter pixelgleich), Tuer-Uebergaenge
  (132/132 gleich) und den 1090-Echtweg (0 Pixel verschieden).
- Gegen den aktuellen master: `git merge-tree --write-tree master HEAD` ohne Konflikt (master seit der Merge-Basis 22abaed9
  ohne Ueberschneidung mit den Pfaden der Spur).

## 9. Urteile
| Punkt | Urteil | Kurzbeleg |
|---|---|---|
| 1a Blaetter 1F/2F/3F/B2 nach den Vorlagen - "Keine Doppelwaende, Tuer immer genau auf Wand platziert, treppen nicht in der Wand" | **teilweise** | Vollzustand (Sweep der echten exe): 1F/2F 0 Klassen-Abweichungen, B2 nur die Kammer (249 px, alle im Kasten x186..201 y113..129, bewusst, Orchestrator-Entscheid), 3F die zwei begruendeten Treppen (H3); Doppelwand 0 auf B1..3F und in allen 43 Einzelzustands-Abzuegen; Treppen nur mit Sprossenenden an Waenden wie die Vorlagen. ABER: im normalen ersten Stand auf 1F (Hof -> Tuer -> 1050, echter Tuerweg gemessen) schwebt die Tuer 1030<->1050 neben der 1050-Wand im Leeren; ebenso 1030<->1040, wenn 1040 vor 1030 betreten wird (N2) |
| 1b "natuerlich muss der Spielermarker da entsprechend passen" | **erfuellt** | N1 behoben: ROOM1210 0 von 10 387 Rasterpunkten neben der Zeichnung (vorher 1 693), echte exe auf dem Abnahme-Weg Ring (191,117) im ROT der Kammer (vorher im Blau), gdigrab = FB-Abzug (13 px Blinkphase), 5 weitere Lagen exe = Sonde = Handrechnung, Querbalken VORHER y79 -> NEU y78 (beide Seiten), Elza-Raum 1211 zu/auf richtig; uebrige Raeume unveraendert (Sweep 12 Blaetter pixelgleich, Uebergaenge 132/132 gleich, Riegel gruen) |
| 1c B1 neu | **erfuellt** | Blatt 0 pixelgleich zu Abnahme 1 (dort erfuellt), Doppelwand 0, unit_r39_karten_b1 gruen |
| 1 gesamt | **teilweise** | wegen N2 (1a) |
| 2 ROOM1090 Leons Marker im aktuellen Raum | **erfuellt** | Nutzerpunkt Ring (183,72) im Rot; echter Weg Leon 10F0 -> Klettern -> Tuer: Ring (182,72) im Rot (0 px verschieden zu Abnahme 1); echter Weg Elza (PL04) 10F1 -> 1091: Ring (182,72) im Rot |

## 10. Maengel (nummeriert, nachpruefbar)
- **N2 (Punkt 1a "Tuer immer genau auf Wand platziert"; vorbestehend, keine Regression, in Abnahme 1 Hinweis H1):** Solange
  ROOM1030 (Eingangshalle) noch nicht gezeichnet ist, schweben seine Tuermarken neben der Wand des Nachbarraums:
  (a) Tuer 1030<->1050 `{ 2, 4, 186, 152, 3, 4, 6, 1 }` (re15_map_zones.h:753) als Balken x186 y150..154 LINKS neben der
  1050-Westwand x187; (b) Tuer 1030<->1040 `{ 2, 4, 119, 129, 3, 4, 5, 1 }` (re15_map_zones.h:730) als Balken x119 y127..131
  RECHTS neben der 1040-Ostwand x118. Ursache: beide Marken liegen auf der 1030-Linie (seit Runde 39 im Vollzustand die
  eine Wand, s_korr mit Traeger r4 = 1030), sind aber per auf_partner = 1 schon mit dem Nachbarn allein sichtbar; ohne
  1030 bleibt richtigerweise dessen eigene Linie (x187 bzw. x118) stehen.
  Erreichbar im normalen Spiel: der 1F-Plan liegt in ROOM1030 selbst (gen/kartenitems_r36.inc:1230), und der Weg ueber
  10F0 -> 1090 -> Hof fuehrt zuerst nach 1050.
  Pruefen (a), echter Tuerweg: `probe_r36_karte karte re15_card.mcr 1090 -10100 -1800 4200 1024 besucht:10F0:8400:-350
  besucht:1090:-13600:1300:5 besucht:1090:-10100:4200:1`, LOAD GAME (RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1),
  `RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT=W1,U0.3,W0.3,A0.1,W8 RE15_INV_OPEN_AT=40#1050 RE15_INV_FB_SHOT=karte.bmp
  RE15_INV_FB_SHOT_AT=60` -> debug.log `[aot] DOOR FIRE slot=0 ... spawn=(19850,0,-22300)`, `PC loaded room1050.rdt`; im
  Abzug x186 y150..154 gelb, (186,149)/(186,155) Grund, x187 y150..154 Wand. (b): Spielstand ROOM1040 (-21008,0,-13134)
  nur 1040 besucht -> x119 y127..131 gelb, x118 Wand, (119,126)/(119,132) Grund.
  Soll (Wortlaut "Tuer immer genau auf Wand platziert"): in jedem erreichbaren Kartenzustand
  liegt jeder gezeichnete Tuerbalken auf einer gezeichneten Wand (Riegel: Einzelzustaende 1050/1040 ohne 1030, Enden des
  Balkens auf Wand, keine Wand quer daneben). Wie (Tuer mit der Linie wandern lassen, eigene Linie des Nachbarn auf die
  1030-Linie legen, oder Sichtbarkeit an 1030 binden) ist eine Bau-Entscheidung; die uebrigen 39 Einzelzustaende sind
  frei von schwebenden Tueren (1210 allein ist nicht erreichbar, nur ueber 11E0).

## 11. Hinweise (kein Mangel)
- **HK (bewusst, dem Nutzer melden):** B2 weicht mit der ROOM1210-Kammer von mapB2_correct.png ab (Orchestrator-Entscheid
  Beta -> Retail; begehbar mit eigenen Kameraschnitten 6/7). Dossier "Fuer den Nutzer (Nachbesserung 2)" nennt es mit
  Bildern; eigenes Bild des echten Fensters: G39abn2/gdi_1210/gdi_kammer_x4.png.
- **H2/H3/H4/H5/H6** der Abnahme 1 unveraendert (Klemmung reserve 4; 3F-Treppen 1060/1170 bewusst anders als die Vorlage;
  1130-Wandnasen allein gezeichnet; 1170 Zone 1 nicht per CONTINUE abziehbar - ebenso hier 10F0/1170/11C0/1240 allein wegen
  der Eintritts-Szenen; 11A0 Band 1 auf Blatt 6).
- **H7:** Querbalken ROOM1210: der Marker steht im suedlichsten Streifen jetzt auf der Balkenlinie y78 (Ring-Mitte y77 im
  Balken). Der Balken ist gestaucht gemalt (8 px fuer 11,7 px Welt), feiner geht es mit dieser Kunst nicht.
- **H8 (vorbestehend, nicht Spur G):** 1 von rund 70 exe-Laeufen blieb nach dem Fensterstart stehen (debug.log endet nach
  `[pad] kein Controller gefunden`, Timeout 120 s; al_1050), die Wiederholung lief normal - der bekannte Start-Haenger
  (Memory reai-v2-offen-kein-fenster).

## 12. Ergebnis
**bestanden = false.** Der Mangel N1 der Abnahme 1 ist an der echten exe nachweislich behoben (Kammer gezeichnet, Marker
ueberall auf der Zeichnung, Querbalken-Rest erklaert und behoben, Elza-Variante richtig), ohne Nebenwirkung auf die
anderen Blaetter, Raeume und Tueruebergaenge; Punkt 2 und die Teile 1b/1c sind erfuellt; alle Gates (RE, Vertrag, Pfade,
gezielte Tests 99/99, merge-tree ohne Konflikt) halten. Punkt 1a bleibt **teilweise** wegen N2: im normalen ersten
1F-Zustand steht die Tuer zur Eingangshalle nicht auf der Wand (vorbestehend, aber genau der Wortlaut "Tuer immer genau auf
Wand platziert"; neu belegt mit echtem Tuerweg und Erreichbarkeit).
