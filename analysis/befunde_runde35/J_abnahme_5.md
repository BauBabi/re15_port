# Runde 35 — Spur J "affen": UNABHAENGIGE ABNAHME 5

Baum `.claude/worktrees/r35_affen`, Zweig `r35/affen`, geprueft: HEAD e635342a (Merge-Basis master 154a73c1).
Abnahme vom 2026-10-04 nach Nachbesserung 5 (Maengel P1/P2 aus J_abnahme_4.md).

Massstab:
- der Wortlaut von AUFTRAG.md,
- die Regeln aus VERTRAG.md und CLAUDE.md.

Alle Messlaeufe habe ich selbst gefahren (Scratch `scratchpad/jabn5/`).
- Messkopie: `re15_port/build/platform/pc/re15_pc_jabn5.exe`, md5 f4f4fbc3..., also gleich re15_pc.exe und gleich der Angabe im
  Dossier (Z. 1761).
- Wegen der SDL-Assertion aus dem Dossier liefen alle Laeufe mit `SDL_ASSERT=always_ignore`.
- Beendet habe ich keinen fremden Prozess, nur eigene Laeufe ueber `timeout`.

**Ergebnis: NICHT BESTANDEN.**

Was haelt:
- P1 und P2 aus Abnahme 4 sind in der Sache behoben.
- Beide Gorillas beissen beim r3-Eintritt in denselben Bildern wie das Original (14/14, Tod +1194).
- Die Suite (495/495, selbst gefahren), alle 17 Riegel, das @0x-Gate und das Pfad-Gate halten.

Warum nicht bestanden: Am gebauten Stand habe ich in der exe zwei neue Befunde zu Punkt 4 gemessen, die im Dossier fehlen.
- **M1:** Der Todesgriff im Sprung (Finisher B[8], 600 Schaden) laeuft im Port durch den falschen Spieler-Handler. Leon springt
  dabei in EINEM Bild 14 000 Einheiten weit und verschwindet aus dem Bild. Dann kommt "YOU DIED". Das Original hat dafuer einen
  eigenen cmd-6-Handler 0x8011c3d4; der ist nicht portiert.
- **M2:** Gorilla 2 springt 9-mal hintereinander auf der Stelle gegen eine Wand. Die Gorillas greifen Leon dabei rund 800 Bilder
  lang nicht an (26,6 s, HP konstant 46). Das Dossier nennt den Befund nicht, und am Original ist er nicht geprueft.

Dazu kommen zwei Dossier-Maengel:
- **M3:** Eine Aussage im Abschnitt "Fuer den Nutzer" geht zu weit.
- **M4:** Eine Riegel-Behauptung stimmt nicht; der zugehoerige Riegel-Zugang ist toter Code.

## 0. Bau und Suite
- `bash re15_port/tools/local_build.sh configure`: `=== LOCAL-BUILD-OK (configure)`.
- `... build`: `ninja: no work to do.` und `=== LOCAL-BUILD-OK (build)`. re15_pc.exe md5 f4f4fbc3, Baum sauber.
- `... all`, selbst gefahren (Log `scratchpad/jabn5_suite_all.log`):
  - Ergebnis: `100% tests passed, 0 tests failed out of 495`, Total 1200,0 s, **`=== LOCAL-BUILD-OK (all) — Tests 495/495`**.
  - Die Suite lief parallel zu meinen exe-Laeufen. Trotzdem waren alle fuenf Fenster-Haken im selben Lauf gruen:

    | Haken | Zeit |
    |---|---|
    | weste_load_pin | 5,87 s |
    | boot_bg_pin | 17,76 s |
    | dark_start_pin | 17,71 s |
    | relatch_pin | 22,23 s |
    | save_counter_pin | 13,44 s |

- Ebenfalls gruen:
  - alle 17 `unit_r35_affen_*`,
  - `unit_member`, `unit_maggot_ai`, `unit_maggot_bone_square`, `unit_maggot_zone_leap`, `unit_plc_back_yaw_1090`.

## 1. Messlaeufe (exe, `scratchpad/jabn5/run.sh`)

Der echte Weg geht ueber die Tuer von ROOM11B0, wie in Abnahme 0 bis 4:
- Einstellung: `RE15_SET_FLAG=4:243,3:130 RE15_DEBUG_JUMP=11B0@240 RE15_PLAYER_POS=-25500,-28200,1024 RE15_PRESS=square@300..900`.
- Logs: `RE15_STATE_LOG`, `RE15_EVT_TRACE`, `RE15_ENEMY_DBG`.
- debug.log in jedem Tuer-Lauf:
  - `[aot] DOOR FIRE slot=1 ... spawn=(-25279,0,17268)`
  - `PC loaded room11c0.rdt`
  - `[evt] F6 ... sub=2`
  - `[evt] F1088 ... sub=7`

