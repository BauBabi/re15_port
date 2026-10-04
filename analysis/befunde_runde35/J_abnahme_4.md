# Runde 35 — Spur J "affen": UNABHAENGIGE ABNAHME 4

Baum `.claude/worktrees/r35_affen`, Zweig `r35/affen`, geprueft wurde HEAD 21947700 (Merge-Basis master 154a73c1).
Abnahme vom 2026-10-04 nach Nachbesserung 4 (Maengel N1/N2 aus J_abnahme_3.md). Massstab sind der Wortlaut von AUFTRAG.md
und die Regeln aus VERTRAG.md und CLAUDE.md. Alle Messlaeufe habe ich selbst gefahren (Scratch `scratchpad/jabn4/`).
Die exe-Kopie ist `re15_port/build/platform/pc/re15_pc_jabn4.exe`, md5 b519af3d... und damit gleich re15_pc.exe und gleich
der Angabe im Dossier. Beendet habe ich keinen fremden Prozess.

**Ergebnis: NICHT BESTANDEN.** Alle sechs Nutzer-Punkte sind am gebauten Stand erfuellt, Bau, Suite (495/495), Riegel (17/17),
@0x-Gate und Pfad-Gate halten. N2 aus Abnahme 3 ist in der Sache behoben: Gorilla 1 (Slot 2) beisst in der exe in genau
denselben Bildern wie das Original. Nachbesserung 4 bringt aber zwei neue Maengel, beide vom Typ, an dem Abnahme 3 gescheitert ist:
- **P1:** Nachbesserung 4 verschiebt den Biss-Zyklus von Gorilla 2 (Slot 3) von 104 (= Original) auf 103. Das steht zwar als
  OFFEN N4-1 im Dossier, aber nicht als Folge von Nachbesserung 4. Die Nutzer-Aussage "Die Gorillas beissen jetzt im Rhythmus des
  Originals" ist fuer Gorilla 2 falsch.
- **P2:** Die N1-Behebung wird mit "Startversatz durch (10)-(12) verkleinert" begruendet. Der eigene Riegel misst das Gegenteil:
  Der Startversatz ist in jeder Groesse gewachsen. Die Landung an der Original-Ruhelage ist ein anderer Ausgang derselben chaotischen
  Klemmen-Iteration. Den Ruhelage-Pin in Lauf 0 hatte Nachbesserung 3 genau deshalb ausgeschlossen; Nachbesserung 4 fuehrt ihn
  trotzdem wieder ein.

## 0. Bau und Suite
- `bash re15_port/tools/local_build.sh configure` meldet `=== LOCAL-BUILD-OK (configure)`.
- `... build` meldet `ninja: no work to do.` und `=== LOCAL-BUILD-OK (build)`. re15_pc.exe hat md5 b519af3d, der Baum ist sauber.
- Die Suite habe ich selbst gefahren (`... all`, parallel zu meinen exe-Laeufen). Ergebnis: `test OK — 495/495 bestanden` und
  **`=== LOCAL-BUILD-OK (all) — Tests 495/495`**, Total Test time 1131,5 s (Log `jabn4_suite_all.log`).
  - Alle fuenf Fenster-Haken sind im selben Lauf gruen: weste_load_pin 5,7 s, boot_bg_pin 18,0 s, dark_start_pin 17,8 s,
    relatch_pin 22,6 s, save_counter_pin 14,4 s.
  - Das deckt sich mit dem Dossier (Zeile 1498).
- Gruen sind ausserdem alle 17 `unit_r35_affen_*`, dazu `unit_member`, `unit_maggot_ai`, `unit_maggot_bone_square`,
  `unit_maggot_zone_leap` und `unit_plc_back_yaw_1090`.
- Die Riegel `griff`, `szene` und `takt` habe ich zusaetzlich einzeln gefahren: `jabn4/griff_v.txt`, `szene_v.txt`, `takt_v.txt`.

## 1. Echter Weg
Alle Raum-Messungen laufen ueber die TUER von ROOM11B0, wie in den Abnahmen 0 bis 3 (`jabn4/run.sh`):
`RE15_SET_FLAG=4:243,3:130 RE15_DEBUG_JUMP=11B0@240 RE15_PLAYER_POS=-25500,-28200,1024 RE15_PRESS=square@300..900`, dazu
`RE15_STATE_LOG RE15_EVT_TRACE RE15_ENEMY_DBG`.

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
| w7 / w7b | `RE15_GIVE=7:200 RE15_EQUIP=7`, Feuerskript |
| w3 | `RE15_GIVE=3:250 RE15_EQUIP=3`, Feuerskript |
| w8 | `RE15_GIVE=8:200 RE15_EQUIP=8`, Feuerskript |

