# Runde 41 Spur N "nachtrag30" - unabhaengige Abnahme 0

Baum `.claude/worktrees/r41_nachtrag30`, Zweig `r41/nachtrag30`, Kopf 9d8b9ef68, Merge-Basis 6e213a5a5. Datum 2026-10-31.
Bau: `local_build.sh configure` + `build` exit 0, "ninja: no work to do", exe 00:21:06 >= letzte Code-Aenderung 00:20:59.
Messordner (nicht committet): scratchpad `n/`, Laeufe `n/l/<name>/`, Treiber `n/run.py` + `n/batch*.py`: eigene exe-Kopie `re15_pc_abn0n_*`, eigener Laufordner,
Renderer `direct3d` (kein RE15_SOFTWARE_RENDER), `RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_MSG_LOG=1`, Bilder per RE15_FRAMEDUMP. Jeder Lauf unten ist mit seinen Variablen genannt und wiederholbar.

## Ergebnis

**NICHT BESTANDEN.** Punkt 2 (Schrank) und Punkt 1 (Wasserlauf) sind nur teilweise erfuellt, dazu zwei Nebenwirkungen (Mangel 3, 4). Punkte 3 bis 7 halten dem Wortlaut stand. RE-Gate und Vertrag halten.

| Punkt | Urteil | Kern |
|---|---|---|
| 1 Wasser 2010 | teilweise | Leiterstrom ja, Ende mit Bit 100/103 ja, Raum begruendet; der "Weg" ist ein stehendes dunkles Rechteck mit einzelnen Punkten, es laeuft dort nichts |
| 2 Schrank 2010 | teilweise | Aufklappen und Bild ja; "Mechanismus gleich" nein: der Cursor-Schritt von 2060 fehlt, und keine Spur baut ihn |
| 3 Panel 2030 | erfuellt | Entwurf A wie freigegeben, Regeln, Text 43, Fahrt; Nebenwirkung START (Mangel 3) |
| 4 Notiz | erfuellt | Satz wortgleich, Plakat 30d, Folge 09/02/01/08 |
| 5 Bruecken-Ton | erfuellt | RE2 ROOM4080 sub13, Motor bis zum Halt |
| 6 Alarm | erfuellt | Sirene nach der Ansage, durchgehend, Quelle belegt |
| 7 Lautstaerke | erfuellt | jeder Abtastwert folgt, auch im echten Rueckruf |

## Maengel

1. **Punkt 2, Mechanismus nicht gleich (nicht erfuellter Wortlaut).** "Ab da soll auch der Mechanismus gleich - das Einsetzen der Sicherung etc." Gemessen in 2060 (Lauf `m2060`, `m2060c`, Originalskript im Port):
   Frage -> Ja -> F476 Cut 8 mit CURSOR obj 4 (`prop-render oid=0x04`), der wartet ohne Zeitgrenze; SQUARE neben der Zone tut nichts (F522/F585/F648, bis F720 weiter Cut 8); Steuerkreuz RECHTS (sub16 @0x01660, +200) bringt obj 4 in Zone 7
   (Aot_set Slot 7 @0x0100C, rect (19700,-24300,2050,2050), Cursor startet bei x 19571), SQUARE (@0x0132E) -> sub19: F619 Cut 9 + Text 5 "You've used the Fuse.", F713 Cut 10.
   In 2010 (Lauf `s_ja`): Ja -> nach F600 Cut 8 ohne Cursor, 30 Bilder -> nach F630 Cut 9, 30 Bilder -> Standbild der Ventile, ohne eine Eingabe. Der Bau-Agent nennt das selbst als offen und als Frage an den Nutzer; `O_keyitems.md` (Stand r41/keyitems f53f2cfec) enthaelt keinen Cursor-Schritt fuer 2010: der Punkt hat keinen Eigentuemer.
   Nachpruefen: `RE15_DEBUG_JUMP=2060@250 RE15_PLAYER_POS=-4000,4450,2048,0 RE15_SET_FLAG=3:108`, Skript `A0.1,W1.5,A0.1,W1,A0.1,W2,A0.1,W1.5,A0.1,W2,R0.1,W1,A0.1,W3` gegen `RE15_DEBUG_JUMP=2010@250 RE15_PLAYER_POS=4950,-2770,0,0 RE15_SET_FLAG=3:108`, `RE15_PRESS=square@300,square@368,square@418,square@468,square@538,square@618`.
