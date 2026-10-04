# Runde 35 Spur H "raeume" — Abnahme 2 (unabhaengig, 2026-10-04)

Baum `.claude/worktrees/r35_raeume`, Zweig `r35/raeume`, HEAD **c85895c2** (Nachbesserung 2), Basis 154a73c1
(merge-base mit master; `git diff master` zeigt nur deshalb release/-Dateien, weil master seit 154a73c1 auf v0.8.22
weitergelaufen ist — massgeblich ist `git diff 154a73c1`, 48 Dateien). Gebaut: `local_build.sh configure` + `build` ->
"ninja: no work to do", `=== LOCAL-BUILD-OK (build)`. exe `re15_pc.exe` 06:28:13 ist juenger als jede geaenderte
Quelle (enemy_ai_re2_zombie.c / re15_ai_flavor.h 06:20:39, enemy_ai_re2_zellenarm.c 06:11:51, enemy_ai_common.c
06:09:57); nach dem Bau-Stand nur noch Kommentare in test_r35_raeume_arme.c (c85895c2). Arbeitsbaum sauber.

Gemessen an der Kopie `re15_port/build/platform/pc/re15_pc_abn2h.exe` (eigener Name, nur eigene Prozesse, Kopie danach
geloescht). Alle Laeufe: `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2`. Scratchpad
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/`
(im Folgenden `SCR/`): Laeufe `SCR/runs/a2_<lauf>/` (debug.log, out.txt, st.log = RE15_STATE_LOG, gy.log =
RE15_GEGNER_Y_LOG, hs.log = RE15_HUNDESCHATTEN_LOG, re2_ki.log = RE2-Trace, fd_*.ppm = RE15_FRAMEDUMP), Bilder
`SCR/a2_*.png`, Skripte `SCR/abn2_run.sh`, `SCR/abn2_batch_p4.sh`, `SCR/abn2_batch_p123.sh`, `SCR/abn2_sheet.py`.
Bildzaehler: `g_engine.frame_count` beginnt in jedem Raum bei 0.

## Ergebnis

| Punkt | Urteil |
|---|---|
| 1 ROOM1190 Hunde-Schatten im Luken-Sprung | **erfuellt** |
| 2 ROOM1190 Zielscheiben-Texte | **erfuellt** |
| 3 ROOM1200 Bahren-Zombie kommt auf die Spieler-Ebene | **erfuellt** |
| 4 ROOM1210 Gitterarme beim Griff (Gleichlauf / Clipping) | **teilweise** |

**bestanden = false** (Punkt 4 nicht erfuellt; Maengel M1-M4 unten).

Stand der Maengel aus Abnahme 1 (H_abnahme_1.md), selbst gemessen:

| Abnahme 1 | Stand nach Nachbesserung 2 |
|---|---|
| M1 RE1.5-KI greift ohne Kontakt | **behoben** — unter RE1.5-KI laeuft derselbe RE2-EM2D-Griff; Framedumps F244-330 (Gesicht) und F260-360 (Ruecken) **pixelgleich** mit dem RE2-KI-Lauf (44/44 bzw. 51/51 Bilder), kein HP-Verlust |
| M2 Ruecken-Griff clippt, Original nicht gezeigt | **besteht** — Ruecken-Griff an der exe-Pin-Lage 18/19 Bilder mit Arm-Vertices in Leons Kopf/Rumpf; RE2-Bild/-RAM weiter nicht gegangen (OFFEN 1); dazu neu: das Ueberschneidungsmass haengt an einer PORT-Hoehe (M2 unten) |
| M3 Maske ohne PORT-MAPPING-Kennzeichnung | **behoben** (Kommentar enemy_ai_common.c re15_los_ray_blocked + Aufrufstelle, Dossier; Adressen selbst geprueft) |

---

## Punkt 1 — Hunde-Schatten waehrend des Luken-Sprungs (erfuellt)

Code seit Abnahme 1 unveraendert (`git diff 34f2913a HEAD` beruehrt hundeschatten_1190.c / main.c nicht). Echter
Ausloeser des Hunde-Ereignisses (Raetsel geloest: Scheibe 0 vorn per Bank-5-Bit, Leon an Schalter Slot 2, Seite 1 ->
Seite 2 -> "Yes"):
```
abn2_run.sh p1_s2_re2  RE15_DEBUG_JUMP=1190@gp RE15_PLAYER_POS=-3180,-17300,2048 RE15_SET_FLAG=4:243
                       RE15_SET_FLAG_AT=5:4@1790 RE15_PRESS=square@1800..1803,@1960..1963,@2120..2123 (je Einzeleintrag)
                       RE15_HUNDESCHATTEN_LOG=hs.log RE15_GEGNER_Y_LOG=gy.log RE15_STATE_LOG=st.log
                       RE15_FRAMEDUMP=1790-2900/6:fd_ RE15_EXIT_AT=2900#1190
