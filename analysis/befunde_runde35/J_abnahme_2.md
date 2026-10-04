# Runde 35 — Spur J "affen": UNABHAENGIGE ABNAHME 2

Baum `.claude/worktrees/r35_affen`, Zweig `r35/affen`, gepruefter Stand HEAD b3755c41 (Merge-Basis master 154a73c1),
Abnahme 2026-10-04 nach Nachbesserung 2 (Maengel A1-A3 aus J_abnahme_1.md). Massstab: Wortlaut AUFTRAG.md, Regeln
VERTRAG.md / CLAUDE.md. Alle Messlaeufe selbst gefahren (Scratch `scratchpad/jabn2/`, exe-Kopie
`re15_port/build/platform/pc/re15_pc_jabn2.exe`, md5 da563840... = re15_pc.exe), nur eigene PIDs.

**Ergebnis: NICHT BESTANDEN** — alle sechs Nutzer-Punkte sind am gebauten Stand erfuellt (Punkt 4 jetzt im
Original-Szenario auf 1-4 Bilder je Biss genau). Es bleiben zwei Maengel am Dossier bzw. Vertrag: eine Messung im Dossier,
die HEAD nicht mehr liefert (zwei rot gewordene Pruefungen still entfernt), und eine selbst gefundene Abweichung, die
weder geklaert noch unter OFFEN steht (M1, M2). Code-Fixes braucht es dafuer nicht.

## 0. Bau und Suite
- `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work
  to do.` / `=== LOCAL-BUILD-OK (build)` (Baum sauber, exe 00:41 = letzter Code-Commit-Stand).
- Selbst gefahren: `bash re15_port/tools/local_build.sh all` -> `test OK — 495/495 bestanden` /
  **`=== LOCAL-BUILD-OK (all) — Tests 495/495`** (1131,6 s; alle fuenf Fenster-Haken gruen; Log `jabn2/suite_all.log`).
  Deckt sich mit dem Dossier (Zeile 1034, 478 Basis + 17 Riegel).
- Riegel dieser Spur im Suite-Lauf: `unit_r35_affen_{teile,band,ada,sprung,flug,wagen,brust,kdsonde,biss,frac,anker,
  takt,griff,npcband,schrot,wand,szene}` + `unit_member`, `unit_maggot_ai`, `unit_plc_back_yaw_1090`: alle Passed.
  `griff`, `szene`, `wand`, `takt` zusaetzlich einzeln mit `-V` gefahren (Ausgabe `jabn2/griff_v.txt`).

## 1. Echter Weg
Alle Raum-Messungen ueber die TUER von ROOM11B0 (wie Abnahme 0/1): `RE15_SET_FLAG=4:243,3:130 RE15_DEBUG_JUMP=11B0@240
RE15_PLAYER_POS=-25500,-28200,1024 RE15_PRESS=square@300,...,900` + `RE15_STATE_LOG RE15_EVT_TRACE RE15_ENEMY_DBG`.
debug.log in jedem Lauf: `[aot] DOOR FIRE slot=1 ... spawn=(-25279,0,17268)` -> `[room] PC loaded room11c0.rdt` ->
`[evt] F6 room=11c0 Evt_exec sub=2` -> `[evt] F1088 room=11c0 Evt_exec sub=7`.
Laeufe: t2 (ohne Eingabe), b1 (ohne Eingabe, Bilder 760-1300), b2 (`RE15_FORCE_CUT=4`, Bilder 1075-1320), b3/w7 (Item 7
SUPER REDHAWK), w3 (Item 3 BROWNING), w8 (Item 8 M870), o6 (`RE15_INPUT_SCRIPT=W34.5,U2.5,W1`, Leon im Freien, Bilder
1180-2100/4); dazu j10 (`RE15_DEBUG_JUMP=11C0@240`, keine Eingabe = Lage der Original-Aufnahme r3).
Feuerskript: `RE15_INPUT_SCRIPT=W40,(MA0.1,M0.6)x150 BASIS=spiel START=60`.
Messfalle (kein Mangel der Spur): `RE15_FRAMEDUMP` zaehlt `g_engine.frame_count` raumuebergreifend — ohne
`RE15_EXIT_AT=<n>#11C0` ueberschreiben Folgeraeume (1240/11B0/1260) die Bilder (t2/w7 verworfen, b1-b3 neu gefahren).

