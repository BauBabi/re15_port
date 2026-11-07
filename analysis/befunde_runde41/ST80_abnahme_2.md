# ST80 Abnahme 2 - Nachbesserung 1 (Zug 6030: Fuehrerstand vor dem Wagen)

Unabhaengige Abnahme, Stand 2026-11-07 (13:50-14:25). Gemessen am Integrationsbaum, der Meldung des Bauers und dem
Dossier nichts geglaubt.

**URTEIL: BESTANDEN** - M1 der Abnahme 1 ist behoben. In ROOM6030 laufen beide Reihenfolgen widerspruchsfrei. Bei
"vorne zuerst" nennt niemand Sherry, bevor John sie Leon vorstellt. Annettes Sherry-Zeilen kommen erst nach dem
Wagengespraech, entweder in der vollen Fassung oder im Nachtrag. Jede Zeile laeuft hoechstens einmal, es gibt keine
Schleife, und das Spiel geht in beiden Reihenfolgen mit denselben Flags weiter. Kein Satz ist erfunden. Bau,
Generatoren, Programm-Bytes, Riegel und Gegenprobe sind selbst nachgefahren. Offen bleiben nur nicht blockierende Punkte
(Abschnitt 6): zwei stumme Zeilen ohne Aufnahme, die PSX-Tabellengrenze fuer Id 40, "Where's Annette?" bei "vorne
zuerst" (vom Entscheid in Kauf genommen), Bit 128 noch nicht im VERTRAG und ein Hinweis fuer die spaetere Integration der
Spur AN.

## 1. Pruefgegenstand

* Integrationsbaum `C:/workspace/git/reAi_v2/.claude/worktrees/r41_integration`, Zweig `r41/integration`, Kopf
  **8ffad09ab**, Baum sauber (`git status` leer). Gegenstand sind die drei Commits ueber ad8532cd4:
  **45174ae1f** (fix: story80_r41.c, re15_story80_r41.h, gen/story80_szenen.inc +84/-0, gen/story_r36_texte.inc +27/-0,
  szenen_bauen.py, texte_bauen.py, Unit-Riegel), **ceb920b22** (test: test_r36_story.cmake P4a/P4b/P4h/P4i/P4j/P4k,
  probe_r36_story_karte.c, r36_story.cmake) und **8ffad09ab** (Dossier TEXTE_UMSETZUNG.md Abschnitt 8, Belege nb1/).
* **Bau:** `bash re15_port/tools/local_build.sh build` -> `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`.
  Die Bau-exe `re15_port/build/platform/pc/re15_pc.exe` (2026-11-07 13:12) hat sha1
  f896e5a29e5c45bac77095d7fad84f537775985a, dieselbe wie in der Meldung des Bauers.
* **Generatoren reproduzierbar:** `szenen_bauen.py` und `texte_bauen.py` mit umgebogenem OUT in eine Scratch-Datei
  (`logs/gen_repro.py`) sind **bytegleich** mit `gen/story80_szenen.inc` und `gen/story_r36_texte.inc`. Der Diff von
  45174ae1f an beiden .inc besteht nur aus hinzugefuegten Zeilen, die drei bisherigen Programme sind also unveraendert.
* **Messkopie:** `re15_pc_st80abn.exe` (= Bau-exe) in `scratchpad/st80nb1/abn2/bin/`, je Lauf eine Namenskopie
  `re15_pc_st80abn_<lauf>.exe`. Beendet wurden nur eigene PIDs. Die Stimmen sind eine Kopie von
  `C:/workspace/git/reAi_v2/synchro/STAGE6/room6000|room6030|room6031` nach `abn2/synchro/` (Basiswurzel = exe-Ordner und
  seine Vorfahren). Der Renderer ist der des Nutzers (direct3d11), ohne SOFTWARE_RENDER und ohne AUTOSHOT. Die Bilder
  stammen aus RE15_FRAMEDUMP-Serien. Der Ton lief mit `SDL_AUDIODRIVER=dummy`, weil WASAPI in dieser Sitzung "can't find
  requested audio endpoint" meldet (erster Versuch, abgebrochen). Der Mischer laeuft, Laden und Wartelogik sind messbar.
