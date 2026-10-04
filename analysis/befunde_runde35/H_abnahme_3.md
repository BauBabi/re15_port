# Runde 35 Spur H "raeume" — Abnahme 3 (unabhaengig, 2026-10-04)

Baum `.claude/worktrees/r35_raeume`, Zweig `r35/raeume`, HEAD **9bcc9edd** (Nachbesserung 3), merge-base mit master
**154a73c1**. `git diff master` zeigt release/-, cut_10f0-, granate-, werfer-Dateien usw. nur, weil master seit 154a73c1
auf v0.8.22 weitergelaufen ist; massgeblich fuer die Spur ist `git diff master...HEAD` (= `git diff 154a73c1 HEAD`,
32 Code-/Testdateien + analysis/). Code-Endstand der Nachbesserung 3 = 24029bb4 (danach nur Dossier).

Gebaut: `local_build.sh configure` + `build` -> "ninja: no work to do", `=== LOCAL-BUILD-OK (build)`. exe
`re15_pc.exe` 09:29:00 ist juenger als jede geaenderte Quelle (enemy_ai_re2_zellenarm.c 09:28:43, enemy_ai_common.c /
game_step_common.c 09:08). Arbeitsbaum sauber.

Gemessen an der Kopie `re15_port/build/platform/pc/re15_pc_abn3h.exe` (eigener Name, nur eigene Prozesse, nach den
Laeufen geloescht; das vom Bau-Agenten liegengelassene `re2_ki.log` wurde gesichert und zurueckgelegt). Alle Laeufe:
`RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2`. Scratchpad
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/`
(im Folgenden `SCR/`): Laeufe `SCR/runs/a3_<lauf>/` (debug.log, st.log = RE15_STATE_LOG, gy.log = RE15_GEGNER_Y_LOG,
hs.log = RE15_HUNDESCHATTEN_LOG, re2_ki.log = RE2-Trace, neck.log = RE15_NECK_TRACE, fd_*.ppm = RE15_FRAMEDUMP),
Skripte `SCR/abn3_run.sh`, `SCR/abn3_batch_p4.sh`, `SCR/abn3_batch_p123.sh`, `SCR/abn3_st.py`, `SCR/abn3_gy.py`,
Bilder `SCR/a3_*.png`. Bildzaehler `g_engine.frame_count` beginnt in jedem Raum bei 0.

## Ergebnis

| Punkt | Urteil |
|---|---|
| 1 ROOM1190 Hunde-Schatten im Luken-Sprung | **erfuellt** |
| 2 ROOM1190 Zielscheiben-Texte | **erfuellt** |
| 3 ROOM1200 Bahren-Zombie kommt auf die Spieler-Ebene | **erfuellt** |
| 4 ROOM1210 Gitterarme beim Griff (Gleichlauf / Clipping) | **teilweise** |

**bestanden = false** (Punkt 4 teilweise; Maengel M1-M3 unten).

Stand der Maengel aus Abnahme 2 (selbst gemessen):

| Abnahme 2 | Stand nach Nachbesserung 3 |
|---|---|
| M1 Gesicht-Griff 12/19 | **Ursache gefunden und behoben** (RE2-Koerper-Push FUN_80034D0C, Adressen selbst geprueft, s. Gate). exe: Leon nach dem Pin 1252-1256 vom Arm-Ursprung auf dem Strahl durch die Hand (Original 1251-1255). Das Volumenmass 5/19 gilt aber nur fuer eine Leon-Pose OHNE Kopfdrehung — die exe dreht den Kopf (neuer M1). |
| M2 Armhoehe = Port-Bruecke, Folgerung unbelegt | **behoben im Sinn der Forderung**: Original in 8 Hoehen gemessen (Fixture aus frames.bin selbst neu erzeugt = byte-gleich), Port in 5 Original-Lagen = Original (Wurzel-Rahmen <= 10). Die Hoehe -2500 bleibt gekennzeichnete PORT-BRUECKE. |
| M3 Ruecken-Griff, Original-Bild/-RAM fehlte | **behoben** (RE2-RAM + VRAM-Bild g2/g4/g6/g8). Aber: im Ruecken-Griff weicht der Kopf der exe vom Original ab (neuer M1). |
| M4 Riegel selbstbestaetigend | **behoben** ((3b)/(3)/(3c) entfernt, Pruefungen gegen die Original-RAM). Neue Schwaeche: der Riegel klammert genau die Kopfdrehung aus (M2). |

---

## Punkt 1 — Hunde-Schatten waehrend des Luken-Sprungs (erfuellt)

Code seit Abnahme 2 unveraendert (`git diff a640207b HEAD` beruehrt hundeschatten_1190.c / main.c nicht). Echter
Ausloeser (Raetsel: Scheibe 0 vorn per Bank-5-Bit, Leon an Schalter Slot 2, "Yes"):
```
abn3_run.sh p1_s2_re2  RE15_DEBUG_JUMP=1190@gp RE15_PLAYER_POS=-3180,-17300,2048 RE15_SET_FLAG=4:243
                       RE15_SET_FLAG_AT=5:4@1790 RE15_PRESS=square@1800..1803,@1960..1963,@2120..2123
                       RE15_HUNDESCHATTEN_LOG=hs.log RE15_GEGNER_Y_LOG=gy.log RE15_STATE_LOG=st.log
                       RE15_FRAMEDUMP=1790-2900/6:fd_ RE15_EXIT_AT=2900#1190
