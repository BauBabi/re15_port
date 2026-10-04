# Runde 35 — Spur J "affen": UNABHAENGIGE ABNAHME 3

Baum `.claude/worktrees/r35_affen`, Zweig `r35/affen`, geprueft wurde HEAD 458635e1 (Merge-Basis master 154a73c1).
Abnahme vom 2026-10-04 nach Nachbesserung 3 (Maengel M1/M2 aus J_abnahme_2.md). Massstab sind der Wortlaut von AUFTRAG.md
und die Regeln aus VERTRAG.md und CLAUDE.md. Alle Messlaeufe habe ich selbst gefahren (Scratch `scratchpad/jabn3/`).
Die exe-Kopie ist `re15_port/build/platform/pc/re15_pc_jabn3.exe`, md5 62b8fb5a... und damit gleich re15_pc.exe und gleich
der Angabe im Dossier. Beendet habe ich nur eigene PIDs (56540, 66288, 66844).

**Ergebnis: NICHT BESTANDEN.** Alle sechs Nutzer-Punkte sind am gebauten Stand erfuellt. Die Abnahme-2-Maengel M1 und M2
sind in der Sache behoben; ihre Riegel-Zahlen habe ich nachgemessen. Nachbesserung 3 bringt aber zwei neue Maengel:
- **N1:** Eine Aussage an den Nutzer gilt nur im Lauf mit erzwungenem Original-Zustand. Der eigene Riegel-Lauf zeigt das
  Gegenteil, und das steht nicht im Dossier.
- **N2:** Nachbesserung 3 hat den Biss-Takt von Gorilla 1 gegenueber dem Original verschoben (Zyklus 104 statt 103
  Bilder). Das Dossier nennt diesen Takt "unveraendert".

## 0. Bau und Suite
- `bash re15_port/tools/local_build.sh configure` meldet `=== LOCAL-BUILD-OK (configure)`.
- `... build` meldet `ninja: no work to do.` und `=== LOCAL-BUILD-OK (build)`; der Baum ist sauber.
- Die Suite habe ich selbst gefahren: `bash re15_port/tools/local_build.sh all` endet mit `test OK — 495/495 bestanden` und
  **`=== LOCAL-BUILD-OK (all) — Tests 495/495`** (1142,5 s). Alle fuenf Fenster-Haken sind im Lauf gruen (Log `jabn3/suite_all.log`).
  Das deckt sich mit dem Dossier (Zeile 1263).
- `ctest --test-dir re15_port/build -R r35_affen` ergibt 17/17 Passed. Dazu kommen `unit_member`, `unit_maggot_ai` und
  `unit_plc_back_yaw_1090`, alle Passed.
- Riegel `griff`, `szene` und `takt` habe ich zusaetzlich einzeln gefahren (Ausgaben `jabn3/griff_v.txt`, `szene_v.txt`,
  `takt_v.txt`).

## 1. Echter Weg
Alle Raum-Messungen laufen ueber die TUER von ROOM11B0, wie in den Abnahmen 0 bis 2:
`RE15_SET_FLAG=4:243,3:130 RE15_DEBUG_JUMP=11B0@240 RE15_PLAYER_POS=-25500,-28200,1024 RE15_PRESS=square@300..900`, dazu
`RE15_STATE_LOG RE15_EVT_TRACE RE15_ENEMY_DBG` (Skript `jabn3/run.sh`).

Das debug.log zeigt in jedem Lauf dieselbe Kette:
- `[aot] DOOR FIRE slot=1 ... spawn=(-25279,0,17268)`
- `[room] PC loaded room11c0.rdt`
- `[evt] F6 room=11c0 Evt_exec sub=2`
- `[evt] F1088 room=11c0 Evt_exec sub=7`