2. **Punkt 1, "ueber den Weg bis zum Wasser Pool ... laufen": es laeuft nichts.** Lauf `wzan`/`wzaus` (Cut 10, Spieler weg, F430..F460 je 6 Bilder): die nasse Spur ist ein dunkles Rechteck mit harten Kanten (Bild `n/wz_crop.png`), mittlere Farbe (30,36,29). Innerhalb der Flaeche x 72..104, y 154..170 aendern sich je 6 Bilder nur 9 bis 91 von 512 Bildpunkten, das sind die einzelnen Spritzer-Punkte; die Flaeche selbst steht. In Cut 9 ist sie hinter der Figur kaum zu sehen (Lauf `wgeh`, F180..F240), in Cut 8 und 13 gar nicht. Der Leiterstrom ist dagegen klar da. Die Riegel messen nur Helligkeit gegen den Lauf mit Bit 100 (Weg 8,0 Graustufen), also nur "dunkler", nicht "fliesst".
3. **Panel: START oeffnet ein unsichtbares Inventar.** Lauf `p_st` (Panel offen, Skript `...,S0.1,W3,S0.1`): F759 `[r38q] Inventar-Ton Satz 6 (Oeffnen)`, `[inv] Status-Bild ...`; die Bilder F810 zeigen weiter das Panel, das Inventar liegt unter der Vollbild-Ebene und nimmt die Tasten; F852 `Inventar-Ton Satz 5 (Abbruch)`. Das Panel laeuft ohne die Sperre (2,7) (Dossier §2), deshalb kommt START durch. Kein Haenger, zweites START schliesst. Das Plakat ist nicht betroffen (Sperre (2,7)).
4. **Schrank: die offene Tuer steht in Cut 4 als grobe Flaeche im Bild.** Lauf `s_geh2` (`RE15_DEBUG_JUMP=2010@250`, Wegplan zu Fuss vom Spawn zum Kasten, Text weg, zurueck): die Kamera wechselt selbst auf Cut 4, ab F660 deckt der Fluegel die Spalten 0..86 ueber die ganze Hoehe (16505 von 28800 Bildpunkten der linken Haelfte, Vergleich `s4/f_000290`), die Textur ist grob und verschmiert (Bild `n/s4zoom.png`). Die Aussenseite ist BG03 aus der Sicht von Cut 3 zurueckprojiziert und passt nur dort. Die Tuer bleibt bis zum Raumwechsel offen, jeder Gang weg vom Kasten laeuft durch dieses Bild. Im Dossier unter OFFEN nicht genannt (dort nur die fehlende Kollision).
5. **Kosmetik Panel (kein Grund zum Durchfallen):** Farben gegen `panel_entwurf_a.png` verschoben (Lauf `p_a`, gleicher Stand 50/20/30, Quelle C): EXIT-Pfeil Entwurf RGB (82,107,88), Spiel (91,98,96), der gruene Pfeil wird grau; Fenster Tank A (141,150,108) gegen (152,148,131). Sonst deckungsgleich (mittlere Abweichung 2,2).

## Punkt 1: Wasser 2010

**Wortlaut:** "in room 2010 soll - bis zum ablassen der Wassers, auch ein Wasser Effekt die Leiter runter kommen und ueber den Weg bis zum Wasser Pool in der Mitte laufen." Nutzer-Wahl: bis zur Flut in 2030.