## 2. Nutzer-Punkte — Messprotokoll und Urteil

### Punkt 1 — "nach der Ada Cutscene verschwindet Ada nicht ... muss dann wieder raus kommen, wenn die Monkeys besiegt sind" — **erfuellt**
- Verstecken (t2 state.log, Slot 1 Typ 0x42): `F1088 Ada st=4/5/2 @(-8965,-14347)` (sub07 Plc_dest), `F1100 (-10873,-12883)`,
  `F1140 (-17233,-7994)`, **`F1145 st=4/6/0 @(-18025,-7379)`** (Original r3 t=47.02: (-18000,-7403)), danach bis F2000 dort.
  Die globale dz-Rundung (actor_locomotion.c) hat Adas Gang nicht verschoben (gleiche Ankunft wie Abnahme 1).
- Bild `jabn2/b2_ada.png` (Cut 4, F1075-F1300): F1105-F1135 laeuft Ada zu den Wagen, **ab F1150 nicht mehr im Bild**.
- Zurueckkommen (b3/w7, Item 7, beide Gorillas WIRKLICH getoetet): w7 `Slot 2 F1449 st=3 hp=-20`, `Slot 3 F1974 st=3 hp=-20`;
  debug.log `[evt] F2016 room=11c0 Evt_exec sub=3`, Ada `F2304 st=4/4/2` -> `F2330 st=4/6/0 @(-16264,-8159)`,
  `[evt] F2608 Evt_exec sub=4` -> `PC loaded room11b0.rdt`. Bild `jabn2/b3_ada_zurueck.png`: F2280 Ada steht neben Leon,
  "Ada: Okay, it's over now." / "...There are some outrageous monsters out there." / "Leon: Yeah... Anyway, let's get out
  of here." Ebenso w8 (M870): `F2373 Evt_exec sub=3`, `F2961 sub=4`.

### Punkt 2 — "kommt noch nicht an der Korrekten Position aus dem Auto" — **erfuellt**
- b1 state.log: beide Gorillas ab F1 `st=1/0/0 g=0x30 hp=180`; **`F816 S2 st=1/0/1 g=0x10 mo=22 @(-3617,-17798)`**
  (Original r3 t=36.41: (-3617,0,-17798), g=0x10, Clip 22).
- Bild `jabn2/b1_wagen.png` (F780-F890): Gorilla in der offenen Heckklappe, steigt vor dem Pfeiler aus. Pixelvergleich der
  Port-Bilder F800/810/820/840/870 gegen die Abnahme-1-Bilder (`jabn1/n2b/fd`, dort gegen Original s020-s022 geprueft,
  `jabn1/vgl_wagen.png`): **Differenz 0** in allen fuenf Bildern.

### Punkt 3 — "komisch beweglicher Teil am Oberkoerper" — **erfuellt**
- Ruhend: b1 F840/F870 bitgleich zu Abnahme 1 (dort Zoom-Vergleich mit dem Original, `jabn1/vgl_brust_zoom.png`).
- In Bewegung: `jabn2/o6_brust_zoom.png` (F1252-F1308, aufrechter Gorilla, Nachbar gebeugt) und `o6_griff1/2.png`:
  geschlossenes Fell am Rumpf, kein mitschwingendes Fremdteil. Riegel `teile` gruen (Part 18 am Rumpf @0x80117200-3c,
  in Abnahme 0 selbst disassembliert; in Nachbesserung 2 unveraendert).

### Punkt 4 — "im Original ist die KI zielstrebiger und aggresiver ... Da stimmt noch irgendwas bei der Übernahme nicht" — **erfuellt**
Original-Referenz SELBST aus den GDB-Rohspuren dekodiert (`jnb2/g_frei.txt`: aca58 4->1 bei **VSync 9037**;
`jnb1/g_orig.txt`: Spieler +0x9a je Bild): Treffer relativ zur Freigabe **364** (Heavy), 468, 521, 571, 624, 674, 727, 778,
830, 882, 933, 986, 1036, 1090, 1139, Tod **+1194 Bilder = 39,8 s**; Abstaende 53,50,53,50,53,51,52,52,51,53,50,54,49,55.
- **A1, gleiche Eingabe wie das Original** (j10, `RE15_DEBUG_JUMP=11C0@240`, keine Eingabe): Freigabe `F1071 pst 4->1 bei
  (-7138,-12372)` (= Original-Endlage des sub02-Gangs; Abnahme 1: (-7153,-12351)); Treffer **+365**, 470, 523, 574, 626, 678,
  729, 782, 832, 886, 935, 990, 1038, 1094, 1141, Tod **F2269 = +1198 = 39,9 s**. Jeder Treffer **+1..+4 Bilder** neben dem
  Original; Gleichtakt (Abstaende 53,51,52,52,51,53,50,54,49,55,48,56,47,57, Mittel 52,0). Abnahme 1: 36er-Takt, Tod 33,3 s.
