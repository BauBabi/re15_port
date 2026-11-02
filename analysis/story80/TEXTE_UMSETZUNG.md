# Story-Texte umsetzen (Spur ST80-T, Runde 41)

Stand 2026-11-02, Zweig `r41/story80`. Grundlage: `ABGLEICH.md` (A1, f.2, f.3), `PORT_ZENSUS.md` (§e), Nutzerentscheide
und Nachträge des Koordinators (30E1, 30C1, 1151, 6030 Wagen und Führerstand). Dieses Dossier wird fortlaufend geschrieben.

Keine Datei unter `synchro/`, `pri/`, `Version_80%/` verändert (synchro nur gelesen). Kein Asset-Patch: die RDTs bleiben
byte-true, Texte und Szenen legt der Port beim Laden bzw. beim Start des Original-Subs ab.

## 1. f.2 — Text-Id 20 in ROOM1151 doppelt belegt (gemessen, behoben)

**Messung vor der Behebung** (`unit_r41_story80_hebetisch1151`, echter Engine-Code: Raumstart ROOM1151 mit (3,0x9D)=1 wie
nach der Hinbeuge-Szene `ROOM1151 sub01 @0x00EA8`, Aktion am Tisch = Ereignis 4, Cursor, SQUARE daneben, Text zu, CROSS,
dann echter Aktionsdruck an der Gesprächslage (-20538,-25147) Gier 1500):

```
== hebetisch ROOM1151 ==
  ok: ROOM1151 (3,0x9D)=1: Irons-Platz Slot 2 = Text 20
  ok: Hebetisch-Cursor aktiv nach 10 Bildern
  ok: Fehldruck: Text Id 20 "Nothing happened."
  ok: Fehldruck-Text geschlossen
  ok: Cursor mit CROSS verlassen (Aufraeumbytes)
   Text nach Irons-Klick: Id 20 "Nothing happened."
  FEHLER: Irons nach dem Fehldruck: Id 20 "Nothing happened." (soll 20 "He has already passed...")
  FEHLER: Hebetisch-Id 20 != Irons-Id 20
```

Ursache wie im Abgleich gelesen: `hebetisch_cursor_1150.c:314` setzt „Nothing happened.“ unter `RE15_HC_TEXT_ID` = 20,
`elza_r36.c:272` legt Irons' Text unter `RE15_ELZA_IRONS_MSG_ID` = 20 ab, der Untersuchen-Weg (`re15_scd_show_message`)
liest die Tabelle ohne neu einzusetzen.

**Behebung:** eigene Id 21 für den Hebetisch (`include/re15_hebetisch_cursor.h`). Belegung geprüft: ROOM1150 0..14 (RDT,
Sektion @0x012F8), 20 (bisher Hebetisch), 22..29 (`irons_tod_1150.c` OP_MSG), 30/31 (`re15_kartenitems_r36.h`); ROOM1151
0..3 (RDT @0x010EC), 20 (`re15_elza_r36.h`), 30/31. Id 21 kommt in keinem Arbeitsbaum für 1150/1151 vor (nur 1110, 2060-Satz
in 1050, 11B0). < 32 (PSX `MSG_TABLE_N`).

**Gegenprobe nach der Behebung:**

```
== hebetisch ROOM1151 ==
  ok: Fehldruck: Text Id 21 "Nothing happened."
   Text nach Irons-Klick: Id 20 "He has already passed..."
  ok: Irons nach dem Fehldruck: Id 20 "He has already passed..." (soll 20 "He has already passed...")
  ok: Hebetisch-Id 21 != Irons-Id 20
== hebetisch ROOM1150 ==
  ok: Fehldruck: Text Id 21 "Nothing happened."
   Text nach Irons-Klick: Id 2 "Irons: I'll be fine. Just worry about yourself for now."
```

Dazu grün: `unit_r34n_b_*` (Hebetisch, Prüfung 6 jetzt Id 21), `unit_r36_elza_*`, `unit_r38_kleinkram_*`.

## 2. Texte (Generator `tools/r36_p/texte_bauen.py` -> `engine/src/gen/story_r36_texte.inc`)