* **Raum selbst geprueft.** `scd_dump_room.py ROOM2010.RDT`: eine Tuer (Slot 0 @0x00746 nach 2000), Items, kein Sce_espr_on, kein Objekt Typ 3. Alle 6 Cuts angesehen (Laeufe `s0`..`s5`, `n/s_alle.png`): Buero, Cut 5 = Bildschirm. ROOM2010 hat weder Leiter noch Becken, der Wortlaut ist dort nicht baubar.
* **ROOM2000 passt.** Alle 16 Cuts mit/ohne Bit 100 (`w0an`..`w15aus`): Kanal in der Mitte, Stege seitlich. Zwei Leitern: Sued z -24700 (Slot 1 @0x0163E nach STAGE1) und Nord z 9800 (Slot 4 @0x0169E nach 2050, gesperrt "water pressure"). Gebaut an der Nordleiter neben dem Gitter-Wasserfall: richtig. Die Suedleiter in Cut 0/1/2/3/4/11/12 bleibt trocken, das ist nicht die gesperrte Leiter.
* **Sichtbar:** Differenz > 16 Graustufen gegen Bit 100: Cut 10 2677 Bildpunkte, Cut 9 274, Cut 8 126, Cut 13 70, sonst 0 (Cut 2: 3). Zu Fuss durch die echte Tuer 2010 -> 2000 (`wgeh`, `RE15_FIRE_AOT=0@300#2010`, Wegplan): `[wasserlauf41] ROOM2000: 6 Strahlen, nasse Spur Prop 3`, Kamera wechselt selbst Cut 14 -> 9 -> 10, der Strom haengt an der Leiter (`n/wgeh.png`). Nach 3000 Bildern noch da (`wlang`).
* **Ende:** Bit 100 und Bit 103 (`w103`): kein Log, 0 Bildpunkte Differenz. Leon/Elza + CONTINUE: Riegel `integration_r41_nachtrag30_wasser_*`, `_laden_*` gruen.
* **Effekt-Pool:** 69 von 96 Plaetzen belegt gegen 29 ohne Wasserlauf (`fxan`/`fxaus`, RE15_WAFFEN_LOG). 27 frei, kein Befund.
* **Weg:** Mangel 2. Rueckfrage "2001 statt 2010?" steht im Dossier, die Herleitung ist belegt.

**Urteil: teilweise.**

## Punkt 2: Schrank 2010

* **Aufklappen:** `s3` (Cut 3): Tuer auf, `[schrank2010] Tuer auf (rot_y 1024)`, dann Text 40. Zu Fuss vom Spawn (`s_geh2`): Kamera selbst Cut 4 -> 3, dann Klick, kein erzwungener Cut.
* **Bild:** `n/s3zoom.png` neben fuse_box.bmp: Innenseite und Tuerinnenseite mit Aufklebern wie der Kasten aus 2060. Erfuellt fuer Cut 3. fuse_box.bmp selbst gegen die BSS gemessen (`test_r41_ventilkette bssdiff`): BG06 92 % gleich, Abweichung 3; BG10 92 %; BG08/BG03/BG00 10..13 %. Der Rest ist die rote Umrandung des Nutzers.
* **Nahansichten:** Riegel `_schrank_*`: F635/F665 == BG08/BG09. Stimmt, aber als Standbilder (Mangel 1).
* **Quelle:** ROOM2060 @0x0168C `29 08`, @0x016A2 `29 09`, @0x016C5 `4b 06 0a`, @0x01324 `3e 00 0f 00 07 00`, @0x0132E `51 01 40 00` selbst gelesen. ROOM11F0 @0x01332 `2f 05 40 00`, @0x01336 `0d 00 04 00 10 00` selbst gelesen.
* Keine Kollision am Fluegel: Figur geht hindurch (`s_geh2` F1005/F1095), im Dossier genannt.

**Urteil: teilweise** (Mangel 1, dazu Mangel 4).

## Punkt 3: Panel 2030

