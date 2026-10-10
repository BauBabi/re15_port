# Runde 36 — Spur P "story" (Bau-Teil) — Dossier

Baum: `.claude/worktrees/r36_story`, Zweig `r36/story`, Basis master 78a4a748.
Auftrag: AUFTRAG.md Nachtrag 14:00 + VERTRAG.md Zeile P. Analyse-Teil: `analysis/story_leon_elza/`.

Punkte (Wortlaut des Nutzers):
1. P1 "Ja, wenn es mehr sinn macht, das Roy erst da liegt, wenn der Strom eingeschaltet wurde, dann machen wir das so."
2. P2 "bei Leon ein Text an Elzas Zelle (The cell door is open...) - die leere Zelle als Spur. - kannst du machen."
3. P3 "ROOM1090. Elzas 1091 ist bytegleich Leons Zustand nach der Rettung - Leon war zuerst dort. Hinnehmen und eine Spur ergaenzen (burnt-out shed) << kannst du machen" — KEIN leerer Feuerloescher.
4. P4 "Die duerfen sich niemals begegnet sein, das heisst - alle Textstellen, die darauf hindeuten das sie sich begegnet waeren, muessen wir umschreiben."
5. P5 Prison Key / Funkraum: "kann genauso gut sein, das sie es versucht hat, aber es eben nicht ging, weil die Kommunikationsanlage ja zerstoert war."

Stand: in Arbeit (Abschnitte werden fortlaufend gefuellt).

## 0. Vorgehen / Werkzeuge

* Bau: `bash re15_port/tools/local_build.sh configure|build` im Spurbaum (2026-10-11 00:10, LOCAL-BUILD-OK).
* SCD-Dumps: `re15_port/tools/scd_dump_room.py <RDT>` (opcode-exakt, Laengen aus scd_vm.c).
* Nachrichten roh: eigenes Lesewerkzeug (RDT+0x3C Nachrichtentabelle, Text bis `01 xx`).
* AOT-Belegung nach dem echten Raumstart: `build/tests/unit/probe_r30_irons-diary-welt.exe aots <raum>`
  (laedt RDT, faehrt main00/sub00 ueber scd_room_reenter hoch, listet g_aot.slots).
* Port-Strombit gemessen: `build/tests/unit/r31_generator.exe` (unit_r31_generator, gruen).

## ⛔ Befund zur Reihenfolge (aendert den Einbauort der Text-Overrides)

VERTRAG/Auftrag sehen die Text-Overrides als Installer-Zeile am Ende von scd_room_reenter vor. Der
Tuer-Weg laedt die Nachrichtentabelle aber NACH scd_room_reenter:
`engine/src/room_common.c` Schritt (7) `scd_room_reenter(...)` (Z. 355), Schritt (11)
`re15_msg_load_room_block(c->rdt->messages, ...)` (Z. 397). `re15_msg_load_room_block`
(msg_common.c Z. 312ff) UEBERSCHREIBT die Ids 0..n-1 des Raums. Ein Override bestehender Ids, der in
scd_room_reenter abgelegt wird, waere im Tuer-Weg also sofort wieder weg. Der Boot-/CONTINUE-Weg laedt
die Tabelle dagegen VOR den Installern (main.c Z. 4484) und geht gar nicht durch scd_room_reenter
(main.c Z. 4775ff, eigene Installer-Zeilen). Folge fuer den Bau:
* P4 (Overrides bestehender Ids) haengt am ENDE von `re15_msg_load_room_block` (1-Zeilen-Haken in
  msg_common.c) — dort laufen alle Wege durch (Tuer, Boot/CONTINUE, PSX asset_psx.c Z. 701, Tests).
* P1-P3 (Prop/AOT + neue Ids 20 in Raeumen OHNE Nachrichtenblock) haengen am Installer-Block von
  scd_room_reenter (Ende, "Runde 36 Spur P") UND am Boot-/CONTINUE-Block in main.c. Fuer 1210/1091
  kehrt re15_msg_load_room_block frueh zurueck (`block==NULL`, msg_sec=0) und loescht Id 20 nicht.

## P1 — Roys Leiche ROOM1020 erst nach dem Strom

