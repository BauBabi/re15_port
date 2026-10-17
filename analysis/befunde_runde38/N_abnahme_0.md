# Runde 38 — Spur N "wasser": Abnahme 0 (unabhaengig)

Datum 2026-10-17. Baum `.claude/worktrees/r38_wasser`, Zweig `r38/wasser`, HEAD 0f1e4819 (Basis d70c234e).
Pruefer: Abnahme-Agent 0 (glaubt dem Bau-Agenten nichts, was nicht selbst gemessen ist). Bericht wird fortlaufend geschrieben.

## Nutzer-Punkt (Wortlaut, AUFTRAG.md)

> In Room 1260 und folgende fehlt noch der Wasser Effekt, wenn sich Leon oder Elza im flachen Wasser bewegen.

## 0. Stand / Bau

- `git diff master...HEAD --name-only` (Basis d70c234e): 24 Dateien; Code: `engine/src/wasser_r38.c` (neu),
  `include/re15_wasser_r38.h` (neu), Haken in `game_step_common.c` (+1 Bildanfang, FOOTSTEP-Block 15 Z. -> 1 Aufruf),
  `scd_vm.c` (op_aot_set Breite/Tiefe u16, 2 Z. + Kommentar), `platform/pc/main.c` (+3 Z. Zeichner-Fuetterung);
  Tests `tests/unit/test_r38_wasser.c`, `tests/unit/probes/r38_wasser.cmake` (GLOB-Sonde), `tests/integration/test_r38_wasser.cmake`.