* **Oberflaeche:** Lauf `p_a` gegen `panel_entwurf_a.png`, Stand 50/20/30, Quelle C: deckungsgleich, Bild `n/p_cmp.png`; Farben Mangel 5. Fensterskala 3 (`p_sk3`): Text ueber dem Band, sauber.
* **Regeln:** Log `p_a`: `offen: A 100 B 0 C 0`, A->B `A 50 B 50 C 0`, B->C `A 50 B 20 C 30`. `t_panel` (mit Ton): weiter C->A `A 80 B 20 C 0`, `GELOEST`, Text 43, Ja, Motor F1250, Halt F1310. `p_vl`: B->A erlaubt. `p_voll`: Text 47, Quelle bleibt. `p_bed`: Text 42; die Taste, die die Nachricht schliesst, loest nichts Neues aus.
* **Verlassen:** `p_ex`: EXIT mit dem Cursor bei (258,194), Figur geht wieder, Wiederoeffnen steht auf 100/0/0, CROSS `Abbruch`.
* **Lebende Spinnen:** `p_sp` ohne Kill-Bits: Treffer F122 und F250 (`[giftkugel]`) vor dem Pult, ab Panel offen (F474) 40 s lang keiner mehr. Bits (2,0)/(2,2) halten die KI.
* **Elza:** Lauf `e_p`: ROOM2031, gleiche Folge bis Text 44, Motor F1250, Halt F1310. Schrank `e_s` (ROOM2011) und Wasserlauf `e_w` (ROOM2001, 6 Strahlen) ebenso, Bild `n/e_all.png`.
* **Echte Wege:** Riegel `_laden_*` (CONTINUE, Ring-Tuer 2070 zu Fuss) gruen, Leon + Elza. Ring-Tuer @0x00A5A `3b 01 02 31 ... 96 19 e8 ea 9c ff` = (6550,-5400,-100) selbst gelesen.
* **Toene im Panel:** `t_panel`: `se=10` je Klick, `se=12` bei GELOEST, geladen und gespielt.

**Urteil: erfuellt**, Nebenwirkung Mangel 3.

## Punkt 4: Notiz

* FILE31 selbst dekodiert (`n/tim2png.py`): Seite 1 endet "They mixed up / green and red, and on top of", Seite 2 `that they swapped the / numbers of "black" and / "green"! What are they? ...`. Wortgleich Nachtrag 30b, kein "black" mehr in der Vertauschung, Anfuehrungszeichen sichtbar. Im Leser: `N_belege/notiz_seite3.png`, Riegel v2d.
* Plakat im Spiel (`n_pl` Leon, `n_pl_e` Elza, `n/n_pl.png`): 1. GREEN 01, 2. RED 02, 3. BLACK 09, 4. RED 08.
* `RE15_R41C_LOESUNG {5,1,0,4}` auf {1,2,3,7,8,9} = 09, 02, 01, 08. Farben {2,1,0,2,1,0} = 1 schwarz, 2 gruen, 3 rot, 7 schwarz, 8 gruen, 9 rot, wie der Nutzer sie in 30c aufzaehlt.

* Folge am Spiel: Riegel `_weg`, `_weg_elza`, `_kette`, `_alarm` gruen (eigene Laeufe): `Folge 09 02 01 08 -> RICHTIG`, `Folge 01 02 09 08 -> FALSCH: zurueckgesetzt` + Text 45.

**Urteil: erfuellt.** (Kopfkommentar `test_r41_ventilkette_weg.cmake` Zeile 19 nennt noch die alte Plakatfolge; der Testcode ist richtig.)

## Punkt 5: Bruecken-Ton

* RE2 ROOM4080 selbst dumpt (`re2_scd_dump.py ROOM4080 MAIN 13`): @0x031F2 Se_on 0x0F vor der Fahrt, @0x03314 Se_on 0x10 am Ende, danach Spritz-Effekte 0x1A: die Bruecke landet im Wasser. sub13 startet bei y -9000 (@0x031FE) und endet bei y -1800 (@0x03314): senken; sub12 faehrt umgekehrt und endet mit Satz 0x0E. EDT @0xA3FC `00 01 53 16`, @0xA400 `00 01 64 16`.
* `re2_toene_export.py --pruefen`: EDT/VH/VB bytegleich mit der RDT, Offsets 0x0A3C0/0x0A480/0x0B2A0. Op 0x8C: 0x800593e4 `jal 0x800395b8` selbst disassembliert.
* Echte Folge `t_panel`: F1250 Satz 0x0F, F1310 Satz 0x10, je mit Stimme 6. Riegel `_ton` eigener Lauf gruen (Motor 60 Bilder, vorher/nachher bitgleich).