* **Kartenstand:** `probe_r36_story_karte vor6030` = Leon in ROOM6000 vor der Tuer Slot 3 @0x00DDA, (3,0x64)=1 (Treffen
  mit Annette am Bahnsteig gesehen). Danach laufen **nur normale Eingaben**: kein Debug-Sprung und kein CONTINUE im Zug.

## 2. Mechanismus und Umsetzung selbst nachgeprueft (Bytes)

`re15_port/tools/scd_dump_room.py` auf `shared_assets/PSX/STAGE6/ROOM6030.RDT` (Auszug: `logs/scd_auszug.txt`):

| Stelle | Bytes | Bedeutung |
|---|---|---|
| main00 @0x00C52 | `3b 01 02 31 01 00 ac db 78 a1 84 03 78 05 df f0 00 00 41 a4 00 00 05 03 00 ..` | Tuer Slot 1, Rechteck (-9300,-24200,900,1400), Ziel Stage-Index 5 Raum 03 Cut 0 = 6030 Fuehrerstand, Ankunft (-3873,-23487) |
| main00 @0x00C32 | `3b 00 02 31 00 00 14 ec 78 a1 84 03 a4 06 80 da f8 f8 66 a4 00 08 05 03 08 01 ..` | Tuer Slot 0 (Fuehrerstand -> Gang), Ziel 6030 Cut 8, Ankunft (-9600,-1800,-23450) Gier 2048 |
| main00 @0x00CB2 / @0x00CC6 | `2c 04 0c 31 80 ..` / `2c 05 05 41 00 ..` | Treppe Slot 4 sce 12 / Wagen-Marke Slot 5 (-19730,-25570,1600,6300) |
| sub00 @0x00D28 / @0x00D2C / @0x00D32 | `13 0a 0c 00` / `14 00 04 00 00 00` / `18 07` | Switch(work_vars[10]) Case 0 -> Gosub sub07 |
| sub01 @0x00D62 / @0x00D6A / @0x00D70 | `21 03 16 00` / `23 00 01 00 05 00` / `04 ff 18 02` | Ck(3,22)=0, Marke 5 -> Evt_exec sub02 (Wagen) |
| sub02 @0x00D7A | `22 03 16 01` | Set(3,22)=1 (erste Zeile der Wagen-Szene) |
| sub07 @0x010D0 / @0x010E8 | `21 04 17 00` / `04 ff 18 08` | Ck(4,23)=0 -> Evt_exec sub08, **ohne** Abfrage von (3,22) = die Luecke aus M1 |
| sub07 @0x010EC / @0x010F4 / @0x0110C | `07 00 28 00` / `21 03 59 00` / `04 ff 18 09` | sonst Ck(3,89)=0 (Annette da) -> Evt_exec sub09 |
| sub08 @0x0113A | `22 04 17 01` | Set(4,23)=1 |
| sub09 @0x01376..@0x0138C | `2e 02 00 00 3f 00 12 00 09 0a 50 00 3f 00 13 00 43 00 04 00 01 00` | Annette Pult-Haltung 18/19, Plc_flg 4, Evt_end |

**Umsetzung (story80_r41.c):** Der Haken `re15_story80_evt_exec` (scd_vm.c:1272) prueft je Eintrag jetzt bis zu zwei
Flag-Bedingungen mit `re15_game_flag_get`, also dieselbe Tabelle wie Ck. Es gibt drei Faelle:

* sub08 mit (3,22)=1 -> `k_s80_6030_sub08` (unveraendert, 644 B);
* sub08 mit (3,22)=0 -> `k_s80_6030_sub08_vorne` (576 B);
* sub09 mit (9,128)=1 **und** (3,22)=1 -> `k_s80_6030_sub09_nachtrag` (238 B).

In allen anderen Faellen laeuft das Original-Sub.

**Programme selbst geprueft** (`logs/programm_pruef.py`, eigenes Werkzeug; das abn1-Werkzeug las bei zwei Programmen
mit gleichem Sub den falschen Kommentarblock, daher neu): **0 Fehler** in allen fuenf Programmen. Geprueft wurde
Folgendes:

* Jeder "orig"-Baustein ist bytegleich mit der RDT.
* Jeder "op"-Baustein ist gleich seiner RDT-Vorlage, bis auf die genannten Bytes.
* Die Bausteine decken das Programm lueckenlos ab.
* Die Signatur ist gleich den ersten 16 Bytes des Original-Subs.
* Der SCD-Lauf mit der Laengentabelle geht ohne Versatz bis Evt_end.
* Die Message_on-Folge ist gleich der Folge-Tabelle.

