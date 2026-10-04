# Runde 35 Spur H "raeume" — Abnahme 4 (unabhaengig, 2026-10-04)

Baum `.claude/worktrees/r35_raeume`, Zweig `r35/raeume`, HEAD **36022879** (Nachbesserung 4), merge-base mit master
**154a73c1**. Massgeblich ist `git diff master...HEAD` (= `git diff 154a73c1 HEAD`): `git diff master` zeigt release/-,
cut_10f0-, granate-, werfer-Dateien usw. nur, weil master seit 154a73c1 auf v0.8.22 weitergelaufen ist.
NB4-Code-Diff (`git diff f7562cec HEAD -- re15_port`): enemy_ai_re2_zellenarm.c (+137), game_step_common.c (+2),
skeleton_common.c (+8/-4), re15_enemy_ai_re2_zellenarm.h (+6), Tests/Sonde/Fixture. Selbst gelesen.

Gebaut: `local_build.sh configure` + `build` -> "ninja: no work to do", `=== LOCAL-BUILD-OK (build)`. exe
`re15_pc.exe` 11:53:25 ist juenger als jede NB4-Quelle (zellenarm.c 11:41, skeleton/game_step 11:10/11:11); die Commits
danach (73f475a6, 57453822, 36022879) aendern nur das Dossier. Arbeitsbaum sauber.

Gemessen an der Kopie `re15_port/build/platform/pc/re15_pc_abn4h.exe` (eigener Name, nur eigene Prozesse; ein fremder
`re15_pc.exe` lief parallel und blieb unberuehrt; Kopie nach den Laeufen geloescht; `befund.log` vorher gesichert und
danach byte-gleich zurueckgelegt). Alle Laeufe `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp
RE15_TITLE_SHOT_AF=2`. Scratchpad `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/
c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/` (= `SCR/`): Laeufe `SCR/runs/a4_<lauf>/` (debug.log, st.log =
RE15_STATE_LOG, neck.log = RE15_NECK_TRACE, re2_ki.log = RE2-Trace, gy.log = RE15_GEGNER_Y_LOG, hs.log =
RE15_HUNDESCHATTEN_LOG, fd_*.ppm = RE15_FRAMEDUMP), Skripte `SCR/abn4_run.sh`, `SCR/abn4_batch_p4.sh`,
`SCR/abn4_batch_p123.sh`, `SCR/abn4_neck.py`, Bilder `SCR/a4_*.png`. Bildzaehler beginnt in jedem Raum bei 0; die
Auswertung nimmt jeweils den letzten Raumabschnitt.

## Ergebnis

| Punkt | Urteil |
|---|---|
| 1 ROOM1190 Hunde-Schatten im Luken-Sprung | **erfuellt** |
| 2 ROOM1190 Zielscheiben-Texte | **erfuellt** |
| 3 ROOM1200 Bahren-Zombie kommt auf die Spieler-Ebene | **erfuellt** |
| 4 ROOM1210 Gitterarme beim Griff (Gleichlauf / Clipping) | **erfuellt** |

Gates: Suite (selbst 482/482), @0x-Gate, Pfad-/Vertrags-Gate, Tests — halten. **bestanden = true**.

Stand der Maengel aus Abnahme 3 (selbst gemessen):