Laeufe:
| Lauf | Einstellung |
|---|---|
| t1 | keine Eingabe, Bilder F760-1300/10, `RE15_EXIT_AT=2400#11C0` |
| b2 | `RE15_FORCE_CUT=4`, Bilder F1075-1320/15 |
| o7 | `RE15_INPUT_SCRIPT=W34.5,U2.5,W1` (Leon im Freien), Bilder F1180-2100/4 |
| w7 | `RE15_GIVE=7:200 RE15_EQUIP=7`, Bilder F1900-2800/20 |
| w3 | `RE15_GIVE=3:250 RE15_EQUIP=3` |
| w8 | `RE15_GIVE=8:200 RE15_EQUIP=8` |

Feuerskript fuer w3/w7/w8 (`jabn3/fire.txt`): `W40` + 150 x `MA0.1,M0.6`, BASIS=spiel, START=60.

## 2. Nutzer-Punkte — Messprotokoll und Urteil

### Punkt 1 — "nach der Ada Cutscene verschwindet Ada nicht ... muss dann wieder raus kommen, wenn die Monkeys besiegt sind" — **erfuellt**
- **Verstecken** (b2, state.log Slot 1 Typ 0x42):
  - `F1088 st=4/5/2 @(-8965,-14347)` (sub07 Plc_dest), `F1100 (-10873,-12883)`, `F1140 (-17233,-7994)`.
  - **`F1145 st=4/6/0 @(-18025,-7379)`** (Original r3: (-18000,-7403)), danach bis F1300 unveraendert.
  - Bild `jabn3/b2_ada.png` (Cut 4): F1105-F1135 laeuft Ada zwischen die Wagen, ab **F1150 ist sie nicht mehr im Bild**.
- **Zurueckkommen** (w7, Item 7, beide Gorillas wirklich getoetet):
  - `Slot 2 F1449 st=3 hp=-20`, `Slot 3 F1974 st=3 hp=-20`.
  - debug.log: `[evt] F2016 room=11c0 Evt_exec sub=3`.
  - Ada `F2291 st=4/4/2` -> `F2317 st=4/6/0 @(-16264,-8159)`.
  - `[evt] F2595 Evt_exec sub=4` -> `PC loaded room11b0.rdt`.
  - Bild `jabn3/w7_ada_zurueck.png`: F2220-F2380 steht Ada neben Leon. Untertitel "Ada: Okay, it's over now." /
    "...There are some outrageous monsters out there." / "Leon: Yeah... Anyway, let's get out of here.", ab F2620 der Gang zur Tuer.

### Punkt 2 — "kommt noch nicht an der Korrekten Position aus dem Auto" — **erfuellt**
- t1 state.log: Gorilla 1 bis F815 `st=1/0/0 g=0x30 @(-1220,-21568)`, dann **`F816 st=1/0/1 g=0x10 mo=22 @(-3617,-17798)`**.
  Das Original r3 zeigt (-3617,0,-17798), g=0x10, Clip 22.
- Bild `jabn3/t1_wagen.png` (F770-F880): Der Gorilla sitzt F790-F810 in der offenen Heckklappe und steht ab F820 vor dem Pfeiler.
- Pixelvergleich t1 gegen Abnahme 2 (`jabn2/b1`): F760-F1090 sind **bitgleich** (0 Pixel). Abnahme 2 war bitgleich zu
  Abnahme 1, und dort war gegen das Original geprueft. Ab F1100 weichen die Bilder ab; das erklaert sich durch die neue
  Fusssperre (Kriechgang der Gorillas).

### Punkt 3 — "komisch beweglicher Teil am Oberkoerper" — **erfuellt**
- In Ruhe: F790-F880 bitgleich zu Abnahme 2 (siehe Punkt 2).
- In Bewegung: `jabn3/o7_brust_zoom.png` (F1252-F1308, 2x Zoom) zeigt den aufrechten Gorilla mit geschlossenem Fell am Rumpf
  und ohne mitschwingendes Fremdteil.
- Riegel `teile` ist gruen. Der Code (Part 18 am Rumpf @0x80117200-3c, Parts 19-21 weltfest) ist seit Abnahme 2 unveraendert.