Zu w7 und w7b:
- w7 lief ueber das Raumende hinaus: Sub 4 F2629 -> ROOM11B0 -> Tuer -> ROOM1260. Der Ausstieg `EXIT_AT ...#11C0` griff deshalb nicht,
  und Ende war erst nach dem Timeout.
- Die Bildnummern der Folgeraeume ueberschrieben dabei die Bild-Dateien. Die Bilder stammen deshalb aus **w7b**: identisch, aber
  `RE15_EXIT_AT=2700#11C0` und Bilder F2200-2700/20. Der Zustandsverlauf in 11C0 ist in w7 und w7b gleich (Tode F1701/F1996,
  sub03 F2038, sub04 F2629).

Feuerskript (`jabn4/fire.txt`): `W40` + 150 x `MA0.1,M0.6`, BASIS=spiel, START=60.

## 2. Nutzer-Punkte — Messprotokoll und Urteil

### Punkt 1 — "nach der Ada Cutscene verschwindet Ada nicht ... muss dann wieder raus kommen, wenn die Monkeys besiegt sind" — **erfuellt**
- **Verstecken** (b2, state.log Slot 1 Typ 0x42):
  - `F1088 st=4/5/2` (sub07 Plc_dest), `F1100 (-10873,-12883)`, `F1140 (-17233,-7994)`.
  - **`F1145 st=4/6/0 @(-18025,-7379)`**, danach bis F1320 unveraendert. Das Original r3 zeigt (-18000,-7403).
  - Bild `jabn4/b2_ada.png`: F1105-F1135 laeuft Ada zwischen die Wagen, ab **F1150 ist sie nicht mehr im Bild** (F1150, F1180,
    F1240, F1315).
- **Zurueckkommen** (w7/w7b, Item 7, beide Gorillas wirklich getoetet):
  - `Slot 2 F1701 st=3 hp=-20`, `Slot 3 F1996 st=3 hp=-20`.
  - debug.log: `[evt] F2038 room=11c0 Evt_exec sub=3`.
  - Ada `F2325 st=4/4/2` -> `F2351 st=4/6/0 @(-16264,-8159)`.
  - `[evt] F2629 Evt_exec sub=4` -> `PC loaded room11b0.rdt`.
  - Bild `jabn4/w7_ada_zurueck.png`: F2260-F2420 steht Ada neben Leon, Untertitel "Ada: Okay, it's over now." und
    "Ada: ...There are some outrageous monsters out there.". F2680 zeigt den Gang zur Tuer.

### Punkt 2 — "kommt noch nicht an der Korrekten Position aus dem Auto" — **erfuellt**
- t1 state.log: Gorilla 1 bis F815 `st=1/0/0 g=0x30 @(-1220,-21568)`, dann **`F816 st=1/0/1 g=0x10 mo=22 @(-3617,-17798)`**.
  Das Original r3 zeigt (-3617,0,-17798), g=0x10, Clip 22.
- Bild `jabn4/t1_wagen.png`: F790-F810 sitzt der Gorilla in der offenen Heckklappe, ab F820 steht er vor dem Pfeiler.
- Pixelvergleich t1 gegen Abnahme 3 (`jabn3/t1/fd`): F760-F1080 sind **bitgleich** (0 Pixel in 33 Bildern). Ab F1090, also nach der
  Freigabe F1088, weichen die Bilder ab; das erklaert der neue Kriechgang.

### Punkt 3 — "komisch beweglicher Teil am Oberkoerper" — **erfuellt**
- In Ruhe: F760-F1080 bitgleich zu Abnahme 3 (siehe Punkt 2). Die Kette der Bitgleichheit reicht ueber Abnahme 2 bis Abnahme 1, und
  dort war gegen das Original geprueft.
- In Bewegung: `jabn4/o7_brust_zoom.png` (F1328-F1384, 2x Zoom) zeigt beide Gorillas mit geschlossenem Fell am Rumpf und ohne
  mitschwingendes Fremdteil.
- Nachbesserung 4 aendert keinen Part-Code (main.c, `re15_affen_part_attach` / `_teil_weltfest` sind unveraendert). Riegel `teile` ist gruen.

### Punkt 4 — "im Original ist die KI zielstrebiger und aggresiver ... Da stimmt noch irgendwas bei der Übernahme nicht" — **erfuellt** (Maengel P1/P2 betreffen Dossier und Riegel)
Original-Referenz (Abnahme 2, GDB, relativ zur Freigabe): Heavy +364; Bisse 468, 521, 571, 624, 674, 727, 778, 830, 882, 933,
986, 1036, 1090, 1139; Tod **+1194 = 39,8 s**.

