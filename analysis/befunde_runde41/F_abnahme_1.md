# Runde 41 — Spur F "gatorszene": Abnahme 1 (unabhaengig)

Stand: Zweig `r41/gatorszene` HEAD `2bcda663` (Basis r41/integration `9f10bdb0`, Abnahme 0 auf `76ad856d`), Baum
`.claude/worktrees/r41_gatorszene`. Pruefer: Abnahme 1 (2026-10-25). Ich glaube dem Bau-Agenten nichts, was ich nicht selbst
am gebauten Stand gemessen habe. Abgeschlossen 2026-10-25.

## 0. Bau und Lektuere

- Gelesen: AUFTRAG.md (bis Entscheid F-E1 2026-10-25 02:51), VERTRAG.md, Dossier F_gatorszene.md (Abschnitt Nachbesserung 1
  M1-M6/N1), F_abnahme_0.md, `git log master..HEAD`, `git diff 76ad856d..HEAD` (Nachbesserung: 18 Dateien, +1145/-60),
  `git diff 9f10bdb0..HEAD --stat` (F gesamt: 41 Dateien). `git diff master --stat` zeigt zusaetzlich Loeschungen von
  Dateien, die master NACH der Basis bekam (H/E/K/C-Dokumente, kollision_r41.c, rennstopp_r41.c, werkstatt11b1_r41.c,
  synchro/.../main46.wav usw.) - das sind keine Aenderungen von F (Merge-Basis 9f10bdb0).
- Bau: `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` ->
  `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`. exe 2026-10-25 03:58:26, gatorszene_r41.c 03:58:23 (obj 03:58:25)
  = HEAD-Stand (Arbeitsbaum sauber); die drei nach 04:09 geaenderten Testquellen sind in ihren Test-exe (04:10) gebaut.

Messordner (scratchpad, nicht im Repo):
`C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\F41a1`
(`runs/<lauf>/debug.log|state.log|fd_*.ppm`, Helfer `tools/lauf.sh` = eigene exe-Kopie `re15_pc_f41a1_<lauf>.exe`, `timeout`,
danach geloescht). Alle Laeufe mit `RE15_WINDOW_SCALE=1`, Bilder nur per `RE15_FRAMEDUMP`.

## 1. Gezielte Tests

### 1.1 Lauf 1 (151 Tests, `-j4 --timeout 900`, Regex `F41a1/regex.txt`)
Regex = Regex des Bau-Agenten + Kamera-/Zonen-Riegel der Kamera-Uebergabe (`unit_camera_view`, `unit_cut_old_displayed`,
`unit_r34n_b_cut_old`, `prop_zone_dispatch`, `prop_scd_pc_advance`, `unit_r38_montage_kameras`) + alle Wasser-Riegel der
Runde 38 (`unit_r38_wasser_*`, `integration_r38_wasser`; F legt den Schluessel per `re15_aot_water_at` in die Wasserzonen)
+ Tuer-Riegel (`unit_r31_tuer_ton`, `unit_r35_cut10f0_tuerton`, `unit_r39_karten_tueren`, `integration_r36_tueren`).
Ergebnis: **`99% tests passed, 1 tests failed out of 151`** (Total 592.62 s). Rot: `integration_r38_wasser`.
Einzeln nachgefahren (`ctest -R "^integration_r38_wasser$"`): **wieder rot** (`0% tests passed, 1 tests failed out of 1`),
deterministisch, kein Last-Flattern:
```
CMake Error at .../test_r38_wasser.cmake:46 (message):
  r38_wasser [leon2090]: kein wasser.log (RE15_WASSER_LOG/RE15_FX_LOG) in .../r38_wasser_wd/lauf_902e6836/leon2090
```
debug.log dieses Teil-Laufs: `[debug-menu] JUMP -> 209 ALLIGATOR POOL (ROOM2090) spawn=(-8438,0,-25550) cut=0`,
`[gatorszene] Folge A ROOM2090 F250 ...`, `[gatorszene] Folge P ROOM2090 F250 wert=1/0` -> die Ankunftsszene (Programm 1,
Riegel Set(2,7,1)/(1,27,1)) haelt Leon fest, das Eingabeskript "U1.6,W1" ab Bild 30 bewegt ihn nicht, kein Watschritt bis
EXIT_AT 80. Vorher gruen: B_re2gator.md:428 (`integration_r38_wasser Passed 180.00 sec`), A_abnahme_0.md 9.1 (Passed
206.65 s), I_belege/nb1_ctest_integration.log (Passed 258.68 s). Der Riegel stand weder in der Regex des Bau-Agenten noch in
der von Abnahme 0 (dort nur `unit_r38_wasser_aotbreite`). -> Mangel M1 (s. 6).

