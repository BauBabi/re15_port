# Runde 36 — Spur Q "schluessel" (Dossier, fortlaufend)

Baum: `.claude/worktrees/r36_schluessel`, Zweig `r36/schluessel`, Basis master 78a4a748.
Modul: `re15_port/engine/src/schluessel_r36.c` + `re15_port/include/re15_schluessel_r36.h`
(alle Konstanten mit Beleg im Kopf), RE2-Bytes `re15_port/engine/src/gen/schluessel_r36.inc`
(Werkzeug `re15_port/tools/r36_schluessel/schluessel_export.py`), Bilder `Q_belege/`.

## Auftrag (woertlich, AUFTRAG.md Z. 45 + Z. 49)

> - Ich möchte bei Leon das die Tür zum Parking Lot ROOM 11C0 verschlossen bleibt, bis ein Parking Lot
>   Schlüssel im Communication ROOM 10F0 gefunden wurde. Wähle dir einen Schlüssel aus den Resident Evil 2
>   Items aus, der am ehesten den optischen Eindruck eines Parking Lot Schlüssels macht.
> - Ich möchte bei Elza das der prison key nicht in Room 1011 ist, wie aktuell, sondern er soll da sein, wo
>   dann auch der Parking Lot Schlüssel in Leons Szenario ist. Auch hier - suche die ein gutes Model aus
>   Resident Evil 2 aus für einen prison key.

Ressourcen (VERTRAG.md): Bank-9-Bits 82 (Parking Lot Key genommen, Leon 10F0), 83 (Prison Key genommen,
Elza 10F1), dazu **93 aus der Reserve** (Begruendung 3.4); Nachrichten-IDs 28/29 in ROOM11B0; Ereignis 25.

Story-Hintergrund (Nachtrag 14:00, nur Dossier): Kendo/John sitzt wegen des sabotierten Funkraums in
Untersuchungshaft; die Beamten entzogen Roy (Kendos Freund) die Befugnis, ihn zu befreien — deshalb liegt
der Gefaengnisschluessel nicht bei Roy, sondern im Communications Room. KEIN leerer Haken/Text fuer Leon
(nicht gebaut). Passt zum Original-Text des Funkraums "It's out of commission" (Anlage zerstoert).

## 0. Status

- [x] Bestandsaufnahme (Item-Tabelle, Prison Key in 1011, Tuer 11B0->11C0, RE2-Schluessel-Modelle)
- [x] RE-Belege (RE1.5-Tuer-Handler ohne Schluessel, RE2-Tuer-Handler, Item-Aufnahme, ROOM1011/1211)
- [x] Umsetzung (Modul + Haken + RE2-Bytes + TUERSE.VBS-Aufschliess-Satz)
- [x] Riegel unit_r36_schluessel_{raum,tuer,zelle,namen}
- [x] Echte exe: integration_r36_schluessel (Abschnitt 4/5)
- [x] Wegwerf-Kette Parking Lot Key (RE2 @0x80051670)

## 1. Messung vorher (Stand 78a4a748)

**Leon, Tuer 11B0 -> 11C0.** ROOM11B0 main00 @Datei 0x00FAA
`3b 01 02 31 00 00 96 97 0c 8b c4 09 ac 0d 41 9d 00 00 74 43 fb 11 00 1c 00 00 00 00 00 00 00 00` =
Door_aot_set Slot 1, sce 2, sat 0x31, Band 0, Rechteck (-26730,-29940, 2500 x 3500) -> Raum 0x1C
(ROOM11C0), Spawn (-25279,0,17268), Richtung 0x11FB, Cut 0, **key_id = key_type = 0**. Einzige Bedingung
im Original: sub01 @0x011F2 If / @0x011F6 `21 04 f3 00` Ck(4,0xF3)==0 -> @0x01216
`46 01 01 31 0f 00 ff ff 00 00` Aot_reset(1 -> Text msg 15 "It's too dark to see anything...").
Mit Strom (4,0xF3)=1 ist die Tuer im Port immer offen (Riegel `tuer` Fall O: (9,93) gesetzt = Original-
Verhalten). Kein Parking-Lot-Schluessel im Spiel (Namenstabelle, 102 Eintraege, item_namen.json).

**Elza, Prison Key.** Kein Inventar-Gegenstand, nur das Flag (4,0xEC): ROOM1011 sub06 = msg 19 "He is
holding something..." + msg 20 "You've taken the Prison key." + @0x00E2C `22 04 ec 01` Set(4,0xEC) +
@0x00E30 Set(3,0x78) + Aot_reset Slot 5 -> sub07; sub07 = msg 18 (Zettel "0513"). Bewaffnet wird Slot 5
von sub00 @0x00A52 (sce 3 -> sub06, Ck(4,0xFA)==1 & var0A==0), @0x00A78 (Ck(3,0x78)==1 -> sub07) und am
Dialogende sub02 @0x00DC0 (-> sub06). Leser: ROOM1211 sub00 @0x01DEA Ck(4,0xEC)==0 -> Slot 8 Text msg 10
"It's locked. It seems that there is someone inside..."; sonst @0x01E36 John (Typ 0x49) + Slot 6 -> sub02
(Befreiung). Riegel `zelle` Fall R zeigt den Port-Zustand vorher: Slot 5 zeigt auf sub06.