### Punkt 4 — "im Original ist die KI zielstrebiger und aggresiver ... Da stimmt noch irgendwas bei der Übernahme nicht" — **erfuellt** (mit Mangel N2)
Original-Referenz: in Abnahme 2 selbst aus GDB dekodiert (`jnb2/g_frei.txt`, `jnb1/g_orig.txt`). Treffer relativ zur
Freigabe: +364 (Heavy), 468, 521, 571, 624, 674, 727, 778, 830, 882, 933, 986, 1036, 1090, 1139; Tod **+1194 = 39,8 s**.
- **t1 (Tuerweg, keine Eingabe; Auswertung `takt.py`):**
  - Freigabe `F1088 bei (-7148,-12363)`.
  - Treffer +365, 470, 524, 574, 628, 678, 732, 782, 836, 886, 940, 990, 1044, 1094, 1148.
  - Tod **F2286 = +1198 = 39,9 s**; der mittlere Abstand ist 52,0.
  - Die Todeszeit stimmt damit auf 4 Bilder (0,13 s) mit dem Original ueberein. Die Zahlen decken sich mit dem Dossier-Lauf t3.
- **Griff in der exe** (o7, state.log):
  - Rear-up S3 `F1199 1/15/0`, **Pin F1204** (Leon mo 1).
  - S3 reitet die Clip-0x1c-Bahn: F1204 (-5609,-16230) -> F1210 (-6472,-15483) -> zurueck F1214 (-6175,-15843). Vorher stand
    der Greifer.
  - Clip 0x10 **F1287 = Pin+83**, Clip 0xb **F1303 = +16**, Leon wieder im Leerlauf **F1329 = +26**; das Original hat
    +83/+16/+26. Waehrend des Griffs bleibt die HP bei 88.
  - Weitere Griffe gibt es bei F1576 und F1914. Das entspricht der Dossier-Messung o7.
- **Riegel `griff` (selbst gefahren, `jabn3/griff_v.txt`):**
  - M2: e1 liegt in T254-T288 hoechstens 28 neben dem Original. Der Abstand e1-e2 ist in T256-T264 3171..3211 (Original
    3168..3209); e2 bewegt sich von T254 bis T262 um 570 (Original 542).
  - Die Wurf-Bahn T265-T290 liegt im Mittel 34 und hoechstens 166 (T290) neben dem Original.
  - Pin T254, erste Platzierung T265, P3/P4 16/16, P5/P6 25/25, Freigabe T378, 0 HP.
  - M1 (c): Die Klemme ab dem Original-T290 trifft die Original-Bahn 124/124 bitgleich. Die Zerlegung stimmt 88/88. Alle
    24/24 Starts, die 1-2 Einheiten vom Original abweichen, enden mehr als 100 neben dem Original, 20 davon mehr als 400.
  - Weg 2: Anker (-7507,-10327), Kette T268-T290 23/23, Freigabe T378 (-4759,-10633), Bahn T291-T414 124/124, Ende T495
    (-3009,-11643) mit HP 76 — alles gleich dem Original.
  - **Lauf 0:** frei T378 bei (-6153,-9995), danach **Biss T405 (76 -> 70)**, Ende (-4200,-10658) mit HP 70 (siehe N1).
- **Riegel `szene` / `takt`:** Gang 207/207, Heavy +365, Tod +1198. In `takt` liegt der Gleichtakt bei 8 Bissen hoechstens
  3 Bilder daneben, der Wechseltakt hoechstens 2 Bilder.
- Urteil zum Wortlaut: Die Todeszeit von 39,9 s gegen 39,8 s, der mittlere Takt von 52,0 gegen 51,8 und der Wurf mit gleicher
  Phase in der exe erfuellen "zielstrebiger/aggressiver wie im Original". Die neue Takt-Drift steht als Mangel N2 unten.

### Punkt 5 — "Brust schlagen Animation ..., die sie im Original manchmal ausführen" — **erfuellt**
- In o7 kommt nach jedem der drei Griffe **Clip 3**: `F1252 S3 1/15/5 c3`, `F1279 1/2/1 c3` (Sub 2), `F1613 1/15/5 c3`,
  `F1640 1/2/1`, `F1962 1/15/5 c3`, `F1989 1/2/1`.