### 1.2 Lauf 2 (60 Tests: weitere Riegel, die ROOM2090/2091 betreten oder durch das Tor 20A0 -> 2090 gehen)
Gefunden per `grep -rlE "209[01]" re15_port/tests` + `ctest -N`: `^(integration_r35_granate|integration_r37_itemband|
integration_r41_wasser20b0_npc|integration_r38_wasserzombie|unit_r35_granate|unit_irons_mittelmodell|unit_r34_reaktion|
unit_1210_arme|unit_r30_irons_tisch_licht|unit_r35_affen_.*|unit_r36_szenenleon_.*|unit_r36_ziel1190|unit_r37_pri1100_.*|
unit_r38_npckante_.*|unit_r38_wasserzombie|unit_r41_wasser20b0_.*)$`, `-j4 --timeout 900`. Ergebnis:
**`97% tests passed, 2 tests failed out of 60`** (927.77 s): rot `integration_r37_itemband` (923.15 s) und
`integration_r41_wasser20b0_npc` (466.06 s), beide wegen des Tors (s. 6); gruen u. a. `integration_r35_granate` (178.73 s, Lauf
"gator" in 2090), `integration_r38_wasserzombie` (152.38 s). Die Ursache beider Fehlschlaege ist deterministisch (Tor-Sperre,
nachgestellt in den Einzellaeufen IB1 = Wegplan von weg2090 und LA1 = Eingabe von landung_leon: `ROOM20A0 Slot 0 (Tor)
gesperrt`, `Folge Z ROOM20A0 F27/F215/F245`, kein DOOR FIRE, `EXIT_AT: Bild 600 in Raum 20a0`), kein Last-Flattern.

## 2. Punkt 1a — Ankunftsszene (Leiter 2080 -> 2090; Wortlaut + Nachtraege 6/17/20/21)

### 2.1 Szenenfolge (Leon L1, Elza E1)
Befehl L1: `tools/lauf.sh L1 400 RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
RE15_SET_FLAG=3:245 RE15_STATE_LOG=state.log RE15_DEBUG_JUMP=2080@250 RE15_FIRE_AOT=1@40#2080 RE15_EXIT_AT=700#2090
RE15_FRAMEDUMP=300-700/2:fd_ RE15_AUDIO_CAP_SYNC=audio.raw` (Leitertuer ROOM2080 Slot 1 @0x00BC2, selbst gelesen:
`3b 01 02 31 08 00 4e da da 99 e8 03 e8 03 f8 df 00 00 32 9c 00 00 01 09 00` -> 2090 (-8200,0,-25550), Cut **00**).
debug.log L1 (Auszug): `ROOM2090 Slot 1 (Tor) gesperrt` -> `Folge A F40` -> `P 1` -> `T F98 wert=0/1` (Knall-Bank Satz 0)
-> `P 2 ROOM2080 F142` + `M 8/2` -> `P 3 F156` -> `S F53` -> `G 1/7` -> `G 2/0 F176` -> `K 13 F189` -> `G 3` -> `G 4 F310` ->
`V F369` (Leiche + Schluessel weg) -> `G 5` -> `G 6 F376` -> **`C ROOM2090 F405 wert=0/1`** + `Kamera-Uebergabe: Cut 0
(Eintritts-Cut), Spielfigur im Bild (Fuss-Bildpunkt 88,126)` -> `G 7 F405` -> `B 1 F407` + `[bgm] stage=1 room=09
entry=FF3C -> MAIN3C` + `[bgm] loaded: 1 VAGs, SEQ 100B` (= RE2 MAIN1A-Container). 4 x `Spinne 0x25 ... verworfen`
(2 je 2090-Besuch). Elza E1 (`RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1`, `[pl] Spieler-Familie PL04`, 2081/2091):
dieselbe Folge, dieselben Bildnummern, `C ROOM2091 F405 wert=0/1`.

- Knall (Nachtrag 17), L1 audio.raw (`RE15_AUDIO_CAP_SYNC`, Takt = 1470 Stereo-Frames): Spitzen je Takt bis Takt 572 6000-7640,
  ab **Takt 573 18920**, 575 19687, 580 19005, abklingend bis 593 8543 = derselbe Befund wie Abnahme 0 (Satz 0 der Knall-Bank,
  `Folge T ... wert=0/1`). Mit `RE15_NOAUDIO=1` (L2/E2) steht `wert=0/0` (Bank nicht ladbar ohne Ton - erwartet).
- 2080-Einstellung (Nachtrag 20): `P 2 ROOM2080 F142` + `M ROOM2080 F142 wert=8/2` (8 Szenenfiguren, Cut 2); die Riegel
  `integration_r41_gatorszene_leon`/`_elza` (Lauf 1 gruen: 8 x `t=16`, keiner westlich x -16650, >= 4 gelaufen) messen das;
  Code dieser Teile seit Abnahme 0 unveraendert (`git diff 76ad856d..HEAD` beruehrt nur Wende, Clip-11-Kommentar,
  Kamera-Uebergabe, Schluessel-Lage/-Neigung, Tor-Ton).
- Wende (M6) am state.log L1 (Aktor 35, t=23): F369 r1024 (Clip 0) -> F370 r1099 -> 1174 -> 1249 -> 1324 -> 1399 -> F375 r1474
  -> **F376 r1505** (Rest 1505 - 1474 = 31: `(75 + 31) & 0xfff = 106 < 150` -> einrasten) und im selben Bild Clip 10 (Schrei).
  RE2 FUN_8001569c selbst disassembliert (re2_disasm.py PSX.EXE): `800156a8 sll a2,a2,16` / `800156ac bgez` (negative Rate ->
  Ziel + 0x800 @0x800156b8-bc), `800156c8 subu v0,t0,v1` / `800156cc addu v0,a3,v0` / `800156d0 andi a1,v0,0xfff`, `800156d8 sra
  v0,v0,15` (= 2 x Rate) / `800156dc slt` -> `sh t0,118(a0)` (einrasten), sonst `sh v1,118(a0)` (Gier - Rate) und bei
  `sltiu v0,a1,0x801` @0x800156f0 + 2 x Rate @0x800156fc-704 = ga_drehen Zeile fuer Zeile. EM23 `80102604 jal 0x8001569c` /
  `80102608 addiu a2,zero,75`. Notbremse `GSZ_WENDE_MAX = ceil(0x800/75) = 28` als PORT-WAHL gekennzeichnet und hergeleitet;
  gemessen griff sie nicht (7 Bilder).
