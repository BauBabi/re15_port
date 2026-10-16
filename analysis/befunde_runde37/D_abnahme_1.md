# Runde 37 — Spur D "itemband": UNABHAENGIGE ABNAHME 1 (2026-10-12)

Baum `.claude/worktrees/r37_itemband` (Zweig `r37/itemband`, HEAD d5504016, Merge-Basis master f1e019ca).
Gegenstand: Stand nach "Nachbesserung 1" (Abnahme 0 = D_abnahme_0.md, Maengel 1-4).
Wortlaut (AUFTRAG.md #8): "Items auf erhöhten Flächen lassen sich vom falschen Stockwerk aufnehmen, z.B. die Super
Redhawk in ROOM3050 ohne die Kiste. Das war schon in v0.8.23 so. << muss gefixed werden."
Vertrag (VERTRAG.md, Spur D): Items auf erhoehten Flaechen nur vom richtigen Band aufnehmbar (Band-Gate + Member 0x12),
alle betroffenen Items bleiben auf dem vorgesehenen Weg erreichbar.
Der in diesem Lauf relayte Nutzer-Nachtrag (Beretta M93R: jeder Schuss des Feuerstosses soll treffen) ist laut
AUFTRAG.md "Nachtrag 2026-10-12" und VERTRAG.md §0 der Spur G (r37_beretta) zugeteilt — nicht Gegenstand dieser Abnahme.

(Bericht wird fortlaufend geschrieben — Stand siehe Abschnitt 0.)

## 0. Stand der Abnahme (abgeschlossen)
- [x] Diff gelesen (git diff master...HEAD), Bau configure + build (3 Haken-Dateien per touch neu kompiliert) — Abschnitt 1
- [x] RE-Gate: Adressen/Bytes selbst gelesen — Abschnitt 2
- [x] exe-Messungen Lift A (Nachbesserung): Leon + Elza, beide Richtungen, ueber die ECHTE Tuer aus 2070/2071 — Abschnitt 3
- [x] Gegenprobe Stand 05117c03 (selbst gebaut) an meinen Lift-Stellungen + Riegel gegen 05117c03 rot — Abschnitte 3 G, 8
- [x] Tests der Spur selbst gefahren (unit_r37_itemband, integration_r37_itemband, integration_r37_itemband_lift) — Abschnitt 4
- [x] exe-Messungen Nutzer-Punkt ROOM3050/3051 am Stand d5504016 ueber echte Tueren — Abschnitt 5
- [x] Restrisiko Bewegung waehrend der Fahrt, Sperre nach Aot_reset, Lift B / 3050 aufwaerts — Abschnitt 6
- [x] Gates, gezielte Regression (583/583 Unit nach Einzel-Nachlauf, 18/18 Integration) — Abschnitte 7, 8
- [x] Urteil: #8 erfuellt, keine Maengel, bestanden — Abschnitte 10-13

## 1. Vorpruefung (Diff, Bau)
* `git log --oneline master..HEAD`: erster Stand (bis 05117c03), Abnahme 0 (79233591), Nachbesserung 1 (6dfdc818..d5504016,
  13 Commits). `git diff master...HEAD --name-only` (Drei-Punkte = Inhalt des Zweigs): Dossier/Belege der Spur D +
  `engine/src/actor_common.c` (+4/-1), `engine/src/aot_common.c` (+5), neu `engine/src/itemband_r37.c`,
  `include/re15_itemband_r37.h`, `tests/unit/test_r37_itemband.c`, `tests/unit/probes/r37_itemband.cmake`,
  `tests/integration/test_r37_itemband.cmake`, `tests/integration/test_r37_itemband_lift.cmake`. Die in `git diff master`
  (zwei Punkte) zusaetzlich "geloeschten" Orchestrator-/UEBERGABE-Dateien sind nur der weitergelaufene master (dort seit
  f1e019ca nur doc/orch-Commits) — kein Inhalt des Zweigs.
* Code der Nachbesserung (`git diff 05117c03 HEAD -- re15_port/engine re15_port/include`): aot_common.c +2 Zeilen im
  `default:`-Zweig (GENERIC-Feuer) `if (re15_itemband_r37_liftsperre(i, inside)) break;`, itemband_r37.c Tabelle
  `k_liftsperre` (4 Saetze: ROOM2030/2031 slot 5/4, Unterprogramm 2, Rechteck 5850/11350/1500/1500) + Funktion, .h Abschnitt (3).
  `inside` ist an dieser Stelle der Mitte-Test des Scans (aot_common.c: `abs(player_x - a->x) <= a->half_w && abs(player_z -
  a->z) <= a->half_h`, nur bei Band-Ungleichheit auf 0 gesetzt); `action_fired` ist schon 1 (Druck verbraucht).
  Die Sperre greift nur, wenn Raum, Slot, Typ GENERIC, Unterprogramm UND Rechteck des Satzes passen.
* Bau: `touch` auf aot_common.c/actor_common.c/itemband_r37.c (Inhalt unveraendert, `git status --porcelain` leer), dann
  `bash re15_port/tools/local_build.sh configure` + `build` -> `LOCAL-BUILD-OK (build)`, `[1/701] actor_common.c.obj`,
  `[2/701] itemband_r37.c.obj`, `[3/701] aot_common.c.obj`, re15_pc.exe 14:42 (HEAD-Code). Messkopie `re15_pc_r37d_abn1.exe`
  (md5 = re15_pc.exe).

## 2. RE-Gate (selbst gelesen)
| Zitat im Diff / Dossier | selbst gelesen (Werkzeug) | Urteil |
|---|---|---|
| RE1.5 FUN_80042bac @0x80042ea8 `andi 0x40` (Mitte) / @0x80042ef8 `andi 0x20` (voraus) | `re15_disasm.py dis 0x80042e98`: `80042ea0 lbu v1,1(s0)` / `80042ea8 andi v0,v1,0x40` / `80042eac beq v0,zero,0x80042ef4` / `80042eb8 addiu a0,s1,52` (Entity-Lage) / `80042ecc jal 0x80042b64`; `80042ef8 andi v0,v1,0x20` / `80042efc beq v0,zero,0x8004301c` / `80042f0c addiu a0,sp,24` (Punkt voraus) / `80042f20 jal 0x80042b64` | bestaetigt |
| Satz Lift A ROOM2030/2031 sub00 @0x01CE6 / @0x01D20 (Tabelle `k_liftsperre`) | `xxd` ROOM2030.RDT und ROOM2031.RDT (beide md5 c5bf3129..., `cmp` = info/Re1.5/PSX/STAGE2): `0x1CCE 21 03 de 00` (Ck (3,222)=0), `0x1CE6 2c 05 03 31 03 00 da 16 56 2c dc 05 dc 05 ff 00 18 02 00 00` (slot 5, sce 3, sat 0x31, floor 3, x 5850 z 11350 w 1500 d 1500, Unterprogramm 2), `0x1D20 2c 04 03 31 06 00 da 16 56 2c ...` (slot 4, floor 6, gleiches Rechteck), `0x1E62 34 01 d0 d5` (y -10800) `0x1E66 34 12 06 00` `0x1EA8 34 12 03 00` (Member 0x12) | bestaetigt |
| RE2 ROOM6030 sub00 @0x02B9C Plattformzone sat 0x41 -> (5,0x22) | `xxd` info/re2leon/PL0/RDT/ROOM6030.RDT: `2c 03 06 41 03 00 e8 b8 c0 c7 20 03 b0 04 05 00 22 00 01 00` (slot 3, sce 6, sat 0x41, floor 3, (-18200,-14400,800,1200), Bank 5 Bit 0x22 Wert 1); `0x2BB0 2c 17 05 31 03 00 40 bb ec c8 84 03 4c 04 ff 00 18 0a 00 00` (Bedienfeld sat 0x31 -> Unterprogramm 0x0a) | bestaetigt |
| RE2 ROOM6030 sub10 @0x02F70-7A `if (Ck(5,0x22)==0) Evt_end` | RDT-Kopf (eigenes Skript): Offset[17] = Exec-SCD 0x2A2C, Sub-Tabelle -> **sub10 beginnt bei 0x2F70**; Bytes `06 00 08 00 \| 21 05 22 00 \| 01 00 \| 08 00` = If(Laenge 8) Ck(5,0x22,0) -> Evt_end, EndIf; danach `0x2F7C 2b 00 09 00` (Message_on); `0x2FC6 2e 01 00` (Work_set Spieler), `0x3056 34 11 00 00` | bestaetigt |
| RE2 FLAG_CHG-Handler 0x800a73c4[6] = LAB_800519c0, Bank 5 | `re2_disasm.py table 0x800a73c4` -> [6] = 0x800519c0; `dis 0x800519c0`: `lhu v1,0(a0)` / `lhu a1,2(a0)` / `lhu a0,4(a0)` / `lw v1,30920(at)` (0x800a78c8[Bank]) / `srlv` + `or`/`nor` (Bit setzen/loeschen); `table 0x800a78c8` -> [5] = 0x800d4874 (Bit 0x22 -> Wort +4 = 0x800d4878) | bestaetigt |
| RE2 Bild-Wisch FUN_80052e20 @0x80052e48 | `re2_disasm.py dis 0x80052e20`: `80052e44 lui at,0x800d` / **`80052e48 sw zero,18552(at)` = 0x800d4878** (genau das Wort von (5,0x22)) | bestaetigt |
| RE2 Scan FUN_80051088 Band/Mitte/voraus | `80051180 lbu v1,2(s0)` / `80051188 andi v0,v1,0x80` / `80051194 lbu v0,262(s1)` / `8005119c bne v0,v1,...`; `80051344 andi v0,v1,0x40` (a0 = s1+56); `80051390 andi v0,v1,0x20` | bestaetigt |
| RE2 ROOM5060 sub00 @0x00D7E sat 0x51 | `xxd`: `2c 04 05 51 80 00 ...`; `0x0F40 34 11 08 00`, `0x0F54 34 11 0b 00` | bestaetigt |
| Kabinenzellen (Commit d5504016: r0[49] @0x00AE4, r0[33] @0x00A24) | eigenes Skript, ROOM2030.RDT SCA @0x880 (5 x 76 Zellen): `r0[33] @0x00A24 x 5400..7750 z 12800..13300 band 3`, `r0[34] @0x00A30 x 5400..5900 z 11500..13300 band 3`, `r0[35] @0x00A3C x 7250..7750 band 3`, `r0[49] @0x00AE4 x 5000..8000 z 10650..11150 band 6`, `r0[55] @0x00B2C x 5400..5900 z 10650..13050 band 6`, `r0[56] @0x00B38 x 7250..8000 band 6` | bestaetigt |
| (erster Stand, erneut gelesen) Band-Gate @0x80042cac-ccc fuer jeden Satz | `dis 0x80042cac`: `lbu v0,2(s0)` / `80042cb4 andi v0,v0,0x80` / `80042cb8 bne v0,zero,0x80042cd4` / `80042cc0 lbu v1,130(s1)` / `80042cc4 lbu v0,2(s0)` / `80042ccc bne v1,v0,0x8004301c`; Aufrufer `80031fdc addiu a0,s0,748` (Spieler-Entity) / `80031fe4 jal 0x80042bac` / `80031fe8 ori a2,zero,0x10` | bestaetigt |
| (erster Stand, erneut gelesen) Member 0x12 -> @0x80041228 | `table 0x80010c8c` [18] = 0x80041224; `80041224 j 0x80041230` / Delay-Slot **`80041228 sb a2,130(a0)`** (+0x82) | bestaetigt |
| RE1.5 50D0 ~ RE2 6030 (Tuer slot 0 gleich) | RE1.5 50D0 `0x11DA 3b 00 02 31 03 00 60 9b d5 f4 80 0c 08 07` / RE2 6030 `0x29CA 3b 00 01 31 03 00 60 9b d5 f4 80 0c 08 07`; RE1.5 50D0 `0x12FA 2c 05 03 31 03 00 e8 b8 c0 c7 b0 04 b0 04 ...` (Liftausloeser an derselben Ecke wie die RE2-Zone) | bestaetigt |

Konstanten der Nachbesserung: nur die Tabelle `k_liftsperre` (Raum/Slot/Unterprogramm/Rechteck = Satzbytes @0x01CE6/@0x01D20,
oben selbst gelesen). Das Verhalten (Mitte-Pflicht) ist als **PORT-WAHL nach RE2 ROOM6030 sub10 @0x02F70-7A** gekennzeichnet
(Kommentar in itemband_r37.c/.h, aot_common.c, Dossier NB1.2 c / NB1.8) — der Beleg traegt die Wahl (RE2 Retail laesst den Lift
nur mit der Spielermitte in der Plattformzone fahren). Die byte-true Teile des ersten Stands (Band-Gate @0x80042cac-ccc,
Member 0x12 @0x80041228) hat Abnahme 0 bestaetigt und ich erneut gelesen (Tabellenzeilen "erster Stand"); sie sind seit
05117c03 unveraendert (Diff oben).
Rate-Signale: `git diff master...HEAD -- re15_port | grep -i -E "deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|approx|vermutlich|geschaetzt|ungefaehr"`
-> nur die unveraenderte Kontextzeile `re15_item_modal.h ... deferred grant`. Einziger `getenv` = `RE15_ITEMBAND_LOG`
(Messschrieb, kein Verhalten, kein Env-Schalter als Ausweg).

## 3. Nachbesserung 1 an der exe: Lift A ROOM2030/2031, ECHTER Weg ueber die Tuer aus 2070/2071
Messkopie `re15_pc_r37d_abn1.exe` (HEAD d5504016), Skript Scratchpad `abn_d1/lauf.sh` (RE15_NO_INTRO/NOAUDIO/WINDOW_SCALE=1,
RE15_WEGPLAN, RE15_ITEMBAND_LOG, RE15_CLIMB_TRACE), Laufordner `abn_d1/laeufe/<lauf>`, Plaene `abn_d1/plaene/<lauf>.txt`.
Standplatz der Tuer 2070 -> 2030 selbst bestimmt: `probe_r33_tueren.exe standplatz 2070 2030` -> `Slot 1 -> ROOM2030:
RE15_PLAYER_POS=4880,-6400,0,0` (2071 -> 2031 gleich). Strom (3,126) per RE15_SET_FLAG (Sicherung 2060, Spielvorgabe).
Eigene Druckstellen (nicht nur die des Bau-Agenten): je Richtung zwei Stellen AUSSERHALB der Kabine, deren Punkt 620 voraus
das Ausloese-Rechteck trifft, dazu eine "sichere" Aussenstelle (z 11270, Mitte knapp ausserhalb).

### B1 — aufwaerts, Leon (`b1_tuer_lift_auf`, 153 s)
`bash lauf.sh b1_tuer_lift_auf plaene/b1_tuer_lift_auf.txt RE15_SET_FLAG=3:126 RE15_DEBUG_JUMP=2070@250 RE15_PLAYER_POS=4880,-6400,0,0 RE15_EXIT_AT=3000#2030`
* debug.log: `[room] PC loaded room2070.rdt`, `[aot] DOOR FIRE slot=1 rect=(5500,-6400,hw=500,hh=1500) target_cut=0 spawn=(6550,-5400,-100)`, `[room] PC loaded room2030.rdt`
* itemband.txt:
  `F300 room=2030 LIFT slot 5 sub 2 spieler=(6611,-5400,10764) band 3 mitte im Ausloeser nein -> GESPERRT (RE2 6030 sub10 @0x02F78)`
  `F759 ... spieler=(6416,-5400,10872) ... nein -> GESPERRT` (Blick 3248, Punkt voraus (6582,11469) im Rechteck)
  `F1218 ... spieler=(6616,-5400,11270) ... nein -> GESPERRT`
  `F1667 ... spieler=(6607,-5400,11936) band 3 mitte im Ausloeser ja -> FAEHRT`, `F1758 MEMBER 0x12 Spieler: Kollisionsband 3 -> 6 (@0x80041228) y=-10800`
  `F2392 ITEM slot 11 floor 6 item 0x0D tk 109 spieler-band 6 spieler=(-644,-10800,14504) -> FREI`
* wegplan.txt: `(3,222) = 0` nach jedem Aussendruck (F703, F1162, F1621; Spieler bleibt an der Druckstelle), `F2160 (3,222) = 1`,
  `F2205 (6594,13638) angekommen`, `F2349 (-644,14504) angekommen`, **`F2621 FLAG (9,109) = 1`** (SPAS-12 aufgenommen).

### B2 — abwaerts, Leon (`b2_tuer_lift_ab_sued`, 181 s)
`bash lauf.sh b2_tuer_lift_ab_sued plaene/b2_tuer_lift_ab_sued.txt RE15_SET_FLAG=3:126 RE15_DEBUG_JUMP=2070@250 RE15_PLAYER_POS=4880,-6400,0,0 RE15_EXIT_AT=150#2070`
* `F315 LIFT slot 5 ... (6614,-5400,11947) ... ja -> FAEHRT`, `F406 MEMBER 0x12 ... 3 -> 6`; oben nach Sueden:
  `F891 LIFT slot 4 sub 2 spieler=(6596,-10800,13279) band 6 mitte im Ausloeser nein -> GESPERRT` (Blick 1040),
  `F1342 ... spieler=(6740,-10800,13111) ... nein -> GESPERRT` (Blick 848); `(3,222) = 1` nach beiden (F1294, F1745) = keine Fahrt;
  `F1800 ... spieler=(6616,-10800,12085) ... ja -> FAEHRT`, `F1891 MEMBER 0x12 ... 6 -> 3 y=-5400`, `F2293 (3,222) = 0`;
  `F2450 (6591,2101)`, `F2493 (6600,30) angekommen`; debug.log **`[aot] DOOR FIRE slot=0 rect=(7350,-200,hw=500,hh=1750) target_cut=0
  spawn=(4800,0,-6300)`**, **`[room] PC loaded room2070.rdt`**, `EXIT_AT: Bild 150 in Raum 2070`.

### B1e / B2e — dasselbe mit Elza (`RE15_PSELECT_AUTO_SWITCH=1`, Raeume 2071/2031)
* B1e (149 s): `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`, `loaded room2071.rdt`, `DOOR FIRE slot=1 ... spawn=(6550,-5400,-100)`,
  `loaded room2031.rdt`; `F300 / F759 / F1218 room=2031 LIFT slot 5 ... nein -> GESPERRT`, `F1667 ... ja -> FAEHRT`,
  `F1758 MEMBER 0x12 3 -> 6`, `F2392 room=2031 ITEM slot 11 floor 6 ... spieler-band 6 ... -> FREI`, **`F2621 FLAG (9,109) = 1`**.
* B2e (208 s): `F891 / F1342 room=2031 LIFT slot 4 ... nein -> GESPERRT`, `F1800 ... ja -> FAEHRT`, `F1891 MEMBER 0x12 6 -> 3`,
  **`DOOR FIRE slot=0 ...`**, **`loaded room2071.rdt`**.

### G — GEGENPROBE am Stand VOR der Nachbesserung (05117c03, selbst gebaut)
Quelle `git archive 05117c03 re15_port` (ohne shared_assets) nach Scratchpad `abn_d1/vor_nb1/src`, `diff --strip-trailing-cr`
aot_common.c == 05117c03, eigener Bau (`cmake --build ... --target re15_pc`, `build rc=0`), exe ohne die Zeichenkette
"mitte im Ausloeser", mit "MEMBER 0x12 Spieler"; als `re15_pc_r37d_vor_nb1_abn1.exe` neben die exe gelegt (Assets ueber den
exe-Anker: `[asset] cd-root: .../r37_itemband/re15_port/shared_assets/PSX`). (Hinweis: die im exe-Ordner liegende Messkopie des
Bau-Agenten `re15_pc_r37d.exe` (12:18) ist NICHT der Stand 05117c03 — mit ihr fuhr der Lift ohne Bandwechsel (climb.txt
`pos=(6595,-10800,11948) band=3`), sie enthaelt den Member-Haken nicht; nicht als Referenz verwendet.)
* G1 (`laeufe/vorher/g1_vorher_auf`, gleicher Anlauf wie B1, erster Druck): `F390 MEMBER 0x12 Spieler: Kollisionsband 3 -> 6`,
  `F792 spieler=(6611,10182) ... FLAG (3,222) = 1`, danach `GEHE (6600,12000)` ohne Ankunft, climb.txt `T1613 pad=0010
  pos=(6649,-10800,10182) band=6` -> **Softlock** (= Abnahme 0 L1).
* G2 (`laeufe/vorher/g2_vorher_ab`, gleicher Weg wie B2 bis zum ersten Aussendruck (6596,13279) Blick 1040): `F982 MEMBER 0x12 ...
  6 -> 3`, `F1384 spieler=(6596,13768) ... FLAG (3,222) = 0`, climb.txt `T2213 pad=0010 pos=(6610,-5400,13768) band=3` -> **Softlock**
  (= Abnahme 0 L2).
-> Meine Druckstellen diskriminieren: am Stand 05117c03 fuehren sie in die Falle, am Stand d5504016 sind sie GESPERRT und der
vorgesehene Weg (Druck in der Kabine) faehrt — fuer Leon und Elza, in beiden Richtungen.

## 4. Tests der Spur (selbst gefahren, Spurbaum)
* `ctest --test-dir re15_port/build -R "^unit_r37_itemband$" --timeout 600 --output-on-failure -V` -> `1/1 Test #678: unit_r37_itemband
  ... Passed 1.35 sec`, 75 `ok:`-Zeilen, 0 `FAIL`. Teil L: Eingang (6605,10770)/(6605,10850) Blick Sued und (6605,13245)/(6605,13400)
  Blick Nord -> ev 0, Kabine -> ev 2, je ROOM2030 und ROOM2031; Lift B / 3050 / 50D0 ohne Sperre. Teil F (echte Port-Kollision
  `re15_collision_constrain`, echte Fahrt ueber das Unterprogramm, Flut im 50er-Netz): `2030 Lift A auf: Ausloesestellen 1152,
  Sperre ja (432 gesperrt); Fallen ohne Sperre 108 ... mit Sperre 0`, `2030 Lift A ab: 1351 (433 gesperrt); Fallen ohne Sperre 307
  (x 5925..6775 z 13050..13450 -> Endlage z 13768), mit Sperre 0`, 2031 gleich, Lift B / 3050 / 50D0: `Fallen ohne Sperre 0, mit Sperre 0`.
  Gelesen: der Zensus misst wirklich (Gegenprobe "ohne Sperre gaebe es Fallen" im selben Lauf), Referenz-Komponente muss die
  SPAS-12-Stelle bzw. den Tuer-Standplatz enthalten.
* `ctest ... -R "^integration_r37_itemband(_lift)?$" --timeout 3000 -j 2 --output-on-failure` ->
  `1/2 Test #680: integration_r37_itemband_lift .... Passed 606.66 sec`, `2/2 Test #679: integration_r37_itemband ......... Passed 891.07 sec`,
  **`100% tests passed, 0 tests failed out of 2`**. Laufordner `re15_port/build/r37_itemband_lift_wd/lauf_36894056/`: liftauf2030/2031
  `F254 LIFT slot 5 ... (6605,-5400,10770) ... nein -> GESPERRT`, `F717 ... ja -> FAEHRT`, `F1443 ITEM slot 11 ... -> FREI`, (9,109) 0 -> 1;
  liftab2030/2031 `F844 LIFT slot 4 ... (6605,-10800,13245) ... nein -> GESPERRT`, `F1308 ... ja -> FAEHRT`, `DOOR FIRE slot=0`,
  `loaded room2070.rdt` / `room2071.rdt`.

## 5. Nutzer-Punkt #8 an der exe (Stand d5504016) — Super Redhawk ROOM3050/3051, ECHTE Tueren
Standplaetze der Tueren wie Abnahme 0 (30E0 -> 3050 `RE15_PLAYER_POS=4273,4861,1792,0`; 3040 -> 3050 `-26305,-4975,2048,0`).
### A1 — von OBEN (Band 4), Kiste unberuehrt, Leon (`a1_tuer_oben`, 123 s)
`bash lauf.sh a1_tuer_oben plaene/a1_tuer_oben.txt RE15_DEBUG_JUMP=30E0@250 RE15_PLAYER_POS=4273,4861,1792,0 RE15_EXIT_AT=2600#3050`
* `loaded room30e0.rdt`, `DOOR FIRE slot=6 rect=(3700,4625,hw=325,hh=975) target_cut=8 spawn=(-3596,-7200,1523)`, `loaded room3050.rdt`
* itemband: `F791 / F854 / F917 room=3050 ITEM slot 6 floor 1 item 0x07 tk 92 spieler-band 4 spieler=(-23297,-7200,-22642) -> GESPERRT (@0x80042ccc bne Band)`
* wegplan: `F63 FLAG (9,92) = 0`, `F1020 FLAG (9,92) = 0`; zweite Stellung (-22409,-22782) Blick 1984 (Wand, `KLEMME ... Rest 520`),
  zwei weitere Druecke ohne Item in Reichweite, `F1274 FLAG (9,92) = 0`; climb.txt durchgehend `band=4` (2600 Zeilen). -> **nicht aufnehmbar.**
### A2 — von UNTEN (Band 0) OHNE Kiste, Leon (`a2_tuer_unten`, 98 s) — der woertliche "ohne die Kiste"-Fall
`bash lauf.sh a2_tuer_unten plaene/a2_tuer_unten.txt RE15_DEBUG_JUMP=3040@250 RE15_PLAYER_POS=-26305,-4975,2048,0 RE15_EXIT_AT=1800#3050`
* `loaded room3040.rdt`, `DOOR FIRE slot=1 rect=(-26925,-4975,hw=375,hh=925) target_cut=0 spawn=(-23996,0,1523)`, `loaded room3050.rdt`
* `F422 (-24051,-22622) angekommen` (Kerbe, Kiste oben bei (-18000,-7200,-20800)), itemband `F446 / F509 / F572 ... spieler-band 0
  spieler=(-24051,0,-22622) -> GESPERRT`, `F675 FLAG (9,92) = 0`, `F676 FLAG (3,93) = 0`. -> **nicht aufnehmbar.**
### A2e — dasselbe mit Elza (`a2e_tuer_unten_elza`, 93 s)
`PL04 (character=4, Elza-Bit=1)`, `loaded room3041.rdt`, `DOOR FIRE slot=1 ...`, `loaded room3051.rdt`, `F446 / F509 / F572 room=3051 ITEM
slot 6 ... spieler-band 0 ... -> GESPERRT`, `F675 FLAG (9,92) = 0`.
### A3 — VORGESEHENER Weg (Fall, Lift, Kerbe, Klettern), Leon und Elza (`a3_tuer_weg_leon` 194 s / `a3_tuer_weg_elza` 204 s)
| | Leon (30E0 -> 3050) | Elza (30E1 -> 3051) |
|---|---|---|
| Tuer | `DOOR FIRE slot=6 ... spawn=(-3596,-7200,1523)`, `loaded room3050.rdt` | `DOOR FIRE slot=2 ... spawn=(-3596,-7200,1523)`, `loaded room3051.rdt` |
| Fall | `F1235 SCHIEB: Objekt-y -7200 -> -7192 (Fall)` | `F1241 ... (Fall)` |
| Lift | `F2201 MEMBER 0x12 Spieler: Kollisionsband 4 -> 0 (@0x80041228) y=0`, `F2603 FLAG (3,82) = 1` | `F2205 ... 4 -> 0`, `F2607 FLAG (3,82) = 1` |
| Kerbe | `F3453 SCHIEB: Bedingung erfuellt (obj.z = -22225)`, `F3514 FLAG (3,93) = 1` | `F3577 (obj.z = -22206)`, `F3638 FLAG (3,93) = 1` |
| Aufnahme | `F3807 ITEM slot 6 floor 1 ... spieler-band 1 spieler=(-24072,-1800,-22732) -> FREI`, **`F4036 FLAG (9,92) = 1`** | `F3926 ... spieler-band 1 ... -> FREI`, **`F4155 FLAG (9,92) = 1`** |
-> Am abzunehmenden Stand: Super Redhawk von oben (Band 4) und von unten ohne Kiste (Band 0) GESPERRT, auf dem vorgesehenen Weg
fuer beide Figuren aufnehmbar. Die uebrigen 5 Stellen (3010 Steg, 2090 Plattform, 2040 Halle, 2030 SPAS-12/Schrot) belegt mein
Lauf von integration_r37_itemband (12 Laeufe gruen, Abschnitt 4); die SPAS-12 zusaetzlich B1/B1e ueber die echte Tuer.

## 6. Restrisiko der Sperre: Bewegung zwischen Druck und Fahrt (eigene Zusatzpruefung)
Die Sperre prueft die Spielermitte im Druckbild. Eine Falle bliebe moeglich, wenn der Spieler WAEHREND der Fahrt (nach dem Druck,
vor Member 0x12 am Ende) weiterlaufen koennte. Gemessen ohne Wegplan, nur mit Eingabeskript (UP gehalten ueber die ganze Fahrt):
* B4 (`laeufe/b4_fahrt_mit_gehen`): Start (6605,12500) Blick Nord, `RE15_INPUT_SCRIPT="W10,U0.3,UA0.2,U8"` (Spielbild-Basis) —
  der Druck kam erst bei z ~11500 (Punkt 620 voraus z ~10880 ausserhalb des Rechtecks, sat 0x31): KEINE Fahrt (itemband.txt leer),
  der Spieler laeuft auf Band 3 nach Norden aus der Kabine (climb.txt `T597 pos=(6782,-5400,10316) band=3`) — Original-Verhalten
  des Ausloesers, kein Fehler.
* B4b (`laeufe/b4b_fahrt_mit_gehen`): Start (6605,12332) Blick Nord, `RE15_INPUT_SCRIPT="W10,UA0.2,U8"`: `[input-script] Tick 300 -> F300
  Tasten 0x8010`, itemband `F300 room=2030 LIFT slot 5 sub 2 spieler=(6605,-5400,12257) band 3 mitte im Ausloeser ja -> FAEHRT`,
  `F391 MEMBER 0x12 ... 3 -> 6`; climb.txt waehrend der Fahrt `T557 pm=2 pad=0010 pos=(6605,-5760,12257)` ... `T641 pm=2 pad=0010
  pos=(6605,-10800,12257)` — **x/z bleiben trotz gehaltenem UP stehen** (gemessen: player_mode 2 waehrend der ganzen Fahrt); danach `T647 pm=1 ... pos=(6605,-10800,11807) band=6`, `T653 ... 11618` (Nordwand r0[49] oben) — in der
  oberen Kabine, keine Falle.
* B5 (`laeufe/b5_auf_ab_eingang`, Sperre auch nach dem Neu-Scharfmachen per Aot_reset sub02 @0x01EB0 `46 05 03 31 ff 00 18 02`):
  `F315 LIFT slot 5 ... (6614,-5400,11947) ... ja -> FAEHRT`, `F861 LIFT slot 4 ... (6612,-10800,12095) ... ja -> FAEHRT`,
  `F952 MEMBER 0x12 ... 6 -> 3`, dann im Eingang `F1436 LIFT slot 5 sub 2 spieler=(6595,-5400,10837) band 3 mitte im Ausloeser nein
  -> GESPERRT`, `F1839 FLAG (3,222) = 0`, in der Kabine `F1900 ... (6590,-5400,11947) ... ja -> FAEHRT`, `F2393 (3,222) = 1`.
  (Vorlauf `b5_auf_ab_eingang_v1`: Druck oben bei z 11947 Blick Nord feuert NICHT — Punkt voraus z 11327 < 11350, sat 0x31 wie im
  Original; die Kabine oben verlangt fuer die Abfahrt mit Blick Nord z >= 11970. Kein Fehler, nur die Ausloese-Geometrie.)
-> Zwischen Druck und Fahrt bewegt sich der Spieler hoechstens im Druckbild selbst (hier 75); die Mitte-Pflicht (z >= 11350 bzw.
<= 12850) laesst bis zur Kippstelle der Ausschiebung (10900 / 13050) ueber 450 Reserve. Die Sperre ist an der exe dicht.

### C1 — Nebenwirkung Lift B ROOM2030 (ohne Sperre, Member 0x12 = Band 3 <-> 9) an der exe (`laeufe/c1_liftb`, 107 s)
Lift-B-Saetze selbst gelesen: `0x1D88 2c 07 03 31 03 00 ca d6 22 0b dc 05 dc 05 ff 00 18 04 00 00` (slot 7, floor 3, x -10550..-9050
z 2850..4350, Unterprogramm 4) / `0x1DC2 2c 06 03 31 09 00 ...` (slot 6, floor 9). Weg auf Band 9 zur Tuer slot 1 (`0x01AEA`, floor 9,
Rechteck (-8200,5500,1000,1800)) aus dem Zellmodell geplant (eigenes Skript `abn_d1/pfad.py`: (-9800,3600) -> (-7000,3600) -> (-7000,6400)).
`bash lauf.sh c1_liftb plaene/c1_liftb.txt RE15_SET_FLAG=3:126 RE15_DEBUG_JUMP=2030@250 RE15_PLAYER_POS=-9800,3600,1024,3 RE15_EXIT_AT=150#2080`
-> itemband `F266 MEMBER 0x12 Spieler: Kollisionsband 3 -> 9 y=-16200`, `F872 ... 9 -> 3 y=-5400`, `F1478 ... 3 -> 9`; (3,220) 0 -> 1 -> 0 -> 1;
`F1949 (-7067,3578) angekommen`, `F2018 (-7007,6390) angekommen`, debug.log `[aot] DOOR FIRE slot=1 rect=(-7700,6400,hw=500,hh=900)
target_cut=0 spawn=(-23200,-9000,-16700)`, `loaded room2080.rdt`. -> Lift B faehrt in beide Richtungen, die Band-9-Tuer ist danach
erreichbar (Tuer-Band-Gate verlangt Band 9 = Kollisionsband aus Member 0x12). (Vorlauf `c1_liftb_v1`: mein erster, gerader
Weg (-8700,6400) lag in einer Band-9-Zelle — Wegplanung, kein Befund.)

### C2 — Nebenwirkung Lift ROOM3050 AUFWAERTS (Member 0x12 = Band 0 -> 4) an der exe (`laeufe/c2_3050_auf`, 78 s)
Saetze selbst gelesen: `0x211E 2c 05 03 31 00 00 1c f3 54 d9 f0 0a ac 0d ff 00 18 02 00 00` (slot 5, floor 0, x -3300..-500 z -9900..-6400,
Unterprogramm 2), Tuer slot 1 `0x01EA6` floor 4 Rechteck (-4596,1823,2140,520) -> 30E0. Weg auf Band 4 aus dem Zellmodell
((-1900,-8150) -> (-5400,-8150) -> (-5400,-4500) -> (-3550,-4500) -> (-3550,1300)).
`bash lauf.sh c2_3050_auf plaene/c2_3050_auf.txt RE15_SET_FLAG=3:82 RE15_DEBUG_JUMP=3050@250 RE15_PLAYER_POS=-1900,-8150,0,0 RE15_EXIT_AT=150#30E0`
-> `F230 MEMBER 0x12 Spieler: Kollisionsband 0 -> 4 (@0x80041228) y=-7200`, `F632 FLAG (3,82) = 0`, `F973 (-3530,1269) angekommen`,
`[aot] DOOR FIRE slot=1 rect=(-3526,2083,hw=1070,hh=260) target_cut=6 spawn=(4225,0,4550)`, `loaded room30e0.rdt`. (Die Abfahrt
4 -> 0 belegt A3.) -> Mit B1-B5, C1, C2, A3 sind alle Lifte mit Member 0x12 an der exe in beiden
Richtungen gefahren (50D0: Auffahrt Abnahme 0 N1, Abfahrt Dossier §5.7 am ersten Stand — die Nachbesserung aendert 50D0 nicht);
nur Lift A traegt die Sperre.

## 7. Gates
* **RE-Gate:** erfuellt. Alle verhaltensrelevanten Konstanten tragen eine Adresse/einen Satz-Offset (Tabelle `k_liftsperre` =
  Satzbytes @0x01CE6/@0x01D20; `RE15_R37D_MEMBER_BAND 18` @0x80010c8c[18] -> @0x80041228; `RE15_R37D_BAND_EGAL 0x80` @0x80042cb4),
  die Abweichung vom RE1.5-Original (Lift A faehrt nur mit der Mitte in der Kabine) ist als PORT-WAHL nach RE2 ROOM6030
  gekennzeichnet und mit Bytes/Instruktionen belegt; 12 Tabellenzeilen mit ueber 25 Einzelstellen selbst gelesen (Abschnitt 2), alle bestaetigt. Keine Rate-Signale,
  kein Env-Schalter fuer Verhalten.
* **Erklaert der Fix den Befund?** Ja. (a) Nutzer-Punkt: am Vorher-Stand (Abnahme 0, Vorher-exe) nahm Leon die Super Redhawk von
  Band 4 und von Band 0 ohne Kiste auf; das Band-Gate @0x80042cac-ccc sperrt genau diese Druecke (A1/A2/A2e `spieler-band 4/0 ->
  GESPERRT`), Member 0x12 setzt auf dem vorgesehenen Weg das Kollisionsband (A3 `4 -> 0`), Klettern bringt Band 1 (`-> FREI`).
  (b) Nachbesserung: die Fallen entstehen, weil sat 0x31 nur den Punkt 620 voraus prueft (@0x80042ef8) und Member 0x12 den Spieler
  dort mitnimmt, wo er steht — die Mitte-Pflicht schliesst genau die Druckstellen mit der Mitte ausserhalb (G1/G2 am Stand 05117c03:
  Falle; B1/B2 am Stand d5504016: GESPERRT, aus der Kabine faehrt er).
* **Vertrag:** erfuellt. Keine Bank-9-Bits, keine Nachrichten-Ids, keine Slots/Ereignisse belegt oder umgewidmet (die Sperre
  verbraucht nur den Druck); Pfade nur engine/src/itemband_r37.c, include/re15_itemband_r37.h, Haken aot_common.c (+5 gesamt:
  Include, ITEM-Gate 2 Zeilen, Lift-Sperre 2 Zeilen, Kommentar "Runde 37 Spur D"/"Spur D NB1") und actor_common.c (+4/-1) = die D
  zugeteilten gemeinsamen Dateien; Tests nur ueber tests/unit/probes/r37_itemband.cmake; beide integration-Skripte mit
  `cmake_minimum_required(VERSION 3.16)`; test_r37_itemband.c / itemband_r37.c ohne POSIX-Aufrufe; keine Aenderung unter release/,
  platform/android/, shared_assets/PSX/, keine Test-CMakeLists.
* **Tests je Punkt vorhanden und messend:** unit_r37_itemband (Z/G/M + L/F, mit Gegenprobe im Lauf), integration_r37_itemband
  (12 Laeufe), integration_r37_itemband_lift (4 Laeufe, Verhalten ueber die (3,222)-Folge, nicht nur ueber den Messschrieb) — alle
  selbst gruen gefahren (Abschnitt 4).

## 8. Gezielte Regression (Spurbaum, KEINE volle Suite)
Der Diff beruehrt den AOT-Scan (aot_common.c: ITEM-Gate + GENERIC-Feuer jedes Ereignis-Satzes) und Member_set (actor_common.c,
jede SCD) — deshalb alle Unit-/Sonden-Tests (schnell) und die Integrationsgruppen der beruehrten Module/Raeume.
* `ctest --test-dir re15_port/build -R "^(unit|probe)_" --timeout 600 -j 8 --output-on-failure` -> `99% tests passed, 1 tests failed out
  of 583` (`Total Test time (real) = 1526.88 sec`); einziger Ausfall `unit_r35_android_bash_mutanten (Timeout)` nach 1500.99 s
  (TIMEOUT-Eigenschaft 1500, Mutationstest von release/apk_pruefen.sh + gate_urteil.py, probes/r35_android.cmake) — beruehrt
  keine Datei des Diffs. Einzel-Nachlauf ohne Parallel-Last: `1/1 Test #480: unit_r35_android_bash_mutanten ... Passed 1046.45 sec`
  -> **Unit/Sonden 583/583 gruen**.
* `ctest ... -R "^integration_(elza_vollstart|item_name_census|r30_irons_tisch_(bild|laden|licht)|r30_sicherung_(bild|laden)|r34n_e_dokumente_bild|r35_inventar1050|r35_karte_fahrstuhl|r36_inventar|r36_karte|r36_kleidung_spielweg|r36_klettern|r36_schluessel|r36_schluessel_befreiung|r36_tueren|keypad)$" --timeout 3000 -j 3`
  -> **`100% tests passed, 0 tests failed out of 18`** (`Total Test time (real) = 713.31 sec`), u.a. integration_r36_klettern 573.61 s,
  integration_r35_karte_fahrstuhl 247.86 s, integration_r36_tueren 53.86 s, integration_r30_sicherung_laden 57.82 s.
* Dazu Abschnitt 4: integration_r37_itemband (891.07 s) und integration_r37_itemband_lift (606.66 s) gruen.
* Nicht erneut gefahren: integration_r36_dachkiste (1400 s; endet nur mit der Tuer 2020 -> 2030 Band 9, faehrt Lift A nicht; die
  Nachbesserung aendert nur Lift A) — in Abnahme 0 am Stand 05117c03 und vom Bau-Agenten am Stand d5504016 gruen.
* GEGENPROBE des neuen Riegels (selbst, gegen meine 05117c03-exe):
  `cmake -DRE15_PC_EXE=.../re15_pc_r37d_vor_nb1_abn1.exe -DWORKDIR=... -DLAEUFE=^liftauf2030$ -P tests/integration/test_r37_itemband_lift.cmake`
  -> exit 1 nach 106 s, `r37_itemband_lift: liftauf2030: FLAG (3,222) gelesen '0;1;1', erwartet '0;0;1'` (Spieler danach
  `spieler=(5968,10182)`); `-DLAEUFE=^liftab2030$` -> exit 1 nach 120 s, `liftab2030: FLAG (3,222) gelesen '0;1;0', erwartet ...` —
  der Riegel ist am Stand vor der Nachbesserung ROT, am Verhalten (Logs `abn_d1/gegenprobe_integ/`).

## 9. Maengel der Abnahme 0 — Stand nach der Nachbesserung
| Mangel (D_abnahme_0.md §8) | Stand d5504016 | Beleg (diese Abnahme) |
|---|---|---|
| 1 Softlock Lift A AUFWAERTS aus dem Eingang | behoben | B1/B1e: Eingang (6611,10764) und (6416,10872) GESPERRT, (3,222) bleibt 0, Kabine FAEHRT, SPAS-12 (9,109) = 1; G1 am Stand 05117c03: dieselbe Stelle -> (6611,10182) Klemme |
| 2 Softlock Lift A ABWAERTS suedlich der oberen Kabine | behoben | B2/B2e: (6596,13279) und (6740,13111) GESPERRT, Kabine FAEHRT, Tuer slot 0 -> 2070/2071; G2 am Stand 05117c03: -> (6596,13768) Klemme; B5: auch nach Aot_reset gesperrt |
| 3 Dossier falsch/unvollstaendig | behoben | D_itemband.md §5.5 ("BERICHTIGT (Nachbesserung 1)"), §7 (alte OFFEN-Zeile gestrichen + berichtigt), §8 (Bedienaenderung Lift A, Falle genannt), NB1.0-NB1.10; Zahlen von mir nachgemessen (Teil F 108/307/0, exe-Zeilen) |
| 4 kein Riegel fuer die Ausloesung ausserhalb | behoben | unit Teil L/F + integration_r37_itemband_lift (4 Laeufe) gruen; am Stand 05117c03 von mir ROT gefahren ('0;1;1' / '0;1;0') |

## 10. Urteil je Punkt (gegen den Wortlaut)
| # | Wortlaut | Urteil | Beleg |
|---|---|---|---|
| 8 | "Items auf erhöhten Flächen lassen sich vom falschen Stockwerk aufnehmen, z.B. die Super Redhawk in ROOM3050 ohne die Kiste. ... muss gefixed werden." | **erfuellt** | Am Stand d5504016 selbst gemessen, ueber die ECHTEN Tueren: Super Redhawk von oben (Band 4, A1) und von unten ohne Kiste (Band 0, A2 Leon / A2e Elza) GESPERRT, (9,92) = 0; vorgesehener Weg (Fall, Lift 4 -> 0, Kerbe, Klettern auf Band 1) fuer Leon UND Elza FREI, (9,92) = 1 (A3). Alle 12 erhoehten Saetze ueber integration_r37_itemband (12 Laeufe) gruen, SPAS-12 zusaetzlich ueber die echte Tuer (B1/B1e). Vertragsteil "alle betroffenen Items bleiben auf dem vorgesehenen Weg erreichbar": die zwei Lift-A-Softlocks der Abnahme 0 sind weg (B1/B2/B1e/B2e/B5 gegen G1/G2), kein Raum ohne Ausgang mehr (Teil F: 0 Fallen an allen Liften mit Member 0x12), Restrisiko Bewegung waehrend der Fahrt gemessen ausgeschlossen (B4b). RE-Gate gehalten (Abschnitt 2), Abweichung vom Original als PORT-WAHL nach RE2 belegt. |

Nicht Gegenstand (Hinweis): Der in diesem Lauf relayte Nutzer-Satz zur Beretta M93R ("wenn sie schon mehrfach schießt, ... dass die
Gegner mehrfach getroffen werden") ist AUFTRAG.md "Nachtrag 2026-10-12" / VERTRAG.md §0 Spur G (r37_beretta); der Zweig r37/itemband
beruehrt keinen Schuss-/Waffen-Code (`git diff master...HEAD --name-only`) — weder umgesetzt noch beeintraechtigt.

## 11. Maengel
Keine.

## 12. Hinweise (kein Mangel)
* H1 PORT-WAHL: Lift A in ROOM2030/2031 weicht bewusst vom RE1.5-Original ab (dort faehrt er auch von ausserhalb und setzt fest);
  Vorbild RE2 ROOM6030 (Retail-Fassung von 50D0). Lift B, ROOM3050 und ROOM50D0 bleiben original (dort 0 Fallen). Im Dossier §8 /
  NB1.10 fuer den Nutzer beschrieben ("ausserhalb der Kabine passiert beim Druecken nichts").
* H2 Paket: engine/src/itemband_r37.c ist eine neue Engine-Datei (erster Stand) — beim Android-Bau den GLOB-Cache app/.cxx neu erzeugen.
* H3 Messkopien im exe-Ordner: `re15_pc_r37d.exe` (12:18, Bau-Agent) ist NICHT der Stand 05117c03 (Lift ohne Bandwechsel = ohne
  Member-Haken) — nicht als Vorher-Referenz verwenden. Meine Kopien (`re15_pc_r37d_abn1.exe`, `re15_pc_r37d_vor_nb1_abn1.exe`) habe
  ich nach der Abnahme entfernt; der 05117c03-Bau liegt im Scratchpad `abn_d1/vor_nb1/build`.
* H4 Laufzeiten fuer die Integration: integration_r37_itemband ~890 s, integration_r37_itemband_lift ~610 s (TIMEOUT je 3000).
* H6 Integration (andere Spuren): Das ITEM-Band-Gate gilt auch fuer Port-ITEM-Saetze. `re15_aot_init` nullt beim Raumaufbau alle
  Slots (aot_common.c:94, band = 0), `re15_aot_set` laesst `band` aber stehen — ein Port-Installer, der einen vom Original-SCD schon
  belegten Slot umwidmet, muss das Band selbst setzen (wie dokumente_r34/inhalt_r35/irons_tisch_1150/schluessel_r36). Betrifft u.a.
  den neuen Feuerloescher-Fundort der Spur I (ROOM1050 Slot 13, UEBERGABE 13:36): an der Integrations-exe die Aufnahme dort pruefen.
* H7 Integration mit Spur C (Aufzugton STAGE2, UEBERGABE 13:08 "INTEGRATIONS-PFLICHT 2030 Lift A und B"): ein gesperrter Druck an
  Lift A startet kein Unterprogramm (Druck verbraucht, `break` vor `fired_event_id_this_frame`) — ein Ton, den Spur C an die Fahrt
  haengt, darf dort nicht spielen; an der Integrations-exe mit pruefen.
* H5 `unit_r35_android_bash_mutanten` lief in meinem -j 8-Unit-Lauf in den Timeout (1500 s, Host-Last durch parallele Agenten);
  Einzel-Nachlauf `ctest ... -R "^unit_r35_android_bash_mutanten$"` -> `1/1 Test #480: unit_r35_android_bash_mutanten ... Passed 1046.45 sec`
  (Last-Artefakt; der Test prueft nur release/apk_pruefen.sh + gate_urteil.py, keine Datei des Diffs).

## 13. Ergebnis
**bestanden = true.** Nutzer-Punkt #8 ist am Stand d5504016 erfuellt und selbst gemessen (Super Redhawk von oben und von unten ohne
Kiste gesperrt, vorgesehener Weg fuer Leon und Elza frei; alle 12 erhoehten Saetze ueber den Riegel). Die Maengel 1-4 der Abnahme 0
sind behoben: Lift A faehrt nur noch mit der Spielermitte in der Kabine (PORT-WAHL nach RE2 ROOM6030 sub10 @0x02F70-7A, Bytes und
Instruktionen selbst gelesen), beide Softlocks sind an der exe weg (Leon + Elza, beide Richtungen, echte Tuer) und am Stand 05117c03
nachweislich vorhanden; der neue Riegel ist dort rot. Alle Gates halten; gezielte Regression gruen.