| Lauf | Einstellung |
|---|---|
| t1 | Tuerweg, keine Eingabe, Bilder F760-1300/10 |
| b2 | Tuerweg, `RE15_FORCE_CUT=4`, Bilder F1075-1320/15 |
| o7 | Tuerweg, `RE15_INPUT_SCRIPT=W34.5,U2.5,W1` (BASIS spiel, START 60), Bilder F1180-2100/4 |
| j1 | r3-Eintritt wie das Dossier: `RE15_DEBUG_JUMP=11C0@240 RE15_PLAYER_POS=-22604,14455,0 RE15_PRESS=` |
| w7b | Tuerweg, `RE15_GIVE=7:200 RE15_EQUIP=7`, Feuerskript (`fire.txt`: W40 + 150 x `MA0.1,M0.6`), Bilder F2200-2700/20 |
| w3 / w3x / w3y | Tuerweg, `RE15_GIVE=3:250 RE15_EQUIP=3`, Feuerskript; Ende EXIT_AT 2800 / 4500 / 3070, w3y mit Bildern F2500-3068/4 |
| w8 | Tuerweg, `RE15_GIVE=8:200 RE15_EQUIP=8`, Feuerskript |

Zu w3x: Der Lauf endete mit dem Timeout (EXIT=124). Leon starb in 11C0 (F2813), danach ging das Spiel weiter nach ROOM1170. Die
Bilddateien von w3x sind deshalb von 1170 ueberschrieben und unbrauchbar. Die Bilder stammen aus **w3y**: gleiche Eingabe,
Ausstieg F3070 in 11C0. Der Zustandsverlauf ist in w3, w3x und w3y bis F2800 identisch.

## 2. Nutzer-Punkte — Messprotokoll und Urteil

### Punkt 1 — "nach der Ada Cutscene verschwindet Ada nicht ... muss dann wieder raus kommen, wenn die Monkeys besiegt sind" — **erfuellt**

**Verstecken** (b2, state.log Slot 1 Typ 0x42):
- `F1088 st=4/5/2` (sub07), `F1100 (-10873,-12883)`, `F1140 (-17233,-7994)`.
- **`F1145 st=4/6/0 @(-18025,-7379)`**, unveraendert bis F1320. Das Original r3 zeigt (-18000,-7403).
- Bild `jabn5/b2_ada.png`: F1090-F1135 laeuft Ada zwischen die Wagen; ab **F1150 ist sie nicht mehr im Bild** (F1150, F1180,
  F1240, F1315).

**Zurueckkommen** (w7b, Item 7, beide Gorillas wirklich getoetet):
- `Slot 2 F1596 st=3 hp=-20`, `Slot 3 F1995 st=3 hp=-20`.
- debug.log: `[evt] F2037 ... sub=3`.
- Ada: `F2325 st=4/4/2` -> `F2351 st=4/6/0 @(-16264,-8159)`.
- debug.log: `[evt] F2629 ... sub=4`.
- Bild `jabn5/w7_ada_zurueck.png`: Ada steht neben Leon, mit den Untertiteln "Ada: Okay, it's over now." und
  "Ada: ...There are some outrageous monsters out there.". F2680 zeigt den Gang zur Tuer.

### Punkt 2 — "kommt noch nicht an der Korrekten Position aus dem Auto" — **erfuellt**
- t1 state.log:
  - Gorilla 1 bis F815: `st=1/0/0 g=30 @(-1220,-21568)`.
  - **`F816 st=1/0/1 g=10 mo=22 @(-3617,-17798)`**. Das Original zeigt (-3617,-17798), g=0x10, Clip 22.
- Bild `jabn5/t1_wagen.png`: F790-F810 sitzt der Gorilla in der offenen Heckklappe, ab F820 steht er vor dem Pfeiler.
- Pixelvergleich t1 gegen Abnahme 4 (`jabn4/t1/fd`):
  - F760-F1080 sind **bitgleich** (0 Pixel in 33 Bildern).
  - Ab F1090 (nach der Freigabe) weichen die Bilder ab. Das ist erwartet, weil (13) das Kriechen aendert.

### Punkt 3 — "komisch beweglicher Teil am Oberkoerper, der so nicht im Original existiert" — **erfuellt**
- Nachbesserung 5 aendert keinen Part-Code:
  - `git diff 23c2c930 HEAD` beruehrt nur affen_11c0.c (Pool/Fuss-Sperre/Trefferpunkt), game_step_common.c und re15_affen.h.
  - main.c und `re15_affen_part_attach` / `_teil_weltfest` sind unveraendert.
  - Der Pool aus (13) speist nur Fuss-Sperre und Trefferpunkt, nicht den Zeichner.