- N1 (Spur-B-Programme wieder an), L1 gator_boss.log: `KAMERA SZENE "gatorszene" Ende`, `SZENE zurueck -> Cut 1 F405`,
  `SZENE frei ... Cut 1 F405` (B's Rueckschnitt), state.log F405 cam=0 (F's Cut_chg im selben Bild), spaeter
  `KAMERA FINISHER FINISHER RE2-Cut 7 -> Cut 6 F680 ... (Buehne)` / `FINISHER Halt Cut 6 F681` = B's Finisher-Programm laeuft
  nach der Szene wieder (Abnahme 0 hatte das nicht gemeldet). Bestaetigt.

### 2.2 Abnahme-0-Mangel M1 (Kampfbeginn ohne Spielfigur) — gemessen
- state.log L1, 5. Raumbesuch (F1-F700): cam=0 F368-F663 durchgehend (264 Bilder ab F400), Leon (-8392,-25550) bis F450,
  pm 2 -> 1 (F406) -> 0 (F420); erster Treffer F466 (hp 100 -> 50) - jetzt IM Bild. Danach cam 7 (F664) / 6 (F680) =
  Spur-B-Programme (Leon ohne Eingabe wird gefressen, hp -50 F699) - die Programme laufen wieder (N1).
- Bilder `F41a1/L1_kampfstart.png` (F370/390/404/406/420/440/460/470): Leon an der Leiter links im Bild, Alligator davor.
- Elza E1 mit JEDEM Bild F396-F520 (`RE15_FRAMEDUMP=396-520/1`): cam=0 in allen 125 Bildern; Hintergrund-Ausschnitt
  (150..320 x 0..60) F396-F414 bitgleich zu F400 (kein Cut-1-Bild bei F405, kein Zwischenbild), ab F415 Versatz =
  Bildschirm-Schuetteln des Schreis. Bogen `F41a1/E1_kampfstart.png` (F404/405/406/412/418/420/466/500): Elza im Bild.
- Mechanismus selbst disassembliert (RE1.5 PSX.EXE): FUN_80014230 `lbu v0,-1099(v0)` = Cut-Byte 0x800afbb5 @0x8001423c,
  `lbu v1,22(a0)` @0x80014254, `bne v1,v0` @0x8001425c, Schleife `lbu v1,0(s0)` / `lbu v0,-1099(v0)` / `beq v1,v0` @0x800142c0-d0
  = nur Zonen mit cam_from = angezeigter Cut. Raumlader `lbu v1,10(a0)` @0x8001d930 / `sb v1,-1099(at)` @0x8001d940 = Cut aus
  der Tuer-Nutzlast. Cut_chg LAB_800402a0: `lbu a1,4068(a1)` @0x800402c0 (alter Cut), `ori v0,v0,0x100` @0x800402d4,
  `sb a1,16251(at)` @0x800402e4, `sh a2,4072(at)` @0x800402ec, `sb v0,21591(at)` (=1) @0x800402f4, `sh a0,4068(at)` @0x800402fc,
  `jal 0x800142f4` - deckt sich mit kam_uebergabe (cam_id_prev, cam_id, pending, work_vars[0x0C]/[0x0A], cut_auto 0).
  ROOM1150.RDT @0x12C4 `29 05` (Cut_chg 5), @0x12DC `09 0a 14 00`, @0x12E0 `42 00` (Plc_ret), @0x12E2 `3c 01` (Cut_auto 1),
  @0x12E4 `22 02 07 00`, @0x12E8 `22 01 1b 00` (xxd) = zitierte Form.
- **M1: behoben** (Leon und Elza).

### 2.3 Abnahme-0-Mangel M5 (Puls gesaettigt) — gemessen
L2 (Leon) / E2 (Elza): wie L1/E1, aber `RE15_EXIT_AT=136#2090|2091 RE15_FRAMEDUMP=0-136/1:fd_`; Cut 1 ab F7 (state.log
cam=1 F7-F136). Schluessel-Ring F90: 8 Pixel >= 55 im Kasten (123,127)-(130,136) (`F41a1/L2_key_zoom.png`, 6-fach:
Ring mit Bart ueber dem Ruecken der Leiche). Hellster Pixel im Ausschnitt 112..146 x 118..139 (`tools/region.py`):
F61 73 - F70 84 - F79 95 - F88 106 - **F91 108** - F94 105 - F103 94 - F112 84 - **F121 73** - F130 83; ebenso F31 108 / F61 73.
Periode 60, Dreieck, Hoechstwert nur in EINEM Bild je Periode (F31, F91), max/min = 108/73 = **1,48** (Abnahme 0: 92 -> 112 =
1,22 mit Plateau ueber ~34 von 60 Bildern). Elza E2: Werte Bild fuer Bild gleich (F31 108, F61 73, F91 108, F121 73).
Blink-Folge selbst disassembliert (TITLE.BIN FUN_801028ec): `sltiu v0,v1,0x1f` @0x801028fc, `addiu a0,a0,2` @0x8010290c,
`addiu a0,a0,-2` @0x80102910, `ori v0,zero,0x3c` @0x80102918, Reset `ori a0,zero,0x80` @0x80102928 -> Spitze 0x80 + 31 x 2 =
0xBE, Periode 60. PC-Kappung render_pc.c:2731 `s = (v * 0xFF + 0x40) / 0x80; return s > 0xFF ? 0xFF : s` = gelesen.
Folge (vom Bau-Agenten genannt, gemessen): in Ruhe dunkler (73 statt 92). **M5: behoben** (Leon und Elza).