**Funkraum 10F0/10F1.** AOT- und Prop-Satz beider Varianten bytegleich (Sonde `probe_r34n_e_dokumente
aots 10F0/10F1`): Slots 0..14 belegt, Kamerazonen ab 48, erster freier Slot 15; nOmodel 4 (obj 0..3).

## 2. RE-Belege

### 2.1 RE1.5 hat keinen Schluessel-Mechanismus an Tueren
Tuer-Handler PTR_8007469c[2] = 0x800430BC (ghidra1_V2.txt Z. 154099-154111), vollstaendig:
```
800430bc 01 00 02 34  ori  v0,zero,0x1
800430c4 a8 c9 24 ac  sw   a0,DAT_800ac9a8        ; Nutzlast
800430d4 59 53 22 a0  sb   v0,DAT_800b5359        ; Uebergang = 1
800430d8 00 00 82 8c  lw   v0,DAT_800aca40
800430dc 00 ff 03 3c  lui  v1,0xff00
800430e0 25 10 43 00  or   v0,v0,v1
800430e4 00 00 82 ac  sw   v0,DAT_800aca40        ; Freeze
800430e8 08 00 e0 03  jr   ra
```
Kein Test der Felder. Zensus (Brute-Force ueber alle STAGE*-RDTs, `3b slot 00|02 31`): die RE2-
Schluesselfelder pc[29] key_id / pc[30] key_type sind in **allen 649** Rechteck-Tueren 0 (31 weitere
Treffer sind Datenbytes in Nachrichten/Kamera-Bloecken mit Werten (242,47)/(75,66)). RE1.5-Schluessel
laufen als Raumskript (Kartenleser ROOM10D0/1230/11E0 sub20: Frage, Ck(9,taken), "You've used the X.").

### 2.2 RE2 Retail (Ziel, wo RE1.5 unfertig ist): Tuer-Handler LAB_80051514
ghidra_re2_Leon.txt Z. 169432-169573 (info/re2leon/PSX.EXE), selbst gelesen:
```
800515a8 0f 00 45 92  lbu  a1,0xf(s2)        ; key_id
800515b0 80 00 a2 30  andi v0,a1,0x80        ; verschlossen?
800515c0 d8 dc 01 0c  jal  FUN_80077360      ; a1 = key_id & 0x3f: Schlossbit gesetzt -> offen
800515d0 10 00 50 92  lbu  s0,0x10(s2)       ; key_type
80051628 b3 a5 01 0c  jal  FUN_800696cc      ; key_type im INVENTAR suchen
80051634 1a 00 20 06  bltz s1,LAB_800516a0   ; fehlt
  80051640 05 00 06 24  li   a2,0x5          ; Text 5 "You have used the <Name>." (@0x8009F0E5)
  8005164c 8e bf 00 0c  jal  FUN_8002fe38
  80051654 25 02 04 3c  lui  a0,0x225 / 80051658 jal 0x8005ba28   ; Se_on(2,0x25) Aufschliessen
  80051670 b0 82 22 ac  sw   LAB_80051718    ; Wegwerf-Fortsetzung
  80051690 c3 dc 01 0c  jal  FUN_8007730c    ; Schlossbit setzen
  80051698 be 45 01 08  j    LAB_800516f8 / _clear v0   ; v0 = 0: KEIN Durchgang bei diesem Druck
800516a0 16 02 04 3c  lui  a0,0x216 / 800516a4 jal 0x8005ba28    ; Se_on(2,0x16) Verschlossen
800516b4 b4 ff 06 26  addiu a2,s0,-0x4c / 800516b8 jal FUN_8002fe38 ; Text key_type - 0x4C
```
RE2-Systemtexte Gruppe 0x100 (Offsettabelle @0x8009F368, Basis @0x8009EFCC, Aufloeser FUN_8002FE38
@0x8002ff44/4c): 5 @0x8009F0E5 "You have used the <Name>.", 9 @0x8009F160 "This key is useless now.
Discard?", **0x13 @0x8009F2BB "It's locked. / The door is marked "Power Room"."** = Text fuer key_type 0x5F
(P. Room Key - 0x4C), 0x0C @0x8009F1B3 "It's locked." (key_type 0x58 Cabin Key).