**t1** (Tuerweg, keine Eingabe; Auswertung `jabn4/takt.py`):
- Freigabe `F1088 bei (-7148,-12363)`. Heavy +363.
- Bisse +468, 521, 571, 624, 674, 727, 777, 830, 880, 933, 983, 1036, 1086, 1139.
- Tod **F2277 = +1189 = 39,6 s**.
- Zuordnung der Bisse ueber die state.log-Zeile vor dem HP-Wechsel (Clip 0x12, Bild 13/14): Slot 2 beisst die Treffer 3, 5, ... 15,
  Slot 3 die Treffer 2, 4, ... 14 und den Todesbiss.

| Quelle | Slot 2 (Treffer 3, 5, ... 15) | Slot 3 (Treffer 2, 4, ... 14, Tod) |
|---|---|---|
| Original | 521, 624, 727, 830, 933, 1036, 1139 | 468, 571, 674, 778, 882, 986, 1090, **1194** (Zyklen 103, 103, 104, 104, 104, 104, 104) |
| Abnahme 3 (`jabn3/t1`, Stand 458635e1) | 524, 628, 732, 836, 940, 1044, 1148 (+3 ... +9, Zyklus 104) | 470, 574, 678, 782, 886, 990, 1094, 1198 (+2, +3, dann **+4 konstant**, Zyklus 104) |
| **jetzt** (`jabn4/t1`, Stand 21947700) | **521, 624, 727, 830, 933, 1036, 1139 = Original 7/7** | 468, 571, 674, 777, 880, 983, 1086, 1189 (0, 0, 0, **-1, -2, -3, -4, -5**, Zyklus 103) |

Weitere Messungen:
- **A/B im selben Tick, in der exe sichtbar:** Von 30 Wechseln Sub 3 -> 5 in t1 hat jetzt **keiner** mehr ein Zwischenbild mit
  `ss2=0` (nur Entscheid). In Abnahme 3 hatten alle 30 eines.
- **Weiterkriechen nach dem Biss:** Schrittweite in den ersten fuenf Bildern nach Sub 5 -> 3:
  - jetzt 0/49/82/83/51 (F1569 S3), 0/51/84/85/52 (F1575 S2);
  - Abnahme 3: 0/0/11/17/31;
  - Original (Dossier, g_orig_dec T384-T387): 55/81/89.
- **Griff in der exe** (o7, state.log):
  - Rear-up S3 `F1272 1/15/1`, **Pin F1276** (Leon mo 1).
  - Clip 0x10 **F1359 = Pin+83**, Clip 0xb **F1375 = +16**, Leerlauf **F1401 = +26**; das Original hat +83/+16/+26. HP 76 bleibt.
  - Weitere Griffe: Pin F1648 (Clip 0x10 F1720 = +72) und F2007 (F2079 = +72). Das entspricht der Dossier-Angabe "Rueckgriff im
    Clip-0x1c-Bild 16, Ritt bis Clipende 72".
  - Biss nach der ersten Freigabe: Slot 2 trifft F1415 (76 -> 70), 14 Bilder nach F1401. Das ist so im Dossier unter OFFEN N4-2
    gefuehrt; eine Original-Spur fehlt.
- **Riegel (selbst gefahren):**
  - `takt` Gleichtakt: 219, 269, 322, 372, 425, 475, 528, 578, 631, 681, 734, 784, 837, 887.
    - Original (GDB-Zeilen mit HP-Wechsel, selbst aus `jnb1/g_orig_dec.txt` gezaehlt): 219, 269, 322, 372, 425, 476, 528, 580, 631,
      684, 734, 788, 837, 892.
    - e1 ist 7/7 gleich; e2 liegt bei 0, 0, -1, -2, -3, -4, -5.
    - Die Zeilen-Zuordnung (Port-Bild f = GDB-Zeile f) habe ich nachgeprueft: Die Startzeile F195 traegt e1 c5/10 (-5525,-14883) und
      e2 c5/16 (-8915,-12487) r93 L15. Das ist der Startzustand des Harness, und Zeile F196 c5/11 entspricht Port-S196.
  - `szene`: Einzelbisse Port - Original `+0 -1 +0 -1 +0 -1 -1 -1 -2 -1 -3 -1 -4 -1`, Tod +1189.
- **Urteil zum Wortlaut:** "Zielstrebiger/aggressiver wie im Original" ist erfuellt.
  - Todeszeit 39,6 s gegen 39,8 s.
  - Gorilla 1 beisst bildgleich mit dem Original.
  - Der Biss setzt im Entscheidungsbild ein, und der Gorilla kriecht nach dem Biss sofort weiter wie im Original.
  - Der Griff in der exe hat dieselbe Phase wie das Original.
  - Die neue Zyklus-Drift von Gorilla 2 betrifft 1 Bild je 103 Bilder; sie steht als Dossier-Mangel P1 unten.

