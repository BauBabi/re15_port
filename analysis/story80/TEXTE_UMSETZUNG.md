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

ROOM6030: RDT-Nachrichten 0..20 (Sektion @0x01394). Diese Spur belegt **21..26** (Nachbesserung 1 dazu **31** und **40**,
Abschnitt 8; 27..30 hat Spur AN genommen). Ursprünglich frei für andere Spuren: **27..31**
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

**Volle Suite ohne integration_*** (`ctest -E "^integration_" -j 6`, 1078 Tests, 2668 s): 1070 grün. Rot waren:
`unit_r37_sperre1050_elza50b1` (Folge dieser Spur, angepasst, Abschnitt 6.3, danach grün) und sieben Skript-Riegel
ohne Bezug zur Spur: `unit_r35_android_bash_mutanten` (unter Last 1498 s gegen Deckel 1500, einzeln wiederholt: grün in
1041 s), `unit_r38_releaseskript_trocken/_ablauf/_fehler_bau/_fehler_umfeld/_cmd` und `unit_r39_titelversion_ablauf`
(je Deckel 1200 s). Ursache der sechs gemessen: Das Release-Skript hängt nach „Container-Engine bereit: Attrappe“ in
`df -Pk <fixture>/repo` (Git `df.exe`); dieselben hängenden `df.exe` liefen zur selben Zeit in den Arbeitsbäumen
r41_moosgang, r41_gatorfix und r41_granate, und die Prozesse lassen sich nicht beenden (taskkill: „keine Instanz“).
Umgebungsfehler (WSL/Docker-Desktop-Mounts), keine Datei unter `release/` geändert.
Dazu echte exe: `integration_r34n_b_cursor` (Hebetisch, 174 s) und `integration_r38_kleinkram_irons` (37 s) grün.

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

## 8. Nachbesserung 1 (Zug 6030, Reihenfolge)

Anlass: Abnahme 1 (`analysis/befunde_runde41/ST80_abnahme_1.md`) M1, blockierend. Entscheid des Orchestrators:
Variante (a) — vorne ohne Wagengespräch eine Fassung ohne Sherry, Annettes Sherry-Zeilen erst nach dem Wagengespräch.

### 8.1 Beleg (ROOM6030, `tools/scd_dump_room.py` auf `shared_assets/PSX/STAGE6/ROOM6030.RDT`)

| Stelle | Bytes | Bedeutung |
|---|---|---|
| main00 @0x00C52 | `3b 01 02 31 01 00 ac db ...` | Tür Slot 1, Ziel 6030 Cut 0 (Führerstand) |
| main00 @0x00CB2 | `2c 04 0c 31 80 00 60 cd ...` | Treppe Slot 4 sce 12, Rechteck (-12960,-23500,2180,680) |
| main00 @0x00CC6 | `2c 05 05 41 00 00 ee b2 ...` | Wagen-Marke Slot 5, (-19730,-25570,1600,6300) |
| sub00 @0x00D28 / @0x00D2C / @0x00D32 | `13 0a 0c 00` / `14 00 04 00 00 00` / `18 07` | Switch(work_vars[10]) Case 0 -> Gosub sub07 |
| sub01 @0x00D62 / @0x00D6A / @0x00D70 | `21 03 16 00` / `23 00 01 00 05 00` / `04 ff 18 02` | einziger Leser des Wagen-Flags: Ck(3,22)=0, Marke 5 -> Evt_exec sub02 |
| sub02 @0x00D7A | `22 03 16 01` | einziger Schreiber: Set(3,22)=1 (erste Zeile der Wagen-Szene) |
| sub07 @0x010D0 / @0x010E8 | `21 04 17 00` / `04 ff 18 08` | Ck(4,23)=0 -> Evt_exec sub08 — **ohne** Abfrage von (3,22) |
| sub07 @0x010EC / @0x010F4 / @0x0110C | `07 00 28 00` / `21 03 59 00` / `04 ff 18 09` | sonst: Ck(3,89)=0 (Annette da) -> Annette + Evt_exec sub09 |
| sub08 @0x0113A | `22 04 17 01` | Set(4,23)=1, Führerstand-Szene gesehen |
| sub09 @0x01376..@0x0138C | `2e 02 00 00 3f 00 12 00 09 0a 50 00 3f 00 13 00 43 00 04 00 01 00` | Annette am Pult (Haltung 18/19, Plc_flg 4) |