- Bild `jabn3/o7_brust_zoom.png`: Der Gorilla steht aufrecht, die Arme abwechselnd an der Brust (F1276, F1296) und aussen,
  danach wieder auf allen vieren (F1308).
- Clip-3-Wrap: `F1319 c3/69 (-6922,-16439)` -> `F1320 c3/0 (-6934,-16436)`, also 12 Einheiten ohne Sprung. Der groesste
  Schritt von S3 in oder aus Clip 3 ueber den ganzen Lauf ist 86. Der Sprung von etwa 1300 aus Abnahme 2 ist weg.

### Punkt 6 — "erst springen, wenn sie 3x getroffen wurden, nicht nach jeden Schuss" — **erfuellt**
Auswertung mit `p6.py` (Flinch-Eintritt, Exit-Sub, Eintritte in Sub 7), Segment 11C0:

| Lauf | Waffe | Slot 2 Exit-Subs | Slot 3 Exit-Subs | Spruenge nach Treffer |
|---|---|---|---|---|
| w3 | Item 3 (Spur 0) | 3,3,7,3,3,7,3,3,7,3,3,7,3,3,7 (15 Treffer) | 3,3,7,3,3,7 | 5 + 2 (jeweils beim 3. Treffer) |
| w7 | Item 7 (Spur 1) | 3,3,7 | 3,3,7 | 1 + 1 |
| w8 | Item 8 (Spur 1) | 3,3,7 | 3,3 | 1 + 0 |

- Nach dem Sprung setzt der Zaehler zurueck. Spur 2 ist im Riegel `sprung` und `schrot` (gruen) abgedeckt.
- Selektor-Spruenge aus Sub 4 ohne Treffer bleiben: w3 S3 F1380/1653/1705/1746/1798, w7 S3 F1370, w8 S3 F1312 (Hinweis H3).

## 3. RE-Gate (@0x-Belege, Stichproben-Disasm, Guess-Tells)
Den Code-Diff `git diff 154a73c1 HEAD -- re15_port/engine re15_port/include re15_port/platform` habe ich vollstaendig gelesen
(1069 Zeilen). Nachbesserung 3 (`git diff 062e8f82 HEAD`) umfasst affen_11c0.c +11, enemy_ai_common.c 7 Hunks mit je 1-2
Zeilen und re15_affen.h (8)/(9). Jede neue verhaltensrelevante Konstante traegt eine @0x-Adresse:
- 0x800: @0x8011acac-b0
- Ausnahme fuer Typ 0x27: @0x8002af14
- Bild vor dem Vorschub: @0x8001f40c / @0x8001f610-1c
- Fusssperre: @0x8011bf80-c008

Die Commit-Messages tragen dieselben Adressen.

Stichproben, SELBST disassembliert mit `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` in der jeweils richtigen
Binaerdatei:
1. **STAGE1.BIN Pin-Latch Phase 2 -> Phase 3 (0x8011abe8-ad0c):**
   - `sb v0,6(v1)` (+0x6 = 3) @0x8011abfc, `jal 0x8001ac38` mit a0 = s1 = Spieler @0x8011ac18.
   - `ori v0,v0,0x1000` / `sw` @0x8011ac34-38 (g_entity), Spieler-Bit @0x8011ac4c/54.
   - `jal 0x8001a780` @0x8011ac50, `addiu a0,s0,-372` (Spieler+0x34) @0x8011ac60.
   - **`jal 0x8001a8f8` @0x8011acac mit `ori a1,zero,0x800` im Delay-Slot @0x8011acb0**, danach KEIN Sprung.
   - 0x8011acb4: `lw a0,g_entity` @0x8011acc0, `lw a1,132` / `lw a2,364` @0x8011acc4-c8, **`jal 0x8001ad68` @0x8011accc**,
     anim_set `jal 0x8001f314` (a2 = 0, a3 = 0x200) @0x8011ace8-ec, `+0x6 += v0` @0x8011acfc-ad0c.
   - **Bestaetigt.**