| Abnahme 3 | Stand nach Nachbesserung 4 |
|---|---|
| M1 Leons Kopf im Halten nicht original / nicht gemessen | **behoben.** RE2-Mechanismus selbst disassembliert (FUN_8003DB38 + FUN_800177C0-SELBST-Zweig, s. Gate). exe Ruecken-Griff: Ziel SELBST in 151/151 Halte-Bildern, Akku (0,0) ab F282 (146/151; vorher bis -598 an der Klemme); Gesicht-Griff: Ziel = Halter, Akku -209..415 (Original -207..417). Volumenmass mit dem gezeichneten Kopf selbst gefahren: Gesicht 5/19 (max 4), Ruecken 15/19 (max 6). |
| M2 Riegel klammert Kopfdrehung aus | **behoben.** Leon wird im Riegel wie vom Zeichner posiert (`g_anim_pose_actor = pl`), (6a) prueft Kopf-Rahmen (max 17), Blick-Akku (max 14) und Blickziel Bild fuer Bild gegen die Original-RAM; (1) ohne g3/g4 (1251..1255); neu (N1a)/(N1b). Selbst gelaufen, alle Pruefungen ok. |
| M3 g3/g4 und falsche Abstandsaussagen | **behoben.** Dossier :822/:830/:905/:941 mit `[NB4 berichtigt]`, Code-Kommentar zellenarm.c mit den Werten je Griff und den Store-Adressen 0x800350c8/0x800350cc / 0x80100c20/0x80100c38. |

---

## Punkt 1 — Hunde-Schatten waehrend des Luken-Sprungs (erfuellt)

NB4 beruehrt hundeschatten_1190.c / main.c nicht; neu ist der Blick-Haken in game_step (laeuft in jedem Raum) — deshalb
alle drei Raum-Punkte am neuen Stand nachgemessen. Echter Ausloeser (Raetsel: Scheibe 0 vorn per Bank-5-Bit, Leon an
Schalter Slot 2, "Yes"):
```
abn4_run.sh p1_s2_re2  RE15_DEBUG_JUMP=1190@gp RE15_PLAYER_POS=-3180,-17300,2048 RE15_SET_FLAG=4:243
                       RE15_SET_FLAG_AT=5:4@1790 RE15_PRESS=square@1800..1803,@1960..1963,@2120..2123
                       RE15_HUNDESCHATTEN_LOG=hs.log RE15_GEGNER_Y_LOG=gy.log RE15_STATE_LOG=st.log
                       RE15_FRAMEDUMP=1790-2900/6:fd_ RE15_EXIT_AT=2900#1190
abn4_run.sh p1_s2_re15 (dasselbe + RE15_AI_FLAVOR=re15)
```
* debug.log: `[setflag-at] Frame 1790: flag(5,4/0x04) = 1`, `[ziel1190] ROOM1190 Slot 2 -> Port-Nachricht 6 ...`.
* st.log Kamera: F2272 cam 8 -> **F2352 cam 11** -> F2450 cam 2.
* hs.log RE2-KI: **168 Bilder `luft=1` (F2366-2765), alle `boden_y=0 schatten_y=0`**, z.B. `F2366 t=20 st=4 ss1=0 ss2=1
  koerper_y=-3660 boden_y=0 schatten_y=0 luft=1`, Koerper bis y -3680. RE1.5-KI: **254 Luft-Bilder, alle am Boden**.
  Beides identisch mit Abnahme 3.
* Bild `SCR/a4_p1_re2_luke.png` (F2360..F2420 Cut 11, F2456 Cut 2): in keinem Sprungbild ein Schatten-Quad in der Luft
  oder an der Wand; in Cut 2 liegt der Schatten am Boden.
* Erklaert der Fix den Befund (Vorbedingung im Protokoll): Quad-Hoehe frueher `npc->y`, jetzt +0x1ba (Abnahme 0-3
  selbst disassembliert STAGE1 `8010d91c lh a1,442(v0)` -> `8010d920 jal 0x8001b064`, `801114f0 sh zero,442(v0)`).
* `unit_r35_raeume_hundeschatten` gruen.

## Punkt 2 — Zielscheiben-Texte (erfuellt)

```
abn4_run.sh p2_s0|p2_s1|p2_s3  RE15_DEBUG_JUMP=1190@gp RE15_PLAYER_POS=-3180,{-24500|-20800|-13700},2048
                               RE15_SET_FLAG=4:243 RE15_PRESS=square@1800..1803,@1960..1963
                               RE15_FRAMEDUMP=1790-2100/10:fd_ RE15_EXIT_AT=2100#1190
Slot 2: Lauf p1_s2_re2
```
* debug.log: `Slot 0 -> Port-Nachricht 6 (viele Einschuesse) + Original msg 0`, `Slot 1 -> Port-Nachricht 7 (wenige
  Einschuesse)`, `Slot 2 -> Port-Nachricht 6`, `Slot 3 -> Port-Nachricht 7`.