- **A1, echter Tuerweg** (t2, keine Eingabe): Freigabe F1088 bei (-7148,-12363), dieselben Treffer-Offsets +365 ... +1141,
  Tod **F2286 = +1198 = 39,9 s** (Abnahme 1 n2: +991).
- Riegel `szene` (-V): Gang 207/207 bitgleich, Heavy +365, Takt Mittel 51,6, Tod +1198 — gruen. Riegel `takt`: Gleichtakt
  8 Bisse max. 3 Bilder, Wechseltakt nach Desync max. 4 Bilder — gruen.
- **A2, Wurf ueber den echten Tuerweg, Leon im Freien** (o6): Rear-up S3 `F1197 st=1/15/0`, Pin F1202 (Leon mo 1), Clip 0x10
  **F1285 = Pin+83**, Clip 0xb **F1301 = +16**, Leon wieder Leerlauf **F1327 = +26**, HP 88 -> 88. Original (Riegel-Tabelle
  `s_wurf_orig`, aus g_wer): +0x94 = 16 ab T337 = Pin+83, = 11 ab T353 (+16), +0x94 = 3 ab T379 (+26) — **bildgleich**.
  Rueck-Variante Griff 2: Pin F1572, Clip 0x10 F1644 (+72 = 83-11). Griff 3: Bilder `jabn2/o6_griff3.png` (F1996-F2096: Leon
  liegt am Wagen, kniet F2048-F2064 hoch, steht ab F2068). Der zweite Gorilla kriecht waehrend der Griffe weiter (o6 S2 sub 3
  Clip 5, F1203-F1290 bewegt).
- Riegel `griff` (-V): erste Platzierung T265 (Original T265), Bahn T268-T290 Mittel 19 / max 177 (T290), P3/P4 16 Bilder
  Clip 0x10 PL00 vorwaerts, P5/P6 25 Bilder Clip 0xb PL00 rueckwaerts, Freigabe T378 (Original T378), T291-T377 nur
  Wandklemme (87/87). Riegel `wand`: re15_collision_constrain = FUN_8003b0a4 in 168/168 Original-Bildern, Schub-Kette
  T265-T267 bitgleich.
- Rest (vom Bau-Agenten unter OFFEN, mit Adressen): Spieler-Schub vor statt nach der Gegner-KI (@0x80031cbc), B im
  Folgebild statt im selben Bild nach Sub-Wechsel (@0x80117358/@0x80117378), Ersatz-RNG an Path-B-Muenze @0x801180a8,
  B[7]-Anlauf @0x8011898c, B[1], B[4] im A[3]->4-Wechselbild. Gemessene Folge: die +1..+4 Bilder je Biss oben.
- Urteil: Das Original-Szenario stimmt jetzt auf 1-4 Bilder je Biss und 0,13 s Todeszeit; der Wurf laeuft in der exe
  phasengleich. Die beiden Abnahme-1-Maengel A1/A2 sind in ihrem Kern behoben. Zu den Dossier-Aussagen siehe M1/M2.

### Punkt 5 — "Brust schlagen Animation ..., die sie im Original manchmal ausführen" — **erfuellt**
- o6 (Tuerweg, Leon im Freien): nach jedem der drei Griffe **Clip 3**: `F1250 S3 st=1/15/5 mo=3`, `F1277 1/2/1 mo=3`,
  `F1609 1/15/5 mo=3`, `F1636 1/2/1`, `F1998 1/15/5 mo=3`, `F2025 1/2/1`.
- Bild `jabn2/o6_brust_zoom.png` (F1252-F1308, 4er-Schritt, 3x Zoom): der Gorilla steht aufrecht, Arme abwechselnd an der
  Brust (F1256, F1276/F1280, F1296/F1300) und aussen (F1264/F1268, F1284/F1288), danach wieder auf allen vieren (F1304/F1308).