2. **PSX.EXE FUN_8001a8f8 und FUN_8001ad68:**
   - FUN_8001a8f8: atan2 `jal 0x8001a6d4` @0x8001a928; `subu`/`addu`/`andi 0xfff` @0x8001a960-68; Grenze `sll 16`/`sra 15`
     (= 2*s1) @0x8001a96c-70; **`slt` @0x8001a974** -> bei s1 = 0x800 immer **`sh a0,106(v1)` @0x8001a984** (Yaw := Peilung).
   - FUN_8001ad68: `jal 0x8001ae38` @0x8001ad84, Identitaet 0x80072d4c, RotMatrixY @0x8001addc, ApplyMatrix @0x8001adec,
     **`+0x34 = lh +0xa0 + vx` @0x8001adf4-ae04, `+0x3c = lh +0xa2 + vz` @0x8001ae08-18**.
   - **Bestaetigt.**
3. **PSX.EXE anim_set FUN_8001f314 / FUN_8001f3bc und STAGE1 FUN_8011bf50 (Fusssperre):**
   - FUN_8001f314: `lbu v0,149(t0)` @0x8001f344/@0x8001f35c, `sw a2,360(t0)` @0x8001f36c, `andi 0x8000` @0x8001f378,
     `jal 0x8001f3bc` @0x8001f38c.
   - FUN_8001f3bc: `lw s1,392(v1)` (Pool) @0x8001f40c; **erst danach** `lbu`/`addiu 1`/`sb v0,149(v1)` @0x8001f610-1c,
     `sltu` @0x8001f624, Wrap `sb zero,149(v1)` @0x8001f63c.
   - FUN_8011bf50: `lw s0,392(v0)` @0x8011bf78; CompMatrix(+0x20, rec) `jal 0x80022da0` @0x8011bf80/a4/b4/c4;
     `+0x34 -= (m.tx - rec[84])` @0x8011bfd4-e8; `+0x3c -= (m.tz - rec[92])` @0x8011bfec-c008.
   - **Bestaetigt.**
4. **Paar-Ausnahme:**
   - PSX.EXE FUN_8002aec4: `lw a0,0(s3)` / `lw v1,0(s2)` @0x8002aef8-fc, **`and v0,a0,v1` / `andi 0x1000` / `bne` @0x8002af14-1c**.
   - Roh-Scan von STAGE1.BIN nach `jal 0x8002aec4` (Wort 0x0c00abb1): 14 Stellen. Im Gorilla-Wurzelschwanz liegt der Aufruf
     @0x80116e40 mit a1 = g_entity (@0x80116e3c) und a0 = s0+20 (Spieler), danach `jal 0x8002b544` @0x80116e50.
     Das ist "aec4(Spieler, Gorilla) im Wurzelschwanz".
   - **Bestaetigt.**

Weitere Pruefungen:
- Alle sieben bf50/c024-Aufrufe (enemy_ai_common.c:8925/8978/9073/9102-9104/9411/9466) stehen direkt hinter `re15_maggot_anim`.
  `s_maggot_pose_bild` ist deshalb beim Lesen immer aus demselben Bild.
- Suche nach Guess-Tells in den hinzugefuegten Zeilen (`deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|approx|
  ungefaehr|geschaetzt|vermutlich|getenv`): Einziger Treffer ist `RE15_AFFEN_FUSS` (Mess-Log, kein Spielverhalten). Also
  0 Guess-Tells, und kein Env-Schalter dient als Abschluss.
- Erklaert der Fix den Befund?
  - Ritt-Platzierung: Die Vorbedingung steht im Protokoll (Abnahme 2 `jabn2/griff_v.txt`: e1 fest T254-T273, e2 fest
    T249-T264). Nachher reitet e1 hoechstens 28 neben dem Original, e2 bewegt sich 570.
  - Fusssperre: Die Vorbedingung steht im Protokoll (Weg 2 vorher: Bisse T420/T480, e1-Sprung von etwa 1300 im Clip-3-Wrap).
    Nachher gibt es bis T495 keinen Biss, und der Wrap-Schritt in der exe ist 12.
  - Beide Fixe erklaeren also ihren Befund. Die Nebenwirkung auf den Biss-Takt erklaeren sie nicht (N2).