### Punkt 5 — "Brust schlagen Animation ..., die sie im Original manchmal ausführen" — **erfuellt**
- In o7 kommt nach jedem der drei Griffe **Clip 3**: `F1324 S3 1/15/5 c3`, `F1351 1/2/1 c3` (Sub 2); `F1685 15/5 c3`,
  `F1712 1/2/1`; `F2044 15/5 c3`, `F2071 1/2/1`.
- Bild `jabn4/o7_brust_zoom.png`: Der hintere Gorilla (S3) steht aufrecht, die Arme abwechselnd an Brust und Kopf (F1352, F1368);
  Leon liegt am Boden. `o7_griff_brust.png` zeigt die Uebersicht F1272-F1396.
- Clip-3-Wrap: `F1391 c3/69 (-7431,-14383)` -> `F1392 c3/0 (-7433,-14382)`, also 2 Einheiten. Der groesste Schritt von S3 in oder aus
  Clip 3 ueber den ganzen Lauf ist **86** (F1374). Ein Sprung tritt nicht auf.

### Punkt 6 — "erst springen, wenn sie 3x getroffen wurden, nicht nach jeden Schuss" — **erfuellt**
Auswertung mit `jabn4/p6.py` (Flinch-Eintritt, Exit-Sub, Eintritte in Sub 7), Segment 11C0:

| Lauf | Waffe | Slot 2 Exit-Subs | Slot 3 Exit-Subs | Spruenge nach Treffer |
|---|---|---|---|---|
| w3 | Item 3 (Spur 0) | 3,3,7,3,3,7,3,3,7,3,3,7,3,3 (14 Treffer) | 3,3,7,3,3,7,3,3,7,3,3,7,3,- | 4 + 4, jeweils beim 3. Treffer |
| w7 | Item 7 (Spur 1) | 3,3,7 | 3,3,7 | 1 + 1 |
| w8 | Item 8 (Spur 1) | 3,3,7,3 | 3,3,7,3 | 1 + 1 |

- Nach dem Sprung setzt der Zaehler zurueck. Spur 2 ist im Riegel `sprung` und `schrot` abgedeckt (gruen).
- Selektor-Spruenge aus Sub 4 ohne Treffer bleiben: w3 S3 F1379/F1467, w7 S3 F1369, w8 S3 F1311 (Hinweis H3).

## 3. RE-Gate (@0x-Belege, Stichproben-Disasm, Guess-Tells)
Den Code-Diff von Nachbesserung 4 habe ich vollstaendig gelesen: `git diff e286b87c HEAD -- re15_port/engine re15_port/include
re15_port/platform`, 303 Zeilen. Er umfasst affen_11c0.c +92, re15_affen.h (10)/(11)/(12) und enemy_ai_common.c mit 19 Hunks.
Jede neue verhaltensrelevante Konstante traegt eine @0x-Adresse:

| Konstante | Beleg |
|---|---|
| Rate 0x200 | @0x80118320 / @0x8001f380-88 |
| vx 0x64, nur Knochen 9 | @0x80118380-84 |
| Record 9 + 0x40 | @0x801183c0 |
| A/B-Folge | @0x80117358-78 |
| Fusssperre | @0x8011bfd4-c008 |
| Trefferpunkt | @0x8001c078 |

Die Commit-Messages tragen dieselben Adressen.

Stichproben, SELBST disassembliert mit `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` in der jeweils richtigen Binaerdatei:
1. **STAGE1.BIN Brain-Rumpf (0x80117318-7c):**
   - `lbu v0,5(v0)` @0x80117324, `addiu at,at,5096` = 0x801213e8 @0x80117334, `jalr v0` @0x80117344.
   - Danach **erneut `lbu v0,5(v0)` @0x80117358**, `addiu at,at,5160` = 0x80121428 @0x80117368, `jalr v0` @0x80117378.
   - Tabellen selbst gelesen:
     - A = {7484, 7668, 7858, 7a3c, 7e40, 8268, 8544, 8900, 8dd4, 9284, 9634, 9998, 9c38, 9f70, a378, a878}
     - B = {7574, 7764, 7860, 7c90, 8110, 8270, 854c, 8908, 8ddc, 936c, 971c, 9a6c, 9d0c, a1f8, a44c, a960}
     - Beides gleich dem Dossier.
   - A[2], A[5], A[6], A[7], A[8] sind `jr ra; nop`. A[9..14] haben Logik; der Port erreicht diese Subs aber aus keinem A-Wechsel
     (Zonen-Spruenge gehen nach Sub 7).
   - **Bestaetigt.**
