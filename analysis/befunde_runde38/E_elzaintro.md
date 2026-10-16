# Runde 38 — Spur E "elzaintro" (Dossier, fortlaufend)

Baum `.claude/worktrees/r38_elzaintro`, Zweig `r38/elzaintro`, Basis 1bcbd0ff. Start 2026-10-16 16:35.

## Punkte (Wortlaut AUFTRAG.md)
1. "Bei Elzas Intro fehlt das Voiceover. Es ist das gleiche wie bei Leon."
2. "Im blackscreen vom preintro Text, scheint die map durch vom Raum bei Elza"

## Stand
- 16:35 Dossier angelegt, Pflichtlektuere AUFTRAG/VERTRAG/UEBERGABE gelesen.

## Punkt 1 — Voiceover Elza-Intro
### Messung vorher
(laeuft)
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
(offen)
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