## 4. Vertrags-/Pfad-Gate, Tests
- `git diff 154a73c1 HEAD --name-only`, also der eigene Zweig (22 Dateien):
  - keine Pfade unter `release/`, `platform/android/` oder `shared_assets/PSX/`;
  - keine Edits an `tests/unit/CMakeLists.txt` oder `tests/integration/CMakeLists.txt`.
  - Hinweis: `git diff master --name-only` zeigt zusaetzlich release/- und r35_*-Dateien anderer Spuren. Das sind nur die
    Commits, die master seit der Verzweigung bekommen hat (master 87cc8575 = v0.8.22). Der Zweig hat sie nicht angefasst;
    `git log 154a73c1..HEAD -- release re15_port/platform/android re15_port/shared_assets` ist leer.
- Hakengroesse (`git diff -U0 154a73c1 HEAD`): Kein Hunk in einer gemeinsamen Datei hat mehr als 3 Zeilen. Gemeinsame Dateien
  sind enemy_ai_common.c, game_step_common.c, scd_vm.c, actor_locomotion.c, re15_damage.c, anim_select_common.c,
  actor_common.c, emd_common.c und main.c (3 Hunks). Das haelt Vertrag 1.4 ein.
- Bank-9-Bit 82, die Nachrichten-IDs 20..23 und Ereignis 25 sind nicht belegt: 0 Treffer fuer `game_flag_set` /
  `msg_install_text` / `scd_event_fire` / `aot_install` in den hinzugefuegten Zeilen. Es gibt keine neuen Assets.
- Tests messen je Punkt:

  | Punkt | Riegel |
  |---|---|
  | 1 | `band`, `ada` |
  | 2 | `wagen` |
  | 3 | `teile` |
  | 4 | `flug`, `kdsonde`, `biss`, `frac`, `takt`, `griff`, `wand`, `szene` |
  | 5 | `brust`, `anker`, `griff` |
  | 6 | `sprung`, `schrot` |
  | NPC-Band | `npcband` |

  Alle 17 sind gruen. Luecke (siehe N2): `szene` prueft nur Mittel, kuerzesten Abstand und Tod, keinen Einzelbiss gegen die
  Original-Liste. `takt` prueft nur 8 Bisse. Die Drift von 1 Bild je Zyklus bleibt deshalb unentdeckt.

## 5. Maengel (nummeriert, nachpruefbar)
- **N1 (Dossier "Fuer den Nutzer" / Messung nachher): Eine Aussage gilt nur im Lauf mit erzwungenem Original-Zustand. Der
  Port-eigene Lauf widerspricht ihr, und das Dossier sagt es nicht.**
  - `J_affen.md:1243-1247`, "Fuer den Nutzer (Stand Nachbesserung 3)" (3), sagt: "nach einem Wurf bleiben beide Gorillas wie
    im Original zwischen den Wagen haengen, statt Leon gleich wieder zu beissen." Das gilt nur im Riegel-Lauf **Weg 2**, in dem
    Leon, e1 und e2 bei T254 auf Original-Lage und Original-Yaw gesetzt werden (test_r35_affen.c:851-855).
  - Im **Lauf 0** mit eigenem Port-Zustand ab T196 misst `ctest`/`test_r35_affen.exe griff` (`jabn3/griff_v.txt`):
    `frei T378 bei (-6153,-9995)`, dann `T405 hp70 ... e2 5/1 c18/16`. **e2 beisst Leon 27 Bilder nach der Freigabe
    (76 -> 70)**, Ende (-4200,-10658) mit HP 70.
  - Das Original bleibt bis T518 ohne Biss (Dossier Z. 1151, g_griff F496 HP 76).
  - Dieselbe Zeile steht im Protokoll des Bau-Agenten (`jnb3/griff6.txt`: "Ende (-4200,-10658) hp 70"). Sie fehlt aber in
    "Messung nachher (Riegel)" (Z. 1187-1190) und in OFFEN 1 (Z. 1219-1222). OFFEN 1 nennt nur die "chaotisch verschiedene"
    Freigabe, nicht deren Folge.
  - In der exe (o7, Leon im Freien) beisst nach der Freigabe F1329 schon F1342 der naechste Gorilla (88 -> 82). Fuer diese Lage
    gibt es keinen Original-Bezug; die Aussage stuetzt der Lauf aber auch nicht.
  - Nebenbei: Z. 1189 nennt fuer Lauf 0 "Bahn T265-T290 im Mittel 31". Gemessen sind 34, auch in `jnb3/griff6.txt`.
  - **Fehlt:**
    - (a) Aussage (3) auf den gemessenen Fall einschraenken (Weg 2 = Original-Zustand bei T253) oder streichen.
    - (b) Den Biss in T405 von Lauf 0 in "Messung nachher" nennen und unter OFFEN 1 als Folge des Startversatzes fuehren:
      Biss T405 statt keinem bis T518, Ursache, Messweg Lauf 0 gegen Weg 2.
    - (c) Z. 1189 von 31 auf 34 korrigieren.