2. **STAGE1.BIN B[5] (0x80118300-0x8011843c):**
   - anim_set `jal 0x8001f314` @0x8011831c mit **`ori a3,zero,0x200`** im Delay-Slot @0x80118320, bf50 `jal 0x8011bf50` @0x8011833c,
     Pool `lw s2,392(v0)` @0x80118350.
   - Vektor aus 0x80072d60 nach sp+16, selbst gelesen als (0,0,0,0); dann **`ori v0,zero,0x64` / `sw v0,16(sp)` @0x80118380-84**.
   - **`addiu a0,s2,1612` @0x801183c0** (Delay-Slot), r `ori a2,zero,0x3e8` @0x801183c8, `jal 0x8001bff8` @0x801183cc.
   - Fenster-Tabelle 0x8012146c = {12,13,14,15}.
   - Record-Groesse 172: 9*172+64 = 1612, ebenso 6/10/5 -> 1096/1784/924. Das ist konsistent mit (11).
   - **Bestaetigt.**
3. **PSX.EXE FUN_8001bff8:**
   - Identitaet aus 0x80072d4c, selbst gelesen als (4096,0,0 | 0,4096,...), nach sp+48 (@0x8001c010-54).
   - Translation := a1 (@0x8001c058-7c), **`jal 0x80022da0` @0x8001c078**.
   - `lhu v0,36(sp)` / `lhu v1,0(s1)` @0x8001c080-84, `(pl - (w - r)) & 0xffff`, `slt` gegen 2r @0x8001c088-a0, Z @0x8001c0a8-c0.
   - **Bestaetigt.**
4. **PSX.EXE Zeichen-Schleife und anim_set:**
   - `jal 0x8001e8c8` @0x8001d09c (Spieler) und **@0x8001d108** (je Entity mit Bit 1 @0x8001d0fc), danach `+0x40 := +0x34`
     @0x8001d11c-24.
   - FUN_8001ef54 (Decompilat Z. 7) `FUN_80022da0(param_1[0x1b], param_1+6, param_1+0x10)` = rec+0x18 -> rec+0x40.
   - FUN_8001f314: **`sll v0,a3,16` / `sra` / `sw v0,16(sp)` @0x8001f380-88** (5. Argument = Rate).
   - FUN_8001f3bc Decompilat: frac = +0x8f wird in Z. 23 gelesen, `+0x8f -= 1` erst in Z. 78, Gewicht `0x1000 - param_5*frac` in
     Z. 81. Das passt zu `anim_frac` vor dem Abbau im Port.
   - **Bestaetigt.**
5. **STAGE1.BIN FUN_8011bf50 (0x8011bf68-c014):**
   - `jal 0x80022da0` @0x8011bf80/a4/b4/c4, Ziel sp+16.
   - **`lw a0,84(s0)` @0x8011bfd8** / **`lw a0,92(s0)` @0x8011bff8**: rec+0x54/+0x5c = t-Spalte der +0x40-Matrix des Zeichners.
   - `subu` / `sw 52(a1)` / `sw 60(a1)` @0x8011bfe0-c008.
   - **Bestaetigt.**
6. **STAGE1.BIN A[3]-Gate (N1):**
   - `lbu` Spieler+0x93 @0x80117a54, `bne` @0x80117a5c.
   - `jal 0x8001a804` mit 0xbb8/0x180 @0x80117a60-6c.
   - `lh v0,476(v1)` / `bne` @0x80117a88-90, `sb 5` @0x80117a98.
   - **Bestaetigt.**

Weitere Pruefungen:
- **Guess-Tells** in den hinzugefuegten Zeilen des ganzen Zweigs (`deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|
  approx|ungefaehr|geschaetzt|vermutlich|getenv`): Treffer sind nur `RE15_AFFEN_FUSS` (Mess-Log) und `R35_TAKT_SPUR` (Test-Spur).
  Kein Env-Schalter dient als Abschluss.
- **Zeichenstand-Annahme von (11)/(12):** `re15_affen_zeichen_merk` merkt Lage/Yaw am ANFANG des eigenen Gorilla-Ticks. Das ist nur
  dann der Stand des Zeichners, wenn nichts den Gorilla zwischen Zeichner und eigenem Tick bewegt. Geprueft:
  - `re15_enemy_body_push_tail` ruft `re15_body_push(z2, ..., e, ...)`, und damit bewegt sich nur der tickende Gorilla e selbst.
  - `re15_enemy_selfpush_from_player` ebenso; der Spielerschub bewegt den Spieler.
  - Die Annahme haelt.
- **Erklaert der Fix den Befund?**
  - N2: ja. Die Vorbedingung steht im Protokoll: In Abnahme 3 hatten alle 30 Sub-3->5-Wechsel ein Entscheid-Bild; jetzt hat keiner
    eines. Slot 2 lag bei +3...+9 und liegt jetzt bildgleich.
  - N1: **nein** (Mangel P2). Die Begruendung "Startversatz verkleinert" widerspricht den Riegel-Zahlen.

