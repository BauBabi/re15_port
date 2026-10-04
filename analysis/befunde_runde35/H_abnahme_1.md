# Runde 35 Spur H "raeume" — Abnahme 1 (unabhaengig, 2026-10-04)

Baum `.claude/worktrees/r35_raeume`, Zweig `r35/raeume`, HEAD **cb081bc3** (Nachbesserung 1), Basis 154a73c1
(merge-base mit master; `git diff master` zeigt 188 Dateien nur, weil master seit 154a73c1 weitergelaufen ist —
massgeblich ist `git diff 154a73c1`). Gebaut: `local_build.sh configure` + `build` -> "ninja: no work to do",
`LOCAL-BUILD-OK (build)`; exe 04:09:03 ist juenger als der letzte Code-Commit d0684bcc (04:01:15), Arbeitsbaum
in re15_port/ sauber. Gemessen an der Kopie `re15_port/build/platform/pc/re15_pc_abn1h.exe` (eigener Name, nur
eigene PIDs beendet). Alle Laeufe: `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2`,
Arbeitsordner Scratchpad (`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/
c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/`, Bilder dort als `a1_*.png`, Laeufe unter
`runs/<lauf>/`: debug.log, gy.log = RE15_GEGNER_Y_LOG, st.log = RE15_STATE_LOG,
hs.log = RE15_HUNDESCHATTEN_LOG, fd_*.ppm = RE15_FRAMEDUMP); RE2-Trace `re2_ki.log` neben der exe.
Bildzaehler: nach dem RE15_DEBUG_JUMP beginnt `g_engine.frame_count` im Zielraum bei 0 (Sprung selbst bei
Bild 1768 der Bootkette 1240 -> 1170).

## Ergebnis

| Punkt | Urteil |
|---|---|
| 1 ROOM1190 Hunde-Schatten im Luken-Sprung | **erfuellt** |
| 2 ROOM1190 Zielscheiben-Texte | **erfuellt** |
| 3 ROOM1200 Bahren-Zombie kommt auf die Spieler-Ebene | **erfuellt** |
| 4 ROOM1210 Gitterarme beim Griff (Gleichlauf / Clipping) | **teilweise** |

**bestanden = false** (Punkt 4 nicht erfuellt; Maengel M1-M3 unten).

Stand der Maengel aus Abnahme 0 (H_abnahme_0.md):

| Abnahme 0 | Stand nach Nachbesserung 1 (selbst gemessen) |
|---|---|
| M1 Bahren-Zombie bleibt unter RE2-KI in der Nutzerlage oben | **behoben** — p3_re2_still Sturz F2118-2132, p3_re2_D (= damaliger Fehllauf) F2128-2142 |
| M2 Ruecken-Griff clippt | **besteht** — jetzt in der exe ausgeloest; Konstruktion am RE2-Binaerbild bestaetigt, Original-Bild fehlt -> M2 unten |
| M3 veraltete Kommentare | **behoben** (enemy_ai_common.c:214/:10695/:14669) |
| M4 RE1.5-KI der Arme ungeprueft | **gemessen, Befund**: Griff ohne Kontakt, vom Bau-Agenten als Nutzer-Wahlfrage offen gelassen -> M1 unten |

---

## Punkt 1 — Hunde-Schatten waehrend des Luken-Sprungs (erfuellt)