* Bild `SCR/a4_p2_slots.png` (Zeilen Slot 0/1/3, F1850..F2080): Slot 0 "This target has a surprisingly / large number of
  bullet holes." mit Weiter-Pfeil, Slot 1 und 3 "This target does not have / many bullet holes.", danach die
  Original-Seite "There's a switch here. Push it? Yes No". Zuordnung ganz links = Slot 0, 3. von links = Slot 2 in
  Abnahme 0 an den gerenderten Scheiben belegt; Schlusspunkt im zweiten Satz = PORT-WAHL (gekennzeichnet).
* `unit_r35_raeume_ziel` gruen.

## Punkt 3 — ROOM1200 Bahren-Zombie (erfuellt)

```
P3 = RE15_DEBUG_JUMP=1200@gp RE15_PLAYER_POS=-25880,-16450,2048 RE15_GEGNER_Y_LOG=gy.log RE15_STATE_LOG=st.log
     RE15_PRESS=square@1800..1803,@1900..1903,@2000..2003 RE15_EXIT_AT=2700#1200 RE15_FORCE_CUT=2 RE15_FRAMEDUMP=2060-2180/4
abn4_run.sh p3_re2_still  P3
abn4_run.sh p3_re15_still P3 RE15_AI_FLAVOR=re15  (exit 124: Leon stirbt ohne Eingabe — wie Abnahme 2/3)
abn4_run.sh p3_door  RE15_DEBUG_JUMP=11E0@gp RE15_PLAYER_POS=2000,-24250,3072 RE15_PAD_AT=45:A,46:A ... EXIT_AT=200#1200
```
| Lauf | Sturz (f1c0 0x8001..) | Fallfolge y | Landung |
|---|---|---|---|
| p3_re2_still | **F2118** @(-24487,-1810,-16917) | -1810,-1800,-1770,-1720,-1650,-1560,-1450,-1320,-1170,-1000,-810,-600,-370,-120,0 | **F2132** y=0 b0 f1c0=000f |
| p3_re15_still | **F2083** @(-24134,-1810,-16900) | dieselbe Folge | **F2097** y=0 b0 f1c0=000f |

Bild `SCR/a4_p3_re2_F2104-2140.png` (Cut 2): F2104-2124 Zombie oben auf der Bahren-Ebene, F2128 im Fall, F2132 unten
neben Leon. Tuerweg 11E0 -> 1200 (`loaded room11e0.rdt` -> `loaded room1200.rdt`): gy.log F1 `[2 t=10 ... @(-24249,-1800,
-18579) b1 f1ba=-1800 f1c0=0000]` — der +0x1ba-Seed steht auch auf dem echten Weg. Alles identisch mit Abnahme 3.
`unit_r35_raeume_trage` gruen.

---

## Punkt 4 — ROOM1210 Gitterarme (erfuellt)

Echter Weg in allen Laeufen: Tuer ROOM1220 -> ROOM1210 (debug.log `[debug-menu] AUTO-JUMP -> ROOM1220`, `loaded
room1220.rdt` -> `loaded room1210.rdt`).
```
DOOR = RE15_DEBUG_JUMP=1220@gp RE15_PLAYER_POS=-21750,-6400,0 RE15_PAD_AT=45:A,46:A RE15_INPUT_SCRIPT_BASIS=spiel
       RE15_INPUT_SCRIPT_START=80 RE15_RE2_TRACE=1 RE15_STATE_LOG=st.log RE15_NECK_TRACE=neck.log
p4_front        DOOR RE15_INPUT_SCRIPT=R0.5,W0.3,U14                       RE15_FORCE_CUT=4 FRAMEDUMP 230-420/2
p4_back         DOOR RE15_INPUT_SCRIPT=R0.5,W0.3,U3.57,W0.3,R0.57,W0.3,D8   RE15_FORCE_CUT=4 FRAMEDUMP 260-440/2
p4_re15_front / p4_re15_back   dasselbe + RE15_AI_FLAVOR=re15
p4_front_nocut  wie p4_front ohne FORCE_CUT
```