Der Generator ist um eine Art erweitert: **DIALOG_NEU** = neue Dialog-Id hinter den RDT-Nachrichten eines Raums (< 32 =
PSX `MSG_TABLE_N`), Kopf und Schwanz wörtlich aus einer Original-Nachricht desselben Raums mit demselben Sprecher
(Spalte „Vorlage“). Die Tabellenzeile trägt jetzt `art` und `vorlage` (`include/re15_story_r36.h`);
`re15_story_r36_texte` legt Overrides und neue Dialog-Ids beim Laden des Nachrichtenblocks ab, die Text-Platz-Ids P2/P3
weiter `re15_story_r36_install`. Drei Zeilen sind erlaubt: 28 Original-Dialoge haben zwei Umbrüche (z. B. ROOM10D0
msg 17). Grenze 271 px je Zeile, der Kopf zählt in Zeile 1 mit (Generator und `unit_r41_story80_texte` prüfen beides).

Entfallen gegenüber Runde 36: 30C1 msg 7 (die kurze Fassung ruft msg 7 nicht mehr; im Sweep als „unbenutzt“ erlaubt) und
6030 msg 2 (Sherry spricht wieder den Original-Satz ROOM6030 @0x01416 „She's out there fighting the monsters!“, der die
Nutzer-Zeile 4 ist; John nennt Elza vorher in msg 21).

## 3. Zeilenliste für den Nutzer (Wortlaut, Sprachdatei)

Sprachdatei = `synchro/STAGE<n>/room<ID>/main<Id>.wav` (audio_pc.c; für keine dieser Ids gibt es eine Umleitung in
`stimmen_r39.c`, geprüft in `unit_r41_story80_stimmen`). „bleibt“ = die vorhandene Aufnahme spricht schon diesen Wortlaut.
„/“ = Zeilenumbruch im Untertitel.

### 3.1 ROOM30E1 (Elza trifft John), A1

| Id | Zeile | Datei | Aufnahme |
|---|---|---|---|
| 2 | John: This is bad... those monsters / have us surrounded! | `synchro/STAGE3/room30E1/main02.wav` | neu (spricht „bit me“) |
| 3 | John: ...Damn it, there's no place / around here to hide... | `synchro/STAGE3/room30E1/main03.wav` | neu (spricht „medical supplies“) |

### 3.2 ROOM30C1 (Elza trifft Marvin), kurze Fassung