abn3_run.sh p1_s2_re15 (dasselbe + RE15_AI_FLAVOR=re15)
```
* debug.log: `[setflag-at] Frame 1790: flag(5,4/0x04) = 1`, `[ziel1190] ROOM1190 Slot 2 -> Port-Nachricht 6 ...`.
* st.log Kamera: F2272 cam 8 -> **F2352 cam 11** -> F2450 cam 2.
* hs.log RE2-KI: **168 Bilder `luft=1` (F2366-2765), alle `boden_y=0 schatten_y=0`**, z.B. `F2366 ... koerper_y=-3660
  boden_y=0 schatten_y=0 luft=1`. RE1.5-KI: **254 Luft-Bilder, alle am Boden**.
* Bild `SCR/a3_p1_re2_luke.png` (F2366..F2444 Cut 11, F2456 Cut 2): in keinem Luken-Sprungbild ein Schatten-Quad in der
  Luft oder an der Wand; in Cut 2 liegt der Schatten am Boden unter dem Hund.
* Erklaert der Fix den Befund: Quad-Hoehe vorher `npc->y`, jetzt +0x1ba; selbst disassembliert STAGE1
  `8010d91c lh a1,442(v0)` -> `8010d920 jal 0x8001b064`, Sprungmaschine `801114f0 sh zero,442(v0)`.
* `unit_r35_raeume_hundeschatten` gruen.

## Punkt 2 — Zielscheiben-Texte (erfuellt)

Code unveraendert. Alle vier Schalter angefahren:
```
abn3_run.sh p2_s0|p2_s1|p2_s3  RE15_DEBUG_JUMP=1190@gp RE15_PLAYER_POS=-3180,{-24500|-20800|-13700},2048
                               RE15_SET_FLAG=4:243 RE15_PRESS=square@1800..1803,@1960..1963
                               RE15_FRAMEDUMP=1790-2100/10:fd_ RE15_EXIT_AT=2100#1190