**(a) Pin + Push unveraendert (Vorbedingung).** p4_front re2_ki.log `PIN slot 5 yaw 3664: Parts gemischt Clip 3 Bild 4 ->
(-20609,-15171) ... Leon vorher (-20613,-14853) yaw 1440`; st.log F243 `PL(-20618,-14843,rot=1440)` -> **F244
`PL(-20439,-15060,rot=1671)`** (Pin + Push), Halten mo=0 **F244-F394** (151 Bilder), Loslassen mo=1 F395-F414.
p4_back `PIN slot 5 yaw 3697 ... -> (-20610,-15228) ... Leon vorher (-20433,-15070) yaw 3072`, **F277 `PL(-20411,
-15111,rot=3749)`**, Halten F277-F427, Loslassen F428-F447. Gleich Abnahme 3 (dort an der Original-RAM belegt).

**(b) Leons Kopf im Halten — Nacken-Spur Slot 0 (`SCR/abn4_neck.py`, Halte-Bilder mo=0):**
```
p4_front  F244-F394 (151): (fl,ts) = (00,5) in 151/151 -> Ziel = Halter Slot 5
          acc_yaw -209..415 (Mittel |yaw| 198), acc_pitch -187..199
p4_back   F277-F427 (151): (fl,ts) = (00,0) in 151/151 -> Ziel = SELBST (Spieler-Slot)
          F277 acc=(-433,0) F278 -337 F279 -241 F280 -145 F281 -49 F282 0 (Schritt 96), danach (0,0) bis F427:
          acc==(0,0) in 146/151; z.B. `F277 slot=0 fl=00 tgt=(18,52) res=(0,0) acc=(-433,0) kf=(18,52) step=(96,96) ts=0`
Abnahme-3-Stand (gleiche Eingaben, SCR/runs/a3_p4_neck_back): acc_yaw -598..248, Mittel 284, pitch -186..193, acc==0 in 0/151
```
Original-RAM selbst ausgewertet (alle 15 frames.bin unter `re2_mess/daten/`, eigenes Skript, PL+0x1B8/+0x1C0, Part 8
+0x98/+0x9A): +0x1C0 = 0 in allen Bildern aller Laeufe; **Ruecken g2/g6/g8 (+ n4_g8b/n4_g8c): Ziel SELBST 40/40, Akku
(0,0) 40/40**; Gesicht g5/g7/g9/g11/g12/g13: SELBST bis Bild 20/25/21/28/26/15, danach Halter; g1/g3/g4 wechseln auf einen
Nachbar-Arm (Bild 30/31/25). Die exe macht im Ruecken-Griff genau das Original (Akku 0 = Animationspose); die fuenf
Rueckkehr-Bilder F277-F281 sind die 96er-Rampe aus dem vor dem Griff gewaehlten Blick (im Original-Mitschnitt war Leon
per GDB gestellt, Akku schon 0). Im Gesicht-Griff blickt die exe ab dem Pin auf den Halter, weil die frei laufende Suche
ihn schon vor dem Griff gewaehlt hatte — dieselbe Regel; das Original zeigt nach dem Wechsel dieselbe Spanne
(eigene Auswertung, auf -2048..2047 gefaltet: g5 -201..417, g7 -183..413, g9 -207..413, g11 0..417; exe -209..415).