- In Ruhe: F760-F1080 sind bitgleich zu Abnahme 4 (Kette der Bitgleichheit bis Abnahme 1, dort gegen das Original geprueft).
- In Bewegung: `jabn5/o7_brust_zoom.png` (F1312-F1368, 2x) und `o7_griff_brust.png` zeigen beide Gorillas mit geschlossenem Fell
  am Rumpf, ohne mitschwingendes Fremdteil.
- Riegel `teile` ist gruen.

### Punkt 4 — "im Original ist die KI zielstrebiger und aggresiver ... Da stimmt noch irgendwas bei der Übernahme nicht" — **teilweise**

**Was erfuellt ist (Biss-Takt):**

Original-Liste, relativ zur Freigabe: Heavy +364; Bisse 468, 521, 571, 624, 674, 727, 778, 830, 882, 933, 986, 1036, 1090,
1139; Tod +1194.

- **j1 (r3-Eintritt)**, Auswertung `jabn5/takt.py`:
  - Freigabe F1071 bei **(-7138,-12372) = Original**.
  - Bisse **+468, 521, 571, 624, 674, 727, 778, 830, 882, 933, 986, 1036, 1090, 1139**, Tod **F2265 = +1194 = 39,8 s**.
  - **Alle 14 Bisse und der Tod sind bildgleich mit dem Original.** Die Abstaende 53, 50, 53, 50, 53, 51, 52, 52, 51, 53, 50, 54,
    49, 55 sind die Phasenwanderung des Originals.
  - Der Heavy liegt bei **+363** (Original +364), siehe M5.
- **t1 (Tuerweg):**
  - Freigabe F1088 bei (-7148,-12363).
  - Bisse +468, 522, 571, 625, 674, 728, 777, 831, 880, 934, 984, 1037, 1088, 1140, Tod +1192 (39,7 s).
  - Slot 2 liegt konstant +1, Slot 3 bei 0, 0, 0, -1, -2, -2, -2, -2.
  - Das entspricht genau der Dossier-Angabe (Z. 1661-1665, OFFEN N5-3). Fuer den Tuerweg gibt es keine Original-Aufnahme.
- **Riegel `takt`**, selbst gefahren mit `R35_TAKT_SPUR=1 RE15_AFFEN_FUSS=1` (`jabn5/pt/takt.txt`, `affen_fuss.log`):
  - Treffer 219 269 322 372 425 476 528 580 631 684 734 788 837 892 = Original.
  - Slot 2 ist 7/7 gleich, **Slot 3 ist 7/7 gleich**, Wechseltakt 8/8.
  - Fuss-Sperren-Schritte F196-F209 beider Gorillas, gegen `jnb5/g_stufe_dec.txt` (Original-GDB):
    - e1: (-39,75) (-42,72) (-42,79) (-42,73) (-43,72) (-43,77) (-35,69) (-42,64) (-35,60), dann der B[5]-Teil
      (-78,135) (-180,273) (-263,337) (-255,303) (-211,191);
    - e2: (77,-14) (76,-7) (69,-11) (56,-14) (66,-9) (61,-9) (47,-9) (93,-10) (89,-12) (89,-17) (102,-12) (111,-15) (116,-17)
      (125,-15);
    - **alle bitgleich.**
- **Riegel `szene`** (aus LastTest.log):
  - Einzelbisse `+0 x 14`, Tod +1194.
  - Slot 2 und Slot 3 je +0 .. +0.
- **Griff (o7):**
  - Rear-up S3 F1258, Pin F1262 (Leon mo 1).
  - Clip 0x10 F1345 = Pin +83, Clip 0xb F1361 = +16, Leerlauf F1387 = +26. Das Original hat +83/+16/+26.
  - Biss in freier Lage F1401, 14 Bilder nach dem Leerlauf. Das ist OFFEN N4-2 aus dem Dossier.

**Was nicht erfuellt ist (zwei neue exe-Befunde, beide nicht im Dossier):** M1 und M2 unten.
- Beide treten im Lauf mit Item 3 (w3/w3x/w3y) ueber den echten Tuerweg auf.
- Dort steht Leon nach den Bissen ab F2014 bei (-9975,-10422).
  - Bis F2812 wird er nicht mehr angegriffen; seine HP bleiben konstant bei 46.
  - Die Gorillas machen in dieser Zeit 11 (Slot 2) und 16 (Slot 3) Selektor-Spruenge.
  - Davon gehen 9 auf der Stelle gegen eine Wand (M2).
- In F2813 toetet ihn der Finisher; Leon verschwindet dabei aus dem Bild (M1).
- Den Wortlaut "zielstrebiger/aggressiver wie im Original" erfuellt der Biss-Takt.
- Diese beiden Verhaltensweisen erfuellen ihn nicht, oder sie sind nicht gegen das Original belegt.