## 3. Punkt 1b — "Wenn der Aligator danach tot ist, soll der Schluessel in der Naehe seine Leiche sein und blinken"

Je Lauf ein Zwilling mit `RE15_SET_FLAG=...,9:112` (Taken-Bit vorab -> kein Schluessel), sonst bitgleiche Eingaben; Vergleich
Bild fuer Bild (`tools/diffserie.py`, jede Abweichung zaehlt).

| Lauf | Weg | Kamera bei der Lage (debug.log) | Lage / Abstand | Bilder mit Schluessel | Kasten | hellster Pixel |
|---|---|---|---|---|---|---|
| T1m/T1o Leon | Todesrolle `RE15_GB_TEST=3 RE15_R39_MESS_SPIELER_HP=100`, Sprung 2090, Bits 110 | `Kamera 0 (laufende)` | (-1300,-1729,-24000), Tiefe 4835, `3061 von der Leiche (22 Punkte, Naehe <= 3072)`, rx -416 rz 576, Prim 86 -> 128 | **96 / 96** (F470-F660, vorher 0) | (175,124)-(178,129), **4 px** | 75 -> 111, Dreieck, Spitze F630/F632 |
| K3m/K3o Leon | echter R1-Kanistertod mit NATUERLICHER Kamera: Start (-1000,-25550) Blick Ost, `U1.2` ueber den RVD-Streifen (cam 0 -> 1 bei x -475, state.log F22), dann 12 Schuesse (`RE15_GB_TEST=5 RE15_GATOR_KANISTER=4430,-26160 RE15_GIVE=3:15,21:50 RE15_EQUIP=3`) | `Kamera 1 (laufende)`; gator_boss.log `KANISTER angehaengt F18`, `R1 start`, `R1 ende (Leiche) pos=(3413,-25001)`, `R1 frei ... Cut 1 F114` | (-1600,-1717,-26900), Tiefe 5766, 2985 von der Leiche | **183 / 183** (F116-F480) | (257,124)-(262,127), **6 px** | 75 -> 110 |
| KEm/KEo Elza (2091) | wie K3 mit `RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1` (`Spieler-Familie PL04`) | `Kamera 1 (laufende)` | wie K3 | **183 / 183** | (257,124)-(262,127), 6 px | 75 -> 110 |
| integration_r41_gatorszene_tod / _tod_elza | Riegel (Todesrolle, Leon/Elza) | - | - | Passed 65.70 s / 98.35 s (ctest Lauf 1) | - | - |
| K2m/K2o Leon (Gegenprobe Rezept K wie Abnahme 0, Teleport auf (6500,-25000) OHNE Laufweg) | Sprung-Cut 0 bleibt stehen (Teleport kreuzt keinen RVD-Streifen) | `Kamera 1 (Zonenkamera mit dem groessten Bild)` | (-1600,-1717,-26900) | **0 / 161** in der gezeigten Kamera Cut 0 | - | - |

- Weg zu Fuss (W2, ohne Gator): von der Leiter nach Osten cam 0 bis x -475, ab dort cam 1 -> an der R1-Todesstelle des
  Spielers (6500,-25000) ist Cut 1 die natuerliche Kamera; K2 (Teleport) ist ein Messartefakt, kein Spielweg.
- Aufnahme an der Todeslage (P4, Todesrolle wie T1m, Eingabeskript ab F487: `U3.17` Ost, `L0.3556` (Gier -1056), `U1.0` Nord
  -> Stand (-1397,-24312) IN der Zone (-1300+-500, -24000+-500), Viereck F650/F692/F734): itemband.txt `F650 ... ITEM slot 15
  floor 0 item 0x34 tk 112 spieler-band 0 ... -> FREI`, modal.txt Zustaende 2 -> 3 -> 4 -> 6 (Zoom/Drehung/Frage), debug.log
  `[inv-r39] F734 R2090 ... inv=03:15,15:50,34:1` -> Water Key im Inventar. (P3 mit Druecken im 21-Bilder-Takt: nichts
  aufgenommen - die Drucke fielen in die Modal-Animation; Eingabe-Timing, kein Fehler.)
- Bilder: `F41a1/T1_tod.png` (F630/F600, 2-fach), `F41a1/T1_key_zoom.png` (6-fach: F600, F630, F630 OHNE Schluessel, F660),
  `F41a1/K3_tod.png`, `F41a1/K3_key_zoom.png`.
- **Befund zur Sichtbarkeit:** In beiden Todeswegen besteht der Schluessel aus 4 bzw. 6 Pixeln. 6-fach vergroessert sind es in
  T1 drei bis vier verstreute graue Punkte auf dem Beckenboden (dessen Textur selbst solche Punkte hat), in K3 ein kurzer
  schraeger Strich von 4-6 Pixeln an der Wandkante - als Schluessel nicht erkennbar; in 1-fach ein schwach pulsierender
  Punkt. Der Bau-Agent fuehrt das selbst als Grenze ("Mehr geht nur mit einem anderen bzw. groesseren Modell oder einem
  Kameraschnitt - Nutzerentscheidung"). Abnahme-0-Mangel M2 (Differenzbild > 0, Puls ueber 60 Bilder, beide Wege, Leon und
  Elza) ist damit dem Buchstaben nach erfuellt; der Nutzer-Wortlaut "soll ... blinken" (wie der Feuerloescher, Wortlaut Punkt 1)
  ist fuer den Spieler nur knapp wahrnehmbar.
