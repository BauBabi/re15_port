# Runde 41 Spur O "keyitems" - Abnahme 0

Stand: r41/keyitems 08b21b48e (Basis f5a5e2f7e). Bau: `local_build.sh configure` + `build` = `ninja: no work to do`.
Laufordner aller eigenen Messungen: `$TEMP/o41_abn0/run/<lauf>`, Kontaktbogen `$TEMP/o41_abn0/b_*.png`, Bilder RE15_FRAMEDUMP mit RE15_WINDOW_SCALE=1, exe je Lauf als Messkopie `re15_pc_o41a_<lauf>.exe`.

## Ergebnis

| Punkt | Urteil |
|---|---|
| 1 Kurbel: Leon fliegt nicht mehr an die Position | erfuellt |
| 2 Key Items, die keine Schluessel sind, ueber das Inventar | erfuellt |
| 3 Karten wie RE2 | erfuellt |
| RE-Gate | Adressen stimmen, Kennzeichnung Mangel M2 |
| Vertrag | haelt |
| Tests | **Mangel M1**: ein Riegel rot, Abschnitt Tests |

## Punkt 1: Kurbel

Wortlaut: "leon 'fliegt' quasi immer noch an die richtige Position bis das drehen dann los geht."

Lauf leon_echt (Leon, echte Tuer 2050 -> 2020, Handgun, Flag per RE15_SET_FLAG, START, ITEM, Kurbel, USE; Bogen b_leon_a.png, b_leon_b.png):
```
F255 USE  F256..F262 Blende aus  F263/F264 schwarz
[scd F264] Cut_chg(3)   [scd] Plc_motion(entity=0, motion=0, flags=0x40)
F264 PL(3650,-3018,rot=1024) pst=4 mo=0   F268 erstes volles Bild: Leon kurbelt
[scd F372] Cut_chg(4)  [scd F442] Cut_chg(3)  [keyitems] Waffe 3 wieder in der Hand
```
Lauf e_echt (Elza, RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1, Tuer 2051 -> 2021): dieselben Bildnummern, Lage (3650,-3018), Waffe 1 zurueck (b_elza.png).

Mass und Gegenprobe (Aenderung je Bild im Leon-Ausschnitt 100 x 130, Schwelle 24):
- Altstand mit derselben exe (Inventar voll, Frage + Ja, Lauf alt_stand): F402..F407 = 663, 710, 463, 520, 406, 260 Pixel, danach 90..150. Das ist das Fliegen, der Bogen b_alt.png zeigt es, das Mass sieht es.
- USE aus dem Stand (u_stand): ab dem ersten vollen Bild 67..171 Pixel, keine Spitze.

Gleiches Bild bei jedem Anlauf, Bild N nach Plc_motion gegen u_stand, Leon-Ausschnitt, N = 4..32 (byteweise):
| Lauf | Anlauf | verschiedene Pixel |
|---|---|---|
| u_renn | Rennen nach Westen | 0 von 13000 |
| u_geh | Gehen nach Westen | 0 |
| u_west | Stand, Blick West, vorher Kamera 0 | 0 |
| u_nord | Stand, Blick Nord, vorher Kamera 0 | 0 |
| u_ost | Stand, Blick Ost, vorher Kamera 0 | 0 |
| u_ziel | R1 gehalten (Zielhaltung) | 0 |
Vollbilder N = 0..4 aus Kamera 0 (u_west, u_nord, u_ost): 0 Pixel, das alte Bild blitzt nicht auf.

Szenenende u_stand: Schnitt F512, Aenderung F513 10022 (der Schnitt), F514 46, F515 25, ab F516 0: kein Kippen.

Warum das den Befund erklaert: die Blende @0x80050cfc (+0x8f := 7) mischt die Stehpose in den Kurbel-Schluessel (Altstand F402..F407), davor stand Leon 20 Takte (Sleep @0x0A98) still. Beides faellt im USE-Lauf weg; Pos_set und Plc_motion fallen in dasselbe schwarze Bild.

## Punkt 2: Key Items ueber das Inventar

Wortlaut: "... weil man EXPLIZIT die Items im Player Inventory auswaehlen muss und dann genau richtig steht bei der Nutzung. Das moechte ich bei Key Items, die nicht Schluessel sind bei unseren Port auch haben!"

Eigener Zensus (zensus_fragen.py, alle Message_on-Ziele mit 0x03-Code in 206 RDTs): 12 Skript-Stellen mit Gegenstand plus die Port-Stellen 1050 und 2010 = 14, deckt sich mit `k_stellen`. 40A0 msg 4 und 4001 msg 1 haben kein Message_on (selbst gezaehlt), keine Stelle. Item_aot_set: 0x33 und 0x35 nie platziert.

