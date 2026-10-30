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

- [ ] Pflichtlektuere: AUFTRAG/VERTRAG/UEBERGABE gelesen
- [ ] RE2: wie wird ein Key Item benutzt (Adressen)
- [ ] Zensus der Stellen in RE1.5
- [ ] Messung vorher (Kurbel-Bildbogen)
- [ ] Umsetzung
- [ ] Messung nachher
- [ ] Riegel

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