- **Urteil 1b: teilweise** (Lage bei der Leiche ja, Blinken ja und messbar, Aufnahme ja; Wahrnehmbarkeit 4-6 Pixel offen,
  Nutzerentscheidung angefragt, nicht entschieden).

## 4. Punkt 2 — "Entferne die Spinnen aus dem ROOM 2090, die nutzen wir nicht mehr."

- RE-Beleg selbst gelesen (xxd): ROOM2090.RDT `@0x00AB0 44 00 25 00 00 00 00 73 9c ff 00 00 98 df` (Slot 0, Typ 0x25,
  (-100,-8296)) und `@0x00AC4 44 01 25 00 00 00 00 74 04 f7 00 00 a8 a2` (Slot 1, (-2300,-23896)); ROOM2091 bytegleich (`cmp`),
  shared_assets/PSX/STAGE2/ROOM2090.RDT == info/Re1.5 (`cmp`). Kill-Flag-Tor RE1.5 PSX.EXE: `80042120 lbu a1,7(a1)`,
  `80042124 ori v0,zero,0xff`, `80042128 beq a1,v0,0x8004215c`, `80042130 jal 0x8004efe4` = zitierte Form.
- exe (state.log ` t=25 ` = Typ 0x25): L1 Leon 0 von 1289 Zeilen, E1 Elza (2091) 0 von 1109, T1m 0/911, K3m 0/731, KEm 0/731,
  P4 0/1151; je 2090/2091-Besuch zwei Zeilen `[gatorszene] ROOM209x: Spinne 0x25 bei (-100,-8296)|(-2300,-23896) verworfen`.
- Fremd-Riegel `unit_r38_spinnen` (nach F-E1 nachgezogen: 2090 -> 0 Spinnen, uebrige Raeume unveraendert) gruen (Lauf 1);
  Code-Diff gelesen (`test_r38_spinnen.c`: nur die 2090-Zeile verlangt `n_sp == 0`, Kommentar mit Nutzer-Wortlaut).
- **Urteil Punkt 2: erfuellt** (Leon und Elza). Nebenwirkung `integration_r38_wasser` (Teil leon2090) s. 1.1/M1.

## 5. Nachtrag 21 (Leiter statt Tor, Tor mit dem Schluessel) und Abnahme-0-Mangel M3 (ein Verschlossen-Ton)

- G3 (20A0 ohne Bit 113: `RE15_DEBUG_JUMP=20A0@250 RE15_PLAYER_POS=4950,4100,3072,2 RE15_PAD_AT=60:A RE15_TUERSE_LOG=tuerse.log
  RE15_AUDIO_CAP_SYNC=audio.raw`): `[gatorszene] ROOM20A0 Slot 0 (Tor) gesperrt: Ereignis 20`, `[msgtext] room=20a0 id=1
  "It's locked."`, `Folge Z ROOM20A0 F60`; tuerse.log genau EINE Zeile `F60 raum=20A0 nachricht=1 art=M weg=AOT satz=0(ZU_A)
  nr=1`. Code: `tor_druck` spielt den Ton nur, wenn `lock_stelle(room, msg)` 0 ist; gen/lock_se_sites.inc:49/50 `{ 0x20A0, 1,
  RE15_LOCK_ART_M, 1 }` / `{ 0x20A1, 1, ... }` mit Weg 1 = RE15_LOCK_WEG_AOT (re15_lock_se.h:43) -> in 20A0/20A1 spielt nur die
  Tabelle. Riegel `unit_r41_gatorszene_tor` (Spion g_test_door_se_count: 2090 und 20A0 je `1 x Satz der Art M`) und
  `unit_r30_tuer_verschlossen` (54 Plaetze, 47 Stellen, je Platz 1 Ton) gruen. RE2 selbst disassembliert (re2_disasm.py,
  info/re2leon/PSX.EXE): `800516a0 lui a0,0x216` / `800516a4 jal 0x8005ba28` / `800516a8 addu a1,zero,zero` / `800516b8 jal
  0x8002fe38` = ein Se_on + Text im Nicht-Treffer-Zweig. **M3: behoben.**
- G1 (2090, Bit 110, ohne Schluessel, `RE15_PLAYER_POS=6630,-16350,0,1`, A F60): `[msgtext] room=2090 id=42 "It's locked."`,
  `Folge Z ROOM2090 F60` (Ton ueber den Tor-Handler, msg 42 ist keine Tabellen-Stelle -> tuerse.log leer, erwartet).
- G2 (dto. mit `RE15_GIVE=0x34:1`, A F60/140/200/260): `id=43 "You've opened the lock."`, `Folge U F60`, Wegwerf
  (`inv ... 34:0` F141, danach weg F260), `[aot] DOOR FIRE slot=1 ... spawn=(4950,-3600,4100)`, `[tuer] Sequenz ... DOOR26
  Variante 0`, `EXIT_AT: Bild 120 in Raum 20a0` -> Tor offen.
- Leiter-Zugang: L1/E1 (2.1). **Urteil Nachtrag 21: erfuellt.**

## 6. Abnahme-0-Mangel M4 (Fremd-Riegel, Entscheid F-E1) und neue rote Fremd-Riegel