- **N2 (Punkt 4, Nebenwirkung von Nachbesserung 3, nicht offengelegt): Der Biss-Zyklus von Gorilla 1 (Slot 2) ist jetzt 104
  statt 103 Bilder. Die Einzelbisse driften bis +9 Bilder; das Dossier nennt den Takt "unveraendert".**
  - Gleicher Tuerweg und gleiche Eingabe (keine), Treffer relativ zur Freigabe; Slot 2 beisst die Treffer 3, 5, ... 15
    (state.log t1: F1612/F1716/F2236 Slot 2 in Clip 0x12 Bild 13):

    | Quelle | Treffer 3, 5, 7, 9, 11, 13, 15 | Zyklus |
    |---|---|---|
    | Original (Abnahme 2, GDB) | +521, 624, 727, 830, 933, 1036, 1139 | 103 |
    | Abnahme 2 (`jabn2/t2`, exe md5 da563840, Code-Stand 062e8f82) | +523, 626, 729, 832, 935, 1038, 1141 | 103, gleich dem Original (+2) |
    | **jetzt** (`jabn3/t1`, exe md5 62b8fb5a, HEAD 458635e1) | **+524, 628, 732, 836, 940, 1044, 1148** | **104**, Abweichung +3 ... **+9** |

    Riegel `szene` (`jabn3/szene_v.txt`): 523, 627, 731, 835, 939, 1043, 1147, ebenfalls Zyklus 104. Die Abstaende sind jetzt
    starr 54/50 (vorher 53,51,52,52,51,53,50,54,...). Die Todeszeit bleibt +1198, weil der 16. Treffer von Slot 3 kommt.
  - Zwischen den beiden Staenden hat sich nur der Code von Nachbesserung 3 geaendert (Ritt-Platzierung, Paar-Ausnahme,
    Spielerschub fuer 0x27, Fusssperre mit dem Bild vor dem Vorschub). Die Drift kommt also aus Nachbesserung 3.
  - Das Dossier Z. 1197-1198 sagt: "`szene`: ... Takt Mittel 52,1 / kuerzester 51, Tod + 1198 — **unveraendert gegenueber
    Nachbesserung 2**". Nachbesserung 2 hatte aber laut Dossier Z. 985 "Mittel 51,5 / kuerzester 46" und laut Abnahme 2
    "Mittel 51,6". Die Einzelbisse haben sich geaendert. Die exe-Zeile t3 (Z. 1252-1253) fuehrt +1148 auf, vergleicht es aber
    weder mit +1141 (Nachbesserung 2) noch mit +1139 (Original). OFFEN fuehrt den Punkt nicht.
  - **Fehlt:**
    - (a) Messen, welcher Haken von Nachbesserung 3 den Zyklus von Slot 2 von 103 auf 104 verlaengert. Kandidat ist die
      Fusssperre (9): Sie aendert die Kriech-Strecke B[3] (bf50 @0x80117d90-e04) zwischen zwei Bissen.
    - (b) Das gegen das Original belegen und beheben, oder mit Adresse und Messweg unter OFFEN fuehren.
    - (c) Z. 1197-1198 korrigieren.
    - (d) Den Riegel `szene` um eine Einzelbiss-Pruefung gegen die Original-Liste erweitern (+364 ... +1139, Tod +1194),
      damit eine Drift von 1 Bild je Zyklus nicht unentdeckt bleibt.