### 2.3 Item-Ids der zwei neuen Schluessel
Namenstabelle DEBUG.BIN @0x800c495c (102 Eintraege, Reader FUN_80028840). Zensus Item_aot_set
(`50 slot 09`, Id u16 @+14) und Aot_reset sce 9 ueber alle 206 RDTs: **0x45 "Water Key" und 0x42
"Key Disc" werden nie platziert**. Eigenschaftssatz @0x80074DA8 + Id*12 fuer 0x36..0x47 (alle gleich):
`01 00 00 00 88 4c 07 80 00 00 00 00` = Obergrenze 1, Null-Kombinationssatz @0x80074C88. Klasse
"Schluessel" ab Id 0x22 (`sltiu id,0x22` @0x80049124). Icon-Tile und ITPS-Block liegen in den 72er-
Dateien (Id < 0x48; ab 0x48 macht der Port eine Zone zum Dokument, aot_item_dokument). Gewaehlt:
**0x45 -> Parking Lot Key (Leon), 0x42 -> Prison Key (Elza)**; Name, CHECK-Text, Icon, Bild ersetzt der Port.
Beide ausgelieferten Namen tragen "Key" -> item_prompt_common.c re15_prompt_name_ist_schluessel bleibt 1.

### 2.4 Modellwahl (PORT-WAHL laut Auftrag) — RE2-Weltmodelle, Icons, Item-Bilder
Alle 14 Schluessel-/Karten-Meshes aus extracted_re2_items (katalog.csv) gerendert
(`Q_belege/q_re2_schluessel_modelle.png`), dazu die RE2-Icons (ITEMALL.PIX, Tile == Id; Gegenprobe RE2-Tile
77 "Fuse Case" == RE1.5-Tile 0x40, re15_sicherung.h) und Item-Bilder (ITPS.ITP Block == Id):
- **Parking Lot Key = RE2 0x5F "P. Room Key"** (mesh071): flacher Sicherheitsschluessel mit schwarzem
  Kunststoffkopf und rotem Emblem = Autoschluessel-Optik. Modell ROOM6150.RDT Prop 8, MD1 @0x004FBC 236 B
  md5 3c9176ee9bbb7e91b7c9433d6fe5c371, TIM @0x030B74 34848 B md5 0d43d30f; Icon Tile 95; Bild Block 95.
  Verworfen: C. Panel Key (Ring + Anhaenger "D"/"U", eher Schaltschrank), W. Box Key (Papieranhaenger),
  Precinct Keys (Zierbart), Platform Key (Ring + Bartschluessel).
- **Prison Key = RE2 0x58 "Cabin Key"** (mesh062): grosser Eisenring mit Bartschluessel = Schliesser-
  Schluesselbund. Modell ROOM1050.RDT Prop 0, MD1 @0x001558 140 B md5 569def980f500800614301d40744b741,
  TIM @0x00C650 8736 B md5 be1f46e9; Icon Tile 88; Bild Block 88.
  Verworfen: Small Key (Bart ohne Ring), Platform Key (Icon/Bild zeigen einen Sicherheitsschluessel, nicht
  das Ring-Modell — Modell und Bild passen nicht zusammen).
- Beide RE2-Modelle sind FLACH (alle Ecken y = 0): 4 Dreiecke 152 x 76 bzw. 1 Viereck 180 x 180 —
  RE2 legt Schluessel als liegende Karten in die Welt.
- **Icon ohne Aenderung einsetzbar:** RE1.5-Icon-CLUT (ST_00.TIM CLUT-Zeile 0) und RE2-Icon-CLUT
  (ITEMALL_089.TIM) unterscheiden sich NUR in Eintrag 0 (0x0000 / 0x8000); Index 0 kommt in Tile 88/95
  nicht vor (gemessen).
- **Item-Bild:** RE2-Bloecke tragen crect (0,480) / prect (0,0); RE1.5s CHECK-Foto-Lader @0x800c0258 laedt an
  die EINGEBETTETEN Rechtecke, das Fotofenster liegt bei (832,256) 56x72 mit CLUT-Zeile (0,489) (RE1.5-
  Bloecke 0x01/0x38/0x42/0x45 gemessen). Werkzeug setzt Block+0x0C/0x0E und +0x218/0x21A um, sonst
  bytegleich (Riegel `namen` Fall B).

