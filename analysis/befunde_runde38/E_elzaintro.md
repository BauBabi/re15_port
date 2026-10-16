# Runde 38 — Spur E "elzaintro" (Dossier, fortlaufend)

Baum `.claude/worktrees/r38_elzaintro`, Zweig `r38/elzaintro`, Basis 1bcbd0ff. Start 2026-10-16 16:35.

## Punkte (Wortlaut AUFTRAG.md)
1. "Bei Elzas Intro fehlt das Voiceover. Es ist das gleiche wie bei Leon."
2. "Im blackscreen vom preintro Text, scheint die map durch vom Raum bei Elza"

## Stand
- 16:35 Dossier angelegt, Pflichtlektuere AUFTRAG/VERTRAG/UEBERGABE gelesen.

## Punkt 1 — Voiceover Elza-Intro
### Messung vorher
Stand 1bcbd0ff (= master nach v0.8.25 + Doku), Bau dieses Baums. Werkzeug `E_elzaintro_werkzeug/e_lauf.sh`
(Neues Spiel ueber Titel + Charakterwahl, `RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1` [+ `_SWITCH=1` fuer Elza],
`RE15_AUDIO_CAP_SYNC`, `RE15_WINDOW_SCALE=1`, `RE15_SCD_TRACE=1`, Bild-Waechter `e_waechter.py`).
Laeufe m2_leon / m2_elza (Ausgaben im Scratchpad, Zahlen hier):

| | Leon (ROOM1240) | Elza (ROOM1241) |
|---|---|---|
| Message_on (SCD-Trace) | F181 id0 @0x057E, F383 id1, F563 id2, F765 id3, F967 id4, F1169 id5 @0x05FA | dieselben Bilder, @0x0582 .. @0x05FE |
| `[voice] clip loaded` | main00..main04 (+ main05 nach dem Riegel) | **keine einzige Zeile** |
| msg 5 geparkt (Stimm-Riegel op_message_on) | F1169..F1179 (main04 laeuft noch) | nein |
| Uebergabe in den Folgeraum | F1431 -> ROOM1170 | F1421 -> ROOM1031 |
| Mitschnitt RMS (Mono, Mittel je 30 Bilder) F0..F150 | 0 / 0 / 344 / 785 / 1271 / 1238 | **identisch** (nur Musik) |
| Mitschnitt RMS F180 / F210 / F390 / F570 | 3763 / 2096 / 3522 / 3921 | **949 / 1159 / 1723 / 2374** (keine Stimme) |

Bildvergleich Montage Leon gegen Elza, Bild fuer Bild (1421 Bilder, `e_vergleich.py`): 232 Bilder verschieden, ALLE nur
im Untertitel (F738-739, F940-1420: Leons Text steht laenger, weil op_message_on waehrend der Aufnahme parkt). Die
Montage-Bilder selbst sind gleich. Ursache: siehe RE-Belege.
### Messung vorher — Ursache
`re15_voice_load_clip` (audio_pc.c) sucht `synchro/STAGE1/room1241/main0N.wav`; den Ordner gibt es nicht
(`git log --all -- synchro/STAGE1/room1241` leer = nie vorhanden) -> `re15_voice_play` kehrt still zurueck. Keine
Regression (nie vertont), daher kein Bisect.
### RE-Belege
**Daten (selbst gemessen, Python ueber die RDT-Bytes, `re15_port/tools/rdt_msgdump.py` + Bytevergleich):**
- Nachrichtenblock ROOM1240 @Datei 0x62C (Zeiger RDT+0x3C), ROOM1241 @0x630; Offsettabelle beider
  identisch `[12, 105, 191, 278, 370, 484]` (6 Nachrichten).
- msg 0..5 byte-gleich (je Nachricht bis einschl. 0x01-END verglichen): 92/85/86/91/113/71 Byte, alle EQUAL.
  ROOM1240 msg0 @0x638 = ROOM1241 msg0 @0x63C = `04 00 1d 00 3e 45 56 3d ...` ("A bizarre incident occurred ...").
- SCD (scd_dump_room.py): sub02 beider Raeume ruft Message_on 0..5 in derselben Reihenfolge mit denselben
  Sleeps; ROOM1240 `2b 00 00 00` @0x57E, `2b 01` @0x598, `2b 02` @0x5A0, `2b 03` @0x5BA, `2b 04` @0x5DA,
  `2b 05` @0x5FA; ROOM1241 jeweils +4 (@0x582/0x59C/0x5A4/0x5BE/0x5DE/0x5FE), weil sub02 dort mit
  `22 03 6f 01` + `22 03 70 01` (Set(3,0x6F)/(3,0x70)) statt `22 03 8b 01` beginnt. Einziger weiterer
  Unterschied: Tuerziel main00 @0x0531 = 0x17 (Leon -> ROOM1170) bzw. 0x03 (Elza -> ROOM1031).