**Weg:** diesmal ueber den ECHTEN Ausloeser des Hunde-Ereignisses statt RE15_SUBSTART: Strom an
(`RE15_SET_FLAG=4:243`), Bit (5,4) = "Scheibe 0 vorn" per `RE15_SET_FLAG_AT=5:4@1790` (Bank 5 ist raumlokal, daher
nach dem Sprung), Leon vor Schalter Slot 2 (`RE15_PLAYER_POS=-3180,-17300,2048`), Aktion -> Seite 1 -> Seite 2 ->
"Yes" (`RE15_PRESS=square@1800..1803,@1960..1963,@2120..2123`). sub04 setzt (5,5), sub01 startet sub10 — die Hunde
springen durch die Luke.
```
run.sh a1_p12b_s2      RE15_DEBUG_JUMP=1190@gp RE15_PLAYER_POS=-3180,-17300,2048 RE15_SET_FLAG=4:243
                       RE15_SET_FLAG_AT=5:4@1790 RE15_PRESS=<s.o.> RE15_HUNDESCHATTEN_LOG=hs.log RE15_GEGNER_Y_LOG=gy.log
                       RE15_STATE_LOG=st.log RE15_FRAMEDUMP=1790-2900/6:fd_ RE15_EXIT_AT=2900#1190
run.sh a1_p12b_s2_re15 (dasselbe + RE15_AI_FLAVOR=re15)
```
(Erster Versuch a1_p12_s2 mit `5:4@1700`: das Bit wurde noch in ROOM1170 vor dem Sprung gesetzt und beim
Raumaufbau geloescht — kein Ereignis, kein hs.log. Harness-Falle, kein Befund.)

* debug.log: `[setflag-at] Frame 1790: flag(5,4/0x04) = 1`, `[ziel1190] ROOM1190 Slot 2 -> Port-Nachricht 6 ...`.
* st.log Kamera: F2272 cam 8 -> **F2352 cam 11** -> F2450 cam 2 (sub10 setzt Cut 11 selbst).
* hs.log RE2-KI: **168 Bilder mit Koerper ueber Boden, alle `boden_y=0 schatten_y=0`**; Luken-Spruenge
  F2366-2384, F2398-2416, F2430-2448 (hoechster Punkt `koerper_y=-3680` F2367), danach Angriffsspruenge auf Leon
  (2451-2475 ff.). RE1.5-KI: **254 Luft-Bilder, alle `schatten_y=0`**.
* Bilder (Scratchpad): `a1_p1_re2_jump.png` (F2366-2444 Cut 11 + F2456/2468 Cut 2), `a1_p1_re15_jump.png`:
  in keinem Luken-Sprungbild ein Schatten-Quad an der Wand oder in der Luft. In Cut 2 (F2456) liegt der Schatten
  eines angreifenden Hundes am Boden unter ihm (RE2 Quad-Y = +0x1C2 @0x800268c8) — kein Luken-Sprung.

**Erklaert der Fix den Befund?** Ja (Abnahme 0 bestaetigt, Code seitdem unveraendert): vorher `nsh_y = npc->y`
(Koerper), nachher +0x1ba, das FUN_80111398 ab Bild 0xD auf 0 setzt (`sh zero,442(v0)` @0x801114f0). Hinweis: der
Schatten wird nicht geloescht, sondern liegt wie in beiden Originalen am Boden; unter Cut 11 ist der Boden
nicht im Bild — fuer den Wortlaut ("waehrend des Springens raus") gleichwertig.

---

## Punkt 2 — Zielscheiben-Texte (erfuellt)

* a1_p12b_s2 (Slot 2 = 3. von links): Bild `a1_p2_s2_text.png` — F1850 tippt "This target has a surprisingly",
  F1952 "This target has a surprisingly / large number of bullet holes." + Weiter-Pfeil, F2000/F2096 Seite 2
  "There's a switch here. Push it?  Yes No". Nach "Yes" laeuft das Hunde-Ereignis (Punkt 1) — die
  Original-Mechanik des Raetsels bleibt intakt.
* a1_p2_s1 (Slot 1 = 2. von links; `RE15_DEBUG_JUMP=1190@gp RE15_PLAYER_POS=-3180,-20800,2048 RE15_SET_FLAG=4:243
  RE15_PRESS=square@1800..1803,@1960..1963 RE15_FRAMEDUMP=1790-2100/10:fd_`): debug.log `[ziel1190] ROOM1190 Slot 1
  -> Port-Nachricht 7 (wenige Einschuesse) + Original msg 0`; Bild `a1_p2_s1_text.png`: F1930 "This target does not
  have / many bullet holes." + Weiter-Pfeil, F1990 Seite 2 "There's a switch he..." tippt.