### 2.5 Texte (PORT-WAHL; FORM belegt, jeder Glyphenlauf ausgeliefert)
| Text | Wortlaut | Form-Vorbild |
|---|---|---|
| Name 0x45 | Parking Lot Key | Namensblob-Form `.. 07` (DEBUG.BIN @0x800c4e04 "Chris' Diary") |
| Name 0x42 | Prison Key | Original-Name aus ROOM1011 msg 20 @0x12BF "You've taken the Prison key." |
| CHECK 0x45 | It looks unique. / Now I can access / the parking lot. | RE2-Beschreibung P. Room Key @0x800A1DA4 "It looks unique.<FC>Now I can access<FC>the power room." |
| CHECK 0x42 | I can access the / prison cells / with this. | RE2-Beschreibung Cabin Key @0x800A1C6E "I can access the<FC>cabin in the rear<FC>with this." |
| 11B0 msg 28 | It's locked. [Seite] The door is marked / "Parking Lot." | RE2 0x13 @0x8009F2BB (key_type 0x5F = das Modell); Seite `02 00` wie ROOM10F0 msg 5 @0x12B6; Anfuehrung 0x61..0x19, Punkt innen wie DEBUG.BIN-Text 0x45 |
| 11B0 msg 29 | You've used the Parking Lot Key. | ROOM11E0 msg 12 @0x238C "You've used the Pliers." (Name Farbe 1, `05 01 .. 05 00`) = RE2 Text 5 |

Glyphenlaeufe: "It's locked." ROOM10F0 @0x12CE `25 50 3a 4f 00 48 4b 3f 47 41 40 57`; "The door is "
ROOM11B1 msg 12 @0x179F; "You've used the " ROOM11E0 @0x238E.

### 2.6 Toene (RE2, Mini-Bank TUERSE.VBS)
- Verschlossen = RE2 Se_on(2,0x16) @0x800516a4 -> Satz der Art M = ZU_A (RE2 ROOM1140 Satz 0x16),
  re15_lock_se_satz_fuer_art(RE15_LOCK_SE_ART_M) — dieselbe Wahl wie jede mechanische Port-Tuer (lock_se).
- Aufgeschlossen = RE2 Se_on(2,0x25) @0x80051658 -> **AUF_KEY = RE2 ROOM1140 Satz 0x25** (EDT @0x01484
  `00 00 84 16`, Tone @0x01DD0, VAG @0x04480 5264 B sha1 e12a441e5282). In Runde 30 vorbereitet
  (`tools/re2_door_se_cut.py --mit-aufschliessen`, r30 Dossier tuer-verschlossen.md §6 Punkt 7), hier
  eingebaut: TUERSE.VBS 29976 -> 42808 B, Saetze 0..3 bytegleich, Satz 4 AUF_KEY, Satz 5 AUF_HIER
  (ROOM20A0 0x26, @0x800515f8). probe_r30_tuerse_bank: Satz 4 pitch 0x4f3 (13641 Hz), 9184 Samples 0,673 s.

## 3. Umsetzung

### 3.1 Gegenstaende, Aufnahme, Zelle
- `re15_schluessel_r36_install` (scd_room_setup.c am Blockende "Runde 36 Spur Q" + main.c Boot/CONTINUE):
  10F0 ohne (9,82) -> Prop obj 4 (RE2 P. Room Key) + Item-Zone Slot 15 (0x45, Menge 1, Bit 82, Prop 4,
  sat 0x31); 10F1 ohne (9,83) -> dasselbe mit 0x42 / Bit 83 (Cabin Key). Muster inhalt_r35.c
  (Obj_model_set-Felder LAB_80040914: Flags 0x000B @0x80040998, Band 1; Item_aot_set @0x80040644).
- Aufnahme ueber das vorhandene Item-Modal (FUN_8001db28-Nachbau): Name im Prompt "Will you take the
  <Name>?" ueber re15_item_prompt_walk (Haken), Bild aus ITPS (eingesetzt), Ja -> Inventar + Bit.
- **Prison Key -> (4,0xEC)** (Haken item_modal_common.c direkt nach dem Zone-9-Bit): genau das, was
  ROOM1011 sub06 @0x00E2C mit dem Flag-Schluessel tat. ROOM1211 bleibt Original: mit (4,0xEC) John + Slot 6
  -> sub02 (Befreiung; Spur L haengt dort den Aufschliess-Ton ein), ohne Text msg 10.
- **ROOM1011: Ereignis 6 -> 7** (Haken scd_event_fire, VOR der Programm-Kette): Roy zeigt, was das
  Original nach der Uebergabe zeigt ((3,0x78)==1 -> sub07 @0x00A78) — msg 18 "He has passed out... He is
  holding a slip. The numbers "0513"..." —, ohne msg 19/20 und ohne (4,0xEC).

### 3.2 Lage (PORT-WAHL aus Messung) — EINE Stelle fuer beide
**(8050, -1460, 2700), rot_y 256; Zone x 7850..8850, z 2050..3050** = Suedende der Funkkonsole an der
Ostwand (SCA 10 x 7400..9300, z 2500..14750), links im Eintrittsbild Cut 0 (Tuer von ROOM10D0).
- Plattenhoehe trianguliert am gelben Ordner derselben Konsole: Cut 1 (90,8;156,7) + Cut 2 (235,0;150,0),
  Strahlen mit der Engine-Matrix -> (7808,-1482,6246), Restabstand 18 / 18 -> Platte -1460.