### Punkt 5 — "Brust schlagen Animation ..., die sie im Original manchmal ausführen" — **erfuellt**
- o7 state.log: Nach jedem der vier Griffe bis F2100 kommt Clip 3.
  - `F1310 S3 1/15/5 mo=3`, `F1337 S3 1/2/1 mo=3`;
  - `F1657 S3 15/5 mo=3`, `F1684 1/2/1`;
  - `F1786 S2 15/5 mo=3`, `F1813 1/2/1`;
  - `F2019 S3 15/5 mo=3`, `F2046 1/2/1`.
- Bild `jabn5/o7_brust_zoom.png`: Der hintere Gorilla steht aufrecht, die Arme wechselnd an Brust und Kopf (F1320-F1360); Leon
  liegt am Boden.

### Punkt 6 — "erst springen, wenn sie 3x getroffen wurden, nicht nach jeden Schuss" — **erfuellt**
Auswertung mit `jabn5/p6.py` (Segment 11C0):

| Lauf | Slot 2 Exit-Subs | Slot 3 Exit-Subs |
|---|---|---|
| w3 (Item 3, Spur 0) | 3,3,7,3,3,7,3,3,7,3,3,7,3,3,7 | 3,3,7,3,3,7 |
| w7b (Item 7, Spur 1) | 3,3,7 | 3,3,7 |
| w8 (Item 8, Spur 1) | 3,3,7,3 | 3,3,7,3 |

- Der Vergeltungssprung kommt jeweils beim 3. Treffer; danach setzt der Zaehler zurueck.
- Spur 2 ist in den Riegeln `sprung`/`schrot` abgedeckt (gruen).
- Selektor-Spruenge ohne Treffer bleiben: Original-Regel, Hinweis H3 aus Abnahme 4. Ihre Haeufung in w3 ist M2.

## 3. RE-Gate (@0x-Belege, Stichproben-Disasm, Guess-Tells)

Den Code-Diff von Nachbesserung 5 habe ich vollstaendig gelesen: `git diff 23c2c930 HEAD -- re15_port/engine re15_port/include
re15_port/platform`, 293 Zeilen.

| Datei | Umfang |
|---|---|
| affen_11c0.c | +186/-43 |
| game_step_common.c | 2 Haken: -5/+2 und +1 |
| re15_affen.h | +14 |

Jede neue verhaltensrelevante Konstante traegt eine Adresse:
- MVMVA-Worte und Saettigung: lm=0 @0x80022df0;
- Rate 0x200: @0x80118320;
- Winkel-Faltung 0x800/0x1000: FUN_80020510;
- a1 = (0x64,0,0): @0x80118380-84;
- Identitaet: 0x80072d4c.

Stichproben, SELBST disassembliert mit `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` in der jeweils richtigen
Binaerdatei:

1. **PSX.EXE FUN_80022da0:**
   - **`0x4a49e012` @0x80022df0, @0x80022e38, @0x80022e84** und **`0x4a480012` @0x80022eec**. Selbst dekodiert:
     - 0x4a49e012 = sf=1, mx=RT, v=IR, cv=keiner, lm=0;
     - 0x4a480012 = sf=1, RT*V0+TR.
   - Laden der Spalten: `lhu 0/6/12` + mtc2 IR1..3. TR per `ctc2` aus `lw 20/24/28(a0)`, V0 aus `lhu 0/4` + `lwc2 8` von a1+20.
     Ausgabe ueber `swc2` MAC1..3.
   - `affen_comp` bildet das 1:1 nach. **Bestaetigt.**
2. **STAGE1.BIN FUN_8011bf50:**
   - `jal 0x80022da0` @0x8011bf80 (a0 = Entity+0x20, a1 = Pool+0x18), @0x8011bfa4 (s0-320), @0x8011bfb4 (s0-148),
     @0x8011bfc4 (s0+24).
   - s0 = Pool + a1*516 + 2408 (= 14*172), damit die Kette Record 0 -> 12+3a1 -> 13+3a1 -> 14+3a1.
   - `lw a0,84(s0)` @0x8011bfd8 und `lw a0,92(s0)` @0x8011bff8, `sw 52/60(a1)` @0x8011bfe8/@0x8011c008.
   - Elternkette der EM027-EMR selbst ausgelesen (Child-Tabelle @0x74):
     - Knochen 0 -> {1,12,15}, 1 -> {2,4,8}, 12 -> 13 -> 14, 15 -> 16 -> 17, 4 -> 5 -> 6, 8 -> 9 -> 10.
     - Das deckt sich mit den fest verdrahteten Ketten von bf50 und c024 (0 -> 1 -> 4+4a1 -> 5+4a1 -> 6+4a1).
   - **Bestaetigt.**