**Urteil: erfuellt.**

## Punkt 6: Alarm

* ROOM6000 am Spiel (`b6000`): `[bgm] stage=5 room=00 entry=FF65 -> MAIN25`, `Sce_bgm_control ... vol=70`. Tabelle PSX.EXE 0x80074828[0x62] = 0xFF65, DAT_800748fc[5] = 0x62 selbst gelesen.
* MAIN25 == RE2 MAIN11 (Pruefung), SUB_34 VAG 1/2 == MAIN25 VAG 1/2 (eigener Parser `n/sub34.py`, sha1 52678419f9ef / 9d010af0e8e9). SUB_34 == info/re2leon/COMMON/SOUND/SUB_34.BGM (cmp). SEQ selbst gelesen: Kanal 1 Note 60 Programm 1, Kanal 2 Echo, Kanal 3 Brummen, Kanal 0 VAG 1 (0,9 s Toene 1,5..2,3 kHz, keine Sprache), Ende 14,3 s `b0 63 1e`.
* Mitschnitt des eigenen `_ton`-Laufs (`n/cap.py`): an capTick 674, aus 2724; in der Minute RMS mit Sirene 2444, ohne 1030, Differenz 2216, also deutlich ueber dem Rest. Riegel: keine Luecke.
* Reihenfolge am Log des eigenen `_alarm`-Laufs, Leon: F4227 SUB_25 an + Text 46, F4680 Sirene AN (Text geschlossen), `FLUT: Wasser steigt`, F6480 SUB_25 aus und Sirene AUS im selben Bild; Elza (ROOM2031): dieselben Bilder. Die 453 Bilder zwischen Text und Sirene sind die Wartezeit des Wegplans bis zur Taste, die Sirene haengt am Schliessen des Texts.
* "waehrend der Alert message" laeuft SUB_25, die Sirene setzt nach der Ansage ein. Das folgt Nachtrag 36 ("nach der Ansage").

**Urteil: erfuellt.**

## Punkt 7: Lautstaerke

* Eigene Auswertung der Mitschnitte `_laut`: 2 499 000 Werte, 0 falsch fuer 50 (q 0x2000) und 10 (q 327), RMS-Verhaeltnis 0,25000 und 0,00998, Spitze 16681 -> 4170 -> 166. Log: `[tuer] Sequenz Archiv 2 DOOR26`, Schliesston, Sprung ins Becken.
* Echter SDL-Rueckruf mit Dummy-Treiber (`rv100`/`rv10`, RE15_AUDIO_CAP, SDL_AUDIODRIVER=dummy, gleicher Weg): RMS-Verhaeltnis 0,00999, Spitze 17366 -> 173.
* Nur ein Ausgabeweg: grep SDL_OpenAudio/QueueAudio/PlaySound/waveOut/Mix_ = audio_pc.c:2005. Endstufe audio_pc.c:528 hinter der Mischung.
* Standard 50 ohne Datei: `re15_lautstaerke_r41_laden` -> `_reset` -> `_voreinstellung`; Riegel `vfehlt`. Der Messhaken (CAP ohne RE15_OPTIONS_FILE = 100) ist gekennzeichnet und aendert keinen Spielweg.

**Urteil: erfuellt.**

## Gates