- Sichtbarkeit (Sonde `sicht`, Port-Masken): Cut 0 (97;170) und Cut 1 (123;113) ohne Maske. Framedump
  mit/ohne Bit 82 (RE15_FORCE_CUT, RE15_WINDOW_SCALE=1): Cut 0 **9 Bildpunkte** (dunkler Kopf mit rotem
  Punkt auf der grauen Konsole, `Q_belege/q_lage_cut0_framedump.png`), Cut 1 2 Bildpunkte. Der Schluessel ist
  klein — RE2s Modell misst 152 x 76, und keine Kamera von 10F0 kommt einer Moebelplatte naeher als
  vz ~2600 (Suche ueber alle Platten: groesste Laenge 12 px, nur am Bildrand).
- **Verworfen: der Holzschreibtisch mit Lampe** (Suedende SCA 9 x -2650..-950, Tischhoehe -1300 = Original-
  Remington obj 3 (-1450,-1300,3850)). In Cut 5 und Cut 6 — den einzigen Kameras auf diesen Tisch — deckt
  eine (nachgezeichnete) Maske JEDE der 28 gemessenen Plattenstellen (Raster x -2550..-1050 / z 3250..4250,
  Sonde `sicht`: in jedem der zwei Cuts, in dem die Stelle im Bild liegt, mindestens eine verdeckende Maske):
  Framedump Cut 6 zeigt weder Schluessel noch die Original-Remington
  (`Q_belege/q_verworfen_tisch_cut6.png`). -> Notiz fuer Spur O/den Nutzer (OFFEN).
- Zone: der Aktions-Scan nimmt je Druck den KLEINSTEN treffenden Slot (`j 0x80043028` @0x80042f94); Slot 2
  (Text "A large communications device..." x 7000..7800, z 2600..12300) wuerde jede Zone mit x <= 7800
  schlucken -> Zone ab x 7850. Kein Original-Satz 0..14 schneidet sie (Riegel `raum`). Stand zum Aufheben:
  suedlich der Konsole, z.B. (8350,2050) Blick +z, 620 voraus (@0x80042bd0) = (8350,2670).

### 3.3 Parkplatz-Tuer (nur Leon, nur mit Strom)
`tuer_sperren`: ROOM11B0, (9,93) = 0, (4,0xF3) = 1, Slot 1 ist eine Tuer -> Texte 28/29 installieren,
Nutzlast next_pos merken, Slot 1 per re15_aot_retype zum sce-3-Ereignis 25 (sat 0x31, Band = Tuer-Band;
Ziel/Cut/Richtung bleiben in door_params). Druck -> scd_event_fire(25) -> `re15_schluessel_r36_ereignis`
(Haken in der Programm-Kette) fuehrt RE2s Tuer-Handler aus:
- Parking Lot Key im **Inventar** (re15_inv_find_item = FUN_800696cc-Gegenstueck): msg 29 + Ton AUF_KEY +
  (9,93) = 1 + Slot 1 zurueck zur Original-Tuer (Aot_reset-Semantik, next_pos) — kein Durchgang bei diesem
  Druck (@0x80051698 v0 = 0); der naechste Druck geht nach ROOM11C0.
- sonst: Ton ZU_A + msg 28.
Rueckgabe ein leeres Programm (`01 00` Evt_end). ROOM11B1 (Elza) bleibt unberuehrt.
- **Wegwerf-Kette** (RE2 @0x80051670 nach Text 5 @0x8005164c): der vorhandene Port-Mechanismus
  (item_discard_common.c, Nutzerauftrag 2026-09-21 "bei Key Items, die verwendet wurden ... drop it?") kennt
  nur die erzeugte Tabelle gen/discard_sites.inc. Haken in re15_discard_besitz_vor_nachricht: hinter der
  Tabelle fragt er `re15_schluessel_r36_discard_stelle` -> (ROOM11B0, msg 29) = Parking Lot Key. Der
  Tuer-Handler ruft die Kette in RE2s Reihenfolge: Besitz VOR dem Oeffnen (@0x80051628/34), Fortsetzung
  NACH dem Oeffnen (@0x80051670). Nach dem Schliessen von msg 29 faellt die Anzahl auf 0 (@0x80051810) und
  RE1.5s eigene Zeile DEBUG.BIN @0x800C508B "You don't need this key any more. Discard it?" geht auf
  (@0x80051834; `Q_belege/q_wegwerf_abfrage.png`). Ja -> Schluessel weg, Tuer bleibt offen (Bit 93).

