# ST80 Abnahme 1 - Story nach dem 80%-Stand (Texte, Stimmen, Szenen)

Unabhaengige Abnahme, Stand 2026-11-07 (11:45-12:50). Gemessen am integrierten Stand, den Dossiers nichts geglaubt.

**URTEIL: NICHT BESTANDEN** - ein blockierender Mangel (M1: Im Zug ROOM6030 ist die Reihenfolge "Fuehrerstand zuerst"
im normalen Spiel erreichbar. Dann nennt Leon Sherry, bevor er sie getroffen hat, und fragt danach im Wagen "Who are you
two?". Das ist ein Rueckschritt gegen Runde 36, die genau diese Reihenfolge gemessen und ihre Zeile 12 dafuer
reihenfolgefest gemacht hatte). Alles andere ist in Ordnung: Belege tragen, Texte und Szenen erscheinen richtig, keine
Haenger, Stimmen synchron, alle gefahrenen Riegel gruen.

## 1. Pruefgegenstand

* Integrationsbaum `C:/workspace/git/reAi_v2/.claude/worktrees/r41_integration`, Zweig `r41/integration`, Kopf **9867e3913**,
  Baum sauber. Spur ST80 = Zweig `r41/story80` (Kopf 4c374d4b8), gemergt als **7d8634051** (2026-11-06 19:57).
  `git diff r41/story80 HEAD` ueber alle ST80-Dateien (story80_r41.c, gen/story80_szenen.inc, gen/story_r36_texte.inc,
  story_r36.c, re15_story80_r41.h, re15_story_r36.h, re15_hebetisch_cursor.h, Tests, Riegel) = leer; der Haken in
  scd_vm.c op_evt_exec steht unveraendert (HEAD:1270-1272).
* Commits `68f3f5f13..r41/story80` (16): 8ee305d14 (K, Katalog), 03d2928cb + b8735162a (P, Zensus), eb7232020 + f0eaac08c
  (A, Abgleich), **6fb2f2f22** (fix Hebetisch-Id), **c5e5b03e9** (feat Texte/Szenen), **63da064fd** (test Ada-Sweep),
  90b342340 + b93b16ede (Dossier), c20d2031c (Moosgang-Doku), 4217e55bb + 4c374d4b8 (Aufnahmeliste). Nur die drei fett
  gesetzten aendern Spielcode oder Riegel.
* Messkopie: `re15_pc.exe` aus `re15_port/build/platform/pc/` (2026-11-06 20:12, sha1 d8529b34dea9d2db508e0302186d056ce7b7c3e9,
  gleich der Bau-exe) nach `scratchpad/testint/st80_abn1/bin/`, je Lauf eine eigene Namenskopie, nur eigene PIDs beendet.
  Stimmen: Kopie der betroffenen Ordner aus dem Hauptbaum `C:/workspace/git/reAi_v2/synchro` (die neuen Aufnahmen liegen nur
  dort, siehe N1) nach `st80_abn1/synchro/` (Basiswurzel = exe-Ordner und Vorfahren, asset_root_pc.c:172-200).

## 2. Liste aller Aenderungen am Spiel, mit Beleg-Urteil

### Ae1 - ROOM1150 (Leon) / ROOM1151 (Elza): Hebetisch-Text "Nothing happened." Id 20 -> 21 (6fb2f2f22)