abn2_run.sh p1_s2_re15 (dasselbe + RE15_AI_FLAVOR=re15)
```
* debug.log: `[setflag-at] Frame 1790: flag(5,4/0x04) = 1`, `[ziel1190] ROOM1190 Slot 2 -> Port-Nachricht 6 ...`.
* st.log Kamera: F2340 cam 8 -> **F2352 cam 11** -> F2450 cam 2.
* hs.log RE2-KI: **168 Bilder mit `luft=1`, alle `boden_y=0 schatten_y=0`**; Luken-Spruenge F2366-2384, F2398-2416,
  F2430-2448 (F2366 `koerper_y=-3660`). RE1.5-KI: **254 Luft-Bilder, alle `boden_y=0 schatten_y=0`**.
* Bild `SCR/a2_p1_re2_luke.png` (F2366..F2444 Cut 11, F2456/F2468 Cut 2): in keinem Luken-Sprungbild ein
  Schatten-Quad in der Luft oder an der Wand; in Cut 2 liegt der Schatten eines angreifenden Hundes am Boden.
* Riegel direkt gestartet: `[RE2] Hund 1..3: 21 Luft-Bilder, hoechster Punkt y=-3680, gelandet=1`, ebenso RE1.5.

Erklaert der Fix den Befund? Ja (Abnahme 0/1): Quad-Hoehe vorher `npc->y`, jetzt +0x1ba (`lh a1,442(v0)`
@0x8010d91c), das die Sprungmaschine ab Bild 0xD auf 0 setzt (`sh zero,442(v0)` @0x801114f0).

---

## Punkt 2 — Zielscheiben-Texte (erfuellt)

Code seit Abnahme 1 unveraendert. Alle vier Schalter selbst angefahren (AOT-Rechtecke x -4400..-3800,
z -25700/-22000/-18500/-14900 + 2400, Dossier 2.1):
```
abn2_run.sh p2_s0|p2_s1|p2_s3  RE15_DEBUG_JUMP=1190@gp RE15_PLAYER_POS=-3180,{-24500|-20800|-13700},2048
                               RE15_SET_FLAG=4:243 RE15_PRESS=square@1800..1803,@1960..1963
                               RE15_FRAMEDUMP=1790-2100/10:fd_ RE15_EXIT_AT=2100#1190
Slot 2: Lauf p1_s2_re2 (oben)
```
* debug.log: `Slot 0 -> Port-Nachricht 6 (viele Einschuesse) + Original msg 0`, `Slot 1 -> Port-Nachricht 7 (wenige ...)`,
  `Slot 2 -> Port-Nachricht 6`, `Slot 3 -> Port-Nachricht 7`.
* Bilder `SCR/a2_p2_s0_s1_s3_text.png` (F1930/F2000/F2090) und `SCR/a2_p2_s2_text.png` (F1856/F1952/F2000/F2096):
  Slot 0 und 2 "This target has a surprisingly / large number of bullet holes.", Slot 1 und 3 "This target does not
  have / many bullet holes." mit Weiter-Pfeil, danach jeweils die Original-Seite "There's a switch here. Push it?
  Yes No". Nach "Yes" (Slot 2) laeuft das Hunde-Ereignis (Punkt 1) — Raetselmechanik intakt.
* Zuordnung "ganz links = Slot 0, 3. von links = Slot 2" hat Abnahme 0 an den gerenderten Scheiben (Cut 5/6/7/9)
  belegt. Schlusspunkt im zweiten Satz = PORT-WAHL, im Dossier gekennzeichnet.
* `unit_r35_raeume_ziel` gruen (3 Raumzustaende x 4 Slots + ROOM1191, Gegenprobe ohne Stempel).

---

## Punkt 3 — ROOM1200 Bahren-Zombie (erfuellt)

Code seit Abnahme 1 unveraendert.
```
P3 = RE15_DEBUG_JUMP=1200@gp RE15_PLAYER_POS=-25880,-16450,2048 RE15_GEGNER_Y_LOG=gy.log RE15_STATE_LOG=st.log
     RE15_PRESS=square@1800..1803,@1900..1903,@2000..2003 RE15_EXIT_AT=2700#1200