* `unit_r35_raeume_ziel` selbst gefahren: 3 Raumzustaende x 4 Slots + ROOM1191, Slot 0/2 -> msg 6, Slot 1/3 ->
  msg 7, Gegenprobe ohne Stempel -> Original. Welche Scheibe "ganz links" ist, hat Abnahme 0 am fahrenden
  Scheibenbild belegt (Cut 5); Code seit Abnahme 0 unveraendert.

---

## Punkt 3 — ROOM1200 Bahren-Zombie (erfuellt)

Lage des Nutzers: Leon nimmt den Minidisc-Player und bleibt direkt unter der Kante bzw. geht rueckwaerts.
```
COMMON = RE15_DEBUG_JUMP=1200@gp RE15_PLAYER_POS=-25880,-16450,2048 RE15_GEGNER_Y_LOG=gy.log RE15_STATE_LOG=st.log
         RE15_PRESS=square@1800..1803,@1900..1903,@2000..2003 RE15_EXIT_AT=3000#1200
run.sh p3_re2_still   COMMON                                     (RE2-KI, keine Eingabe nach dem Aufnehmen)
run.sh p3_re2_D       COMMON + RE15_INPUT_SCRIPT=D3 ab Bild 2100  (= Abnahme-0-Lauf p3c_re2_D, dort NIE herunter)
run.sh p3_re15_still  COMMON + RE15_AI_FLAVOR=re15
Bildlaeufe a1_p3_re2_still_c2 / a1_p3_re2_D_c2 = dasselbe + RE15_FORCE_CUT=2 (Logs Bild fuer Bild gleich)
```
Tuerweg (ohne Raumsprung in den Raum): `a1_p3_door_recon` (`RE15_DEBUG_JUMP=11E0@gp RE15_PLAYER_POS=2000,-24250,3072
RE15_PAD_AT=45:A,46:A`): debug.log `loaded room11e0.rdt` -> `loaded room1200.rdt`, Leon (-20154,-25245); gy.log F1
`[2 t=10 ... g=87 @(-24249,-1800,-18579) b1 f1ba=-1800 f1c0=0000]` — der +0x1ba-Seed (Vorbedingung des Sturzes)
steht auch auf dem Tuerweg. Das Aufnehmen selbst lief wie bei Abnahme 0 und Bau-Agent per Raumsprung mit Leon am
Minidisc-Platz (der Weg von der Tuer um die Tische wurde nicht per Skript gefahren).

Slot 2 (= Sce_em_set id 1, grid 0x87 -> 0x89):

| Lauf | Weckung | oben | Sturz (f1c0 0x8001..) | Landung y=0 b0 | danach |
|---|---|---|---|---|---|
| p3_re2_still | F1909 | Gang F1988, Schnappbiss Sub 14 F2062-2091, Gang F2092 | **F2118** @(-24487,-1810,-16917) | **F2132** f1c0=000f | Sub 3 Griff auf Leon, Sub 5 ... |
| p3_re2_D | F1909 | wie oben bis F2092 | **F2128** @(-24252,-1810,-16949) | **F2142** | Sub 3 Griff |
| p3_re15_still | F1909 | Gang ab F2008 | **F2083** @(-24134,-1810,-16900) | **F2097** | Sub 4/6 Angriff |

Fallfolge in allen drei Laeufen -1810, -1800, -1770, -1720, -1650, -1560, -1450, -1320, -1170, -1000, -810,
-600, -370, -120, 0 = -10 + 20t (FUN_8001bd60, a0 @0x801004dc, a1 @0x80100518). Abnahme 0 mass im Lauf
p3c_re2_D 1011 Bilder oben ohne Abstieg — jetzt 220 Bilder nach der Weckung unten.