| | |
|---|---|
| Aenderung | `include/re15_hebetisch_cursor.h:188` `RE15_HC_TEXT_ID` 20 -> 21. Wortlaut unveraendert, keine Stimme. |
| Ursache | Port-Fehler: elza_r36 legt Irons' Text als Id 20 ab (`RE15_ELZA_IRONS_MSG_ID`, re15_elza_r36.h:58), der Hebetisch-Fehldruck ueberschrieb dieselbe Id. |
| Beleg selbst geprueft | Id-Belegung: ROOM1150 RDT-Nachrichten 0..14 (msg_sec 0x12F8), ROOM1151 0..3 (msg_sec 0x10EC), mit `rdt_msgdump.py` selbst gelesen; Id 21 ist in 1150/1151 sonst nirgends belegt (grep include/ engine/: Id-21-Konstanten sonst nur fuer 1110 Leiche, 1050 Rolltor-Satz, 11B0 Szenen-Leon R36 und 6030). In 1150 schaltet `stimmen_r39.c:67-74` Id 21 ohnehin stumm (main21.wav gehoert zu msg 28), ein falscher Ton ist also nicht moeglich. |
| Spiel | Lauf **elza1151**: Fehldruck F118-F180 `[msgtext] room=1151 id=21 "Nothing happened."`, danach Irons-Platz (Slot 2, `RE15_FIRE_AOT=2@450`) `[msgtext] room=1151 id=20 "He has already passed..."`. Vor der Behebung stand dort laut Messung der Spur (Unit-Riegel vor 6fb2f2f22, nicht wiederholt) "Nothing happened.". Lauf **leon1150**: Fehldruck `id=21 "Nothing happened."`. Bild: `ST80_belege/abn1/1151_1150_hebetisch_irons.jpg`. |
| Urteil | **in Ordnung** (technischer Fehler, kein Story-Inhalt). Die Leon-Seite "Irons nach Fehldruck" ist nur im Unit-Riegel gemessen (Slot 2 ist in 1150 sce 5 und laesst sich per FIRE_AOT nicht ausloesen), siehe N6. |

### Ae2 - ROOM30E1 (Elza trifft John): msg 2/3 nach A1 (c5e5b03e9)

| Id | alt (Mod) | neu | Stimme |
|---|---|---|---|
| 2 | John: This is bad... one of those monsters bit me as I tried to get away! | John: This is bad... those monsters have us surrounded! | `synchro/STAGE3/room30E1/main02.wav` (neu, 3,55 s) |
| 3 | John: ...And there doesn't seem to be a place with any medical supplies here... | John: ...Damn it, there's no place around here to hide... | `synchro/STAGE3/room30E1/main03.wav` (neu, 3,09 s) |