### 3.4 Reserve-Bit 93 (Begruendung)
RE2 haelt "aufgeschlossen" in einem eigenen Schlossbit (FUN_8007730c @0x80051690, Feld key_id), getrennt
von Aufnahme und Besitz; nur so bleibt die Tuer offen, wenn der benutzte Schluessel spaeter fehlt
(Wegwerfen, Item-Box). Bit 82 (genommen) kann das nicht tragen. 93 ist der erste Reserve-Platz (VERTRAG
1.1 "93-95 nur mit Begruendung"); in keinem anderen r36-Dossier belegt (Zensus der Arbeitsbaeume 23:10).

### 3.5 Dateien
Neu: engine/src/schluessel_r36.c, include/re15_schluessel_r36.h, engine/src/gen/schluessel_r36.inc,
tools/r36_schluessel/schluessel_export.py, tests/unit/test_r36_schluessel.c,
tests/unit/probe_r36_schluessel_karte.c, tests/unit/probes/r36_schluessel.cmake.
Haken (1-5 Zeilen): scd_room_setup.c, scd_vm.c (scd_event_fire 2x), platform/pc/main.c (Installer,
Prop-Lader, ITEMALL/ITPS-Ladestelle), platform/pc/src/inv_render_pc.c (2x), item_icon_common.c,
itps_common.c, item_prompt_common.c, re15_inv_screen.c (3x), item_modal_common.c, item_discard_common.c.
Asset: shared_assets/RE2/TUERSE.VBS + gen/re2_door_bank.inc (Werkzeug --mit-aufschliessen), Riegel
test_r30_tuer.c Teil bank angepasst (Groesse/Satzzahl + Soll-Zeilen AUF_KEY/AUF_HIER).

## 4. Messung nachher (echte exe, Lade-Weg: Spielstand vom Kartenwerkzeug probe_r36_schluessel_karte + CONTINUE)

| Lauf | Eingabe | Ergebnis (debug.log / Framedump RE15_WINDOW_SCALE=1) |
|---|---|---|
| Leon 11B0 Strom an, ohne Schluessel, vor der Tuer (-25480,-26000) Blick -z | Quadrat Bild 45 (Seite 2: 115) | "Parkplatz-Tuer verschlossen", kein DOOR FIRE, Bild 150 noch 11B0; Text "It's locked." + Seitenpfeil (`q_tuer_11b0_texte.png` oben), Seite 2 "The door is marked / "Parking Lot."" mit Anfuehrung 0x61/0x19 (`q_tuer_11b0_seite2.png`) |
| dasselbe mit Parking Lot Key | Quadrat 45 / 140 / 245, Halten 270-282 | "Parking Lot Key benutzt -> Tuer offen, Flag (9,93) = 1", Text "You've used the Parking Lot Key." (Name gruen), Abfrage "You don't need this key any more. Discard it? Yes No", dann `DOOR FIRE slot=1 ... spawn=(-25279,0,17268)`, Bild 5 in ROOM11C0 |
| Leon 10F0 vor der Konsole (8350,2050) Blick +z | Quadrat 45, Bestaetigen | Aufnahme-Modal mit RE2-Bild P. Room Key, "Will you take the Parking Lot Key?", "PARKING LOT KEY genommen: Item 0x45, Bit (9,82)" (`q_aufnahme_modal_10f0_10f1.png`) |
| Elza 10F1 gleiche Stelle | dito | Modal mit RE2-Bild Cabin Key (Ring), "Will you take the Prison Key?", "PRISON KEY genommen ... (9,83)", "Prison Key genommen -> Flag (4,0xEC) = 1" |
| Statusschirm, beide Schluessel | START, ITEM | Icons RE2-Tile 95/88 im Raster, Name "Parking Lot Key" / "Prison Key" in Spielschrift (`q_statusschirm_namen_icons.png`) |
| CHECK (Harness RE15_INV_CHECK_SHOT) | — | "CHECK-Foto Item 0x45 / 0x42: crect (0,489) prect (832,256) 56x72 -> Foto und CLUT geladen", Foto + Text "It looks unique. / Now I can access / the parking lot." bzw. "I can access the / prison cells / with this." (`q_check_foto_text.png`) |
| 10F0 Cut 0/1 mit/ohne Bit 82 | — | Differenz Cut 0 9 Bildpunkte bei (96..102 ; 169..170), Cut 1 2 Bildpunkte (`q_lage_cut0_framedump.png`) |

Riegel (gleicher Code, Engine-Wege ohne Fenster): Roy-Ereignis 6 laeuft als sub07 (msg 18), keine msg 19/20,
(4,0xEC) bleibt 0; ROOM1211 mit (4,0xEC) John (Typ 0x49) + Slot 6 -> sub02, ohne Text msg 10.