Bilder: `a1_p3_re2_D_c2_F2116-2152.png` (Cut 2): F2116-2131 laeuft der Zombie oben auf der Bahren-Ebene, F2134-2140
faellt er an der Kante (Arme schon zum Griff ausgestreckt), F2143 steht er unten bei Leon und greift.
`a1_p3_re2_D_c2_zoom_oben.png`: Ausschnitt oben links, Zombie auf der Bahren-Ebene bis zur Kante.

**Erklaert der Fix den Befund (M1 aus Abnahme 0)?** Ja. Vorbedingung gemessen: Vorher-Lauf des Bau-Agenten
`m1_D_vorher/gy.log` (alte exe) **los=0 in allen 2681 Raumbildern**; meine Laeufe p3_re2_still/p3_re2_D und sein
m1_D_nach **los=1 ab Bild 6** (2995 von 3000). Sub 0 kommt nur ueber das Sicht-Bit weiter (DECISION @0x80101308-1C
/ @0x80101544-7C, Dossier). Sonde `probe_r35_raeume_trage_los` selbst gefahren: alle Plaetze unter der Kante
`los_clear=1`, nur die Band-1-Wand Zelle 9 (u0 FF) blockt ((-24049,-15000), (-24300,-12500)).

Hinweis (kein Mangel): im Rueckwaerts-Lauf beginnt der RE2-Griff (Sub 3, ab F2133 Clip 12) bei y=-1450, also
noch im Fall (9 Bilder), weil FUN_8001bd60 +0x82 schon an der Kante senkt (@0x8001be4c-54) und der Griff-Block auf
gleiches Band prueft. Unter RE1.5-KI beginnt der Angriff ebenfalls im Fall (F2095, y=-370) — dieselbe
Reihenfolge der Original-Funktionen. Vom Bau-Agenten als Beobachtung dokumentiert.

---

## Punkt 4 — ROOM1210 Gitterarme (teilweise)

Echter Weg in allen Laeufen: Tuer ROOM1220 -> ROOM1210 (`RE15_DEBUG_JUMP=1220@gp RE15_PLAYER_POS=-21750,-6400,0
RE15_PAD_AT=45:A,46:A`), Spawn in ROOM1210 bei (-20622,-6560) Blick 0; Eingabe
`RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=80`, `RE15_RE2_TRACE=1`, Bilder Cut 4 (`RE15_FORCE_CUT=4`).
Gemessen: Gehen 60 Einheiten/Bild nach -z (x an der Westwand x=-20622 geklemmt), R/L drehen 96/Bild.

**(a) Gesicht-Griff, RE2-KI** (`a1_p4_front`, `R0.5,W0.3,U14`):
re2_ki.log `[re2arm] PIN slot 5 yaw 3664: Parts-Pose Clip 3 Bild 4 -> (-20588,-15148); Clip 5 Bild 0 (bisher) ->
(-20728,-15056); Leon vorher (-20622,-15020) yaw 1440`; st.log F244 Leon (-20588,-15148) rot 1667 (= Richtung
Arm-Wurzel (-21490,-15747)), Halten bis F415. Bild `a1_p4_front_zoom.png`: Haende an Kopf/Schulter.

