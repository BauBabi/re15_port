# Runde 36 — Spur P "story" (Bau-Teil) — Dossier

Baum: `.claude/worktrees/r36_story`, Zweig `r36/story`, Basis master 78a4a748.
Auftrag: AUFTRAG.md Nachtrag 14:00 + VERTRAG.md Zeile P. Analyse-Teil: `analysis/story_leon_elza/`.

Punkte (Wortlaut des Nutzers):
1. P1 "Ja, wenn es mehr sinn macht, das Roy erst da liegt, wenn der Strom eingeschaltet wurde, dann machen wir das so."
2. P2 "bei Leon ein Text an Elzas Zelle („The cell door is open…") – die leere Zelle als Spur. - kannst du machen."
3. P3 "ROOM1090. Elzas 1091 ist bytegleich Leons Zustand nach der Rettung – Leon war zuerst dort. Hinnehmen und eine
   Spur ergänzen („burnt-out shed") << kannst du machen" — KEIN leerer Feuerloescher.
4. P4 "Die dürfen sich niemals begegnet sein, das heißt - alle Textstellen, die darauf hindeuten das sie sich begegnet
   wären, müssen wir umschreiben."
5. P5 Prison Key / Funkraum: "kann genauso gut sein, das sie es versucht hat, aber es eben nicht ging, weil die
   Kommunikationsanlage ja zerstört war. In die Richtung würde ich auch gehen."

Stand: **fertig** (alle fuenf Punkte gebaut/geprueft, gezielte Tests gruen, Abschnitt "Tests").

## 0. Vorgehen / Werkzeuge

* Bau: `bash re15_port/tools/local_build.sh configure|build` im Spurbaum (LOCAL-BUILD-OK).
* SCD-Dumps: `re15_port/tools/scd_dump_room.py <RDT>` (opcode-exakt, Laengen aus scd_vm.c).
* Nachrichten roh: RDT+0x3C-Tabelle wie re15_msg_load_room_block (msg_common.c), Text bis `01 xx`.
* AOT-/Prop-Belegung nach dem ECHTEN Raumstart: `build/tests/unit/probe_r30_irons-diary-welt.exe aots|pool <raum>`.
* Port-Strombit gemessen: `build/tests/unit/r31_generator.exe` (unit_r31_generator).
* Neu: `re15_port/tools/r36_p/texte_bauen.py` (Texte -> `engine/src/gen/story_r36_texte.inc`),
  `tests/unit/test_r36_story.c` (Riegel + `mess`-Standortsuche), `tests/unit/probe_r36_story_karte.c`
  (Spielstaende fuer die exe), `tests/integration/test_r36_story.cmake` (echte exe).
* Messhaken NEU: `RE15_MSG_LOG` schreibt jetzt zusaetzlich `[msgtext] room=xxxx id=N "<Wortlaut>"` je geoeffnetem
  Text (story_r36.c re15_story_r36_text_offen, gerufen am Ende von re15_dialog_open_mask = einziger Oeffner von
  Message_on UND Untersuchen-Platz). Der alte `[msg] room=.. id=..`-Eintrag nennt keinen Wortlaut und fehlt am
  Untersuchen-Weg ganz. Kein Spielverhalten (env-gegatet, PC).

## ⛔ Befund zur Reihenfolge (aendert den Einbauort der Text-Overrides gegenueber VERTRAG 1.4)

VERTRAG/Auftrag sehen die Text-Overrides als Installer-Zeile am Ende von scd_room_reenter vor. Der Tuer-Weg laedt
die Nachrichtentabelle aber NACH scd_room_reenter: `engine/src/room_common.c` Schritt (7) `scd_room_reenter(...)`,
Schritt (11) `re15_msg_load_room_block(c->rdt->messages, ...)`. `re15_msg_load_room_block` (msg_common.c)
UEBERSCHREIBT die Ids 0..n-1 des Raums. Ein Override bestehender Ids aus scd_room_reenter waere im Tuer-Weg sofort
wieder weg. Der Boot-/CONTINUE-Weg laedt die Tabelle VOR den Installern (main.c, `if (g_room_rdt.messages)
re15_msg_load_room_block`) und geht gar nicht durch scd_room_reenter (eigener Installer-Block in main.c).
Folge (und gemessen, unit_r36_story_reihenfolge: beide Reihenfolgen):
* P4 (Overrides bestehender Ids) haengt am ENDE von `re15_msg_load_room_block` (2-Zeilen-Haken msg_common.c) —
  dort laufen alle Wege durch (Tuer, Boot/CONTINUE, PSX asset_psx.c, Tests).
* P1-P3 (Prop/Platz + neue Id 20 in Raeumen OHNE Nachrichtenblock) haengen am Installer-Ende von
  scd_room_reenter ("Runde 36 Spur P") UND am Boot-/CONTINUE-Block in main.c. Fuer 1210/1091 kehrt
  re15_msg_load_room_block frueh zurueck (RDT+0x3C = 0) und loescht Id 20 nicht.

---

## P1 — Roys Leiche ROOM1020 erst nach dem Strom

### Messung vorher
Echter Raumstart ROOM1020 ohne Flags (`probe_r30_irons-diary-welt.exe aots 1020`, Basis 78a4a748):
`[scd] Obj_model_set[6] id=0x06 type=0 pos=(-4488,0,-10684) rot=(0,3072,0)` (Roy liegt da) und
`AOT slot= 5 type=0 flags=0x31 band=0x00 evt=2 mitte=(-4350,-9600) halb=(500,500)` (Anklicken -> sub02 -> msg 0).
(4,0xF3) ist dabei 0 -> der Port zeigte Roy OHNE Strom.

### RE-Belege
* Roy-Satz ROOM1020.RDT main00 @0x01DEE (selbst gelesen):
  `2d 06 00 00 01 00 0a 00 00 00 78 ee 00 00 44 d6 00 00 00 0c ...` = Obj_model_set obj 6, (-4488,0,-10684),
  rot y 3072 — UNBEDINGT (kein Ifel_ck davor; erster Ifel_ck @0x01E10). Kollisionskasten 0 (box h(0,0,0)).
  Elzas ROOM1021 legt dieselbe Leiche @0x01E40 unter Ck(3,0x5F) @0x01E28 selbst an (D_gegenpruefung Nr. 2).
* Anklick-Platz ROOM1020 main00 @0x01DDA: `2c 05 03 31 00 00 0e ed 8c d8 e8 03 e8 03 ff 00 18 02 00 00`
  = Aot_set Slot 5, sce 3 (Ereignis), Flags 0x31, Rechteck (-4850,-10100,1000,1000), Nutzlast -> sub02.
  sub02 @0x01F70 `46 05 00 00 00 00 00 00 00 00` (Aot_reset Slot 5 sce 0 = Platz still), @0x01F86 `2b 00 ff ff`
  = Message_on msg 0 "It's the RPD officer, Roy.He appears to have died recently." (@0x2034). Einziges
  Message_on 0 im Raum.
* Strom-Bit = (4,0xF3): einziger Setzer ROOM11F0/11F1 sub18 @0x016F6 `22 04 f3 01` (selbst gelesen; flag_xref:
  2 SET, 38 CK in 13 Raumpaaren). (4,0x06) ist KEIN Strom-Bit: ROOM11B0 main00 @0x00F52 `21 04 f3 01` ->
  @0x00F56 `22 04 06 01` (Garagen-Durchgang, Folge des Stroms).
* Port fuehrt dasselbe Bit: kein Port-Code setzt Bank 4 (`re15_game_flag_set(4` = 0 Treffer in engine/src +
  main.c); der Generator laeuft ueber das Original-sub18 — gemessen `r31_generator.exe`: "ROOM11F0: ... sub18
  (4:243) im Bild 50", "[OK  ] sub18 laeuft im selben Bild (4:243 @0x016F6)" (unit_r31_generator gruen).

### Umsetzung
`engine/src/story_r36.c` re15_story_r36_install (Konstanten `include/re15_story_r36.h` RE15_S36_ROY_*/STROM_*):
nur in 0x1020 (Leon), NACH dem Init-Lauf von main00, wenn (4,0xF3)==0:
* `scd_prop_hide_by_obj_id(6)` — Zeichen-Bit des Pool-Eintrags aus (dasselbe wie die Aufnahme @0x80021fc8);
* `re15_aot_retype(5, 0, 0x00, 0,0,0)` = genau das Aot_reset, mit dem das Original den Platz in sub02 @0x01F70
  stilllegt (sce 0 = Handler[0] inert, Rechteck/Band bleiben) — nur wenn Slot 5 wirklich der Ereignis-Satz ist.
Mit (4,0xF3)==1 bleibt alles Original. Elzas 1021 unberuehrt. Kein neues Bank-9-Bit noetig (VERTRAG 1.1: Bit 92
bleibt Reserve).

### Messung nachher
* `aots 1020` (ohne Flags): `[story36] ROOM1020: Strom (4,0xF3)=0 -> Roy noch nicht da (obj 6 versteckt, Slot 5
  sce 0)`, `AOT slot= 5 type=11 flags=0x00 ...` (inert), `pool 1020`: `PROP slot=6 obj_id=6 active=0`.
* unit_r36_story_roy (10 Pruefungen): ohne Strom Prop aus + Slot 5 inert + echter Aktionsdruck vor Roy
  (-4350,-8700) Blick -Z (Standort begehbar, Klemmpfad) -> KEIN Text; mit Strom Prop an (rot 3072) + Slot 5 =
  Ereignis sub02 + Druck -> msg 0 "It's the RPD officer, Roy.He appears to have died recently."; ROOM1021 mit
  (3,0x5F): kein Eingriff, Roy sichtbar.
* Echte exe (integration_r36_story_leon): P1a Tuer ROOM1010 -> ROOM1020 ohne Strom: `[story36] ROOM1020: Strom
  (4,0xF3)=0 -> Roy noch nicht da`, `[fire-aot] slot=5` -> kein `[msgtext] room=1020 id=0`; P1b CONTINUE in 1020 mit
  Strom + Aktionstaste: `[msgtext] room=1020 id=0 "It's the RPD officer, Roy.He appears to have died recently."`.
* Bild (RE15_FRAMEDUMP, RE15_WINDOW_SCALE=1, Cut 10, Bild 45): `P_story_bilder/p1_roy_1020_cut10_ohne_mit_strom.png`
  — links ohne Strom kein Koerper am Boden, rechts mit Strom liegt Roy vor Leon (655 Pixel verschieden).

## P2 — ROOM1210 Text an der leeren Zelle

### Messung vorher
ROOM1210.RDT hat keinen Nachrichtenblock (RDT+0x3C = 0). Echter Raumstart (`aots 1210`): Slots 0-5 Tueren,
6 AUTO (sub02), 7/8 Items, 49-63 Kamerazonen — KEIN Text-Platz an der Zelle; freie Slots 9..48.

### RE-Belege
* Elzas Zellentuer-Platz ROOM1211 sub00 @0x01DEA `21 04 ec 00` (solange John noch drin ist) -> @0x01DEE
  `2c 08 01 31 00 00 5c aa 00 a8 0e 03 d4 06 0a 00 ff ff 00 00` = Aot_set Slot 8, sce 1 (Text), Flags 0x31
  (Aktion + 620-Vorwaertspunkt), Band 0, Rechteck (-21924,-22528,782,1748), msg 10, Pausemaske 0xffff.
  msg 10 @0x0259D: `04 02 25 50 3a 4f 00 48 4b 3f 47 41 40 57 02 00 25 50 ... 08 45 4a 4f 45 40 41 57 57 57 01 00`
  = "It's locked." [Seite] "It seems that there is someone" / "inside..." -> FORM des neuen Texts: Kopf `04 02`,
  Seitenumbruch `02 00`, Zeilenumbruch `08`, Ende `01 00`, "..." = `57 57 57`.
* Zellentuer-Modell obj 0 (-21750,0,-20760) in BEIDEN Varianten; Leon ROOM1210 main00 @0x01E4E rot y 1216 (offen),
  Elza ROOM1211 main00 @0x01D86 rot y 1225, dort setzt sub00 @0x01E02-06 (`2e 03 00`, `33 00 00 ...`) die Drehung
  auf 0 (zu), solange (4,0xEC)==0. Bei Leon ist die Tuer OFFEN — "The cell door is open..." passt zum Bild.

### Umsetzung
story_r36.c: in 0x1210 Id 20 ablegen (Text + Dauer) und den Satz `k_zelle_satz` = ROOM1211 @0x01DEE mit Slot
8 -> 9 (erster freier Slot) und msg 10 -> 20 (VERTRAG 1.2) GENAU wie op_aot_set (scd_vm.c, sce-1-Zweig) ablegen
(`text_platz`: Mitte/halbe Ausdehnung, msg = pc[14], Maske = u16 pc[16..17], band = pc[4], sce_flags = pc[3]).
Text (PORT-WAHL, Anfang = Zitat des Nutzers): Seite 1 "The cell door is open...", Seite 2 "Whoever was locked up
here" / "is gone." (65 B; Zeilen 151/191/51 px).

### Messung nachher
* `aots 1210`: `[story36] ROOM1210: Text-Platz Slot 9 (Rechteck ROOM1211 @0x01DEE) -> msg 20`,
  `AOT slot= 9 type=5 flags=0x31 band=0x00 evt=20 ... x[-21924..-21142] z[-22528..-20780]`.
* unit_r36_story_zelle (7): Slot 9 in 1210 == Slot 8 in 1211 (am Original gemessen, alle Felder), msg 20 abgelegt,
  Standort (-20600,-21650) begehbar, echter Aktionsdruck Blick -X -> msg 20; in 1211 kein Port-Platz.
* Echte exe: P2a CONTINUE in 1210 + Aktionstaste -> `[msgtext] room=1210 id=20 "The cell door is open...Whoever
  was locked up here is gone."`; P2b Tuer ROOM11E0 -> ROOM1210 (Tuer-Weg) -> `[story36] ROOM1210: Text-Platz Slot 9`
  + Platz gefeuert -> derselbe Text.
* Bild `P_story_bilder/p2_zelle_1210_cut5_text.png` (Cut 5, Bilder 40/185/330): Leon vor der offenen Zellentuer,
  Seite 1 und Seite 2 des Texts.

## P3 — ROOM1091 Text am ausgebrannten Schuppen

### Messung vorher
ROOM1091.RDT hat keinen Nachrichtenblock. Echter Raumstart (`aots 1091`): Slots 0/1 Tueren, 2 AUTO (sub02),
4 Shotgun Shells (r35 Spur F), 39-63 Kamerazonen. Kein Text-Platz am Schuppen. Frei: 3, 5..38.

### RE-Belege
* Leons Feuer-Platz ROOM1090 sub00 @0x02306 `06 00 24 00 21 03 85 00` (solange kein Loescher) -> @0x0230E
  `2c 02 01 b1 01 00 64 00 14 05 3a 07 66 08 de 0d 30 f8 08 07 10 f5 07 00 ff ff 00 00` = Aot_set Slot 2, sce 1,
  Flags 0xB1 (Bit 0x80 = Viereck-Form, 28 B), Band 1, Viereck (100,1300) (1850,2150) (3550,-2000) (1800,-2800),
  msg 7 "I must hurry up and get something to / put out this fire to save that woman!" (@0x028E6). Mit Loescher
  @0x02336 dasselbe Viereck als sce 3 -> sub06 (Loeschen). Nach dem Loeschen ((3,0x85) ROOM1000 sub01 @0x00D0C,
  (3,0x81) ROOM1090 sub06 @0x0271E) legt Leons 1090 KEINEN Platz mehr an.
* ROOM1091 main00 @0x02202 obj 2 `... 6a 09 a3 f5 2e ff 40 00 40 07` = Leons Nach-Rettungs-Lage (1090 sub00
  @0x0237E), kein Typ 0x26 (Feuer) — der Schuppen ist bei Elza bereits geloescht.

### Umsetzung
story_r36.c: in 0x1091 Id 20 ablegen und `k_schuppen_satz` = ROOM1090 @0x0230E mit Slot 2 -> 5 (Slot 4 = r35
Shotgun Shells) und msg 7 -> 20 ablegen (Viereck-Form: xs/zs + has_quad wie op_aot_set). Text (PORT-WAHL,
Stichwort des Nutzers "burnt-out shed"): Seite 1 "It's a burnt-out shed.", Seite 2 "Someone must have put out" /
"the fire." (63 B; 142/180/57 px). KEIN Feuerloescher.

### Messung nachher
* `aots 1091`: `[story36] ROOM1091: Text-Platz Slot 5 (Viereck ROOM1090 @0x0230E) -> msg 20`,
  `AOT slot= 5 type=5 flags=0xB1 band=0x01 evt=20 ... quad=1 -> x[100..3550] z[-2800..2150]`.
* unit_r36_story_schuppen (8): Slot 5 in 1091 == Slot 2 in 1090 (Viereck/Band/Flags/Maske, am Original gemessen),
  msg 20, Slot 4 bleibt, Standort (0,-1800,250) Blick +X (gefunden mit `test_r36_story mess 1091 -1000 1500 -1500
  1500 250 -1800 0`, begehbar) -> msg 20; Leon 1090 nach dem Loeschen: kein Platz (wie Original).
* Echte exe (integration_r36_story_elza, Figur 4): P3a CONTINUE in 1091 + Aktionstaste ->
  `[msgtext] room=1091 id=20 "It's a burnt-out shed.Someone must have put out the fire."`; P3b Tuer ROOM1051 ->
  ROOM1091 (Tuer-Weg) -> `[story36] ROOM1091: Text-Platz Slot 5` + Platz gefeuert -> derselbe Text.
* Bilder (Cut 5): `P_story_bilder/p3_schuppen_1091_cut5_text.png` (Elza, kein Feuer, Text) und zum Vergleich
  `p3_vergleich_leon_1090_cut5_feuer.png` (Leon am SELBEN Standort: Feuer + Original-msg 7) — der Platz ist die
  Stelle des Feuers.

## P4 — Text-Sweep "nie begegnet"

### Messung vorher (Sweep)
Quellen: analysis/story_leon_elza/texte_alle.json (alle Nachrichten beider Varianten, Dekoder = Port-Glyphen).
Gelesen: ALLE Dialogzeilen (Modus 00; 221 Leon, 248 Elza) + Untersuchungstexte (Modus 02) per Schlagwortsuche
(someone|whoever|he|she|cop|officer|girl|woman|friend|biker|motorcycle|partner|rookie|survivor|already|before).
Der Name des jeweils anderen kommt vor: Leon-RDTs "Elza" in 3060 msg 1-8 und 6030 msg 1; Elza-RDTs "Leon" in
30C1 msg 2/3 und 6031 msg 0. Weitere Begegnungs-/Bekanntschafts-Hinweise ohne Namen: 30C1 msg 4/7, 6031 msg 1,
6030 msg 2/12, 50B0 msg 1, 6000 msg 0/3 (Tabelle unten).

ROOM3060 (Leon-RDT) msg 1-8 = Elza/Sherry/John-Dialog: UNBENUTZT. Beleg: Elza hat ein eigenes ROOM3061.RDT
(Raumlader Elza = Leon + 1, PSX.EXE @0x800397e4 `srl a0,a0,31` / @0x800397ec `addu a0,a0,v0`); im SCD von ROOM3060
(main00 @0x0E76..0x0F1C, sub00..sub03 @0x0F24..0x10E8) steht genau EIN Message_on: sub02 @0x01034 `2b 09 80 ff`
= msg 9 ("For some reason the door isn't locked anymore..."), dazu der Text-Platz main00 @0x00EBE (Nutzlast `0a 00`
= msg 10). msg 1-8 werden nie angezeigt -> keine Aenderung (Elzas Szene laeuft aus ROOM3061 mit eigenen Texten).

⛔ Reihenfolge-Befund ROOM6030 (gemessen, echte exe): CONTINUE in 6030 (Eintritts-Cut 0) spielt sub07 -> sub08
(Annette vorne, msg 11..19: "Where's that biker girl?" / "Keeping the monsters at bay...") OHNE die Wagen-Szene
sub02 (msg 0..8). Original: sub00 `13 0a ..` Switch(work_vars[10]) Cut 0 -> Gosub 7 (@0x00D2C), Cut 6/8/12 ->
sub06; die Wagen-Szene startet nur ueber die Marke Slot 5 @0x00CC6 x[-19730..-18130] (sub01 Cmp(work_vars[1]==5));
Leons Spawn aus ROOM6000 Tuer Slot 3 @0x00DDA = (-17970,-20380) liegt OESTLICH der Marke, die Tuer nach vorne
(Slot 1 @0x00C52, Ziel 6030 Cut 0) bei x -9300 — Leon kann vorne sein, bevor Sherry ihm etwas gesagt hat. Dann
weiss er ohne Quelle, was "that biker girl" tut = Bekanntschaft -> msg 12 gehoert in den Sweep.

### RE-Belege (Form)
Jede umgeschriebene Dialogzeile hat im Original die Form `04 00 05 <Farbe> <Sprecher> 16 05 00 00 <Satz> 04 01 01
63` (Farben: Elza/Leon 01, Annette/Ada 02, Sherry 06, Marvin/John 07; 99-Bilder-Halt `01 63`). Das Werkzeug
`tools/r36_p/texte_bauen.py` uebernimmt Kopf (bis inkl. `16 05 00 00`) und Schwanz `04 01 01 63` WOERTLICH aus der
Original-Nachricht DERSELBEN Id und bricht ab, wenn eine Vorlage diese Form nicht hat; nur der Satz ist neu.
Glyphen = Umkehrung von msg_common.c re15_msg_glyph (Punkt = 0x57 wie "RPD." @0x00F07 `2e 2c 20 57`), Umbruch
0x08, jede Zeile <= 271 px (font_width.h = DEBUG.BIN[0x4416+code]; Ausnahme: 30C1 msg 7 Zeile 2 = bytegleiche
Original-Zeile, 272 px im Auslieferungsstand), <= 128 B (PSX MSG_RAW_LEN). Dauer = Original (unit_r36_story_texte).

### Umsetzung (alt -> neu) — Wortlaut PORT-WAHL (Story), zur Freigabe durch den Nutzer

| Raum (Szenario) | Id | Offset | alt | neu | warum |
|---|---:|---|---|---|---|
| 30C1 (Elza) | 3 | @0x00F37 | Elza: Leon?! How is he? / We got separated back at the RPD. | Elza: Leon? I don't know him. / I never ran into him at the RPD. | Sorge + "getrennt" = Begegnung |
| 30C1 (Elza) | 4 | @0x00F7A | Marvin: He's all right and he's helping / me and Ada escape the city. | Marvin: He's a rookie, and he's helping / me and Ada escape the city. | Antwort auf "How is he?" |
| 30C1 (Elza) | 7 | @0x0100D | Elza: Will you tell him that me and John / and Sherry are here and we're all right? | Elza: Will you tell them that me and John / and Sherry are here and we're all right? | persoenliche Nachricht an einen Bekannten -> an "sie" (Leon + Ada) |
| 6031 (Elza) | 0 | @0x01148 | Elza: Where's Leon? | Elza: Sherry! Is anyone else here? | fragt nach Leon |
| 6031 (Elza) | 1 | @0x01165 | Sherry: He's fighting off monsters. / He wants you to start the train. | Sherry: A policeman! He's fighting off / monsters. He wants us to start the train. | "wants YOU" = Leon kennt Elza |
| 6030 (Leon) | 1 | @0x013F9 | Leon: Where's Elza? | Leon: Is anyone else with you? | fragt nach Elza |
| 6030 (Leon) | 2 | @0x01416 | Sherry: She's out there fighting / the monsters! | Sherry: Elza is out there fighting / the monsters! | Sherry stellt Elza als Fremde vor (Szenenfunktion: John/Annette sprechen danach von "her") |
| 6030 (Leon) | 12 | @0x015FC | Leon: Keeping the monsters at bay. / Can you start this thing? | Leon: I've never met her. / Can you start this thing? | Reihenfolge frei (s.o.); neu in beiden Reihenfolgen wahr |
| 50B0 (Leon) | 1 | @0x00BE0 | Ada: Your other friends are down there / with her too. | Ada: Some other survivors are / down there with her too. | "deine anderen Freunde" = Leon kennt Elza/John/Sherry |
| 6000 (Leon) | 0 | @0x01240 | Annette: You must be that cop. / Your friends are already here. | Annette: You must be that cop. / Your friend is already here. | dito; Freund = Marvin (50B0 msg 0 "I left Marvin with her") |
| 6000 (Leon) | 3 | @0x012C1 | Annette: Come on. Let's join your / friends and get out of here. | Annette: Come on. Let's join the others / and get out of here. | dito |

Bewusst UNVERAENDERT (geprueft): 30C1 msg 2 "I'm here with Leon Kennedy." (Marvin nennt seinen Begleiter; Elzas
neue Antwort stellt klar, dass sie ihn nicht kennt), 30C1 msg 5/6/8; 6030 msg 0/3/4/5 (Sherry/John sprechen von
"her" nach Sherrys neuer Vorstellung), msg 11 (Annette kennt "that biker girl" — keine Leon-Elza-Begegnung);
6031 msg 2/5-10; ROOM1030 msg 8 "Seems like whoever rode this crashed it through the front gate. Wonder if they're
ok..." (anonyme Spur von Elzas Motorrad — passt zu "nie begegnet"); 3060 msg 1-8 (unbenutzt, s.o.).
Abweichung von der Vorab-Liste des Orchestrators (30C1 2-6, 6030 1-5, 6031 0-2, 3060 1-8): 30C1 2/5/6, 6030 3-5,
6031 2 tragen keinen Hinweis und bleiben; 30C1 7, 6030 12, 50B0 1, 6000 0/3 kamen durch den Sweep hinzu; 3060 ist
unbenutzt.

Einbau: Tabelle `engine/src/gen/story_r36_texte.inc` (11 Overrides + 2 neue Ids), story_r36.c
re15_story_r36_texte(room) am Ende von re15_msg_load_room_block (Text + Dauer, re15_msg_install_text/_durations).

### Messung nachher
* unit_r36_story_texte (69 Pruefungen): je Zeile Original an der Id == alt (Offset aus RDT+0x3C), nach dem Laden == neu, Kopf
  und Schwanz bytegleich, Dauer gleich (99 Bilder), alle anderen Nachrichten des Raums bytegleich zur RDT, 11
  Overrides.
* unit_r36_story_sweep: alle 206 ladbaren RDTs beider Szenarien mit Port-Lader + Overrides: der andere Name nur
  noch an 8 belegten Stellen (3060 msg 1-4/6 unbenutzt, 30C1 msg 2/3, 6030 msg 2), keines der 10 alten Muster
  ("How is he", "separated", "Where's Elza", "Where's Leon", "He wants you", "Your other friends", "Your friends
  are", "join your friends", "tell him that me", "Keeping the monsters at bay") mehr irgendwo.
* Echte exe, angezeigter Wortlaut (`[msgtext]`): P4a Tuer 6000 -> 6030 (Cut 6) + 3 s vorwaerts in die Marke: msg
  0..8, `id=1 "Leon: Is anyone else with you?"`, `id=2 "Sherry: Elza is out there fighting the monsters!"`;
  P4b CONTINUE 6030 (Annette zuerst): `id=12 "Leon: I've never met her. Can you start this thing?"`; P4c 6000:
  `id=0 ... Your friend is already here.`, `id=3 ... Let's join the others ...`; P4d 50B0: `id=1 "Ada: Some other
  survivors are down there with her too."`; P4e (Elza) Tuer 6001 -> 6031 + Drehung + vorwaerts (Elzas Marke
  @0x00CC6 liegt westlicher: (-25130,-25270,1660,5200)): `id=0 "Elza: Sherry! Is anyone else here?"`, `id=1
  "Sherry: A policeman! He's fighting off monsters. He wants us to start the train."`; P4f (Elza) 30C1 mit
  (3,0x39): `id=3 "Elza: Leon? I don't know him. I never ran into him at the RPD."`, `id=4`, `id=7` neu; die alten
  Wortlaute kommen in keinem Lauf vor.

## P5 — Funkraum / Prison Key
### Pruefung
ROOM10F0 und ROOM10F1 sind bytegleich (A_zensus). msg 0 @0x1174: "A large communications device.It's out of
commission.I can't send any messages from / here." Elza ROOM1021 msg 4 @0x23AC: "John, I couldn't call for help
using the radio." Leon ROOM10D0 msg 22/23, ROOM1150 msg 8/9 ("the system is damaged", "some officers were trying
to repair it"), Port-Szene 10F0 (cut_10f0.c k_msg16 "Anyway... looks like we can't contact anyone with this thing
anymore.") — KEIN Text behauptet eine Reihenfolge Elza-vor-Leon; alle sagen nur: die Anlage ist kaputt. Das traegt
die Lesart des Nutzers ("sie hat es versucht, ging nicht, Anlage zerstoert") und die Untersuchungshaft-Begruendung
(Sabotage). Kein leerer Haken fuer Leon (Nachtrag 14:00). -> keine Textaenderung, kein Code.

## Tests

Neu (probes/r36_story.cmake, registriert nur dort):
* unit_r36_story_roy / _zelle / _schuppen / _texte / _sweep / _reihenfolge (test_r36_story.c)
* integration_r36_story_leon (8 exe-Laeufe P1a/P1b/P2a/P2b/P4a-d) / integration_r36_story_elza (4 Laeufe
  P3a/P3b/P4e/P4f) — test_r36_story.cmake, Karten probe_r36_story_karte

Ergebnisse (woertlich):
* `ctest -R "^unit_r36_story_"` -> `100% tests passed, 0 tests failed out of 6`
* `ctest -R "^integration_r36_story_elza$"` -> `100% tests passed, 0 tests failed out of 1` (125 s erster Lauf rot
  wegen Elzas westlicherer Marke, behoben mit Drehung R0.45 — gemessen 1100 -> 2444; zweiter Lauf gruen)
* `ctest -R "^integration_r36_story_leon$"` -> `100% tests passed, 0 tests failed out of 1`
* Beruehrte Module/Raeume (msg_common.c, scd_room_setup.c, Raeume 1020/1090/1091/1210/6030/...):
  `ctest -R "^unit_(msg|text|savepoint|action_msg|1090|1210|r34n_e|r34n_f|r34n_b_kreuztext|r35_inhalt|r35_cut10f0|r35_cut1150|r33_tuer1120|r34n_d|r34n_a|r35_karte_r1210|r35_raeume|re2_room1090|map_1090|plc_back_yaw_1090|room1050|r36_story|scd|room)" -j 4`
  -> `100% tests passed, 0 tests failed out of 96`
* Schlusslauf: siehe Abschnitt "Schlusslauf" unten.

## OFFEN
* Keine offenen Punkte der Spur P. Hinweise ausserhalb des Auftrags (nur gemeldet, nicht geaendert):
  - ROOM50B1 msg 0 "It's Ada. She's dead." — Elza hat Ada nie gesehen (sie kennt nur den Namen aus Marvins 30C1
    msg 4/6). Kein Leon-Elza-Thema; Entscheidungsfrage an den Nutzer.
  - ROOM6030 msg 6 "Leon: Where's Annette?" ist in der Reihenfolge "vorne zuerst" (s.o.) ebenso schief wie im
    Original (Leon hat Annette dann schon gesehen) — kein Begegnungs-Thema, unveraendert.

## Fuer den Nutzer
* **Freigabe der Wortlaute:** Tabelle "Umsetzung (alt -> neu)" oben (11 Zeilen) + die zwei neuen Spuren:
  - ROOM1210 (Leon, Zellengang vor der offenen Zellentuer): "The cell door is open..." / "Whoever was locked up
    here is gone."
  - ROOM1091 (Elza, Hinterhof an der Stelle von Leons Feuer): "It's a burnt-out shed." / "Someone must have put out
    the fire."
  Jede Zeile einzeln aenderbar: `re15_port/tools/r36_p/texte_bauen.py` (Listen UMSCHREIBEN/NEU) ausfuehren, die
  .inc wird neu erzeugt.
* **Bedienhinweise:** P1 — Roy liegt in ROOM1020 (Leon) erst, nachdem der Generator in ROOM11F0 den Strom
  eingeschaltet hat; vorher ist der Gang leer und nichts anklickbar. P2 — Leon im Zellengang ROOM1210 vor der
  offenen Zelle die Aktionstaste druecken. P3 — Elza im Hinterhof ROOM1091 an der Stelle, wo bei Leon das Feuer
  brennt. P4 — die Szenen in ROOM30C1 (Elza/Marvin), ROOM6030/6031 (Zug), ROOM50B0 (Ada), ROOM6000 (Annette).
* **Neue Assets / Sprachdateien:** keine (keine shared_assets, keine RE2-Dateien). Fuer die neuen Texte Id 20 in
  ROOM1210/1091 und die umgeschriebenen Zeilen gibt es keine synchro-Dateien (synchro/STAGE3/STAGE5/STAGE6 und
  room1210/room1091 existieren nicht) — sie bleiben stumm mit Untertitel. Wer sprechen lassen will:
  `synchro/STAGE1/room1210/main20.wav`, `synchro/STAGE1/room1091/main20.wav`.
* **Entscheidungsfragen:** (1) Freigabe der Wortlaute; (2) ROOM50B1 "It's Ada." (Elza kennt Ada nicht) — so lassen
  oder z.B. "A woman... Could this be Marvin's 'Ada'?"; (3) Annette/Ada nennen die Ueberlebenden am Zug jetzt
  "the others"/"some other survivors" — falls dir das zu vorsichtig ist, kann 6000 msg 0/3 auch Original bleiben
  (dort ist der Hinweis am schwaechsten).