## 5. Tests
Neu (tests/unit/probes/r36_schluessel.cmake):
- `unit_r36_schluessel_raum` — P/Z/G/D/E: Prop + Zone in 10F0 und 10F1, kein Original-Satz 0..14 schneidet die
  Zone, Bit -> weg, echter Aktionsdruck (re15_aot_scan, 620 voraus) -> Modal mit 0x45/0x42 -> Ja -> Bit 82/83 +
  Inventar + Modell weg; Zellen-Flag (4,0xEC) nur beim Prison Key; RE2-Modelle parsen.
- `unit_r36_schluessel_tuer` — S/Z/B/W/O/D/E: Sperre nur mit Strom und ohne Bit 93; ohne Schluessel msg 28 + Satz
  ZU_A, kein Raumwechsel, auch mit Bit 82 ohne Inventar; mit Schluessel msg 29 + AUF_KEY + Bit 93, Wegwerf-
  Abfrage fuer 0x45, Ja -> weg, Slot 1 wieder Original-Tuer, naechster Druck -> ROOM11C0; ohne Strom Text 15;
  ROOM11B1 unberuehrt.
- `unit_r36_schluessel_zelle` — ROOM1011 Roy (Ereignis 6 -> sub07, msg 18), ROOM1211 Zelle mit/ohne (4,0xEC).
- `unit_r36_schluessel_namen` — Prompt-Namen [0]/[2], CHECK-Text, Icon-Tiles = RE2-Tiles 95/88 (sonst kein Byte
  geaendert), ITPS-Rechtecke (0,489)/(832,256) + CLUT/Bild = RE2-Block, Modal-Leser zeichnet.
- `integration_r36_schluessel` — echte exe (Tabelle Abschnitt 4: zu, auf, leon, elza, check45, check42), 131 s.
Angepasst: `unit_r30_tuerse_bank` (TUERSE.VBS 42808 B, 6 Saetze, Soll-Zeilen AUF_KEY/AUF_HIER).

### 5a. Summenzeilen (gezielte Laeufe, keine volle Suite)
Lauf 1 (alle Gruppen der beruehrten Module, `-j 3`), -R-Ausdruck woertlich:
`r36_schluessel|r21_discard|r30_tuer|r31_tuer_ton|door_1170_lock|integration_door_lock|unit_inv_fsm|unit_item_|unit_inv_grant|itembox|pri_inventar_riegel|r26_inventar|r35_inventar1050|item_name_census|android_n1_inventar|sicherung|r35_cut10f0|r22_10f0|r35_inhalt|r34n_e_dokumente|r30_irons_tisch|unit_msg_dialog|action_msg_gate|r33_tuer1120|rolltor|adaruf|r35_cut1150|fenster1120|r35_fenster|elza_selbsttuer|elza_zweig`
-> `98% tests passed, 2 tests failed out of 102`. Beide Fehler sind GEWOLLTE Verhaltensaenderungen dieser Spur,
keine Regressionen, und beide Riegel wurden angepasst:
- `unit_r26_inventar` Teil B4 ("kein Kollateral in den uebrigen Skripten" fuer alle 102 Namen): die Prompts von
  0x42/0x45 tragen jetzt die Port-Namen. B4 nimmt die zwei Ids aus (wie die Dokument-Ids 0x48..), neuer Teil
  B4c verlangt in allen 9 Skripten genau den Port-Namen.
- `integration_r35_cut10f0` Teil E (Leon geht von ROOM11B0 durch die Parkplatz-Tuer nach ROOM11C0, ohne
  Schluessel): die Tuer ist jetzt bis zum Parking Lot Key zu (Auftrag). Das Kartenwerkzeug
  probe_r35_cut10f0_karte setzt bei `vor11c0` zusaetzlich (9,93)=1 (= Tuer schon aufgeschlossen).
Lauf 2 (nach den Anpassungen, die zwei Riegel + eigene + Bank/Tuer + Wegwerfen), -R woertlich:
`^unit_r26_inventar$|^integration_r35_cut10f0$|r36_schluessel|r30_tuer|r21_discard`
-> **`100% tests passed, 0 tests failed out of 14`** (integration_r35_cut10f0 248 s, integration_r36_schluessel
117 s). Die Engine-Dateien sind seit Lauf 1 unveraendert (nur Riegel-Code und Dossier), die 100 uebrigen Tests von
Lauf 1 gelten also fuer denselben Motor-Stand.

## 6. OFFEN
- **Holzschreibtisch 10F0/10F1 Cut 5/6 vollstaendig maskiert** (Sonde `sicht`, 28/28 Stellen): auch die
  ORIGINAL-Remington (obj 3, Slot 13) ist dort unsichtbar. Masken = nachgezeichnete MSK-Seitendaten; Spur O
  (PRI 10F0) bzw. der Nutzer koennen pruefen, ob die Tischmasken die Platte freigeben sollten. Der Schluessel
  liegt deshalb auf der Konsole.