## 4. Vertrags-/Pfad-Gate, Tests
- `git diff 154a73c1 HEAD --name-only` (der eigene Zweig, 23 Dateien):
  - keine Pfade unter `release/`, `platform/android/` oder `shared_assets/PSX/`;
  - keine Edits an `tests/unit/CMakeLists.txt` oder `tests/integration/CMakeLists.txt`.
  - `git log 154a73c1..HEAD -- release re15_port/platform/android re15_port/shared_assets <beide CMakeLists>` ist leer.
  - `git diff master --name-only` zeigt zusaetzlich release/- und r35_*-Dateien anderer Spuren. Das sind nur die Commits, die master
    (87cc8575 = v0.8.22) seit der Verzweigung bekommen hat.
- **Hakengroesse** (`git diff -U0 154a73c1 HEAD`): Kein Hunk in einer gemeinsamen Datei aendert mehr als 3 Zeilen. Der groesste hat
  3 geaenderte Zeilen (je 3 Zeilen `-`/`+`).
  - enemy_ai_common.c hat jetzt 61 Haken, davon 19 aus Nachbesserung 4. Alle liegen in den Gorilla-Funktionen und sind mit
    `Runde 35 Spur J` markiert, sofern sie nicht nur ` ab = 1;` an eine bestehende, belegte Zeile anhaengen.
  - Das haelt Vertrag 1.4 (Hinweis H4).
- **Zuteilung:** Bank-9-Bit 82, die Nachrichten-IDs 20..23 und Ereignis 25 sind nicht belegt; es gibt keine neuen Assets. Bei den
  Nachrichten-IDs ist das ein Rueckgriff auf das Raumskript, wie es Vertrag 1.1 bevorzugt.
- **Tests messen je Punkt:**

  | Punkt | Riegel |
  |---|---|
  | 1 | `band`, `ada` |
  | 2 | `wagen` |
  | 3 | `teile` |
  | 4 | `flug`, `kdsonde`, `biss`, `frac`, `takt`, `griff`, `wand`, `szene` |
  | 5 | `brust`, `anker`, `griff` |
  | 6 | `sprung`, `schrot` |
  | NPC-Band | `npcband` |

  Alle 17 sind gruen. Neu in Nachbesserung 4:
  - `takt`: 14 Bisse, Slot 2 7/7. Die alte Soll-Liste (Zeile - 1) haette am alten Stand rot gemeldet.
  - `szene`: Einzelbisse; der Slot-2-Versatz muss gleich bleiben.
  - `griff`: T265 hat eine eigene Pruefung, dazu kommt die N1-Pruefung Lauf 0. Zur N1-Pruefung siehe P2.

## 5. Maengel (nummeriert, nachpruefbar)

- **P1 (Punkt 4, Nebenwirkung von Nachbesserung 4, nicht als solche offengelegt; Nutzer-Aussage zu weit): Gorilla 2 (Slot 3) beisst
  jetzt mit Zyklus 103 statt 104. Vor Nachbesserung 4 hatte er den Original-Zyklus. Das Dossier meldet "Die Gorillas beissen jetzt
  im Rhythmus des Originals".**
  - **Messung exe t1** (gleicher Tuerweg, keine Eingabe, Treffer relativ zur Freigabe; Zuordnung ueber Clip 0x12 Bild 13/14):
    - Slot 3 jetzt: 468, 571, 674, 777, 880, 983, 1086, Tod 1189 = Original **0, 0, 0, -1, -2, -3, -4, -5**.
    - Abnahme 3 (`jabn3/t1`): 470, 574, 678, 782, 886, 990, 1094, 1198 = Original +2, +3, dann **+4 konstant** (Zyklus 104 wie das
      Original ab dem 3. Biss).
  - **Messung Riegel `takt`:**
    - e2 jetzt 269, 372, 475, 578, 681, 784, 887 gegen das Original 269, 372, 476, 580, 684, 788, 892.
    - Vorher (Dossier Z. 1305): "e2 104 durchgehend (+2 .. +3)".
    - Folge im Gesamtbild: Der Port wechselt starr 53/50 Bilder. Im Original wandert die Phase zwischen den beiden Gorillas (53, 50,
      53, 50, 53, 51, 52, 52, 51, 53, 50, 54, 49). In laengeren Kaempfen waechst die Abweichung um 1 Bild je Zyklus weiter.
  - **Dossier:**
    - OFFEN N4-1 (Z. 1443) beschreibt den Zyklus 103, sagt aber nicht, dass Nachbesserung 4 ihn verursacht hat. Z. 1471 nennt
      N4-1 sogar den "geschrumpften" Rest von OFFEN 1/2.
    - Die Ergebnis-Tabelle (Z. 1486-1495) fuehrt die Nebenwirkung nicht.
    - "Fuer den Nutzer (Stand Nachbesserung 4)" (1), Z. 1476, sagt "Die Gorillas beissen jetzt im Rhythmus des Originals". Fuer
      Gorilla 2 ist das falsch, und Gorilla 2 hat seinen Original-Zyklus gerade erst verloren.
    - Das ist derselbe Typ wie N2 in Abnahme 3: Eine Nachbesserung verschiebt einen Takt gegen das Original, und das Dossier stellt
      es anders dar.
  - **Fehlt:**
    - (a) Aussage (1) auf Gorilla 1 einschraenken. Fuer Gorilla 2 offen sagen: "beisst jetzt je Zyklus 1 Bild frueher als das
      Original (vorher konstant 4 Bilder spaeter)".
    - (b) OFFEN N4-1 als "durch Nachbesserung 4 verursacht" kennzeichnen (Zyklus 104 -> 103; exe +4 konstant -> 0 ... -5) und in
      die Ergebnis-Tabelle aufnehmen.
    - (c) Entweder den Mechanismus fuer e2 (Treffer bei +0x95 = 14 statt 15, Fenster {12..15} @0x8012146c) nach dem in N4-1 genannten
      Messweg belegen und beheben, oder den Riegel `szene` so fassen, dass er den Slot-3-Zyklus misst. Heute laesst die Schranke
      "groesste Abweichung <= 6" eine Drift von -1 je Zyklus 14 Bisse lang durch.