**(b) Ruecken-Griff, RE2-KI — erstmals in der exe ausgeloest** (`a1_p4_back`,
`R0.5,W0.3,U3.57,W0.3,R0.57,W0.3,D8`: Leon geht bis z -12980, dreht auf Blick 3072 (+z) und geht RUECKWAERTS
auf Arm 5 zu): re2_ki.log `[re2arm] PIN slot 5 yaw 3705: Parts-Pose Clip 3 Bild 4 -> (-20552,-15206); ...
Leon vorher (-20622,-15220) yaw 3072` -> FUN_80015910: (3705-3072+0x400)&0xFFF = 1657 < 0x800 -> Flip. st.log F277
Leon (-20552,-15206) **rot 3757** = Richtung Arm-Wurzel (~1709) + 2048, Halten bis F448. Bild
`a1_p4_back_zoom.png`: Leon mit dem Ruecken zum Fenster, die Haende greifen von hinten an Kopf/Nacken.
Kamera-Nachlauf (`a1_p4_back_c0..c8`, gleicher Lauf mit RE15_FORCE_CUT 0/1/2/3/5/6/7/8; st.log in allen F278 Leon
(-20552,-15206) rot 3757 = deterministisch): nur Cut 4 zeigt die Griffstelle (`a1_p4_back_cams.png`,
`a1_p4_back_cams2.png` = Flurabschnitte/Zellen ohne Leon). In Cut 4 (`a1_p4_back_kopf.png`, 6-fach, oben Ruecken
F280-296, unten Gesicht F248-264) liegen Arm und Hand im Ruecken-Griff ueber Kopf und Brust (F288/F292/F296); aus
der seitlichen Kamera ist "davor" und "darin" nicht sicher zu trennen — Beleg bleibt das Mass der Sonde.

**(c) RE1.5-KI** (`a1_p4_re15`, `R0.5,W0.3,U4.8`, `RE15_AI_FLAVOR=re15`): Arm 5 (Typ 0x1a, EM01A) Sub 4 ab F278
bei x=-22580; Leon springt von (-20622,-15200) auf (-19764,-15507) und pendelt im Halten zwischen
(-19111..-19318, -15324..-15507) bis F353; Hand-Anker laut Port-Code an der Flurkante x ~ -20622 (Bau-Agent,
Anker-Klemme; nicht separat ausgelesen) -> **rund 1300-1500 Einheiten ohne Kontakt** (im ersten Halte-Bild ~900);
HP 100 -> 60. Bild `a1_p4_re15_zoom.png`: der bleiche Arm greift im Fenster ins Leere, Leon spielt
mitten im Flur die Opfer-Animation. Bestaetigt die Messung des Bau-Agenten (M4 / OFFEN 6).

**Mess-Sonde selbst gefahren** (`probe_r35_raeume_arme.exe`, Phase d=1 = byte-true Leon = Arm + 1):

| Fall | vorher (Pin Clip 5 Bild 0) | nachher (Parts-Pose) |
|---|---|---|
| Gesicht (Leon-Yaw 1665) | Hand < 120 an der Brustachse 9 von 19, min 33 | **0 von 19, min 169** |
| Ruecken (Leon-Yaw 3713) | 2 von 19, min 118 | **11 von 19, min 4** (d=0..18: 6..16 von 19) |

Riegel `unit_r35_raeume_arme` selbst gefahren: Gesicht 0 von 44 (min 170), Ruecken 24 von 44 (min 5); (4a-c) gruen
(Blick 57/2105, Hand-Abweichung 0, Spiegel-Abweichung 2).