### Messung vorher
Echter Raumstart ROOM1020 ohne Flags (`probe_r30_irons-diary-welt.exe aots 1020`, Basis 78a4a748):
`[scd] Obj_model_set[6] id=0x06 type=0 pos=(-4488,0,-10684) rot=(0,3072,0)` (Roy liegt da) und
`AOT slot= 5 type=0 flags=0x31 band=0x00 evt=2 mitte=(-4350,-9600) halb=(500,500)` (Anklicken -> sub02
-> msg 0). (4,0xF3) ist dabei 0 -> der Port zeigt Roy OHNE Strom.

### RE-Belege
* Roy-Record ROOM1020.RDT main00 @0x01DEE (selbst gelesen):
  `2d 06 00 00 01 00 0a 00 00 00 78 ee 00 00 44 d6 00 00 00 0c ...` = Obj_model_set obj 6,
  (-4488,0,-10684), rot y 3072 — UNBEDINGT (kein Ifel_ck davor; erster Ifel_ck @0x01E10).
  Elzas ROOM1021 main00 @0x01E40 traegt dieselben Lage-Bytes `78 ee 00 00 44 d6 00 00 00 0c`
  unter Ck(3,0x5F) @0x01E28 (D_gegenpruefung.md Nr. 2).
* Anklick-Platz ROOM1020 main00 @0x01DDA: `2c 05 03 31 00 00 0e ed 8c d8 e8 03 e8 03 ff 00 18 02 00 00`
  = Aot_set Slot 5, sce 3 (Ereignis), Flags 0x31, Rechteck (-4850,-10100,1000,1000), Nutzlast -> sub02.
  sub02 @0x01F70 `46 05 00 00 00 00 00 00 00 00` (Aot_reset Slot 5 sce 0 = Platz still), @0x01F86
  `2b 00 ff ff` = Message_on msg 0 "It's the RPD officer, Roy.He appears to have died recently." (@0x2034).
  Keine weitere Stelle zeigt msg 0 (einziges Message_on 0 im Raum).
* Strom-Bit = (4,0xF3): einziger Setzer ROOM11F0/11F1 sub18 @0x016F6 `22 04 f3 01` (selbst gelesen,
  flag_xref.txt FLAG (4,0xF3): 2 SET, 38 CK in 13 Raumpaaren). (4,0x06) ist KEIN Strom-Bit, sondern
  ROOM11B0 main00 @0x00F52 `21 04 f3 01` -> @0x00F56 `22 04 06 01` (Garagen-Durchgang, Folge des Stroms).
* Port fuehrt dasselbe Bit: kein Port-Code setzt Bank 4 (grep `re15_game_flag_set(4` = 0 Treffer in
  engine/src + main.c); der Generator laeuft ueber das Original-sub18 — gemessen
  `r31_generator.exe`: "ROOM11F0: ... sub18 (4:243) im Bild 50" / "[OK  ] sub18 laeuft im selben Bild
  (4:243 @0x016F6)" (ctest unit_r31_generator: 100% tests passed, 0 tests failed out of 1).

## P2 — ROOM1210 Text an der leeren Zelle

### Messung vorher
ROOM1210.RDT hat keinen Nachrichtenblock (RDT+0x3C = 0, A_zensus). Echter Raumstart
(`aots 1210`): Slots 0-5 Tueren, 6 AUTO (sub02), 7/8 Items, 49-63 Kamerazonen — KEIN Text-Platz an der
Zelle. Freie Slots 9..48.

### RE-Belege
* Elzas Zellentuer-Platz ROOM1211 sub00 @0x01DEA `21 04 ec 00` (solange John noch drin ist) ->
  @0x01DEE `2c 08 01 31 00 00 5c aa 00 a8 0e 03 d4 06 0a 00 ff ff 00 00` = Aot_set Slot 8, sce 1
  (Text), Flags 0x31 (Aktion + 620-Vorwaertspunkt), Band 0, Rechteck (-21924,-22528,782,1748),
  msg 10, Pausemaske 0xffff. msg 10 @0x0259D: `04 02 25 50 3a 4f 00 48 4b 3f 47 41 40 57 02 00 25 50 ...
  08 45 4a 4f 45 40 41 57 57 57 01 00` = "It's locked." [Seite] "It seems that there is someone" /
  "inside..." — Form des neuen Texts: Kopf `04 02`, Seitenumbruch `02 00`, Zeilenumbruch `08`, Ende `01 00`.
* Zellentuer-Modell: obj 0 (-21750,0,-20760) in BEIDEN Varianten; Leon ROOM1210 main00 @0x01E4E rot y
  1216 (offen), Elza ROOM1211 main00 @0x01D86 rot y 1225, dort setzt sub00 @0x01E02-06 (`2e 03 00`,
  `33 00 00 ...`) die Drehung auf 0 (zu), solange (4,0xEC)==0. Bei Leon ist die Tuer also OFFEN — "The
  cell door is open..." passt zum Bild.