**(c) RE1.5-KI.** debug.log `EM01A-Griff: Opferbank von EM010 geliehen (victim_ok=1, 14 Clips)` (RE2-Lauf: 17 Clips) =
wirklich RE1.5-Flavor. Nacken-Spur ROOM1210 Slot 0 **identisch** (front 456/456, back 476/476 Zeilen gleich);
Framedumps p4_front vs p4_re15_front **96/96 pixelgleich**, p4_back vs p4_re15_back **91/91 pixelgleich**.
p4_front_nocut: gleicher PIN, gleiche Nacken-Spur (ts=5, acc -209..415).

**(d) Volumenmass mit dem gezeichneten Kopf, selbst gefahren** (`probe_r35_raeume_arme.exe` mit MEINEN Nacken-Spuren):
```
R35_NECK_LOG_FRONT=SCR/runs/a4_p4_front/neck.log R35_NECK_PIN_FRONT=244
R35_NECK_LOG_BACK=SCR/runs/a4_p4_back/neck.log  R35_NECK_PIN_BACK=277  ./probe_r35_raeume_arme.exe
EXE-KOPF front ... Kopf WIE GEZEICHNET (exe-Akku) -> Volumen  5/19 Bilder (max 4 Vertices), |Akku-yaw| max 413
EXE-KOPF back  ... Kopf WIE GEZEICHNET (exe-Akku) -> Volumen 15/19 Bilder (max 6 Vertices), |Akku-yaw| max 0
dasselbe mit den Abnahme-3-Spuren (a3_p4_neck / a3_p4_neck_back):
EXE-KOPF front ... 5/19 (max 4) | EXE-KOPF back ... 17/19 (max 12), |Akku-yaw| max 598
```
Original an den Nachbarhoehen (Riegel (6b), Fixture): Gesicht hoechstens 11/19, Ruecken hoechstens 19/19; (6a) Original
g5 11/19, g6 19/19 (max 14), g7 7/19, g8 17/19 (max 22), g11 4/19. Der Port ueberschneidet an der exe-Lage nicht mehr
als das RE2-Original.

**(e) Bilder (Anschauung, kein Beleg).** `SCR/a4_p4_front_kopfzoom_F262-306.png` (Gesicht: Hand an Kopf/Schulter, nicht
durch das Gesicht) und `SCR/a4_p4_back_kopf_alt_neu_F300-330.png` (Ruecken, obere Reihe Abnahme-3-Lauf: Kopf zur Klemme
gedreht; untere Reihe NB4: Kopf in der Rumpf-Animation, Hand am Hinterkopf wie im Original-Ruecken-Griff).

**(f) Riegel selbst gelaufen** (`test_r35_raeume_arme.exe`, alle Pruefungen ok): (N1b) Port-Wahl = RAM-Ziel in n4_g8c
(SELBST trotz Satz 4 im Kegel) und n4_g1b (Halter statt des naeheren Satz 8); (N1a) neun saubere Laeufe = RAM-Ziel;
(6a) g5 Wurzel 9 / KOPF 17 / Akku 14 / Ziel gleich, g6 9/9/0, g7 10/10/10, g8 10/10/0, g11 9/10/10, Ueberschneidung Port
10/19, 19/19, 6/19, 17/19, 3/19 gegen Original 11, 19, 7, 17, 4; (1) alle Faelle 1251 (Original 1251..1255); (5b)
RE1.5-KI = RE2-KI. Die Fixture `r35_raeume_re2orig.inc` habe ich mit `re2_fixture.py` (Ausgabe in den Scratchpad
umgeleitet) aus den frames.bin/extra.txt neu erzeugt -> **byte-gleich** (84315 Bytes).