Echtes Menue an der exe (START, ITEM, Platz, USE), je Stelle eigener Lauf:
| Stelle | Lauf | Log / Bild |
|---|---|---|
| 1090 Loescher | s1090 | Frage 8 entfaellt, msg 9, Wegwerf-Abfrage, Szene |
| 10D0 Blue Keycard | kton | Frage 7 entfaellt, msg 9, auf.log `art=KARTE_EIN` |
| 11E1 Yellow Keycard, Elza | e11e1 | msg 9, Tastenfeld |
| 11E0 Zange | s11e0z | Frage 11 entfaellt, Panel |
| 1100 Minidisc | s1100 | Frage 3 entfaellt, Panel Cut 12 |
| 1050 Sicherung | s1050 | Frage 20 entfaellt, msg 21, `Sce_item_lost(0x40)` |
| 2010 Sicherung | s2010 | Frage 41 entfaellt, Eintrag weg, Standbild, msg 42 |
| 2020 / 2021 Kurbel | leon_echt, e_echt | siehe Punkt 1 |
| 2040 Bombe | b3 | Frage 5 entfaellt, Feld, Schalter, msg 4, Eintrag weg |
| 2060 Sicherung | s2060 | Frage 4 entfaellt, Panel Cut 8 |
| 3050 / 3051 | s3050, e3051 | Frage 4 entfaellt, Panel |
| 4000 | s4000 | Marker-Weg, msg 2, Wegwerf-Abfrage |
| 4030 / 4031 | s4030, e4031, kerze | Marker-Weg, Szene, Kerze danach nicht im Inventar |

Aktionstaste (a4000, a11e0z, kaktion, Riegel aktion): nur Beschreibung, keine Auswahl, wiederholbar; Karten-Leser endet vor "Will you operate the card reader?" (b_kakt.png).
USE an falscher Stelle: Riegel falsch, Lauf vor_kurbel: "You can't use it here.", Inventar bleibt offen.
Inventar voll (voll2010, b_voll.png): Modal "You can't carry any more items", Flag bleibt 0, Kurbel bleibt liegen.
Name und CHECK: Crank, "The end is / square-shaped" (b_auf.png), Bombe b_bname.png, O_belege/nachher_check_bombe.png. `keyitems_export.py --pruefen`: aktuell.
Kombination: Eigenschaftssatz 0x33 bis 0x35 ohne Paare (@0x8007500c ff., selbst gelesen).

"Genau richtig stehen": Cut 3 steht bei (10456,-6184,956) und blickt nach Westen. (2914,-3388) und (3650,-3018) liegen auf derselben Sichtlinie: Spalte 88,7, Fuss Zeile 146,9 gegen 156,2 (proj.py aus dem Kamera-Satz ROOM2020 @0x60). Die 870 Einheiten liegen in der Blende, das Bild danach ist aus jeder Lage dasselbe. Siehe Hinweis H1.

## Punkt 3: Karten wie RE2

Wortlaut: "Schaue wie es in Resident Evil 2 gemacht wird, und mache es genauso."

RE2 selbst gelesen:
- Leser: Red Card Key ROOM2110 @0x01812 `2c 09 06 21 .. 0a 00 34 00 01 00` (sce 6), @0x01AD8 `23 00 02 00 34 00`, @0x01AEA `62 34`, Aktionstaste @0x017FE sce 5 Text.
- Tuerschloss: ROOM6140 @0x00BB2 `.. 97 61 00`, ROOM6060 @0x00E36 `.. 98 62 00`, ROOM7000 @0x0143A `.. 9a 63 00`; Handler @0x80051628 `jal 0x800696cc`, @0x8005164c `jal 0x8002fe38` a2 = 5.
Port: Leser 10D0, 11E0, 1230, Computer 3050, Panel 4000 = USE (gemessen, Tabelle oben). Tuer 3010 = Aktionstaste ohne Frage (@0x02302 `22 03 3c 01 2b 01 ff ff 02 00 47 02`, unveraendert). Die Wegwerf-Abfrage geht weiter (s4000, s1090, Riegel). Siehe Hinweis H4.

## RE-Gate