- Nachgezogen und gruen (Lauf 1): `unit_r36_tueren_schott` (Passed; Diff gelesen: ohne (9,113) 0 begehbare Tueren nach 20A0,
  mit (9,110)+(9,113) DOOR26 V0), `unit_r38_spinnen` (Passed), `unit_r30_tuer_verschlossen` (Passed 28.67 s),
  `integration_r41_re2gator` (Passed 278.31 s; Diff gelesen: fuenf Laeufe `RE15_SET_FLAG=9:110,9:113` mit Kommentar Nachtrag 21,
  `RE15_R41_HP_AT=175:60` im Lauf nische, kanister EXIT_AT 1400 -> 1500, sechster Lauf "szene" ueber die Leiter mit Clip-Folge
  6/4/5/11/0/10 und DURCHBRUCH -> FINISHER -> Ende). Querbezuege sind belegt (Nachtrag-/Wortlaut-Kommentar).
- **Neu rot durch F, NICHT nachgezogen** (alle drei vorher gruen, alle drei laufen durch 2090 oder das Tor 20A0 -> 2090):
  | Riegel | Ausgabe | Ursache (gemessen) | vorher gruen |
  |---|---|---|---|
  | `integration_r38_wasser` (Runde 38 N) | `r38_wasser [leon2090]: kein wasser.log ... Wasser-Schritt fehlt` (Lauf 1 und einzeln) | Teil-Lauf leon2090 springt ohne Bit 110 nach 2090 -> `Folge A`/`P 1` F250, Riegel haelt Leon, kein Watschritt bis EXIT_AT 80 | B_re2gator.md:428 (180 s), A_abnahme_0.md 9.1 (206.65 s) |
  | `integration_r37_itemband` (Runde 37 D) | `r37_itemband: weg2090: Tuer 20A0 slot 0 @0x01746 nicht ausgeloest — fehlt: '[aot] DOOR FIRE slot=0 ...'` (923 s, Wartezeit bis Zeitgrenze) | Tor 20A0 -> 2090 seit Nachtrag 21 zu; Nachlauf IB1 (Wegplan des Riegels): `ROOM20A0 Slot 0 (Tor) gesperrt`, `Folge Z ROOM20A0 F84`, kein DOOR FIRE | B_abnahme_4.md 5 (`Passed 807,18 s`), H_abnahme_0.md 5.4 (Integrationsgruppe 81/81) |
  | `integration_r41_wasser20b0_npc` (Spur A) | `r41_wasser20b0_npc[landung_leon]: Tuer 20a0 -> 2090 nicht genommen (DOOR FIRE=-1, geladen=-1)` (466 s) | Landung "ueber den ECHTEN Weg: Tuer 20A0 -> 2090" (Leon und Elza) - Tor zu | A_abnahme_2.md (`Passed 229.03 s`, `landung_leon/landung_elza: Landungen 1, fx-Zeilen id=6 sub=3: 104`) |
  Alle drei sind dieselbe Lage wie die vier F-E1-Riegel ("neue Regel widerlegt alten Riegel"): Querbezug mit Beleg
  (Nachtrag 21 / Nutzer-Punkt "neue Cutscene") nachziehen, z. B. Bits (9,110)/(9,113) in den betroffenen Laeufen, oder den
  Weg auf die Leiter umstellen. -> Mangel M1.

## 7. RE-Gate (Stichproben selbst disassembliert/gelesen, richtige Binaerdatei)

