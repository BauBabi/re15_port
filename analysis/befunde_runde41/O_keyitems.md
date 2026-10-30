# Runde 41 Spur O — keyitems (Dossier, fortlaufend)

Basis: master f5a5e2f7e, Zweig r41/keyitems, Baum .claude/worktrees/r41_keyitems.
Wortlaut (AUFTRAG.md Nachtrag 34):

1. "Auch das mit der Kurbel scheint noch nicht gefixed - leon 'fliegt' quasi immer noch an die richtige Position bis das
   drehen dann los geht."
2. "Bei Resident Evil 2 ist die Kurbel Animation offenbar deshalb nicht kaputt/fliegend, weil man EXPLIZIT die Items im
   Player Inventory auswaehlen muss und dann genau richtig steht bei der Nutzung. Das moechte ich bei Key Items, die nicht
   Schluessel sind bei unseren Port auch haben!"
3. Karten: "Schaue wie es in Resident Evil 2 gemacht wird, und mache es genauso."

## Stand

- [x] Pflichtlektuere: AUFTRAG/VERTRAG/UEBERGABE gelesen
- [x] RE2: wie wird ein Key Item benutzt (Adressen)
- [x] Zensus der Stellen in RE1.5
- [x] Messung vorher (Kurbel-Bildbogen)
- [x] Umsetzung
- [x] Messung nachher
- [x] Riegel

## RE-Belege RE2 (selbst disassembliert, info/re2leon/PSX.EXE, ghidra_re2_Leon.txt; Stand Schritt 1)

### Wie RE2 ein Key Item benutzt (vollstaendige Kette)
1. **Nutzstelle = AOT sce 6 FLAG_CHG, sat 0x21** (Spieler, Auto-Scan, Vorwaerts-Punkt 620 @0x80051088 `li 0x26c`): Nutzlast
   `0a 00 <Item-Id> 00 01 00` = Bank 0x0A, Bit = Item-Id, Wert 1. Handler 0x800a73c4[6] = 0x800519c0 (group/bit/on). Das
   Rechteck ist dasselbe wie das des Text-AOT sce 4 / Ereignis-AOT sce 5 an derselben Stelle, Beispiele:
   ROOM1080 @0x01982 `2c 03 06 21 00 00 b0 aa 3d aa 46 05 54 06 0a 00 32 00 01 00` (Valve Handle 0x32, Rechteck wie Text
   @0x0196E `2c 02 04 31 ...`), ROOM1110 @0x020A0 `2c 07 06 21 00 00 0a a2 ec bc 08 07 08 07 0a 00 43 00 01 00` (Crank 0x43,
   Rechteck wie Text @0x020B4 `2c 08 04 31 ...`), ROOM2110 @0x01812 `2c 09 06 21 00 00 36 a0 c4 cc 6c 07 08 07 0a 00 34 00 01 00`
   (Red Card Key 0x34, Rechteck wie Ereignis @0x017FE `2c 05 05 31 ... ff 00 18 0a` -> sub10 Text).
2. **Bank 0x0A = DAT_800cfbe0..ec (128 Bit)** ist bildlokal: FUN_800536c4 (Skript-Lauf je Bild) ruft am Ende FUN_80052e20
   (@0x800537a0): Bank 0x0A = 0, Variablen 0x800d47ec/ee/f0/f2 = -1 (@0x80052e38 `sh v0,DAT_800d47f0`).