Selbst disassembliert (Binaerdatei richtig, Sprungziel gelesen):
- RE1.5 PSX.EXE @0x80050cf8 `ori v0,zero,0x7`, @0x80050cfc `sb v0,143(v1)`, @0x80050d1c `lhu v0,452(v1)`, @0x80050d24 `andi v0,v0,0x40`, @0x80050d30 `sb zero,143(v1)`.
- Op 0x3F = 0x80041b90 (Tabelle 0x800744a8[63]): @0x80041b9c `lhu a1,2(v0)`, @0x80041bac `srl a1,a1,8`, @0x80041bc8 `sh a1,452(v0)`: pc[3] ist +0x1C4, die Deutung von Bit 0x40 stimmt.
- @0x80042bd0 `ori v0,zero,0x26c`, @0x80041f90 `sb v1,4(v0)`, @0x80042cac, @0x80042ea8, @0x80042ef8, @0x80042f3c, @0x8001cc34.
- RE2 @0x8006b21c, @0x8006b220, @0x8006b248, @0x8006b2e0, @0x8006b2ec, @0x80068cfc/d04/d0c, @0x80068d34, @0x80025cb0, @0x80052e38, @0x800537a0, @0x800585e4.
- RDT-Bytes: RE2 ROOM1110 @0x02FC1 `3f 00 0f 45`, @0x020A0, @0x020B4, @0x02F24, @0x02F4E, @0x02F66, @0x02336; ROOM2020 @0x0A5C..@0x0B16, @0x009AE; ROOM2010 @0x008CA; ROOM1110 @0x00D32; ROOM2030 @0x01FF2; ROOM2060 @0x0169E; ROOM2040 @0x01624; ROOM4030 @0x0068C.
- Zensus Plc_motion: 206 RDTs, 1359 Stueck, 0 mit Bit 0x40.
- ITPS: 53 Bloecke (0,489)/(832,256), Block 0x35 (0,480)/(0,0): die Rechtecke im Code sind belegt.
Suche im Diff nach deferred, tunable, interim, for now, faithful, plausibel, TODO: kein Treffer. Kein Env-Schalter als Abschluss: RE15_USE_TEST und RE15_KEYITEMS_LOG sind Messhaken.
Der Fix erklaert den Befund (Punkt 1).

## Vertrag

`git diff f5a5e2f7e HEAD --name-only`: kein release/, platform/android/, shared_assets/, pri/, synchro/, keine Test-CMakeLists; neuer Test in tests/unit/probes/. Keine neuen Bits, keine Nachrichten-Ids (nur (12,31)). Gemeinsame Dateien mit markierten kleinen Haken. Test parallel-fest, erbt keine RE15_*-Variablen, cmake_minimum_required vorhanden, libm auf Linux.

## Tests

Alle Laeufe am Endstand 08b21b48e, keine volle Suite im Spurbaum.
- `ctest -R r41_keyitems -j 5`: `100% tests passed, 0 tests failed out of 21`.
- Breiter Regex (Regex des Bau-Agenten + dachkiste, masken36_nb2, rekorder, g2_cursor, story, sperre1050, keypad, keydoor, tueren, scd, plc, cut, szene, motion, anim, ohne ventilkette), -j 5, unter Last anderer Spuren: `99% tests passed, 5 tests failed out of 441`, 2 Disabled (gatorszene_tod_r1/_elza gehoeren Spur P).
- `-R "^(unit_|prop_|probe_)" -j 5`: `99% tests passed, 2 tests failed out of 1031`, 3 Skipped.
- Zusaetzlich `integration_r33_speichern`, `r37_kiste`, `r37_kiste2feld`, `door_lock`, `r34n_b_cursor`, `r38_cursor`, `r36_elza_unlock`, `flag_census`, `elza_vollstart`, `cross_stage`, `r37_fahrstuhl`, `headless_run`: `100% tests passed, 0 tests failed out of 12`.
- Ventilkette seriell am Endstand: `weg` Passed 2016 s, `weg_elza` Passed 2478 s, `ventilkette` Passed, `_b` Passed, `_alarm` Passed, `_kette` Passed, `_sperre` Passed (die letzten acht einzeln: `100% tests passed, 0 tests failed out of 8`). Das schliesst die Luecke des Bau-Agenten (seine vier Laeufe lagen vor der Zwei-Sicherungen-Regel).

Rote Laeufe, einzeln nachgefahren:
| Test | breit | einzeln | Bewertung |
|---|---|---|---|
| unit_r36_tueren_aufschliessen | Failed | Failed, Basis-Build GRUEN | **Mangel M1** |
| integration_r36_dachkiste | Timeout 2700 s | Passed 1735 s | Last |
| integration_r38_rolltor20a0 | Timeout 1200 s | Passed 271 s | Last |
| integration_r38_rolltor20a0_abe | Timeout 1200 s | Passed 191 s | Last |
| integration_r38_dining1141_s | Failed "576 fehlen" | Passed 643 s | Last |
| unit_r35_android_bash_mutanten | Failed, Kontrolle 124 | Passed 585 s | Last, keine Datei der Spur |