| Zitat im Diff (Nachbesserung 1) | selbst gelesen | Ergebnis |
|---|---|---|
| Zonen-Zustandsmaschine `lbu v1,22(a0)` @0x80014254 (RE1.5 PSX.EXE FUN_80014230) | `8001423c lbu v0,-1099(v0)` (0x800afbb5), `80014254 lbu v1,22(a0)`, `8001425c bne v1,v0,0x800142d8`, Schleife `800142c0 lbu v1,0(s0)` / `800142c8 lbu v0,-1099(v0)` / `800142d0 beq v1,v0,0x8001426c`, Treffer `800142a8 lbu a0,1(s0)` + `800142ac jal 0x800142f4` | stimmt (nur die Zonen-Gruppe des angezeigten Cuts) |
| Raumlader @0x8001d940 (Cut aus der Tuer-Nutzlast) | `8001d930 lbu v1,10(a0)`, `8001d940 sb v1,-1099(at)` (0x800afbb5), `8001d948 sh v1,4068(at)` (0x800b0fe4) | stimmt |
| Cut_chg LAB_800402a0 @0x800402c0/d4/e4/ec/fc | `800402c0 lbu a1,4068(a1)`, `800402d4 ori v0,v0,0x100`, `800402e4 sb a1,16251(at)`, `800402ec sh a2,4072(at)`, `800402f4 sb v0,21591(at)` (v0 = 1), `800402fc sh a0,4068(at)`, `80040300 jal 0x800142f4` | stimmt (kam_uebergabe setzt dieselben sechs Groessen) |
| ROOM1150 sub08 `29 05` @0x012C4, Plc_ret @0x012E0, Cut_auto @0x012E2 | xxd ROOM1150.RDT: @0x12C4 `29 05`, @0x12E0 `42 00`, @0x12E2 `3c 01`, @0x12E4 `22 02 07 00`, @0x12E8 `22 01 1b 00` | stimmt |
| Leiter ROOM2080 Slot 1 @0x00BC2 -> Cut 0 | xxd: `3b 01 02 31 08 00 ... f8 df 00 00 32 9c 00 00 01 09 00` | stimmt |
| RE2 Wende `addiu a2,zero,75` @0x80102608 / FUN_8001569c | EM23_OVL_0000.BIN `80102604 jal 0x8001569c` / `80102608 addiu a2,zero,75`; FUN_8001569c s. 2.1 (RE2_Quellcode_V2/FUN_8001569c.c deckt sich) | stimmt |
| RE2 Clip 11 `addiu a1,zero,11` @0x801011d8 / `jal 0x8001a240` @0x801011e0; FUN_8001a240 @0x8001a258-60, @0x8001a28c; FUN_80015cb8 @0x801011cc | EM23: `801011cc jal 0x80015cb8`, `801011d8 addiu a1,zero,11`, `801011e0 jal 0x8001a240`; PSX.EXE `8001a258 sb a1,332(a0)` / `8001a25c sb zero,333(a0)` / `8001a260 sb a2,334(a0)`, `8001a28c jal 0x8002959c` | stimmt (Abnahme-0-Ungenauigkeit M6 berichtigt) |
| RE2 R1-Ende: Tot-Flag @0x80102654-64, Kamera frei @0x80102688-94 | EM23 `80102654 lhu v0,270(s1)` / `8010265c ori v0,v0,0x4000` / `80102664 sh v0,270(s1)`; `80102688 addiu v1,zero,-257` / `8010268c and` / `80102694 sw v0,-1064(at)` (0x800cfbd8 &= ~0x100) | stimmt |
| Blink-Folge TITLE.BIN FUN_801028ec | s. 2.3 | stimmt |
| Alligator-Kasten @0x80118b98 | re15_disasm.py `read ... --bin STAGE2.BIN` -> `[1000, -720, 0, 2200, 720, 800]` | stimmt |
| RE2 Tuer-Handler @0x800516a0/a4 | s. 5 | stimmt |
| Quadrat +-90 / Normale (0,-4096,0) (ROOM7040.RDT MD1 @0x354C) | info/re2leon/PL0/RDT/ROOM7040.RDT: Kopf `7c 00 00 00 00 00 00 00 02 00 00 00`, Objekt-Tabelle ab +0xC (Ecken-Offset 0x38, 4 Stueck; Normalen-Offset 0x58, 1 Stueck; Basis +0xC) -> Ecken (-90,0,90) (-90,0,-90) (90,0,90) (90,0,-90) bei +0x44..+0x63, Normale `00 00 00 f0` = (0,-4096,0) bei +0x64; gen/gatorszene_r41_schluessel.inc (140 B) gleich | Werte stimmen; der Kommentar nennt "+0x38" (Tabellenwert) und "+0x60" fuer die Normale (absolut +0x64) - Schreibweise, kein Verhaltenswert |

Konstanten der Nachbesserung: GSZ_KEY_AUF_RX/RZ 544/-464 (PORT-WAHL, leiche_lage.py-Rechnung im Kommentar), GSZ_WENDE_MAX
(PORT-WAHL, aus @0x80102608 + 0x800 hergeleitet), GSZ_RASTER 100 / Bildrand 24..296 x 40..205 / Kippung 60 Grad (PORT-WAHL-Block
"wie leiche_lage.py"), GSZ_GATOR_HALBBREITE 800 (@0x80118b98), GSZ_PRIM_KAPPE 0x80 (render_pc.c), r_nah-Ersatz 3055 (Buehnen-
Messzeile). Das Neigungsraster in key_neigung (rx/rz -1024..1024 Schritt 32) steht ohne eigene Kennzeichnung, ist aber Teil des
gekennzeichneten Suchverfahrens (kein Verhaltenswert). Suche im Nachbesserungs-Diff nach deferred/tunable/interim/for now/
faithful/plausib/TODO/vorerst/geschaetzt/geraten: 0 Treffer; kein getenv im F-Code. Erklaert der Fix den Befund? M1 ja
(Zonen-Gruppe), M3 ja (Tabellen-Stelle), M5 ja (Kappung), M6 ja, M2 nur teilweise (Lage/Neigung erklaeren "unsichtbar", die
Groesse des Modells bleibt).

## 8. Vertrag

- Bits nur (9,110..113) (VERTRAG 1.1), Raum-Scratch (5,28/29/30) nur 2090; Nachrichten 2090/2091 Ids 42/43 (zugeteilt 42-46);
  Ereignisse 18/19/20, AOT-Plaetze 15/20 - unveraendert seit Abnahme 0.
- Pfade `git diff 9f10bdb0..HEAD --name-only`: keine release/, platform/android/, shared_assets/PSX/, pri/, synchro/, keine
  Test-CMakeLists; neu nur shared_assets/RE2/SOUND/MAIN1A.BGM (Dossier nennt es). Die Nachbesserung (76ad856d..HEAD) aendert
  ausser F-eigenen Dateien nur die vier F-E1-Fremd-Riegel (test_r41_re2gator.cmake, probe_r36_tueren.c, test_r30_tuer.c,
  test_r38_spinnen.c) - jeweils mit Querbezug-Kommentar.
- Riegel je Punkt vorhanden und messend; gezielte Gruppen s. 1 (151 + 60 Tests). Nebenwirkungen: drei Fremd-Riegel rot (6).
- Hinweise (keine Maengel): exe-Kopien der eigenen Laeufe tragen fest ".exe" (wie Spur B, Abnahme 0 H); der exe-Riegel fuer M2
  misst nur die Todesrolle (GB_TEST=3), der Spiel-Todesweg R1 ist nur im Unit-Teil `tod` (mit Ersatz-Pose Clip 0, `(void)clip`)
  gepinnt.

## 9. Maengel (nummeriert, nachpruefbar)