3. **USE** = Klassifizierer FUN_8006affc (Item-Seite, Unterzustand 3 @0x8006af38): Waffe < 0x14 -> anlegen; Fall 0:
   Id < 0x1e -> Text 5; Id-0x23 <= 0xb -> Heilung (Tabelle); sonst @0x8006b21c `FUN_80077360(&DAT_800cfbe0, id)` = Bit-Test:
   gesetzt -> Unterzustand 3; nicht gesetzt -> Text 7 @0x8006b248 `FUN_8002fe38(0xaf0010,0xe400,7,0)` ("kann ich hier nicht
   benutzen"), Unterzustand 1 wartet (@0x800e873c & 0x80).
   Unterzustand 3 @0x8006b2e0: `sb zero,DAT_800d5bf1` (Schliessen) + @0x8006b2ec `sh v0,DAT_800d47f0` = Variable 2 := Item-Id.
4. **Schliessen**: Tabelle @0x800a9aac[0] = 0x80068f08 FUN_8002c1a0(0x200,0x1800,7,1) Blende aus, [1] = 0x80068f40 wartet, Blende
   an; Ende-Zustand 0x80068cd4: **@0x80068cf8 `lh v1,DAT_800d47f0` / @0x80068cfc `li v0,0x32` / @0x80068d04 `li v0,0x4b` /
   @0x80068d0c `li v0,0x43`**: fuer Valve Handle, Manhole Opener und Crank wird die WAFFE ABGELEGT (@0x80068d20-d40: 5bf8 := 0x80,
   5bfa := 0, 69f3 := 1, 69f4/f5 := alter Platz/Waffe) - die naechste Oeffnung legt sie wieder an (@0x80068be4-c34).
5. **Skript**: der Raum-Task prueft je Bild `cmp 0,2,0,<Item-Id>` (Op 0x23 @0x80054474, Variablen s16 @0x800d47ec[i]):
   ROOM1080 sub01 @0x01B98 `23 00 02 00 32 00` -> evt_exec 6; ROOM1110 sub01 @0x02336 `23 00 02 00 43 00` -> evt_exec 0x0d;
   ROOM2110 sub06 @0x01AD8 `23 00 02 00 34 00`. Spielschleife: Skript @0x800263b0 (FUN_80053644) -> Spiel -> AOT-Scan @0x80026690.
   Waehrend der Blende nach dem Status steht das Skript (@0x80025cb0 `lui v1,0xef00` -> DAT_800cfbdc).
6. **Aktionstaste an derselben Stelle zeigt NUR den Text** (ROOM1080 msg 0 "A water pressure valve", ROOM1110 Text-AOT Slot 8):
   keine Frage, kein Besitz-Test.
7. **USE an falscher Stelle**: Text 7 (siehe 3), Inventar bleibt offen.

### Kurbel in RE2 (das Vorbild des Nutzers), ROOM1110
sub13 @0x02EA8: Set(2,7), Set(1,0x1b), Pos_set @0x02EDA/@0x02EE6, Member_set Gier @0x02EF0 `34 0f 00 0c`, Evt_next @0x02EF4, dann
Evt_exec Thread 8 sub15 @0x02F06 -> sub15 @0x02FC1 **`3f 00 0f 45` Plc_motion(0,0x0F,0x45)**: Flags 0x45 = 0x40 (KEINE Blende)
+ 0x04 (Schleife) + 0x01 (@0x80065d7c Hand-Effekt FUN_80016200). RE2-Handler @0x80056b8c: `sh a2,460(v1)` = +0x1CC.
RE1.5-Blende: @0x80050cf8 `ori v0,zero,0x7` / @0x80050cfc `sb v0,143(v1)` +0x8f := 7, @0x80050d24 `andi v0,v0,0x40` / @0x80050d30
`sb zero,143(v1)`: Bit 0x40 der Flags schaltet die Blende ab. ROOM2020 @0x0A9C `3f 00 00 00` hat Flags 0 = 7-Bild-Blende.
Am Ende von sub13 @0x02F66 `62 43` sce_item_lost(0x43): die Kurbel verlaesst das Inventar.

### Karten in RE2 (Frage des Nutzers, Antwort "mache es genauso")
Zensus aller 495 RE2-RDTs (Skript scratch re2_usezensus.py / re2_doorkeys.py):
- **Red Card Key 0x34, ROOM2110: Kartenleser = USE wie die Kurbel**: Slot 9 sce 6 + sub06 @0x01AC2: Ck(4,0x0e)==0, Var0==9,
  Var2==0x34, Ck(4,0x3c)==1 -> @0x01AE6 Set(4,0x0e), @0x01AEA `62 34` sce_item_lost, sub08; Strom aus -> @0x01AF4 Message_on 1.
  Aktionstaste am Leser: Slot 5 sce 5 -> sub10 (nur Text "The power to the card reader is off").
- **Lab Card Key 0x61 (ROOM60C0 Slot 2, ROOM6140 Slot 0), Master Key 0x62 (6060), Platform Key 0x63 (7000): TUERSCHLOSS** wie die
  Schluessel 0x58-0x5C, 0x5F: Door_aot_set Lock-Bit @pc[29] & 0x80, key_type @pc[30]; Handler 0x80051514 sucht das Item
  (@0x80051628 FUN_800696cc) und benutzt es bei der Aktionstaste ohne Frage (@0x8005164c Text 5 "You have used the X").
- Regel daraus: Karte an einem LESER/Ereignis -> USE; Karte als Schloss einer Tuer (Tuer-Satz) -> Aktionstaste.

### Alle USE-Stellen in RE2 (Item-Namen PSX.EXE-Tabelle ab Datei 0x8ed4f, 0xF7-getrennt)
0x2F Lighter 2060/20D0/4030/5010/60A0; 0x32 Valve Handle 1080/4080/4090; 0x33 Red Jewel 10B0; 0x34 Red Card Key 2110; 0x3B-0x3E
Plugs 3030; 0x43 Crank 1110; 0x44-46/0x50 Film 2080; 0x47 Unicorn Medal 2000; 0x48/49 Medaillen 4070; 0x4A G. Cogwheel 1110; 0x4B
Manhole Opener 21A0; 0x4C Main Fuse 6080/6110; 0x4D Fuse Case 60D0; 0x54/55 Joint Plugs 7020; 0x56 Cord 2040/20F0; 0x5D/5E C. Panel
Key 5040/5030; 0x60 MO Disk 6160.

## Messung vorher (gebauter Stand master f5a5e2f7e, Lauf k_vor)

Weg wie integration_r41_kleinkram_b_kurbel (Leon, Tuer 2050 -> 2020, Handgun, Lauf nach Westen, Aktionstaste, Text, Ja), Bilder
RE15_FRAMEDUMP mit RE15_WINDOW_SCALE=1 (F325..F385):
- F341 Frage zu, Lage (2914,-3388) Gier 1920; F342 Lage (3650,-3018) Gier 1024 (Pos_set @0x0A8C): gehaltenes Apply-Bild + 6 Bilddauern;
  F343..F361 Stehen an der Kurbel, F362 Plc_motion (`[scd] Plc_motion(entity=0, motion=0, flags=0x00)`).
- **F362..F366 ist das "Fliegen"**: die 7-Bild-Blende @0x80050cfc mischt die Stehpose in den ersten Kurbel-Schluessel: der Koerper
  schiebt sich seitlich ueber die Reling, kippt in F364 mit abgespreiztem Bein nach links, in F366 steht die Kurbel-Pose
  (Zoom: Spalten F361..F366 in `k_vor_z.png`, scratchpad). Die Lage aendert sich dabei nicht (F362..F399 (3650,-3018)): es ist der
  Wurzel-Versatz des Raum-RBJ Clip 0 gegenueber der Stehpose, der interpoliert wird. Das passt zum Wortlaut "fliegt quasi an die
  richtige Position bis das drehen dann los geht"; die Spuren davor haben Sprung und Standbild behandelt, nicht diese Blende.
- Im Original steht dieselbe Blende (ROOM2020 @0x0A9C `3f 00 00 00`, E-Dossier: "Stehen, Zwischenpose, Kurbeln gleich"); RE2 hat sie
  an der Kurbel NICHT: `3f 00 0f 45` (Flags 0x40 = keine Blende, siehe RE-Belege). Der Mechanismus des Nutzers ist damit belegt.

## RE-Belege RE1.5 (Zensus aller 206 RDTs mit Header)

Die Kurbel und die Zeitbombe sind in RE1.5 **keine Gegenstaende**, nur Flags: "You've taken the Crank." (3,114) ROOM2010 sub02 @0x008CA,
"You've taken the Time Bomb." (3,146) ROOM1110 sub03 @0x00D32, "You've taken the Fuse." (3,108) ROOM2030 sub06 @0x01FF2. Im Inventar
gibt es dafuer nichts auszuwaehlen. Namenstabelle 102 Eintraege (DEBUG.BIN @0x800c495c): 0x35 "Timer Bomb" (Bild im Block 0x35 vorhanden,
Rechtecke RE2-Form), 0x40 "Fuse", kein "Crank". RE2: Item 0x43.

| Raum | Item | Slot / Beleg Aot_set | Ereignis | Beschreibung | Frage | Besonderheit |
|---|---|---|---|---|---|---|
| 1090 | 0x31 Fire Extinguisher | 2 sub00 @0x02336 sat 0xB1 (Viereck) | sub06 @0x02702 | msg 7 | msg 8 @0x02708 | nur Leon-Raum |
| 10D0/10D1 | 0x38 Blue Keycard | 1 main00 @0x0103E | sub20 | Kombi msg 7 | Kombi msg 7 @0x019AE (10D1 @0x01998) | Tastenfeld danach |
| 11E0/11E1 | 0x39 Yellow Keycard | 4 main00 @0x0158E | sub20 @0x01F96 | Kombi msg 7 | Kombi msg 7 | |
| 1230/1231 | 0x37 Red Keycard | 5 main00 @0x00BB0 | sub20 @0x01476 | Kombi msg 7 | Kombi msg 7 | |
| 11E0/11E1 | 0x30 Pliers | 6 sub00 @0x017D0 | sub26 @0x020AC | msg 10 | msg 11 @0x020B2 | |
| 1100/1101 | 0x44 Minidisc Player w/ Disc | 2 sub00 @0x00B14 | sub07 @0x00D50 | msg 1 | msg 3 @0x00D56 | |
| 1050/1051 | 0x40 Fuse | 7 sub00 @0x00C22 | 2 (Port k_mit) | msg 0 + msg 2 | Port 20 | rolltor_1050.c |
| 2010/2011 | 0x40 Fuse | 8 Port-Platz | 20 (Port) | Port 40 | Port 41 | ventilkette_r41.c |
| 2020/2021 | **Crank** | 3 sub00 @0x009AE | sub02 @0x00A5C | msg 0 | msg 1 @0x00A62 | Flag-Item, Pos_set |
| 2040/2041 | 0x35 Timer Bomb | 8 sub00 @0x01456 | sub07 @0x016E8 | msg 0 | msg 5 @0x016EE | Flag-Item |
| 2060/2061 | 0x40 Fuse | 9 sub00 @0x010C6 | sub18 @0x01678 | msg 3 | msg 4 @0x0167E | Flag-Item |
| 3050/3051 | 0x46 Red Master Keycard | 10 main00 @0x01FB4 | sub10 @0x02578 | msg 3 | msg 4 @0x0257E | |
| 4000 | 0x47 Blue Master Keycard | 2 sce 5 main00 @0x010BC | sub01 @0x0137C -> sub02 @0x01426 | msg 0 | msg 1 @0x01440 | 4001: msg 1 ohne Message_on |
| 4030/4031 | 0x41 Spark Plug | 5 sce 5 main00 @0x00516 | sub01 @0x005DE -> sub02 @0x00612 | msg 0 | msg 1 @0x0063A (4031 @0x0067C) | Tor Ck(5,3,1) @0x00636 |

Bleiben wie im Original:
- **Green Keycard 3010/3011**: Slot 6 sce 3 @0x020D2 -> sub02 @0x02302 ohne Frage ("You've used the Green Keycard.", Aot_on 2): Karte als
  TUERSCHLOSS, genau die RE2-Form Lab Card Key (Aktionstaste, Item wird gesucht). USE dort: "You can't use it here." wie RE2 @0x8006b248.
- **Latch Key 3091** und alle Tuerschluessel (Auftrag; RE2 Tuer-Handler @0x80051628).
- **Research Center ID Card 40A0/40A1**: msg 4 "Use the Research Center ID Card?" hat in keinem Skript ein Message_on (Zensus), kein Item
  in der 102er-Tabelle, msg 3 "An ID card is required" Text-Platz: nicht erreichbar, nichts zu tun.
- **4001**: msg 1 ohne Message_on (Elzas Skript fuehrt dort anders), keine Stelle.

## Umsetzung

Neue Dateien: engine/src/keyitems_r41.c, include/re15_keyitems_r41.h, engine/src/gen/keyitems_r41.inc (erzeugt von
tools/r41_keyitems/keyitems_export.py aus info/re2leon, md5 im Kopf; `--pruefen`). Keine neuen Bits, keine neuen Nachrichten-Ids, kein
Patch an shared_assets/PSX, die RDT-Skripte bleiben bytegleich.

Haken in gemeinsamen Dateien (je 1-2 Zeilen, Kommentar "Runde 41 Spur O"):
| Datei | Stelle | Wirkung | Beleg |
|---|---|---|---|
| menu_common.c | state5_classifier vor c3=6 | USE auf der Stelle: Inventar schliesst (s_phase 2) | RE2 @0x8006b21c, @0x8006b2e0 |
| menu_common.c | close_phase Ende | Ereignis startet, s_stage 0: die Welt laeuft in der Blende | PORT-WAHL, siehe unten |
| scd_vm.c | op_message_on | Frage entfaellt (Aktion: Nein), USE-Lauf: alle Texte bis zur Frage entfallen (Ja) | RE2 ROOM1110 Slot 8 sce 4 / sub13 |
| scd_vm.c | op_evt_next | Evt_next hinter einem entfallenen Text zaehlt nicht | damit Cut_chg/Pos_set im ERSTEN Skripttakt fallen |
| scd_vm.c | op_sleep | Kurbel: der Sleep 20 @0x0A98 vor Plc_motion entfaellt | RE2 sub13 -> sub15 ohne Sleep |
| scd_vm.c | op_plc_motion | Kurbel: Flag 0x40 keine Blende: anim_frac 0 statt "bleibt" (re15_actor_set_motion hat vorher 7 gesetzt) | RE2 @0x02FC1 `3f 00 0f 45`, RE1.5 @0x80050cfc 7, @0x80050d24 `andi 0x40`, @0x80050d30 `sb zero,143(v1)`; 0 von 1359 Plc_motion der RDTs tragen 0x40 |
| scd_vm.c | op_plc_ret | Kurbel-Ende: Stehpose ohne Mischung, der Wechsel faellt in den Schnitt @0x0B0E | PORT-WAHL, Form RE2 sub13: Blende @0x02F24 `53 02 02 07 00 18`, Pos_set, Plc_ret, Blende @0x02F4E `53 02 02 07 00 e8` |
| msg_common.c | re15_dialog_step Seitenumbruch | Kartenleser: Text endet vor der Frage-Seite (FSM 4 END_WAIT) | msg 7 `02 00` vor "Will you operate" |
| game_step_common.c | vor ventilkette_tick | Besitz der Flag-Gegenstaende, Waffe zurueck, Ende des USE-Laufs | |
| scd_room_setup.c | Ende des Blocks | Merker je Raum zuruecksetzen | |
| re15_inv_screen.c, item_prompt_common.c, item_icon_common.c, itps_common.c, inv_render_pc.c, main.c | Muster re15_gatorszene_r41_* | Name, CHECK-Text, Icon, Bild der Kurbel; Rechtecke Block 0x35 | tools/r41_keyitems |
| main.c | neben RE15_FIRE_AOT | Messhaken RE15_USE_TEST (kein Spielverhalten) | |

Konstanten:
- RE15_R41O_KURBEL_ITEM 0x33: PORT-WAHL, nie platziert (0 von 164 Item_aot_set), Eigenschaftssatz @0x8007500c wie Schluesselkarten, Name/CHECK/
  Icon/Bild im Muster re15_schluessel_r36 ersetzt. Form = RE2 Item 0x43 (Name @0x8F041, CHECK @0x9206D, Tile 67, Block 67).
- RE15_R41O_BOMBE_ITEM 0x35: DEBUG.BIN Name "Timer Bomb", CHECK "Time Bomb", Bild vorhanden, Rechtecke auf (0,489)/(832,256) gesetzt.
- VORWAERTS 620: @0x80042bd0 `ori 0x26c`. INV_KAPAZITAET 10: 0x800b0fbc, FUN_8004df2c @0x8001df14.

Ablauf USE (Kurbel): Klassifizierer -> re15_keyitems_r41_use (Band, Vorwaerts-Punkt/Zentrum wie die Aktionstaste) -> close_phase (Blende aus,
Teardown wie EXIT) -> scd_event_fire(2), s_stage 0 -> erster Skripttakt: msg 0/1 entfallen, Ck(12,31,0) wahr, Aot_reset, Set(3,113), Cut_chg 3,
Pos_set, Gier, Sleep entfaellt, Plc_motion ohne Blende -> Spielbild blendet ein, Leon kurbelt schon.

PORT-WAHL "Welt laeuft in der Blende": RE1.5 @0x8001cc34 und RE2 @0x80025cb0 halten das Skript waehrend der Einblende an; RE2 setzt danach
Pos_set sichtbar, faellt dort aber kaum auf, weil die Kurbel im Rechteck liegt (ROOM1110). In ROOM2020 liegt Pos_set 870 Einheiten neben dem
Standort beim Druecken (Rechteck @0x009AE ist 1000 westlich der Kurbel), also muss die Lage VOR dem ersten Bild stehen (Auftrag: "Cut_chg und
Pos_set laufen, BEVOR das Spielbild zurueckkommt").

PORT-WAHL Waffe: RE2 legt die Waffe bei Valve Handle/Manhole Opener/Crank ab (@0x80068cfc-@0x80068d40, Waffe 0 @0x80068d34) und erst bei der
naechsten Inventar-Oeffnung wieder an (@0x80068be4-@0x80068c34): bis dahin ist der Spieler unbewaffnet. Uebernommen: Waffe 0 waehrend der
Szene (kein Anhang an Knochen 11, die kreisende Pistole ist weg). Abweichung: zurueck bei Plc_ret @0x0B16 statt bei der naechsten Oeffnung,
weil ROOM2020 vier Zombies setzt (main00 @0x008F6..) und ein stilles Unbewaffnet-Sein wie ein Fehler aussaehe; der Ausruest-Platz 25c8 bleibt.

Flag-Gegenstaende: soll = genommen && !benutzt (Kurbel (3,114)/(3,113), Bombe (3,146)/(3,53), Sicherung (3,108)/(3,144) + (9,125) im Kasten).
soll und nirgends (Inventar, Kiste) -> Eintrag; soll faellt -> Eintrag weg (nur an der Flanke, weil Item 0x40 auch die Sicherung aus ROOM1150 sein
kann). Das heilt auch Spielstaende von vor dieser Aenderung. Ist das Inventar dann voll, bleibt an der Stelle die alte Frage (keine tote
Stelle). Beim Aufnehmen mit vollem Inventar: Item-Modal "You can't carry any more items" (Zustand 5/8) statt der Frage, Flag bleibt 0.
Die Zuendkerze (Item 0x41, keine "used"-Zeile, also keine Wegwerf-Abfrage) verlaesst das Inventar mit (3,78) @0x0068C (RE2 sce_item_lost).

Weitere PORT-WAHL "Szenenende": RE2 verdeckt den Wechsel Kurbel -> Stehen mit einer Blende um Pos_set und Plc_ret (sub13 @0x02F24 ... @0x02F4E). RE1.5 hat
dort keine Blende, nur den Schnitt @0x0B0E `29 03` im selben Takt wie Plc_ret @0x0B16. Mit der Original-Mischung kippte Leon NACH dem Schnitt sieben Bilder lang
aus der Kurbel-Pose in die Stehpose (Lauf n_ende vorher: F443 Kurbel-Pose, F444..F447 gekippt, F448 steht), also dasselbe "Fliegen" am anderen Ende. Darum steht
die Stehpose im USE-Lauf ohne Mischung im ersten Bild nach dem Schnitt. Altstand-Weg (Frage + Ja) bleibt mit Mischung wie im Original (Gegenprobe im Riegel).

ROOM2010 Kasten traegt `RE15_R41O_FLAGITEM`: ohne Eintrag (Altstand, Inventar voll) bleibt die Frage wie an den anderen Flag-Stellen.

## Messung nachher (Engine-Stand be7f0c811, Laeufe in `$TEMP/o41/run`, RE15_FRAMEDUMP mit RE15_WINDOW_SCALE=1)

Lauf n_stand (Leon, Weg von 2050, Handgun, wie Messung vorher, dann START, ITEM, Kurbel, USE):
- F255 USE, F256..F262 Inventar blendet aus, F263 schwarz, **F264 Pos_set @0x0A8C, Cut_chg 3 und Plc_motion(flags=0x40) im selben Takt**, F265..F267 blendet ein (Lichter zuerst),
  F268 erstes volles Bild: Leon steht an der Kurbel und kurbelt (Zoom `O_belege/nachher_kurbel_leon.png`, Spalten F267..F282). Vorher lagen zwischen Pos_set und
  Plc_motion 20 Bilder Stehen und danach fuenf Bilder Blende; beides gibt es auf diesem Weg nicht mehr.
- Keine Pistole am Knochen 11 waehrend der Szene (Waffe 0, RE2 @0x80068d34); `[keyitems] Waffe 3 wieder in der Hand` genau bei Plc_ret.
- Lage ab dem ersten Bild (3650,-3018) Gier 1024, Kommandowort 4.

Anlauf und Richtung (je Lauf Bild N nach dem Pos_set gegen n_stand, Leon-Ausschnitt 100 x 130 = 13000 Pixel, Pixelbytes verglichen, N = 4..32):
| Lauf | Anlauf | START bei | Pos_set-Bild | verschiedene Pixel | mittlere Abweichung (0..255) |
|---|---|---|---|---|---|
| n_renn | Rennen nach Westen | x 3617 | 341 | 23..58 | hoechstens 0,05 |
| n_geh | Gehen nach Westen | x 3356 | 340 | 22..58 | hoechstens 0,06 |
| n_west | Stand, Blick West (Gier 2048) | x 3300 z -3382 | 174 | 49..71 | hoechstens 0,07 |
| n_nord | Stand, Blick Nord (Gier 1024) | (2600,-3018) | 334 | 24..45 | hoechstens 0,05 |
| n_ost | Stand, Blick Ost (Gier 0) | (1700,-3388) | 334 | 24..45 | hoechstens 0,05 |
| n_elza | Elza, Stand, ROOM2021 | (2914,-3388) | 174 | 886..955 | 2,1 (anderes Modell, gleiche Lage, Bogen `nachher_kurbel_elza.png`) |

Die Reste sind einzelne Pixel im Licht-/Wasserflimmern, Anlauf und Richtung gehen in das Bild nicht ein: das ist der Sinn von "dann genau richtig steht".
Rennen: `O_belege/nachher_kurbel_rennen.png`.

Szenenende (Lauf n_ende nachher, `O_belege/nachher_kurbel_ende.png`, F440..F449): F442 letztes Bild Cut 4, F443 erstes Bild Cut 3, Leon steht schon, die Waffe ist da, kein Kippen.

Inventar: `O_belege/nachher_check_kurbel.png` (Bild RE2 Crank, Text "The end is square-shaped"), `O_belege/nachher_check_bombe.png` (Bild Block 0x35 mit gesetzten Rechtecken, Text "Time Bomb").

Andere Stellen an der echten exe: ROOM10D0 USE Blue Keycard: Inventar zu, ohne msg 7 direkt "You've used the Blue Keycard." (Lauf n_karte, Riegel integration_r41_keyitems_karte);
ROOM2040 USE Zeitbombe: Inventar zu, ohne Text das Zeitzuender-Feld (Lauf n_bombe); ROOM2010: "Will you take the Crank?", Ja, "You've taken the Crank.", Eintrag im Inventar, CHECK-Bild (Lauf n_auf).

Rennen und Gehen laufen im Gang z -3388, weil (2600,-1500) in soliden Zellen liegt (Koerper-Schub auf z -3018 in F6/F7). Der Riegel rechnet ab Bild 260: eine Taste in einem
frueheren Bild trifft sonst den Vorraum, und START waehrend dessen Text bleibt stehen (aca3c |= 0x8000 @0x8001cd48, game_step_common.c) und oeffnet das Inventar spaeter.

## Tests

Neu (nur probes/r41_keyitems.cmake, tests/unit/test_r41_keyitems.c, tests/unit/test_r41_keyitems_exe.c, tests/integration/test_r41_keyitems.cmake):
- unit_r41_keyitems_stellen: jede Zeile der Tabelle gegen die ausgelieferte RDT (Message_on der Frage, Ck(12,31,0) dahinter, Aot_set Slot/Ereignis), 3010 @0x02302 ohne Frage.
- unit_r41_keyitems_use: je Stelle und Raumvariante falsches Item -> 0, falscher Ort -> 0, USE an der Stelle -> 1, Frage entfallen, Antwort Ja, Wirkung des Ja-Zweigs.
- unit_r41_keyitems_aktion: je Stelle Beschreibung ja, Ja/Nein-Auswahl nein, Antwort Nein, Wirkung nein; Kartenleser endet vor "Will you operate the card reader?".
- unit_r41_keyitems_besitz / _voll: Eintrag aus dem Flag, Verbrauch, Kiste, Altstand, fremde Sicherung bleibt, volles Inventar (Aufnahme-Modal, Stelle bleibt mit Frage).
- unit_r41_keyitems_kurbel: Pos_set und Plc_motion im ersten Skripttakt, anim_frac 0, Clip 0, Cut 3; Gegenprobe Altstand: Frage bleibt, Blende am Anfang und am Ende.
- unit_r41_keyitems_waffe: Waffe 0 waehrend der Szene, zurueck bei Plc_ret, Platz 25c8 bleibt, Stehpose ohne Mischung.
- unit_r41_keyitems_daten: Name, CHECK-Text, Icon, Bild gegen info/re2leon (tools/r41_keyitems/keyitems_export.py --pruefen).
- integration_r41_keyitems_{kurbel,elza,renn,geh,west,nord,ost,falsch,aktion,karte,karte_aktion,aufnahme}: echte exe, echtes Menue (START, ITEM, Platz, USE).

Nachgezogen, weil die Stellen keine Ja-Eingabe mehr kennen (Riegel bleibt inhaltlich, Weg neu):
- probe_r26_inventar B3/B4/B4c (fuenfter Port-Name 0x33), test_1090_flame_out_pin, test_r37_feuerloescher, test_r37_kistenregel, test_room1050_sicherung, test_r34n_a_rolltor (W/L ueber Ja-Weg),
  r21_discard_wegwerfen, test_r39_inventar (Teil discard: Beschreibung, dann USE), test_r41_kleinkram_b_kurbel (USE), test_r41_kleinkram_b_takt (Teil frage im Altstand: Inventar voll),
  test_r41_kleinkram_b_exe (Pos_set und Plc_motion duerfen im selben Bild fallen).
- integration: test_r37_codepanel, test_r37_kistenregel, test_r37_feuerloescher, test_r39_inventar (G), test_r41_kleinkram_b (kurbel = USE; geh/renn/losl = Altstand mit vollem Inventar).

Gegenproben, die die Riegel selbst scharf gemacht haben: `use` lief zuerst rot, weil room_boot das Inventar leerte und die Karten-Zeilen das Bank-9-Bit nicht setzten (Ck(9,52,1) @0x019C0: ohne Bit
kommt msg 8 statt msg 9); `kurbel` lief rot mit `anim_frac 7`: daraus kam der echte Mangel (Bit 0x40 nullte nicht, siehe Haken-Tabelle); `falsch` trug eine Lage, die der Koerper-Schub auf die Stelle schob.

## OFFEN

Aus den drei Punkten ist nichts offen. Bewusst nicht angefasst:
- Die Stelle der Kurbel bleibt das Rechteck @0x009AE (x 2200..3000, z -4150..-3350): der Original-Platz der Aktionstaste liegt rund 1000 westlich der Kurbel, die Lage danach kommt aus Pos_set @0x0A8C.
- Altstand: ein Spielstand mit gesetztem Flag und vollem Inventar behaelt an Kurbel, Bombe und Sicherung die alte Frage (samt Original-Blende), bis ein Platz frei ist; dann erscheint der Eintrag.
- Gebaut und getestet nur am PC (Spurgrenze); keine Patches an shared_assets/PSX, keine neuen Assets unter shared_assets/RE2, keine Sprachdateien, keine neuen Nachrichten-Ids, keine Bits aus 126-133.

## Fuer den Nutzer

- **Bedienung:** START, ITEM, Gegenstand waehlen, USE. Steht man an der richtigen Stelle (gleiche Pruefung wie die Aktionstaste: Ebene und Blickpunkt), schliesst das Inventar und die Handlung startet sofort, ohne Text und ohne Frage. Sonst "You can't use it here.", das Inventar bleibt offen.
- **Aktionstaste an diesen Stellen:** zeigt nur noch die Beschreibung, keine Frage mehr. Das gilt fuer Feuerloescher (1090), Zange (11E0), Minidisc-Spieler (1100), Kurbel (2020), Zeitbombe (2040), Sicherung (1050, 2010, 2060), Zuendkerze (4030) und die Karten unten.
- **Kurbel:** das Spielbild kommt erst zurueck, wenn Leon an der Kurbel steht und schon dreht. Aus dem Stand, im Gehen, im Rennen und aus drei Blickrichtungen ist das Bild dasselbe (Tabelle oben), Elza ebenso. Am Ende der Szene kippt er nicht mehr zurueck, der Wechsel faellt in den Schnitt.
- **Neu im Inventar:** Kurbel und Zeitbombe gab es im Original nur als Flag. Die Kurbel hat Name, Bild, Symbol und Text aus RE2, die Bombe das ausgelieferte Bild. Beide verschwinden bei der Benutzung. Das gilt auch fuer laufende Spielstaende. Volles Inventar beim Aufnehmen: "You can't carry any more items", der Gegenstand bleibt liegen.
- **Karten wie in RE2:** Karte am LESER = USE aus dem Inventar (Blue/Yellow/Red Keycard 10D0, 11E0, 1230, Red Master Keycard 3050, Blue Master Keycard 4000), RE2-Vorbild Red Card Key. Karte als TUERSCHLOSS = Aktionstaste wie bisher (Green Keycard 3010), RE2-Vorbild Lab Card Key. Schluessel, Latch Key 3091 und "You don't need this key any more" bleiben unveraendert.
- **Waffe:** waehrend der Kurbel-Szene hat Leon die Waffe nicht in der Hand wie in RE2, danach ist sie wieder da. RE2 legt sie erst beim naechsten Oeffnen des Inventars wieder an; das habe ich nicht uebernommen, weil in dem Raum vier Zombies warten.