Nicht gefahren: rund 190 Integrationstests ausserhalb aller Regexe (Waffen, Gegner, Masken, Karte). Das ist die Grenze "keine volle Suite im Spurbaum".

## Maengel

**M1 (blockiert die Suite): Riegel `unit_r36_tueren_aufschliessen` ist rot und wurde nicht nachgezogen.**
Ausgabe: `FEHLER: ROOM10D0 Kartenleser Ja (KARTE_EIN): 0 Toene statt 1`, `aufschliessen: ROT (1 Fehler)`, deterministisch; derselbe Teil im Basis-Build: `aufschliessen: GRUEN (0 Fehler)`.
Ursache: tests/unit/probe_r36_tueren.c Teil 5b' feuert `re15_aot_fire_slot(1)` und bestaetigt mit Quadrat die Frage in msg 7, die es seit dieser Spur nicht mehr gibt. Das Spiel ist richtig: Lauf kton (echtes Menue) `[msg] room=10d0 id=9` und auf.log `F334 raum=10D0 nachricht=9 art=KARTE_EIN satz=0(KARTE_EIN) bild=+0 nr=1`.
Zu tun: Teil 5b' auf USE umstellen (Blue Keycard ins Inventar, `re15_keyitems_r41_testlauf(0x38)`), Erwartung bleibt ein Ton Satz 0 bei Bild 0; Kopfkommentar anpassen. Kein Engine-Code.

**M2 (Kennzeichnung): zwei PORT-WAHL stehen nur im Dossier.**
menu_common.c close_phase `s_stage = 0` weicht von RE1.5 @0x8001cc34 und RE2 @0x80025cb0 ab (dort steht das Skript in der Einblende) und traegt im Code weder Adresse noch das Wort PORT-WAHL. scd_vm.c op_plc_ret `anim_frac = 0` traegt Adressen, aber nicht das Wort. Beide Kommentare ergaenzen.

## Hinweise (kein Mangel, fuer die Release Notes oder den Nutzer)

- **H1 Stelle der Kurbel:** Lauf vor_kurbel: auf der Kurbel-Lage selbst (3650,-3018), Blick Nord, meldet USE "You can't use it here.", die Aktionstaste zeigt dort nichts (Lauf aktion_vor). Es gilt das Original-Rechteck @0x009AE, wie im Auftrag vorgeschlagen, und die Rueckmeldung ist einheitlich: wo "It's a square hole." kommt, geht USE. Ob USE auch direkt an der Kurbel gehen soll, entscheidet der Nutzer.
- **H2 Altstand:** Flag gesetzt, Inventar voll, kein Eintrag: Frage und Fliegen bleiben, bis ein Platz frei ist (Lauf alt_stand). Nur alte Spielstaende.
- **H3 Text in der Einblende:** die Welt laeuft bei allen 14 Stellen in der Blende, gebraucht wird das nur an der Kurbel. "You've used ..." beginnt dadurch auf fast schwarzem Bild (s4000, s1050 F335).
- **H4 Karten an Tueren:** 3010 bleibt Aktionstaste (RE2-Form Lab Card Key), 4000 ist USE (Panel, im Original mit Frage). Beide haben ein RE2-Vorbild; die Regel ist am Text des Raums belegbar, aber der Spieler sieht zwei Wege.
- **H5 Sicherung aus 2030 am Rolltor:** Lauf r2030: USE am Rolltor 1050 nimmt den Eintrag, der Besitz-Takt legt ihn wieder an, weil (3,108) gilt. Die Sicherung reicht dann fuer Rolltor und Generator. Keine Sackgasse.
- **H6 Name:** Inventar "Timer Bomb" (DEBUG.BIN), Skripte und CHECK "Time Bomb"; beides ausgeliefert.
- **H7 Android:** neue Datei keyitems_r41.c: Memory `reai-v2-android-glob-cache` beachten (app/.cxx). Nur PC gebaut.

## Urteil

Alle drei Nutzer-Punkte sind an der exe erfuellt. Nicht bestanden wegen M1: ein durch diese Spur roter Riegel blockiert die volle Suite. Die Nachbesserung ist klein und beruehrt nur Test und zwei Kommentare.