- Der Schluessel ist klein (Cut 0 9 Bildpunkte): RE2-Originalmodell 152 x 76 und keine 10F0-Kamera naeher als
  vz ~2600 an einer Platte. Kein Glanz-/Markierungseffekt gebaut (RE2 hat keinen).
- Prison Key: KEINE "You've used..."-Zeile und KEINE Wegwerf-Abfrage an der Zelle — die Befreiung ist die
  Original-Szene ROOM1211 sub02 (Spur L haengt dort den Aufschliess-Ton ein); eine Abfrage mitten in die Szene
  zu legen waere eine eigene Choreografie-Entscheidung (Vorschlag: Benutzungsstelle (1211, msg 9) = Ende der
  Szene). Der Schluessel bleibt im Inventar.
- Elza braucht fuer den Prison Key jetzt den Funkraum-Nebenweg (Minidisc 1201 -> Stimmschloss 1101 -> Blue
  Keycard 1111 -> Kartenleser 10D1, story_leon_elza/ablauf_elza.md Nebenwege) — Folge der Nutzer-Vorgabe.
- TUERSE.VBS ist eine geteilte Datei: falls Spur G (Karten-"Piep"/Aufschliess-Toene) dieselbe Bank erweitert,
  muss sie beim Zusammenfuehren neu erzeugt werden (Werkzeug-Aufruf im Kopf von gen/re2_door_bank.inc).

## 7. Fuer den Nutzer
- **Parking Lot Key** (Leon): liegt im Funkraum (ROOM10F0) auf der Konsole gleich links neben der Tuer, durch die
  man von 10D0 hereinkommt — vor dem Suedende der Konsole stehen und untersuchen. Modell/Icon/Bild = RE2
  "P. Room Key" (schwarzer Kunststoffkopf, rotes Emblem: der Autoschluessel-Look).
- **Prison Key** (Elza): dieselbe Stelle in ROOM10F1. Modell/Icon/Bild = RE2 "Cabin Key" (grosser Eisenring mit
  Bartschluessel). Roy (ROOM1011) haelt nur noch den Zettel "0513"; mit dem Prison Key oeffnet die Zelle in
  ROOM1211 wie bisher.
- **Parkplatz-Tuer ROOM11B0 -> 11C0** (nur Leon, sobald Strom da ist): ohne Schluessel "It's locked." / "The door
  is marked "Parking Lot."" mit RE2-Verschlossen-Ton; mit Schluessel "You've used the Parking Lot Key." mit
  RE2-Aufschliess-Ton, danach "You don't need this key any more. Discard it?"; der naechste Druck oeffnet die Tuer.
- Neue/geaenderte Assets fuer das Paket-Gate: `re15_port/shared_assets/RE2/TUERSE.VBS` (geaendert, 42808 B,
  md5 9ce7576443eecfb46355339994b89b35). Alles andere ist eingebacken (gen/schluessel_r36.inc). Keine Sprachdateien.
- Texte (PORT-WAHL, Form aus RE2/RE1.5 belegt, Abschnitt 2.5) — bei Wunsch einfach anderen Wortlaut nennen.

## Nachbesserung 1 (2026-10-11, nach Abnahme 0 `Q_abnahme_0.md`)

Mangel aus der Abnahme: **M1** — der Prison Key bekommt nach der Befreiungsszene ROOM1211 sub02 keine
Wegwerf-Abfrage und bleibt dauerhaft im Inventar (Lauf Z3_nach_szene). Dazu der Hinweis **H4** (Begruendung
"v0 = 0 -> kein Durchgang" ungenau). Alle als erfuellt bewerteten Punkte bleiben unangetastet (ihre Riegel
werden mitgefahren).

### N1.0 Plan
1. Messung vorher am gebauten Stand (HEAD 6af69ce2): Lauf Z3 nachfahren, Zeitlinie von sub02 (msg 9 auf/zu,
   Szenenende Set(1,0x1B)=0 / Plc_ret / Evt_end), Inventar danach.
2. Belege: ROOM1211 sub02 Bytes (Message_on-Masken, Ablauf nach msg 9), Leser von (4,0xEC)/(4,0xFC),
   RE2-Regel @0x80051670/@0x80051718..@0x80051834, Spur L (gleiche Szene, Unlock-Ton @0x020F8).
3. Umsetzung ohne neue Id: (ROOM1211, msg 9) als Port-Benutzungsstelle fuer 0x42 in
   `re15_schluessel_r36_discard_stelle`.
4. Messung nachher (Unit + echte exe wie Z3), Riegel.