- **P2 (N1-Behebung: die Begruendung widerspricht der eigenen Messung; ein Pin auf einen chaotischen Ausgang kommt zurueck): Der
  Startversatz ist durch Nachbesserung 4 GEWACHSEN, nicht kleiner geworden. Die Landung von Lauf 0 an der Original-Ruhelage ist ein
  anderer Ausgang derselben chaotischen Klemmen-Iteration.**
  - **Dossier:**
    - Ergebnis-Tabelle N1 (b), Z. 1494: "Startversatz durch (10)-(12) verkleinert".
    - Abschnitt N1, Z. 1400-1403: "der Startversatz kam aus dem Anlauf (...= N2-Ursachen). Mit (10)/(11)/(12) landet Lauf 0 an der
      Original-Ruhelage".
    - Z. 1471: "OFFEN 1/2 (Startversatz Lauf 0, e2-Kriechen) sind auf N4-1 geschrumpft (e1 beim Pin hoechstens 37 daneben ...)".
  - **Messung Riegel `griff` Lauf 0**, selbst gefahren am alten und am neuen Stand (`jabn3/griff_v.txt` gegen `jabn4/griff_v.txt`),
    Abstand zum Original:

    | Groesse | Nachbesserung 3 | Nachbesserung 4 |
    |---|---|---|
    | Leon beim Pin T254 | 8 | **14** (T255-T264: 13) |
    | e1 T254 | 24 (Yaw 2821) | **33** (Yaw 2813; Original 2823) |
    | e1 T254-T288 hoechstens | 28 | **37** |
    | e2 T254 | 30 (Yaw 21) | **43** (Yaw 41; Original 27) |
    | Anker | 37 ((-7539,-10347)) | **40** ((-7536,-10354)) |
    | T265 | 64 | **1238** |
    | Wurf-Bahn T265-T290 Mittel | 34 | **74** |
    | T290 = Start der Klemmen-Iteration | 166 | **183** |
    | Freigabe T378 neben Original (-4759,-10633) | ~1530 | ~2020 (bei (-3018,-11651)) |

    - Die Ruhelage (-3018,-11651) ist genau der Punkt, den schon die alten Dossier-Zahlen von Nachbesserung 2 aus einem anderen
      T290-Stand lieferten (Riegel-Zeile "die Dossier-Zahlen von Nachbesserung 2 ... Ruhe (-3018,-11651)").
    - Er ist also ein Ausgang, den die Iteration aus verschiedenen Starts erreicht, und keine Folge eines kleineren Versatzes.
  - **Pin:**
    - Nachbesserung 3 hat begruendet, warum die Pruefung "Ruhelage <= 60" in Lauf 0 NICHT zurueckkommt (Z. 1217-1219: "dort
      entscheidet der Anker-Versatz 39 aus dem Anlauf ueber die chaotische Klemmen-Iteration"; Riegel: 24/24 Starts 1-2 Einheiten
      daneben enden > 100 daneben).
    - Nachbesserung 4 fuehrt genau diese Pruefung wieder ein (`test_r35_affen.c` N1-Pruefung: Ruhelage <= 60, kein Biss bis T495,
      e1/e2 +-250), obwohl der Anker-Versatz jetzt 40 statt 37 betraegt.
    - Ausserdem ist die alte Schranke "T265-T267 <= 200" (am Stand Nachbesserung 3: 64) fuer T265 weggefallen. Sie ist durch eine
      Pruefung ersetzt, die die Klemme nur auf die eigenen aufgezeichneten Eingaben anwendet.
    - Der Pin ist heute gruen, misst aber keinen Mechanismus. Jede fremde Aenderung am Anlauf kann ihn ohne Bezug zum Gorilla rot
      oder gruen machen.
  - Die Nutzer-Aussage selbst (Z. 1481-1484) ist bedingt formuliert und so haltbar. Der Mangel liegt in der Kausal-Behauptung und
    im Riegel.
  - **Fehlt:**
    - (a) Z. 1494, Z. 1400-1403 und Z. 1471 an die Messung anpassen: Der Startversatz ist gewachsen. Die Landung ist ein
      Iterationsausgang und keine Folge von (10)-(12).
    - (b) Die N1-Pruefung in Lauf 0 entweder entfernen (wie in Nachbesserung 3 begruendet) oder die Empfindlichkeit dieses Ausgangs
      messen, etwa wie in M1 (c) mit 24 Starts 1-2 Einheiten neben dem Lauf-0-T290, und das Ergebnis dazuschreiben.
    - (c) Den gewachsenen Startversatz (Leon 8 -> 14, e1 24 -> 33, e2 30 -> 43, T290 166 -> 183) unter OFFEN fuehren, mit Ursache oder
      Messweg. Er steckt in derselben e2-Lage, die N4-1 als Ursache nennt.