### Punkt 6 — "erst springen, wenn sie 3x getroffen wurden, nicht nach jeden Schuss" — **erfuellt**
Auswertung `jabn2/p6.py` (Flinch-Eintritt st 1->2, Exit-Sub beim Austritt, sub-7-Eintritte), Segment 11C0:
| Lauf | Waffe | Slot 2 Exit-Subs | Slot 3 Exit-Subs | Spruenge nach Treffer |
|---|---|---|---|---|
| w3 | 3 BROWNING HP (Spur 0) | 3,3,7,3,3,7,3,3,7,3,3,7,3,3,7 (15 Treffer) | 3 (1) | 5 + 0 (je der 3.) |
| w7 | 7 SUPER REDHAWK (Spur 1) | 3,3,7 | 3,3,7 | 1 + 1 |
| w8 | 8 REMINGTON M870 (Spur 1) | 3,3,7,3,3 | 3,3,7,3 | 1 + 1 |
- Zaehler setzt nach dem Sprung zurueck (w3: alle drei Treffer ein Sprung). Spur 2 (Granate/Werfer) im Riegel `sprung`.
- Nicht-Treffer-Spruenge des Selektors aus sub 4 (Path B @0x80118028-8100) bleiben: w3 S3 F1380, w7 S3 F1370, w8 S3 F1312
  (Hinweis H3, wie Abnahme 1 H1).

## 3. RE-Gate (@0x-Belege, Stichproben-Disasm, Guess-Tells)
Code-Diff `git diff 154a73c1 HEAD -- re15_port` vollstaendig gelesen (Nachbesserung 2: `git diff f604a1dc HEAD`, 11 Dateien).
Jede neue verhaltensrelevante Konstante traegt @0x...; Ausnahme kosmetisch: `b4_fest` (enemy_ai_common.c:9052) wiederholt die
Path-B-Bedingung mit `dist >= 6001` ohne eigene Adresse, dieselbe Bedingung ist 9 Zeilen hoeher mit @0x8011806c belegt (H4).
Stichproben, SELBST disassembliert (`.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`, richtige Binaerdatei):
1. **PSX.EXE FUN_800245d8 + RotMatrixY (dz-Rundung):** Vektor (+0x8c,0,0) @0x800245f0-80024600, Identitaet 0x80072d4c
   kopiert @0x80024604-48, `jal 0x800659d0` @0x80024660 (a0 = Yaw +0x6a + a0), Matrix per `ctc2` @0x80024674-90, `lwc2`
   @0x8002469c-a0, **`.word 0x4a486012` @0x800246ac = cop2 0x0486012 = MVMVA sf=1, mx=RT, v=V0, cv=none**, `mfc2`
   @0x800246bc-c4, x/z += IR1/IR3 @0x800246d4-f4. Sprungziel 0x800659d0: t1 = -sin (beide Vorzeichen-Zweige
   @0x800659d8-a30), t0 = cos -> R31 = -sin. dz = (-sin*v) SAR 12 = Port-Zeile `a->z += ((-s * speed) >> 12)`. **Bestaetigt.**
2. **PSX.EXE FUN_8001ab9c (Kegeltest):** `subu v0,v0,v1` (Peilung - Yaw) @0x8001abe4, `addu v0,s0,v0` (+k) @0x8001abe8,
   `andi 0xfff` @0x8001abec, `sll v0,a0,1` (2k) @0x8001abf8, **`slt v0,v1,v0` @0x8001abfc** -> im Kegel <=> -k <= d < k.
   Aufrufe `ori a2,zero,0x15e` @0x80030dbc / @0x80030b84, `ori a2,zero,0x60` @0x800313d4. Port: `delta = target - cur`
   (yaw_delta, actor_locomotion.c:147-152), Test `delta >= -k && delta < k`. **Bestaetigt.**