**RE-Pruefung der Ruecken-Griff-Konstruktion am Binaerbild** (EM2D-Overlay selbst aus CDEMD0.EMS geschnitten:
TOC RE2-EXE 0x8009ADF4, Typ 0x2D k=1 = Index 117 -> Sektor 2483, 5416 B, md5 82ab57601b18a0233338d56c287017f1,
Wort 0..5 = 0x13, 0x801012a8, 0x8010131c, 0x80101338, 0x8010134c, 0x80101374):
```
80100c18: lw v0,92(v1) / 80100c20: sw v0,-976(at)  (0x800cfc30 PL.x)   80100c24: lw v0,100(v1) / 80100c38: sw v0,-968(at) (PL.z)
80100c3c: lw v0,392(s0) / 80100c44: sw v0,-640(at) (0x800cfd80 = PL+0x188)   80100c48: lw v1,396(s0) / 80100c5c: sw v1,-636(at) (PL+0x18C)
80100c4c: addiu v0,zero,5 / 80100c54: sw v0,-1028(at) (0x800cfbfc = PL+0x4)
801012a8: lui v0,0xf / 801012ac: sw v0,332(s1)      (Opfer-Clipwort 0x000F0000 = Clip 0)
801012e0: jal 0x80015910 / 801012ec: addiu a3,zero,2048 / 801012f8: jal 0x80015558 / 801012fc: addu s0,v0,zero
80101304: beq s0,zero,0x80101320 / 8010130c: lhu v0,118(s1) / 80101314: addiu v0,v0,2048 / 80101318: sh v0,118(s1)
RE2 PSX.EXE 80015910: lh v0,118(a1) / lh v1,118(a0) / subu / addiu 1024 / andi 0xfff / jr ra / slti v0,v0,2048
RE2 PSX.EXE 8001558c: jal 0x800154ac ... 800155cc: slt v0,a0,v0 (a0 < 4096 immer) / 800155d8: j / 800155dc: sh v1,118(s1)
```
Die Konstruktion "eine Opferbank, ein Clip, nur Flip +2048" stimmt am Binaerbild. Daraus folgt der Ruecken-Griff
als gespiegelter Gesicht-Griff — **sofern** die Port-Geometrie des Gesicht-Griffs der RE2-Geometrie entspricht;
das ist nur rechnerisch (Pin @0x80100C18-38, Leon = Arm + 1) und nicht am RE2-Bild belegt (OFFEN 1 des
Bau-Agenten, Messweg pcsx-redux nicht gegangen).