**Urteil gegen den Wortlaut** ("bewegen sich nicht synchron zu Leon beim schuetteln, dadurch clipped er"): Takt (Leon =
Arm-Bild + 1, Original-RAM), Pin auf der gemischten Hand, Koerper-Push (exe: Leon 1255,6 / 1252,5 vom Arm-Ursprung
(-21490,-15747); Original 1251..1255) und jetzt
auch Leons Kopf (Ziel und Akku Bild fuer Bild wie die RE2-RAM, an der exe gemessen) sind RE2-genau. Die verbleibende
Ueberschneidung an der exe-Lage (Gesicht 5/19 max 4 Vertices, Ruecken 15/19 max 6) ist kleiner/gleich der des
RE2-Originals in denselben bzw. benachbarten Lagen — sie ist die Konstruktion des Originals (Beta -> Retail: RE2 ist das
Ziel, RE1.5 hat diesen Griff nicht). -> **erfuellt**.

---

## Gates

### Suite
* Dossier, Abschnitt "Suite (Nachbesserung 4)" (H_raeume.md:1270-1272), traegt woertlich
  `=== LOCAL-BUILD-OK (all) — Tests 482/482` (Schranke 478) am Code-Endstand (Lauf 3).
* Selbst: `ctest -R "r35_raeume|1210|writher|neck|headlook|blitz|irons|grab|victim|arme"` -> **26/26 gruen** (190 s;
  darunter integration_r30_cut_blitz, der einzige Leser der Nacken-Spur, die NB4 um ` ts=` erweitert hat).
* Volllauf selbst: **482/482** (s. Nachtrag Suite am Ende).

### @0x-Gate (Stichproben selbst disassembliert, richtige Binaerdatei)
1. **RE2 PSX.EXE Aufruf** (re2_disasm.py, info/re2leon/PSX.EXE): `8003c1a4 jalr v0` (Zustands-Dispatch, Tabelle
   `lw v0,16432(at)` = 0x800a4030), `8003c1b0 addiu a1,zero,7000`, `8003c1b4 jal 0x8003db38`, `8003c1b8 addiu
   a2,zero,1500`, `8003c1bc lh a1,118(s0)`, `8003c1c0 jal 0x800177c0`. Pause-Tor `8003bfc0 bltz v0,0x8003c1fc` auf
   `lw v0,-1060(v0)` (0x800cfbdc). Wie zitiert.
2. **RE2 FUN_8003DB38 komplett** (0x8003db38-0x8003dd44): `8003db78 lbu v0,448(s3)` / `andi 0x1` / `bne -> 0x8003dd14`
   (VOR dem Zaehler); `8003db90 lbu v1,16388(v1)` (0x800a4004) / `8003db98 addiu v0,v1,255` / `8003dba0 sb` /
   `8003dba4 bne v1,zero -> Ende` / `8003dba8 addiu v0,zero,45` / `8003dbb8 sb v0,16388(at)`; Tor `8003dbb0 lbu
   0x800cfbf3` bzw. `0x800cfbd8 & 0x10000000` -> sonst `0x8003dd10`; Liste ab `0x800cfe18` bis `*(0x800ce334)`;
   `8003dbf4 andi v0,v0,0x1`; `8003dc08 andi v0,v0,0xc000`; `8003dc10 addiu a2,zero,8320` (0x2080) / `8003dc14 addiu
   a3,zero,1`, Part-Stride 172 (= 0xAC aus der shift/sub-Folge) + 92 fuer beide Seiten, `8003dc70 jal 0x80050858`;
   `8003dc8c jal 0x80015614` mit a1/a2 = E+0x38/+0x40, a3 = 1500; `8003dc24 lw s1,496(s0)` (+0x1F0); `8003dca4 andi
   0x2000`; Klasse A `8003dcb0 sltu v0,s1,s4` mit Nachfuehrung `8003dcbc addu s4,s1,zero` (naechster), Klasse B
   `8003dcac sltu v0,s1,s5` (s5 = 0x7fffffff); Speicher `8003dcfc sw s7,440(s3)` (A zuerst), `8003dd0c sw s6,440(s3)`,
   `8003dd10 sw s3,440(s3)` (SELBST). re15_re2arm_look_waehle bildet das 1:1 ab (A vor B, streng <, Nachfuehrung).
   +0x1F0: `800265a4..d8` dx²+dz² -> `jal 0x8008d2f4`, `800265e0 sw v0,496(s0)`. Kegel FUN_80015614: `(a3 + atan -
   +0x76) & 0xfff`, 0 nur fuer `< 2*a3` — gleich re15_ai_arc_test (re15_damage.c:3811).