* **Beleg selbst nachgeprueft:** `Version_40%/PSX/STAGE3/ROOM30E1.RDT @0x0067D` gelesen und mit
  `analysis/story_begegnungen/werkzeuge/jp.py` dekodiert: 「エルザ こんな所にいたか」「まずいぜ…東りを化け物どもに 東まれちまった！」
  「…東生 東れられそうな所なんか ありゃしねえ…」「エルザ そっちはどうだ…」 (東 = Ersatzzeichen, Bytes `0b 3a`). Zum Objekt
  周り passt grammatisch nur 囲まれ (umstellt); 畜生 / 隠れられそうな所 = "Verdammt, nirgends ein Versteck". Die Zeile laeuft auf
  der Disc: sub00 @0x002BE `21 03 01 00` Ck(3,1)=0, @0x002C2 `21 03 04 01` Ck(3,4)=1, @0x00302 `04 ff 18 02` Evt_exec sub02,
  sub02 @0x00358 `2b 0c 00 00` Message_on 12 (scd_dump_room.py auf die Disc-RDT); Schreiber ROOM3071 @0x3236 `22 03 04 01`
  (einzige Fundstelle in der Datei). 80 %: T26 (A#059, Raum 309) John "fast gebissen", also nicht gebissen. Port-RDT alt
  bestaetigt (ROOM30E1 @0x0F4F und @0x0FA2).
* Nutzerregel: Widerspruch nachweislich (gerufene Capcom-Zeile) und gravierend (Biss = Ansteckung, im Port ohne Folge);
  Nutzerentscheid A1 (Protokoll runde41 Z. 88). Wortlaut = Nutzerwortlaut. Raum unveraendert.
* Spiel: Lauf **elza30e1** - alle 18 Zeilen in Folge (F20..F2276), msg 2/3 neu, die neuen Aufnahmen werden geladen
  (`[voice] clip loaded: main02.wav (32000 Hz x1 -> 156672 @44100 Hz)`), Folgezeilen warten auf die laufende Stimme
  (stimme.log: msg 3 7 Bilder, msg 5 18, msg 7 26, msg 11 80, msg 13 44, msg 15 58), danach Spielerkontrolle (F2600 pst=1).
  Bild `30e1_john_a1.jpg`.
* **Urteil: Beleg traegt, umgesetzt wie entschieden, in Ordnung.**

### Ae3 - ROOM30C1 (Elza trifft Marvin): kurze Fassung (c5e5b03e9)

| Folge/Id | alt (Port vorher) | neu | Stimme |
|---|---|---|---|
| 1 | Elza: Who are you? | unveraendert | `synchro/STAGE3/room30C1/main01.wav` (bleibt) |
| 2 | Officer: I'm Marvin Branagh, RPD. I'm here with Leon Kennedy. | Officer: Marvin Branagh, RPD. | `synchro/STAGE3/room30C1/main02.wav` (neu) |
| 3 | Elza: Leon? I don't know him. I never ran into him at the RPD. (Runde 36) | Elza: Are there any others still alive? | `synchro/STAGE3/room30C1/main03.wav` (neu) |
| 4 | Marvin: He's a rookie, and he's helping me and Ada escape the city. (Runde 36) | Marvin: A few. There are other survivors out there. | `synchro/STAGE3/room30C1/main04.wav` (neu) |
| 5 | Elza: Ada? | Elza: Then I hope they make it out. | `synchro/STAGE3/room30C1/main05.wav` (neu) |
| 6-8 | Marvin "Someone we rescued...", Elza "Will you tell them...", Marvin "Will do..." | laufen nicht mehr | main06-08 werden nie gerufen (der Nutzer hat sie nach `synchro/unused/STAGE3/room30C1/` gelegt) |

* Beleg: Lueckenfueller des Nutzers (Protokoll Z. 92, Wortlaut woertlich umgesetzt); 80 %: K074 (A#058) Elza und Marvin am
  Tresen ohne Text, also kein Widerspruch. Leon-Elza-Regel gewahrt (Leon wird nicht mehr genannt).
* Szene = Programm aus Original-Bytes, selbst nachgeprueft (`szenen_pruef.py`): @0x00C2C..@0x00D9E (370 B),
  @0x00D9E..@0x00DA2, @0x00E16..@0x00E6A (84 B) bytegleich, Ende `01 00`. Der weggelassene Bereich @0x00DA2..@0x00E16 enthaelt
  nur Message_on 6/7/8, Plc_motion, Plc_flg, Sleep, Work_set, **keinen Flag-Schreiber** (Disassembly selbst gelesen); die
  Sperren (2,7)/(1,27) hebt der uebernommene Schluss @0x00E4E/@0x00E52 wieder auf.
* Spiel: Lauf **elza30c1** (Bildfolge) und **elza30c1_ton** (Stimmen): Texte 1..5 ab F144/F264/F384/F504/F624 (je 99 Bilder,
  msg 4 mit Stimme 103), keine msg 6..8, Marvin winkt (F780) und geht, Cut 3, Spielerkontrolle (F2600 pst=1 cam=3).
  Bild `30c1_kurze_fassung.jpg`.
* **Urteil: in Ordnung.**

### Ae4 - ROOM6030 (Leon), Wagen: 13 Zeilen (c5e5b03e9)

| Folge | Id | alt (Port vorher) | neu | Stimme |
|---|---|---|---|---|
| 1 | 0 | Leon: Hang on, Marvin. We're getting out of here. | unveraendert | `synchro/STAGE6/room6030/main00.wav` |
| 2 | 1 | Leon: Is anyone else with you? (Runde 36) | Leon: Who are you two? | `synchro/STAGE6/room6030/main01.wav` |
| 3 | 21 | - | John: Name's John. This is Sherry. / We made it down here with / a girl named Elza. | `synchro/STAGE6/room6030/main21.wav` |
| 4 | 2 | Sherry: Elza is out there fighting the monsters! (Runde 36) | Original-Satz: Sherry: She's out there fighting the monsters! | `synchro/STAGE6/room6030/main02.wav` |
| 5 | 3 | John: Yeah! She's trying to keep the outside clear so we can escape! | John: She's trying to keep the outside clear so we can escape. | `synchro/STAGE6/room6030/main03.wav` |
| 6 | 22 | - | John: She said she'd catch up with us. | `synchro/STAGE6/room6030/main22.wav` |
| 7 | 4 | Leon: Why aren't you out there? | unveraendert | `synchro/STAGE6/room6030/main04.wav` |
| 8 | 5 | John: She told me to stay and keep an eye on Marvin here. | John: She told me to stay with the kid... and look after your friend here. | `synchro/STAGE6/room6030/main05.wav` |
| 9 | 6 | Leon: Where's Annette? | unveraendert | `synchro/STAGE6/room6030/main06.wav` |
| 10 | 7 | John: Up front, trying to start the train. | John: The woman in the lab coat? Up front, trying to start the train. | `synchro/STAGE6/room6030/main07.wav` |
| 11 | 23 | - | Sherry: A lab coat...? Is it Mom?! | `synchro/STAGE6/room6030/main23.wav` |
| 12 | 24 | - | John: Easy, kid. Stay with me. | `synchro/STAGE6/room6030/main24.wav` |
| 13 | 8 | Leon: I see. Thanks. | Leon: I'll find out. Stay here. | `synchro/STAGE6/room6030/main08.wav` |

* Beleg: Nutzerentscheide (Protokoll Z. 94-96: Vorstellung John/Sherry, Elza-Verbleib Variante a, Sherry/Annette-Zeilen).
  Besetzung John/Sherry/Marvin im Zug = Disc 604; Lueckenfueller ohne 80%-Widerspruch. Leon-Elza-Regel gewahrt (nur der Name).
* Szene selbst nachgeprueft: 24 Original-Bausteine bytegleich mit ROOM6030 (Kopf @0x00D7A..@0x00E5C, Schluss
  @0x00FA2..@0x00FE8, Gesten @0x00EEE/@0x00E9E/@0x00F4A usw.); die 5 erzeugten Befehle weichen nur in der Message-Id
  (`2b 15`/`2b 16`/`2b 17`/`2b 18`) bzw. im Ziel von Sherrys Drehung (`40 00 09 21 11 9e bb a1`, Ziel = Leons Marke @0x00E0E)
  von ihrer Vorlage ab. Die Flag-Schreiber (3,22)/(2,7)/(1,27) des Originals bleiben erhalten.
* Spiel: Lauf **kabine1** (echtes Audiogeraet) und **leon6030wagen** (Bildfolge): 13 Zeilen in Sollfolge (F392..F2106), alle
  13 neuen Aufnahmen werden geladen, der Untertitel bleibt stehen, solange die Stimme laeuft (msg 21 132-134 Bilder, msg 5
  145-154, msg 7 110-116), die Folgezeile wartet (msg 6: 10 bzw. 1 Bild). Danach Cut 12 und Spielerkontrolle (F2700 pst=1).
  Dreizeiler msg 21: Zeile 3 auf dem unteren Kinobalken, vollstaendig lesbar (`6030_msg21_dreizeiler_ausschnitt.png`).
  Bild `6030_wagen.jpg`.
* **Urteil: in der Reihenfolge Wagen -> Fuehrerstand in Ordnung.** Zur anderen Reihenfolge siehe M1.

### Ae5 - ROOM6030 (Leon), Fuehrerstand (c5e5b03e9)

| Folge | Id | alt (Port vorher) | neu | Stimme |
|---|---|---|---|---|
| a | 11 | Annette: Where's that biker girl? | Annette: Is everyone aboard? | `synchro/STAGE6/room6030/main11.wav` |
| b | 12 | Leon: I've never met her. Can you start this thing? (Runde 36) | Leon: Everyone we could find. There's a / little girl in the back... Sherry. | `synchro/STAGE6/room6030/main12.wav` |
| c | 25 | - | Annette: Sherry...? My daughter is / on this train? ...Thank God. | `synchro/STAGE6/room6030/main25.wav` |
| d | 26 | - | Leon: Can you start this thing? | `synchro/STAGE6/room6030/main26.wav` |
| danach | 13..19 | unveraendert ("Certainly." ...) | | `synchro/STAGE6/room6030/main13.wav` .. `main19.wav` |

* Beleg: Nutzerentscheide (Z. 95 "biker girl" gestrichen, Z. 96 "Annette erfaehrt im Fuehrerstand von Sherry"). Szene
  bytegleich nachgeprueft (Kopf @0x0113A..@0x011CC, Gesten @0x012EA/@0x012C6, Rest @0x011CC..@0x01376; `2b 19`/`2b 1a` mit
  Vorlage @0x011D0/@0x0130A); Flag (4,23) wie Original.
* Spiel: Lauf **kabine4** (echtes Audiogeraet): 11, 12, 25, 26, 13..19 in Folge; msg 25 wartet 12 Bilder auf main12,
  msg 26 25 Bilder auf main25; Untertitel 12/25 stehen 131/144 Bilder (= Stimmlaenge).
* **Urteil: Szene laeuft richtig - aber M1.**

### Ae6 - Texttabelle Runde 36 umgebaut (c5e5b03e9, 63da064fd)

`re15_story_r36_text_t` um `art`/`vorlage` erweitert (story_r36.c:167 filtert jetzt `art == PLATZ` statt `off_alt == 0`);
die R36-Overrides 30C1 msg 7 und 6030 msg 2 sind entfernt (msg 7 wird nicht mehr gerufen, msg 2 ist wieder der
Original-Satz). Die uebrigen R36-Texte (6031 msg 0/1, 50B0 msg 1, 6000 msg 0/3, Plaetze 1210/1091, 50B1 msg 0) sind
unveraendert und in `integration_r36_story_leon/_elza` (P4c, P4d, P4e, P1-P3) gruen. Der Riegel
`unit_r37_sperre1050_elza50b1` erwartet "Ada" im Elza-Lauf jetzt 0x statt 2x (Folge der kurzen Fassung; stuetzt 50B1
"It's a woman. She's dead."). **In Ordnung.**

### Stimmen - Spracherkennung selbst wiederholt

41 Dateien aus `C:/workspace/git/reAi_v2/synchro` (Whisper large-v3-turbo und medium.en, Beam 5, `logs/asr.py`, Ergebnis
`ST80_belege/abn1/logs/spracherkennung_abn1.json`): **alle 24 Dateien der Spur sprechen den neuen Untertitel** (30C1 main01-05,
30E1 main02/03, 6030 main00-08, main11/12, main21-26), ebenso die unveraenderten 6030 main13-19 und 30E1 main00/01/04/05; Abweichungen nur in der Schreibweise des Erkenners ("Brenna", "Elsa",
"a net"). Ebenso die R36-Dateien 6031 main00/01, 6000 main00/03, 50B0 main01. Das ABGLEICH-Dossier (f.1) nannte fuer
6000 main00 noch "Your friends" - die Datei ist inzwischen ersetzt (4,14 s statt 3,34 s) und sagt "Your friend is already
here.". Format ueberall 32000 Hz, mono, 16 bit.

## 3. Spiel-Laeufe (eigene Messkopie, Renderer des Nutzers, kein SOFTWARE_RENDER/AUTOSHOT, Fenster per SetWindowPos frei gestellt)

| Lauf | Figur / Raum | Weg | Ergebnis |
|---|---|---|---|
| kabine1 | Leon 6000 -> 6030 | Tuer, Lauf nach -Z in die Marke | Wagen 13 Zeilen, echte Tonausgabe, keine Haenger |
| leon6030wagen | Leon 6000 -> 6030 | P4a-Weg "A0.3,W8,U3,W62" | 13 Zeilen F392-F2106, Cut 12, Kontrolle F2700, Bildfolge |
| kabine4 | Leon 6000 -> 6030 -> Fuehrerstand | Gang, Treppe, Tuer (siehe M1) | **Fuehrerstand zuerst**, echte Tonausgabe |
| kabine5 | Leon 6000 -> 6030 -> Fuehrerstand -> Wagen | wie kabine4, danach zurueck in den Wagen | **beide Szenen in umgekehrter Folge**, Bildfolge (M1) |
| elza30c1, elza30c1_ton | Elza 30C1 (CONTINUE, (3,0x39)) | Szene startet beim Laden | 5 Zeilen, Abgang, Cut 3, Kontrolle |
| elza30e1 | Elza 30E1 (CONTINUE Cut 0, (3,0xB5)) | Szene startet beim Laden | 18 Zeilen, A1 neu, Kontrolle |
| elza1151 | Elza 1151 ((3,0x9D)=1) | Hebetisch, Fehldruck, Irons-Platz | Id 21, danach Id 20 "He has already passed..." |
| leon1150 | Leon 1150 | Hebetisch, Fehldruck | Id 21 "Nothing happened." |

Ton: kabine1 und kabine4 mit echtem Geraet; ab etwa 12:05 meldete WASAPI "can't find requested audio endpoint" (Umgebung der
Sitzung), die spaeteren Laeufe deshalb mit `SDL_AUDIODRIVER=dummy` (der Mischer laeuft, Laden und Wartelogik sind messbar).
Keine Abstuerze, keine Haenger, kein Text doppelt, keiner abgeschnitten (Breiten <= 271 px laut Riegel, im Bild geprueft).
Auszuege je Lauf: `ST80_belege/abn1/logs/<lauf>.txt`.

## 4. M1 (BLOCKIEREND) - ROOM6030: Fuehrerstand vor dem Wagengespraech erreichbar, Leon nennt Sherry, bevor er sie kennt

**Gemessen (Lauf kabine5, echte exe, normale Eingaben, kein Debug-Sprung, kein CONTINUE im Fuehrerstand):** Leon kommt aus
ROOM6000 durch die Tuer (6000 Slot 3 @0x00DDA, Ziel 6030 Cut 6, Lage (-17970,-20380)), laeuft nach Suedost in den Gang
zwischen den SCA-Zellen #2 (x -16900..-7800, z -22290..-18450) und #3 (z -27781..-24471), an dessen Ende die Treppe
(Aot_set Slot 4 sce 12, main00 @0x00CB2, Rechteck (-12960,-23500,2180,680)) auf Band 1 fuehrt, und oeffnet die Tuer Slot 1
(main00 @0x00C52, Ziel 6030 Cut 0). Die Wagen-Marke (Slot 5, main00 @0x00CC6, x -19730..-18130; Ausloeser sub01 @0x00D62
`21 03 16 00` Ck(3,22)=0 und @0x00D6A `23 00 01 00 05 00` work_vars[1]==5) wird nie betreten. Der Eintritt mit Cut 0 laeuft
sub00 @0x00D28 Switch(work_vars[10]) Case 0 @0x00D2C -> Gosub sub07 @0x00D32; sub07 @0x010D0 `21 04 17 00` Ck(4,23)=0 ->
@0x010E8 `04 ff 18 08` Evt_exec sub08 (= ST80-Programm), ohne (3,22) abzufragen. Ergebnis (state.log/debug.log):

```
F671  [msgtext] room=6030 id=11 "Annette: Is everyone aboard?"
F791  [msgtext] room=6030 id=12 "Leon: Everyone we could find. There's a little girl in the back... Sherry."
F911  [msgtext] room=6030 id=25 "Annette: Sherry...? My daughter is on this train? ...Thank God."
...   (13..19, Tuer zurueck in den Wagen, Treppe, Marke)
F3426 [msgtext] room=6030 id=1  "Leon: Who are you two?"
F3547 [msgtext] room=6030 id=21 "John: Name's John. This is Sherry. We made it down here with a girl named Elza."
F4299 [msgtext] room=6030 id=6  "Leon: Where's Annette?"
F4815 [msgtext] room=6030 id=8  "Leon: I'll find out. Stay here."
```

Bildbeleg: `ST80_belege/abn1/6030_kabine_zuerst.jpg` (F600 Leon oben an der Tuer, F840 Zeile 12, F960 Zeile 25, F3480 "Who
are you two?", F3600 Johns Vorstellung, F4350 "Where's Annette?", F4860 "I'll find out."). Weg mit Lagen: `logs/kabine5.txt`;
Lauf derselben Reihenfolge mit echter Tonausgabe: `logs/kabine4.txt`.

**Warum blockierend:** (1) Leon kennt in dieser Reihenfolge weder Sherry noch ihren Namen und nennt sie trotzdem; danach
stellt John ihm Sherry vor. Das ist ein sichtbarer Story-Widerspruch in einem normalen Spielweg. (2) **Rueckschritt:** Runde 36
hat genau diese Reihenfolge an der echten exe gemessen (`analysis/befunde_runde36/P_story.md` Z. 183-189: "Leon kann vorne
sein, bevor Sherry ihm etwas gesagt hat") und Zeile 12 deshalb reihenfolgefest gemacht ("I've never met her. Can you start
this thing?"). ST80 hat diese Absicherung ersetzt, ohne die Reihenfolge erneut zu pruefen; der eigene Riegel P4b
(`test_r36_story.cmake:20` und `:157-166`, "CONTINUE in ROOM6030 (Cut 0): Annette-Szene ZUERST") verlangt die Sherry-Zeile sogar genau in
dieser Reihenfolge. (3) Der beauftragte Punkt "Annette erfaehrt im Fuehrerstand von Sherry" setzt das Wagengespraech voraus
und ist in dieser Reihenfolge im Spiel kaputt.

**Loesungswege (Entscheid des Nutzers, keine Zahl ohne Beleg):** (a) Im Fuehrerstand bei (3,22)=0 (Flag-Schreiber sub02
@0x00D7A `22 03 16 01`, Abfrage sub01 @0x00D62 `21 03 16 00`) eine reihenfolgefeste Fassung ohne Sherry, z. B. der erste
Wortlaut des Nutzers "Everyone we could find. Can you start this thing?", und Zeilen 25/26 nur nach dem Wagengespraech;
(b) die Fuehrerstand-Tuer bis zum Wagengespraech mit einem Hinweistext sperren; (c) so lassen. Bei (a) und (b) muss P4b
angepasst werden. "Where's Annette?" (msg 6) bleibt in der umgekehrten Reihenfolge so schief, wie Runde 36 es schon vermerkt
hat; mit (a) bliebe nur das.

## 5. Tests (eigene Laeufe, Git-Bash, PATH=/c/msys64/mingw64/bin:$PATH, SDL_ASSERT=always_ignore, --test-dir .../r41_integration/re15_port/build)

* `-R "^(unit_r41_story80_|unit_r36_story_)" -j 2` -> `100% tests passed, 0 tests failed out of 11`
  (`Total Test time (real) =   0.40 sec`)
* `-R "^(unit_r34n_b_|unit_r36_elza_|unit_r38_kleinkram_|unit_r37_sperre1050_|unit_r31_hebetisch|unit_r32_hebetisch_faecher)" -j 2`
  -> `100% tests passed, 0 tests failed out of 26` (`Total Test time (real) =   0.67 sec`)
* `-R "^(integration_r36_story_leon|integration_r36_story_elza|integration_r36_elza_irons|integration_r38_kleinkram_irons|integration_r34n_b_cursor)$" -j 2 --timeout 3000`
  -> `100% tests passed, 0 tests failed out of 5` (`Total Test time (real) = 408.27 sec`; story_elza 272,29 s, story_leon
  408,19 s, r34n_b_cursor 83,59 s, r36_elza_irons 11,46 s, r38_kleinkram_irons 25,95 s). Damit sind die beiden in der
  Gegenpruefung (H2) als "seit den Merges nicht gelaufen" genannten Irons-Riegel gruen.
* `-R "^(integration_r35_zgirl_5090|integration_r37_sperre1050|integration_r41_stage3szenen_a_3061|integration_r37_itemband_lift)$" -j 2 --timeout 3000`
  -> `100% tests passed, 0 tests failed out of 4` (`Total Test time (real) = 1394.46 sec`; zgirl_5090 181,89 s (6030 Slot 3 ->
  5090), sperre1050 267,32 s (u. a. Elza 50B1 nach dem Ada-Sweep), stage3szenen_a_3061 70,28 s (3061 -> 30C1 als Sherry,
  ohne die Marvin-Szene), itemband_lift 1394,38 s). Nachbar-Riegel der beruehrten Raeume, alle gruen.
* Generatoren reproduzierbar: `texte_bauen.py` und `szenen_bauen.py` in eine Scratch-Datei laufen lassen (`logs/` bzw.
  `st80_abn1/werkzeuge/gen_repro.py`) - inhaltsgleich mit `gen/story_r36_texte.inc` und `gen/story80_szenen.inc` (nur CRLF
  der Arbeitskopie verschieden). Die Szenen-Programme sind gegen die RDT-Bytes geprueft (`logs/szenen_pruef.py`, 0 Abweichungen).

Gruene Riegel heissen hier nicht "M1 behoben": kein Riegel prueft die Reihenfolge "Fuehrerstand zuerst" auf ihre Story-Logik
(P4b schreibt den Widerspruch fest).

## 6. Restmaengel (nicht blockierend)

* **N1 (mittel, Release):** Die Aufnahmen liegen nur unversioniert im Hauptbaum (`git status` dort:
  `?? synchro/STAGE3/room30C1/`, `?? synchro/STAGE3/room30E1/`, `?? synchro/STAGE6/`, `?? synchro/STAGE5/`; `git ls-files` fuer
  diese Ordner = 0 Dateien, unversioniert 126 wav, davon 51 in room30C1/room30E1/room6030). Im Integrationsbaum fehlen die Ordner
  ganz; ohne sie bleiben alle Zeilen von 30C1, 30E1 und 6030 (auch die unveraenderten) stumm. Vor dem Paket einchecken bzw. ins Paket nehmen (UEBERGABE "synchro/ 45 offene Eintraege").
* **N2 (gering):** 6030 msg 21 ist ein Dreizeiler, Zeile 3 liegt auf dem unteren Kinobalken (lesbar, ganz im Bild). Wie die 28
  Original-Dreizeiler liegen, ist weiterhin nicht gemessen.
* **N3 (gering, Doku):** TEXTE_UMSETZUNG Abschnitt 3 und 7 fuehren die Aufnahmen noch als "neu"/"stumm" (Stand 2026-11-02);
  alle sind am 2026-11-03 geliefert. Die Aufnahmeliste enthaelt die AN-Dateien 6030 main27-30 und 5090 main40-43; deren Code
  ist Welle 2 und nicht im Integrationsbaum, die Dateien spielen bis dahin nie.
* **N4 (gering, Prozess):** Merge 7d8634051 heisst "Abnahme bestanden", eine Abnahme gab es nicht (Gegenpruefung H1); dies ist
  die erste.
* **N5 (Kenntnis):** 6030 msg 6 "Leon: Where's Annette?" - Annette nennt ihren Namen nirgends (6000 msg 0..4); Mod-Name
  (ABGLEICH B16), von ST80 nicht beruehrt.
* **N6 (gering):** "Irons nach Fehldruck" fuer Leon in 1150 nur im Unit-Riegel `unit_r41_story80_hebetisch1151` gemessen (Id 2);
  im exe-Lauf nur der Fehldruck (Id 21). Leons Irons-Zeile ist ein Dialog ueber das SCD, nicht der betroffene Untersuchen-Weg.

## 7. Fragen an den Nutzer (fuer die Release Notes, Sie-Form)

1. **Zug, Fuehrerstand vor dem Wagen (M1):** Sie koennen in ROOM6030 zuerst nach vorne zu Annette gehen, ohne im Wagen mit John
   und Sherry gesprochen zu haben. Dann sagt Leon "There's a little girl in the back... Sherry." und fragt danach im Wagen "Who are
   you two?". Wie moechten Sie das loesen: (a) vorne ohne Wagengespraech nur "Everyone we could find. Can you start this thing?"
   und Annettes Sherry-Zeilen erst nach dem Wagengespraech, (b) die Tuer nach vorne bis zum Wagengespraech sperren (mit
   Hinweistext) oder (c) so lassen?
2. **Ihre Aufnahmen:** Die neuen Zeilen (30C1 main02-05, 30E1 main02/03, 6030 main01/03/05/07/08/11/12/21-26) und die ganzen
   Ordner `synchro/STAGE3/room30C1`, `synchro/STAGE3/room30E1`, `synchro/STAGE5` und `synchro/STAGE6` liegen nur in Ihrem
   Arbeitsordner, nicht im Repository. Duerfen wir sie fuer v0.8.30 einchecken bzw. ins Paket nehmen? Ohne sie bleiben diese
   Szenen im Paket stumm.

## 8. Dateien

* Bericht: `analysis/befunde_runde41/ST80_abnahme_1.md`
* Bilder: `analysis/befunde_runde41/ST80_belege/abn1/6030_kabine_zuerst.jpg` (+ `_klein`), `6030_wagen.jpg` (+ `_klein`),
  `6030_msg21_dreizeiler_ausschnitt.png`, `30c1_kurze_fassung.jpg` (+ `_klein`), `30e1_john_a1.jpg` (+ `_klein`),
  `1151_1150_hebetisch_irons.jpg` (+ `_klein`)
* Laufauszuege und Werkzeuge: `analysis/befunde_runde41/ST80_belege/abn1/logs/` (kabine1, kabine4, kabine5, leon6030wagen,
  elza30c1, elza30c1_ton, elza30e1, elza1151, leon1150; spracherkennung_abn1.json; lauf.sh, asr.py, szenen_pruef.py,
  msgzeiten.py, sca.py, tueren.py, disc30e1.py, blatt.py, gen_repro.py)
* Rohdaten (Framedumps, volle Logs): `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/testint/st80_abn1/`

Nichts committet, nichts unter pri/, synchro/, Version_80%/ geaendert; Spurbaeume und Hauptbaum nur gelesen.