| Programm | Bausteine | Message_on | Ck/Set |
|---|---|---|---|
| `sub08_vorne` | orig @0x0113A..0113E, op `22 09 80 01` (Vorlage @0x0113A `22 04 17 01`), orig @0x0113E..01198, op `2b 1f 00 00` (Vorlage @0x01198 `2b 0c 00 00`), orig @0x0119C..011CC, orig @0x011CC..01376 | 11, 31, 13..19 | Set(4,23)=1, **Set(9,128)=1**, Sperren (2,7)/(1,27) an ... aus, (1,28)/(1,29) wie Original |
| `sub09_nachtrag` | op `22 09 80 00` (Vorlage @0x0136C `22 01 1b 00`), orig @0x0113E..01174, @0x01194..01198, op `2b 28 00 00` (Vorlage @0x01198), orig @0x0119C..011CC, @0x011CC..011D0, op `2b 19 00 00` (Vorlage @0x011D0 `2b 0d 00 00`), orig @0x012EA..01306, @0x0131E..01376 | 40, 25 | **Set(9,128)=0** zuerst, Sperren an ... aus, kein Set(4,23) |

Damit gilt: Das Bit (9,128) wird nur von `sub08_vorne` gesetzt. Der Nachtrag loescht es als erste Anweisung, bevor eine
Zeile laeuft. Er kann deshalb hoechstens einmal laufen, auch wenn er abbricht.