Slot 2: Lauf p1_s2_re2
```
* debug.log: `Slot 0 -> Port-Nachricht 6 (viele Einschuesse) + Original msg 0`, `Slot 1 -> Port-Nachricht 7 (wenige ...)`,
  `Slot 2 -> Port-Nachricht 6`, `Slot 3 -> Port-Nachricht 7`.
* Bild `SCR/a3_p2_alle_slots.png` (Zeilen Slot 0/1/2/3): Slot 0 und 2 "This target has a surprisingly / large number of
  bullet holes.", Slot 1 und 3 "This target does not have / many bullet holes." mit Weiter-Pfeil, danach die
  Original-Seite "There's a switch here. Push it? Yes No". Zuordnung ganz links = Slot 0, 3. von links = Slot 2 in
  Abnahme 0 an den gerenderten Scheiben belegt; Schlusspunkt im zweiten Satz = PORT-WAHL (gekennzeichnet).
* `unit_r35_raeume_ziel` gruen.

## Punkt 3 — ROOM1200 Bahren-Zombie (erfuellt)

Code unveraendert.
```
P3 = RE15_DEBUG_JUMP=1200@gp RE15_PLAYER_POS=-25880,-16450,2048 RE15_GEGNER_Y_LOG=gy.log RE15_STATE_LOG=st.log
     RE15_PRESS=square@1800..1803,@1900..1903,@2000..2003 RE15_EXIT_AT=2700#1200 RE15_FORCE_CUT=2 RE15_FRAMEDUMP=2060-2180/4
abn3_run.sh p3_re2_still  P3
abn3_run.sh p3_re15_still P3 RE15_AI_FLAVOR=re15  (exit 124: Leon stirbt ohne Eingabe, hp<0 ab F2246 — wie Abnahme 2)
abn3_run.sh p3_door  RE15_DEBUG_JUMP=11E0@gp RE15_PLAYER_POS=2000,-24250,3072 RE15_PAD_AT=45:A,46:A ... EXIT_AT=200#1200
```
| Lauf | Sturz (f1c0 0x8001..) | Fallfolge y | Landung |
|---|---|---|---|
| p3_re2_still | **F2118** @(-24487,-1810,-16917) | -1810,-1800,-1770,-1720,-1650,-1560,-1450,-1320,-1170,-1000,-810,-600,-370,-120,0 | **F2132** y=0 b0 f1c0=000f |
| p3_re15_still | **F2083** @(-24134,-1810,-16900) | dieselbe Folge | **F2097** y=0 b0 f1c0=000f |

Bild `SCR/a3_p3_re2_F2104-2140.png` (Cut 2): F2104-2124 Zombie oben auf der Bahren-Ebene, F2128 im Fall, F2132 unten
neben Leon. Tuerweg 11E0 -> 1200 (debug.log `loaded room11e0.rdt` -> `loaded room1200.rdt`): gy.log F1 `[2 ... @(-24249,
-1800,-18579) b1 f1ba=-1800 f1c0=0000]` — der +0x1ba-Seed steht auch auf dem echten Weg. Selbst disassembliert:
STAGE1 `801004dc addiu a0,zero,-10`, `80100514 jal 0x8001bd60`, `80100518 ori a1,zero,0x14`. `unit_r35_raeume_trage` gruen.

---

## Punkt 4 — ROOM1210 Gitterarme (teilweise)

Echter Weg in allen Laeufen: Tuer ROOM1220 -> ROOM1210.
```
DOOR = RE15_DEBUG_JUMP=1220@gp RE15_PLAYER_POS=-21750,-6400,0 RE15_PAD_AT=45:A,46:A RE15_INPUT_SCRIPT_BASIS=spiel
       RE15_INPUT_SCRIPT_START=80 RE15_RE2_TRACE=1 RE15_STATE_LOG=st.log