## P3 — ROOM1091 Text am ausgebrannten Schuppen

### Messung vorher
ROOM1091.RDT hat keinen Nachrichtenblock. Echter Raumstart (`aots 1091`): Slots 0/1 Tueren, 2 AUTO
(sub02), 4 Shotgun Shells (r35 Spur F), 39-63 Kamerazonen. Kein Text-Platz am Schuppen. Frei: 3, 5..38.

### RE-Belege
* Leons Feuer-Platz ROOM1090 sub00 @0x02306 `06 00 24 00 21 03 85 00` (solange kein Loescher) ->
  @0x0230E `2c 02 01 b1 01 00 64 00 14 05 3a 07 66 08 de 0d 30 f8 08 07 10 f5 07 00 ff ff 00 00`
  = Aot_set Slot 2, sce 1 (Text), Flags 0xB1 (Bit 0x80 = Viereck-Form, 28 B), Band 1, Viereck
  (100,1300) (1850,2150) (3550,-2000) (1800,-2800), msg 7 "I must hurry up and get something to / put
  out this fire to save that woman!" (@0x028E6). Mit Loescher @0x02336 dasselbe Viereck als sce 3
  -> sub06 (Loeschen). Nach dem Loeschen ((3,0x81)=1) legt Leons 1090 KEINEN Platz mehr an.
* ROOM1091 main00 @0x02202 obj 2 `... 6a 09 a3 f5 2e ff 40 00 40 07` = Leons Nach-Rettungs-Lage
  (1090 sub00 @0x0237E), kein Typ 0x26 (Feuer) — der Schuppen ist bei Elza bereits geloescht.

## P4 — Text-Sweep "nie begegnet"

### Messung vorher (Sweep)
Quellen: analysis/story_leon_elza/texte_alle.json (alle Nachrichten beider Varianten, Dekoder =
Port-Glyphentabelle) — alle Dialogzeilen (Modus 00) beider Szenarien gelesen (221 Leon, 248 Elza) +
Untersuchungstexte (Modus 02) per Schlagwortsuche (someone|whoever|he|she|cop|girl|friend|biker|...).
Treffer und Urteil siehe Tabelle in "Umsetzung (alt -> neu)".

ROOM3060 (Leon-RDT) msg 1-8 = Elza/Sherry/John-Dialog: UNBENUTZT. Beleg: ROOM3060 hat ein eigenes
Elza-Gegenstueck ROOM3061.RDT (Raumlader Elza = Leon + 1, PSX.EXE @0x800397e4 `srl a0,a0,31` /
@0x800397ec `addu a0,a0,v0`, D_gegenpruefung Nr. 1); im SCD von ROOM3060 (main00 @0x0E76..0x0F1C,
sub00..sub03 @0x0F24..0x10E8, scd_dump_room.py) steht genau EIN Message_on: sub02 @0x01034
`2b 09 80 ff` = msg 9 ("For some reason the door isn't locked anymore..."); dazu ein Text-Platz
main00 @0x00EBE (Nutzlast `0a 00` = msg 10). msg 1-8 werden nie angezeigt -> keine Aenderung noetig
(Elzas eigene Szene laeuft aus ROOM3061 mit deren eigenen Texten).

## P5 — Funkraum / Prison Key
### Pruefung
ROOM10F0 und ROOM10F1 sind bytegleich (A_zensus). msg 0 @0x1174: "A large communications device.It's out
of commission.I can't send any messages from / here." Elza ROOM1021 msg 4 @0x23AC: "John, I couldn't call
for help using the radio." Leon ROOM10D0 msg 22/23, ROOM1150 msg 8/9 ("the system is damaged",
"some officers were trying to repair it"), Port-Szene 10F0 (cut_10f0.c k_msg16 "looks like we can't
contact anyone with this thing anymore") — KEIN Text behauptet eine Reihenfolge Elza-vor-Leon; alle
sagen nur: die Anlage ist kaputt. Das traegt die Lesart des Nutzers ("sie hat es versucht, ging nicht,
Anlage zerstoert"). Kein leerer Haken fuer Leon (Nachtrag 14:00). -> keine Textaenderung.

## Tests
## OFFEN
## Fuer den Nutzer