3. **PSX.EXE FUN_8001e8c8:**
   - `jal 0x80068098` (RotMatrix, a0 = +0x68, a1 = +0x20).
   - Bei `word0 & 0x800` folgt ScaleMatrix `jal 0x80065ff0` mit (+0x166,+0x166,+0x166).
   - ScaleMatrix @0x80065ff0: `multu` + `sra 12` + `andi 0xffff` je Element = `(short)(m*s>>12)`.
   - **Bestaetigt.** (Hinweis: Der Port skaliert bei `render_scale_q12 != 0` statt bei `word0 & 0x800`; fuer den Gorilla ist das
     gleichwertig.)
4. **PSX.EXE FUN_8001f3bc / FUN_80020510** (Decompilate gelesen):
   - Wurzel: `gte_ldIR0(0x1000-iVar10)` + gpf12, dann `gte_ldIR0(iVar10)` + gpl12 = zwei getrennte >>12.
   - Winkel: `FUN_80020510(r, rec+0x78, r, 0x1000 - param_5*frac)`, Faltung `(kf - prev) + 0x800 > 0x1000`, dann
     LoadAverageShort12.
   - frac wird vor dem Abbau gelesen.
   - `affen_winkel` und der Wurzel-Zweig in `re15_affen_pool_anim` entsprechen dem. **Bestaetigt.**
5. **PSX.EXE (14), Treffer-Handler:**
   - FUN_80035af0 [2] endet mit **`jal 0x800245d8` / `ori a0,zero,0x800` @0x80035f18-1c**, dann `acae0 -= acaf2` mit Klemme 0
     @0x80035f20-50 und `jr ra`. Keine Kollision.
   - [3] **`jal 0x800245d8` / `addu a0,zero,zero` @0x8003609c-a0**.
   - FUN_800245d8 ruft nur `jal 0x800659d0` + GTE (MVMVA 0x4a486012 @0x800246ac), keine Klemme.
   - Schwanz des Dispatchers: `jal 0x8002b544` @0x80031cbc, Wandklemme `jal 0x8003b0a4` @0x80031d70.
   - Hauptschleife: `jal 0x8001a50c` @0x8001ce04 -> `jal 0x80031c44` @0x8001ce0c -> **`jal 0x8002bd44` @0x8001ce14**.
   - FUN_8002bd44 (Decompilat) ruft `FUN_8002cabc(&DAT_800aca54, obj, 0)` ohne Abfrage des Spieler-Kommandos. Der Objekt-Pass im
     Flinch-Zweig ist damit belegt.
   - **Bestaetigt.**

Weitere Pruefungen:
- **Guess-Tells** in den hinzugefuegten Zeilen des ganzen Zweigs (`deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|
  approx|ungefaehr|geschaetzt|vermutlich|getenv`): Einziger Treffer ist `getenv("RE15_AFFEN_FUSS")` (Mess-Log). Kein Env-Schalter
  dient als Abschluss.
- **Erklaert der Fix den Befund?** Ja.
  - Vorbedingung (13): Fuss-Sperre F196 (-37,75) gegen das Original (-39,75). Das steht im Protokoll; nachher ist sie bitgleich
    (selbst nachgemessen).
  - Vorbedingung (14): vs10283, Handler-Station (-6593,-12546) gegen (-6560,-12509).
  - Ergebnis: Der Takt von Slot 3 ist von 2/7 auf 7/7 gekommen.

## 4. Vertrags-/Pfad-Gate, Tests
- `git diff 154a73c1 HEAD --name-only` (der eigene Zweig, 24 Dateien):
  - keine Pfade unter `release/`, `platform/android/` oder `shared_assets/PSX/`;
  - keine Edits an `tests/unit/CMakeLists.txt` oder `tests/integration/CMakeLists.txt`;
  - `git log 154a73c1..HEAD -- release re15_port/platform/android re15_port/shared_assets <beide CMakeLists>` ist leer.
  - `git diff master --stat` zeigt zusaetzlich release/- und r35_*-Dateien anderer Spuren. Das sind die Commits, die master
    (87cc8575, v0.8.22) seit der Verzweigung bekommen hat. Der Zweig ist nicht auf master nachgezogen.
- **Haken in game_step_common.c (N5):** -5/+2 und +1 Zeile, beide mit "Runde 35 Spur J (14)". Das haelt Vertrag 1.4.
- **Zuteilung:** keine neuen Bank-9-Bits, Nachrichten-IDs, AOT-Slots, Ereignisse oder Assets.
- **Tests:** 17 Riegel, alle gruen, siehe oben.
  - Neu in N5 und messend: `takt` Slot 3 7/7, `szene` Slot 3 + Todesbiss mit gleichbleibendem Versatz.
  - Neu in `griff`:
    - N1 am Original-Zustand: "kein Biss bis T495, e1 3925 / e2 4996";
    - Empfindlichkeit: "4 Ruhelage, 16 Biss, 0 gleich wie Lauf 0";
    - Wurf-Bahn T265-T289 im Mittel 25, hoechstens 62.
  - Alle drei habe ich in LastTest.log nachgelesen. Fuer M1 und M2 gibt es keinen Riegel.