- Code-Endstand a8e08251 = HEAD (`git diff --stat a8e08251 HEAD -- re15_port` leer).
- Bau (selbst): `local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `local_build.sh build` -> `ninja: no work to do`,
  `=== LOCAL-BUILD-OK (build)`. exe 2026-10-17 11:23:55 ist juenger als jede geaenderte Quelle (wasser_r38.c 10:37,
  game_step_common.c 08:43, scd_vm.c 09:04, main.c 16.10. 23:31); exe enthaelt `RE15_WASSER_LOG` und `zustand=%s` (Lauf-3-Stand).

## 1. Eigene Tests der Spur (gebauter Stand, seriell)

`ctest --test-dir re15_port/build -R "r38_wasser" --timeout 900 --output-on-failure` ->
`100% tests passed, 0 tests failed out of 9` (unit_r38_wasser_pfuetze/zeitfolge/trocken/stempel/lauf/aotbreite/zensus/
skriptlauf je 0.01 s, integration_r38_wasser 182.43 s, Lauf-Verzeichnis `build/tests/integration/r38_wasser_wd/lauf_d46043b6`).
Die Unit-exe einzeln (`test_r38_wasser.exe alle`) meldet 49 `ok`, `alle: 0 Fehler`; die Checks messen echte Werte
(Anker/Part/Tonsatz/Skala/Flags am echten ROOM1260/2090-RDT + PL00/PL00W01), keine Platzhalter.

## 2. Messung am gebauten Spiel (eigene Laeufe, Messkopie `re15_pc_abn0n_mess.exe`, md5 = re15_pc.exe)

Werkzeug: `scratchpad/abn0/lauf.sh` (Umgebung wie integration_r38_wasser: RE15_NO_INTRO, RE15_DEBUG_JUMP=<raum>@250,
RE15_PLAYER_POS, RE15_INPUT_SCRIPT_BASIS=spiel, RE15_WASSER_LOG, RE15_FX_LOG, RE15_FOOT_DEBUG, RE15_AUDIO_CAP_SYNC,
RE15_EXIT_AT, zusaetzlich RE15_FRAMEDUMP mit RE15_WINDOW_SCALE=1). Ton-Nachweis: Korrelation des Mitschnitts mit den
dekodierten VAGs der Raum-Bank (Skript `N_belege/vag_korr.py`, eigener Lauf; Mitschnitt-Tick = Spielbild + 254, selbst aus
den Energie-Spitzen 337/352/367 = F83/F98/F113 bestimmt).

### M1 Leon ROOM1260, Gehen in den Fleck P0 (FLR 0x97)
`lauf.sh m1_leon1260 leon 1260 "-8600,-7900,0,1" "W0.5,U1.5,W2" 60 140 RE15_FRAMEDUMP=80-125/1:fd_` -> rv=0, `EXIT_AT: Bild 140`.
```
F83  bild=7  lage=(-7804,-1800,-7246) boden=0x1d zweig=trocken edt=0x1d
F98  bild=22 lage=(-7068,-1800,-7246) boden=0x97 zweig=boden code=0x06050d00 part=4 anker=(-7555,-1873,-7385) quelle=zeichner edt=0x17
F113 bild=37 lage=(-7068,-1800,-7246) boden=0x97 zweig=boden code=0x06050d00 part=7 anker=(-7595,-1887,-7100) quelle=zeichner edt=0x17
debug.log: [foot] SE foot=7 type=29 vag=1 / foot=4 type=23 vag=7 / foot=7 type=23 vag=7
fx.log: id=6 sub=5 in 26 Bildern (F98-F110, F113-F125), frame=0 bei F=98 (Schrittbild)
```
Original-GDB (`N_belege/gdb_1260_gehen.txt`, gelesen): Schritt an der Wand Part 4 `anker=(-7555,-1873,-7385)`, Part 7
`anker=(-7595,-1887,-7100)`, TON 0x17 — Port Wert fuer Wert gleich.
Bild (Framedump, Ausschnitt 90..190 x 120..200, 5x): `scratchpad/abn0/m1_sheet_crop.png` — am hinteren Fuss erscheint ab F102
ein kleiner gruenlich-weisser Spritzer (Bildmitte ~(143,166) nativ; Original-POLY_FT4 laut B9 (135..148,159..172)), waechst bis
F104-F106, zerfaellt F108 in Tropfen, F110 weg. Vollbild `m1_sheet_full.png`. Form/Verlauf wie Vergleichsstreifen
`N_belege/vergleich_original_leon_elza.png` (Original-Zeile).
Ton (eigener Mitschnitt): F83 EDT 0x1d/VAG1 r=0.46; F98 EDT 0x17/VAG7 r=0.48; F113 EDT 0x17/VAG7 r=0.43; Kontrollfenster
F76/F91 (kein Schritt) alle r <= 0.08. => Wasser-Schritt-Ton im Fleck, Beton-Ton davor.
Pixelgenauer Bildvergleich (eigener Lauf `m1b_trilog`, `RE15_TRILOG=tri.log@120,140,170,190`, EXIT_AT 111): die 13 Quads in
TIM-Slot 37 (Effekt-Id 6) gegen die 13 Original-POLY_FT4 aus `N_belege/gdb_1260_paket.txt` (Schritt Part 4, wpos (-7555,-1873,-7385)):
```
c0=0  Port xy (140,166)-(143,169) uv (224,24)   Original xy (140,166)-(143,169) uv (224,96)
c0=1  Port xy (138,162)-(145,169) uv (184,32)   Original xy (138,162)-(145,169) uv (184,104)
c0=3  Port xy (136,160)-(147,171) uv (224,0)    Original xy (136,160)-(147,171) uv (224,72)
c0=4..12 Port xy (135,159)-(148,172) uv (184,48),(216,48),(0,0),(32,0)..(192,0)   Original gleiche xy, uv v+72
```
Alle 13 Bildschirm-Rechtecke identisch; uv = Datei-v + 72 (VRAM-Versatz des Originals, FUN_800194f8) in allen Zellen.
**Ergebnis M1: Effekt (Bild) + Wasser-Ton vorhanden, Lage = Original.**

### M2 Elza ueber den echten Tuerweg ROOM10B1 -> ROOM1261 (Fleck P5)
`lauf.sh m2_elza10b1_tuer elza 10B1 "-21370,3000,3072,0" "U0.6,A0.2,U3" 30 "150#1261" RE15_FRAMEDUMP=36-80/1:fd_` -> rv=0,
`EXIT_AT: Bild 150 in Raum 1261`; debug.log `Spieler-Familie PL04 (character=4, Elza-Bit=1)`, `[tuer] Sequenz ... DOOR16`.
```
F39 room=10B1 boden=0x1d zweig=trocken edt=0x1d                              (vor der Tuer)
F39 room=1261 lage=(-20250,-1800,-7375) boden=0x97 zweig=boden code=0x06050d00 part=7 anker=(-20133,-1865,-6834) edt=0x17
F63 room=1261 lage=(-20250,-1800,-8725) boden=0x97 zweig=boden code=0x06050d00 part=7 anker=(-20133,-1865,-8184) edt=0x17
F77..F137 room=1261 z -9775..-14275 boden=0x1d zweig=trocken edt=0x1d        (hinter dem Fleck)
[foot] SE foot=7 type=23 vag=7 (2x), danach type=29 vag=1; fx.log id=6 sub=5 in 26 Bildern
```
Bild `scratchpad/abn0/m2_sheet_crop.png` (8x um Elzas Fuesse): gruenliche Spritzer an den Stiefeln F43-F49 und F67-F70 (Elza
weit hinten im Gang, Spritzer entsprechend klein). **Ergebnis M2: Elza, echter Weg, Effekt + Wasser-Ton im Fleck, Beton ohne.**

### M3 Leon ROOM1260 LAUFEN durch P0 (Echtzeit-Lauf, a0 = 1)
`lauf.sh m3_leon1260_lauf leon 1260 "-8600,-7900,0,1" "W0.5,UX1.5,W2" 60 140 RE15_FRAMEDUMP=70-120/1:fd_`
```
F76  a0=1 boden=0x1d zweig=trocken edt=0x1e                                  (Lauf-Betonsatz 0x1d+1)
F88  a0=1 bild=12 boden=0x97 zweig=boden part=4 anker=(-7872,-2875,-7442) edt=0x18
F98  a0=1 bild=22 boden=0x97 zweig=boden part=4 anker=(-6548,-1891,-7302) edt=0x18
F110 a0=1 bild=34 boden=0x97 zweig=boden part=4 anker=(-7872,-2875,-7442) edt=0x18
[foot] SE foot=4 type=30 vag=1 / type=24 vag=7 (3x); fx.log id=6 sub=5 in 39 Bildern
```
Original-GDB Lauf (`N_belege/gdb_1260_lauf.txt`, gelesen): Bildwort 0x700c -> anker=(-7872,-2875,-7442), Bildwort 0x5000 ->
anker=(-6548,-1891,-7302), TON 0x18 (Pfuetze) / 0x1e (Beton) — Port Wert fuer Wert gleich (auch der hochgezogene Fuss
y -2875 ist Original). Bild `m3_sheet_crop.png`: Spritzer F90-F98 und F104-F110 sichtbar.

### M4 ROOM2000 — Fleck-Bereich im Raum 2000 ohne Kamera (kein Defekt dieser Spur)
Laeufe `m4_leon2000` (Band 0), `m4c` (Band 0), `m4e` (Band 1, Gang z -6355): WASSER_LOG zeigt `zweig=boden` an den Flecken,
aber fx.log enthaelt KEIN `id=6 sub=5` und Leon ist im Bild nicht zu sehen (Kamera bleibt Cut 0, RE15_CAM_TRACE `req(cam_id)=0`).
Ursache gemessen an den RVD-Daten (RDT+0x28, eigenes Skript): in ROOM2000 decken die Anker-Regionen aller 16 Cuts nur
z <= -17201 oder x <= -6502 ab; die Fleck-Streifen x -8190..6090 / z -13900..-1450 liegen in KEINER Cut-Region (in ROOM20B0
decken Cuts 11-14 genau diesen Gang ab: x -21907..8301, z -10800..-2199). Der Effekt wird dort wie im Original per Regionstest
@0x80053334 verworfen. Der Bau-Agent hatte 2000 mit Band 0 (y 0) gemessen ("Anker y -73/-87 ueber Boden 0"), der Gang liegt
auf Band 1 (Sprung-Spawn (4350,-1800,-20500), Tueren Band 1) — die 2000-Zeile seiner Raumliste ist also ein Log-Befund an
einer im Spiel nicht sichtbaren Stelle, kein Bildbeleg (Mangel an der Dokumentation, nicht am Code).

### M5 Leon ROOM20B0 Flut-Szene (Plc-Lauf, Skript)
`lauf.sh m5_leon20b0 leon 20B0 - - 30 120 RE15_FRAMEDUMP=14-80/1:fd_` -> rv=0.
```
F17 bild=0  lage=(2380,-1800,-6355) boden=0x97 zweig=boden part=4 anker=(2377,-1850,-6122) edt=0x17
F29 boden=0x1d trocken | F39 boden=0x17 (ohne Bit 0x80) zweig=trocken edt=0x17 (Wasser-Ton ohne Effekt, Original-Daten FLR[4])
F51 bild=34 lage=(-4420,..) boden=0x97 zweig=boden anker=(-3617,-2875,-6160) -> rel (803,-1075,195)
F61 bild=44 lage=(-6420,..) boden=0x97 zweig=boden anker=(-6941,-1891,-6300) -> rel (-521,-91,55)
fx.log id=6 sub=5 in 39 Bildern; [foot] type=23 vag=7 in den Flecken, type=29 vag=1 auf Beton
```
Relative Anker = Original-Lauf-Anker (gdb_1260_lauf.txt, um 180 Grad gedreht: (804,-1075,196) / (-520,-91,56)) auf +-1.
Bild `m5_sheet_full.png`: Spritzer am Fuss F21/F24 (Kamera nah), spaeter klein (Kamera fern). Cuts 11/12/13 wechseln mit.

### M6 ROOM2050 (Septic Pool) — Becken-Boden, Band 0
Raumaufbau gemessen: Laufsteg Band 3 (Sprung-Spawn (9100,-5400,4100)), die FLR-0x97-Flecken liegen im Becken (Band 0, y 0;
Cuts 10-13 zeigen den Beckenboden mit dunklen Pfuetzen, `m6f_cuts2.png`). Laeufe mit RE15_FORCE_CUT (nur Kamerawahl, kein Ablauf):
`m6i_leon2050_b0` (Cut 12) und `m6j_b0_cut13` (Cut 13), Lage (1100,2500) Blick 1024 Band 0:
```
F83 lage=(1100,0,1182) boden=0x97 zweig=boden part=7 anker=(1246,-87,1708) edt 0x17 | F98 lage=(1100,0,57) zweig=boden part=4
F113.. boden=0x1d trocken; [foot] type=23 vag=12 (Raum-Bank 2050: EDT 0x17 -> VAG 12), type=29 vag=1
```
Bild `m6j_cut13_sheet.png`: Spritzer hinter Leons hinterem Fuss auf der Pfuetze F88-F92 und F104-F108 sichtbar. In Cut 12
(`m6i_sheet.png`) liegt der Spritzer (Projektion sx 83 sy 176 laut FX_LOG) hinter der Vordergrund-Kiste — verdeckt.
Der echte Weg vom Laufsteg ins Becken wurde nicht gefunden (Laufsteg x 9100 endet bei z -4225 an einer Wand); M6 belegt die
Mechanik am Beckenboden, nicht den Weg.

### M7 Leon ROOM2090 Becken (sce-8-Stempel, Watschritt)
`lauf.sh m7_leon2090 leon 2090 "-8438,-25550,0,0" "U1.6,W1" 30 80 RE15_FRAMEDUMP=34-56/1:fd_`
```
F38 lage=(-7838,0,-25550) boden=0x1d stempel=-1620 zweig=stempel code=0x06032000 part=6 anker=(-8071,-836,-25376) edt 0x1a
[foot] SE foot=7 type=26 vag=3; fx.log id=6 sub=3 in 13 Bildern (sx 90, sy 107..112)
```
Anker relativ (-233,-836,+174) = Original-GDB (`gdb_2090_becken.txt`: Leon (-8433,0,-25503) -> anker (-8666,-836,-25329)).
Bild: der Sub-3-Spritzer ist im dunklen Becken neben den umherschlagenden Spinnenbeinen (EM25) visuell nicht eindeutig
abzugrenzen; ein Original-POLY_FT4-Mitschnitt fuer Sub 3 liegt nicht vor (Beobachtung, s. Maengel/Hinweise).

### M8 Leon ROOM20A0 Band-1-Zone (Stempel -540)
`lauf.sh m8_leon20a0 leon 20A0 "8000,-25000,0,1" "W0.5,U2,W1" 60 150`
```
F83  lage=(8600,-1800,-25000) stempel=-540 zweig=stempel code=0x06032000 part=6 anker=(8367,-2636,-24826) -> rel (-233,-836,174)
F98  lage=(9725,..)           stempel=-540 zweig=stempel part=3 anker=(9509,-2637,-25166)                -> rel (-216,-837,-166)
[foot] type=26 vag=3 (4x); fx.log id=6 sub=3 in 52 Bildern
```
= Original `gdb_20a0_band1_zone.txt` (rel (-233,-836,+174) / (-216,-837,-166), TON 0x1a).

### M9 Elza ROOM2091 Becken / M10 Elza ROOM1261 Laufen
```
M9  F39 room=2091 lage=(-7763,0,-25550) stempel=-1620 zweig=stempel code=0x06032000 part=6 anker=(-7963,-799,-25411); type=26 vag=3; sub3 13 Bilder
M10 F76 a0=1 trocken edt 0x1e | F87 part=4 anker=(-7884,-2797,-7422) | F98 part=7 anker=(-7896,-2783,-7021) | F109 part=4 (-7884,-2797,-7422)
    alle zweig=boden edt=0x18; [foot] type=24 vag=7 (3x); sub5 39 Bilder