Die Lücke ist damit im Original selbst angelegt: sub07 startet sub08 unabhängig davon, ob die Wagen-Szene lief. Der Port
macht die Verzweigung am Wagen-Flag (3,22) — `story80_r41.c` liest es mit `re15_game_flag_get` (dieselbe Tabelle wie Ck,
Handler 0x8003fcf4) vor dem Umleiten des Evt_exec.

### 8.2 Umsetzung (kein Asset-Patch, Programme aus Original-Bytes, `szenen_bauen.py`)

| Aufruf | Bedingung | Programm | Zeilen |
|---|---|---|---|
| sub07 @0x010E8 -> sub08 | (3,22)=1 (Wagen zuerst) | `k_s80_6030_sub08` (644 B, unverändert) | 11, 12, 25, 26, 13..19 |
| sub07 @0x010E8 -> sub08 | (3,22)=0 (vorne zuerst) | `k_s80_6030_sub08_vorne` (576 B) | 11, **31**, 13..19; setzt (9,128) |
| sub07 @0x0110C -> sub09 | (9,128)=1 und (3,22)=1 | `k_s80_6030_sub09_nachtrag` (238 B) | **40**, 25; löscht (9,128) |
| sub07 @0x0110C -> sub09 | sonst | Original-sub09 | — |

`k_s80_6030_sub08_vorne`: Original @0x0113A..@0x0113E (Set(4,23)), Set(9,128)=1 (`22 09 80 01`, Form @0x0113A),
Original @0x0113E..@0x01198 (Sperren, Cut 1, Drehungen, msg 11 + Gesten, Work_set Leon), msg 31 (`2b 1f 00 00`, Form
msg 12 @0x01198), Leons Gesten nach msg 12 @0x0119C..@0x011CC, dann Original @0x011CC..@0x01376 (ab „Certainly.“ bis
Evt_end).

`k_s80_6030_sub09_nachtrag`: Set(9,128)=0 (`22 09 80 00`, Form @0x0136C), sub08 @0x0113E..@0x01174 (Sperren, Cut 1,
Leon und Annette drehen sich zueinander, Warten auf (5,0x21)), Work_set Leon @0x01194, msg 40 (`2b 28 00 00`, Form
@0x01198), Leons Gesten @0x0119C..@0x011CC, Work_set Annette @0x011CC, msg 25 (Form msg 13 @0x011D0), Annettes Gesten
@0x012EA..@0x01306 (wie im bisherigen Programm), Schluss wie sub08 @0x0131E..@0x01376: Annette dreht zum Pult, Cut 0,
Pult-Haltung 18/19 + Plc_flg 4 (= Inhalt von sub09), Leon dreht ab, Sperren auf, Cut_auto, Plc_ret, Evt_end.

Port-Bit (9,128) „Sherry-Nachtrag offen“ (`RE15_S80_BIT_NACHTRAG`): Bank-9-Zensus 2026-11-07
(`tools/r36_kartenbits/bank9_zensus.py`) — im Original frei (Block 110..133), im Integrationsbaum und in allen
r41-Zweigen unbelegt (belegt sind dort 110-117, 125; moosgang 126/127, annette 133). Eingetragen in
`tests/unit/test_r36_kartenbits.c` k_port[]. **Nachtrag im VERTRAG Runde 41 §1.1 steht noch aus (Orchestrator).**
Gespeichert mit g_game.flags; alte Spielstände tragen 0 -> nie ein Nachtrag (kein Widerspruch).

### 8.3 Texte (Herkunft je Satz; `texte_bauen.py`, Kopf/Schwanz Leon = msg 12 @0x015FC)