**RE-Gate.** Grep im Diff nach deferred/tunable/interim/for now/faithful/plausibel/TODO/geschaetzt: 0 Treffer. Neue getenv nur RE15_SE_DEBUG (Log) und der Messhaken. Stichproben selbst disassembliert/gelesen:
1. PSX.EXE @0x800456b8 `lui a0,0x605`, @0x800456bc `ori a0,a0,0xd00`, Sprung @0x800456d4 nach @0x80045710 `jal 0x80019700`: Code 0x06050D00 wie zitiert.
2. PSX.EXE @0x800456f0 `lui a0,0x603`, @0x800456f4 `ori a0,a0,0x2000`: Sub 3 wie zitiert.
3. RE2 PSX.EXE @0x800593e4..@0x8005940c `jal 0x800395b8`: Vibration wie zitiert, kein Ton.
4. RDT-Bytes alle wie zitiert: ROOM2000 @0x0174C/5C/6C `3a 00 0b 00 00 00 00 19 ...`, @0x0169E, @0x016E6/@0x01708/@0x0172A; ROOM2010 @0x00746; ROOM2060 siehe Punkt 2; ROOM2050 @0x021FA, @0x0240C, `2f 02 c8 00` liegt @0x023C6 in sub05 ab @0x023C2; ROOM2080 @0x00CD8, @0x00C2E; ROOM6000 @0x00D52; RE2 ROOM2130 @0x01752 `23 00 05 00 50 00`, @0x0171C `09 0a 1e 00`, @0x01762, @0x01192, @0x01800; RE2 ROOM6040 @0x02360.
Port-Wahlen sind als solche benannt (Skala, Spritz-Takt, Lage, 30 Bilder je Nahansicht, Tankwerte = NUTZER-VORGABE). **Haelt.**

**Vertrag.** `git diff 6e213a5a5..HEAD --name-only`: 65 Dateien, nur analysis/, engine/include/platform/pc, tests, tools, shared_assets/RE2. Keine release/, android/, shared_assets/PSX, pri/, synchro/, keine Test-CMakeLists. Bits: keine neuen (101 umbenannt, k_port[] nachgezogen). Ids 2030 40/41/42/47 aus der Zuteilung. Bank 5 in 2010/2011/2030/2031: 0 Treffer im Raumskript. audio_pc.c +23 Zeilen statt 1-5, benannt, Muster der Spur C. Paket-Gate fuer SUB_34.BGM, BRUECKE4080.* steht im Dossier, make_package.sh gehoert der Integration. **Haelt.**

**Merge.** `git merge-tree HEAD master`: konfliktfrei (29 Commits voraus). Gegen r41/keyitems: genau ein Konflikt `test_r41_ventilkette_weg.cmake`, wie vom Bau-Agenten vorhergesagt.

**Tests (eigene Laeufe, kein voller Suite-Lauf).** `ctest -R r41_nachtrag30`: 11/11 gruen. Weg-Riegel: `integration_r41_ventilkette_weg` und `_weg_elza` gruen (1452 s / 1676 s), dazu `integration_r38_dining1141_s`, `r41_kleinkram_a_gift`, `r39_dining1141_d`, `r41_kanister2080_kampf`.
Dann die Gruppe der beruehrten Module (Regex in scratchpad `n/regex2.txt`, 99 Tests, dazu r36_dachkiste, r37_itemband, g2_cursor, re2doc_bildebene, die der Bau-Agent nicht in seinem Regex hatte): `100% tests passed, 0 tests failed out of 99`, darin `integration_r41_ventilkette` (722 s), `_alarm` (719 s), `_kette`, `_sperre`, `_b` (511 s, der Test, den der Bau-Agent nachgezogen hat), `integration_r41_kleinkram_a_lautstaerke`. Die uebrigen Story-Riegel aus dem Regex des Bau-Agenten (r38_*, r39_*) habe ich nach sechs gruenen Laeufen abgebrochen: 3,5 h Laufzeit, kein Bezug zum Diff ausser dem Messhaken der Lautstaerke, und der haelt 100 %.

**Riegel-Qualitaet.** Aenderungen an alten Riegeln fachlich nachvollzogen (Folge, Panel statt Ventil 13, Takte), nichts aufgeweicht ausser der Elza-Korrelation in `_alarm`, deren Messwert sinkt, die Schwelle 0,3 bleibt. Luecken: Mangel 2 und 4 fangen die Riegel nicht, START im Panel auch nicht.