3. **PSX.EXE FUN_8001af20 (RNG):** `lhu t1,0(v0)` @0x8001af28 (t1 nie benutzt), `srl v1,a0,7` / `andi 0xff` @0x8001af30-34,
   `addu` / `andi 0xff` @0x8001af38-3c, `sll v1,v1,8` / `or a0,a0,v1` @0x8001af40-44, `sw a0` @0x8001af48, `andi v0,a0,0xff`
   @0x8001af4c = re15_affen_rng_a0. a0-Quellen: A[0] `bne +0x1dc` @0x80117498 / `lw a0,g_entity` @0x801174d0-d4; A[3]
   `ori a0,zero,0xbb8` im Delay-Slot @0x80117a60, sonst `jal 0x8001a804` @0x80117a6c; B[7] `lw a0,g_entity` @0x80118a24 vor
   `jal 0x8001af20` @0x80118a3c; B[4] (0x80118110) setzt kein a0, A[4] laedt `addiu a0,a0,52` (Entity+0x34) vor `jal
   0x8003b93c` @0x80117ef8/@0x80117f8c; Roh-Scan STAGE1.BIN: einziger Schreiber auf +0x1e2 im Gorilla-Bereich ist der INIT
   `sb` @0x8011709c (=4) -> der Zonen-Zweig laeuft immer. **Bestaetigt.**
4. **STAGE1 Griff-Paar / Wurf-Handler:** `ori v0,v0,0x1000` / `sw` @0x8011ac34-38 (g_entity), Spieler @0x8011ac4c/54, Phase 4
   0x8011ad10 (einbildig: +0x6 = 5 @0x8011ad44) loescht `addiu v1,zero,-4097` / `and` / `sw` @0x8011ad8c-94; FUN_8002aec4
   `and v0,a0,v1` / `andi 0x1000` @0x8002af14-18. P0 `beq v1,zero` @0x8011c208, Rueck-Variante +0x95 = 0xc @0x8011c214-1c,
   `j 0x8011c3b8` @0x8011c220; P1 `ori v0,zero,0xb` / `bne` @0x8011c24c-50; P2 `sltiu v0,v0,0x25` @0x8011c278, `jal
   0x8001ad68` @0x8011c294, anim_set @0x8011c2b0, Spieler-Bit-Clear @0x8011c2c8-dc; P3 a2=0 @0x8011c318, P5 `ori a2,zero,0x1`
   @0x8011c348, `lw a0,-13608` / `lw a1,-13376` @0x8011c350/58; P7 `ori v0,zero,0x1` / `sw aca58` @0x8011c384-8c. **Bestaetigt.**
- Guess-Tell-Suche in den hinzugefuegten Zeilen (`deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|approx|
  ungefaehr|geschaetzt|vermutlich|getenv`): nur `RE15_AFFEN_FUSS` (Mess-Log, kein Spielverhalten). 0 Guess-Tells.
- Fix erklaert Befund: A1 — Vorbedingung im Protokoll (Abnahme-1-Lauf n10: Endlage (-7153,-12351), 36er-Takt, Tod 33,3 s;
  Dossier: dz -57 statt -58 je Schritt @0x800246ac, B[4]-Slew 64-95 statt +73 je Bild); nachher j10/t2 Gleichtakt, Tod 39,9 s.
  A2 — Vorbedingung (Riegel vorher: ab T289 auf (-3965,-12507) festgeklemmt, frei T339) im Protokoll; nachher Platzierung ->
  Schub -> FUN_8003b0a4, Freigabe T378, exe +83/+16/+26.

## 4. Vertrags-/Pfad-Gate, Tests
- `git diff 154a73c1 HEAD --name-only` (eigener Zweig) und `git diff master --name-only`: keine Pfade unter `release/`,
  `platform/android/`, `shared_assets/PSX/`; keine Edits an `tests/unit/CMakeLists.txt` / `tests/integration/CMakeLists.txt`.
- Hakengroesse (`git diff -U0 154a73c1 HEAD`, je Hunk +/-): enemy_ai_common.c 37 Hunks max. 5; game_step_common.c 7 Hunks
  max. 3; main.c max. 2; scd_vm.c 1; actor_locomotion.c / re15_damage.c / anim_select_common.c / actor_common.c /
  emd_common.c je max. 2. Haelt Vertrag 1.4 (1-5 Zeilen).
- Bank-9-Bit 82, Nachrichten-IDs 20..23, Ereignis 25: nicht belegt (`git diff` ohne game_flag_set/msg_install_text/
  scd_event_fire); keine Assets.