## 5. Maengel (nummeriert, nachpruefbar)

### M1 (Punkt 4): Der Finisher im Sprung laeuft durch den Wurf-Handler; Leon springt 14 000 Einheiten und verschwindet

**Messung w3y** (Tuerweg, Item 3, Feuerskript), state.log:
- `F2806 S2 1/8/1 mo=21` — Finisher B[8], Clip 0x15, nach dem Vergeltungssprung des 3. Treffers F2786.
- **`F2813 PL hp 46 -> -554`** (Biss-Fenster, `pl->hp -= 600`). Leon steht noch bei (-9975,-10422), Gorilla bei (-9992,-11485).
- **`F2814 PL (-327,483)`**: ein Sprung von rund 14 000 Einheiten in EINEM Bild, nahe am Welt-Ursprung, 13 000 vom Gorilla.
- Danach (-608,941), (-941,1439), (-1300,1888), (-1662,2203), ... (-4330,-285) ab F2840.
- Leon spielt dabei die Wurf-Clips: `mo=1` (Pin) bis F2884, dann `mo=16` (Clip 0x10 = P3 des Wurfs), weiter mit `pst=1` und
  hp -554.

**Bild `jabn5/w3y_finisher_zoom.png`:**
- F2780-F2812: Leon steht zielend am Polizeiwagen.
- F2816: Der Gorilla landet auf ihm.
- **F2820: Leon ist weg**, nur der Gorilla steht dort.
- `w3y_finisher.png`: ab F2900 Ausblendung, F2960/F3040 "YOU DIED".
- Der Nutzer sieht also den Tod nicht; die Figur verschwindet vorher.

**Original (selbst disassembliert):**
- Finisher @0x801191a8-ac `addiu v0,v0,-600` auf player.hp.
- **@0x801191c4-cc `ori v0,zero,0x6` / `sw v0,0x800aca58` = Spieler-Kommando 6**.
- Opfer-Baenke @0x801191dc-9204: acbfc = Gorilla, acbcc = +0x178, acbd0 = +0x17c.
- **Kein** Anker-Setzen.
- cmd-6-Handler 0x800368c0: Hook = `*(0x800aca5a - 514 + Typ*4)` (`lw v0,-514(v0)` @0x8003692c), also 0x800ac858 + 0x27*4 =
  **0x800ac8f4**.
- Gorilla-INIT: **`addiu v0,v0,-15404` = 0x8011c3d4 / `sw v0,0x800ac8f4` @0x8011eab8-c8**.
- Der Wurf-Handler 0x8011c118 gehoert dagegen zu cmd 5: `lw v0,-770(v0)` @0x800368a0 -> 0x800ac7f4, registriert @0x8011ea2c-38.
- **Das Original fuehrt den Finisher also durch 0x8011c3d4 (Dispatch ueber aca59 @0x8011c3d8, `jalr` @0x8011c3fc), nicht durch
  den Wurf.**

**Port:**
- `re15_player_victim_devour(e)` wird aus B[8] aufgerufen (enemy_ai_common.c, case 8).
- Er setzt g_player_victim = 2, Variante 1 (Zombie-Regel `sub_state_1 >= 6`).
- `re15_player_victim_tick` behandelt **jeden** 0x27-Greifer als Wurf (Kommentar "dedizierter Pin-Opfer-Handler 0x8011c118").
  Damit startet der Wurf bei Bild 0x0b, und das Fenster `0x0b + Variante` ist sofort offen.
- `re15_victim_place` setzt Leon aus der Wurzel des Opfer-Clips relativ zu `pl->anchor_*`.
- Diesen Anker setzt nur `re15_affen_pin_anker` im Rear-up (Sub 15) — im Finisher-Pfad ist er alt.
- Spur J hat fuer 0x27 die Begehbar-Klemme in `re15_victim_place` abgeschaltet (`vg_ty != 0x27u`, (6d)). Die Klemme im Schwanz
  (`re15_player_body_and_walls(c, pl, wurf_alt_x, wurf_alt_z)`, game_step_common.c:2373) haelt einen Sprung ins Freie nicht auf.
- `grep -rn 8011c3d4 re15_port` findet nichts.

**Fehlt:**
- (a) Den cmd-6-Handler 0x8011c3d4 des Gorillas disassemblieren: Sub-Tabelle ueber aca59, Platzierung, Clips, Tod.
- (b) Den Finisher im Port durch diesen Handler fuehren statt durch den Wurf.
- (c) Ein Riegel misst: Finisher bei HP < 50 -> kein Lagesprung des Spielers, Tod nach dem Original-Ablauf.
- (d) Vorher als OFFEN mit Adresse ins Dossier. Dort fehlt der Finisher heute ganz.

### M2 (Punkt 4): Endlos-Zonensprung von Gorilla 2 gegen die Wand, rund 800 Bilder ohne Angriff, nicht im Dossier und nicht am Original geprueft