p4_front        DOOR RE15_INPUT_SCRIPT=R0.5,W0.3,U14                       RE15_FORCE_CUT=4 FRAMEDUMP 230-420/2
p4_back         DOOR RE15_INPUT_SCRIPT=R0.5,W0.3,U3.57,W0.3,R0.57,W0.3,D8   RE15_FORCE_CUT=4 FRAMEDUMP 260-440/2
p4_re15_front / p4_re15_back   dasselbe + RE15_AI_FLAVOR=re15
p4_front_nocut  wie p4_front ohne FORCE_CUT
p4_neck         wie p4_front ohne TRACE/FRAMEDUMP + RE15_NECK_TRACE=neck.log (EXIT_AT=420#1210)
p4_neck_back    wie p4_back  ohne TRACE/FRAMEDUMP + RE15_NECK_TRACE=neck.log (EXIT_AT=440#1210)
```

**(a) Pin + Koerper-Push an der exe (Vorbedingung und Wirkung des Fixes).**
* p4_front re2_ki.log: `PIN slot 5 yaw 3664: Parts gemischt Clip 3 Bild 4 -> (-20609,-15171) ... Leon vorher (-20613,
  -14853) yaw 1440`; st.log F243 `PL(-20618,-14843)` -> **F244 `PL(-20439,-15060,rot=1671)`** (Pin-Bild) bis F415.
  Arm-Ursprung (-21490,-15747): Pin 1052,6 vom Ursprung, Leon **1255,6**, Strahl-Abweichung sin -0,0001.
* p4_back: `PIN slot 5 yaw 3697 ... -> (-20610,-15228) ... Leon vorher (-20433,-15070) yaw 3072`; **F277
  `PL(-20411,-15111,rot=3749)`** (1252,5 vom Ursprung, sin -0,0003), ab F278 (-20409,-15110), Halten bis F428.
* Original-RAM (Fixture, selbst ausgewertet): Leon - Halter-Ursprung im ersten Halte-Bild 1251 (g5/g6/g7/g8/g11),
  1252 (g9/g12/g13), 1255 (g1/g2), 1295 (g3/g4, s. M3). Push und Abstand der exe stimmen mit den sauberen
  Original-Griffen.
* Erklaert der Fix den Befund: ja fuer die Lage — vorher stand Leon auf der Hand (1052 vom Ursprung, Abnahme 2
  F244 `PL(-20609,-15171)`), jetzt im Original-Abstand; die Vorbedingung (Pin auf der Hand, dann Push im selben Bild)
  steht im Protokoll (re2_ki.log PIN-Zeile + st.log F244).
* Vor dem Griff schiebt der sichtbare Arm Leon schon waehrend REACH (p4_back st.log F272-276: x -20622 -> -20469 bei
  Vorwaertslauf) — RE2-Verhalten, im Original-Mitschnitt push_s2 ebenso (Dossier OFFEN 3). Kein Mangel.

**(b) RE1.5-KI.** debug.log RE1.5-Lauf `Opferbank von EM010 geliehen (victim_ok=1, 14 Clips)` (RE2-Lauf: 17 Clips) — der
Lauf lief wirklich im RE1.5-Flavor. Gleicher PIN, gleiche st.log-Lagen; Framedumps p4_front vs p4_re15_front
**96/96 Bilder pixelgleich**, p4_back vs p4_re15_back **91/91 pixelgleich**.

**(c) Bilder.** `SCR/a3_p4_front_kopf_F262-292.png`, `SCR/a3_p4_front_kopfzoom_F262-300.png` (Gesicht) und
`SCR/a3_p4_back_kopfzoom_F296-334.png` (Ruecken), dazu der Vergleich mit Abnahme 2 `SCR/a2_p4_front_kopf_F262-292.png`:
vorher lag der Unterarm quer ueber/in Leons Kopf (F274/F276/F290/F292), jetzt steht Leon weiter vom Fenster weg und die
Hand liegt an Kopf/Nacken; im Ruecken-Griff deckt die Hand in F310-F314 den Hinterkopf. Das gleicht den RE2-VRAM-Bildern
`H_raeume/nb3_re2_original_halten_satz5_satz0.png` (Hand an Kopf/Schulter). Ein Bild ist kein Beleg — das Mass:

**(d) Volumenmass.** Sonde selbst gefahren (`probe_r35_raeume_arme.exe`, EXE-PIN, Lagen = meine exe-Lagen):
```
EXE-PIN front nach Push (byte-true)  Pin (-20439,-15060) Leon-Yaw 1670: waagerecht <120 in 0/19 (min 242), Volumen  5/19 (max 4)
EXE-PIN front Pin ohne Push (NB2)    Pin (-20609,-15171):               waagerecht 0/19 (min 175),          Volumen 12/19 (max 8)
EXE-PIN back  nach Push (byte-true)  Pin (-20411,-15111) Leon-Yaw 3748: waagerecht 7/19 (min 48),           Volumen 15/19 (max 6)
```
Riegel direkt (`test_r35_raeume_arme.exe`): alle Pruefungen ok; (6a) g5/g6/g7/g8/g11 Wurzel-Rahmen max 9/9/10/10/9,
Port 11/19/5/17/2 gegen Original 11/19/7/17/4; (6b) Port y -2500 Gesicht 7/19, Ruecken 19/19 <= Original 11/19, 19/19.

**(e) ABER: die exe dreht Leons Kopf im Halten — das Mass nicht.** Riegel und Sonde rechnen Leons Pose mit
`g_anim_pose_actor = NULL` (test_r35_raeume_arme.c:159, r35_raeume_volumen.h:104, probe_r35_raeume_arme.c:91/166/232),
also ohne die Nacken-FSM in re15_skel_compute_pose (skeleton_common.c:367 ff.). Der Spieler-Zeichner der exe setzt
`g_anim_pose_actor = player_ref` (main.c:8641) — auch im Opfer-Zustand. Gemessen (neck.log, Slot 0 = Spieler):
```
p4_neck  (Gesicht, Halten F244-394, 151 Bilder): fl=00 (Blick aktiv) in allen, acc_yaw -209..415, acc_pitch -187..199,
         Mittel |yaw| 198 (~17 Grad); z.B. F260 tgt=(347,-29) acc=(315,10), F350 tgt=(438,-33) acc=(284,166),
         F390 tgt=(512,-33) acc=(413,118)
p4_neck_back (Ruecken, Halten F277-427, 151 Bilder): fl=00 in allen, acc_yaw -598..248, acc_pitch -186..193,
         Mittel |yaw| 284 (~25 Grad); Ziel an der Klemme: F300 tgt=(-512,-31) acc=(-598,144), F340 acc=(-551,188)
```
Das Original dagegen (Fixture, selbst ausgewertet): Kopf relativ zum Rumpf zwischen Gesicht- und Ruecken-Griff
5..39 Grad, Mittel 19/15/12 Grad (g5/g6, g7/g8, g1/g2); Rumpf/Arme gleich (Part 0 max 1, Part 9/12 max 3 Q12). Und der
eigene Riegel zeigt, dass der Port-Kopf OHNE Nacken-FSM im Ruecken-Griff mit dem Original uebereinstimmt (Hand im
Kopf-Rahmen g6/g8 max 9/10, `R35_ARME_DUMP=1`), im Gesicht-Griff nicht (g5 max 182, g7 164, g11 170 Einheiten;
Richtung bis 39 Grad, Bilder 10..14 des Zyklus). Folgerungen:
1. Im **Ruecken-Griff** haelt das Original den Kopf in der Animationspose; die exe dreht ihn im Mittel ~25 Grad, an der
   Klemme bis ~53 Grad (tgt -512) — der gezeichnete Kopf weicht vom Original ab.
2. Im **Gesicht-Griff (Nutzerfall)** dreht das Original den Kopf (Mittel 12-19 Grad), die exe auch (Mittel ~17 Grad) —
   ob Ziel, Richtung und Klemme dieselben sind, hat niemand gezeigt.
3. Die Zahlen "Gesicht 5/19 / Ruecken 15/19 an der exe-Lage" und (6a)/(6b) beschreiben eine Kopfpose, die die exe
   NICHT zeichnet. Die Dossier-Aussage "Der Port dreht den Kopf im Halten nicht" (H_raeume.md:902/:941,
   test_r35_raeume_arme.c:82) ist fuer die exe falsch.

**Urteil gegen den Wortlaut** ("bewegen sich nicht synchron zu Leon beim schuetteln, dadurch clipped er"): Takt
(Leon = Arm-Bild + 1, im Original-RAM bestaetigt), Pin und jetzt auch der Koerper-Push sind RE2-genau und an der exe
gemessen; der Unterarm liegt nicht mehr durch den Kopf. Die Beziehung Hand <-> Leons Kopf — genau das, was der Nutzer als
Clipping sieht — ist aber an der exe nicht gemessen: die exe dreht den Kopf im Halten, das Mass rechnet ohne, und im
Ruecken-Griff weicht die exe-Kopfpose nachweislich vom Original ab; der RE2-Mechanismus der Kopfdrehung ist nicht
RE'd (Dossier OFFEN 1 ohne Adresse). -> **teilweise**.

---

## Gates

### Suite
* Dossier, Abschnitt "Suite (Nachbesserung 3)" (H_raeume.md:986-989), traegt woertlich
  `=== LOCAL-BUILD-OK (all) — Tests 482/482` (Schranke 478) am Code-Endstand 24029bb4.
* Selbst: `ctest -R "r35_raeume|1210|writher"` -> **11/11 gruen**; `test_r35_raeume_arme.exe` direkt: alle ok (s. 4 d).
  Volllauf: Nachtrag Suite (unten).

### @0x-Gate (Stichproben selbst disassembliert, richtige Binaerdatei)
1. **RE2 PSX.EXE Spieler-Pass** (re2_disasm.py, info/re2leon/PSX.EXE): `80026620 jal 0x8003bfac` / `80026628 jal 0x800355c4`
   (nach dem Spieler-Tick); FUN_800355C4 `800355e0 jal 0x80035408` (Segmente), Liste `800355f8 lw v0,-7372(v0)`
   (0x800ce334) / `80035600 addiu s0,s0,-492` (0x800cfe14), `80035624 andi v0,v0,0x1`, `80035630 jal 0x80034d0c` mit
   `80035634 addu a1,s1,zero` (Spieler = Geschobener), `80035658 sh s3,14(s1)`. Wie zitiert.
2. **RE2 FUN_80034D0C**: `80034d48 or v0,a0,v1` / `80034d4c andi v0,v0,0x2` / `bne` (Ausstieg Bit 2),
   `80034d58 and` / `andi 0x1000`, `80034d68 andi v0,v1,0x4` (Geschobener Bit 4); Schreiber `800350c8 sw v0,56(s1)`
   (PL+0x38) / `800350cc sw v1,64(s1)` (PL+0x40). Decompile RE2_Quellcode_V2/FUN_80034d0c.c Zeile fuer Zeile gegen
   re15_re2arm_body_push_player gelesen: Breitphase `(uint)(d+R) <= (uint)(2R)`, pen = R - SquareRoot0, Hoehenband
   `-(hA+hB) < dy < hA+hB` mit Spieler-Segment-y = y + (+0x98), Schub d*pen/(dist+1), Vorzeichen-Zweig mit
   2*R_Schieber — stimmt. FUN_80035408: Segment = Lage + R_y(+0x76)·(+0x94,+0x98,+0x96) — fuer Lokal-x/z = 0 wie im Port.
3. **RE2 Spieler-Segment** `8003bdc0 addiu v1,zero,450` / `8003bdc4 sh v1,154(s2)` (+0x9A), `8003bde0 addiu v0,zero,-1530`
   / `8003bde4 sh v0,152(s2)` (+0x98), `8003bde8 addiu v0,zero,1530` / `8003bdec sh v0,158(s2)` (+0x9E). Wie zitiert.
4. **EM2D-Overlay** (CDEMD0_EM2D_ai1.BIN, md5 82ab57601b18a0233338d56c287017f1): `80100328 sw v0,488(s0)` (v0 = 1 aus
   `8010031c addiu v0,zero,1`), `8010032c addiu v0,zero,500` / `80100330 sh v0,158(s0)`, `80100338 addiu v1,zero,800` /
   `8010033c sh v1,154(s0)`, `8010035c/60/64 sh zero,148/152/150(s0)`, `80100394 ori v0,v0,0x1404` (Bit 0x4: Arm wird nie
   geschoben). Pin `80100c20 sw v0,-976(at)` (0x800cfc30) / `80100c38 sw v0,-968(at)` (0x800cfc38). Wie zitiert.
5. **Original-Daten**: Fixture `r35_raeume_re2orig.inc` mit `re2_fixture.py` aus den frames.bin neu erzeugt ->
   **byte-gleich**. Arm-Ursprung in jedem Mitschnitt = ROOM2050-Satz (RDT @0x1970 + n*0x16, x/y/z s16 an +10/+12/+14,
   z.B. Satz 5 (-12100,-2480,-7000)); Arm word0 0x0c003405, PL word0 0x40000001, Arm +0xC = Satz + 2. Plausibel echt.
* Konstanten im NB3-Diff: RE2ARM_SEG_R/H, RE2ARM_PL_R/Y/H tragen alle @0x (Code + Commit 412adb3f/9bcc9edd). Wortsuche
  im Spur-Diff (deferred/tunable/interim/for now/faithful/plausib/TODO/approx/vorerst/geschaetzt): nur das Zitat
  "look helper, deferred" in re15_trage1200.h. Neue getenv nur Messschienen (RE15_GEGNER_Y_LOG, RE15_HUNDESCHATTEN_LOG,
  RE15_RE2_TRACE, R35_ARME_DUMP/R35_TRAGE_DBG in Tests); RE15_ARM_ANKER stammt aus 154a73c1 (vorbestehender Mess-Haken).
* Hinweis (kein Mangel): GDB meldet den PC NACH dem Store. Die Schreiber sind 0x800350c8/0x800350cc (Push) und
  0x80100c20/0x80100c38 (Pin); Code-Kommentar enemy_ai_re2_zellenarm.c:228-229 und Commit nennen "pc 0x800350cc /
  0x800350d0" — 0x800350d0 ist `lw v0,0(s2)`. Beim naechsten Anfassen auf die Store-Adressen korrigieren.

### Pfad-/Vertrags-Gate
* `git diff 154a73c1 HEAD --name-only`: keine release/, platform/android/, shared_assets/PSX/,
  tests/unit/CMakeLists.txt, tests/integration/CMakeLists.txt. NB3 an gemeinsamen Dateien: enemy_ai_common.c 2 Zeilen
  (Haken in re15_body_push_player), game_step_common.c 2 Zeilen (Pin-Bild -> grabbed_branch) — kleine Haken.
* Nachrichten-Ids 6/7 in ROOM1190 (zugeteilt 6..11); keine Bank-9-Bits; kein Ereignis belegt. Ok.

### Tests
* Punkt 1-3: messende Riegel, gruen, mit den exe-Laeufen deckungsgleich (Luft-Bilder, Slots, Fallfolge).
* Punkt 4: `unit_r35_raeume_arme` prueft jetzt gegen die Original-RAM ((O1) (4d) (1) (6a) (6b)) und den Port-Push echt.
  Luecken: s. M2.

---

## Maengel

**M1 (Punkt 4): Leons Kopf im Halten ist an der exe nicht original und nicht gemessen.** Die exe fuehrt im Opfer-Zustand
die Spieler-Nacken-FSM (main.c:8641 `g_anim_pose_actor = player_ref`; neck.log Slot 0 `fl=00` in allen 151 Halte-Bildern):
Gesicht acc_yaw -209..415 / pitch -187..199, Ruecken acc_yaw -598..248 (Ziel an der Klemme tgt -512) / pitch -186..193
(Laeufe `SCR/runs/a3_p4_neck`, `a3_p4_neck_back`). Das Original haelt im Ruecken-Griff den Kopf in der Animationspose
(Port-Pose ohne Nacken = Original, Hand im Kopf-Rahmen <= 10, g6/g8) und dreht ihn nur im Gesicht-Griff (5..39 Grad,
Part 8, Fixture). Damit (1) weicht die gezeichnete Kopfpose im Ruecken-Griff im Mittel ~25 Grad vom Original ab, (2) ist
fuer den Gesicht-Griff (Nutzerfall) nicht gezeigt, dass Ziel/Richtung/Klemme der exe-Drehung die des Originals sind,
(3) gelten die Clipping-Zahlen 5/19 und 15/19 an der exe-Lage nicht fuer das gezeichnete Bild, und die Aussage "der Port
dreht den Kopf im Halten nicht" (H_raeume.md:902, :941; test_r35_raeume_arme.c:82) ist falsch. Noetig: den Schreiber der
Part-8-Rotation im RE2-Halten finden (Messweg Dossier OFFEN 1, Schreib-Wachpunkt Leon PL+0x198 + 8*0xAC +0x68..),
Ziel/Klemme/Gate belegen, den Port daran angleichen (RE1.5-Autolook re15_autolook_scan / Nacken-FSM im Opfer-Zustand
des Arms belegt fuehren oder abschalten) und das Volumenmass mit der tatsaechlich gezeichneten Kopfpose nachmessen.

**M2 (Tests, Punkt 4): Der Riegel klammert die Kopfdrehung aus.** (6a) vergleicht nur im Wurzel-Rahmen ("Unabhaengig von
Leons Kopfdrehung", test_r35_raeume_arme.c:177-178) und toleriert +-2 Ueberschneidungsbilder mit der Begruendung "der
Port dreht den Kopf im Halten nicht" (:80-85, R35_6A_ZTOL). Leon wird ohne Nacken-FSM posiert (:159; ebenso
r35_raeume_volumen.h:104, Sonde :91/:166/:232). Gemessen mit `R35_ARME_DUMP=1`: Hand im Kopf-Rahmen Port vs. Original
g5 max 182 / g7 164 / g11 170 Einheiten (bis 39 Grad), ohne dass eine Pruefung faellt. Noetig: Pose wie der Zeichner
(Spieler als g_anim_pose_actor mit Nacken-Zustand) und eine Pruefung der Hand im KOPF-Rahmen bzw. der Part-8-Rotation
gegen die Original-RAM. Dazu (1): die Original-Spanne odmin..odmax = 1251..1295 enthaelt die verfaelschten Laeufe g3/g4
(M3) und laesst damit 44 Einheiten mehr zu, als die sauberen Griffe zeigen (1251..1255).

**M3 (Belegdaten/Doku, Punkt 4): g3/g4 sind durch einen Nachbar-Arm verfaelscht und die Abstandsaussage ist falsch.**
In g3/g4 (Halter Satz 2, Ursprung (-27150,-12050)) steht Leon 1295 vom Halter, aber **1251 vom Ursprung von Satz 0**
(-27150,-12350) — der per RAM auf Sub 7 gesetzte, weiter sichtbare Nachbar hat geschoben; push_s2 zeigt nach dem
Griff-Bild weitere Schuebe (Treffer 4-7: x -25981 -> -25977, z -12492 -> -12494). Trotzdem gehen g3/g4 in die
Original-Spannen "Gesicht 4..17/19, Ruecken 6..19/19" (Dossier, Commit 9bcc9edd) und in (1) ein. Die Aussage "in ALLEN
zwoelf Griffen 1251..1252" (H_raeume.md:830) bzw. "in JEDEM der elf ... 1251..1252" (enemy_ai_re2_zellenarm.c:229-230)
ist falsch: gemessen 1251 (g5-g8, g11), 1252 (g9, g12, g13), 1255 (g1/g2), 1295 (g3/g4). Noetig: g3/g4 kennzeichnen
oder ohne Nachbar-Push wiederholen, Spannen und Kommentar korrigieren.

Hinweise (keine Maengel): GDB-PC = Folgeinstruktion (s. @0x-Gate); Dossier OFFEN 2 (Satz 1/4), OFFEN 3 (Push schon
waehrend REACH, RE2-Verhalten) und OFFEN 4 (Zeichner-Ueberblendung, tote RE1.5-Writher-Maschine) offen gefuehrt.

---

## Nachtrag Suite
Selbst gefahren am HEAD-Stand 9bcc9edd (nach allen Messlaeufen, exe-Kopie vorher entfernt):
`bash re15_port/tools/local_build.sh test` -> `100% tests passed, 0 tests failed out of 482` (1133 s; alle fuenf
Fenster-Haken im ersten Lauf gruen) -> **`=== LOCAL-BUILD-OK (test) — Tests 482/482`**. Suite-Gate haelt.