- Tests messend je Punkt: P1 `band`/`ada`; P2 `wagen`; P3 `teile`; P4 `flug`/`kdsonde`/`biss`/`frac`/`takt`/`griff`/`wand`/
  `szene`; P5 `brust`/`anker`/`griff`; P6 `sprung`/`schrot`; NPC-Band `npcband`. 17/17 gruen (Suite).
- Geaenderte Fremd-Pins: `unit_plc_back_yaw_1090` neu gepinnt (Endyaw 111 -> 133, (-77,-1997) -> (-77,-1999); "Mode 5 Endyaw
  == bearing" -> Drift <= 0x20) — Begruendung im Testkommentar mit @0x800246ac/@0x8001abfc (H2).

## 5. Maengel (nummeriert, nachpruefbar)
- **M1 (Dossier/Riegel `griff`): Eine Messung im Dossier liefert HEAD nicht; die zwei Pruefungen dazu wurden still entfernt.**
  `J_affen.md:858-859` (A2 "Messung nachher (Riegel)") behauptet: "Freigabe T378 bei (-4580,-10518) (Original (-4759,-10633));
  0 HP; danach schiebt die Wandklemme Leon zur Ruhelage (-3018,-11651) (Original (-3009,-11643))". Am gebauten Stand druckt
  `ctest -R "^unit_r35_affen_griff$" -V` (`jabn2/griff_v.txt`): `frei T378 bei (-5433,-10693)` (677 vom Original) und keine
  Ruhelage: Leon wird weiter geschoben und **T396 (76 -> 70) und T450 (70 -> 64) gebissen**, `Ende (-5893,-10250) hp 64`
  (T412: (-5840,-10727) statt (-3009,-11643)). Die beiden PRUEFs, die genau das gegen das Original maßen
  (`dist(Freigabe, (-4759,-10633)) <= 400` und `Ruhelage <= 60 von (-3009,-11643) bis T412`), hat Commit 1ac1ea84 entfernt
  ("chaotische Ruhelage nicht mehr gepinnt"). Ersetzt wurden sie durch eine reine Strukturpruefung ("nur die Wandklemme
  bewegt"). Weder der Abschnitt "Tests (Stand Nachbesserung 2)" (Z. 966-975) noch die OFFEN-Liste sagt, dass sie rot wurden
  und entfernt sind. "Chaotisch" ist behauptet, nicht gemessen.
  Fehlt: (a) Z. 858-859 auf den HEAD-Stand korrigieren. (b) Im Dossier nennen, dass und warum die beiden Pruefungen fielen.
  (c) Die Chaos-Aussage messen. Weg 1: die Port-Klemme ab dem Original-Zustand T290 ((-5381,-10551), Bezug T289) iterieren
  und zeigen, dass sie die Original-Bahn T291-T377 bis zur Freigabe (-4759,-10633) reproduziert. Weg 2: die Herkunft des
  Startversatzes belegen. Messbar ist: Riegel-e1 beim Pin T254 (-5865,-14392) gegen Original-e1 (-5895,-14394) aus
  `jnb1/g_griff.txt`; daraus Platzierungsversatz 2-17 in T268-T288, 64 in T289, 177 in T290.
- **M2 (Vertrag 2.4, OFFEN unvollstaendig): Eine selbst gefundene KI-Abweichung ist weder geklaert noch gelistet.**
  `J_affen.md:860-863`: "Der Port-e2 steht T250-T264 still, das Original-e2 gleitet in derselben Zeit ~400 an Leons Koerper
  entlang -> gehoert zu A1 (Lage/Bewegung im Koerperkontakt), dort weiter." Der A1-Abschnitt (Z. 879-959) und OFFEN
  (Z. 977-1001) greifen das nicht mehr auf. Am HEAD unveraendert (`griff_v.txt`): Riegel-e2 T249-T264 fest auf
  (-8706..-8710,-12437..-12448).
  Original selbst dekodiert (`jnb1/g_griff.txt`, e2-Record): sub 3/1, +0x1dc zaehlt 15 (T240) -> 0 (T255), +0x9e konstant 21
  ab T255 (a0 = 0xbb8 bei Spieler +0x93 = 1, @0x80117a60). Ab T257 bewegt sich e2 nach (-9094,-12016) (T263), Yaw 27 -> 123,
  +0x1d4 2048 -> 2468. Folge im Riegel: T265-T267 liegt Leon 1241/1573/207 neben dem Original; diese Bilder nimmt die Pruefung
  ausdruecklich aus.
  In der exe (o6) kriecht der zweite Gorilla waehrend des Griffs weiter (S2 sub 3 Clip 5, F1203-F1290 bewegt). Das spricht
  fuer einen Startzustands-Effekt des Harness, ist aber nicht belegt.
  Fehlt: die Ursache (Harness-Startzustand gegen Original, z.B. +0x1dc/+0x9e/a0-Kette von e2 bei T250) belegen, oder den
  Punkt mit Adresse und Messweg unter OFFEN fuehren.

### Hinweise (keine Maengel)
- H1: OFFEN-Liste der Spur (Spieler-Schub-Reihenfolge @0x80031cbc; A/B im selben Bild @0x80117358/78 — gorilla-spezifisch;
  Ersatz-RNG an Path-B-Muenze @0x801180a8, B[7]-Anlauf @0x8011898c, B[1], B[4] im A[3]->4-Wechselbild): gemessene Folge
  +1..+4 Bilder je Biss, Heavy +1 — im Spiel nicht spuerbar, im Code aber noch nicht byte-true.
- H2: Die dz-Rundung (actor_locomotion.c) und der halboffene Kegel gelten fuer ALLE Plc_dest-Gaenge (Leon und NPCs, alle
  Raeume), die Rueckstoss-Rundung (re15_damage.c re15_player_knockback_delta) fuer alle Rueckstoesse. Belegt per Disasm
  (Stichproben 1/2), gegen das Original nachgemessen nur in 11C0 sub02 (207/207). `unit_plc_back_yaw_1090` ist auf neue
  Port-Werte umgepinnt, ohne Original-Messung in ROOM1090.
- H3 (Auslegung Punkt 6, wie Abnahme 1 H1): Der Selektor-Fernsprung (sub 4 -> 7) kommt ohne 3 Treffer (w7 S3 F1370 nach 0
  Treffern). Original-Verhalten; der Wortlaut "nicht nach jeden Schuss" zielt auf den Vergeltungssprung.
- H4: `b4_fest` (enemy_ai_common.c:9052) ohne eigenes @0x (Bedingung = Path-B-Gate @0x80118034-8011806c, Z. 9041-9043).
- H5: Gorilla-Schatten ohne Koerper am Boden sichtbar (o6 F1200-F1244, `o6_griff1.png`) — OFFEN aus Nachbesserung 1
  (Box[6]+100/+200 @0x801171d8-ec), kein Nutzer-Punkt dieser Runde.

## 6. Ergebnis
| Punkt | Urteil |
|---|---|
| 1 Ada versteckt sich / kommt nach dem Sieg zurueck | erfuellt (Tuerweg, echte Kills, Bilder b2/b3) |
| 2 Monkey kommt an der korrekten Position aus dem Auto | erfuellt (F816 (-3617,-17798) = Original, Bilder bitgleich zur Abnahme 1) |
| 3 komisch beweglicher Teil am Oberkoerper | erfuellt (Ruhe + Bewegung, Zoom) |
| 4 KI zielstrebiger/aggressiver wie im Original | erfuellt (j10/t2: Treffer +1..+4 Bilder, Tod 39,9 s gegen 39,8 s; Wurf phasengleich) |
| 5 Brust-schlagen-Animation | erfuellt (Clip 3 nach jedem Griff, Zoom-Bild) |
| 6 Sprung erst nach 3 Treffern | erfuellt (Spur 0 und 1 in der exe, Spur 2 im Riegel) |

Gates: Bau gruen; Suite 495/495 (selbst gefahren); Riegel 17/17; @0x-Gate gehalten (4 Stichproben-Gruppen bestaetigt,
0 Guess-Tells); Pfad-Gate gehalten. Dossier-/Vertrags-Gate NICHT gehalten (M1: Messbehauptung, die HEAD nicht liefert, und
entfernte Pruefungen nicht offengelegt; M2: gefundene Abweichung nicht unter OFFEN). **Abnahme 2: NICHT BESTANDEN**
(nur M1/M2; beide brauchen eine Messung und Dossier-Text, keinen Spiel-Code).
Messlaeufe/Bilder: `scratchpad/jabn2/` (j10, t2, b1-b3, w3, w7, w8, o6, *.png, takt.py, p6.py, griff_v.txt, suite_all.log).