- BSS-Standbilder `BSS/ROOM1240/BG00..08` = `BSS/ROOM1241/BG00..08` (cmp: alle 9 SAME).
- synchro/: `STAGE1/room1240/main00..05.wav` vorhanden (versioniert, 48 kHz 24 bit Stereo, 160..210
  Spielbilder lang); `STAGE1/room1241/` gibt es NICHT.
- Leons Erzaehler danach (ROOM1170 msg 8..11 "We barricaded ourselves ...") ist NICHT vertont
  (room1170 hat main00..07, 13, 14, 16). Elzas Erzaehler ROOM1031 msg 20..23 (textgleich) ebenfalls nicht.
  "Das gleiche wie bei Leon" betrifft also genau die Montage-Zeilen 0..5.

**Port-Mechanismus (audio_pc.c re15_voice_load_clip):** Pfad = `synchro/STAGE%u/room%04X/main%02d.wav` mit dem
LAUFENDEN Raum (`re15_voice_play((uint16_t)g_current_room_id, evt.sample_id)`), also fuer Elza
`synchro/STAGE1/room1241/main00.wav` -> Datei fehlt -> `re15_voice_play` kehrt still zurueck.
Kein Original-Mechanismus betroffen: RE1.5 hat keine englische Sprachausgabe (synchro/ = eigene Produktion,
scd_vm.c Kopf von scd_queue_voice). Die Zuordnung 1241 -> 1240 ist NUTZER-VORGABE ("Es ist das gleiche wie
bei Leon"); ihre FORM (Zeile fuer Zeile dieselbe Id) belegen die byte-gleichen Nachrichten + Message_on-Folgen.
### Umsetzung
(offen)
### Messung nachher
(offen)

## Punkt 2 — Pre-Intro-Schwarz: Raum scheint durch (Elza)
### Messung vorher
Welche Schwarzblende: Die Montage (ROOM1240/1241) ist bei Leon und Elza bildgleich (s. Punkt 1, nur Untertitel-Dauer).
Der Unterschied liegt im ERZAEHLER danach ("We barricaded ourselves ..." auf Schwarz): Leon ROOM1170 sub11 Cut 7,
Elza ROOM1031 sub15 Cut 13 (@0x02A74 `29 0d`).

Bild fuer Bild (Laeufe m2, 441 Erzaehlerbilder F0..F440): **438 von 441 Bildern verschieden.**
- Elza: ein 8x6-Pixel-Feld bei x 91..98, y 107..112 (320x240) mit Farben (128,120,104), (104,104,96), (72,80,72) ...,
  ab F3 (11 Pixel, Tuer-Einblendung) / F4 (46) / ab F5 dauerhaft 48 Pixel bis F440; dazu Grund (1,1,1) statt (0,0,0).
- Leon: reines (0,0,0) + Text (Sicherheitsnetz s_scene_black, main.c, Raum 0x1170 + Cut 7).
- Bilder: `E_elzaintro_bilder/vorher_erzaehler_f200_leon_links_elza_rechts.png` (Leon links, Elza rechts, das graue
  Feld = die Karte), `vorher_elza_erzaehler_f200_ausschnitt_karte.png`, `..._verstaerkt_x40.png`.
- Log Elza beim Eintritt in ROOM1031 (nach `Cut_chg(13)`): `[kartenitems] ROOM1031: Karte Blatt 2 (Bit 9,89) Prop obj
  15 bei (-1692,-2769,-23638), Zone Slot 8` und `[prop-render] pi=0 oid=0x0F pos=(-1692,-2769,-23638)` — **"die map vom
  Raum" ist woertlich der Lageplan 1F** (Runde 36 Spur N, `engine/src/kartenitems_r36.c`), ein Port-Prop an der Wand
  der Lobby.
- GEGENPROBE (eine Variable): derselbe Elza-Lauf mit `RE15_SET_FLAG=9:89` (Lageplan 1F genommen -> kein Prop,
  `re15_kartenitems_r36_install` legt nur an, solange das Bank-9-Bit fehlt): keine `[kartenitems]`-Zeile, die farbigen
  Pixel je Erzaehlerbild = genau Leons Textpixel (1875 / 1271 / 1914 / 354 / 0). Das Feld IST die Karte.

Warum nur Elza: Elzas Erzaehler laeuft IN der Lobby (ROOM1031), Leons im Helipad-Raum ROOM1170 (dort haengt keine
Karte, und das Netz s_scene_black faerbt Cut 7 ohnehin vollflaechig schwarz). Die Erzaehler-Kamera ist in BEIDEN
Raeumen dieselbe — und in ROOM1031 ist sie die Lobby-Kamera (Bytes s. RE-Belege) — sie sieht den Lageplan.

Regression: `kartenitems_r36.c` kam mit 554877f9 (Runde 36 Spur N), gemergt cb1d5f59, erstmals ausgeliefert v0.8.24
e8b3958f; in v0.8.23 3ec5b1f8 gibt es die Datei nicht (`git ls-tree`). Bisect-Laeufe: siehe Tabelle unten.
### RE-Belege
(offen)
### Umsetzung
(offen)
### Messung nachher
(offen)

## Tests
(offen)

## OFFEN
(offen)

## Fuer den Nutzer
(offen)