**Urteil gegen den Wortlaut** ("Die Zombie Arme ... wenn sie einen greifen bewegen sich nicht synchron zu Leon
beim schuetteln, dadurch clipped er"): erfuellt fuer den Griff von vorn unter RE2-KI. Nicht erfuellt fuer
(i) den Griff von hinten unter RE2-KI — im Spiel ueber den echten Weg erreichbar (Leon geht/steht mit dem Ruecken zum
Fenster), dort steckt die Hand nach dem Fix in 11 von 19 Halte-Bildern < 120 an Leons Brustachse (vorher 2 von 19);
dass das Original genauso clippt, ist statisch hergeleitet, aber nicht gezeigt — und (ii) die RE1.5-KI (im Spiel
waehlbar), wo der Griff ohne jeden Kontakt laeuft (Leon 1300-1500 neben der Hand) — genau "nicht synchron".
-> **teilweise**, Maengel M1/M2.

---

## Gates

### Suite
* Dossier, Abschnitt "Suite (Nachbesserung 1)", traegt woertlich `=== LOCAL-BUILD-OK (all) — Tests 482/482`
  (Schranke 478) am Endstand. Damit nach Auftrag kein Pflicht-Volllauf.
* Selbst gefahren: `ctest -R "r35_raeume|1210_arme_re2"` -> **5/5 gruen**; die Testprogramme direkt gestartet: echte
  Ausgaben (trage: `[B RE2 Spieler unter der Kante] slot 2: Sturz ab Bild 218, Landung Bild 232`, RE1.5 168/182,
  `[D] ... 5/5 Plaetze unter der Kante frei, Wand Zelle 9 blockt`; arme: s. o.; hundeschatten 21 Luft-Bilder je
  Hund, beide KI; ziel: alle Slot-/Zustandsfaelle 1190 + 1191 inkl. Gegenprobe ohne Stempel). Volllauf: siehe
  Nachtrag Suite.

### @0x-Gate (Stichproben selbst disassembliert)
1. **RE2 0x80050858** (info/re2leon/PSX.EXE): `8005089c addu fp,a2,zero`, `800508bc lhu v1,8(t1)`,
   `800508c4 and v0,v1,fp`, `800508c8 beq v0,zero,0x800508b0` — der zitierte Maskenfilter steht dort.
   Aufrufer Navigator **0x8004a808**: `8004a868 addiu a2,zero,8192` (**Maske 0x2000, Konstante**),
   `8004a86c addiu a3,zero,1`, `8004a8d8 jal 0x80050858`. Weitere jal-Aufrufer: 0x8003da90, 0x8003dc70,
   0x800412f0, 0x800422a8, 0x80045db8, 0x800592e0, 0x8005ccf4, 0x80065678. -> Mangel M3 (Kennzeichnung).
2. **STAGE1 0x8010c8cc-f4** (RE1.5-Writher): `8010c8cc jal 0x8001af20` (rng), `8010c8d4 andi v0,v0,0x1`,
   `8010c8e0 addiu v0,v0,2`, `8010c8e4 sb v0,5(v1)`, `8010c8f4 sb zero,6(v0)` — wie zitiert (+0x5 := 2/3). Dass
   die Zustaende 2/3 keinen Spieler-Griff ausloesen, ist mit diesem Ausschnitt allein nicht belegt (nicht
   weiter verfolgt; fuer M1 unerheblich, weil der Port-Griff dort in jedem Fall ohne Kontakt laeuft).
3. **EM2D-Overlay 0x80100C18-5C / 0x801012A8-1318** und **RE2 0x80015910 / 0x80015558** — s. Punkt 4, am selbst
   geschnittenen Binaerbild: alles wie zitiert.
* Wortsuche im Code-Diff (deferred/tunable/interim/for now/faithful/plausibel/TODO/approx): nur das Zitat des alten
  Kommentars in re15_trage1200.h ("look helper, deferred"). Neue getenv: nur Messschienen
  (RE15_GEGNER_Y_LOG, RE15_HUNDESCHATTEN_LOG, RE15_RE2_TRACE), kein Verhaltensschalter.
* M3 aus Abnahme 0 (Kommentare enemy_ai_common.c:214/:10695/:14669) — erledigt (Diff gelesen).

### Pfad-/Vertrags-Gate
* `git diff 154a73c1 --name-only`: keine release/, platform/android/, shared_assets/PSX/,
  tests/unit/CMakeLists.txt, tests/integration/CMakeLists.txt. Gemeinsame Dateien nur mit kleinen Haken:
  enemy_ai_common.c (Include, Schwerkraft-Aufruf 1 Zeile, LOS-Parameter + Filter 1 Zeile, 2 Aufrufstellen,
  Kommentare), scd_vm.c (2 Includes + 3 Haken), main.c (2 Includes + 2 Haken), re15_actor.h (1 Feld).
* Nachrichten-Ids 6/7 in ROOM1190 (zugeteilt 6..11); keine Bank-9-Bits; Ereignis 27 nicht belegt. Ok.

### Tests
* Punkt 1-3: messende Riegel; P3-Riegel deckt jetzt die Nutzerlage ab (Spieler unter der Kante, beide KI) +
  Sicht an der Kante (D). Punkt 4: Riegel prueft Pin, Gleichlauf Arm+1, Gesicht-Clipping = 0 und die
  Ruecken-Konstruktion (4a-c) — **nicht** die Abwesenheit von Clipping im Ruecken-Griff und **gar nicht** die
  RE1.5-KI-Arme (kein Riegel fuer den kontaktlosen Griff).

---

## Maengel

**M1 (Punkt 4, RE1.5-KI): Gitterarm-Griff ohne Kontakt, als "Nutzer-Entscheid" offen gelassen.** Gemessen
(a1_p4_re15, echter Tuerweg 1220 -> 1210, `R0.5,W0.3,U4.8`, `RE15_AI_FLAVOR=re15`): Arm 5 (EM01A, x=-22580) haelt
F278-F353 (Sub 4), Leon springt auf (-19764,-15507) und steht 1300-1500 Einheiten neben dem Hand-Anker
(Flurkante x ~ -20622), spielt die geliehene Zombie-Opfer-Animation, verliert 40 HP; Bild a1_p4_re15_zoom.png.
Der Bau-Agent fuehrt das als OFFEN 6 mit Wahlfrage an den Nutzer ("ohne Griff wie RE1.5" oder "RE2-EM2D-Griff
auch fuer EM01A") — das widerspricht den Projektregeln "Original oder nicht — keine Wahlfragen" und "Beta ->
Retail: wo RE1.5 unfertig ist, gilt RE2". Noetig: nach der Regel entscheiden und umsetzen (RE1.5-Writher hat
nach @0x8010c8cc-f4 keinen Spieler-Griff -> entweder die Nachruestung im RE1.5-Geschmack entfernen, oder RE1.5
als unfertig einstufen und den RE2-Griff (Pin @0x80100C18-38, Opferbank @0x80100C3C-5C) uebernehmen), mit
Riegel, der fuer die RE1.5-KI Kontakt bzw. Nicht-Griff misst.

**M2 (Punkt 4, RE2-KI): Griff von hinten clippt weiter; "Original clippt genauso" nicht gezeigt.** In der exe
ueber den echten Weg ausgeloest (a1_p4_back: Leon Blick 3072 rueckwaerts auf Arm 5; PIN slot 5 yaw 3705, Leon
danach rot 3757 = Flip). Sonde: Hand < 120 an der Brustachse in 11 von 19 Halte-Bildern, min 4 — vor dem Fix 2
von 19, min 118; der Fix hat das Clipping-Mass in diesem Fall verschlechtert. Die Herleitung "Original =
gespiegelter Gesicht-Griff" ist am Binaerbild korrekt (s. Punkt 4), haengt aber an der ungezeigten Annahme, dass
die Port-Geometrie des Gesicht-Griffs der RE2-Geometrie gleicht. Noetig fuer "erfuellt": OFFEN 1 gehen — ein
RE2-Bild/RAM-Wert eines Ruecken-Griffs (pcsx-redux, `C:/Users/mjoedicke/Downloads/ePSXe2018/re2leon.cue`,
PL+0x38/+0x40/+0x76 + Hand-Part im Halten), der dieselbe Ueberschneidung zeigt — oder den Unterschied finden.

**M3 (Gate, klein): Maskenwert des neuen Sichtfilters ist Port-Mapping, aber nicht so gekennzeichnet.**
enemy_ai_common.c re15_los_ray_blocked (Filter `if (re2_maske && !(re2_maske & c->u0)) continue;`) und
re15_re2_los_clear uebergeben die Aktor-Kollisionsmaske (`e->sca_mask`, Standard 4). Das Original uebergibt im
Zombie-Navigator die **Konstante 0x2000** (`addiu a2,zero,8192` @0x8004a868), die Kraehe 0x8400
(enemy_ai_common.c:2958-2963 zitiert @0x801001C0-E8). Der Code-Kommentar sagt "= Zelle u0 & Maske", das Dossier
"uebergibt die Maske des Aktors (+0x1D7, Default 4)" — beides liest sich, als laese das Original die Aktor-Maske.
Die Abbildung RE2-Satzattribut-Bit 0x2000 -> RE1.5-Zellbyte u0 & 4 ist eine PORT-WAHL (RE1.5-Raumdaten), wirkt seit
dem Fix auch auf die RE2-Kraehe und ist nirgends als solche markiert. Noetig: Kommentar + Dossier mit
"RE2 a2 = 0x2000 @0x8004a868 (Kraehe 0x8400) -> PORT-MAPPING auf u0 & sca_mask" ergaenzen.

---

## Nachtrag Suite
Selbst gefahren am HEAD-Stand cb081bc3 (nach allen Messlaeufen, exe-Kopie entfernt):
`bash re15_port/tools/local_build.sh test` -> `100% tests passed, 0 tests failed out of 482` (1104 s; alle fuenf
Fenster-Haken im ersten Lauf gruen) -> **`=== LOCAL-BUILD-OK (test) — Tests 482/482`**. Suite-Gate haelt.