| Id | Zeile | Herkunft | Aufnahme |
|---|---|---|---|
| 31 | Leon: Everyone we could find. / Can you start this thing? | Satz 1 = msg 12 (Nutzerwortlaut ST80); Satz 2 = msg 26 (Nutzer ST80), zugleich Schluss der Runde-36-Zeile 12 und der Mod-Zeile ROOM6030 msg 12 @0x015FC („Keeping the monsters at bay. Can you start this thing?“) | **keine** — stumm |
| 40 | Leon: There's a little girl / in the back... Sherry. | Satz 2 von msg 12 (Nutzerwortlaut ST80) | **keine** — stumm |

Kein Satz ist neu erfunden; beide Zeilen sind Teile vorhandener Nutzerzeilen. Für beide gibt es keine Aufnahme
(`synchro/STAGE6/room6030/main31.wav` / `main40.wav` fehlen), sie bleiben stumm, bis der Nutzer sie liefert.
Id 31 liegt unter PSX `MSG_TABLE_N` 32; Id 40 folgt VERTRAG Runde 41 §1.2 („Port-Ids ab 40“, 27..30 hat Spur AN) und
wird nur auf dem PC angezeigt (PSX: `re15_msg_install_text` verwirft Ids >= 32 ohne Absturz). PSX-Syntaxprüfung
`story80_r41.c`, `story_r36.c` mit PSn00bSDK 0.24 `mipsel-none-elf-gcc -fsyntax-only -ffreestanding`: ohne Fehler.

### 8.4 Ablauf beider Reihenfolgen (gemessen, echte exe, normale Eingaben)

**Vorne zuerst** (Lauf `weg1`/`bild_vorne`, Weg der Abnahme 1 Lauf kabine5 als Eingabe plus Rückweg; = Riegel P4j):