```
Elza (PL04, `Elza-Bit=1`) in beiden Zweigen. M10: ALLE drei Elza-Laufschritte liegen am hochgezogenen Fuss (y ~1000 ueber dem
Boden) — gleicher Mechanismus wie Leons Original-Bild-12-Schritt; fuer Elza nicht gegen das Original gemessen (OFFEN des
Bau-Agenten, bestaetigt).

### M11/M12 Rueckwaerts / Drehen auf der Stelle im Fleck P0 (Nebenfaelle "bewegen")
```
M11 "W0.5,D2.5,W1" ab (-6900,-7900): F83 bild=7 fuss=4 lage=(-7628,..,-7900) boden=0x97 zweig=boden; danach Beton trocken; sub5 13 Bilder
M12 "W0.5,L2,W1"  ab (-7200,-7900): F83/F98/F113/F128 im Fleck je zweig=boden (fuss 7/4), [foot] type=23 vag=7 (4x); sub5 52 Bilder
```
Beide laufen ueber denselben Fusstakt (Motion 105); keine Abstuerze, keine Doppel-Ausloesung.

### M13/M14 ROOM2070/2071 "TUN. FLOOD DRY" ueber den ECHTEN Tuerweg (knoecheltiefes Wasser, Spiegel -200)
Selbst ermittelt (SCD-Dump `tools/scd_dump_room.py`): ROOM2070 main00 setzt die sce-8-Zone slot 6 (x -19400..5450,
z -21000..-17450, Band 0, Spiegel -200) NUR in `Switch work_vars[0x0a]` (= Ankunfts-Cut) Fall 9 und 12 (@0xc2e / @0xc94).
Der Debug-Sprung kommt mit Cut 0 an -> keine Zone (daher die Dossier-Zeile "im Sprung-Zustand keine Zone aktiv"). Tueren mit
Ankunft Cut 9/12 (eigener Scan aller Door_aot_set): ROOM2040/2041 slot 4 (Band 5) -> 2070/2071 Cut 9 (4650,0,-19200);
ROOM11A0/11A1 slot 0 -> Cut 12 (-18650,0,-19300). Gemessen ueber 2040 -> 2070 (Leon) und 2041 -> 2071 (Elza):
```
lauf.sh m13_leon2040_tuer leon 2040 "-20450,-19800,2048,5" "W0.3,A0.2,W0.5,U3" 30 "160#2070"
  [tuer] Sequenz Archiv 2 DOOR0A ... ; F69..F144 room=2070 je stempel=-200 zweig=stempel code=0x06032000 part=6/3
  anker rel (+232,-836,-175) bzw. (+215,-837,+165) (Blick 2048, = Original-Relativanker gedreht); [foot] type=26 vag=3 (6x); sub3 78 Bilder