3. **RE2 FUN_800177C0 SELBST-Zweig**: `80017a28 bne s0,s4,0x80017a64` / `80017a38 andi v0,v0,0x80` / `80017a44 lhu
   v0,106(s1)` + s3 + s2 -> `80017a54 sh v0,18(sp)` / `80017a58 lhu v0,108(s1)` -> `80017a60 sh v0,20(sp)`; danach die
   Klemme (`80017a6c lhu a0,160(s1)`) und der Slew `80017ad8..80017b40` (Delta = Ziel - (kf + yaw + Wurzel) - Akku ->
   bei SELBST laeuft der Akku auf 0), `80017bb8 sh v0,106(s1)` (kf += Akku). Reihenfolge 0x10 -> SELBST -> Klemme wie
   im Port (skeleton_common.c:513/517/536). Dass die Klemme auch das SELBST-Ziel faengt, sieht man in der exe beim
   Loslassen (F433-F444 kf-pitch 325..381 > 312 -> acc -13..-69) — gleich gebaut wie RE2.
4. **RE2 INIT**: `8003c250 sw s1,440(s1)` (+0x1B8 = SELBST), `8003c264/68 addiu v0,zero,8 / sb v0,449(s1)`, `8003c270 sb
   zero,448(s1)`; Raum-INIT `80049ffc sw zero,4(s2)`. Wie zitiert.
5. **EM2D-Overlay** (CDEMD0_EM2D_ai1.BIN, md5 82ab57601b18a0233338d56c287017f1): Scan aller sb/sh/sw mit Offset
   440/448/449 -> nur `801001a8 sb zero,448(s0)`, `801001d0 sb a0,449(s0)`, `80100260 sb zero,448(s0)`, `80100288 sb
   a0,449(s0)` (eigener Eintrag) — das Arm-Overlay fasst Leons Blickfelder nicht an. Wie zitiert.
* Konstanten im NB4-Diff: RE2LOOK_ZAEHLER 45 (@0x8003dba8/@0x8003dbb8), RE2LOOK_RADIUS 7000 (@0x8003c1b0), RE2LOOK_KEGEL
  1500 (@0x8003c1b8), RE2LOOK_AUS 0xC000 (@0x8003dc08), RE2LOOK_KLASSE_B 0x2000 (@0x8003dca4) — alle mit @0x im Code
  und in den Commits f8f856d2/36022879. Sicht = PORT-MAPPING re15_re2_los_clear, Arm-Schlaf-Abbildung = PORT-BRUECKE,
  beide benannt. Test-Grenzen R35_6A_KTOL/NTOL/ZTOL als TEST-TOLERANZ gekennzeichnet.
* Wortsuche im Spur-Diff (deferred/tunable/interim/for now/faithful/plausib/TODO/approx/vorerst/geschaetzt): nur das
  Zitat "look helper, deferred" (re15_trage1200.h, beschreibt den alten Port-Kommentar). Neue getenv nur Messschienen /
  Test-Haken (RE15_GEGNER_Y_LOG, RE15_HUNDESCHATTEN_LOG, RE15_RE2_TRACE, R35_ARME_DUMP, R35_TRAGE_DBG, R35_NECK_*);
  `re15_re2arm_look_debug` ist ein Test-Aufruf ohne getenv. Kein Env-Schalter als Abschluss.
