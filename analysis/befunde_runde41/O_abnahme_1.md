# Runde 41 Spur O "keyitems" - Abnahme 1

Stand: r41/keyitems 8271c2726 (Basis f5a5e2f7e, master ist 28 Commits weiter). Bau: `local_build.sh configure` + `build` = `no work to do`, exe md5 18978cc1...
Laufordner aller eigenen Messungen: `$TEMP/o41a1/run/<lauf>`, Bogen `$TEMP/o41a1/b_*.png`, Skripte `$TEMP/o41a1/py/`, Treiber `$TEMP/o41a1/lauf.sh` (Messkopie `re15_pc_o41a1_<lauf>.exe`, keine geerbten RE15_*, Bilder RE15_FRAMEDUMP mit RE15_WINDOW_SCALE=1). Kein Code geaendert, Mutanten nur als Kopie ausserhalb des Baums.

## Ergebnis

| Punkt | Urteil |
|---|---|
| 1 Kurbel: Leon fliegt nicht mehr an die Position | erfuellt |
| 2 Key Items, die keine Schluessel sind, ueber das Inventar | erfuellt |
| 3 Karten wie RE2 | erfuellt |
| Nachbesserung M1 (Riegel 5b') | behoben, gemessen |
| Nachbesserung M2 (PORT-WAHL im Code) | behoben, gelesen |
| RE-Gate | haelt |
| Vertrag | haelt |
| Tests | gruen; ein Rot ist Altlast der Basis, in master schon behoben (Abschnitt Tests) |

**Bestanden. Keine Maengel.** Neun Hinweise am Ende, keiner blockiert.

## Punkt 1: Kurbel

Wortlaut: "leon 'fliegt' quasi immer noch an die richtige Position bis das drehen dann los geht."

Lauf k_stand (Leon, echte Tuer 2050 -> 2020, Handgun, zu Fuss an die Stelle, START, ITEM, Kurbel, USE; b_k_stand_a.png, b_k_stand_zoom.png):
```
F255 USE (pad=8000)  F256..F262 Inventar blendet aus  F263 schwarz  PL(2914,-3388) rot 2016 cam=3
[scd F264] Cut_chg(3)  [scd] Plc_motion(entity=0, motion=0, flags=0x40)  F264 PL(3650,-3018,rot=1024) pst=4 mo=0
F265..F267 blendet ein, F268 erstes volles Bild: Leon kurbelt
[scd F372] Cut_chg(4)  [scd F442] Cut_chg(3)  [keyitems] Waffe 3 wieder in der Hand
```
Aenderung je Bild im Leon-Ausschnitt (40,70)-(140,200), Schwelle 24: F268 2648 (letzter Blendenschritt), F269..F330 11..160, keine Spitze. Szenenende: F443 6016 (Schnitt), F444 21, F445 14, ab F446 0: kein Kippen.

Gegenprobe des Masses, selbe exe (Lauf alt, Inventar voll, Frage + Ja): Plc_motion F352 flags=0x00, F352..F357 = 338 473 264 274 234 146, danach 58..92. Das Mass sieht das Fliegen, im USE-Lauf gibt es die Spitze nicht (b_alt.png).

Weitere Anlaeufe:
- k_renn (Rennen nach Westen, START bei (3617,-3382)): Bild N nach Pos_set gegen k_stand, N = 4..32, 13000 Pixel: 0 verschiedene Pixel in jedem Bild.
- k_ost (Stand x 1700, vorher Kamera 0): F334 Pos_set in Schwarz, F335..F337 Einblende schon Kamera 3, kein Bild der alten Kamera (b_k_ost.png).
- k_halten (Aktionstaste 36 Bilder ueber das USE hinaus gehalten): dieselben Bildnummern, kein zweiter Start.
- e_stand (Elza, Charakter 4 im Log, Tuer 2051 -> 2021): F264 wie Leon, Lage (3650,-3018), `Waffe 1 wieder in der Hand` (b_e_stand.png).
- k_grab (echtes Fenster, gdigrab 30 Bilder/s, Bogen b_grab.png): Inventar blendet aus, 9 Grabbilder schwarz, 3 Grabbilder Einblende, im ersten vollen Bild steht Leon an der Kurbel in der Kurbel-Pose. Der Readback maskiert hier also nichts.
- k_ton (ohne RE15_NOAUDIO): gleicher Ablauf, exit 0.
- k_schuss: nach Plc_ret F512 R1 + Feuer, `mg=15` -> `mg=14` in F570: die Pistole schiesst wieder, keine Lage-Drift nach der Szene (k_renn, k_stand bleiben bei (3650,-3018)).

Sollwert der Lage selbst nachgelesen, Original-Lauf E41n2/o_stand1.txt: `n89 vs9535 ... pl=3650,-2925 ... spd=0`, `n90 vs9569 ... pl=3650,-3018 ... spd=0`, `n124 ... cmd=4/0/1/0 mo=0/15 pl=3650,-3018`. Der Riegel-Sollwert -3018 stimmt.

Warum das den Befund erklaert: die Blende @0x80050cfc (+0x8f := 7) mischt die Stehpose in den Kurbel-Schluessel, davor steht Leon 20 Takte (Sleep @0x0A98). Beides faellt im USE-Lauf weg, Pos_set @0x0A8C und Plc_motion @0x0A9C fallen in dasselbe schwarze Bild.

Urteil: erfuellt.

## Punkt 2: Key Items ueber das Inventar

Wortlaut: "... weil man EXPLIZIT die Items im Player Inventory auswaehlen muss und dann genau richtig steht bei der Nutzung. Das moechte ich bei Key Items, die nicht Schluessel sind bei unseren Port auch haben!"

Eigener Zensus, unabhaengig von k_stellen (alle 206 RDTs mit Inhalt):
- Fragen mit Gegenstand (zensus_used.py): 1090, 10D0/1, 1100/1, 11E0/1 (2x), 1230/1, 2020/1, 2040/1, 2060/1, 3050/1, 4000, 4030/1. 4001 msg 1 und 40A0 msg 4 haben 0 Message_on. Das sind die 12 Skript-Stellen der Tabelle, dazu die Port-Stellen 1050 und 2010.
- Fragen ohne Gegenstand bleiben Fragen: Schalter, Hebel, Computer, Ruestung (1040, 1190, 2050, 2080, 5030, 5050, 5060, 5080, 6000 ...).
- Key-Item-Texte OHNE Frage, im Skript nachgesehen: ROOM4090 msg 0/1 ("hole to set the Enzyme", Enzyme 0x3C nie vergeben, Slot 1 sce 1 reiner Text), ROOM50A0 msg 0 (Slot sce 1), ROOM1190 msg 1 "There's a driver here" (kein Message_on), ROOM3010 msg 1 (Karte als Tuerschloss, Punkt 3), ROOM3091 msg 32 (Latch Key = Schluessel). Keine weitere Stelle.
- `Ck(9,..)` aller Skripte (zensus_ck9.py): 10D0/1 @0x019C0/@0x019AA, 11E0/1 @0x01FA8, 1230/1 @0x01488 (Tabelle), 3091 Latch Key; die uebrigen sind Aufnahme-Merker ohne Frage und ohne Benutzungstext. Op 0x5E Keep_Item_ck: 0 in allen RDTs.
- Gegenstaende (Item_aot_set + Namenstabelle, items.py): Head of Akuma 0x32 (2040) und Pocket Watch 0x3F (2090) haben in keinem Skript eine Benutzung. 0x33 und 0x35: 0 Platzierungen.
- Flag-Bits: alle `Set` auf (3,114), (3,113), (3,146), (3,53), (3,108), (3,144), (3,78) (zensus_set.py): je genau die zitierte Stelle, kein Set auf 0.
- Jede Frage-Id und Aufnahme-Id der Tabellen hat in ihrem Raum genau ein Message_on (zensus_frage.py, 28 Paare): die Haken verschlucken keine fremde Nachricht.
- Ja-Zweige der uebrigen Stellen: kein Pos_set, kein Plc_motion. Die Kurbel ist die einzige Stelle mit dem Fliegen-Mechanismus, RE15_R41O_SOFORT nur dort ist richtig.

Echtes Menue an der exe, alle 14 Stellen (START, ITEM, Platz, Auswahl, USE):
| Stelle | Leon | Elza |
|---|---|---|
| 1090 Loescher | s1090: Frage 8 entfaellt, Cut 5, msg 9 (b_s1090.png) | nur Leon-Raum |
| 10D0 Blue Keycard | s10d0: msg 9, Taste, Cut 11, Tastenfeld (b_s10d0.png) | unit |
| 11E0 Yellow Keycard | unit | e11e1: Frage 7 entfaellt, msg 9 |
| 1230 Red Keycard | s1230: Frage 7 entfaellt, msg 9 | e1231: dasselbe |
| 11E0 Zange | s11e0: Frage 11 entfaellt, Cut 14 | e11e1z: dasselbe |
| 1100 Minidisc | s1100: Frage 3 entfaellt, Cut 12 | e1101: dasselbe |
| 1050 Sicherung | s1050: Frage 20 entfaellt, msg 21 | e1051: Cut 7, Cut 8 |
| 2010 Sicherung | s2010: Frage 41 entfaellt, Eintrag weg | e2011: dasselbe |
| 2020 Kurbel | k_stand | e_stand |
| 2040 Bombe | s2040: Frage 5 entfaellt, Cut 14, Zuender-Feld (b_s2040.png) | e2041: dasselbe |
| 2060 Sicherung | s2060: Frage 4 entfaellt | e2061: Cut 8 |
| 3050 Red Master Keycard | s3050: Frage 4 entfaellt, Cut 15 | e3051: dasselbe |
| 4000 Blue Master Keycard | s4000: Frage 1 entfaellt, msg 2 (b_s4000.png) | Stelle fehlt in 4001 |
| 4030 Zuendkerze | s4030: Frage 1 entfaellt, Cut 5 | e4031: dasselbe |

Aktionstaste (a1090, a11e0, a2060, a4000, a3050, a10d0, Riegel aktion): nur Beschreibung, kein Ja/Nein, kein Ereignis. Leser a10d0 (b_a10d0.png): `Frage 7: nur Beschreibung (Seite endet @98)`, drei Seiten, Schluss "unlock the door", kein "Will you operate".
Falsche Stelle (k_falsch, b_k_falsch.png): "You can't use it here", Inventar bleibt offen, Kurbel bleibt.
Aufnahme: auf_tuer: 2010 Frage, Ja, `Gegenstand 0x33 ins Inventar ((3,114) gesetzt)`, msg 1, echte Tuer -> ROOM2000, Inventar zeigt die Kurbel (b_auf_tuer.png). e_auf ROOM2011: dasselbe (b_e_auf.png). voll2010 (b_voll.png): Bild der Kurbel, "You can't carry any more items", Flag bleibt 0.
CHECK: chk_kurbel Bild Kurbel, "The end is square-shaped" (b_chk_kurbel.png); chk_bombe Bild Bombe, "Time Bomb" (b_chk_bombe.png).

"Genau richtig stehen": Lage nach USE aus allen Anlaeufen (3650,-3018), Abschnitt Punkt 1. Vergleich RE2 ROOM1110, selbst gelesen: USE-Rechteck @0x020A0 `2c 07 06 21 00 00 0a a2 ec bc 08 07 08 07 0a 00 43 00 01 00` = x -24054..-22254, z -17172..-15372; Spieler-Lage sub13 @0x02EDA `32 00 11 a0 6c 00 34 bd` (-24559,108,-17100), @0x02EE6 `32 00 0d a1 6c 00 62 bd` (-24307,108,-17054), Gier @0x02EF0 `34 0f 00 0c`. Rechteck-Mitte -> Lage: 1393 Einheiten, RE1.5 ROOM2020 (2600,-3750) -> (3650,-2925): 1335. Auch RE2 setzt die Lage per Skript und steht beim USE nicht auf der Dreh-Lage. Beim Gehen nach Westen im Gang greift USE ab x 3620 (Vorwaerts-Punkt 620 auf x 3000), gemessen k_renn (3617,-3382): nur 365 Einheiten neben der Dreh-Lage.

Urteil: erfuellt.

## Punkt 3: Karten wie RE2

Wortlaut: "Schaue wie es in Resident Evil 2 gemacht wird, und mache es genauso."

RE2 selbst gelesen: Leser Red Card Key ROOM2110 @0x01812 `2c 09 06 21 00 00 36 a0 c4 cc 6c 07 08 07 0a 00 34 00 01 00`, @0x01AD8 `23 00 02 00 34 00`, @0x01AEA `62 34`. Tuerschloss: Handler @0x80051628 `jal 0x800696cc` (Gegenstand suchen), @0x80051640 a2 = 5, @0x8005164c `jal 0x8002fe38` (Text), keine Frage.
Port an der exe: Leser 10D0, 11E1, 1230, 1231 = USE; Computer 3050/3051 = USE; Panel 4000 = USE. Tuer 3010 (Karte als Schloss): Aktionstaste `[msg] room=3010 id=1` "You've used the Green Keycard." (g3010a), USE dort `keine Stelle` (g3010u).
ROOM3010 Slot 6: ohne Karte sce 1 Text 0 "It's locked. An ID card is required to open it.", mit Karte sce 3 -> sub02 @0x02302 `22 03 3c 01 2b 01 ff ff 02 00 47 02`, danach Door_aot_set @0x020EA auf demselben Rechteck: im Spiel eine Tuer, keine eigene Stelle (Hinweis H4).
Research Center ID Card: ROOM40A0/40A1 msg 4 hat 0 Message_on, msg 3 ist reiner Text. Nichts zu tun. Die Wegwerf-Abfrage laeuft weiter (Riegel unit_r21_discard, integration_r39_inventar gruen).

Urteil: erfuellt.

## Nachbesserung 1: Behauptungen des Bau-Agenten

- M1: `unit_r36_tueren_aufschliessen` selbst gefahren: `ROOM10D0 Kartenleser USE (KARTE_EIN): Ton 0 = Satz 0 bei Bild +0`, `aufschliessen: GRUEN (0 Fehler)`. Schaerfe selbst gemessen: Kopie von probe_r36_tueren.c mit `re15_keyitems_r41_test_ja(1)` vor dem Aot-Druck: `FEHLER: ROOM10D0 Aktionstaste: 1 Toene statt 0`, `... msg 9 ohne USE`, `aufschliessen: ROT (2 Fehler)`. Bestaetigt.
- M2: Wort PORT-WAHL und Adressen stehen in close_phase, op_plc_ret, keyitems_r41.c und im Kopf. Die Adressen habe ich selbst disassembliert (RE-Gate). Bestaetigt.
- "Nur Kommentare seit 08b21b48e": `gcc -fpreprocessed -dD -E -P` der vier seit 6fd81d33b geaenderten Dateien (keyitems_r41.c, menu_common.c, scd_vm.c, re15_keyitems_r41.h): ohne Kommentare bytegleich. Bestaetigt, ohne seine Objektvergleiche zu brauchen.
- Bit 0x40 wirkt auf Spur C: 12 Zeilen `0x3f,0x00,..,0xc0` in gen/ventilkette_r41_programme.inc, RE2 ROOM1080 @0x021EC `3f 00 10 c0`, @0x021F6 `3f 00 0f c0`. Weitere Port-Programme haben kein Bit 0x40 (Treffer der Mustersuche in dining/szl sind Text und Blobs). Lauf ventil nachgefahren, Kamera 5, Ausschnitt (138,58)-(190,165): F487..F502 = 58 23 0 0 0 0 0 0 35 71 84 54 73 63 61 71, F539..F552 = 82 121 46 63 87 90 34 29 65 72 26 47 105 74. Clipwechsel laut Pose-Spur F494 (Clip 16) und F544 (Clip 15): keine Spitze. Bestaetigt.
- Riegel z -3018: Auswerter k_stand, e_stand, k_renn rc 0; Log mit z -3369: `FAIL: erstes Kurbelbild z -3369 statt -3018`, rc 1. Bestaetigt.
- Abstand 824: (2914,-3388) -> (3650,-3018) = 823,8. Bestaetigt.

## RE-Gate

Selbst disassembliert (richtige Binaerdatei, Sprungziel gelesen):
- RE1.5 PSX.EXE: Tabelle 0x800744a8[63] = 0x80041b90 Plc_motion: @0x80041b9c `lhu a1,2(v0)`, @0x80041bac `srl a1,a1,8`, @0x80041bc8 `sh a1,452(v0)`. Blende @0x80050cf8 `ori v0,zero,0x7`, @0x80050cfc `sb v0,143(v1)`, @0x80050d24 `andi v0,v0,0x40`, @0x80050d30 `sb zero,143(v1)`. Stufe 4 @0x8001cc54 `lui v0,0xff00`, @0x8001cc5c `or a0,a0,v0`; Gate @0x8003f044 `lui v1,0x200`, @0x8003f04c `bne`. @0x80042bd0 `ori v0,zero,0x26c`. @0x80041f90 `sb v1,4(v0)`. Eigenschaftssatz 0x80074da8 + id * 12: Id 0x33 @0x8007500c `01 00 00 00 88 4c 07 80 00 00 00 00`.
- RE2 PSX.EXE: @0x8006b21c/@0x8006b220 Bit-Test, @0x8006b244/@0x8006b248 Text 7, @0x8006b2e0 `sb zero,1(s0)`, @0x8006b2ec `sh v0,-31224(at)`, @0x80068cfc/d04/d0c Valve, Manhole, Crank, @0x80068d34 `sb zero,23546(at)`, @0x80025cb0 `lui v1,0xef00`, @0x8005365c `lui v1,0x200`, @0x80056bbc `sh a2,460(v1)`, @0x800585e4 sce_item_lost.
- RE2 Blende, im Dossier nur fuer RE1.5 zitiert, von mir gefunden: @0x800669f4 `lhu v0,460(s0)`, @0x800669f8 `addiu v1,zero,7`, @0x800669fc `andi v0,v0,0x40`, @0x80066a04 `sb v1,334(s0)`, @0x80066a08 `sb zero,334(s0)`; gleicher Block @0x80066e30..@0x80066e44. Bit 0x40 heisst in RE2 dasselbe.
- RDT-Bytes (belege.py): 57 Stellen ohne Abweichung: alle Message_on, Aot_set, Set der Tabellen in keyitems_r41.c, Sleep, Plc_motion, Pos_set, Cut_chg, Plc_ret ROOM2020, ROOM4000 @0x0137C/@0x0142A. Dazu RE2 ROOM1110 @0x02FC1 `3f 00 0f 45`, @0x02F24 `53 02 02 07 00 18`, @0x02F4E `53 02 02 07 00 e8`, @0x02F66 `62 43`.
- Zensus Plc_motion selbst: 206 RDTs, 1359 Stueck, 0 mit Bit 0x40.
- Kennzeichnung: PORT-WAHL an Id 0x33, Welt in der Blende, Szenenende, Waffe-Rueckkehr; NUTZER-VORGABE an den Flag-Gegenstaenden.
- Suche im Diff gegen die Basis nach deferred, tunable, interim, for now, faithful, plausibel, TODO, FIXME, approx: 0 Treffer. RE15_USE_TEST und RE15_KEYITEMS_LOG sind Messhaken, kein Schalter als Abschluss. `keyitems_export.py --pruefen`: aktuell.

## Vertrag

`git diff f5a5e2f7e...8271c2726 --name-only`: 51 Dateien, kein release/, platform/android/, shared_assets/, pri/, synchro/, keine CMakeLists; neuer Testsatz in tests/unit/probes/r41_keyitems.cmake. Keine neuen Bits, keine neuen Nachrichten-Ids, (12,31) ist die Antwort-Flagge. Id 0x33/0x35 vergibt keine andere Spur (grep master, r41_nachtrag30, r41_alligator2, r41_bahnsteig). Gemeinsame Dateien: markierte Haken, je 1 bis 6 Zeilen.
Mit master ueberschneidet sich nur platform/pc/main.c, `git merge-tree` ohne Konfliktmarke. Die 27 Testdateien, die master seit der Basis aendert, beruehren die Stellen nicht.
Neue Tests: eigener Laufordner, exe-Kopie mit Praefix, erben keine RE15_*, libm auf Linux, file(COPY_FILE) wie 119 andere Skripte (Projekt verlangt CMake 3.21).
Nachgezogene alte Riegel gelesen: alle stellen auf USE um oder fuellen das Inventar fuer den Altstand, keiner wurde still abgeschwaecht.

## Schaerfe der Riegel (eigene Mutanten, ausserhalb des Baums)

Gemutierte Kopie von keyitems_r41.c bzw. scd_vm.c uebersetzt, gegen die fertige libre15_engine.a gelinkt ($TEMP/o41a1/mutanten.sh):
| Mutante | rote Teile von test_r41_keyitems |
|---|---|
| Basis (unveraendert) | keiner |
| plc_flags gibt 0 | kurbel |
| Sleep wartet | kurbel, waffe |
| Stelle trifft ueberall | use |
| anim_frac wie vor der Spur | kurbel |
| plc_ret gibt 0 | waffe |
| Frage bleibt an der Aktionstaste | aktion |
| kein Eintrag aus dem Flag | use, aktion, besitz |
| Waffe bleibt in der Hand | waffe |
| Leser-Text ohne Ende | aktion |
| kein Verbrauch | besitz, kurbel |
| volles Inventar egal | voll |

## Tests

Alle Laeufe am Endstand 8271c2726, keine volle Suite im Spurbaum, unter Last anderer Spuren.
- `ctest -R r41_keyitems -j 4`: `100% tests passed, 0 tests failed out of 21`.
- Breit: `^(unit_|prop_|probe_)` + 110 Integrationstests der Module und Raeume des Diffs, -j 5: `100% tests passed, 0 tests failed out of 1139`, 3331,83 s. Skipped: unit_r17_birkin_1zu1_pin, unit_r18_birkin_restluecken_pin, unit_r20_birkin_push_pin. Disabled: integration_r41_gatorszene_tod_r1, _elza (Spur P). Darin alle Riegel, die die Spur nachgezogen hat, ausser test_r41_ventilkette_weg.
- Eigener Querbezug (ungefahren.py, `ctest --show-only=json-v1`): 46 Integrationstests ausserhalb des Regex betreten einen betroffenen Raum und haben Eingaben (r35_entladen, r35_cut10f0, r37_beretta, r37_elzaems, r37_masken36_stage3..6, r38_cursor, r38_spinnen_echtlauf, r39_funk10f1, r39_johnruf1041_tuer, r39_stimmen, r41_kleinkram_a_lautstaerke, r41_spinnenanim), -j 3: `98% tests passed, 1 tests failed out of 46`.
- Ventil: `^integration_r41_ventilkette(_b|_alarm|_sperre|_kette)?$`, -j 2: `100% tests passed, 0 tests failed out of 5`.
- Zusaetzlich door_lock, keydoor, keypad, r34n_b_cursor, r36_elza_unlock, flag_census, r36_g2_cursor_rahmen, r37_sperre1050, r37_kiste2feld (Regex trifft 8): `100% tests passed, 0 tests failed out of 8`.

Das eine Rot, einzeln nachgefahren, deterministisch:
`integration_r37_elzaems_elza5091`: `finaler Birkin = RE2-EM36 (NUTZER-VORGABE) fehlt`. Nicht diese Spur: engine/src/birkin_formen.c:60 (aus der Basis, nicht im Diff) spawnt in 5090/5091 seit "Vierbein, dann G5" zuerst 0x30, das Skript im Zweig kennt das noch nicht. master 31745dd08 setzt dort RE15_ENDKAMPF_NUR_G5=1. Das master-Skript gegen diese exe: `finaler Birkin = RE2-EM36 (NUTZER-VORGABE): [enemy] FINALER Birkin: RE2 EM036 geladen`, `r37_elzaems[elza5091]: OK`. Erledigt sich beim Merge.

Nicht gefahren: integration_r41_ventilkette_weg und _weg_elza (je rund 2000 s). Abnahme 0 hat beide am selben Code gruen gemessen, der Vergleich ohne Kommentare oben belegt die Gleichheit, das Skript ist seit Abnahme 0 unveraendert. Ebenso nicht rund 130 Integrationstests ohne betroffenen Raum: das bleibt der vollen Suite bei der Integration.

## Maengel

Keine.

## Hinweise (kein Mangel, fuer die Release Notes oder den Nutzer)

- **H1 Stelle der Kurbel:** auf der Dreh-Lage selbst meldet USE "You can't use it here", es gilt das Original-Rechteck @0x009AE. Neu belegt: RE2 ROOM1110 hat dieselbe Form (Punkt 2), die Frage an den Nutzer ist damit nicht noetig.
- **H2 Altstand:** Flag gesetzt, Inventar voll, kein Eintrag: Frage und Fliegen bleiben, bis ein Platz frei ist (Lauf alt).
- **H3 Text in der Einblende:** die Welt laeuft an allen 14 Stellen in der Blende, gebraucht wird das nur an der Kurbel. "You've used ..." beginnt dadurch auf fast schwarzem Bild (s4000 F340).
- **H4 Karten an Tueren:** die Regel im Dossier ("Leser/Ereignis -> USE, Tuer-Satz -> Aktionstaste") passt auf 3010 nur aus Spielsicht: dort steht bis zur Benutzung ein Ereignis-AOT, erst danach der Tuer-Satz. Die Umsetzung ist richtig (RE1.5 fragt dort nicht, RE2 Lab Card Key auch nicht). Der Spieler sieht zwei Wege: 3010 Aktionstaste, 4000 USE.
- **H5 Sicherung aus 2030 am Rolltor:** USE am Rolltor 1050 nimmt den Eintrag, der Besitz-Takt legt ihn wieder an. Reicht fuer Rolltor und Generator, keine Sackgasse.
- **H6 Name:** Inventar "Timer Bomb" (DEBUG.BIN), Skripte und CHECK "Time Bomb"; beides ausgeliefert.
- **H7 Android und PSX:** neue Datei keyitems_r41.c, Memory `reai-v2-android-glob-cache` beachten (app/.cxx). Nur PC gebaut, Spurgrenze.
- **H8 Kasten 2010 bei vollem Inventar (neu, Lauf kasten_voll):** die Frage zum Herausnehmen (Port-Text 43) kennt das volle Inventar nicht: "You've taken the Fuse.", (9,125) = 0, kein Eintrag, also Altstand-Form bis ein Platz frei ist. Keine Sackgasse: die Frage bleibt an Kasten und Generator.
- **H9 Kartenleser ohne Karte (neu):** msg 8 "You have not the ... Keycard to operate it." ist nicht mehr erreichbar, die Aktionstaste zeigt in jedem Fall nur die Beschreibung. RE2-Form, fuer die Release Notes.

## Urteil

Alle drei Nutzer-Punkte sind an der exe erfuellt, M1 und M2 sind behoben, RE-Gate und Vertrag halten, die Riegel sind scharf. Bestanden.