abn2_run.sh p3_re2_still  P3 RE15_FORCE_CUT=2 RE15_FRAMEDUMP=2060-2180/4:fd_
abn2_run.sh p3_re15_still2 P3 RE15_AI_FLAVOR=re15     (exit 124: Leon stirbt ohne Eingabe F2246, hp -1, danach 1170 —
                                                       EXIT_AT wird nie erreicht; Fallbilder davor vollstaendig)
abn2_run.sh p3_door  RE15_DEBUG_JUMP=11E0@gp RE15_PLAYER_POS=2000,-24250,3072 RE15_PAD_AT=45:A,46:A ... EXIT_AT=200#1200
```
| Lauf | Weckung | oben | Sturz (f1c0 0x8001..) | Landung y=0 b0 | danach |
|---|---|---|---|---|---|
| p3_re2_still | F1909 | Gang F1988, Schnappbiss Sub 14 F2062-2091 | **F2118** @(-24487,-1810,-16917) | **F2132** f1c0=000f | Sub 3 Griff, Sub 5 Abstoss |
| p3_re15_still2 | F1909 | Gang F2008 | **F2083** @(-24134,-1810,-16900) | **F2097** | Sub 4 Angriff |

Fallfolge beider Laeufe -1810, -1800, -1770, -1720, -1650, -1560, -1450, -1320, -1170, -1000, -810, -600, -370, -120,
0 (= FUN_8001bd60 mit -10/+20, a0 @0x801004dc, a1 @0x80100518) — Bild fuer Bild gleich Abnahme 1. Bild
`SCR/a2_p3_re2_F2108-2144.png` (Cut 2): F2108-2124 Zombie oben auf der Bahren-Ebene, F2128 im Fall, F2132 unten bei
Leon. Tuerweg 11E0 -> 1200: debug.log `loaded room11e0.rdt` -> `loaded room1200.rdt`, gy.log F1
`[2 t=10 st=0/0/0/0 g=87 ... @(-24249,-1800,-18579) b1 f1ba=-1800 f1c0=0000]` — der +0x1ba-Seed (Vorbedingung des
Sturzes) steht auch auf dem echten Tuerweg. Riegel: `[B RE2 Spieler unter der Kante] slot 2: Sturz ab Bild 218,
Landung Bild 232`, RE1.5 168/182.

---

## Punkt 4 — ROOM1210 Gitterarme (teilweise)

Echter Weg in allen Laeufen: Tuer ROOM1220 -> ROOM1210.
```
DOOR = RE15_DEBUG_JUMP=1220@gp RE15_PLAYER_POS=-21750,-6400,0 RE15_PAD_AT=45:A,46:A RE15_INPUT_SCRIPT_BASIS=spiel
       RE15_INPUT_SCRIPT_START=80 RE15_RE2_TRACE=1 RE15_STATE_LOG=st.log RE15_FORCE_CUT=4