**Messung w3/w3x/w3y** (identisch bis F2800):
- Leon steht ab F2014 bei (-9975,-10422) und zielt; seine HP bleiben 46 bis F2812 (798 Bilder).
- Slot 3 macht ab F2240 **16 Selektor-Spruenge** (Sub 4 -> 7, +0x7 = 3 = Blind-Zonen-Sprung). **9 davon gehen auf der Stelle**
  (F2524-F2844, alle 40 Bilder).
  - Anlauf bei (-13518,-403) mit Yaw-Snap auf 0 (+X).
  - Abflug F2533 auf (-13418,-403); der ganze Flug F2533-F2558 bleibt auf (-13418,-403) stehen.
  - Landung F2559 zurueck auf (-13518,-403), F2563 Sub 4, F2564 wieder Sub 7.
- Slot 2 macht ab F2165 11 Selektor-Spruenge, ueber (-13418,-88), (7682,5987), (7682,-4108) und (2048,-14201).
- Mechanismus im Port (gelesen, nicht gegen das Original gemessen):
  - Zonen-Abfrage attr 0x20 (Pad 19/55, x[-11900..-9300] z[200..9900], Radius hit_radius+100) mit freiem LOS-Latch
    (@0x80117f78-8011802c).
  - Im Flug klemmt die Band-1-Zelle 40/57 (x[-11800..4800] z[-8200..-700], fl 0x13) bei x = -13418.
  - Nach der Landung schiebt die Band-0-Zelle 2 (x[-11900..-1940], fl 0x03) den Gorilla auf -13518 zurueck.
  - Die Wandklemme des Originals laeuft zwar in jedem Tick, auch im Flug (`jal 0x8003b0a4`, a2 = 4, @0x80116e70). Ob das Original
    aber ueberhaupt in diesen Zustand kommt (Lage, Yaw 0, LOS-Latch frei), ist nicht belegt.
- In Abnahme 4 trat der Befund nicht auf. Mit (13)/(14) nimmt Leon jetzt einen anderen Weg und kommt dorthin.

**Fehlt:**
- (a) Den Befund unter OFFEN fuehren.
- (b) Am Original pruefen: DuckStation/GDB mit Gorilla 2 an (-13518,-403), Yaw 0, LOS-Latch frei. Die Lage per
  Speicherschreiben setzen, wie beim Desync-Experiment `jnb1/g_desync`. Dann Sub 4/7 und die Lage je Bild aufzeichnen.
- (c) Je nach Ergebnis beheben oder als Original-Verhalten belegen.

### M3 (Dossier, Nutzer-Aussage zu weit): "Fuer den Nutzer (Stand N5)" (3)
- Wortlaut (Z. 1738-1739): "ihre Lage am Biss-Kreis stimmt Bild fuer Bild mit dem Original ueberein (vorher bis zu 120 Einheiten
  daneben)".
- Gemessen: Riegel `takt` (`jabn5/pt/takt.txt`, S-Zeilen) gegen `jnb1/g_orig_dec.txt`, Port-Bild f gegen GDB-Zeile f, F196-F891.
  - Nur **14 von 696 Bildern** haben beide Gorilla-Lagen bitgleich.
  - Hoechstens **e1 26 (F585), e2 22 (F597)** Einheiten daneben.
- Das Dossier selbst fuehrt den Rest als OFFEN N5-1 (25/17). Es ist derselbe Mangel-Typ wie P1 (a) in Abnahme 4.
- **Fehlt:** (3) auf "im Kriechen F196-F209 bitgleich, danach hoechstens rund 25 Einheiten daneben" einschraenken.

### M4 (Dossier/Riegel): "Riegel prueft die Kette" stimmt nicht; `re15_affen_kette_test` ist toter Code
- Dossier Z. 1579 und re15_affen.h (13) nennen `re15_affen_kette_test` als Riegel-Zugang. Dossier: "(... Elternkette der EMR; fuer
  bf50 = Records 0, 12+3a1, 13+3a1, 14+3a1 @0x8011bf80-c4, Riegel prueft die Kette)".
- `grep -rn kette_test re15_port/tests` ist leer; die Funktion (affen_11c0.c:270) ruft niemand auf.
- Die Kette selbst ist korrekt (EMR selbst ausgelesen, siehe Stichprobe 2). Belegt ist sie nur indirekt, ueber die bitgleichen
  Fuss-Sperren-Schritte.
- **Fehlt:** Entweder pruefen die Riegel die Ketten fuer bf50 (Knochen 14/17) und c024 (Knochen 6/10) ueber
  `re15_affen_kette_test`, oder Funktion und Behauptung entfallen.