* Erklaert der Fix den Befund: ja. Vorbedingung im Protokoll: Abnahme-3-Lauf `ts` = Arm hinter Leon, Klemme -512/-598;
  NB4-Lauf mit derselben Eingabe `ts=0`, Akku -> 0. Seiteneffekte geprueft: re15_re2_los_clear / re15_los_ray_blocked
  schreiben nichts; neck_target_slot == eigener Slot entsteht nur durch den neuen Verbrauch (alle Schreiber:
  actor_common.c:56/69 = -1, enemy_ai_common.c:10561 / game_step_common.c:943 = Spieler-Slot fuer Gegner/NPC,
  game_step_common.c:848-854 >= 1) -> der SELBST-Zweig greift sonst nirgends; die Nacken-Haken-Tests sind gruen.

### Pfad-/Vertrags-Gate
* `git diff 154a73c1 HEAD --name-only`: keine release/, platform/android/, shared_assets/PSX/,
  tests/unit/CMakeLists.txt, tests/integration/CMakeLists.txt. NB4 an gemeinsamen Dateien: game_step_common.c 2 Zeilen
  (Aufruf), skeleton_common.c 4 Zeilen + Spur-Feld — kleine Haken. Spur gesamt an gemeinsamen Dateien: enemy_ai_common.c
  (Haken + Maskenparameter), scd_vm.c 8, main.c 6, game_step_common.c 4, re15_actor.h 4 (Feld fall_1c0).
* Nachrichten-Ids 6/7 in ROOM1190 (zugeteilt 6..11); keine Bank-9-Bits; kein Ereignis belegt. Ok.

### Tests
* Punkt 1-3: messende Riegel (`unit_r35_raeume_hundeschatten`, `_ziel`, `_trage`), gruen, deckungsgleich mit den
  exe-Laeufen.
* Punkt 4: `unit_r35_raeume_arme` prueft gegen die Original-RAM ((O1) (4d) (N1a) (N1b) (1) (6a) (6b)) mit Leon wie vom
  Zeichner posiert; `unit_1210_arme_re2` gruen.

---

## Maengel

Keine.

Hinweise (keine Maengel, fuer spaeter):
1. Nach dem Loslassen (Spieler-Zustand verlaesst die Opfer-Routine) uebernimmt wieder die RE1.5-Blickwahl ohne Kegel;
   im Ruecken-Fall blickt Leon beim Weggehen zum Arm hinter sich (p4_back F449 ff. `ts=5 tgt=(-512,7)`, Akku -> -494).
   RE2 wuerde auch dort FUN_8003DB38 mit Kegel fahren. Ausserhalb des Griffs und nicht Teil des Nutzerpunkts.
2. Dossier-OFFEN 3 (benannt): sind in ROOM1210 vorher Arme der Gegenwand gezeigt worden (4600 entfernt, < 7000), blickt
   Leon im Ruecken-Griff zu ihnen. An der exe nicht nachgefahren; der Riegel-Nutzerfall zeigt dafuer Ziel Slot 8 und
   19/19 (max 14 Vertices) — im Rahmen der Original-Ruecken-Griffe (g6 19/19 max 14, g8 17/19 max 22).
3. Dossier H_raeume.md:912 (Abschnitt "Je Mangel (Abnahme 2)") nennt noch "Wachpunkt pc 0x800350cc/d0" — historischer
   GDB-PC; Code-Kommentar und NB4-Abschnitt tragen die Store-Adressen.

---

## Nachtrag Suite
Selbst gefahren am HEAD-Stand 36022879 (nach allen Messlaeufen, exe-Kopie vorher entfernt; kein anderer Bau im selben
build/): `bash re15_port/tools/local_build.sh test` -> `100% tests passed, 0 tests failed out of 482` (1124,72 s; alle
fuenf Fenster-Haken im ersten Lauf gruen) -> **`=== LOCAL-BUILD-OK (test) — Tests 482/482`** (Schranke 478). Log
`SCR/abn4_suite.log`. `befund.log` im exe-Ordner danach wieder auf den gesicherten Stand zurueckgelegt. Suite-Gate haelt.