| Bild | Zeile |
|---|---|
| F671 | Annette: Is everyone aboard? (11) |
| F791 | Leon: Everyone we could find. Can you start this thing? (31) |
| F911..F1917 | 13..19 („Certainly.“ .. „Good luck to both of us!“), Spieler frei ab F2466 |
| F3200..F4815 | Wagen: 0, **1 „Who are you two?“**, **21 Johns Vorstellung (Sherry)**, 2, 3, 22, 4, 5, 6, 7, 23, 24, 8 |
| F5460..F5820 | Rückweg: L0.40 (Gier 1045 -> -107), XU3 (an SCA #3/#2 entlang nach (-13307,-22758)), Treppe, Tür Slot 1 |
| F5838 | Leon: There's a little girl in the back... Sherry. (40) |
| F5958 | Annette: Sherry...? My daughter is on this train? ...Thank God. (25) |
| F6300 | Szene aus, Cut 0, Spieler frei |

Jede Zeile genau einmal (24 Zeilen), kein msg 12/26. Erneutes Betreten danach: sub09 Original, kein Text (Unit-Riegel).
Bildleiste: `analysis/befunde_runde41/ST80_belege/nb1/6030_vorne_zuerst.jpg` (+ `_klein`; RE15_FRAMEDUMP-Serie,
F720 .. F6330, Renderer des Nutzers, kein SOFTWARE_RENDER/AUTOSHOT). Laufauszüge (Eingabe, Karte, `[msgtext]`, Lagen):
`ST80_belege/nb1/logs/weg1.txt`, `weg2.txt`, `bild_vorne.txt`, `gegenprobe_alt_p4b.txt`, Laufskript `lauf.sh`
(Messkopie unter eigenem Namen, nur eigene PID). exe sha1 f896e5a29e5c45bac77095d7fad84f537775985a.

**Wagen zuerst** (Lauf `weg2`, P4a-Weg plus derselbe Rückweg; = Riegel P4k): Wagen 0, 1, 21, 2, 3, 22, 4, 5, 6, 7, 23,
24, 8, dann vorne 11, 12, 25, 26, 13..19 wie bisher; kein msg 31/40, kein Nachtrag.

Danach geht das Spiel in beiden Reihenfolgen gleich weiter: (4,23)=1 nach dem Führerstand, (3,22)=1 nach dem Wagen,
Sperren (2,7)/(1,27) frei, keine anderen Flags berührt ((3,89), Batterien, Tür nach 5090 unverändert).
Bleibt schief (wie Runde 36 und Abnahme 1 vermerkt): Leon fragt in der Reihenfolge „vorne zuerst“ im Wagen „Where's
Annette?“ (msg 6), obwohl er sie schon getroffen hat.

### 8.5 Riegel

| Test | Inhalt |
|---|---|
| `unit_r41_story80_reihenfolge6030` (neu) | VM: vorne vor dem Wagen (11, 31, 13..19, keine Sherry-Zeile, (9,128)=1) -> erneut vorne (nichts) -> Wagen (13 Zeilen, msg 1 vor 21) -> vorne (Nachtrag 40, 25, (9,128)=0, Cut 0, Sperren frei) -> nochmals vorne (nichts); jede Zeile höchstens einmal |
| `unit_r41_story80_ablauf6030` | Wagen -> Führerstand mit reisenden Flags (volle Fassung, (9,128)=0), danach erneut vorne: nichts |
| `unit_r41_story80_texte` / `_stimmen` | + msg 31/40 (Breiten 203/166 bzw. 171/137 px), beide als stumm ausgewiesen |
| `unit_r36_story_texte` | 8 neue Dialog-Ids; Ids < 32 oder 40..63 |
| `unit_r36_kartenbits` | Bit 128 in k_port[] |
| `integration_r36_story_leon` | P4a „15 Nachrichten“; **P4b** CONTINUE vorne mit (3,22)=0: 11, 31, 13, keine Sherry-Zeile, kein msg 12/25/26/40; **P4h** mit (3,22)=1: 11, 12, 25, 26, 13 wie bisher; **P4i** mit (3,22)=1, (4,23)=1, (9,128)=1: Nachtrag 40, 25, kein 11/13/31 |
| `integration_r36_story_zug` (neu) | **P4j** vorne zuerst mit normalen Eingaben (kabine5 + Rückweg): Reihenfolge, jede der 24 Zeilen genau einmal, bis msg 21 keine Sherry-Zeile; **P4k** Wagen zuerst mit normalen Eingaben: heutiger Ablauf, kein 31/40 |

Läufe (Git-Bash, `PATH=/c/msys64/mingw64/bin:$PATH`, `SDL_ASSERT=always_ignore`, Integrationsbaum-Bau):
- `-R "^(unit_r41_story80_|unit_r36_story_|unit_r36_kartenbits$)" -j 4` -> `100% tests passed, 0 tests failed out of 13`
- `-R "^(unit_r41_story80_|unit_r36_story_|unit_r36_kartenbits$|unit_r35_affen_|unit_r36_dachkiste_|unit_r37_fahrstuhl_|unit_r41_stage3szenen_a_|unit_r41_wasser20b0_|unit_r37_sperre1050_)" -j 4`
  -> `100% tests passed, 0 tests failed out of 64`
- `integration_r36_story_elza` Passed 271.57 sec, `integration_r36_story_zug` Passed 451.04 sec (erster Lauf; leon dort rot
  an der eigenen alten P4a-Zahl „13 Nachrichten“, berichtigt auf 15)
- `-R "^(integration_r36_story_leon|integration_r35_zgirl_5090)$" -j 2` -> `100% tests passed, 0 tests failed out of 2`
  (leon 528.16 sec, zgirl_5090 150.06 sec)

Gegenproben (Stand ad8532cd4 = exe sha1 d8529b34, Bau 2026-11-06 20:12, letzte Engine-Änderung davor c9297033c):
- neuer `test_r36_story.cmake` TEIL=leon mit der alten exe: **rot** in P4b; die alte exe zeigt vorne
  `id=12 "Leon: Everyone we could find. There's a little girl in the back... Sherry."` und `id=25`.
- `unit_r41_story80_reihenfolge6030` mit abgeschaltetem Gate der vollen Fassung (= alter Ablauf): **rot** (Texte
  11 12 25 26 ..., keine Sherry-freie Fassung, kein Nachtrag).