| Folge | Id | Zeile | Datei | Aufnahme |
|---|---|---|---|---|
| 1 | 1 | Elza: Who are you? | `synchro/STAGE3/room30C1/main01.wav` | bleibt (Original-Satz) |
| 2 | 2 | Officer: Marvin Branagh, RPD. | `synchro/STAGE3/room30C1/main02.wav` | neu (spricht „... I'm here with Leon Kennedy.“) |
| 3 | 3 | Elza: Are there any others still alive? | `synchro/STAGE3/room30C1/main03.wav` | neu |
| 4 | 4 | Marvin: A few. There are other / survivors out there. | `synchro/STAGE3/room30C1/main04.wav` | neu |
| 5 | 5 | Elza: Then I hope they make it out. | `synchro/STAGE3/room30C1/main05.wav` | neu |

Sprecherkopf wie heute: msg 2 trägt den Original-Kopf „Officer:“ (ROOM30C1 @0x00EF0, Farbe 07), msg 4 „Marvin:“.
msg 6..8 laufen nicht mehr; `main06.wav`, `main07.wav`, `main08.wav` werden nie gespielt (die Ids werden nicht gerufen,
eine Stummschaltung in `stimmen_r39.c` ist deshalb nicht nötig).

### 3.3 ROOM6030 (Leon), Wagen-Szene sub02

| Folge | Id | Zeile | Datei | Aufnahme |
|---|---|---|---|---|
| 1 | 0 | Leon: Hang on, Marvin. / We're getting out of here. | `synchro/STAGE6/room6030/main00.wav` | bleibt |
| 2 | 1 | Leon: Who are you two? | `synchro/STAGE6/room6030/main01.wav` | neu (spricht „Where's Elza?“) |
| 3 | 21 | John: Name's John. This is Sherry. / We made it down here with / a girl named Elza. | `synchro/STAGE6/room6030/main21.wav` | neu |
| 4 | 2 | Sherry: She's out there fighting / the monsters! | `synchro/STAGE6/room6030/main02.wav` | bleibt (spricht genau das) |
| 5 | 3 | John: She's trying to keep the outside / clear so we can escape. | `synchro/STAGE6/room6030/main03.wav` | neu empfohlen (alte Aufnahme beginnt mit „Yeah!“) |
| 6 | 22 | John: She said she'd catch up with us. | `synchro/STAGE6/room6030/main22.wav` | neu |
| 7 | 4 | Leon: Why aren't you out there? | `synchro/STAGE6/room6030/main04.wav` | bleibt |
| 8 | 5 | John: She told me to stay with the kid... / and look after your friend here. | `synchro/STAGE6/room6030/main05.wav` | neu |
| 9 | 6 | Leon: Where's Annette? | `synchro/STAGE6/room6030/main06.wav` | bleibt |
| 10 | 7 | John: The woman in the lab coat? / Up front, trying to start the train. | `synchro/STAGE6/room6030/main07.wav` | neu |
| 11 | 23 | Sherry: A lab coat...? Is it Mom?! | `synchro/STAGE6/room6030/main23.wav` | neu |
| 12 | 24 | John: Easy, kid. Stay with me. | `synchro/STAGE6/room6030/main24.wav` | neu |
| 13 | 8 | Leon: I'll find out. Stay here. | `synchro/STAGE6/room6030/main08.wav` | neu (spricht „I see. Thanks.“) |

### 3.4 ROOM6030 (Leon), Führerstand sub08 (PORT-WAHL, vom Nutzer freigegeben)

| Folge | Id | Zeile | Datei | Aufnahme |
|---|---|---|---|---|
| a | 11 | Annette: Is everyone aboard? | `synchro/STAGE6/room6030/main11.wav` | neu (spricht „Where's that biker girl?“) |
| b | 12 | Leon: Everyone we could find. There's a / little girl in the back... Sherry. | `synchro/STAGE6/room6030/main12.wav` | neu (spricht „Keeping the monsters at bay...“) |
| c | 25 | Annette: Sherry...? My daughter is / on this train? ...Thank God. | `synchro/STAGE6/room6030/main25.wav` | neu |
| d | 26 | Leon: Can you start this thing? | `synchro/STAGE6/room6030/main26.wav` | neu |
| danach | 13..19 | „Certainly.“ usw. unverändert | `main13.wav` .. `main19.wav` | bleiben |

„bleibt“ für 6030 main00/04/06 und 13..19 heißt: Wortlaut = Original-RDT-Satz, und der Abgleich (f.1) führt diese
Dateien nicht unter den abweichenden. Per Spracherkennung geprüft ist davon nur main02 („She's out there fighting the
monsters!“, `abgleich/spracherkennung.json`).

## 4. Szenen (Generator `tools/r41_story80/szenen_bauen.py` -> `engine/src/gen/story80_szenen.inc`)

Kein Asset-Patch. Der Port tauscht nur das Ziel eines Evt_exec (`op_evt_exec`, Haken `re15_story80_evt_exec`,
`engine/src/story80_r41.c`), wenn die ersten 16 Bytes des Original-Subs der Signatur gleichen:

| Raum | Aufruf | Original-Sub | Programm |
|---|---|---|---|
| 30C1 (Elza) | sub00 @0x00C02 `04 ff 18 03` | sub03 @0x00C2C (574 B, msg 1..8) | `k_s80_30c1_sub03` (458 B, msg 1..5) |
| 6030 (Leon) | sub01 @0x00D70 `04 ff 18 02` | sub02 @0x00D7A (622 B, msg 0..8) | `k_s80_6030_sub02` (900 B, 13 Zeilen) |
| 6030 (Leon) | sub07 @0x010E8 `04 ff 18 08` | sub08 @0x0113A (572 B, msg 11..19) | `k_s80_6030_sub08` (644 B, msg 11, 12, 25, 26, 13..19) |

Jedes Programm besteht nur aus ausgelieferten Byte-Bereichen desselben Raums (auf Opcode-Grenzen geprüft) und Message_on
in ausgelieferter Form mit getauschter Id. Das Werkzeug geht jedes Programm mit der Opcode-Längentabelle ab und bricht ab,
wenn es nicht genau auf Evt_end endet oder die Message-Folge nicht der Sollfolge gleicht. Ablauf = Nutzerentscheid;
Gesten und Blicke = PORT-WAHL aus Original-Blöcken desselben Sprechers. Die Bausteinliste mit Offsets steht im Kopf jedes
Programms in `gen/story80_szenen.inc`.

**30C1 kurze Fassung:** Original sub03 @0x00C2C..@0x00D9E (bis msg 5 mit Elzas Gesten), Work_set(2,0) Marvin @0x00D9E,
dann Marvins Abgang @0x00E16..@0x00E68 unverändert (Plc_motion(0,6) + Sleep 50, Blick zurück, Plc_dest + Gosub sub05,
Evt_exec sub06, Pos_set, Cut 3, Sperren auf, Cut_auto, Plc_ret). Entfallen: msg 6..8 mit ihren Gesten
(@0x00D9E..@0x00E16). Kamera: Cut 9 für das Gespräch und Cut 3 nach dem Abgang wie bisher.

**6030 Wagen:** Kopf @0x00D7A..@0x00E5C und Schluss @0x00FA2..@0x00FE8 wie Original. Neue Zeilen mit Gestenblöcken
desselben Sprechers: John msg 21 mit dem Block nach msg 5 (@0x00EEE, der längste), msg 22 mit dem nach msg 3 (@0x00E9E),
msg 24 mit dem nach msg 7 (@0x00F4A); Sherry msg 23 mit Sleep 100 wie nach msg 2 (@0x00E84). Blicke: Leon zu John =
@0x00E88 (Plc_neck auf Johns Platz, Sce_em_set sub06 @0x0101C), Leon zu Sherry = @0x00DEC (Sherrys Platz, @0x01044).
Zeile 11: Sherry dreht sich zu Leon = Work_set(2,2) @0x00E4A + Plc_dest Modus 9 Bit 0x21 in der Form von Annettes
Drehung sub08 @0x01160 mit Leons Marke @0x00E0E (-25071,-24133) als Ziel + Warten @0x01168. Zeile 12: John blickt zu
Sherry (@0x00DEC), Leon zu John.

**6030 Führerstand:** Original @0x0113A..@0x011CC (a, b), dann c = msg 25 Annette mit dem Gestenblock nach msg 18
(@0x012EA), d = msg 26 Leon mit dem nach msg 17 (@0x012C6), dann Original ab @0x011CC (msg 13 „Certainly.“ .. Evt_end).

## 5. Message-Ids ROOM6030 und Integrationsstelle r41/annette

ROOM6030: RDT-Nachrichten 0..20 (Sektion @0x01394). Diese Spur belegt **21..26**. Frei für andere Spuren: **27..31**
(< 32 = PSX `MSG_TABLE_N`, VERTRAG Runde 36 §1.2) bzw. ab 40 auf dem PC (Spur AN nahm in 5090 die Ids 40..42). Stimmen
gehen bis Id 63 (`VOICE_MAX_MSG` 64).

Zweig r41/annette (Stand ddff83389, nur gelesen): ändert 6030 noch nicht; geplant ist eine Zugstart-Szene nach dem Tor mit
neuen Ids. Berührungspunkte beim Zusammenführen:
- `scd_vm.c` `op_evt_exec`: AN hängt `re15_annette_r41_evt_exec` hinter `re15_tuersperre_r39_evt_exec` ein, diese Spur
  `re15_story80_evt_exec` vor `re15_dining_r38_evt_exec` (getrennte Stellen). Beide prüfen nur ihre eigenen (Raum, Sub).
  Wenn AN 6030 sub08 oder sub02 selbst umleiten will, muss es auf `k_s80_6030_sub08` / `k_s80_6030_sub02` aufsetzen.
- Annettes Zeile 6030 msg 18 „I'll be ready to start the train when you do!“ (AN Frage F3) bleibt in sub08 unverändert.
- Zeile 23 (Sherry: „Is it Mom?!“) und c (Annette erfährt von Sherry) setzen voraus, dass Annette im Führerstand lebt;
  ihre Szene in 5090 (AN) liegt danach.

## 6. Nachweis

### 6.1 Riegel (ctest)

| Test | Inhalt | Stand |
|---|---|---|
| `unit_r41_story80_hebetisch1151` | f.2, Abschnitt 1 | grün |
| `unit_r41_story80_texte` | 25 Zeilen der Liste nach dem Laden: Wortlaut, ≤ 128 B, ≤ 3 Zeilen, ≤ 271 px (Breiten `font_width.h`) | grün |
| `unit_r41_story80_ablauf30c1` | Szene über die VM: Texte 1, 2, 3, 4, 5 (Bild 144/264/384/504/624), Stimmen 1..5, msg 6..8 nie, danach Cut 3 und Sperren (2,7)/(1,27) frei | grün |
| `unit_r41_story80_ablauf6030` | Wagen: 13 Texte und 13 Stimmen in der Sollfolge (Bild 62 .. 1677), Sherry Gier 3072 -> 2847 (= atan2 auf Leons Marke, `actor_locomotion.c`), danach Cut 12, (3,22)=1; Führerstand: 11, 12, 25, 26, 13..19, (4,23)=1 | grün |
| `unit_r41_story80_stimmen` | 25 Pfade `main<Id>.wav`, keine Umleitung in `stimmen_r39` / `elza_r36` | grün |
| `unit_r36_story_*` | angepasst: 19 Overrides, 6 neue Dialog-Ids (Kopf/Schwanz/Dauer = Vorlage), Sweep (6030 msg 21 nennt Elza, 30C1 msg 7 unbenutzt), Reihenfolge prüft 6030 msg 1/21 | grün |
| `integration_r36_story_leon` | echte exe: P4a Tür 6000 -> 6030, alle 13 Wagenzeilen im `[msgtext]`-Protokoll, kein „Where's Elza“, kein „I see. Thanks.“; P4b Führerstand 11, 12, 25, 26, 13, kein „biker girl“ | grün (482 s) |
| `integration_r36_story_elza` | echte exe: P4f 30C1 msg 1..5, keine msg 6..8, kein „Leon Kennedy“; P4g (neu) 30E1 msg 2/3 nach A1, kein „bit me“/„medical supplies“ | grün (364 s) |

### 6.2 Bildleisten (echtes Fenster)

`tools/r41_story80/s80_lauf.sh`: eigene exe-Kopie, Renderer des Nutzers (kein AUTOSHOT, kein SOFTWARE_RENDER),
Fensterskala 2, Speicherkarte aus `probe_r36_story_karte` + CONTINUE, ffmpeg gdigrab `hwnd=` der eigenen PID, nur die
eigene PID beendet. Leisten mit `tools/r41_story80/s80_blatt.py`:

| Datei | Inhalt |
|---|---|
| `belege/30c1_kurze_fassung.jpg` | 30C1: fünf Zeilen, dann Marvins Abgang (Winken = Plc_motion(0,6) @0x00E16 wie im Original nach msg 8, Weglaufen) und Cut 3 |
| `belege/30e1_john_a1.jpg` | 30E1: msg 0, 1, **2, 3 (A1 neu)**, 4, 5 |
| `belege/6030_wagen.jpg` | 6030 Wagen: alle 13 Zeilen; Bild 140 Sherry zu Leon gedreht |
| `belege/6030_fuehrerstand.jpg` | 6030 Führerstand: a, b, c, d (neu), „Certainly.“ |

Alle Untertitel lesbar, keine Zeile über den Rand. Bei msg 21 (drei Zeilen) liegt die dritte Zeile auf dem unteren
Kinobalken (Ausschnitt Bild 68 vergrößert geprüft), ganz im Fenster und lesbar; der Text bleibt bis zu seinem eigenen
Ende stehen (Bild 66..72 bei 2 Bildern/s), msg 2 folgt danach. Dreizeiler gibt es auch im Original (28 Dialoge, z. B.
ROOM10D0 msg 17); wie diese dort liegen, ist nicht gemessen.

Szenendauer im Riegel (VM-Bilder vom Programmstart bis Evt_end): 30C1 876, 6030 Wagen 1809, Führerstand 1824. Mit den künftigen Aufnahmen wartet jede nächste Zeile ohnehin auf das Ende der laufenden
Aufnahme (`op_message_on`, Stimmen-Riegel).

### 6.3 Folgeanpassung ausserhalb der Spur

- `unit_r37_sperre1050_elza50b1`: Der Sweep aller Elza-RDTs erwartete „Ada“ genau zweimal (30C1 msg 4 Marvin, msg 5
  „Elza: Ada?“). Die kurze Fassung nennt Ada nicht mehr, Elza hört den Namen nie; erwartet jetzt 0. Das stützt 50B1
  „It's a woman. She's dead.“ (Runde 37 #5): Elza kann die Tote nicht kennen.
- PSX: `story80_r41.c`, `story_r36.c`, `hebetisch_cursor_1150.c` mit `mipsel-none-elf-gcc -fsyntax-only -DPSN00BSDK`
  (PSn00bSDK 0.24) ohne Fehler; die neuen Ids 21..26 liegen unter `MSG_TABLE_N` 32.

## 7. Offen

- Aufnahmen: alle „neu“-Zeilen aus Abschnitt 3 (Nutzer). Bis dahin spielen die vorhandenen Dateien den alten Wortlaut
  unter den neuen Untertiteln: 30E1 main02/03, 30C1 main02..05, 6030 main01, main03, main05, main07, main08, main11,
  main12. Für 6030 main21..26 gibt es noch keine Datei (die Zeilen bleiben stumm, `[voice] ... keine Aufnahme`).
- r41/annette: Integrationsstelle Abschnitt 5.