**Zweiter Schreiber von (3,22), selbst gefunden (nicht im Dossier):** ROOM6000 sub07, der Torschalter ("A device to open
the tunnel gate. Will you turn on the switch?", msg 5), hat folgende Anweisungen:

* @0x011B4 `22 03 59 01` Set(3,89)=1;
* @0x011BC `21 03 16 00` Ck(3,22)=0 und @0x011C0 `22 03 16 01` Set(3,22)=1;
* @0x011CA/@0x011CE setzt er ebenso (4,23)=1.

Die Aussage "einziger Schreiber/Leser" im Commit und im Dossier 8.1 gilt also nur innerhalb von ROOM6030. Fuer die
Nachbesserung ist das unschaedlich. Wer (3,22) ueber den Schalter setzt, hat im selben Lauf (3,89)=1 gesetzt.
sub07 @0x010F4 `21 03 59 00` laesst sub09 dann nie mehr laufen, ein falscher Nachtrag ist deshalb ausgeschlossen. Fuer
(3,89) gibt es keinen Ruecksetzer (Zensus STAGE5/6: nur Set(3,89)=1 in 6000/6001). Im Port-Code gibt es ausser dem
Wagen-Programm keinen weiteren Schreiber von (3,22).

**Bit (9,128):** Diesen Zensus habe ich selbst gefahren (`tools/r36_kartenbits/bank9_zensus.py`). Ergebnis: Im Original
ist 128 frei (Block 110..133), im Integrationsbaum benutzt nur `gen/story80_szenen.inc` das Bit (Set @Z.208/@Z.262).
Ich habe alle 38 Zweige `r41/*` per git grep durchsucht: Bank 9 Bit 128 kommt sonst nirgends vor (moosgang 126/127,
annette 133). Das Bit ist in `test_r36_kartenbits.c` k_port[] eingetragen. Im VERTRAG ist es noch nicht eingetragen,
siehe N4.

## 3. Herkunft jedes Satzes der neuen Fassung

| Id | Zeile | Satz | Herkunft (selbst geprueft) | Urteil |
|---|---|---|---|---|
| 31 | Leon: Everyone we could find. / Can you start this thing? | "Everyone we could find." | Satz 1 von msg 12, Nutzerwortlaut ST80 (TEXTE_UMSETZUNG 3.4, "vom Nutzer freigegeben"). Der Nutzer hat ihn selbst aufgenommen: `synchro/STAGE6/room6030/main12.wav`, Spracherkennung abn1 "Everyone we could find. There's a little girl in the back. Sherry.". Die ganze Zeile 31 ist ausserdem der **erste Wortlaut des Nutzers** fuer #12 ("biker girl ergibt keinen Sinn -> #11 'Is everyone aboard?' / #12 'Everyone we could find. Can you start this thing?'"), so zitiert in Abnahme 1 Abschnitt 4/7 und im Entscheid des Orchestrators | belegt |
| 31 | | "Can you start this thing?" | Mod-RDT ROOM6030 msg 12 @0x015FC "Leon: Keeping the monsters at bay. / Can you start this thing?" (rdt_msgdump.py); Runde-36-Zeile 12 "Leon: I've never met her. / Can you start this thing?" (`analysis/befunde_runde36/P_story.md` Z. 211); Nutzerzeile msg 26 (`main26.wav`) | belegt |
| 40 | Leon: There's a little girl / in the back... Sherry. | "There's a little girl in the back... Sherry." | Satz 2 von msg 12, Nutzerwortlaut ST80, vom Nutzer aufgenommen (`main12.wav`, s. o.) | belegt |

Der Disc-Text (Capcom) belegt keinen dieser Saetze, er widerspricht ihnen aber auch nicht.
`Version_40%/PSX/STAGE6/ROOM6030.RDT`
und `ROOM6031.RDT` haben **keinen Nachrichtenblock** (RDT-Kopf 0x3C = 0, mit jp.py geprueft). Die englischen Zug-Texte
sind Mod-Text. 80 %-Material: K094/K096 zeigen die Frau im Kittel am Bahnsteig und am Pult, ohne Text. Kopf und Schwanz
beider Zeilen sind die Original-Bytes von msg 12 @0x015FC (`04 00 05 01 28 41 4b 4a 16 05 00 00` ... `04 01 01 63`).
Das prueft der Generator, und er ist reproduzierbar (Abschnitt 1). **Kein Satz ist erfunden.**

## 4. Spiel-Laeufe (eigene Messkopie, normale Eingaben)

Die Eingaben stammen aus dem Lauf kabine5 der Abnahme 1. Der Rueckweg ist neu: Tuer Slot 0 aus dem Fuehrerstand, dann
`R0.71` (21 Bilder Rechtsdrehung, etwa eine halbe Umdrehung) und wieder Tuer Slot 1. Laufauszuege mit Eingabe,
`[story80]`, `[msgtext]`, `[voice]`, stimme.log und den Untertitel-Bildbereichen aus state.log liegen unter
`logs/<lauf>.txt`. Ausgewertet hat `logs/auswerten.py`.

### (A) vorne zuerst -> Wagen -> wieder vorne -> nochmals vorne (Lauf `a_vorne`, 293 s, exit 0, EXIT_AT F7240)

| Bild | Ereignis | Stimme |
|---|---|---|
| F643 | Tuer Slot 1 -> Fuehrerstand, `[story80] ... sub08 -> Programm (576 B): Fuehrerstand vor dem Wagen` | |
| F671 | 11 "Annette: Is everyone aboard?" | main11.wav |
| F791 | **31 "Leon: Everyone we could find. Can you start this thing?"** | `keine Aufnahme (.../main31.wav) - Zeile bleibt stumm` |
| F911..F2015 | 13..19 ("Certainly." .. "Good luck to both of us!") | main13..main19.wav |
| F2223 | Spieler frei, Cut 0, Annette am Pult (t=4d mo=19 @(-1790,-22272)) | |
| F3150..F4943 | Wagen: 0, **1 "Who are you two?" (F3426), 21 Johns Vorstellung mit Sherry (F3547)**, 2, 3, 22, 4, 5, 6, 7, 23, 24, 8 | main00..main24.wav, je Id die eigene Datei |
| F4978 | Spieler frei, Cut 12 | |
| F5810 | Rueckweg (Gang, Treppe, Tuer Slot 1), `[story80] ... sub09 -> Programm (238 B): ... Nachtrag msg 40,25` | |
| F5838 | **40 "Leon: There's a little girl in the back... Sherry."** | `keine Aufnahme (.../main40.wav) - Zeile bleibt stumm` |
| F5958..F6099 | **25 "Annette: Sherry...? My daughter is on this train? ...Thank God."** (142 Bilder = Stimmlaenge) | main25.wav |
| F6284 | Szenenende: Cut 0, Annette am Pult (mo 19), Leon Gier 2047, Spieler frei | |
| F6413..F6800 | Tuer Slot 0 -> Cut 8 (-9600,-23450) -> Wende -> Tuer Slot 1 -> Fuehrerstand | |
| F6800..F7240 | **nochmals vorne: kein `[story80]`, kein Text**, Original-sub09 (Annette mo 18 -> 19 @(-1790,-22272)), Spieler frei | |

Folge der Texte: `11 31 13 14 15 16 17 18 19 0 1 21 2 3 22 4 5 6 7 23 24 8 40 25`. Das sind 24 Zeilen, **keine doppelt**,
und vor msg 21 steht **keine Zeile mit Sherry/daughter/little girl**. Bildleiste: `6030_A_vorne_zuerst.jpg` (+ `_klein`).

### (B) Wagen zuerst -> vorne -> nochmals vorne (Lauf `b_wagen`, 234 s, exit 0, EXIT_AT F5830)

| Bild | Ereignis | Stimme |
|---|---|---|
| F392..F2139 | Wagen 0, 1, 21, 2, 3, 22, 4, 5, 6, 7, 23, 24, 8 (Programm 900 B) | main00..main24.wav |
| F2605 | Fuehrerstand, `[story80] ... sub08 -> Programm (644 B): Fuehrerstand msg 11,12,25,26` | |
| F2633..F4261 | 11, **12** (Sherry), **25**, **26**, 13..19, wie bisher | main11/12/25/26/13..19.wav; msg 25 wartet 14 Bilder auf main12, msg 26 wartet 30 Bilder auf main25 |
| F4469 | Szenenende Cut 0, Spieler frei | |
| F5008..F5400 | Tuer Slot 0 -> Cut 8 -> Wende -> Tuer Slot 1 | |
| F5400..F5830 | **nochmals vorne: kein Nachtrag, kein Text**, Original-sub09, Spieler frei | |

Folge der Texte: `0 1 21 2 3 22 4 5 6 7 23 24 8 11 12 25 26 13 14 15 16 17 18 19`. Das ist die Sollfolge. Kein msg 31/40,
nichts doppelt. Bildleiste: `6030_B_wagen_zuerst.jpg` (+ `_klein`).

### (C) Zusatz: vorne -> erneut vorne VOR dem Wagen -> Wagen -> vorne (Lauf `c_zweimal_vorne`, 326 s, exit 0)

* Fuehrerstand F643..F2223 wie in (A) (11, 31, 13..19).
* Tuer hinaus und wieder hinein (F2686..F3046), dann F3100..F3500 im Fuehrerstand mit Cut 0. Dort laeuft **kein
  `[story80]` und kein Text**, Annette steht mit mo 18 -> 19 am Pult. Bei (3,22)=0 greift das Nachtrag-Gate also nicht.
* Danach wieder hinaus, Treppe, Wagen F4040..F5787 (13 Zeilen) und Nachtrag F6678 (40) / F6798 (25). Spieler frei ab
  F7124.
* Folge wie in (A), 24 Zeilen, keine doppelt. Bildleiste: `6030_C_zweimal_vorne.jpg` (+ `_klein`).

**Stimmen:** Jede Zeile laedt genau ihre eigene Datei `main<Id>.wav` aus `synchro/STAGE6/room6030/`. Das ist dieselbe
Zuordnung wie in ST80, ohne Umleitung in stimmen_r39. Die Zeilen 31 und 40 bleiben stumm und werden gemeldet.
Untertitel mit Stimme stehen so lange, wie die Stimme laeuft (msg 21 149-153, msg 5 170, msg 25 141-149 Bilder), und die
Folgezeile wartet darauf (stimme.log). Es gab keine Abstuerze, keine Haenger und keine abgeschnittenen Texte
(Bildleisten).

**Abfahrt/Szenenende wie heute:** Beide Fuehrerstand-Programme enden mit demselben Original-Schwanz @0x0131E..@0x01376
(Cut 0, Pult-Haltung, Sperren (2,7)/(1,27) frei, Cut_auto, Plc_ret). Nach (A) mit Nachtrag und nach (B) sind die
Spielflags gleich: (3,22)=1, (4,23)=1, (9,128)=0. Die Abfahrt haengt am Torschalter ROOM6000 sub07 ((3,89)=1). Die
Nachbesserung beruehrt ihn nicht, und er liest (9,128) nicht. Bleibt Leon nach dem Wagen nicht im Fuehrerstand, bleibt
(9,128)=1 stehen. Das Bit wirkt dann nirgends mehr, weil sub09 ab (3,89)=1 nicht mehr laeuft.

**Elza-Zug:** Die Nachbesserung beruehrt ROOM6031 nicht. Alle fuenf Eintraege in `k_s80` tragen Raum 0x30C1 bzw.
0x6030, und das Gate prueft die Raum-Id vor den Flags. `integration_r36_story_elza` (6031 msg 0/1, 30C1, 30E1) ist gruen
(Abschnitt 5).

### Gegenprobe: Stand ad8532cd4 in Weg (A)

Die exe habe ich **selbst gebaut**: `git archive ad8532cd4 re15_port` ohne shared_assets nach
`scratchpad/st80nb1/abn2/alt_src`, gebaut mit `local_build.sh configure/build` (RE15_TESTS=OFF). Die Assets kommen ueber
`RE15_CD_ROOT` aus dem Integrationsbaum; `shared_assets` ist zwischen ad8532cd4 und HEAD unveraendert (git diff leer). Die
exe hat sha1 8eb9002d8a9e4d60259f1160bfbff776e6cf3b2d. Lauf `gegen_a_vorne` mit **denselben Eingaben wie (A)**:

```
[story36] ROOM6030: 13 Nachrichten umgeschrieben/neu (Runde 36 + Runde 41 ST80-T)
[story80] ROOM6030 Evt_exec sub08 -> Programm (644 B): Fuehrerstand msg 11,12,25,26 vor 'Certainly.'
F671  [msgtext] room=6030 id=11 "Annette: Is everyone aboard?"
F791  [msgtext] room=6030 id=12 "Leon: Everyone we could find. There's a little girl in the back... Sherry."
F924  [msgtext] room=6030 id=25 "Annette: Sherry...? My daughter is on this train? ...Thank God."
F1073 [msgtext] room=6030 id=26 "Leon: Can you start this thing?"
...   (13..19, Tuer zurueck, Treppe, Marke)
F3426 [msgtext] room=6030 id=1  "Leon: Who are you two?"
F3547 [msgtext] room=6030 id=21 "John: Name's John. This is Sherry. We made it down here with a girl named Elza."
Folge: 11 12 25 26 13 14 15 16 17 18 19 0 1 21 2 3 22 4 5 6 7 23 24 8   (kein 31, kein Nachtrag)
```

Der alte Stand zeigt bei "vorne zuerst" also wieder den M1-Fehler: 12/25 mit Sherry vor "Who are you two?" (msg 1) und
vor Johns Vorstellung (msg 21). `auswerten.py` meldet "Sherry-Zeilen vor Johns Vorstellung: [12, 25]", der neue Stand
dagegen "keine". Der eingecheckte Riegel **`TEIL=zug` mit der alten exe ist rot**:
`r36_story[p4j]: erwartet, aber fehlt: [story80] ROOM6030 Evt_exec sub08 -> Programm (576 B): Fuehrerstand vor dem Wagen`
(test_r36_story.cmake:259), rc=1. Im debug.log dieses P4j-Laufs stehen 12/25 wieder vor msg 1/21, also wuerden auch `keine_sherry_zeile` und
`vor(... id=21 ... id=40)` scheitern (`logs/gegen_riegel_zug.txt`). Bildleiste: `6030_gegenprobe_ad8532cd4.jpg`
(+ `_klein`; F810 Leon nennt Sherry vorne, F930 Annettes msg 25, F3450 erst danach "Who are you two?", F3570 Johns
Vorstellung).

## 5. Tests (eigene Laeufe, Git-Bash, PATH=/c/msys64/mingw64/bin:$PATH, SDL_ASSERT=always_ignore, --test-dir .../r41_integration/re15_port/build)

* `-R "^(unit_r41_story80_|unit_r36_story_|unit_r36_kartenbits$)" -j 4` -> `100% tests passed, 0 tests failed out of 13`
  (`Total Test time (real) =   0.53 sec`). Im Verbose-Lauf zeigt `unit_r41_story80_reihenfolge6030` den ganzen Weg
  `11 31 13 14 15 16 17 18 19 0 1 21 2 3 22 4 5 6 7 23 24 8 40 25`, "jede Zeile hoechstens einmal (24 Zeilen)", (9,128)
  1 -> 1 (erneut vorne vor dem Wagen) -> 0 nach dem Nachtrag, Cut 0, Sperren frei.
* `-R "^(unit_r41_story80_|unit_r36_story_|unit_r36_kartenbits$|unit_r35_affen_|unit_r36_dachkiste_|unit_r37_fahrstuhl_|unit_r41_stage3szenen_a_|unit_r41_wasser20b0_|unit_r37_sperre1050_)" -j 4`
  -> `100% tests passed, 0 tests failed out of 64` (`Total Test time (real) =  21.90 sec`)
* `-R "^(integration_r36_story_leon|integration_r36_story_elza|integration_r36_story_zug|integration_r35_zgirl_5090)$" -j 4 --timeout 3000`
  -> `100% tests passed, 0 tests failed out of 4` (`Total Test time (real) = 524.46 sec`): story_leon 524,38 s (mit
  P4a/P4b/P4h/P4i), story_zug 453,20 s (P4j/P4k), story_elza 273,39 s, zgirl_5090 148,57 s. Die vier liefen parallel
  neben eigenen exe-Laeufen und den Laeufen anderer Agenten. Keiner war rot, Nachfahren war daher nicht noetig. Die
  Textfolgen der Riegel-Laeufe stehen in `logs/tests.txt`: P4b `11 31 13..19`, P4h `11 12 25 26 13..`, P4i `40 25`, P4j
  `11 31 13..19 / 0 1 21 .. 8 / 40 25`, P4k `0 1 21 .. 8 / 11 12 25 26 13..19`.
* PSX-Syntax: `mipsel-none-elf-gcc -fsyntax-only -ffreestanding -DRE15_PLATFORM_PSX` (PSn00bSDK 0.24) auf story80_r41.c
  und story_r36.c -> rc 0, keine Meldung.

## 6. Restmaengel (nicht blockierend)

* **N1 (mittel, Nutzer):** Fuer msg 31 "Everyone we could find. Can you start this thing?" und msg 40 "There's a little
  girl in the back... Sherry." gibt es keine Aufnahme. Bei "vorne zuerst" ist dadurch eine Zeile Leons zwischen
  vertonten Zeilen stumm, im Nachtrag ebenso die erste. Benoetigt werden `synchro/STAGE6/room6030/main31.wav` und
  `main40.wav`. Laut Entscheid ist das erlaubt (melden, nicht erfinden).
* **N2 (gering, PSX):** Id 40 liegt ueber PSX `MSG_TABLE_N` 32 (msg_common.c:31). `re15_msg_install_text` verwirft die
  Id (msg_common.c:274), und Message_on blockiert nicht (scd_vm.c, `re15_dialog_open_mask`, `t->pc += 4`). Auf der PSX
  zeigte der Nachtrag deshalb nur Annettes msg 25, ohne Leons Satz davor. Das waere ein Widerspruch, der aber nur auf dem
  PSX-Ziel auftritt, und das baut derzeit nicht (FindPSn00bSDK/SDK 0.24). Eine freie Id < 32 gibt es in 6030 nicht mehr:
  0..20 RDT, 21..26 ST80, 27..30 Spur AN, 31 hier.
* **N3 (Kenntnis, vom Entscheid gedeckt):** Bei "vorne zuerst" fragt Leon im Wagen weiterhin msg 6 "Where's Annette?",
  obwohl er gerade bei ihr war. Das war schon in Runde 36 vermerkt, und Abnahme 1 schrieb "mit (a) bliebe nur das". Im
  Lauf (A) ist es die einzige schiefe Stelle (Bild F4350). Wer das auch loesen will, braucht eine reihenfolgefeste
  Wagen-Zeile, also eine Nutzerentscheidung (Frage 2).
* **N4 (Prozess, Orchestrator):** Das Bank-9-Bit 128 (`RE15_S80_BIT_NACHTRAG`) steht noch nicht im VERTRAG Runde 41
  Abschnitt 1.1. Dort heisst es "125-133 vergibt der Orchestrator", die Spur hat das Bit selbst gewaehlt. Nach meinem
  Zensus ist es frei (Abschnitt 2). Der Orchestrator muss es eintragen.
* **N5 (gering, Doku):** Die Aussage "(3,22) hat genau einen Schreiber und einen Leser" in 45174ae1f, im Dossier 8.1 und
  in re15_story80_r41.h gilt nur fuer ROOM6030. ROOM6000 sub07 @0x011BC/@0x011C0 (Torschalter) liest und setzt (3,22)
  ebenfalls. Fuer die Funktion ist das unschaedlich, siehe Abschnitt 2. Ausserdem zeigt die exe-Gegenprobe des Bauers
  (`nb1/logs/gegenprobe_alt_p4b.txt`, "rot in P4b") P4a mit dem alten Stand gruen. Das geht nur mit der frueheren
  P4a-Erwartung "13 Nachrichten", die der Bauer nach eigener Meldung erst danach auf 15 berichtigt hat. Mit dem
  eingecheckten Riegel scheitert der alte Stand schon an P4a, denn er meldet laut Lauf `gegen_a_vorne` "[story36]
  ROOM6030: 13 Nachrichten", gefordert sind 15. Rot ist der Riegel trotzdem.
* **N6 (Hinweis Integration Spur AN, nicht in diesem Baum):** Die AN-Zugstart-Szene (Zweig `r41/annette`,
  `gen/annette_r41_texte.inc`) setzt voraus, dass Annette von Sherry weiss: ROOM6030 msg 28 "Annette: What was that?
  ...Sherry is back there!" und msg 30 "No. She's my daughter. ...". Mit der Nachbesserung gilt das nur in zwei Faellen:
  Wagen zuerst, oder vorne zuerst mit Nachtrag. Geht Leon nach dem Wagen nicht mehr nach vorne, oder legt er den
  Torschalter vor dem Wagen um, hat ihr niemand von Sherry erzaehlt. Bei der AN-Integration ist deshalb (9,128) bzw.
  (3,22) aus der Wagen-Szene zu beachten. Der Bauer hat das selbst als "nicht geprueft" gemeldet.

## 7. Fragen an den Nutzer (fuer die Release Notes, Sie-Form)

1. **Zwei neue Zeilen ohne Aufnahme:** Wenn Sie im Zug zuerst nach vorne zu Annette gehen, sagt Leon jetzt "Everyone we
   could find. Can you start this thing?". Erst nach dem Gespraech im Wagen sagt er beim naechsten Besuch vorne "There's a
   little girl in the back... Sherry.", und Annette antwortet mit Ihrer Aufnahme main25. Beide Saetze Leons stammen aus
   Ihrer Zeile main12 bzw. aus Ihrem ersten Wortlaut. Moechten Sie sie als `synchro/STAGE6/room6030/main31.wav` und
   `main40.wav` aufnehmen? Bis dahin bleiben sie stumm.
2. **"Where's Annette?" bei "vorne zuerst":** In dieser Reihenfolge fragt Leon im Wagen nach Annette, obwohl er gerade
   bei ihr war. Soll das so bleiben, wie Runde 36 und Abnahme 1 es vermerkt haben, oder wuenschen Sie fuer diese
   Reihenfolge eine eigene Zeile?

## 8. Dateien

* Bericht: `analysis/befunde_runde41/ST80_abnahme_2.md`
* Bilder: `analysis/befunde_runde41/ST80_belege/abn2/6030_A_vorne_zuerst.jpg`, `6030_B_wagen_zuerst.jpg`,
  `6030_C_zweimal_vorne.jpg`, `6030_gegenprobe_ad8532cd4.jpg` (je + `_klein`), RE15_FRAMEDUMP-Serien, Renderer des Nutzers
* Laufauszuege und Werkzeuge: `analysis/befunde_runde41/ST80_belege/abn2/logs/` (a_vorne, b_wagen, c_zweimal_vorne,
  gegen_a_vorne, gegen_riegel_zug, tests.txt, scd_auszug.txt, programm_pruef.txt; lauf.sh, auswerten.py,
  programm_pruef.py, blatt.py, gen_repro.py)
* Rohdaten (Framedumps, volle Logs, alter Bau):
  `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/st80nb1/abn2/`

Committet sind nur dieser Bericht und die Belege unter `ST80_belege/abn2/`. Unter pri/, synchro/ und Version_80%/ habe
ich nichts geaendert. Spurbaeume und Hauptbaum habe ich nur gelesen.