- **M1 Drei Fremd-Riegel durch F rot, nicht nachgezogen** (Vertrag 2 "Nebenwirkungen", Muster Entscheid F-E1). Alle drei waren
  vor F gruen und scheitern deterministisch an F's Aenderungen (Ankunftsszene ohne Bit 110 bzw. Tor 20A0 -> 2090 bis Bit 113):
  1. `integration_r38_wasser` Teil `leon2090` (test_r38_wasser.cmake:147, `RE15_DEBUG_JUMP` nach 2090 ohne Bit 110): Programm 1
     haelt Leon, `kein wasser.log ... Wasser-Schritt fehlt` (ctest Lauf 1 und einzeln, `F41a1/ctest_wasser1.log`).
  2. `integration_r37_itemband` Teil `weg2090` (test_r37_itemband.cmake:196, Weg ueber das Tor 20A0 Slot 0 @0x01746):
     `Tuer 20A0 slot 0 @0x01746 nicht ausgeloest` (`F41a1/ctest2.log`; Nachlauf IB1: `ROOM20A0 Slot 0 (Tor) gesperrt`, `Folge Z`).
  3. `integration_r41_wasser20b0_npc` Teile `landung_leon`/`landung_elza` (test_r41_wasser20b0_npc.cmake:258-259, Landung ueber die
     Tuer 20A0 -> 2090): `Tuer 20a0 -> 2090 nicht genommen (DOOR FIRE=-1, geladen=-1)` (`F41a1/ctest2.log`).
  Soll: je Riegel ein belegter Querbezug wie bei den vier F-E1-Riegeln (Kommentar mit Nutzer-Wortlaut "neue Cutscene" /
  Nachtrag 21; z. B. `RE15_SET_FLAG=9:110` bzw. `9:110,9:113` in genau diesen Laeufen, oder Weg ueber die Leiter), danach gezielt
  gruen gefahren. Die Regex des Bau-Agenten (133 Tests) enthielt keinen der drei.
- **M2 Punkt 1b: Water Key nach dem Tod nur 4-6 Pixel, als Schluessel nicht erkennbar.** Gemessen T1 (Todesrolle, Cut 0):
  4 Pixel, Kasten (175,124)-(178,129), 75 -> 111; K3/KE (R1-Kanistertod mit natuerlicher Kamera Cut 1, Leon/Elza): 6 Pixel,
  Kasten (257,124)-(262,127), 75 -> 110. 6-fach vergroessert drei bis vier verstreute graue Punkte bzw. ein kurzer Strich, im
  Bodenrauschen der Beckentextur (`F41a1/T1_key_zoom.png`, `F41a1/K3_key_zoom.png`). Der Bau-Agent nennt das selbst eine Grenze
  und stellt dem Nutzer die Frage "groesseres/massiveres Modell oder Kameraschnitt nach dem Tod". Solange das nicht entschieden
  und umgesetzt ist, ist "soll der Schluessel ... sein und blinken" fuer den Spieler nur knapp wahrnehmbar -> Entscheid
  (Orchestrator/Nutzer) noetig; danach Messung wie hier (Differenzbild, Kasten, Puls, Leon + Elza, R1 mit natuerlicher Kamera).

Hinweise (keine Maengel): H1 nach dem Tod wieder MAIN2E (RE2 blendet aus, `51 00 05` @+0x762, Abnahme 0 H1) - offen beim Nutzer.
H2 Ersatzlage nach dem Laden eines Spielstands (Bau-Agent: 1 Pixelspalte) und "Alligator nach dem Laden lebend?" nicht
nachgemessen. H3 2080-Lagen von Spur G kopiert (bei der Integration gleich halten). H4 PSX-Bau ungeprueft. H5 MD1-Offsets im
Kommentar (s. 7). H6 Die M2-Riegel pinnen den Spiel-Todesweg R1 nur im Unit-Teil mit Ersatz-Pose.

## 10. Urteile

| Punkt | Urteil | Beleg |
|---|---|---|
| 1a Ankunftsszene (Leiche + blinkender Schluessel, Knall, 2080-Zombies oben, erneut Leiche, Alligator aus dem Wasser frisst Leiche samt Schluessel, Schrei/Schuetteln zu Leon, RE2-Kampfmusik; Leon + Elza) | erfuellt | 2.1 Folge L1/E1 vollstaendig, Knall Takt 573, MAIN1A; M1 behoben (2.2: cam 0 F368-F663, Spielfigur im Bild, kein Cut-1-Bild); M5 behoben (2.3: 73 -> 108 Dreieck, Leon = Elza) |
| 1b Schluessel nach dem Tod bei der Leiche, blinkend | teilweise | 3: Lage <= Kopf-Wurzel-Abstand, Blinken 75 -> 111 / 110 in 96/96 bzw. 183/183 Bildern, Aufnahme an der Todeslage (P4 `inv ... 34:1`); aber nur 4-6 Pixel (M2) |
| Nachtrag 21 Leiter statt Tor, Tor mit dem Schluessel; M3 ein Ton | erfuellt | 5: G1/G2/G3, tuerse.log 1 Zeile, Unit-Spion 1 Ton |
| 2 Spinnen aus 2090 entfernen | erfuellt | 4: 0 Spinnen-Zeilen in allen Laeufen, Leon + Elza |
| RE-Gate | erfuellt | 7: 13 Stichproben stimmen, Konstanten belegt/gekennzeichnet, keine Rate-Woerter |
| Vertrag/Riegel | nicht_erfuellt | 1, 6, M1: drei Fremd-Riegel rot |

**Ergebnis: NICHT bestanden** (M1, M2).