### Hinweise (keine Maengel)
- **H1:** OFFEN N4-1 bis N4-5 und die Reste 5/6 aus Nachbesserung 3 sind mit Adressen und Messwegen gefuehrt. Ausnahme ist die in
  P1/P2 genannte Einordnung.
- **H2:** re15_affen.h (10) nennt die Sprungmarke `ab_wechsel`; im Code heisst sie `ab_b` (enemy_ai_common.c:8894). Die Doku sollte
  der Marke im Code folgen.
- **H3:** Die Auslegung von Punkt 6 ist dieselbe wie in Abnahme 0 bis 3: Der Selektor-Fernsprung (Sub 4 -> 7) kommt ohne Treffer.
  Das ist Original-Verhalten; der Wortlaut zielt auf den Vergeltungssprung.
- **H4:** Die einzelnen Haken in enemy_ai_common.c halten die 1-5-Zeilen-Regel. Die Zahl der Haken (61) macht das Zusammenfuehren
  mit anderen Gegner-Aenderungen in dieser Datei aufwendig. Der Orchestrator sollte die Reihenfolge der Zusammenfuehrung beachten.
- **H5:** Der Lauf w7 zeigt nebenbei den Weg nach dem Sieg: 11C0 sub03 -> sub04 -> ROOM11B0 (sub06/07/08 mit Ada) -> Tuer -> ROOM1260.

## 6. Ergebnis

| Punkt | Urteil |
|---|---|
| 1 Ada versteckt sich / kommt nach dem Sieg zurueck | erfuellt (Tuerweg, echte Kills, Bilder b2/w7b) |
| 2 Monkey kommt an der korrekten Position aus dem Auto | erfuellt (F816 (-3617,-17798) = Original, F760-F1080 bitgleich zu Abnahme 3) |
| 3 komisch beweglicher Teil am Oberkoerper | erfuellt (Ruhe bitgleich, Bewegung Zoom o7, kein Part-Code geaendert) |
| 4 KI zielstrebiger/aggressiver wie im Original | erfuellt (Tod 39,6 s gegen 39,8 s, Slot 2 bildgleich, kein Entscheid-Bild, Griff +83/+16/+26) |
| 5 Brust-schlagen-Animation | erfuellt (Clip 3 nach jedem der drei Griffe, Wrap 2 Einheiten, groesster Schritt 86) |
| 6 Sprung erst nach 3 Treffern | erfuellt (Spur 0 und 1 in der exe, Spur 2 im Riegel) |

| Gate | Stand |
|---|---|
| Bau | gruen |
| Suite | 495/495, selbst gefahren |
| Riegel | 17/17 |
| @0x-Gate | gehalten: 6 Stichproben-Gruppen bestaetigt, 0 Guess-Tells |
| Pfad-Gate | gehalten |
| Dossier-/Riegel-Gate | **NICHT gehalten** (P1, P2) |

**Abnahme 4: NICHT BESTANDEN.**

Messlaeufe, Bilder und Auswertungen liegen in `scratchpad/jabn4/`:
- Skripte: run.sh, fire.txt, ana.py, takt.py, p6.py, sheet.py
- Laeufe: t1, b2, o7, w7, w7b, w3, w8
- Bilder: t1_wagen.png, t1_kampf.png, b2_ada.png, o7_griff_brust.png, o7_brust_zoom.png, w7_ada_zurueck.png
- Auswertungen und Logs: o7_ana.txt, griff_v.txt, szene_v.txt, takt_v.txt, `../jabn4_suite_all.log`