### M5 (Dossier, still gewordene Abweichung): Heavy +363 gegen das Original +364
- Gemessen in j1, t1 und im Riegel `szene` ("Freigabe F1066, Heavy +363"). Das Original hat +364: vs9765 - vs9037 = 728 VSyncs,
  Dossier Z. 934-935. Alle 14 Bisse und der Tod sind danach bildgleich.
- N3/N4 schrieben noch "Heavy +363 (Original +364)" (Z. 1386, 1415). In N5 steht nur "Heavy +363" (Z. 1656, 1662), ohne
  Original-Vergleich und ohne OFFEN-Eintrag.
- **Fehlt:** Die 1-Bild-Abweichung des ersten Angriffs unter OFFEN fuehren, mit Messweg: Schreib-Station der HP im Heavy-Bild,
  GDB-Haltepunkt auf player.hp (+0x9a) in vs9763-9767.

### Hinweise (keine Maengel)
- **H1:** Im Flinch-Zweig steht noch der alte Kopfkommentar "then clamp to the room walls/objects so a shove into a wall stops"
  (game_step_common.c, vor `int32_t fl_ax`). Seit (14) klemmt der Handler nicht mehr.
  - `(void)ox; (void)oz;` haelt nur tote Variablen am Leben.
  - Der Kommentar in `re15_player_body_and_walls` ("Ob und wie er [Objekt-Pass] je Kommandowort laeuft, ist nicht
    disassembliert") ist durch die N5-Messung (FUN_8002bd44 ohne Kommando-Abfrage) ueberholt.
- **H2:** Der Objekt-Pass im Flinch-Zweig ruft nur `re15_collision_objects`, nicht `re15_prop_push_tick`. Im Original laeuft
  FUN_8002bd44 ganz, also auch der Schub-Zaehler `puVar4[-2] := 0` ausserhalb des Schiebens. Ohne sichtbare Wirkung in dieser Runde.
- **H3:** Ein Gorilla mit hp = 0 lebt weiter (w3: Slot 2 F2774 hp 0 -> Vergeltungssprung -> Finisher). Das passt zur
  Todesgrenze "< 0" (Riegel-Ausgabe "Tod erst < 0 @0x80012ee8" fuer den Spieler). Fuer den Gorilla habe ich die Grenze nicht
  disassembliert.
- **H4:** Die Ergebnisse von N5 (j1 bildgleich, Riegel) sind echte Fortschritte. M1 und M2 sind keine Folgen von (13)/(14):
  - M1 entsteht im Opfer-Pfad aus (6d)/Altbestand.
  - M2 liegt in der Zonen-Logik aus Runde S5.
  - Beide werden erst durch die geaenderte Kampfbahn erreicht.

## 6. Ergebnis

| Punkt | Urteil |
|---|---|
| 1 Ada versteckt sich / kommt nach dem Sieg zurueck | erfuellt (Tuerweg, echte Kills, b2/w7b) |
| 2 Monkey kommt an der korrekten Position aus dem Auto | erfuellt (F816 (-3617,-17798) = Original, F760-F1080 bitgleich zu Abnahme 4) |
| 3 komisch beweglicher Teil am Oberkoerper | erfuellt (kein Part-Code geaendert, Ruhe bitgleich, Zoom o7) |
| 4 KI zielstrebiger/aggressiver wie im Original | **teilweise**: Biss-Takt j1 14/14 + Tod bildgleich; Finisher-Pfad falsch (M1), Endlos-Zonensprung unbelegt (M2) |
| 5 Brust-schlagen-Animation | erfuellt (Clip 3 nach jedem der vier Griffe in o7) |
| 6 Sprung erst nach 3 Treffern | erfuellt (w3 5+2, w7b 1+1, w8 1+1 Spruenge, jeweils beim 3. Treffer) |

| Gate | Stand |
|---|---|
| Bau | gruen |
| Suite | 495/495, selbst gefahren, alle Fenster-Haken gruen |
| Riegel | 17/17 |
| @0x-Gate | gehalten (5 Stichproben-Gruppen bestaetigt, 0 Guess-Tells) |
| Pfad-Gate | gehalten |
| Dossier-Gate | **NICHT gehalten** (M3, M4, M5; M1/M2 fehlen unter OFFEN) |

**Abnahme 5: NICHT BESTANDEN.**

Messlaeufe, Bilder und Auswertungen liegen in `scratchpad/jabn5/`:
- Skripte: run.sh, fire.txt, ana.py, takt.py, p6.py, sheet.py.
- Laeufe: t1, b2, o7, j1, w7b, w3, w3x, w3y, w8; Riegel-Spur pt/ (takt.txt, affen_fuss.log).
- Bilder: b2_ada.png, t1_wagen.png, o7_griff_brust.png, o7_brust_zoom.png, w7_ada_zurueck.png, w3y_finisher.png,
  w3y_finisher_zoom.png, w3y_tod.png.
- Suite-Log: `scratchpad/jabn5_suite_all.log`.