lauf.sh m14_elza2041_tuer elza 2041 ... "160#2071"
  Elza-Bit=1; F70..F144 room=2071 je stempel=-200 zweig=stempel part=6/3 anker rel y -799/-783; [foot] type=26 vag=3; sub3 78 Bilder
```
Bild `m13_sheet_full.png`: heller gruen-weisser Spritzer neben Leons Knie F134-F138 (Kamera nah), F74/F78 hinter der Spinne.
Damit ist das "flache Wasser" der Kanalraeume ausserhalb der Pfuetzen (Stempel-Zweig) ueber den echten Weg fuer Leon UND Elza belegt.

### Gegenprobe v0.8.25 (Paket `release/pkg-win/re15_port_v0.8.25`, nach scratchpad/v0825 kopiert, md5 7aa892fc...)
Gleicher Lauf wie M1 (`ABN0_EXE=.../v0825/re15_pc.exe lauf.sh v0825_leon1260 ...`): `[foot] SE foot=4 type=151 vag=7` (Ton da),
KEIN fx.log (kein einziger Effekt gezeichnet), kein Spritzer in F97-F110 (`v0825_sheet_crop.png`, gleiche Posen wie M1).
=> Der Nutzer-Befund war real; der Spurstand behebt ihn.
Schritt-Ton-Zeitpunkt (Mitschnitt-Energiespruenge, eigenes Skript): v0.8.25 Ticks 336/351/366, Spurstand 337/352/367 = genau
1 Spielbild spaeter — wie im Dossier angekuendigt und wie das Original (GDB: Schritt bei +0x95 = 23 mit Bildwort[22];
anim_set `sw a2,360(t0)` @0x8001f36c vor dem Vorschub `sb v0,149(v1)` @0x8001f61c selbst disassembliert).

## 3. RE-Gate (selbst disassembliert, `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`, info/Re1.5/PSX.EXE)

Stichprobe weit ueber 3 Adressen — jede zitierte Stelle enthaelt das Behauptete:

| Behauptung (Code/Header) | eigene Disassemblierung |
|---|---|
| Wasser-Bit 0x80, Effekt 0x06050D00 | `800456ac andi v0,s0,0x80` / `800456b0 beq v0,zero,0x800456dc` / `800456b8 lui a0,0x605` / `800456bc ori a0,a0,0xd00` |
| Versatz (0,100,0), Gier +0x6a | `800456b4 ori v0,zero,0x64` (Delay-Slot) / `800456c4-cc sw zero,16 / sw v0,20 / sw zero,24` / `800456d0 lh a1,106(a1)` |
| Stempel +0x88, Gier 0x800, Watsatz -3 | `800456dc lh v0,136(a1)` / `800456e4 beq v0,zero,0x80045718` / `800456e8 ori a1,zero,0x800` (Delay-Slot) / `800456ec addiu s0,a0,-3` |
| Effekt 0x06032000, Part a1-1 | `800456f0 lui a0,0x603` / `800456f4 ori a0,a0,0x2000` / `800456f8 addiu a2,a2,-108` (Part-Stride: `8004567c-a4` = a1*172 = 0xac) |
| Tonindex, Objekt -> 9 | `80045718 andi v0,s0,0x7f` / `80045724 addu a0,s3,v0`; `80045774 lw v0,-13764(v0)` (0x800aca3c) / `80045780 andi v0,v0,0x4000` / `80045790 ori s3,zero,0x9` |
| FLR nur x/z | FUN_800437d4 `lh`/`subu`/`sltu` auf x (+2) und z (+4), Typ `lbu v0,0(a2)` (+10 = Eintrag+8); kein y |
| Echtzeit-Gehen: Schritt vor anim_set/pos_advance | `8003252c lw v0,-13380(v0)` (0x800acbbc = Spieler+0x168) / `8003253c andi v0,v1,0x4000` / `80032544 addu a0,zero,zero` / `80032548-60` a1 = 7-3*Bit12 / `8003255c jal 0x80045630` / `80032578 jal 0x8001f314` / `800325ec jal 0x800245d8` |
| Echtzeit-Lauf a0 = 1 | `80032850 ori a0,zero,0x1` (Delay-Slot des beq) / `80032868 jal 0x80045630` / `8003295c jal 0x800245d8` |
| Plc-Gehen / Plc-Lauf a0 = 0 | `80030c1c andi v0,v1,0x4000` / `80030c28 addu a0,zero,zero` / `80030c3c jal 0x80045630` / `80030c5c jal 0x800245d8`; `80030e60 lw v0,-13380(v0)` / `80030e70 andi` / `80030e7c addu a0,zero,zero` / `80030e90 jal` / `80030eb0 jal 0x800245d8`. Zusatz: der Sub-Zustand 1 des Plc-Gehens ruft den Schritt @0x80030bdc VOR anim_set @0x80030bf8 (ohne pos_advance) — gleiche Regel |
| Bildwort des gezeichneten Bilds | anim_set FUN_8001f314: `8001f344 lbu v0,149(t0)` -> `8001f36c sw a2,360(t0)` (+0x168 := &Wort[+0x95]); Vorschub erst `8001f610 lbu v0,149(v1)` / `8001f618 addiu` / `8001f61c sb v0,149(v1)` |
| +0x88 nach dem Dispatch loeschen | Spielertakt 0x80031c44: `80031c64 addiu s0,v0,720` (0x800ac784+0x2d0 = Spieler 0x800aca54) / `80031cb4 jalr v0` (Dispatch) / `80031da4 sh zero,-13604(at)` = 0x800acadc = Spieler+0x88; Objekt-Bit: `80031cdc and v0,v0,v1` (v1 = ~0x4000) / `80031d24 ori v0,v0,0x4000` |
| Reihenfolge Hauptschleife | `8001ce04 jal 0x8001a50c` / `8001ce0c jal 0x80031c44` / `8001ce14 jal 0x8002bd44` / `8001ce1c jal 0x800436a8` (AOT-Scan) / `8001ce2c jal 0x80019e20` (ESP) |
| sce-8 stempelt +0x88 | Handler 0x8004330c: `80043314 lhu v0,0(a0)` / `8004331c sh v0,136(v1)` |
| Breite/Tiefe vorzeichenlos | FUN_80042b64: `80042b70 lhu a0,4(a1)` / `80042b78 sltu a0,a0,v1` / `80042b8c lhu a0,6(a1)` / `80042b94 sltu`; Aufrufer FUN_80042bac `jal 0x80042b64` @0x80042ecc / @0x80042f20 |
| Band-Sperre | `80042cac lbu v0,2(s0)` / `80042cb4 andi v0,v0,0x80` / `80042cc0 lbu v1,130(s1)` / `80042ccc bne v1,v0,0x8004301c` |

Zensus selbst nachgerechnet (`N_belege/breite_zensus.py`): 1859 Kurzform-Saetze (0x2c 1046, 0x3b 649, 0x50 164), Breite oder
Tiefe > 32767 genau 8 Saetze, alle op 0x2c, alle sce 8 (11A0/11A1 @0x1108, 1260/1261 @0x11ca/@0x11de, 2020/2021 @0x8ac).

Diff-Suche (`git diff master...HEAD -- re15_port`, nur `+`-Zeilen): "deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|
approx|geschaetzt|vermut" -> **0 Treffer**. Einziger getenv: `RE15_WASSER_LOG` (Mess-Schiene, kein Verhaltensschalter).
Konstanten im Header (`include/re15_wasser_r38.h`) tragen alle `@0x…`. Ohne Adresse sind nur Port-Interna: Gueltigkeit der
Zeichner-Fuetterung "selber Raum und <= 2 Bilder alt" (`gez_gueltig`, wasser_r38.c:119-123) und `WASSER_FOLGE_MAX 8` — beides
Architektur des Ports (Zeichner nach dem Spielschritt), nicht als PORT-WAHL beschriftet (Hinweis H2, kein Original-Verhalten).

Erklaert der Fix den Befund? Ja: v0.8.25 hatte nur den Ton-Teil von FUN_80045630 (game_step_common.c alter FOOTSTEP-Block,
`re15_audio_footstep(foot, re15_rdt_floor_sound(...))`); der Effekt-Zweig @0x800456ac-0x80045710 fehlte vollstaendig — die
Gegenprobe v0.8.25 zeichnet in derselben Lage keinen einzigen Effekt, der Spurstand den Original-Spritzer.

## 4. Vertrag / Pfade / Nebenwirkungen

- Pfade (`git diff master...HEAD --name-only`): nur eigene Dateien + `analysis/befunde_runde38/N_*`; kein `release/`,
  `platform/android/`, `shared_assets/PSX/`, keine Test-CMakeLists (Registrierung ueber die GLOB-Sonde `tests/unit/probes/r38_wasser.cmake`).
- Gemeinsame Dateien (VERTRAG §1.4 erlaubt): `scd_vm.c` (+4/-2, Kommentar "Runde 38 Spur N"), `game_step_common.c` (+1 include,
  +1 Bildanfang-Aufruf, FOOTSTEP-Block 15 Z. -> 1 Aufruf + 3 Z. Kommentar), `platform/pc/main.c` (+3). Klein, kommentiert.
- Bank-9-Bits / Nachrichten-Ids / Items / Dokumente: keine belegt (grep im Diff: kein Flag-/MSG-Zugriff). Vertrag haelt.
- Merge-Probe: `git merge-tree --write-tree` gegen `master` und gegen `r38/integration` -> konfliktfrei. Kein anderer Spurstand
  (integration, wasserzombie) liest `water_y`; die u16-Breite wirkt nur auf die 8 sce-8-Zonen (s. Zensus).
- Nebenwirkung "alle Schritt-Toene 1 Bild spaeter": gemessen (v0.8.25 336/351/366 -> 337/352/367) und am Original belegt (s. M1/RE-Gate).
- Beide Szenarien: Leon (M1/M3/M5/M7/M8/M11/M12) und Elza (M2 echter Tuerweg, M9, M10, integration elza1261/elza20b1).
- Hinweis Release: `engine/src/wasser_r38.c` ist eine NEUE Engine-Datei — der Android-Bau friert die GLOB-Liste ein
  (app/.cxx-Cache, Memory reai-v2-android-glob-cache); beim Paketbau neu konfigurieren, sonst fehlen die Symbole beim Linken.

(in Arbeit)