### Hinweise (keine Maengel)
- **H1:** OFFEN der Spur (Nachbesserung 3, Z. 1218-1239) ist mit Adressen und Messwegen gefuehrt: Startversatz Lauf 0,
  e2-Kriechen ab T271, gemischte Pose am Sub-2-Eintritt, Clip-Wechsel-Bild der Fusssperre, typ-unabhaengige
  Zombie-Paar-Naeherung fuer 0x20/0x21 sowie die Reste aus Nachbesserung 2.
- **H2:** w8 (M870, festes Feuerskript): Leon stirbt bei Freigabe+936 (Wechseltakt 36, 5 Treffer gesamt). In Abnahme 2 fielen
  mit demselben Skript beide Gorillas. Das liegt an der chaotischen Wechselwirkung fester Eingabe mit geaenderter Bahn. Es ist
  kein eigener Befund, die Exit-Folge 3,3,7 / 3,3 haelt.
- **H3:** Die Auslegung von Punkt 6 ist dieselbe wie in Abnahme 0 bis 2: Der Selektor-Fernsprung (Sub 4 -> 7) kommt ohne
  Treffer. Das ist Original-Verhalten; der Wortlaut zielt auf den Vergeltungssprung.
- **H4:** Die Paar-Ausnahme `re15_affen_griff_paar` gilt fuer Sub 15 Phase 3/4. Das Spieler-Bit faellt im Original erst am Ende
  von P2 (etwa T337). Das UND ist also die Gorilla-Spanne; das passt zum Kommentar in affen_11c0.c.

## 6. Ergebnis
| Punkt | Urteil |
|---|---|
| 1 Ada versteckt sich / kommt nach dem Sieg zurueck | erfuellt (Tuerweg, echte Kills, Bilder b2/w7) |
| 2 Monkey kommt an der korrekten Position aus dem Auto | erfuellt (F816 (-3617,-17798) = Original, F760-F1090 bitgleich zu Abnahme 2) |
| 3 komisch beweglicher Teil am Oberkoerper | erfuellt (Ruhe + Bewegung, Zoom o7) |
| 4 KI zielstrebiger/aggressiver wie im Original | erfuellt (Tod 39,9 s gegen 39,8 s, Wurf in der exe phasengleich); Drift N2 |
| 5 Brust-schlagen-Animation | erfuellt (Clip 3 nach jedem Griff, Zoom-Bild, Wrap ohne Sprung) |
| 6 Sprung erst nach 3 Treffern | erfuellt (Spur 0 und 1 in der exe, Spur 2 im Riegel) |

Gates:
| Gate | Stand |
|---|---|
| Bau | gruen |
| Suite | 495/495, selbst gefahren |
| Riegel | 17/17 |
| @0x-Gate | gehalten: 4 Stichproben-Gruppen bestaetigt, 0 Guess-Tells |
| Pfad-Gate | gehalten |
| Dossier-/Vertrags-Gate | **NICHT gehalten** |

Zum Dossier-/Vertrags-Gate: N1 ist eine Aussage an den Nutzer, die nur im Lauf mit erzwungenem Original-Zustand gilt; der
Biss in Lauf 0 ist nicht offengelegt. N2 ist eine von Nachbesserung 3 verursachte Takt-Drift gegen das Original, die als
"unveraendert" gemeldet und nicht unter OFFEN gefuehrt ist.

**Abnahme 3: NICHT BESTANDEN.**

Messlaeufe, Bilder und Auswertungen liegen in `scratchpad/jabn3/`: run.sh, fire.txt, t1, b2, o7, w7, w3, w8, *.png, ana.py,
takt.py, p6.py, griff_v.txt, szene_v.txt, takt_v.txt, suite_all.log.