p4_front      DOOR RE15_INPUT_SCRIPT=R0.5,W0.3,U14                          FRAMEDUMP 230-330/2
p4_back       DOOR RE15_INPUT_SCRIPT=R0.5,W0.3,U3.57,W0.3,R0.57,W0.3,D8      FRAMEDUMP 260-360/2
p4_re15_front / p4_re15_stand (U4.8) / p4_re15_back   dasselbe + RE15_AI_FLAVOR=re15
```
(re2_ki.log liegt neben der exe und wird je Lauf neu geschrieben — nach jedem Lauf in den Laufordner kopiert.)

**(a) Gleichlauf und Pin, RE2-KI.** re2_ki.log p4_front: `[re2arm] PIN slot 5 yaw 3664: Parts gemischt Clip 3 Bild 4 ->
(-20609,-15171); rein (ohne +0x14E) -> (-20588,-15148); Clip 5 Bild 0 -> (-20728,-15056); Leon vorher (-20622,-15020)
yaw 1440`; st.log F244-F415 `PL(-20609,-15171,rot=1671,hp=100)`. p4_back: `PIN slot 5 yaw 3705: Parts gemischt ...
(-20603,-15239); rein (-20552,-15206)`, st.log F277-F448 `PL(-20603,-15239,rot=3759)` (Flip). Der Ueberblend-Rest ist
also im echten Weg vorhanden (31 bzw. 61 Einheiten) — Vorbedingung des M2-Fixes erfuellt. Riegel (2): Leon zeigt jedes
Halte-Bild Arm-Bild + 1.

**(b) RE1.5-KI (Mangel M1 aus Abnahme 1).** p4_re15_front / p4_re15_stand: derselbe PIN (-20609,-15171), Halten
F244-F415, hp bleibt 100; debug.log `Opferbank von EM010 geliehen (victim_ok=1, 14 Clips)` (RE2-Lauf: 17 Clips) belegt,
dass der Lauf wirklich im RE1.5-Flavor lief. Framedumps p4_front gegen p4_re15_front F244-330: **44 von 44 Bildern
pixelgleich**; p4_back gegen p4_re15_back F260-360: **51 von 51 pixelgleich**. Vorher (Abnahme 1) stand Leon
1300-1500 neben der Hand. -> behoben.

**(c) Clipping — Nutzerfall Gesicht-Griff (Leon kommt von der Tuer).** Bilder `SCR/a2_p4_front_F240-284.png` und
`SCR/a2_p4_front_kopf_F262-292.png` (5-fach, Kopfbereich): die Hand greift an Kopf/Hals; in F274/F276 und F290/F292
liegt der Unterarm ueber bzw. in Leons Kopf — aus der seitlichen Kamera 4 nicht sicher von "dahinter" zu trennen.
Das Mass des Bau-Agenten selbst gefahren (`probe_r35_raeume_arme.exe`, Abschnitt EXE-PIN):
```
EXE-PIN front gemischt (byte-true)  Pin (-20609,-15171): waagerecht <120 in  0/19 (min 175), Volumen 12/19 Bilder (max 8 Vertices)
EXE-PIN front rein C3B4 (NB1)       Pin (-20588,-15148): waagerecht <120 in  0/19 (min 169), Volumen  7/19 Bilder (max 6 Vertices)
EXE-PIN front Clip5B0 (Basis)       Pin (-20728,-15056): waagerecht <120 in  9/19 (min 33),  Volumen 16/19 Bilder (max 10 Vertices)
```
Riegel direkt (`test_r35_raeume_arme.exe`): Gesicht `44 Halte-Bilder, Hand < 120 an der Brustachse in 4, Minimum 83`,
`Pin-Quellen Gesicht: Volumen/waagerecht gemischt 30/4 | rein C3B4 25/0 | Clip5B0 37/19 (von 44)`.
Nach dem Mass, das der Bau-Agent selbst fuer massgeblich erklaert (beim Ruecken-Griff verwirft er das waagerechte
Mass als "Artefakt"), steckt der Arm im Nutzerfall in **12 von 19** Halte-Bildern in Leons Kopf/Rumpf-Volumen — mehr
als in Nachbesserung 1 (7/19) und nur wenig weniger als im Basisstand des Nutzerbefunds (16/19). Der Riegel (3)
verlangte in Nachbesserung 1 noch "0 von 44 Bildern" (waagerecht) und verlangt jetzt nur noch "weniger als der
Basisstand" (30 < 37 Volumen, 4 < 19 waagerecht).

**(d) Clipping — Ruecken-Griff.** Bild `SCR/a2_p4_back_kopf_F296-326.png`: Unterarm/Hand ueber Leons Kopf und Schulter
(F296, F306-310, F324-326). Sonde: `EXE-PIN back gemischt Pin (-20603,-15239): waagerecht <120 in 9/19 (min 24),
Volumen 18/19 Bilder (max 12 Vertices)`. Ein RE2-Bild/-RAM-Wert des Ruecken-Griffs liegt weiter nicht vor (Dossier
OFFEN 1).

**(e) Haengt "das Original clippt genauso" wirklich nur an RE2-Bytes? — Nein: die Armhoehe ist eine PORT-BRUECKE.**
Der Bau-Agent schreibt "Jede Eingangsgroesse der Ruecken-Griff-Geometrie ist ein RE2-Byte". Die senkrechte Lage der
Hand zu Leon — genau die Groesse, die das Volumenmass entscheidet (der Pin setzt nur x/z) — kommt aber aus
`RE2ARM_1210_Y = -2500` (enemy_ai_re2_zellenarm.c:210, Kommentar :189-202 "Port-Bruecke, am Original-Hintergrund
gemessen"). RE2 selbst stellt seine zehn Arme in ROOM2050 auf **zehn verschiedene** Hoehen (selbst gelesen,
`info/re2leon/PL0/RDT/ROOM2050.RDT`, Sce_em_set-Saetze @0x1970 + n*0x16, y = s16 an Satz+12):
```
rec 0 @0x1970 y=-2580 (0x197C)  rec 1 -2430 (0x1992)  rec 2 -2180 (0x19A8)  rec 3 -1930 (0x19BE)  rec 4 -2200 (0x19D4)
rec 5 @0x19DE y=-2480 (0x19EA)  rec 6 -2000 (0x1A00)  rec 7 -2700 (0x1A16)  rec 8 -2540 (0x1A2C)  rec 9 -2160 (0x1A42)
```
Mit dem vorhandenen Mess-Haken `RE15_ARM_ANKER=400,<y>` (arm_anker_1210, kein Spielpfad) denselben Riegel gefahren:

| Armhoehe y | Volumen Gesicht (von 44) | Volumen Ruecken (von 44) | Riegel (3) | Riegel (3c) |
|---|---|---|---|---|
| -1930 (RE2 rec 3) | 44 (max 18) | 44 (max 18) | **FAIL** | **FAIL** |
| -2200 (RE2 rec 4) | 42 (max 8) | 31 (max 10) | **FAIL** | **FAIL** |
| **-2500 (Port)** | 30 (max 8) | 35 (max 18) | ok | ok |
| -2700 (RE2 rec 7) | 20 (max 14) | 42 (max 30) | ok | **FAIL** |

(Das waagerechte Mass bleibt erwartungsgemaess 4 / 18.) Die Zahl der Ueberschneidungsbilder in beiden Griff-Lagen und
das Urteil des Riegels haengen also an einem Portwert innerhalb des RE2-Bandes, nicht an einem RE2-Byte. Damit folgt
"das Original zeigt im Ruecken-Griff dieselbe Ueberschneidung" nicht aus den Daten; welcher RE2-Arm (welche Hoehe) der
Massstab fuer ROOM1210 Arm 5 ist, ist nicht belegt.

Gegenprobe Leon-Modell (nicht im Dossier, selbst gemessen): RE2 `PL0/PLD/PL00/PL00.md1` gegen RE1.5 `PLD/PL00.MD1`,
Mesh 0 (Rumpf) x -267..226 / -255..254, y -834..-112 / -822..10; Mesh 8 (Kopf) x -104..291 / -138..251 — aehnlich, aber
nicht byte-gleich (das Volumenmass rechnet mit dem RE1.5-Modell). EMR-Versaetze Bones 1-6, 8, 9, 12 byte-gleich wie
behauptet (Bone 0 -1810 / -1804, Bone 7 811 / 810, Unterarme 10/11/13/14 verschieden).

**Urteil gegen den Wortlaut** ("bewegen sich nicht synchron zu Leon beim schuetteln, dadurch clipped er"): Gleichlauf
(Leon = Arm + 1) und Pin sind RE2-genau belegt, die RE1.5-KI greift jetzt mit Kontakt. Das Clipping ist aber nicht
weg: im Nutzerfall nach dem eigenen Volumenmass 12 von 19 Halte-Bildern, im Ruecken-Griff 18 von 19; die Begruendung
"so baut es RE2" ist am Original nicht gezeigt und haengt an der Port-Armhoehe -2500. -> **teilweise**, Maengel M1-M4.

---

## Gates

### Suite
* Dossier, Abschnitt "Suite (Nachbesserung 2)", traegt woertlich `=== LOCAL-BUILD-OK (all) — Tests 482/482` (Schranke
  478) am Code-Endstand 95e93e2a; danach nur Kopfkommentar in test_r35_raeume_arme.c.
* Selbst gefahren: `ctest -R "r35_raeume|1210|writher"` -> **11/11 gruen** (unit_writher_ai/_hit_feedback/_kill_flag/
  _abtauchen, unit_1210_gitterhaende, unit_1210_arme, unit_1210_arme_re2, unit_r35_raeume_hundeschatten/_ziel/_trage/
  _arme); Testprogramme direkt gestartet, echte Ausgaben s. o. Volllauf: Nachtrag Suite.

### @0x-Gate (Stichproben selbst disassembliert)
1. **RE2 PSX.EXE 0x80029614 (M2-Ueberblendung):** `800296a8 lbu t3,334(s2)` / `800296b0 beq t3,zero` /
   `800296b8 andi v0,a3,0xffff` / `800296bc mult v0,t3` / `800296e8 mflo t1`; `800299c0 lbu v0,334(s2)` /
   `800299c8 addiu v0,v0,-1` / `800299cc sb v0,334(s2)` (EIN Dekrement vor der Part-Schleife, Decompile
   RE2_Quellcode_V2/FUN_80029614.c bestaetigt); `800299f0 mtc2 t1,IR0` (0x48894000) auf die aktuelle Rotation +0x68,
   `80029a10-aa4` Ziel +0x7C mit +-4096-Kuerzestweg, `80029aac subu v0,v0,t1` / `80029ab0 mtc2 v0,IR0`. Die Formel
   +0x68 := (t1*aktuell + (4096-t1)*Ziel) >> 12 mit t1 aus dem Wert VOR dem Dekrement stimmt.
2. **EM2D-Overlay** (aus CDEMD0.EMS geschnitten, md5 82ab57601b18a0233338d56c287017f1, wie Abnahme 1): `80100acc lui
   v0,0xf` / `80100aec ori v0,v0,0x3` / `80100af0 sw v0,332(s0)` (0x000F0003), `80100b24 jal 0x8002959c` /
   `80100b28 addiu a3,zero,256`, `80100a10 lbu v0,333(s0)` / `80100a18 sltiu v0,v0,0x5`, Pin `80100c18 lw v0,92(v1)` /
   `80100c20 sw v0,-976(at)` / `80100c24 lw v0,100(v1)` / `80100c38 sw v0,-968(at)`, Bank `80100c3c..c5c`. Wie zitiert.
3. **RE2 PSX.EXE 0x8001abe4-ac00 (Opferbank):** `lw v1,20(s0)` (dir[5]) -> `8001abf0 sw v1,396(s1)` (+0x18C),
   `lw v1,24(s0)` (dir[6]) -> `8001ac00 sw v1,392(s1)` (+0x188). Wie zitiert.
4. **M3:** RE1.5 STAGE1 `80100624 lbu a2,471(a0)` -> `8010062c jal 0x8003b0a4`; PSX.EXE `8003b128 sb a2,24(sp)`,
   `8003b244 lhu v0,-2(s2)` / `8003b248 lbu v1,24(sp)` / `8003b250 sra v0,v0,24` / `8003b254 and v1,v1,v0` /
   `8003b258 bne v1,zero`; RE2 `8004a868 addiu a2,zero,8192`. Wie zitiert; Kennzeichnung PORT-MAPPING vorhanden.
5. **M1 Writher-Zensus** (STAGE1 0x8010c1ec-0x8010d774, 1379 Instruktionen, `SCR/abn2_writher.dis`): jal 15x 0x8001af20,
   11x 0x8001f314, 5x 0x800245d8, 3x 0x80019700, je 1x 0x8001af5c/0x8001bd60/0x8002aec4/0x8002b498/0x8002b544/0x8003b0a4/
   0x80065f60; kein 0x80012d60. Kein Store mit negativem Versatz; Lasten mit negativem Versatz nur -14460 (g_entity) und
   -13688/-13680 (playerX/Z @0x8010c238/258/360/378). Alle 9 jalr ueber Tabellen @0x8012093c/95c/968/984/c40,
   0x801209a0/0x80120c94 — jedes Ziel liegt in 0x8010c33c..0x8010d770. Griff fehlt im RE1.5-Writher: bestaetigt.
* Wortsuche im Code-Diff (deferred/tunable/interim/for now/faithful/plausib/TODO/approx/vorerst/geschaetzt): nur das
  Zitat des alten Kommentars ("look helper, deferred") in re15_trage1200.h. Neue getenv nur Messschienen
  (RE15_GEGNER_Y_LOG, RE15_HUNDESCHATTEN_LOG, RE15_RE2_TRACE, R35_ARME_DUMP/R35_TRAGE_DBG in Tests). Der neue
  Test-Haken `re15_ai_writher_re15_pin()` ist kein Env-Schalter (nur Tests rufen ihn).
* RE-Gate-Verletzung als Aussage (kein Code-Wert): die Behauptung "Jede Eingangsgroesse ... ist ein RE2-Byte" im
  Dossier (Nachbesserung 2, Opferbank-Analyse "=>") und im Code-/Commit-Text ist fuer die Armhoehe falsch (Punkt 4 e).
  `RE2ARM_1210_Y` selbst ist im Code als Port-Bruecke mit Messweg gekennzeichnet — der Fehler liegt in der
  Schlussfolgerung, nicht in der Konstante.

### Pfad-/Vertrags-Gate
* `git diff 154a73c1 --name-only`: keine release/, platform/android/, shared_assets/PSX/, tests/unit/CMakeLists.txt,
  tests/integration/CMakeLists.txt. Gemeinsame Dateien nur mit kleinen Haken: enemy_ai_common.c (Include, 1 Aufruf,
  LOS-Parameter + Filter, 2 Aufrufstellen, Kommentare), scd_vm.c (2 Includes + 3 Haken), main.c (2 Includes + 2 Haken),
  re15_actor.h (1 Feld), enemy_ai_re2_zombie.c (1 Zeile + Test-Haken + Kommentar), re15_ai_flavor.h (1 Deklaration).
* Typ-0x1A-Zensus (Flavor-Zeile wirkt global): Sce_em_set-Saetze mit Typ 0x1A nur in ROOM1210/1211 (je 10, Satzabstand
  0x14); die uebrigen Bytefolgen `44 ?? 1A` in anderen RDTs sind Nachrichtentext/Daten (geprueft an 1011/1141/1170/
  11B1/5011/50B0/50D0/30E1).
* Nachrichten-Ids 6/7 in ROOM1190 (zugeteilt 6..11); keine Bank-9-Bits; Ereignis 27 nicht belegt. Ok.

### Tests
* Punkt 1-3: messende Riegel, gruen, Ausgaben plausibel zum exe-Lauf (Sturzfolge, Luft-Bilder, Slots).
* Punkt 4: `unit_r35_raeume_arme` misst Pin, Gleichlauf, RE1.5-KI (5a/5b) echt. Aber: (3b) prueft den Port gegen seine
  eigene Spiegelung (kann nur kippen, wenn der Code die Konstruktion aendert — kein Bezug zum Original); (3)/(3c)
  vergleichen nur Port-Varianten untereinander und halten nur bei der Port-Armhoehe -2500 (Tabelle Punkt 4 e); kein
  Riegel prueft das Kriterium des Nutzers (kein Arm in Leon) oder einen Original-Wert.

---

## Maengel

**M1 (Punkt 4, Nutzerfall Gesicht-Griff): Clipping nach dem eigenen Volumenmass nicht beseitigt.** An der exe-Pin-Lage
(-20609,-15171) liegen Unterarm-/Hand-Vertices des Arms in **12 von 19** Halte-Bildern in Leons Kopf/Rumpf-Volumen
(max 8; `probe_r35_raeume_arme.exe` Abschnitt EXE-PIN), im Riegel 30 von 44 — mehr als Nachbesserung 1 (7/19 bzw.
25/44), kaum weniger als der Basisstand des Nutzerbefunds (16/19 bzw. 37/44). Bild `SCR/a2_p4_front_kopf_F262-292.png`
(F274/F276/F290/F292 Arm ueber/in Leons Kopf). Der Riegel (3) wurde von "0 von 44" (Nachbesserung 1, waagerecht) auf
"weniger als Basisstand" gelockert. Noetig: entweder zeigen, dass das Original im selben Fall dieselbe Ueberschneidung
hat (RE2-Bild/-RAM: Arm +0x3C, Hand part+0x5C/+0x60/+0x64, PL+0x38/+0x3C/+0x40/+0x76 im Halten), oder den Unterschied
finden — und den Riegel auf das Nutzer-Kriterium bzw. den Original-Wert stellen.

**M2 (Punkt 4, RE-Beleg): "Jede Eingangsgroesse ist ein RE2-Byte" ist fuer die Armhoehe falsch; das Clipping-Mass und
der Riegel haengen an der Port-Bruecke `RE2ARM_1210_Y = -2500`** (enemy_ai_re2_zellenarm.c:210). RE2 ROOM2050 stellt die
zehn Arme auf -2580/-2430/-2180/-1930/-2200/-2480/-2000/-2700/-2540/-2160 (ROOM2050.RDT y an 0x197C/0x1992/0x19A8/
0x19BE/0x19D4/0x19EA/0x1A00/0x1A16/0x1A2C/0x1A42). Mit `RE15_ARM_ANKER=400,y` gemessen: Volumen Gesicht/Ruecken 44/44
(y -1930), 42/31 (-2200), 30/35 (-2500), 20/42 (-2700); Riegel (3)/(3c) fallen bei -1930, -2200 und (3c) bei -2700.
Noetig: die Armhoehe (bzw. die senkrechte Lage Hand <-> Leon) fuer ROOM1210 Arm 5 aus RE2 belegen (welcher RE2-Satz,
welche Hoehe, Leon-Boden) oder am RE2-Emulator messen; bis dahin darf das Dossier nicht "Original zeigt dieselbe
Ueberschneidung" folgern.

**M3 (Punkt 4, Ruecken-Griff, fortbestehend aus Abnahme 1 M2):** Ruecken-Griff an der exe-Pin-Lage (-20603,-15239):
Volumen 18/19 (max 12), waagerecht 9/19 (min 24); Bild `SCR/a2_p4_back_kopf_F296-326.png`. Das in Abnahme 1 als
Bedingung genannte Original-Bild/-RAM (Dossier OFFEN 1, pcsx-redux + re2leon.cue, ROOM2050) ist nicht gegangen; die
statische Herleitung leidet unter M2.

**M4 (Tests, Punkt 4): Der Riegel ist teils selbstbestaetigend.** (3b) vergleicht den Ruecken-Lauf mit dem gespiegelten
Gesicht-Lauf desselben Port-Codes (Tautologie der Konstruktion); (3)/(3c) vergleichen Port-Pin-Quellen untereinander
und halten nur bei y = -2500 (M2). Noetig: ein Riegel, der gegen das Nutzer-Kriterium (keine Arm-Vertices in Leon,
oder eine am Original belegte Obergrenze) bzw. gegen einen Original-Messwert prueft.

Hinweise (keine Maengel): Dossier OFFEN 2 (Port-Zeichner mischt einen Schritt voraus, skeleton_common.c, gemeinsame
Datei) betrifft nur die Uebergangsbilder und ist mit Adressen (@0x800296a8, @0x80029B30) und Messweg offen gefuehrt;
OFFEN 3 (tote RE1.5-Writher-Maschine, Referenz-Pins ueber Test-Haken) ist ein Integrations-Entscheid.

---

## Nachtrag Suite
Selbst gefahren am HEAD-Stand c85895c2 (nach allen Messlaeufen, exe-Kopie vorher entfernt):
`bash re15_port/tools/local_build.sh test` -> `100% tests passed, 0 tests failed out of 482` (1120 s; alle fuenf
Fenster-Haken im ersten Lauf gruen) -> **`=== LOCAL-BUILD-OK (test) — Tests 482/482`**. Suite-Gate haelt. Die
Abweichungslaeufe mit `RE15_ARM_ANKER` (Punkt 4 e) sind Einzelaufrufe des Testprogramms mit Env, kein Suite-Lauf.
