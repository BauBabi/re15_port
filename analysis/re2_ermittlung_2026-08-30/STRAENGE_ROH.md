# Strang: boss-identitaet

## Kernaussage
Das Finale von RE2 (PL0) ist STAGE7 = ROOM7000..ROOM7040; die Tuer-Records (opcode 0x3B, Byte +22 = stage-1, Byte +23 = Raum-Index) ergeben genau eine Kette 6170/6010/F170 -> 7000 -> {7010 -> 7020} und 7000 -> 7030 -> 7040, wobei ROOM7040 eine Sackgasse mit nur einer Ruecktuer ist und damit die Arena. Die Sce_em_set-Records (opcode 0x44, 22 Byte, Handler 0x8005714c, PC-Advance addiu +22 @0x800576e4) spawnen dort — und NUR dort im ganzen Spiel — 1x Typ 0x36 (Slot 0) und 4x Typ 0x37 (Slots 1-4), identisch in ROOM7040.RDT@0x0011ac.. und ROOMG040.RDT@0x001184..; alle fuenf geparkt auf (-32000,0,-32000). Der eigentliche Endgegner ist Typ 0x36: ROOM7040 Bank1/sub07 pollt per work_set 2e 03 00 (= entity_list[1] = em_set-Slot 0, Handler 0x80055904/0x8005597c: work = *(0x800cfe1c + idx*4)) das Member 0x17 dieses Gegners und startet danach BGM-Stop, Kamerafahrt und XA-Abspann — Typ 0x37 haengt in derselben Arena, hat aber keine Abbruchbedingung. Einen NAMEN fuer die Kreatur gibt es in den Bytes nicht: der englische MSG-Block von ROOM7040 @0x1cf4 sagt woertlich "I have no choice but to stop that creature here." Das RE2-Gegenstueck zur RE1.5-Dispatch-Tabelle @0x80072bac ist 0x800CFE5C (96 Eintraege, Index = entity+0x08, gelesen in FUN_8004a694 @0x8004a73c-0x8004a754) — im EXE-Image komplett 0x00000000, weil es zur Laufzeit aus der 2-Bank-Tabelle 0x800CFFDC (2 x 96 x 4 B, Stride 384) nachgeladen wird (0x8001b80c / 0x8001b868); die Bank-Tabelle selbst wird vom EXE-Initialisierer 0x8001b274..0x8001b708 statisch befuellt, und dort steht fuer Typ 0x36 der Wert 0x801000BC (Bank0, sw @0x8001b45c) bzw. 0x8010D0BC (Bank1, sw @0x8001b694) — beides echte AI-Root-Funktionen im Gegner-Overlay-Slot 0x80100000 bzw. 0x8010D000. Die Overlay- und Modell-Bytes fuer 0x36/0x37 sind NICHT als eigene .BIN im Repo, wohl aber vollstaendig in CDEMD0.EMS (TOC in der RE2-EXE @0x8009adf4, 4 Records je Typ ab 0x10): Typ 0x36 = Sektor 3444/3456 (AI, je 23476 B), 3468 (TIM, 132640 B), 3533 (EMD, 112340 B); Typ 0x37 = 3588/3600 (AI, je 23024 B), 3612 (TIM, 33312 B), 3629 (EMD, 59376 B) — dieselbe TOC ist im Port bereits als re15_port/engine/src/gen/re2_ems_toc.inc vendored und stimmt Zahl fuer Zahl.

## Befunde

### SCD-Dispatch-Tabelle RE2  [byte-belegt]
Die RE2-SCD-Opcode-Dispatch-Tabelle liegt @0x800A74C8 und hat genau 143 Eintraege (0x00..0x8E). Sie wird beim Raumstart mit 1024 Byte nach 0x1F800000 (Scratchpad) kopiert und im VM-Kern per lbu/sll 2/lw indiziert.

**Beleg:** 0x80053598: lui a1,0x800a / 0x8005359c: addiu a1,a1,29896 -> 0x800a74c8 ; 0x800535a0: jal 0x80010778 ; 0x800535a4: addiu a2,zero,1024 (a0 = lui 0x1f80). VM-Dispatcher 0x80053f70: lbu v0,0(a1) ; 0x80053f78: sll v0,v0,2 ; 0x80053f7c-84: lui at,0x800a / addu at,at,v0 / lw v0,29896(at) ; 0x80053f8c: jalr v0. Eintrag [0]=0x800537e4 ... [142]=0x80057714; [143] ist bereits Datenbereich (0x10111010).

**Port-Stand:** nicht geprueft (Port hat eine eigene RE1.5-scd_vm.c; RE2-Opcodes werden dort nicht dispatcht)

### Sce_em_set RE2 = Opcode 0x44, 22 Byte  [byte-belegt]
Handler = Tabellen-Index 68 = 0x8005714c; der PC wird um 22 (0x16) erhoeht. Feldbelegung byte-belegt: +2 = Slot, +3 = Gegner-TYP (-> entity+0x08), +4 u16 -> entity+0x10E, +6 = Etagen-Index (entity+0x106, und entity+0x1C2 = -1800*floor), +7 -> entity+0x1FA, +8 -> entity+0x1CF, +9 -> entity+0x1CE, +10/12/14 i16 = X/Y/Z, +16 u16 = Yaw (entity+0x76), +18 u16 -> entity+0x158, +20 u16 -> entity+0x15A.

**Beleg:** 0x800576dc: lw v1,28(s5) ; 0x800576e4: addiu v1,v1,22 ; 0x800576e8: sw v1,28(s5). Felder: 0x800571ec lbu a0,3(v0) -> jal 0x8001b710 ; 0x80057334 lbu v0,3(v1) / 0x8005733c sb v0,8(s0) ; 0x80057204 lb v0,2(v0) ; 0x8005734c lhu v0,4(v1) / 0x80057354 sh v0,270(s0) ; 0x80057388 sb v0,262(s0) aus +6 ; 0x800573a8-c0 (v*8-v)<<5+v)<<3 negiert -> sh v0,450(s0) ; 0x80057274 lbu v0,7(v0) / 0x80057280 sb v0,506(s0) ; 0x800572c8/e0/f8 lhu +10/+12/+14 -> sh 68/70/72(s0) + sw 56/60/64(s0) ; 0x80057328 lhu v0,16(v1) / 0x80057330 sh v0,118(s0) ; 0x800576b4 lhu v0,18(v1) -> sh 344 ; 0x800576c4 lhu v1,20(v1) -> sh 346.

**Port-Stand:** nicht geprueft

### NPC-Grenze 0x40  [byte-belegt]
Wie in RE1.5 trennt RE2 bei Typ < 0x40 (Gegner) vs >= 0x40 (NPC): entity+0x09 wird auf 4 bzw. 128 gesetzt.

**Beleg:** 0x800573c4: lbu v1,8(s0) ; 0x80057410: sltiu v1,v1,0x40 ; 0x8005741c/0x80057420: addiu v0,zero,4 ; 0x80057424: addiu v0,zero,128 ; 0x80057428: sb v0,9(s0)

**Port-Stand:** nicht geprueft

### RDT-Kopf RE2: SCD-Bank-Offsets  [byte-belegt]
RE2-RDT-Kopf = 8 Byte Zaehler + 23 u32 Offsets (0x08..0x60), erste Sektion beginnt bei 0x64. SCD-Bank 0 steht bei RDT+0x48, SCD-Bank 1 bei RDT+0x4C. Jede Bank beginnt mit einer u16-Zeigertabelle, erster Eintrag = Tabellenlaenge.

**Beleg:** 0x800535c4: lw v0,-7388(v0) (=0x800ce324, RDT-Basis; gesetzt @0x80049eb8 aus 0x800ce328) ; 0x800535d4: lw v0,72(v0) [RDT+0x48] -> sw 0x800d8cbc, jal 0x80053138 mit a0=0 ; 0x800535f4: lw v0,76(v0) [RDT+0x4C] -> sw 0x800d8cbc, jal 0x80053138 mit a0=1. Dass 0x800d8cbc die u16-Zeigertabelle ist, zeigt der gosub-Handler 0x800541e8-f8: lw v1,0x800d8cbc / sll a2,a2,1 / addu a2,a2,v1 / lhu v0,0(a2). Gegenprobe ROOM7000.RDT: 0x48=0x1438, 0x4C=0x147c -> Bank0 = 68 B = 2 B Tabelle + 66 B Code (= main00.scd 66 B in der bereits entpackten room7000/scd).

**Port-Stand:** nicht geprueft

### Finale-Raumkette STAGE7  [byte-belegt]
Genau 17 Tuer-Records im ganzen Spiel zielen auf Stage-Byte 6 (= STAGE7). Eingaenge: ROOM6010, ROOM6170, ROOMF170 -> ROOM7000. Innen: 7000->7010, 7000->7030, 7010->7000, 7010->7020, 7020->7010, 7030->7000, 7030->7040, 7030->7030 (Etagenwechsel), 7040->7030. ROOM7040 hat exakt EINE Tuer (zurueck nach 7030) = Sackgasse = Arena.

**Beleg:** Door_aot_set = Opcode 0x3B, 32 Byte (Handler 0x80054be4, 0x80054c40: addiu v0,v0,32; Record-Koerper ab +2 wird in die AOT-Tabelle 0x800ce558[aot] gelegt, 0x80054c30/0x80054c34). Feld +22 = Ziel-Stage (0-basiert), +23 = Ziel-Raum, +24 = Cut. Beispiele: ROOM6170.RDT@0x0035a2 -> stage=6 room=0 ; ROOM7000.RDT@0x00143a -> stage=6 room=1 ; ROOM7000.RDT@0x00145a -> stage=6 room=3 ; ROOM7030.RDT@0x001c78 -> stage=6 room=4 cut=12 ; ROOM7040.RDT@0x0010fe raw=3b 00 01 31 00 00 61 4e 08 9e 08 07 48 0d f4 98 00 00 c9 a5 00 00 06 03 04 00 2a 02 00 00 00 00 -> stage=6 room=3. Gegenprobe der 0-Basis: ROOM6160.RDT@0x001d2a -> stage=5 room=23 = ROOM6170 (0x17=23).

**Port-Stand:** nicht geprueft (Port kennt nur RE1.5-Raeume)

### ROOM7040 = Endkampf-Arena (Spawns)  [byte-belegt]
ROOM7040.RDT (245348 B, nCut=15, nOmodel=7, SCD0@0x10fc, SCD1@0x1120) spawnt in Bank1/sub00 fuenf Gegner: Slot 0 = Typ 0x36, Slots 1..4 = Typ 0x37. Alle auf X=-32000, Y=0, Z=-32000 (0x8300 = -32000) geparkt, alle mit Feld+7 = 0x27. Slot 0 traegt Flag 0x8000 in +4, Slot 1 traegt 0x0001, Slots 2-4 0x0000.

**Beleg:** ROOM7040.RDT 0x0011ac: 44 00 00 36 00 80 00 27 00 ff 00 83 00 00 00 83 00 00 00 00 00 00 | 0x0011c2: 44 00 01 37 01 00 00 27 00 ff 00 83 00 00 00 83 00 00 00 00 00 00 | 0x0011d8: 44 00 02 37 00 00 ... | 0x0011ee: 44 00 03 37 ... | 0x001204: 44 00 04 37 ...

**Port-Stand:** nicht geprueft

### ROOMG040 = identische Arena im zweiten Szenario  [byte-belegt]
ROOMG040.RDT (162808 B, nCut=15) enthaelt dieselben fuenf Records in derselben Reihenfolge; auch der englische Text ist identisch. Die Raumnamen ROOMG0x0 sind die zweite Szenario-Variante der Stage-7-Raeume (Zaehlung 1:1 zu ROOM7xx0: je 5 Dateien; ueber alle Stages: 1/A=30, 2/B=26, 3/C=14, 4/D=16, 5/E=10, 6/F=24, 7/G=5).

**Beleg:** ROOMG040.RDT 0x001184: 44 00 00 36 00 80 00 27 00 ff 00 83 00 00 00 83 00 00 00 00 00 00 ; 0x00119a/0x0011b0/0x0011c6/0x0011dc: 44 00 0{1..4} 37 ... . Tuer ROOMG040.RDT@0x0010d6 -> stage=6 room=3.

**Port-Stand:** nicht geprueft

### Typen 0x36/0x37 sind spielweit einmalig  [byte-belegt]
Census ueber alle 250 ROOM*.RDT: Typ 0x36 kommt 2x vor (ROOM7040, ROOMG040), Typ 0x37 8x (4x + 4x in denselben beiden Raeumen). In keinem anderen Raum des Spiels.

**Beleg:** Vollstaendiger Sce_em_set-Census (Walker mit der aus den 143 Handlern extrahierten RE2-Opcode-Laengentabelle) ueber info/re2leon/PL0/RDT/ROOM*.RDT: 0x36 -> {ROOM7040 x1, ROOMG040 x1}; 0x37 -> {ROOM7040 x4, ROOMG040 x4}. Zum Vergleich haeufigste Typen: 0x1f=245, 0x26=58, 0x21=51, 0x41=51, 0x29=48.

**Port-Stand:** nicht geprueft

### Typ 0x36 ist der Abbruch-Gegner (Endgegner)  [byte-belegt]
ROOM7040 Bank1/sub07 pollt in einer do/edwhile-Schleife das Member 0x17 des Gegners in em_set-Slot 0 (Typ 0x36), maskiert mit 2, vergleicht und schaltet danach BGM aus, faehrt Kamera-Cut 0x0b, versetzt Spieler und Gegner und startet XA 0x0d. Kein solcher Poll existiert fuer Slot 1..4 (Typ 0x37).

**Beleg:** ROOM7040.RDT 0x001992: 11 00 1c 00 (do) ; 0x001996: 2e 03 00 (work_set kind=3 idx=0) ; 0x00199a: 3d 10 17 (member_copy var0x10 <- member 0x17) ; 0x00199e: 26 00 06 10 02 00 (calc AND 2) ; 0x0019a4: 25 04 10 (copy) ; 0x0019aa: 12 06 / 0x0019ac: 23 00 04 05 02 00 (cmp) ; danach 0x0019be: 51 01 05 00 00 00 (sce_bgm_control) ; 0x0019ce: 29 0b (cut_chg) ; 0x0019ea: 59 00 0d 00 (xa_on). work_set-Semantik aus dem Handler 0x80055904: lbu v1,1(v0) / lb a1,2(v0) / addiu v0,v0,3 / v1-1 / sltiu <5 / lw 0x800111f0[v1-1] / jr v0 ; Zweig kind=3 @0x8005597c: sll v0,a1,2 ; lw v0,-484(at) = 0x800cfe1c + idx*4. Und em_set legt seinen Entity-Zeiger genau dorthin: 0x80057214 sll a0,a1,2 (a1 = Slot+2), 0x80057234 sw v1,15404(v0) mit s4=0x800cc1e8 -> 0x800CFE14 + (Slot+2)*4 = 0x800CFE1C + Slot*4.

**Port-Stand:** nicht geprueft

### Der Name der Kreatur steht NICHT in den Daten  [byte-belegt]
Der englische MSG-Block von ROOM7040 (RDT+0x40 = 0x1cf4) enthaelt drei Strings; der Kampf-String nennt den Gegner ausdruecklich nur 'that creature'. Es gibt im gesamten STAGE7-Text keinen Eigennamen. RE2 nutzt dieselbe MSG-Codierung wie RE1.5 (ASCII = Byte + 0x24).

**Beleg:** ROOM7040.RDT 0x1cf4 Zeigertabelle [6,35,85]; Eintrag[1] dekodiert: "I can't put my friends in danger<END> I have no choice but to stop that creature here." Rohbytes u.a. 0x45+0x24='i', 0x4f+0x24='s'. Eintrag[0]: "Various research materials"; Eintrag[2]: "An old typewriter / I could save my progress if I had an ink ribbon / There's no time for that." ROOMG040 (RDT+0x40) ist wortgleich. Verifikation der Codierung an ROOM7000.RDT@0x1aa4[0]: 3f 3d 4a 3a 50 00 43 4b 00 3e 3d 3f 47 00 4a 4b 53 1a -> "I can't go back now."

**Port-Stand:** nicht geprueft

### STAGE7-Kontext aus dem Raumtext  [byte-belegt]
STAGE7 ist der Untergrund-/Zug-Komplex: ROOM7000 'A device to open the tunnel gate / The power is not supplied'; ROOM7010 'Storage for the high capacity plugs'; ROOM7020 'A high yield generator ... the emergency train will be activated upon restoration of the power'; ROOM7030 'There's an activation lever for the train motor / insufficient power supply'; ROOM7040 = Arena.

**Beleg:** Englische MSG-Bloecke (RDT+0x40): ROOM7000@0x1aa4, ROOM7010@0x1e94, ROOM7020@0x1e00, ROOM7030@0x32e4, ROOM7040@0x1cf4 — dekodiert mit ASCII = Byte+0x24.

**Port-Stand:** nicht geprueft

### ROOM7000 Spawns  [byte-belegt]
ROOM7000.RDT (175544 B, nCut=15, SCD0@0x1438, SCD1@0x147c): 6 Sce_em_set — 1x Typ 0x4F (Slot 0xFF, alles 0, Flag 0x4000) und 5x Typ 0x17 auf realen Positionen.

**Beleg:** 0x0014a4: 44 00 ff 4f 00 40 00 00 00 ff 00*12 ; 0x001680: 44 00 00 17 06 00 00 2e 00 a6 b4 15 00 00 41 e6 8b 07 00 00 00 00 -> (5556,0,-6591) dir=1931 ; 0x001696 Slot1 (13330,0,506) dir=1035 ; 0x0016ac Slot2 (15219,0,-1320) dir=1083 ; 0x0016c2 Slot3 (14263,0,-5096) dir=843 ; 0x0016d8 Slot4 (18839,0,-9470) dir=2011.

**Port-Stand:** nicht geprueft

### ROOM7010 / ROOM7020 / ROOM7030 Spawns  [byte-belegt]
ROOM7010 (215824 B, nCut=12, nOmodel=10): 0 Sce_em_set. ROOM7020 (160188 B, nCut=16): 2 — Typ 0x41 (Slot 0xFF) und Typ 0x2B (Slot 0, spielweit einziger 0x2B). ROOM7030 (255092 B, nCut=14, nOmodel=11): 3 — Typ 0x55 (Slot 0, spielweit einziger 0x55) und 2x Typ 0x4F (beide Slot 1, zwei Varianten mit +18 = 272 bzw. 271).

**Beleg:** ROOM7020.RDT 0x001220: 44 00 ff 41 00 02 06 00 00 ff a0 92 c8 ce 14 ba 40 06 00 00 00 00 -> (-28000,-12600,-17900) dir=1600 ; 0x001236: 44 00 00 2b 00 00 00 1a 00 28 c0 e0 28 23 58 9e 00 00 00 00 00 00 -> (-8000,9000,-25000). ROOM7030.RDT 0x001eba: 44 00 00 55 00 40 00 00 00 ff 87 c1 00 00 5f a8 00 f8 00 00 00 00 -> (-15993,0,-22433) ; 0x001f00 und 0x001f34: 44 00 01 4f 00 40 ... 8f be 00 00 de 99 00 04 1{0,f} 01 44 00 -> (-16753,0,-26146) dir=1024.

**Port-Stand:** nicht geprueft

### Entity-Dispatch-Tabelle RE2 = 0x800CFE5C  [byte-belegt]
Das RE2-Gegenstueck zu RE1.5' 0x80072bac ist 0x800CFE5C: die Frame-Schleife FUN_8004a694 liest entity+0x08 (Typ), skaliert mit 4, addiert 0x800cfe18+68 = 0x800CFE5C und ruft den geladenen Zeiger per jalr. Dieselbe Indizierung noch einmal fuer den Zweitaufruf (Schadens-/Reaktions-Pfad).

**Beleg:** 0x8004a6f8: addiu s2,s2,-488 (lui 0x800d) -> s2 = 0x800CFE18 ; 0x8004a708: addiu s4,s2,68 -> s4 = 0x800CFE5C ; 0x8004a73c: lbu v0,8(s0) ; 0x8004a744: sll v0,v0,2 ; 0x8004a748: addu v0,v0,s4 ; 0x8004a74c: lw v0,0(v0) ; 0x8004a754: jalr v0. Zweiter Aufruf: 0x8004a76c lbu v1,8(s0) / 0x8004a77c sll v1,v1,2 / 0x8004a780 addu v1,v1,s4 / 0x8004a788 lw v0,0(v1) / 0x8004a790 jalr v0.

**Port-Stand:** nicht geprueft

### 0x800CFE5C ist im EXE-Image LEER (Laufzeit-Tabelle)  [byte-belegt]
Ein table-Dump von 0x800CFE5C ueber 96 Eintraege liefert 96 x 0x00000000 — RE2 hat KEINE statische Typ->Handler-Tabelle im EXE wie RE1.5. Die Tabelle wird beim Laden eines Gegner-Overlays aus einer Bank-Tabelle nachgefuellt.

**Beleg:** python .claude/skills/re15-psx-disasm/scripts/re2_disasm.py table 0x800cfe5c 96 -> alle 96 Eintraege 0x00000000 (Werkzeug meldet selbst 'WARNING: 96/96 entries are 0x00000000'). Datei-Offset = 0x800 + 0x800cfe5c-0x80010000 = 0xC065C in info/re2leon/PSX.EXE.

**Port-Stand:** nicht geprueft

### Bank-Tabelle 0x800CFFDC (2 x 96 Eintraege, Stride 384)  [byte-belegt]
Beim Overlay-Wechsel kopiert FUN_8001b710 den Handler aus 0x800CFFDC + bank*384 + typ*4 nach 0x800CFE5C + typ*4. bank ist der Gegner-Overlay-Slot (0 oder 1).

**Beleg:** 0x8001b7ec-0x8001b80c: sll v0,a3,1 / addu v0,v0,a3 / sll v0,v0,7 (= a3*384) / addu v0,v0,t0 (t0 = 0x800cc1e8, gesetzt @0x8001b72c) / addu v0,v0,v1 (v1 = typ*4, @0x8001b7e4) / lw v0,15860(v0) [= 0x800CFFDC + ...] / addu v1,t0,v1 / sw v0,15476(v1) [= 0x800CFE5C + typ*4]. Zweite, identische Stelle 0x8001b860/0x8001b868.

**Port-Stand:** nicht geprueft

### Eintrag des Endgegner-Typs 0x36 in der Bank-Tabelle  [byte-belegt]
Der EXE-Initialisierer 0x8001b274..0x8001b708 schreibt beide Baenke statisch. Fuer Typ 0x36: Bank0-Slot 0x800CFFDC+0x36*4 = 0x800D00B4 <- 0x801000BC ; Bank1-Slot 0x800D015C+0x36*4 = 0x800D0234 <- 0x8010D0BC. Fuer Typ 0x37: 0x800D00B8 <- 0x80100178 und 0x800D0238 <- 0x8010D178. Der Delta 0xD000 zwischen den Baenken ist genau die Slot-Groesse (= 53248 = Paddinggroesse eines Gegner-Overlays).

**Beleg:** 0x8001b450: lui v0,0x8010 ; 0x8001b454: addiu v0,v0,188 (= 0x801000BC) ; 0x8001b45c: sw v0,180(at) mit at = lui 0x800d -> 0x800D00B4. 0x8001b460/64: addiu v0,v0,376 (= 0x80100178) ; 0x8001b46c: sw v0,184(at) -> 0x800D00B8. Bank1: 0x8001b688/8c: lui v0,0x8011 / addiu v0,v0,-12100 (= 0x8010D0BC) ; 0x8001b694: sw v0,564(at) -> 0x800D0234. 0x8001b698/9c: addiu v0,v0,-11912 (= 0x8010D178) ; 0x8001b6a4: sw v0,568(at) -> 0x800D0238. Zum Vergleich Zombie-Typen 0x10..0x1F: alle <- 0x801002C0 (0x8001b288-0x8001b30c).

**Port-Stand:** nicht geprueft

### AI-Root Typ 0x36 = 0x801000BC (echter Code, disassembliert)  [byte-belegt]
An 0x801000BC im Overlay-Chunk fuer Typ 0x36 steht eine vollstaendige AI-Root-Funktion mit dem RE2-Gegner-Prolog: Freeze-Gate auf 0x800CFBDC & 0x20000000, danach Dekrement des Zaehlers entity+0x1D3 (7-Bit-Maske) und Test von entity+0x226 Bit0.

**Beleg:** CDEMD0.EMS Sektor 3456 (Datei-Offset 0x6C0000), Chunk-Offset 0xBC, Ladeadresse 0x80100000: 27bdffc8 addiu sp,sp,-56 / afb3002c sw s3,44(sp) / 00809821 addu s3,a0,zero / 3c02800d lui v0,0x800d / 8c42fbdc lw v0,-1060(v0) [0x800CFBDC] / 3c032000 lui v1,0x2000 / 00431024 and v0,v0,v1 / 144000a9 bne v0,zero,0x8010038c / 926301d3 lbu v1,467(s3) / 3062007f andi v0,v1,0x7f / 10400002 beq / 2462ffff addiu v0,v1,-1 / a26201d3 sb v0,467(s3) / 96620226 lhu v0,550(s3).

**Port-Stand:** nicht geprueft

### AI-Root Typ 0x37 = 0x80100178 (echter Code, disassembliert)  [byte-belegt]
An 0x80100178 steht die AI-Root von Typ 0x37, gleicher Freeze-Prolog; sie hat zusaetzlich einen frueh liegenden Despawn-Pfad: wenn entity+0x10E == 8, ruft sie 0x80104F64 und loescht Bit0 von entity+0x00 (Entity deaktivieren).

**Beleg:** CDEMD0.EMS Sektor 3600 (Datei-Offset 0x708000), Chunk-Offset 0x178, Ladeadresse 0x80100000: 27bdffc8 / afb1002c / 00808821 / 3c02800d / 8c42fbdc / 3c032000 / 00431024 / 144000c0 bne v0,zero,0x8010049c / 9623010e lhu v1,270(s1) / 24020008 addiu v0,zero,8 / 14620008 bne v1,v0,0x801001cc / 0c0413d9 jal 0x80104f64 / 8e220000 lw v0,0(s1) / 2403fffe addiu v1,zero,-2 / 00431024 and v0,v0,v1 / ae220000 sw v0,0(s1).

**Port-Stand:** nicht geprueft

### Gegner-Overlay-Slots liegen bei 0x80100000 und 0x8010D000  [byte-belegt]
Jeder Gegner-Typ wird in zwei Link-Varianten geliefert: rec1 ist gegen 0x80100000 gelinkt, rec0 gegen 0x8010D000. Damit koennen zwei Gegner-Typen gleichzeitig aktiv sein — genau das, was ROOM7040 mit 0x36 (Slot0) + 0x37 (Slot1) braucht.

**Beleg:** Chunk Sektor 3456 (Typ 0x36 rec1): interne Zeiger 0x801003CC..0x80105A10, jal-Ziele 0x80100784..0x80105388. Chunk Sektor 3444 (Typ 0x36 rec0): Zeiger 0x8010D3CC..0x80111E38, jal-Ziele 0x8010D784..0x80112388 — exakt +0xD000. Gleiches Bild fuer Typ 0x37: Sektor 3600 -> 0x801004F0.., Sektor 3588 -> 0x8010D4F0... Die STAGE*.BIN sind dagegen KEINE Gegner-Overlays: STAGE7.BIN beginnt mit `48 00 00 00` + MIPS, dessen lui/addiu-Paare 0x8011A054/0x8011A0F4/0x8011A144 bilden, also Ladeadresse ~0x8011A000 (Ziel-Tabelle @0x800107F0 = 0x8011A000).

**Port-Stand:** nicht geprueft

### Gegner-Datei-TOC der RE2-EXE @0x8009ADF4  [byte-belegt]
Die Datei-Tabelle fuer Gegner ist ein (Sektor,Groesse)-Paar-Array mit 4 Records je Typ ab Typ 0x10: rec0 = AI-Overlay (0x8010D000-gelinkt), rec1 = AI-Overlay (0x80100000-gelinkt), rec2 = TIM, rec3 = EMD. Die Sektoren sind relativ zum Anfang von CDEMD0.EMS. Eine zweite Tabelle @0x8009B880 gilt fuer das andere Szenario.

**Beleg:** Index-Bildung im Binder 0x8001aaa8: 0x8001ab44 addiu s0,s0,-16 ; 0x8001ab48 sll s0,s0,2 ; 0x8001ab4c ori a0,s0,0x2 -> ((typ-0x10)*4)|2 = rec2 (TIM). Zugriff in 0x8001b934: 0x8001b994 sll v1,t1,3 ; 0x8001b9a8 lw v0,-21000(at) [0x8009ADF8 = Groesse] ; 0x8001b9c0 lw t0,-21004(at) [0x8009ADF4 = Sektor]. Zweite Tabelle: 0x8001ba28 lw v0,-18300(at) [0x8009B884] / 0x8001ba40 lw t0,-18304(at) [0x8009B880]. Verifikation gegen Bytes: idx0 = (0,53068) und CDEMD0.EMS[26*2048 .. +53068] ist BYTE-IDENTISCH mit info/re2leon/COMMON/BIN/EMZ0.BIN.

**Port-Stand:** BEREITS IM PORT: re15_port/engine/src/gen/re2_ems_toc.inc (600 Werte) + re15_port/engine/src/re2_ems.c (re2_ems_toc_entry: i = ((kind-0x10)*4+rec)*2, off = sector*0x800). Selbst gegengeprueft: Port-TOC liefert fuer kind 0x36 exakt 3444/23476, 3456/23476, 3468/132640, 3533/112340 — identisch zu meiner EXE-Lesung.

### Dateien fuer Typ 0x36 und 0x37 — was im Repo liegt  [byte-belegt]
Es gibt KEINE eigene EMOVL36_*.BIN / EMS36.BIN im Repo. Die Bytes sind aber vollstaendig vorhanden, naemlich als Chunks in CDEMD0.EMS: Typ 0x36 AI = Sektor 3444 und 3456 (je 23476 B), TIM = Sektor 3468 (132640 B), EMD = Sektor 3533 (112340 B); Typ 0x37 AI = Sektor 3588 und 3600 (je 23024 B), TIM = Sektor 3612 (33312 B), EMD = Sektor 3629 (59376 B). Die Chunks sind wohlgeformt: die TIMs beginnen mit Magic 0x10/Flags 0x09 (8bpp+CLUT), die EMDs mit dir_offset+count=8.

**Beleg:** CDEMD0.EMS@0x006C6000 (Sektor 3468): 10 00 00 00 09 00 00 00 0c 06 00 00 00 00 e0 01 (CLUT-Block 0x60C = 3 Paletten a 256) ; @0x006E6800 (3533): b4 b6 01 00 08 00 00 00 04 00 00 00 28 00 00 00 ; @0x0070E000 (3612): 10 00 00 00 09 00 00 00 0c 02 00 00 (CLUT-Block 0x20C = 1 Palette) ; @0x00716800 (3629): d0 e7 00 00 08 00 00 00 ; @0x00702000 (3588): 3a 00 00 00 14 db 10 80 00 dc 10 80 40 dc 10 80 (Zeigertabelle 0x8010DB14..). Die im Repo liegenden Overlay-BINs decken andere Typen ab (byte-identisch verifiziert): EMZ0.BIN = Sektor 26 (Typ 0x10 rec1), EMD0G_MOD0.BIN = Sektor 1206 (Typ 0x20 rec1), EMOVL21_S0.BIN = Sektor 1320 (Typ 0x21 rec1), EMS25.BIN = Sektor 1825 (Typ 0x25 rec1), EMS26.BIN = Sektor 1901 (Typ 0x26 rec1).

**Port-Stand:** Die Bytes sind im Port vorhanden: re15_port/shared_assets/RE2/CDEMD0.EMS ist byte-identisch zu info/re2leon/PL0/PLD/CDEMD0.EMS (sha256 8724200c1ed5e8d3235ceb85..., 11124736 B), ebenso liegt shared_assets/PSX/EMD/CDEMD0.EMS. Es existiert KEIN Port-Code fuer Typ 0x36/0x37 (engine/src hat nur enemy_ai_re2_{zombie,dog,crow,spider}.c).

### Typ->Modellgruppen-Map @0x800A7704  [byte-belegt]
Eine Byte-Paar-Tabelle (typ, modell_id), 0xFF-terminiert, Default 0x80, wird von FUN_80052AF4 linear durchsucht. Fuer 0x36 und 0x37 ist die Abbildung die Identitaet (0x36->0x36, 0x37->0x37); NPC-Typen 0x41..0x5B werden dagegen auf 0x50/0x51 gefaltet, 0x4A auf 0x10.

**Beleg:** 0x80052af4: lui v1,0x800a / addiu v1,v1,30468 (=0x800A7704) / addiu a1,zero,255 / lbu v0,0(v1) / beq v0,a0 -> lbu v0,1(v1) / addiu v1,v1,2 / ... / addiu v0,zero,128 (Default). Bytes @0x800A7704: 10 10 11 10 12 10 13 10 15 10 16 10 17 10 18 10 1e 10 1f 10 20 20 21 21 ... 36 36 37 37 39 39 ... 41 51 42 50 43 51 44 50 45 45 46 50 47 51 48 50 49 51 4a 10 4b 51 4f 45 50 50 51 51 54 50 55 51 58 50 59 51 5a 50 5b 51 ff 00. Aufrufer in Sce_em_set: 0x800574a0 lbu a0,8(s0) / 0x800574a4 jal 0x80052af4.

**Port-Stand:** nicht geprueft

### RE2-SCD-Opcode-Laengen (aus den Handlern extrahiert)  [byte-belegt]
Alle 143 RE2-Opcode-Laengen wurden direkt aus dem PC-Advance jedes Handlers gezogen (nicht aus Doku). Wichtige Werte: 0x2C aot_set=20, 0x2D obj_model_set=38, 0x3A sce_espr_on=16, 0x3B door_aot_set=32, 0x44 Sce_em_set=22, 0x4E item_aot_set=22, 0x60 kage_set=14, 0x67 aot_set_4p=28, 0x68 door_aot_set_4p=40, 0x69 item_aot_set_4p=30, 0x8D/0x8E=24. 0x07 else_ck springt um die u16 an +2 (Record 4 Byte); 0x09 sleep advanciert nur 1 und laesst 0x0A sleeping (3) folgen.

**Beleg:** Beispiele: 0x44 @0x800576e4 addiu v1,v1,22 ; 0x2C @0x80054b50 addiu v0,v0,20 ; 0x3B @0x80054c40 addiu v0,v0,32 ; 0x2D @0x80055324 (+38) ; 0x3A @0x80056620 addiu s1,s1,16 / sw s1,28(s2) ; 0x67 @0x80054bd8 (+28) ; 0x68 @0x80054cc8 (+40) ; 0x69 (+30) ; 0x07 @0x80053974 lhu v0,2(v1) / addu v1,v1,v0 / sw v1,28(a0) ; 0x09 Handler 0x800539dc (+1), 0x0A Handler 0x80053a24 (+3).

**Port-Stand:** nicht geprueft (Port hat nur die RE1.5-Tabelle in scd_vm.c; die weicht nachweislich ab)

### DATENHYGIENE: info/re2leon/PL0/PLD/CDEMD0/EM0*.EMD|TIM sind FALSCH benannt  [byte-belegt]
Die dort abgelegten Split-Dateien decken sich NICHT mit der EXE-TOC. Wer aus EM036.EMD/EM036.TIM misst, misst nicht Typ 0x36. Autoritativ ist ausschliesslich die TOC @0x8009ADF4 bzw. re2_ems_toc.inc.

**Beleg:** EM010.EMD (147456 B) findet sich in CDEMD0.EMS bei Datei-Offset 0x29800 = Sektor 83; die TOC sagt fuer Typ 0x10 rec3 Sektor 85. EM036.TIM (133120 B) findet sich bei 0x61B800 = Sektor 3127; die TOC sagt fuer Typ 0x36 rec2 Sektor 3468. EM036.EMD (149504 B) findet sich bei 0x63C000 = Sektor 3192; die TOC sagt fuer Typ 0x36 rec3 Sektor 3533/112340 B. Zusaetzlich enthaelt die Datei EM036.EMD ab Offset 0xB800 einen fremden Overlay-Chunk (`38 00 00 00` + Zeigertabelle 0x8010DDDC..), was ein reiner Modell-Container nicht tut.

**Port-Stand:** Der Port nutzt korrekt die TOC (re2_ems_locate/re2_ems_toc_entry) und NICHT diese Dateinamen — dort besteht kein Fehler.

### Weitere spielweit einmalige Typen (Boss-Kandidaten ausserhalb STAGE7)  [byte-belegt]
Aus dem Census sind ausser 0x36/0x37 nur wenige Typen singulaer: 0x23 nur ROOM40A0, 0x2B nur ROOM7020, 0x2C nur ROOM6030, 0x33 nur ROOM5090, 0x34 nur ROOM6170, 0x3E nur ROOM60D0. 0x31 kommt 3x vor (ROOM5000, ROOM5050, ROOM5090).

**Beleg:** Census aus dem Sce_em_set-Walk aller 250 ROOM*.RDT (Opcode 0x44, 22 B). Zaehlungen: 0x23=1, 0x2b=1, 0x2c=1, 0x33=1, 0x34=1, 0x36=2, 0x37=8, 0x3e=1, 0x31=3.

**Port-Stand:** nicht geprueft

### Restrisiko im Census  [abgeleitet]
Der lineare SCD-Walk erzeugt in Datenbereichen einzelne Falschtreffer. Im Gesamtcensus tauchen 9 Typwerte ausserhalb des gueltigen Bereichs auf (0xAC, 0xBA, 0xC0, 0xD5 x4, 0xF0, 0xF3 x2, 0xF7 x2, 0xF8 x3) — das sind Desync-Artefakte, keine Spawns. Alle STAGE7-Treffer liegen dagegen im gueltigen Bereich und stehen in sauber terminierten Skriptbloecken.

**Beleg:** Gueltiger Typbereich ist 0x00..0x5F (Dispatch-Tabelle 0x800CFE5C hat 96 Eintraege; die Modell-Map @0x800A7704 endet bei 0x5B mit 0xFF). Die STAGE7-Funde 0x17/0x2B/0x36/0x37/0x41/0x4F/0x55 liegen alle darin und wurden zusaetzlich einzeln in der Opcode-Umgebung geprueft (ROOM7040 sub00: aot_set/aot_set/aot_set/aot_set/ifel_ck/ck/aot_set/5x Sce_em_set/3x obj_model_set/cut_replace...).

**Port-Stand:** -

## Fehlende Daten
- Ein RE2-Ghidra-Decompilat der Gegner-Overlays: RE2_Quellcode_Overlays/STAGE1..7/ ist praktisch leer (nur STAGE6 hat 4 Dateien). Es fehlt damit jede decompilierte Fassung der AI-Roots 0x801000BC (Typ 0x36) und 0x80100178 (Typ 0x37) — die muessen aus CDEMD0.EMS Sektor 3456 bzw. 3600 frisch disassembliert werden.
- Ausgeschnittene Overlay-BINs fuer die Endkampf-Typen: es gibt kein info/re2leon/COMMON/BIN/EMOVL36_S0.BIN / _S1.BIN und kein EMOVL37_S*.BIN (analog zu EMOVL10/EMOVL21/EMS25/EMS26). Inhalt waere CDEMD0.EMS[3444*2048 .. +23476] und [3456*2048 .. +23476] bzw. [3588*2048 .. +23024] und [3600*2048 .. +23024].
- Ausgeschnittene Modell-/Textur-Dateien fuer 0x36/0x37 unter korrektem Namen: die vorhandenen info/re2leon/PL0/PLD/CDEMD0/EM036.* und EM037.* sind nachweislich die falschen Chunks. Korrekt waere CDEMD0.EMS[3468*2048 .. +132640] (TIM 0x36), [3533*2048 .. +112340] (EMD 0x36), [3612*2048 .. +33312] (TIM 0x37), [3629*2048 .. +59376] (EMD 0x37).
- Ein RE2-Savestate/RAM-Dump aus ROOM7040: 0x800CFE5C ist im EXE-Image null, die tatsaechlich installierten Handler-Zeiger und die HP/Member-Werte des Bosses (Member 0x17, entity+0x1D3, entity+0x226) lassen sich nur dynamisch belegen. Im Repo liegt kein RE2-Savestate (stage_saves/ enthaelt nur RE1.5).
- Eine RE2-Szenario-Zuordnung der Dateinamen: nicht byte-belegt ist, ob ROOM7xx0 = Szenario A und ROOMGxx0 = Szenario B (oder umgekehrt). Es fehlt die EXE-Routine, die aus Stage-Index + Szenario den RDT-Dateinamen bildet — die haette ich fuer eine harte Aussage gebraucht.
- Der englische Name des Endgegners ist nirgends in den Daten: weder in den MSG-Bloecken von ROOM7040/ROOMG040 noch als String in info/re2leon/PSX.EXE (strings findet dort nur 'd:/bio2/room/emd/em000.emd' und '.tim'). Eine Namensnennung waere eine Erfindung.

## Gegenpruefung — GEKIPPT

### [9] Zeigertabelle des englischen MSG-Blocks ROOM7040
Die zitierte Tabelle [6,35,85] steht NICHT bei 0x1cf4. Sie steht bei RDT+0x3C = 0x1c4c, dem ZWEITEN (japanischen, kanji-codierten) MSG-Block. Der Ermittler hat den Beleg aus dem falschen Block kopiert und trotzdem den Text des richtigen zitiert.

**Gegenbeleg:** ROOM7040.RDT Header idx13(+0x3c)=0x1c4c, idx14(+0x40)=0x1cf4. Bytes @0x1cf4 = 06 00 26 00 7d 00 -> ptrs [6,38,125]. Bytes @0x1c4c = 06 00 23 00 55 00 -> ptrs [6,35,85]; dessen Eintraege dekodieren zu '{ef}{80}{ef}{81}{ee}G...' (Kanji-Codes), nicht zu ASCII. Der Text selbst ('...stop that creature here.') liegt korrekt bei 0x1cf4.

### [19] 'exakt +0xD000' — der zitierte rec0-Zeigerbereich ist falsch
Angegeben wird fuer Typ 0x36 rec0 'Zeiger 0x8010D3CC..0x80111E38'. 0x80111E38 = rec1-Max 0x80104E38 + 0xD000, aber rec1-Max ist 0x80105A10. Die eigene Zahl widerlegt das eigene 'exakt +0xD000'.

**Gegenbeleg:** Eigener Wort-Scan ueber die geschnittenen Chunks (CDEMD0.EMS Sektor 3456 bzw. 3444, je 23476 B): t36_rec1 71 Zeiger 0x801003cc..0x80105a10; t36_rec0 71 Zeiger 0x8010d3cc..0x80112a10. 0x80112a10-0x80105a10 = 0xD000. Der SCHLUSS stimmt, die zitierte Zahl 0x80111E38 nicht.

### [4] 'Genau 17 Tuer-Records' — die Aufzaehlung enthaelt nur 12
Die Zahl 17 stimmt, die daneben stehende Liste ist unvollstaendig und wird als vollstaendige Kette praesentiert. Es fehlen ROOM7030@0x2470, ROOMG000 (2x), ROOMG010 (2x) und ROOMG040@0x10d6 (das der Ermittler in [6] selbst zitiert). 3 Eingaenge + 9 Innen = 12, nicht 17.

**Gegenbeleg:** Eigener Scan aller 250 ROOM*.RDT ueber [scd0 .. naechster Header-Offset > scd1] auf 0x3B-Records mit +22==6: 17 Treffer = ROOM6010@0x502->0, ROOM6170@0x35a2->0, ROOM7000@0x143a->1, ROOM7000@0x145a->3, ROOM7010@0x1e52->0, ROOM7010@0x1e72->2, ROOM7020@0x1106->1, ROOM7030@0x1c58->0, ROOM7030@0x1c78->4, ROOM7030@0x2470->3, ROOM7040@0x10fe->3, ROOMF170@0x3572->0, ROOMG000@0x14a6->0, ROOMG000@0x14c6->0, ROOMG010@0x1e02->0, ROOMG010@0x1e22->2, ROOMG040@0x10d6->3.

### [4] Feldbelegung +22 Stage / +23 Raum / +24 Cut ist NICHT byte-belegt
Als Beleg steht nur der Handler (Laenge 32, AOT-Eintrag = Record+2) plus Rohbytes und eine Plausibilitaets-Gegenprobe. Es wird KEINE Instruktion gezeigt, die diese Offsets als Ziel-Stage/Raum/Cut LIEST. Das ist abgeleitet aus Datenkonsistenz, nicht disassembliert.

**Gegenbeleg:** 0x80054be4..0x80054c4c disassembliert (selbst gefahren): lbu v0,1(v0) -> Index in 0x800ce558; addiu v0,v0,2 / sw v0,0(v1) legt den Koerper ab; addiu v0,v0,32 / sw v0,28(a0). Kein Zugriff auf +20 der AOT-Struktur. Ein Scan der gesamten EXE nach addiu-Immediate 0xe558 findet NUR Setzer (0x800530c4, 0x80054b04, 0x80054b70, 0x80054bf4, 0x80054c60, Loads 0x80055170/0x800551f4) — der Konsument wurde nie lokalisiert.

### [23] 0x2D obj_model_set: Adresse 0x80055324 traegt den Wert 38 nicht
Der zitierte PC-Advance steht dort nicht.

**Gegenbeleg:** 0x80055324 = 'sw a2,56(s1)' (selbst disassembliert, 0x80055318-0x8005532c). Der echte Advance liegt bei 0x800554e0 'addiu v0,v0,38' / 0x800554e4 'sw v0,28(...)' — reproduziert durch eigene handler-begrenzte Extraktion aller 143 Advances.

### [23] '0x07 else_ck ... (Record 4 Byte)'
Die 4 stammt aus dem if-Stack-Pointer, nicht aus einer Record-Laenge. Opcode 0x07 advanciert den PC ueberhaupt nicht um eine feste Laenge, er springt.

**Gegenbeleg:** 0x80053964: lw v0,320(a0) / 0x8005396c: addiu v0,v0,-4 / 0x80053970: sw v0,320(a0)  <- das ist der Stack bei VM+0x140. Danach 0x80053974 lhu v0,2(v1) / 0x8005397c addu v1,v1,v0 / 0x80053980 sw v1,28(a0) = PC += u16@+2.

### [1] Sce_em_set: zwei Zweige verschwiegen
Die Feldliste wird als vollstaendige, unbedingte Belegung praesentiert. Tatsaechlich gibt es (a) einen fruehen Ausstieg, der ALLE Feldschreiber ueberspringt, und (b) eine bedingte Zusatzschreibung. Ausserdem gehen +10/+14 nicht nur nach 0x44/0x48.

**Gegenbeleg:** (a) 0x800571c0 lbu a1,9(v0) / 0x800571c8 beq a1,255 / 0x800571d0 jal 0x80077360 / 0x800571d8 bne v0,zero,0x800576dc — springt direkt auf den PC-Advance. (b) 0x800576c0 lhu v0,270(s0) / 0x800576c8 andi v0,v0,0x4000 / 0x800576cc beq v0,zero,0x800576dc / 0x800576d4 addiu v0,zero,146 / 0x800576d8 sb v0,448(s0) = entity+0x1C0. (c) 0x80057318 sh v0,280(s0) (aus +10) und 0x80057324 sh v0,282(s0) (aus +14) = entity+0x118/+0x11A.

### [8] 'Kein solcher Poll existiert fuer Slot 1..4'
In ROOM7040 Bank1 gibt es sehr wohl work_set kind=3 auf Slot 1 und Slot 3. Nur der member-0x17-Poll fehlt dort. Die Formulierung ist zu weit.

**Gegenbeleg:** ROOM7040.RDT Bank1-Region 0x1120..0x1c24, Byte-Muster '2e 03 kk': idx0 @0x18da/0x1996/0x19dc/0x19f7; idx1 @0x17a0 und @0x1a10; idx3 @0x1a1c. Kontext 0x17a0: '2e 03 01 00 32 00 00 ce 88 e1 ee a3 34 17 01 00' — work_set Slot1, danach 0x32 (Position) und 0x34 (member_set 0x17=1), kein 0x3d-Poll. Kontext 0x1a10/0x1a1c: '2e 03 01 ... 32 00 00 83 00 00 00 83' = Parken auf -32000/-32000.

### [11] ROOM7000 Typ 0x4F: 'Slot 0xFF, alles 0'
Byte +9 ist 0xFF, nicht 0.

**Gegenbeleg:** ROOM7000.RDT@0x14a4 = 44 00 ff 4f 00 40 00 00 00 ff 00 00 00 00 00 00 00 00 00 00 00 00. Index 9 = 0xff. Und dieses Byte ist kein totes Feld: 0x80057358 lbu v0,9(v1) / 0x80057360 sb v0,462(s0) = entity+0x1CE, ausserdem der Gate-Test 0x800571c0/0x800571c8.

### [20] 'Eine zweite Tabelle @0x8009B880 gilt fuer das andere Szenario' — abgeleitet, nicht belegt
Es wird kein Code gezeigt, der zwischen den Tabellen nach Szenario umschaltet. Die Sektorwerte legen eher eine andere DATEI (CDEMD1.EMS) nahe als ein anderes Szenario derselben Datei.

**Gegenbeleg:** Eigene Lesung beider Tabellen (Datei-Offset 0x800+addr-0x80010000, Stride 8): Tabelle1 Typ 0x36 = (3444,23476)(3456,23476)(3468,132640)(3533,112340); Tabelle2 = (3447,...)(3459,...)(3471,...)(3536,...). Nur +3 Sektoren, IDENTISCHE Groessen. Der einzige zitierte Code (0x8001ba28/0x8001ba40) zeigt nur die Adressen, nicht das Auswahlkriterium.

### [16] '0xD000 ... = Paddinggroesse eines Gegner-Overlays'
Das Delta 0xD000 ist byte-true, die Deutung als Overlay-Paddinggroesse ist geraten. Kein Overlay im Repo ist 53248 Byte gross.

**Gegenbeleg:** Groesste beobachtete AI-Overlay-Groesse aus der TOC @0x8009ADF4: 53068 (Typ 0x10 rec0/rec1) < 53248. Die 0xD000 folgt ausschliesslich aus den beiden Link-Basen 0x80100000/0x8010D000 (0x8001b454 addiu v0,v0,188 vs 0x8001b68c addiu v0,v0,-12100).

### [17] 'Freeze-Gate' als Semantik
Der AND-Test und der Fruehausstieg sind byte-true, die Bedeutung 'Freeze' ist aus RE1.5 uebertragen und fuer RE2 nicht belegt.

**Gegenbeleg:** 0x800CFBDC ist ein allgemeines Flagwort mit 36 Zugriffen in der RE2-EXE (eigener Immediate-Scan auf 0xfbdc mit op lw/sw): u.a. 0x8001d31c lw, 0x800257e4 sw, 0x80026424 lw, 0x8003bedc lw, 0x800516dc lw, 0x80053658 lw. Keine Stelle wurde gezeigt, die Bit 0x20000000 als Freeze setzt.

### [17]/[18] Der 'Rohbyte'-Strom ist nicht der literale Wortstrom
Die zitierte Instruktionsfolge laesst die dazwischenliegenden Register-Sicherungen weg und liest sich wie ein zusammenhaengender Dump.

**Gegenbeleg:** Eigener Wort-Dump aus CDEMD0.EMS Sektor 3456 Offset 0xBC: 801000bc 27bdffc8 / c0 afb3002c / c4 00809821 / c8 3c02800d / cc 8c42fbdc / d0 3c032000 / d4 afbf0030 / d8 afb20028 / dc afb10024 / e0 00431024 / e4 144000a9 / e8 afb00020 / ec 926301d3. Die zitierte Folge ueberspringt d4/d8/dc/e8. Inhaltlich stimmt alles, inklusive Sprungziel 0x801000e4+4+0xa9*4 = 0x8010038c.

### [2] 'Wie in RE1.5' — die RE1.5-Haelfte ist unbelegt
Fuer RE2 ist die 0x40-Grenze belegt, fuer RE1.5 wird KEINE Adresse genannt. Der Vergleich ist eine Behauptung ohne Beleg.

**Gegenbeleg:** Der gesamte BELEG-Block von [2] (0x800573c4, 0x80057410, 0x8005741c/20/24, 0x80057428) liegt in info/re2leon/PSX.EXE. Kein einziges RE1.5-Zitat. (Nebenbei: 0x8005741c ist 'j 0x80057428', das 'addiu v0,zero,4' steht im Delay-Slot 0x80057420.)

### [9] EXE-String-Suche war unvollstaendig (Methode, nicht Ergebnis)
'strings findet dort nur d:/bio2/room/emd/em000.emd und .tim' ist falsch, und wichtiger: eine ganze verschobene Stringtabelle ist mit plain strings unsichtbar.

**Gegenbeleg:** Eigener ASCII-Scan (len>=12) der RE2-EXE findet zusaetzlich 0x94808 'd:/bio2/pl0/pld/pl00ch.pld', 0x135c 'd:/bio2/room/data/tetubox1.tim', 0x137c '...tm2'. Ausserdem liegt ab ~0x8ed00 eine um -4 verschobene Stringtabelle ('OAIE;=QPKI=PE?' -> 'SEMI_AUTOMATIC', 0x91380). Ich habe 0x8e800-0x93000 mit -4 dekodiert: reine Item-/Beschreibungstexte, KEIN Eigenname eines Gegners. Das Ergebnis von [9] haelt, die Belegfuehrung nicht.

### [6] 'identische Arena im zweiten Szenario' — die G-Kette ist keine Kette
ROOMG040 ist echt, aber G010/G020/G030 sind Stummel ohne Skript und ohne jeden Text. Von einer zweiten spielbaren STAGE7-Kette traegt der Befund nichts.

**Gegenbeleg:** Eigene Header-Lesung: ROOMG010 Dateigroesse 7756 B, scd0=0x1e00 scd1=0x1e44, Bank1 = 2 Subs; ROOMG020 scd0=0x10fc scd1=0x1104 (Bank0 = 8 B), Bank1 2 Subs; ROOMG030 scd0=0x1bdc scd1=0x1be4, Bank1 2 Subs. Bei G000/G010/G020/G030 sind Header-Index 13 UND 14 (beide MSG-Bloecke) = 0x00000000, also gar kein Text. Nur ROOMG040 (16 Subs, Text wortgleich zu ROOM7040) und ROOMG000 (13 Subs) sind gefuellt.

### Pfadangabe CDEMD0.EMS
Der Befund nennt die Datei durchgehend ohne Pfad; unter info/re2leon/COMMON/ liegt sie NICHT. Wer die zitierten Sektoren nachschneiden will, laeuft ins Leere.

**Gegenbeleg:** ls info/re2leon/COMMON/ -> BIN BSS DATA DOOR SOUND. Die Datei liegt unter info/re2leon/PL0/PLD/CDEMD0.EMS (11124736 B), daneben CDEMD1.EMS (11196416 B) und das Verzeichnis info/re2leon/PL0/PLD/CDEMD0/.

### [26] '9 Typwerte ausserhalb des gueltigen Bereichs'
Die eigene Aufzaehlung nennt 8 verschiedene Werte (0xAC, 0xBA, 0xC0, 0xD5, 0xF0, 0xF3, 0xF7, 0xF8) bei 15 Vorkommen.

**Gegenbeleg:** Abzaehlen der im Befund selbst gelisteten Werte. (Mein eigener, grob ueberinklusiver Roh-Scan liefert eine andere Artefakt-Menge: 0x71 x2, 0xff x2, 0x80, 0xf8, 0x74 — Artefakte haengen am Walker, die Zahl ist kein stabiler Befund.)

## Gesamturteil der Gegenpruefung
Der Kern des Strangs haelt. Ich habe jede zitierte Adresse selbst disassembliert und jeden zitierten Byte-Block selbst gelesen; die harten Aussagen sind reproduzierbar: die 143er-Dispatch-Tabelle @0x800A74C8, Sce_em_set @0x8005714c mit 22 B und der kompletten Feldabbildung, die RDT-SCD-Bank-Offsets +0x48/+0x4C, die Rohbytes der fuenf Spawns in ROOM7040 und ROOMG040, die Einmaligkeit von 0x36/0x37 (durch einen unabhaengigen, bewusst ueberinklusiven Muster-Scan gegengeprueft), die leere Laufzeittabelle 0x800CFE5C, die Bank-Tabelle 0x800CFFDC mit Stride 384, die statischen Eintraege 0x801000BC/0x80100178 und 0x8010D0BC/0x8010D178, die AI-Root-Wortstroeme in CDEMD0.EMS Sektor 3456/3600, die Gegner-TOC @0x8009ADF4 (durch Byte-Identitaet von EMZ0.BIN/EMD0G_MOD0.BIN/EMOVL21_S*/EMS25/EMS26 mit ihren TOC-Sektoren verifiziert), die Modell-Map @0x800A7704 und der Falschbenennungs-Nachweis fuer PL0/PLD/CDEMD0/EM0xx.*. Die 143 Opcode-Laengen habe ich unabhaengig neu extrahiert und sie stimmen. Gekippt habe ich 18 Punkte, davon vier substanziell: (1) [9] belegt den englischen MSG-Block mit der Zeigertabelle des japanischen Blocks; (2) [4] behauptet 17 Tuer-Records und zaehlt nur 12 auf, verschweigt ROOM7030@0x2470 und die vier G-Records, und die Feldoffsets +22/+23/+24 sind reine Datenkonsistenz — der konsumierende Code wurde nie gefunden (mein EXE-weiter Scan auf 0x800ce558 findet nur Setzer); (3) [23] zitiert fuer 0x2D eine Adresse, an der 'sw a2,56(s1)' steht; (4) [19] zitiert eine rec0-Zeigerobergrenze, die dem eigenen '+0xD000' widerspricht. Dazu kommen mehrere als 'byte-belegt' etikettierte Deutungen, die tatsaechlich abgeleitet sind: 'Freeze-Gate' fuer 0x800CFBDC&0x20000000, 'Paddinggroesse' fuer das 0xD000-Delta, 'zweites Szenario' fuer die TOC @0x8009B880 (deren Sektoren nur +3 liegen, bei identischen Groessen), und die RE1.5-Haelfte von [2] ohne eine einzige RE1.5-Adresse. Neu und relevant fuer die Boss-Identitaet: ROOMG010/G020/G030 haben praktisch leeren SCD (zwei Subs) und BEIDE MSG-Header-Eintraege = 0 — die 'zweite Szenario-Kette' existiert als Datei, nicht als Kette; nur ROOMG000 und ROOMG040 sind gefuellt. Und die Namenssuche in der EXE war methodisch zu duenn: es gibt ab ~0x8ED00 eine um -4 verschobene Stringtabelle, die plain strings nicht sieht. Ich habe sie dekodiert (reine Item-/Beschreibungstexte, u.a. 'SEMI_AUTOMATIC', 'Umbrella's', 'typewriter') — kein Eigenname eines Gegners. Das Ergebnis von [9] haelt damit sogar besser als sein Beleg. Achtung fuer die Reproduktion: CDEMD0.EMS liegt unter info/re2leon/PL0/PLD/, nicht unter COMMON/.

---

# Strang: boss-ki

## Kernaussage
Der Gegner-Typ ist selbst aus den RDTs ermittelt: STAGE7 enthaelt genau EINEN Gegner-Cluster, naemlich Typ 0x36 (1x) plus 4x Typ 0x37 in ROOM7040.RDT (und identisch im Szenario-B-Zwilling ROOMG040.RDT) — beide Typen kommen game-weit in KEINEM anderen Raum vor; ROOM7020 hat zusaetzlich den einmaligen Typ 0x2B, ROOM7000/7030 nur Zombies 0x17 bzw. NPCs 0x4F/0x55 (Typ>=0x40 wird von em_set als NPC markiert). Die Wurzel-Funktion des Hirns ist byte-belegt bekannt — Typ 0x36 -> 0x801000BC (Bank 0) bzw. 0x8010D0BC (Bank 1), Typ 0x37 -> 0x80100178/0x8010D178, Typ 0x2B -> 0x8010187C/0x8010E87C, eingetragen von der EXE-Tabelle @0x8001B280-0x8001B704 in das RAM-Array 0x800CFFDC[bank*96+typ] —, ABER die Datei, die diesen Code enthaelt, liegt NICHT im Repo: die CD-Datei-Tabelle @0x8009ADF4 nennt fuer Typ 0x36 zwei Code-Overlays a 23476 Byte (LBA 3444 / 3456), fuer 0x37 a 23024 Byte (LBA 3588 / 3600), fuer 0x2B a 19816 Byte (LBA 2315 / 2325); im Repo sind nur die Overlays der Typen 0x10 (EMZ0/EMOVL10, 53068 B), 0x20 (EMD0G_MOD0, 22266 B), 0x21 (EMOVL21, 19080 B), 0x25 (EMS25, 26324 B), 0x26 (EMS26, 4346 B). Damit sind Zustaende, Uebergaenge, Abstands-/Winkeltore, Angriffs-Trigger, Anim-Kopplung, Start-HP und die Todes-/Kampfende-Logik des Endgegners zu 0 % byte-belegt — sie stehen alle in dem fehlenden Overlay. Byte-belegt ist dagegen praktisch der komplette Rahmen: Spawn-Record-Layout (Sce_em_set = SCD-Opcode 0x44, 22 Byte, Handler 0x8005714C), das Entity-Struct-Layout (584 B, Typ +0x8, Zustandswort +0x4, Distanz +0x1F0, HP +0x156, Yaw +0x76, Pos +0x38/+0x3C/+0x40, Boden-Y +0x1C2 = -1800*Etage), die Per-Frame-Tick-Schleife @0x80026560 mit Vor-Berechnung der Spieler-Distanz und dem Ruhe-Gate +0x10E&0x8000, der Treffer-Aufloeser @0x800470C8 samt der PER-TYP-SCHADENSTABELLE (Typ 0x36 -> 0x800A5EDC, Typ 0x2B -> 0x800A5478, Typ 0x37 -> Default 0x800A412C) und der Spieler-Schadens-API 0x800401D4. Ein starkes Verhaltens-Detail des Endgegners ist trotzdem belegt: Typ 0x36 wird mit +0x10E = 0x8000 gespawnt, und genau dieses Bit laesst die Tick-Schleife @0x80026598 die KI ueberspringen — der Boss startet also SCHLAFEND und wird per Skript scharfgeschaltet; sein em_flag ist 0xFF (nie persistiert, spawnt bei jedem Betreten neu), waehrend 0x2B in ROOM7020 mit em_flag 0x28 ein normaler, dauerhaft toetbarer Gegner ist.

## Befunde

### Endgegner-Typ (selbst ermittelt)  [byte-belegt]
STAGE7 spawnt genau einen Boss-Cluster: ROOM7040.RDT enthaelt 1x Sce_em_set Typ 0x36 und 4x Typ 0x37; ROOMG040.RDT (Szenario-B-Zwilling) identisch. Beide Typen kommen in keinem der 495 RDTs sonst vor.

**Beleg:** info/re2leon/PL0/RDT/ROOM7040.RDT, SCD_MAIN (Sektion 17 @0x1120), Roh-Records: @0x11ac 44 00 00 36 00 80 00 27 00 ff 00 83 00 00 00 83 00 00 00 00 00 00 | @0x11c2 44 00 01 37 01 00 00 27 00 ff ... | @0x11d8 44 00 02 37 ... | @0x11ee 44 00 03 37 ... | @0x1204 44 00 04 37 ...  (pc[0]=0x44 Sce_em_set, pc[2]=Slot, pc[3]=Typ). Zensus ueber alle 495 RDTs (SCD-Sektionen 16/17): Typ 0x36 n=2 (7040,G040), Typ 0x37 n=8 (7040,G040).

**Port-Stand:** nicht geprueft — re15_port hat RE2-KI nur fuer 0x10/0x20/0x25/0x26 (enemy_ai_re2_zombie.c, _dog.c, _spider.c, _crow.c)

### Sce_em_set = SCD-Opcode 0x44, 22 Byte  [byte-belegt]
RE2s Sce_em_set ist Opcode 0x44 mit 22 Byte Laenge. Handler LAB_8005714C, eingetragen in der SCD-Dispatch-Tabelle PTR_LAB_800a74c8[0x44] = 0x800A75D8.

**Beleg:** 0x800576DC: lw v1,0x1c(s5) / 0x800576E4: addiu v1,v1,22 / 0x800576E8: sw v1,0x1c(s5)  (PC-Advance = 22). Tabelleneintrag: 0x800a75d8 = 4c 71 05 80 (ghidra_re2_Leon.txt Zeile 330734). Dispatcher FUN_80053F50 @0x80053F84: lw v0,offset PTR_LAB_800a74c8(at).

**Port-Stand:** re15_port/engine/src/scd_vm.c s_opcode_sizes[0x44]=20 — das ist die RE1.5-Laenge (korrekt fuer RE1.5); RE2 hat 22

### Sce_em_set Feld-Layout (RE2)  [byte-belegt]
pc[2]=Slot (Entity-Index = pc[2]+2), pc[3]=TYP -> Entity+0x8, pc[4..5]=u16 Flags -> Entity+0x10E, pc[6]=Etage -> Entity+0x106, pc[7] -> Entity+0x1FA, pc[8] -> Entity+0x1CF, pc[9]=em_flag_id -> Entity+0x1CE, pc[10..11]=X, pc[12..13]=Y, pc[14..15]=Z, pc[16..17]=Yaw -> Entity+0x76, pc[18..19] -> +0x158, pc[20..21] -> +0x15A. RE1.5 hat den Typ bei pc[2], RE2 bei pc[3] (ein Byte mehr).

**Beleg:** 0x80057204 lb v0,2(v0) / 0x8005720C addiu a1,v0,2 / 0x80057238 sb a1,12(s0)  |  0x80057334 lbu v0,3(v1) / 0x8005733C sb v0,8(s0)  |  0x8005734C lhu v0,4(v1) / 0x80057354 sh v0,270(s0)  |  0x80057388 lbu v0,6(v0)/sb v0,262(s0)  |  0x80057274 lbu v0,7(v0)/sb v0,506(s0)  |  0x80057370 lbu v0,8(v0)/sb v0,463(s0)  |  0x80057358 lbu v0,9(v1)/sb v0,462(s0)  |  0x800572C8..0x8005730C lhu 10/12/14 -> sh 68/70/72 + sw 56/60/64  |  0x80057328 lhu v0,16(v1)/sh v0,118(s0)  |  0x800576B4 lhu v0,18(v1)/sh v0,344(s0)  |  0x800576C4 lhu v1,20(v1)/sh v1,346(s0)

**Port-Stand:** nicht geprueft

### Wurzel-Funktion des Hirns — Adresse belegt, Code fehlt  [byte-belegt]
Die KI-Wurzel jedes Gegner-Typs steht als fest einkompilierte Overlay-Adresse in der EXE. Typ 0x36 = 0x801000BC (Bank 0) / 0x8010D0BC (Bank 1). Typ 0x37 = 0x80100178 / 0x8010D178. Typ 0x2B = 0x8010187C / 0x8010E87C. Bank 0 = Overlay geladen @0x80100000, Bank 1 = @0x8010D000.

**Beleg:** EXE-Installer 0x8001B280..0x8001B704 schreibt die Tabelle 0x800CFFDC[bank*384 + typ*4]. Konkret: 0x8001B45C sw v0,0x800D00B4 (=0x800CFFDC+0x36*4, Bank0) mit v0=0x801000BC; 0x8001B46C sw v0,0x800D00B8 (Typ 0x37) mit v0=0x80100178; 0x8001B3CC sw v0,0x800D0088 (Typ 0x2B) mit v0=0x8010187C; 0x8001B694 / 0x8001B6A4 / 0x8001B60C = die Bank-1-Zwillinge. Gegenprobe an vier vorhandenen Dateien: Typ 0x20 -> 0x80100004 == EMD0G_MOD0.BIN Datei-Offset 0x04 (dort steht `e8ff bd27` = addiu sp,sp,-0x18); Typ 0x26 -> 0x8010001C == EMS26.BIN Offset 0x1C (`e8ff bd27`); Typ 0x10 -> 0x801002C0 == EMZ0.BIN (gueltiger Prolog); Typ 0x21 -> 0x8010013C == EMOVL21_S0.BIN (gueltiger Prolog).

**Port-Stand:** nicht geprueft — Tabelle 0x8001b280/0x800cffdc kommt im Port nirgends vor (grep)

### Laufzeit-Dispatch Typ -> Hirn  [byte-belegt]
Pro Frame laeuft die Entity-Liste; das Hirn wird als AIroot[typ] aus 0x800CFE5C[typ] gerufen, das der Loader vorher aus 0x800CFFDC[bank*96+typ] kopiert.

**Beleg:** Tick-Schleife 0x800265DC: lbu v1,8(s0) / sll v1,v1,2 / addu v1,s3,v1 / lw v0,15476(v1)  mit s3=0x800CC1E8 -> 0x800CC1E8+15476 = 0x800CFE5C; 0x800265F4 jalr v0 / addu a0,s0,zero. Kopie: 0x8001B7EC-0x8001B80C: sll v0,a3,1 / addu v0,v0,a3 / sll v0,v0,7 (=bank*384) / addu v0,v0,t0(=0x800CC1E8) / addu v0,v0,v1(=typ*4) / lw v0,15860(v0) (=0x800CFFDC) / sw v0,15476(v1)

**Port-Stand:** nicht geprueft

### Dispatch-Form INNERHALB des Hirns (+0x4)  [byte-belegt]
Jede Gegner-Wurzel dispatcht ueber das Zustands-Byte Entity+0x4 in eine Overlay-lokale Zeigertabelle; das Zustandswort ist ein 32-bit-Wort bei +0x4 mit den Byte-Bahnen +0x4 (Haupt), +0x5 (Sub), +0x6, +0x7. Fuer Typ 0x36 ist diese Tabelle NICHT lesbar (Overlay fehlt).

**Beleg:** EMS25.BIN (Typ 0x25) @0x80100118: lbu v0,4(s0) / sll v0,v0,2 / lui at,0x8010 / addu at,at,v0 / lw v0,25632(at) (= Tabelle @0x80106420) / jalr v0 / addu a0,s0,zero. Gleiche Form in EMZ0.BIN @0x801003AC ff. Beleg fuer die Byte-Bahnen: Treffer-Aufloeser 0x80047264 lw v1,4(s1) / 0x8004726C addiu v0,zero,3074 (=0x0C02 -> +0x4=2, +0x5=0x0C) und 0x80047288/0x80047290 sw v0(=2 bzw. 3),4(s1).

**Port-Stand:** nicht geprueft

### Globales KI-Gate (Freeze)  [byte-belegt]
Jede Gegner-Wurzel prueft zuerst DAT_800CFBDC & 0x20000000 (bzw. DAT_800CFBD8 & 0x10000000 beim Zombie) und springt bei gesetztem Bit ans Ende — der Freeze ist global, wie in RE1.5.

**Beleg:** EMS25.BIN 0x801000D4-E4: lui v0,0x800d / lw v0,-1060(v0) (=0x800CFBDC) / lui v1,0x2000 / and / bne v0,zero,0x80100198. EMOVL21_S0.BIN 0x8010014C identisch. EMZ0.BIN 0x801002E4: lw v0,-1064(v0) (=0x800CFBD8) / lui v1,0x1000.

**Port-Stand:** nicht geprueft

### Endgegner startet SCHLAFEND (+0x10E Bit 0x8000)  [byte-belegt]
Der 0x36-Spawn setzt Entity+0x10E = 0x8000. Die Per-Frame-Schleife ueberspringt die komplette KI, wenn +0x10E & 0x8000 gesetzt ist. Der Boss wird also inaktiv gespawnt und muss vom Raum-Skript scharfgeschaltet werden. Die vier 0x37 kommen mit 0x0001 (Slot 1) bzw. 0x0000 (Slots 2-4).

**Beleg:** ROOM7040.RDT @0x11ac Bytes pc[4..5] = 00 80 -> u16 0x8000, gespeichert nach +270 (0x8005734C/54). Gate: 0x80026590 lhu v0,270(s0) / 0x80026598 andi v0,v0,0x8000 / 0x8002659C bne v0,zero,0x800265FC (= naechste Entity, KEIN jalr auf das Hirn).

**Port-Stand:** nicht geprueft

### Endgegner persistiert nicht (em_flag 0xFF)  [byte-belegt]
0x36 und alle vier 0x37 haben pc[9] = 0xFF. em_set ueberspringt den Kill-Bitmap-Test nur bei 0xFF; d.h. der Boss spawnt bei jedem Betreten neu und wird nie als 'schon tot' markiert. Der ROOM7020-Gegner 0x2B hat dagegen em_flag 0x28 (normaler, dauerhaft toetbarer Gegner).

**Beleg:** em_set 0x800571C0: lbu a1,9(v0) / 0x800571C4 addiu v0,zero,255 / 0x800571C8 beq a1,v0,0x800571E0 (=Test ueberspringen) / sonst 0x800571D0 jal 0x80077360 (Bit-Test auf 0x800D489C) / 0x800571D8 bne v0,zero,0x800576DC (Spawn abbrechen). Record-Bytes: ROOM7040 @0x11ac pc[9]=ff; ROOM7020 @0x1236 pc[9]=28.

**Port-Stand:** nicht geprueft

### Abstand zum Spieler wird VOR dem Hirn berechnet (+0x1F0)  [byte-belegt]
Die Tick-Schleife legt in Entity+0x1F0 (496) die Ganzzahl-Wurzel aus dx^2+dz^2 zum Spieler ab, BEVOR sie die KI-Wurzel ruft. Alle sltiu-Abstandstore der Gegner-Overlays vergleichen gegen dieses Feld.

**Beleg:** 0x800265A4 lw v0,56(s0) / 0x800265A8 lw v1,56(s2) (s2 = 0x800CC1E8+14864 = 0x800CFBF8 = Spieler) / subu / mult / 0x800265B8 lw v0,64(s0) / 0x800265BC lw v1,64(s2) / subu / mult / 0x800265D4 jal 0x8008D2F4 / 0x800265E0 sw v0,496(s0). 0x8008D2F4 = GTE-LZCS/LZCR-Ganzzahlwurzel (0x8008D2F4: mtc2 a0,cop2r30 ; 0x8008D300: mfc2 v0,cop2r31).

**Port-Stand:** nicht geprueft

### Winkel-Tor (Kegel-Test) der gemeinsamen Bibliothek  [byte-belegt]
FUN_80015910(a0,a1) liefert 1, wenn a1 innerhalb +-90 Grad vor a0 liegt: ((yaw(a1)-yaw(a0)+1024) & 0xFFF) < 2048. Wird von allen fuenf vorhandenen Gegner-Overlays gerufen.

**Beleg:** 0x80015910: lh v0,118(a1) / lh v1,118(a0) / subu v0,v0,v1 / addiu v0,v0,1024 / andi v0,v0,0xfff / jr ra / slti v0,v0,2048.

**Port-Stand:** nicht geprueft

### Gemeinsame KI-Bibliothek (EXE), die auch das fehlende Boss-Overlay benutzt  [byte-belegt]
Schnittmenge der jal-Ziele aller fuenf vorhandenen Gegner-Overlays in die EXE (14 Routinen): 0x80015FE8 (RNG, 483 Aufrufe), 0x8002959C (Anim-Frame-Advance, liest +332/+333), 0x8001BF10 (Sound), 0x8005BD6C, 0x800152C8/0x80015350 (Fortbewegung entlang Yaw+0x76), 0x80015558/0x80015614 (Drehen/Steuern auf (x,z) via atan2 0x800154AC), 0x80015910 (Winkel-Tor), 0x80016480, 0x8003947C/0x80039514 (Effekt-/Partikel-Spawn, Pools 0x800EAAD8/0x800EAC28), 0x800401D4 (Spieler-Schaden), 0x8004AAB8/0x8004AA50 (Boden/Nav).

**Beleg:** jal-Scan ueber EMZ0.BIN, EMOVL21_S0.BIN, EMD0G_MOD0.BIN, EMS25.BIN, EMS26.BIN (Ziele < 0x80100000). RNG-Kern 0x80015FE8: lui a0,0x800d / addiu a0,a0,-7400 (=0x800CE318) / lw v0,0(a0) / srl v1,v0,7 / andi v1,v1,0xff / addu v0,v1,v0 / andi v0,v0,0xff / sll v1,v1,8 / or / andi v1,v0,0xffff / sw v1,0(a0).

**Port-Stand:** 0x80015FE8 / 0x8002959C / 0x8001BF10 / 0x800401D4 sind im Port bereits als RE2-Referenzen zitiert (enemy_ai_common.c:1070, enemy_ai_re2_dog.c:414)

### Schaden AN den Spieler (Angriffs-API)  [byte-belegt]
FUN_800401D4(dmg, mode) zieht dem Spieler Schaden ab. Spieler-HP = u16 bei Spieler+0x156 (Spielerbasis 0x800CFBF8, also 0x800CFD4E). Schwierigkeits-Skalierung: wenn (DAT_800CFB74 & 0x40) ODER (HP < 121) und dmg < 41, dann dmg += dmg>>1. Wenn DAT_800D482A == 3: dmg<30 -> dmg*5, sonst dmg<60 -> dmg*2. Jedes Gegner-Overlay ruft das genau EINMAL — dort steht die Schadenszahl des jeweiligen Angriffs.

**Beleg:** 0x800401D4-0x80040254: lw v0,0x800CFB74 / andi 0x40 ; lh v0,0x800CFD4E / slti v0,v0,121 ; slti v0,a0,41 / sra v0,a0,1 / addu a0,a0,v0 ; lh v1,0x800D482A / li v0,3 ; slti v0,a0,30 / sll v0,a0,2 / addu a0,a0,v0 ; slti v0,a0,60 / sll a0,a0,1 ; lhu v0,342(a2) / subu a0,v0,a0 / sh a0,342(a2). Aufruf-Beispiele: EMD0G_MOD0.BIN 0x80104EB8 addiu a0,zero,20 / 0x80104EBC jal 0x800401D4 (Hundebiss = 20). EMZ0.BIN 0x801028F4 lbu a0,1(s0) / 0x801028F8 jal 0x800401D4 (Zombie: Schaden aus Entity+1).

**Port-Stand:** im Port als re2z_player_damage / FUN_800401d4-Port vorhanden (enemy_ai_common.c:1440,1472; enemy_ai_re2_dog.c:414)

### Schaden AM Gegner + Trefferzonen + Waffentabelle  [byte-belegt]
FUN_800470C8(a0,a1,a2,a3) ist der gemeinsame Treffer-Aufloeser. a3 packt: Bits 0..15 = Waffen-ID, Bit 0x10000 = Schaden anwenden, Bit 0x20000 = Rueckstoss, Bits 28..31 = Trefferzone (0..2). Schaden = ((*(DMG[typ] + (waffe-1)*20)) >> (zone*10)) & 0x3FF. Danach HP(+0x156) -= dmg; HP<0 -> Zustand +0x4 = 3 (Tod), sonst +0x4 = 2 (Treffer-Reaktion); +0x1D2 = 1.

**Beleg:** 0x80047114 srl s6,s5,28 (Zone) ; 0x80047208 lui v0,0x1 / and v0,s5,v0 (Bit 0x10000) ; 0x80047218 lbu v0,8(s1) / sll v0,v0,2 / 0x8004722C lw a1,27272(at) -> Zeigertabelle 0x800A6A88 ; 0x80047230-3C sll v0,v1,2 / addu v0,v0,v1 / sll v0,v0,2 / addiu v0,v0,-20 (= (waffe-1)*20) ; 0x8004724C lw v1,0(a1) / 0x80047244-50 sll v0,s6,2 / addu v0,v0,s6 / sll v0,v0,1 (= zone*10) / srlv v1,v1,v0 / andi v1,v1,0x3ff ; 0x80047258 lhu v0,342(s1) / subu / 0x80047268 sh v0,342(s1) ; 0x8004727C lh v1,342(s1) / bgez -> 0x80047288 sw v0(=2),4(s1) sonst 0x80047290 sw v0(=3),4(s1) ; 0x80047298 sb v0(=1),466(s1). Vorfilter am Funktionsanfang 0x80047138-0x80047164: +0x1D3!=0, HP<0, +0x10E & 0xC000 -> ueberspringen.

**Port-Stand:** nicht geprueft — 0x800470c8 / 0x800a6a88 kommen im Port nirgends vor

### Schadenstabelle des Endgegners (Typ 0x36) — VOLLSTAENDIG in der EXE  [byte-belegt]
Typ 0x36 hat eine EIGENE Waffen-Schadenstabelle @0x800A5EDC (Zeiger in 0x800A6A88[0x36]). Zone0/1/2 je Waffe (Waffen-ID 1..19): 1:[15,0,0] 2:[17,16,15] 3:[17,16,15] 4:[17,16,15] 5:[75,73,70] 6:[100,100,100] 7:[50,48,45] 8:[70,68,65] 9:[80,80,80] 10:[70,70,5] 11:[70,70,10] 12:[20,20,20] 13:[17,16,15] 14:[60,60,60] 15:[10,10,10] 16:[5,5,5] 17:[200,200,200] 18:[17,17,17] 19:[17,16,15]. Typ 0x37 benutzt die Default-Tabelle 0x800A412C, Typ 0x2B die eigene @0x800A5478 (u.a. Waffe 17 = 500).

**Beleg:** Zeigertabelle 0x800A6A88 (96 Eintraege, Index = Typ): [0x36]=0x800A5EDC, [0x37]=0x800A412C, [0x2B]=0x800A5478, [0x21]=0x800A45A0, [0x10]=0x800A412C. Werte gelesen aus info/re2leon/PSX.EXE, Wort 0 jedes 20-Byte-Eintrags, 3x10 Bit. Stride-Beleg: 0x800A42A8-0x800A412C = 0x17C = 19*20.

**Port-Stand:** nicht geprueft

### HP und Trefferzonen-Geometrie  [byte-belegt]
HP liegt bei Entity+0x156 (u16) — gleiches Feld fuer Spieler UND Gegner. Sce_em_set setzt KEIN HP; die Start-HP kommt aus dem Init-Zustand des jeweiligen Overlays und ist fuer Typ 0x36 damit NICHT belegbar. Die Kollisions-/Treffer-Masse setzt em_set generisch: +0x90=+0x92=+0x9A=+0x9C=450, +0x98=-1530, +0x9E=+1530, +0x94=+0x96=0; der Aufloeser liest +0x98 (lh a0,152(s0)).

**Beleg:** em_set 0x8005742C-0x80057450: addiu v0,zero,-1530 / addiu v1,zero,450 / sh v0,152(s0) / addiu v0,zero,1530 / sh v0,158(s0) / sh v1,154(s0) / sh v1,156(s0) / sh v1,144(s0) / sh v1,146(s0). Aufloeser 0x80047170: lh a0,152(s0). HP-Feld: 0x80047258 lhu v0,342(s1) / 0x80047268 sh v0,342(s1) und 0x80040248 lhu v0,342(a2) (Spieler).

**Port-Stand:** nicht geprueft

### Boden-Y aus dem Etagen-Byte  [byte-belegt]
pc[6] (Etage) erzeugt Entity+0x1C2 = -1800 * Etage. Fuer den 0x36-Spawn ist pc[6]=0, also Boden-Y = 0 (passt zu Y=0 im Record).

**Beleg:** em_set 0x800573A0-0x800573C0: lbu v1,6(v0) / sll v0,v1,3 / subu v0,v0,v1 (=7v) / sll v0,v0,5 (=224v) / addu v0,v0,v1 (=225v) / sll v0,v0,3 (=1800v) / subu v0,zero,v0 / sh v0,450(s0).

**Port-Stand:** nicht geprueft

### Gegner vs. NPC — warum 0x4F/0x55 KEINE Bosse sind  [byte-belegt]
em_set setzt Entity+0x9 = 4 fuer Typ < 0x40 und = 0x80 fuer Typ >= 0x40. Zusaetzlich bildet die Tabelle 0x800A7704 (Paare Typ,Modellgruppe, terminiert mit 0xFF) die Typen 0x41..0x5B fast alle auf die Sammelmodelle 0x50/0x51 ab: 0x4F->0x45, 0x55->0x51, 0x41->0x51. Typ 0x36 -> 0x36 und 0x37 -> 0x37 (eigene Modelle).

**Beleg:** em_set 0x80057410-0x80057428: sltiu v1,v1,0x40 / beq -> addiu v0,zero,128 / sonst addiu v0,zero,4 / sb v0,9(s0). Tabelle 0x800A7704 Rohbytes: 10 10 11 10 12 10 13 10 15 10 16 10 17 10 18 10 1e 10 1f 10 20 20 21 21 ... 36 36 37 37 39 39 ... 41 51 42 50 43 51 44 50 45 45 46 50 47 51 48 50 49 51 4a 10 4b 51 4f 45 50 50 51 51 54 50 55 51 58 50 59 51 5a 50 5b 51 ff 00. Lookup-Code FUN_80052AF4 @0x80052AF4.

**Port-Stand:** nicht geprueft

### Zwei Gegner-Overlay-Baenke; Zombies teilen sich eine  [byte-belegt]
Es gibt genau zwei residente Gegner-Overlay-Slots (Bank 0 @0x80100000, Bank 1 @0x8010D000). Typen 0x10..0x1F werden vor der Slot-Suche auf 0x10 zusammengefasst (alle Zombie-Varianten teilen ein Overlay). ROOM7040 braucht zwei verschiedene Typen (0x36 und 0x37) — also je einen Slot.

**Beleg:** FUN_8001B710 0x8001B738-48: addiu v0,t1,-16 / sltiu v0,v0,0x10 / beq / addiu t1,zero,16. Slot-Suche 0x8001B75C-0x8001B7B4 ueber zwei Halbworte (0x800D4134/0x800D4138). Bank-Stride 384 = 96 Typen * 4 (0x8001B7EC-F4: sll v0,a3,1 / addu v0,v0,a3 / sll v0,v0,7).

**Port-Stand:** nicht geprueft

### CD-Datei-Tabelle: 4 Dateien pro Gegner-Typ  [byte-belegt]
Datei-Index = (typ-0x10)*4 + k. k=1 = Code-Overlay fuer Bank 0, k=0 = Code-Overlay fuer Bank 1, k=2 = EMD-Modell, k=3 = TIM-Textur. Tabelle @0x8009ADF4, 8 Byte pro Eintrag (LBA, Groesse).

**Beleg:** Overlay-Pfad: 0x8001B870-80: addiu v1,t1,-16 / sll v1,v1,2 / addiu v1,v1,1 / subu v1,v1,a3 / sll a0,v1,3 ; 0x8001B894 lw v0,-21000(at) (=0x8009ADF8 = +4 = Groesse) ; 0x8001B8B4 lw a0,-21004(at) (=0x8009ADF4 = LBA). Modell/Textur-Pfad: FUN_8001AAA8 0x8001AB48-54: sll s0,s0,2 / ori a0,s0,0x2 / jal 0x8001B934 und 0x8001AB7C ori a0,s0,0x3.

**Port-Stand:** nicht geprueft

### Welche Gegner-Overlays im Repo LIEGEN (per Groesse identifiziert)  [byte-belegt]
Aus der CD-Tabelle @0x8009ADF4 identifiziert: EMZ0.BIN == EMOVL10_S0.BIN (53068 B) = Typ 0x10; EMOVL21_S0/S1.BIN (19080 B) = Typ 0x21; EMD0G_MOD0.BIN (22266 B) = Typ 0x20 (Hund); EMS25.BIN (26324 B) = Typ 0x25; EMS26.BIN (4346 B) = Typ 0x26. EMZ0.BIN und EMZ0_d1.BIN sind byte-identisch.

**Beleg:** CD-Tabelle: idx64/65 (Typ 0x20) size=22266 == Dateigroesse EMD0G_MOD0.BIN; idx68/69 (0x21) 19080 == EMOVL21_S0.BIN; idx84/85 (0x25) 26324 == EMS25.BIN; idx88/89 (0x26) 4346 == EMS26.BIN; idx0/1 (0x10) 53068 == EMZ0.BIN. Gegenprobe ueber die Wurzel-Adressen (siehe Befund 'Wurzel-Funktion'): 0x80100004 == EMD0G_MOD0.BIN+0x04 mit MIPS-Prolog, 0x8010001C == EMS26.BIN+0x1C mit MIPS-Prolog.

**Port-Stand:** nicht geprueft

### EMD0G_MOD0.BIN ist CODE, kein Modell  [byte-belegt]
Die vom Auftrag geforderte Pruefung: EMD0G_MOD0.BIN enthaelt MIPS-Code (Hund, Typ 0x20), kein Modell. Wort 0 = 6 (Header/Zaehler), ab Offset 4 folgt ein regulaerer MIPS-Prolog.

**Beleg:** xxd EMD0G_MOD0.BIN: 00000000: 0600 0000 e8ff bd27 1000 b0af 2180 8000 -> Offset 0x04 = 27 bd ff e8 = addiu sp,sp,-0x18; 0x08 = af b0 00 10 = sw s0,0x10(sp); 0x0C = 00 80 80 21 = move s0,a0. Zusaetzlich ruft die Datei 0x800401D4 (Spieler-Schaden) @0x80104EBC mit a0=20 und 0x80018FB0 (Kill-Flag) @0x80104194 / 0x801043B8.

**Port-Stand:** nicht geprueft

### Tod / Kampfende: Kill-Bitmap  [byte-belegt]
FUN_80018FB0(entity) markiert den Gegner als endgueltig tot: setzt Bit (Entity+0x1CE) in der Bitmap @0x800D489C (Stage>=3 oder DAT_800CFBD8 & 0x2000000), sonst @0x800D487C. Genau diese Bitmap prueft em_set beim Spawn. Die Funktion wird AUSSCHLIESSLICH aus den Gegner-Overlays gerufen (0 EXE-Aufrufer) — der Todes-Pfad des Endgegners liegt also komplett in der fehlenden Datei. Fuer 0x36/0x37 ist er ohnehin wirkungslos (em_flag 0xFF).

**Beleg:** 0x80018FB0-0x8001900C: lui a2,0x800d / addiu a2,a2,18460 (=0x800D481C) / lh v0,0(a2) / slti v0,v0,3 / bne -> lbu a1,462(a0) / addiu a0,a2,128 (=0x800D489C) ; sonst lw v0,0x800CFBD8 / lui v1,0x200 / and / bne -> gleiche Bitmap ; sonst lbu a1,462(a0) / addiu a0,a2,96 (=0x800D487C) ; jal 0x8007730C. jal-Scan: 0 Treffer in der EXE, 17 Treffer in 4 der 5 vorhandenen Overlays (EMZ0 10x, EMOVL21 4x, EMD0G 2x, EMS25 1x).

**Port-Stand:** nicht geprueft

### Entity-Struct-Groesse und Pool  [byte-belegt]
Eine Gegner-/NPC-Entity ist 584 Byte (0x248) gross und wird linear aus dem Heap-Zeiger 0x800D2144 alloziert; der Entity-Zeiger landet in 0x800D3C2C + slot*4.

**Beleg:** em_set 0x80057228-0x80057248: lw v1,8516(s4) (s4=0x800CC1E8 -> 0x800D2144) / addu s0,v1,zero / sw v1,15404(v0) (-> 0x800D3C2C+slot*4) / sb a1,12(s0) / addiu v0,v0,584 / sw v0,8516(s4).

**Port-Stand:** nicht geprueft

### RE2-SCD-Opcode-Laengen (fuer den RDT-Walker)  [byte-belegt]
Vollstaendige RE2-Laengentabelle aus den 143 Handlern der Dispatch-Tabelle 0x800A74C8 abgeleitet (jeder Handler addiert seine Laenge auf ctx+0x1C). Abweichungen zu RE1.5 u.a.: 0x13=2 (RE1.5 4), 0x2D=38 (RE1.5 dynamisch/34), 0x38=3 (RE1.5 12), 0x44=22 (RE1.5 20), 0x4D=22 (RE1.5 10), 0x4E=22 (RE1.5 5). Nicht ableitbar (Kontrollfluss-Opcodes ohne festes Advance): 0x01,0x03,0x07,0x10,0x17,0x18,0x19,0x1A,0x31,0x37,0x49,0x6B,0x7C,0x7E,0x7F.

**Beleg:** Automatischer Scan aller 143 Handler ab 0x800A74C8 (Tabelle in ghidra_re2_Leon.txt Zeile 330661 als PTR_LAB_800a74c8 bestaetigt): gesucht wurde jeweils das Paar `addiu rX,rY,N` + `sw rX,0x1c(rZ)`. Verifiziert an 0x44 (0x800576E4 addiu v1,v1,22) und 0x2C (LAB_80054AF4).

**Port-Stand:** scd_vm.c enthaelt nur die RE1.5-Tabelle (0x00..0x5E), mit einem Kommentar-Vergleich zu RE2 fuer 0x52..0x5E

### STAGE7.BIN ist KEIN Code-Overlay des Bosses  [byte-belegt]
info/re2leon/COMMON/BIN/STAGE1..7.BIN (420..9984 B) sind winzige Stage-Init-Stubs, keine Gegner-/Raum-Logik. STAGE7.BIN installiert nur drei Funktionszeiger (0x8011A054 -> 0x800CFAE8, 0x8011A0F4 -> 0x800CE540, 0x8011A144 -> 0x800CE544) und ruft 0x80031FE4.

**Beleg:** xxd STAGE7.BIN: Offset 0x00 = 48 00 00 00 (Codelaenge 0x48), 0x04 = e8ff bd27 (addiu sp,sp,-0x18), 0x08 = 1280 023c / 54a0 4224 (lui v0,0x8012 ; addiu v0,v0,-0x5fac = 0x8011A054), 0x10 = 0d80 013c / e8fa 22ac (sw v0,0xfae8(at) = 0x800CFAE8), ... 0x3c = f9c7 000c (jal 0x80031FE4). Der EXE-Loader liest 0x800CE544 in FUN_800129B0 @0x80012A48.

**Port-Stand:** nicht geprueft

### Der zweite STAGE7-Kandidat: Typ 0x2B in ROOM7020  [byte-belegt]
ROOM7020 spawnt genau einen Gegner Typ 0x2B, an (-8000, 9000, -25000) — Y=9000 ueber dem Boden (Etage pc[6]=0) —, mit Flags 0x0000 (also SOFORT aktiv, kein 0x8000-Schlaf), pc[7]=0x1A und em_flag 0x28 (persistiert). Typ 0x2B kommt game-weit nur hier vor; Wurzel 0x8010187C.

**Beleg:** info/re2leon/PL0/RDT/ROOM7020.RDT SCD_MAIN @0x1236: 44 00 00 2b 00 00 00 1a 00 28 c0 e0 28 23 58 9e 00 00 00 00 00 00 (pc[10..11]=0xE0C0=-8000, pc[12..13]=0x2328=9000, pc[14..15]=0x9E58=-25000). Wurzel-Eintrag 0x8001B3CC sw v0(=0x8010187C),0x800D0088.

**Port-Stand:** nicht geprueft

### STAGE7-Belegung insgesamt (Roh-Scan, kein Walker-Desync)  [byte-belegt]
ROOM7000: 5x Typ 0x17 (Zombie-Variante, Flags 0x0006, Etage 0x00). ROOM7010: kein Gegner. ROOM7020: 1x 0x2B. ROOM7030: 1x 0x55 + 3x 0x4F (beides NPCs, Typ>=0x40). ROOM7040: 1x 0x36 + 4x 0x37.

**Beleg:** Roh-Scan (jedes Byte 0x44 in den SCD-Sektionen 16/17, danach Plausibilitaet pc[1]==0, Slot<0x18, 0x10<=Typ<=0x5B): ROOM7000 @0x1680/@0x1696/@0x16ac/@0x16c2/@0x16d8 je `4400xx17 0600 002e 00xx ...`; ROOM7030 @0x1eba `44000055 0040 0000 00ff ...`, @0x1f00/@0x1f34/@0x1f6e `4400014f ...`; ROOM7040 wie oben.

**Port-Stand:** nicht geprueft

### Zustaende / Uebergaenge / Angriffs-Trigger / Anim-Kopplung des Endgegners  [byte-belegt]
NICHT ermittelbar. Alle Zustandsfunktionen, deren Zeigertabelle, die sltiu-Abstandsschwellen, die Anim-Frame-Kopplung, der Schadenswert der Boss-Angriffe, die Start-HP und der Phasenwechsel liegen in dem 23476 Byte grossen Code-Overlay von Typ 0x36, das im Repo nicht existiert. Belegbar ist nur die Einsprungadresse 0x801000BC/0x8010D0BC und die Aufrufkonvention (a0 = Entity-Zeiger).

**Beleg:** Negativ-Beleg: ls info/re2leon/COMMON/BIN/ listet nur CONFIG, DIEDEMO, EMD0G_MOD0, EMOVL10_S0/S1, EMOVL21_S0/S1, EMS25, EMS26, EMZ0, EMZ0_d1, ENDING, MEM_CARD, OPENING, RESULT, SELECT, STAGE1..7, TITLE. Keine Datei mit 23476 oder 23024 Byte. CD-Tabelle 0x8009ADF4 idx152/153 (Typ 0x36) = LBA 3444/3456, size 23476; idx156/157 (Typ 0x37) = LBA 3588/3600, size 23024.

**Port-Stand:** kein Boss im Port vorhanden

## Fehlende Daten
- Code-Overlay Gegner-Typ 0x36 (ENDGEGNER), Bank-0-Variante: CD-LBA 3444, 23476 Byte. Enthaelt die KI-Wurzel @0x801000BC, deren Zustands-Zeigertabelle (Dispatch auf Entity+0x4), alle Zustandsfunktionen, Abstands-/Winkeltore, Angriffs-Trigger + Schadenszahlen (Aufruf 0x800401D4), Start-HP (schreibt Entity+0x156), Phasenwechsel und die Todeskette (Aufruf 0x80018FB0). Dateiname im vorhandenen Namensschema vermutlich EMOVL36_S0.BIN — Schema aber NICHT durchgaengig (Hund heisst EMD0G_MOD0.BIN, 0x25/0x26 heissen EMS25/EMS26.BIN), daher ist die LBA+Groesse die verlaessliche Kennung.
- Code-Overlay Gegner-Typ 0x36, Bank-1-Variante: CD-LBA 3456, 23476 Byte (identischer Code, geladen @0x8010D000, Wurzel 0x8010D0BC).
- Code-Overlay Gegner-Typ 0x37 (die 4 Begleiter des Endgegners in ROOM7040): CD-LBA 3588 und 3600, je 23024 Byte. Wurzel 0x80100178 / 0x8010D178.
- Code-Overlay Gegner-Typ 0x2B (einmaliger Gegner in ROOM7020): CD-LBA 2315 und 2325, je 19816 Byte. Wurzel 0x8010187C / 0x8010E87C.
- EMD-Modell + TIM des Endgegners: Typ 0x36 EMD = CD-LBA 3468, 132640 Byte; TIM = LBA 3533, 112340 Byte. Typ 0x37 EMD = LBA 3612, 33312 Byte; TIM = LBA 3629, 59376 Byte. Ohne die EMD ist die Anim-Frame-Kopplung (welcher Clip auf welchen Zustand) nicht pruefbar.
- Code-Overlays der uebrigen einmaligen Gegner der Boss-Kette (fuer den Vergleich Phase-zu-Phase): Typ 0x30 (LBA 2709/2724, 30380 B), 0x31 (LBA 2863/2879, 32088 B), 0x33 (LBA 3039/3052, 26552 B), 0x34 (LBA 3215/3238, 45780 B).
- RE2_Quellcode_Overlays/STAGE7/ ist praktisch leer (nur STAGE6 hat 4 Dateien) — es gibt kein Decompilat des Boss-Overlays, das man gegenlesen koennte.
- Ein RE2-Savestate/RAM-Dump von ROOM7040 (z.B. DuckStation), um die Wurzel 0x801000BC bzw. 0x8010D0BC und die dort liegende Zustandstabelle DYNAMISCH aus dem RAM zu lesen — das waere der Ersatzweg, falls die CD-Dateien nicht beschafft werden.

## Gegenpruefung — GEKIPPT

### [19]/[20]/[28] k=2/k=3 vertauscht — was als "EMD des Endgegners" gemeldet wird, ist die TIM
Der Ermittler behauptet k=2 = EMD-Modell, k=3 = TIM-Textur. Es ist umgekehrt. Damit sind auch die gemeldeten FEHLENDEN DATEN falsch etikettiert: Typ 0x36 k=2 = LBA 3468 / 132640 B ist die TEXTUR-Klasse, k=3 = LBA 3533 / 112340 B ist das Modell. Analog Typ 0x37: k=2 3612/33312 = Textur, k=3 3629/59376 = Modell.

**Gegenbeleg:** Ground truth im Repo: CD-Tabelle 0x8009ADF4, Typ 0x20: idx66 (k=2) size=66592, idx67 (k=3) size=121144. Dateien: EM_TYPE20.TIM = 66592 B, EM_TYPE20.EMD = 121144 B. TIM-Magic geprueft: xxd EM_TYPE20.TIM -> `10 00 00 00 09 00 00 00` (TIM id 0x10, flag 9 = 8bpp CLUT). EMD: `18 d9 01 00` = Dir-Offset 0x1d918. Code stuetzt das: 0x8001AB4C `ori a0,s0,0x2` -> laden -> 0x8001AB6C `jal 0x80076a40` (VRAM-Upload) ; 0x8001AB7C `ori a0,s0,0x3` -> laden -> 0x8001AB88 `lw v1,0(s2)` / 0x8001AB94 `lw v1,0(s0)` / 0x8001ABA0 `sw v1,444(s1)` (Verzeichnis-Parse = Modell). Zusatzindiz: k=2 ist ueber viele Typen identisch gross (0x10/0x20/0x25 = 66592; 0x21/0x26/0x37 = 33312; 0x2b/0x30/0x31/0x33/0x34 = 99872), k=3 dagegen je Typ unterschiedlich.

### [12] Schwierigkeits-Skalierung: Bedingung INVERTIERT
Behauptet: "wenn (DAT_800CFB74 & 0x40) ODER (HP < 121) und dmg < 41, dann dmg += dmg>>1". Der Boost greift bei HP >= 121, nicht bei HP < 121.

**Gegenbeleg:** 0x800401FC `slti v0,v0,121` (v0 = HP<121) ; 0x80040200 `bne v0,zero,0x80040214` -> bei HP<121 wird an 0x80040214 gesprungen, also HINTER `0x8004020C sra v0,a0,1` / `0x80040210 addu a0,a0,v0`. Nur der Fall HP>=121 (bne nicht genommen) faellt auf 0x80040208 `beq v0,zero,0x80040214` (v0 = dmg<41 aus dem Delay-Slot 0x80040204) und damit in den Boost.

### [6] Zombie-"Freeze-Gate" ist die falsche Instruktion und die falsche Polaritaet
Behauptet: "bzw. DAT_800CFBD8 & 0x10000000 beim Zombie ... springt bei gesetztem Bit ans Ende". Der zitierte Test ist ein BEQ (springt wenn das Bit NICHT gesetzt ist) und sein Ziel liegt mitten in der Funktion, nicht am Ende. Der echte Freeze-Gate des Zombies ist derselbe wie bei allen anderen: 0x800CFBDC & 0x20000000.

**Gegenbeleg:** EMZ0.BIN Wurzel 0x801002C0: 0x801002E4 `lw v0,-1064(v0)` (=0x800CFBD8) / 0x801002E8 `lui v1,0x1000` / 0x801002EC `and` / 0x801002F0 **`beq v0,zero,0x80100344`** — und 0x80100344 ist Code (`addu a0,s0,zero` / `jal 0x8004a808`), nicht das Epilog. Der echte Gate steht 0x18 Instruktionen spaeter: 0x8010035C `lui v0,0x800d` / 0x80100360 `lw v0,-1060(v0)` (=0x800CFBDC) / 0x80100364 `lui v1,0x2000` / 0x8010036C `bne v0,zero,0x80100640`. Ich habe alle 0x800CFBDC/0x800CFBD8-Lesungen in den 5 Overlays gescannt: EMZ0 hat 0x800CFBDC genau einmal, @0x8010035C.

### [6] "springt ans Ende" gilt nicht durchgaengig
Bei EMS25 fuehrt der Freeze-Zweig nicht ins Epilog, sondern in einen weiteren Codeblock, der noch laeuft.

**Gegenbeleg:** EMS25.BIN 0x801000E4 `bne v0,zero,0x80100198`; 0x80100198 = `lbu v0,575(s0)` / `beq v0,zero,0x801001D8` / `lw a0,56(s0)` / `jal 0x800527B4` — kein `jr ra`. Zum Vergleich EMOVL21_S0 0x80100158 `bne -> 0x80100258` und dort 0x80100258 `lw ra,52(sp)` ... 0x80100264 `jr ra` (echtes Epilog), ebenso EMD0G 0x80100020 `bne -> 0x801000E0` = `lw ra,20(sp)` ... `jr ra`.

### [23] Entity-Pool-Adressen falsch (uebernommene Werkzeug-Annotation)
"Heap-Zeiger 0x800D2144" und "Entity-Zeiger landet in 0x800D3C2C + slot*4" sind beide falsch. Der Ermittler hat die Annotationsspalte des Disassemblers abgeschrieben, die bei registerrelativen Zugriffen stumpf (lui_upper<<16)+offset rechnet und das vorangehende addiu ignoriert. Die Struktgroesse 584 stimmt.

**Gegenbeleg:** s4 wird 0x80057188 `addiu s4,s4,-15896` -> 0x800CC1E8. Dann 0x80057228 `lw v1,8516(s4)` = 0x800CC1E8+0x2144 = **0x800CE32C** (nicht 0x800D2144) und 0x80057234 `sw v1,15404(v0)` mit v0=s4+slot*4 -> Basis 0x800CC1E8+0x3C2C = **0x800CFE14** (nicht 0x800D3C2C). Gegenprobe aus der Tick-Schleife: 0x80026544 `addiu s1,s3,15408` mit s3=0x800CC1E8 (gesetzt 0x800257CC/0x800257D0) = 0x800CFE18 = Basis+4, und die AIroot-Tabelle liegt bei s3+15476 = 0x800CFE5C — exakt 18 Slots hinter 0x800CFE14. Mit 0x800D3C2C waere die Nachbarschaft unmoeglich.

### [18] Slot-Marker-Adressen falsch (gleiche Werkzeug-Artefakt-Klasse)
"Slot-Suche ... ueber zwei Halbworte (0x800D4134/0x800D4138)" — t0 ist 0x800CC1E8, nicht 0x800D0000.

**Gegenbeleg:** 0x8001B72C `addiu t0,t0,-15896` -> t0=0x800CC1E8. 0x8001B760/0x8001B7AC `lhu v0,16692(v1)` mit v1=t0 bzw. t0+4 -> **0x800D031C / 0x800D0320**. Ebenso 0x8001B78C `lhu v0,16694(t0)` = 0x800D031E und 0x8001B77C `lhu v0,16698(t0)` = 0x800D0322. Die Typ-0x10-Kollabierung (0x8001B738 `addiu v0,t1,-16` / `sltiu v0,v0,0x10` / `addiu t1,zero,16`) und der Bank-Stride 384 (0x8001B7EC-F4) sind dagegen korrekt.

### [26]/[27] Zensus unvollstaendig — Slot-0xFF-Records wurden vom eigenen Filter geloescht
Der Filter "Slot<0x18" wirft gueltige Sce_em_set-Records weg. ROOM7020 spawnt NICHT nur den 0x2B, ROOM7000 nicht nur 5x 0x17. Slot 0xFF ist kein Muell, sondern ein Sonderpfad.

**Gegenbeleg:** ROOM7020.RDT @0x1220 (innerhalb SCD_MAIN 0x1128..letzter Sub 0x1B64): `44 00 ff 41 00 02 06 00 00 ff a0 92 c8 ce 14 ba 40 06 00 00 00 00` = Typ 0x41, Slot 0xFF, Flags 0x0200, Etage 6, em_flag 0xFF, (X,Y,Z)=(-28000,-12600,-17900), Yaw 1600 — 22 Byte, exakt anschliessend der 0x2B-Record @0x1236. ROOM7000.RDT @0x14A4: `44 00 ff 4f 00 40 00 00 00 ff 00 00 ...` = Typ 0x4F, Slot 0xFF, Flags 0x4000. Mechanismus: em_set 0x80057204 `lb v0,2(v0)` ist ein SIGNED lb -> 0xFF = -1, 0x8005720C `addiu a1,v0,2` -> Entity-Index 1, und 0x80057210 `beq a1,a2(=1),0x80057228` ueberspringt das Inkrement des Aktiv-Zaehlers 0x8005721C/0x80057224.

### [11] Schnittmenge der EXE-Aufrufe falsch besetzt
Er nennt "14 Routinen" und listet 15 Adressen. Drei davon sind NICHT in allen fuenf Overlays, zwei echte Schnittmengen-Mitglieder fehlen.

**Gegenbeleg:** Eigener jal-Scan (op==3) ueber EMZ0.BIN, EMOVL21_S0.BIN, EMD0G_MOD0.BIN, EMS25.BIN, EMS26.BIN, Ziele <0x80100000, Mengenschnitt = genau 14: 0x800152C8, 0x80015558, 0x80015614, 0x80015910, 0x80015FE8, 0x80016480, 0x8001BF10, 0x8002959C, **0x80035530**, **0x8003567C**, 0x8003947C, 0x80039514, 0x800401D4, 0x8005BD6C. NICHT drin: 0x80015350, 0x8004AA50, 0x8004AAB8. (RNG-Gesamtzahl 483 stimmt.)

### [10] Semantik des "Kegel-Tests" falsch
Behauptet "liefert 1, wenn a1 innerhalb +-90 Grad VOR a0 liegt". Die Funktion vergleicht die BLICKRICHTUNGEN beider Entities (+0x76 gegen +0x76). Es geht keine Position ein, es gibt kein atan2. Es ist ein Facing-Alignment-Test (gleiche Richtung +-90 Grad), kein Positions-Kegel.

**Gegenbeleg:** 0x80015910 `lh v0,118(a1)` / 0x80015914 `lh v1,118(a0)` / `subu v0,v0,v1` / `addiu v0,v0,1024` / `andi v0,v0,0xfff` / `jr ra` / `slti v0,v0,2048`. 118 = 0x76 = das Yaw-Feld, das em_set aus pc[16..17] fuellt (0x80057328/0x80057330). Kein Lesen von +0x38/+0x40 (x/z). Die RE1.5-Entsprechung ist im Werkzeug selbst als "facing_aligned" (0x8001A780) gefuehrt, nicht als arc_test.

### [24] Behauptete Laengen-Divergenz bei Opcode 0x13 existiert nicht
"0x13=2 (RE1.5 4)". RE1.5 und RE2 haben bei 0x13 dieselbe Funktion und dasselbe Advance = 2.

**Gegenbeleg:** RE1.5 Handler 0x8003FA5C: 0x8003FA7C `addiu a3,a3,4`, Endstore 0x8003FB2C mit `addiu v1,v1,2`. RE2 Handler 0x80054020: Endstore 0x800540EC mit 2. Die Bytefolge ist Instruktion fuer Instruktion identisch (RE1.5 0x8003FAC0-0x8003FAF8 == RE2 0x80054084-0x800540BC, nur die lui-Basis 0x800B vs 0x800D unterscheidet sich).

### [24] "RE1.5 12" fuer 0x38 und "RE1.5 10" fuer 0x4D sind nicht disassembliert, sondern aus Prosa uebernommen
In beiden RE1.5-Handlern existiert gar kein `sw rX,0x1c(rY)`, aus dem sich ein Advance ableiten liesse. Die Zahl 10 steht woertlich in RE15_SCD_OPCODES_REFERENCE.md Zeile 132 ("copy a 10-byte operand block") — das ist ein Decompilat-Kommentar, kein Byte-Beleg.

**Gegenbeleg:** Eigener Scan ueber RE1.5 0x800417AC (op 0x38) und 0x800408A8 (op 0x4D) bis zum ersten `jr ra` bzw. +0x800: null Treffer fuer `sw *,28(*)`. Zum Vergleich liefert derselbe Scan fuer RE1.5 0x2D (0x80040914) korrekt 34 @0x80040AA8, fuer 0x4E (0x80041980) 5 @0x80041A70 und fuer 0x44 (0x800420A0) 20 @0x8004262C/0x80042630 — diese drei sind belegt, jene zwei nicht.

### [5] Falsche Adresse fuer die EMZ0-Dispatch-Form
"Gleiche Form in EMZ0.BIN @0x801003AC ff." — dort steht kein +0x4-Dispatch.

**Gegenbeleg:** EMZ0.BIN 0x801003AC: `lw a0,0(s0)` / 0x801003B0 `lui v0,0xc00` / `and v1,a0,v0` / `bne v1,v0,0x8010045C` — ein Bit-Test auf Entity+0x0. Ich habe alle Zugriffe mit Offset 4 und Basis s0 in EMZ0 gescannt: genau zwei (0x801004CC `lbu v0,4(s0)`, 0x8010061C `lw`). Der echte Dispatch ist 0x801004CC `lbu v0,4(s0)` / 0x801004D4 `sll v0,v0,2` / 0x801004D8 `lui at,0x8011` / 0x801004E0 `lw v0,-14288(at)` (Tabelle @0x8010C830) / 0x801004E8 `jalr v0`. Die FORM stimmt also, die zitierte Adresse nicht.

### [14] "96 Eintraege" in 0x800A6A88
Nur die Indizes 0x00..0x3F enthalten Zeiger. Ab Index 64 stehen Daten.

**Gegenbeleg:** Eigener table-Dump 0x800A6A88, 96 Worte: [0..63] alle 0x800A4xxx-0x800A6xxx (EXE-Bereich), [64]=0x32036400, [65]=0x32083206, danach alternierend dieselben zwei Worte bis [95]. Die Grenze 0x40 deckt sich exakt mit der Gegner/NPC-Trennung in em_set 0x80057410 `sltiu v1,v1,0x40`. Die konkret zitierten Lookups ([0x36]=0x800A5EDC, [0x37]=0x800A412C, [0x2B]=0x800A5478, [0x21]=0x800A45A0, [0x10]=0x800A412C) sind dagegen alle korrekt.

### [13] "Bit 0x10000 = Schaden anwenden"
Das Bit waehlt zwischen ZWEI Schadenspfaden, es schaltet den Schaden nicht ab. Das Etikett ist abgeleitet, nicht belegt.

**Gegenbeleg:** 0x80047210 `beq v0,zero,0x80047434` fuehrt nach 0x80047434, und dort steht dieselbe Rechnung noch einmal: 0x8004743C `lbu v0,8(s1)` / 0x80047450 `lw a1,27272(at)` (=0x800A6A88) / 0x80047454-60 `sll v0,v1,2 / addu v0,v0,v1 / sll v0,v0,2 / addiu v0,v0,-20` / 0x80047468 `sll v0,s6,2`. Ebenso ist "Bits 28..31 = Trefferzone (0..2)" nur teilbelegt: 0x80047114 `srl s6,s5,28` liefert 0..15; eine Begrenzung auf 0..2 steht nirgends im Aufloeser.

### [22] Verzweigungspolaritaet im zitierten Trace vertauscht (Schlussfolgerung bleibt richtig)
Er schreibt "slti v0,v0,3 / bne -> lbu a1,462(a0) / addiu a0,a2,128 (=0x800D489C)". Der bne wird bei stage<3 genommen und fuehrt zur FLAG-Pruefung; der 0x800D489C-Pfad ist der Durchfall bei stage>=3.

**Gegenbeleg:** 0x80018FC8 `slti v0,v0,3` / 0x80018FCC `bne v0,zero,0x80018FE0` ; Durchfall 0x80018FD4 `lbu a1,462(a0)` / 0x80018FD8 `j 0x8001900C` / 0x80018FDC `addiu a0,a2,128` (=0x800D489C). 0x80018FE0 ist der Zweig mit `lw 0x800CFBD8` / `lui v1,0x200`. Bei nicht gesetztem Bit 0x80018FF0 `beq v0,zero,0x80019004` -> 0x80019008 `addiu a0,a2,96` = 0x800D487C. Nettoergebnis stage>=3 ODER Bit -> 0x800D489C stimmt also, die Instruktionsfolge im Bericht nicht.

### [0] "495 RDTs"
Es sind 250 .RDT-Dateien. Die 495 entstehen, weil neben jeder RDT ein gleichnamiges Kleinschreib-Verzeichnis liegt (245 Stueck) und `ls | wc -l` beides zaehlt.

**Gegenbeleg:** `ls info/re2leon/PL0/RDT/*.RDT | wc -l` = 250 ; `ls -d info/re2leon/PL0/RDT/*/ | wc -l` = 245 ; 250+245 = 495.

### [21] "Wort 0 = 6 (Header/Zaehler)"
Unbelegtes Etikett. Wort 0 hat ueber die sechs vorhandenen Gegner-Overlays keinen konsistenten Sinn.

**Gegenbeleg:** xxd Offset 0: EMD0G_MOD0=6 (danach SOFORT Code, null Zeiger), EMOVL21_S0=7 (danach 7 Zeiger 0x801007EC..0x801008FC), EMOVL21_S1=0x26 (danach 0x8010D7EC..), EMS25=0x0B (11 Zeiger), EMS26=0x0C aber nur 6 Zeiger, dann @0x1C `e8ff bd27` = Code, EMZ0=5 (danach keine Zeiger, sondern Bytefolge `0002 0002 0002 0200 0b0b 0e0e`). Dass EMD0G Code ist, bleibt korrekt und ist unabhaengig belegt (0x80100010 `lui v0,0x800d` / `lw v0,-1060` = 0x800CFBDC, der Standard-Gate).

### [0]/[16] Verschwiegene Bedingung: alle fuenf ROOM7040-Records stehen auf (-32000, 0, -32000)
Der Bericht nennt nur Typ, Slot, Flags und em_flag und leitet aus pc[6]=0 "Boden-Y = 0 (passt zu Y=0 im Record)" ab, verschweigt aber, dass X und Z auf -32000 geparkt sind. Der Boss und seine vier Begleiter werden also NICHT an ihre Kampfposition gesetzt — die muss das Raum-Skript liefern. Das aendert die Aussage "Boden-Y aus dem Etagen-Byte passt" von einer Bestaetigung in eine Nichtaussage.

**Gegenbeleg:** ROOM7040.RDT @0x11AC..0x1219, pc[10..11] und pc[14..15] jedes der 5 Records = `00 83` = 0x8300 = -32000; pc[12..13] = 0x0000. Identisch in ROOMG040.RDT @0x1184..0x11F1. Zum Kontrast ROOM7020 @0x1236 (Typ 0x2B): X=-8000, Y=9000, Z=-25000, also echte Koordinaten.

## Gesamturteil der Gegenpruefung
Das Geruest haelt, die Peripherie nicht. Bestaetigt und selbst nachgelesen sind: die Zensus-Aussage (Typ 0x36/0x37 existieren nur in ROOM7040 + ROOMG040), Opcode 0x44 = 22 Byte mit Handler 0x8005714C, das komplette Feld-Layout, alle Wurzel-Adressen inkl. der von mir rekonstruierten vollstaendigen Installer-Tabelle 0x8001B280-0x8001B704, der Laufzeit-Dispatch ueber 0x800CFE5C, der 0x8000-Schlafgate, der em_flag-0xFF-Pfad, +0x1F0-Abstand, die Trefferaufloeser-Mathematik und ALLE 57 Schadenswerte des Typs 0x36 @0x800A5EDC, die CD-Tabellen-Zahlen und der Negativ-Beleg fuer das fehlende Overlay. 19 Befunde fallen ganz oder teilweise. Die schwerwiegendsten drei: (1) k=2/k=3 sind vertauscht — die als \"EMD des Endgegners\" gemeldete Datei (LBA 3468, 132640 B) ist in Wahrheit die Textur, das Modell ist LBA 3533 / 112340 B; belegt an EM_TYPE20.TIM=66592=k=2 und EM_TYPE20.EMD=121144=k=3 im Repo. (2) Die Schadensskalierung an den Spieler ist invertiert abgeschrieben (Boost bei HP>=121, nicht HP<121, 0x80040200 `bne`). (3) Der \"Zombie-Freeze-Gate\" ist die falsche Instruktion mit falscher Polaritaet — der echte steht bei 0x8010035C und ist derselbe 0x800CFBDC & 0x20000000 wie ueberall. Dazu eine systematische Fehlerklasse: der Ermittler hat mehrfach die Annotationsspalte von re2_disasm.py abgeschrieben, die bei registerrelativen Zugriffen (lui_upper<<16)+offset rechnet und das addiu ignoriert; daher sind 0x800D2144, 0x800D3C2C, 0x800D4134/0x800D4138 alle falsch (korrekt: 0x800CE32C, 0x800CFE14, 0x800D031C/0x800D0320). Weiter: eine erfundene Laengen-Divergenz bei SCD-Opcode 0x13 (RE1.5 und RE2 sind dieselbe Funktion mit demselben Advance 2), zwei aus einem Decompilat-Kommentar statt aus Bytes stammende RE1.5-Laengen (0x38=12, 0x4D=10), eine falsch besetzte Schnittmenge der EXE-Bibliothek, ein als Positions-Kegel etikettierter reiner Facing-Vergleich (0x80015910), und ein Zensus-Filter (\"Slot<0x18\"), der gueltige Slot-0xFF-Spawns wegwirft — ROOM7020 spawnt zusaetzlich Typ 0x41, ROOM7000 zusaetzlich Typ 0x4F. Verschwiegen wurde ausserdem, dass alle fuenf ROOM7040-Records auf (-32000, 0, -32000) geparkt sind, der Boss also gar nicht aus dem Record positioniert wird.

---

# Strang: boss-modell

## Kernaussage
KEINE der genannten .BIN-Dateien ist ein Modell — EMD0G_MOD0.BIN/EMS25.BIN/EMS26.BIN/EMOVL21_S*.BIN/EMZ0*.BIN sind MIPS-Code-Overlays (@0x80100000) und byte-identische Kopien von Records aus CDEMD0.EMS; EMD0G_MOD0.BIN ist beweisbar das AI-Overlay von kind 0x20 (nicht "G"). Die RE2-Gegnermodelle liegen ausschliesslich als Records in info/re2leon/PL0/PLD/CDEMD0.EMS (11.124.736 B = exakt 5432 Sektoren), adressiert ueber die EXE-Sektor-TOC @0x8009adf4 (Datei 0x8B5F4, 300 x {u32 Sektor, u32 Groesse}, Index (kind-0x10)*4+rec) — selbst nachgelesen im Reader FUN_8001b934 @0x8001b9c0/@0x8001b9a8; alle 55 belegten EMD-Records tragen dir_off == size-0x20 und dir_count == 8. Die G-Birkin-Familie ist damit vollstaendig vermessen: kinds 0x30 (16 Bones, 21 Clips, MD1 1426 V/947 F), 0x31 (18 Bones, 24 Clips, 1368 V/965 F), 0x33 (29 Bones, 21 Clips, 1576 V/1065 F), 0x34 (27 Bones, 29 Clips, 2062 V/1427 F — groesster Record 273.812 B) und 0x36 (nur 2 Bones, 11 Clips a 100/180/150 Frames, 1756 V/963 F, TIM 512x256 statt 384x256). Der Lauf-Lader ist tabellengetrieben, NICHT dateinamenbasiert: die Debug-Strings "d:/bio2/room/emd/em000.emd" @0x8009d410 und ".tim" @0x8009d444 haben im gesamten EXE NULL Referenzen (weder lui/addiu-Paar noch Zeigerwort) — tot. Gespawnt wird ueber genau zwei SCD-Opcodes (Dispatch-Tabelle @0x800a74c8): 0x44 (FUN_8005714c, 22 B) und 0x8E (FUN_80057714, 24 B), beide lesen den kind aus Instruktionsbyte +5 und rufen den Overlay-Lader FUN_8001b710 — ein EXE-weiter jal-Scan findet keine weiteren Aufrufer. Damit belegt: 0x31 spawnt in ROOM60C0, und die LETZTEN Raeume beider Szenarien (ROOM7040 + ROOMG040) spawnen kind 0x3A; fuer 0x30/0x33/0x34/0x36 existiert in KEINER der 495 RDTs ein 0x44/0x8E-Spawn — deren Instanziierung ist aus den RDTs allein nicht aufloesbar (offene Luecke). Die Zuordnung "EM030..EM036 = Birkin 1..5" stammt aus BioModels.h:197-203 (Fremdquelle, NICHT byte-belegt). RE1.5 hat die Gegenstuecke: shared_assets/PSX/EMD/CDEMD0.EMS Blob 16 = Typ 0x30 (226.404 B, 16 Bones, 21 Clips, 1198 V/738 F) und Blob 17 = Typ 0x36 (128.600 B, 16 Bones, 5 Clips, 550 V/349 F) — beide mit TIM IM EMD (dir_count 9 statt 8) und mit anderer Bone-Topologie als RE2. Die Luecke zum Anzeigen des RE2-Bosses im Port ist klein auf der Datenseite (shared_assets/RE2/CDEMD0.EMS ist md5-identisch mit dem Original, TOC ist vendored, re2_ems_load_bank ist kind-generisch) und gross auf der Code-Seite (keine Hybrid-Permutation fuer 16/18/27/29/2-Bone-Rigs, keine portierte RE2-Boss-KI — die liegt als undisassemblierte Overlay-Records in der EMS).

## Befunde

### EMD0G_MOD0.BIN ist KEIN Modell  [byte-belegt]
EMD0G_MOD0.BIN (22.266 B) ist ein MIPS-Code-Overlay und byte-identisch mit CDEMD0.EMS[1206*0x800 : +22266] = TOC-Record (kind 0x20, rec 1 = Overlay-Kopie Slot 0 @0x80100000). Der Name 'G' fuehrt in die Irre.

**Beleg:** Selbst verglichen: md5(EMD0G_MOD0.BIN)=b33a2e2962035312039c7d2510d315b3 == md5(info/re2leon/PL0/PLD/CDEMD0.EMS[1206*2048:+22266]) (Python-Vergleich seg==o -> True; Sektor 1195 ergibt False). Disasm @0x80100004 (--bin EMD0G_MOD0.BIN): 80100004 addiu sp,sp,-24 / 80100008 sw s0,16(sp) / 80100010 lui v0,0x800d / 80100014 lw v0,-1060(v0) [=0x800cfbdc] / 80100018 lui v1,0x2000 / 8010001c and v0,v0,v1 / 80100020 bne v0,zero,0x801000e0 (globales Freeze-Gate) / 80100028 lbu v1,467(s0) / 80100030 andi v0,v1,0x7f / 8010003c sb v0,467(s0) (Entity-Timer +0x1D3) / 80100040 lbu v0,562(s0) (+0x232).

**Port-Stand:** nicht geprueft (der Port nutzt keine .BIN-Overlays aus COMMON/BIN)

### Alle uebrigen COMMON/BIN-Kandidaten sind Overlays  [byte-belegt]
EMS25.BIN=26324 B=kind 0x25 Overlay, EMS26.BIN=4346=kind 0x26, EMOVL21_S0/S1.BIN=19080=kind 0x21 rec1/rec0, EMZ0.BIN=EMZ0_d1.BIN=EMOVL10_S0.BIN=53068=kind 0x10 (md5 alle 7f7a39e651064e07c95830db1bebdb89), EMOVL10_S1.BIN=zweite Kopie (md5 6123ab3c...). Die Namen kodieren die kind-Nummer.

**Beleg:** TOC-Groessen (Sektor,Groesse) aus s_re2_cdemd0_toc: kind 0x10 (0,53068)/(26,53068); 0x21 (1310,19080)/(1320,19080); 0x25 (1812,26324); 0x26 (1898,4346); 0x20 (1195,22266)/(1206,22266) — decken sich 1:1 mit den Dateigroessen (ls -la info/re2leon/COMMON/BIN). Dateikopf EMS25.BIN @0x00: 0b 00 00 00 60 04 10 80 8c 05 10 80 7c 04 10 80 ... = u32-Zaehler 0x0B + Zeiger nach 0x80100460/0x8010058c/... = overlay-relative Codezeiger.

**Port-Stand:** nicht geprueft

### Fundort des Modells  [byte-belegt]
Die RE2-Gegnermodelle liegen als EMD-Records IN info/re2leon/PL0/PLD/CDEMD0.EMS (11.124.736 B = 5432 x 2048, exakt der von der TOC ueberdeckte Bereich). Kein Record hat einen Dateinamen; adressiert wird ueber Sektor+Groesse aus der EXE-TOC.

**Beleg:** TOC @0x8009adf4 (Datei 0x8B5F4). Selbst im Reader nachgelesen — FUN_8001b934: 8001b994 sll v1,t1,3 (idx*8) / 8001b9a0 lui at,0x800a / 8001b9a4 addu at,at,v1 / 8001b9a8 lw v0,-21000(at) [=0x8009adf8 = Groesse] / 8001b9b4 sw v0,21256(at) [DAT_800d5308] / 8001b9c0 lw t0,-21004(at) [=0x8009adf4 = Sektor] / 8001b9c8 sw t0,21268(at) [DAT_800d5314]. Validierung aller 55 belegten EMD-Records: u32@+0 (dir_off) == size-0x20 und u32@+4 == 8 fuer JEDEN Record (z.B. kind 0x30 @0x00572000: dir_off 0x2525c, size 0x2527c).

**Port-Stand:** re15_port/engine/src/gen/re2_ems_toc.inc vendored dieselbe Tabelle (s_re2_cdemd0_toc[600]); re2_ems_toc_entry()/re2_ems_locate() in re15_port/engine/src/re2_ems.c:21-47 nutzen sie

### Vor-extrahierte CDEMD0/EM0xx-Dateien sind UNBRAUCHBAR  [byte-belegt]
Die 103 Dateien in info/re2leon/PL0/PLD/CDEMD0/ sind ein naiver, um ganze Sektoren VERSCHOBENER Split; sie sind keine gueltigen EMD-Records und der Versatz waechst durch das Archiv.

**Beleg:** Alle Groessen sind 2048-Vielfache (EM030.EMD 219136), die echten Records nicht (kind 0x30 = 152188 B). Suche der ersten 4 KB im Archiv: EM010.EMD -> 0x29800 (TOC sagt 0x2A800, also 0x1000 zu frueh), EM010.TIM -> 0x19000 (TOC 0x1A000), EM030.EMD -> 0x4C6000 (TOC 0x572000, 0xAC000 Versatz). Kopf-Bytes bestaetigen den Schrott: EM031.EMD @0x00 = 00 60 f7 00 00 00 4b 0e (dir_off/dir_count-Paar unmoeglich), EM033.EMD = 00 00 00 00 00 ba 0f 04, EM030.EMD = 06 00 06 00 07 00 00 00.

**Port-Stand:** Der Port liest diese Dateien nicht — er splittet den Record direkt aus dem EMS (re2_emd_parse_bank)

### Einzige korrekte Einzel-Extraktion im Repo  [byte-belegt]
info/re2leon/COMMON/BIN/EM_TYPE20.EMD (121.144 B) + EM_TYPE20.TIM (66.592 B) sind exakt der EMD-/TIM-Record von kind 0x20.

**Beleg:** TOC kind 0x20: TIM (Sektor 1217, 0x10420 = 66592), EMD (Sektor 1250, 0x1D938 = 121144) — identisch mit den Dateigroessen. EMD-Kopf: 18 d9 01 00 08 00 00 00 = dir_off 0x1D918, dir_count 8; Directory @0x1D918 = 08 00 00 00, 0c 00 00 00, bc 0e 00 00, 40 57 01 00, 44 57 01 00, 48 57 01 00, 94 59 01 00, ec 86 01 00. TIM-Kopf: 10 00 00 00 09 00 00 00 (Magic 0x10, Flag 9 = 8bpp+CLUT).

**Port-Stand:** nicht geprueft

### RE2-EMD-Struktur (Directory)  [byte-belegt]
RE2-EMD = u32 dir_off @+0, u32 dir_count == 8 @+4, Directory am DATEIENDE (dir_off = size-0x20). Kein TIM im EMD (RE1.5 hat 9 Eintraege inkl. TIM). Belegung: dir[0] Reserve, dir[1]/[2] EDD/EMR-Paar 1 (dir[2] traegt die Bone-STRUKTUR), dir[3]/[4] Paar 2, dir[5]/[6] Paar 3 (durchgaengig 15 Bones / kf 80 = Opfer-Rig), dir[7] MD1.

**Beleg:** Binder FUN_8001aaa8, selbst disassembliert: 8001ab88 lw v1,0(s2) (dir_off) / 8001ab90 addu s0,s2,v1 / 8001aba0 sw v1,444(s1) [dir0->+0x1BC] / 8001abb0 sw v1,380(s1) [dir1->+0x17C] / 8001abc0 sw v1,264(s1) [dir2->+0x108] / 8001abd0 sw v1,388(s1) [dir3->+0x184] / 8001abe0 sw v1,384(s1) [dir4->+0x180] / 8001abf0 sw v1,396(s1) [dir5->+0x18C] / 8001ac00 sw v1,392(s1) [dir6->+0x188] / 8001ac14 sw v1,20(s1) [dir7->+0x14].

**Port-Stand:** re15_port/include/re2_ems.h Kopf und re15_port/engine/src/re2_ems.c:88-120 dokumentieren/implementieren exakt diese Bindung

### Modell kind 0x30 (Birkin 1 nach Fremdquelle)  [byte-belegt]
EMD Sektor 2788 (@EMS 0x00572000), 152.188 B. dir = [0x000008(5596), 0x0015e4(5196), 0x002a30(84604), 0x0174ac(4), 0x0174b0(4), 0x0174b4(924), 0x017850(17528), 0x01bcc8(38292)]. Paar1: 21 Clips / 1277 Frames [30,68,70,70,76,56,94,30,40,25,25,58,68,46,59,92,100,40,156,64,10], EMR 16 Bones, kf_size 84, 1005 Keyframes. Paar2 leer (4-B-Stubs). Paar3: 8 Clips / 222 Frames, 15 Bones / kf 80. MD1: nObj 38 = 19 Meshes, 1426 Vertices, 947 Faces (562 Tri + 385 Quad). TIM Sektor 2739, 99.872 B, 8 bpp, 3 CLUTs @(0,480), 384x256 px. parent[] = [-1,0,1,2,2,4,5,2,7,8,0,10,11,0,13,14].

**Beleg:** Selbst geparst aus info/re2leon/PL0/PLD/CDEMD0.EMS @0x572000 mit den Parserregeln aus re15_port/engine/src/emd_common.c:136-196 (EMR u16 @+0/+2/+4/+6), :48-99 (EDD u16 count/offset0, clip_count=offset0/4) und md1_common.c:41-100 (u32 length/unk/nObj, Mesh-Header 56 B). TOC-Eintrag kind 0x30: TIM (2739, 99872), EMD (2788, 152188).

**Port-Stand:** re2_emd_parse_bank() wuerde diesen Record heute parsen (dir_count==8 erfuellt); geladen wird er nur, wenn der Typ mit RE2-Flavor gespawnt wird

### Modell kind 0x31  [byte-belegt]
EMD Sektor 2944 (@0x005c0000), 193.124 B. Paar1: 24 Clips / 1540 Frames, EMR 18 Bones, kf 96, 1357 Keyframes. Paar3: 1 Clip / 100 Frames (15 Bones). MD1: nObj 36 = 18 Meshes, 1368 Vertices, 965 Faces. TIM Sektor 2895, 99.872 B, 3 CLUTs, 384x256. parent[] = [-1,0,1,2,2,4,5,5,5,2,9,10,0,12,13,0,15,16]. Overlay (AI-Code) Sektoren 2863/2879, je 32.088 B.

**Beleg:** Wie oben, EMS-Offset 0x5C0000 (dir_off 0x2f244, count 8). TOC kind 0x31: (2863,32088) (2879,32088) (2895,99872) (2944,193124).

**Port-Stand:** nicht geprueft

### Modell kind 0x33  [byte-belegt]
EMD Sektor 3114 (@0x00615000), 204.968 B. Paar1: 21 Clips / 1092 Frames, EMR 29 Bones, kf 144, 883 Keyframes. Paar3: 7 Clips / 276 Frames. MD1: nObj 58 = 29 Meshes, 1576 Vertices, 1065 Faces. TIM Sektor 3065, 99.872 B, 3 CLUTs, 384x256. parent[] = [-1,0,1,2,2,4,5,2,7,8,2,10,11,12,13,13,13,2,17,18,19,19,19,0,23,24,0,26,27] (drei Kinder an Slot 13 und an 19 = mehrgliedrige Extremitaeten).

**Beleg:** Geparst @EMS 0x615000, dir_off 0x32088, count 8. TOC kind 0x33: (3039,26552) (3052,26552) (3065,99872) (3114,204968).

**Port-Stand:** nicht geprueft

### Modell kind 0x34 — groesster Gegner-Record des Spiels  [byte-belegt]
EMD Sektor 3310 (@0x00677000), 273.812 B (groesster EMD-Record in CDEMD0.EMS). Paar1: 29 Clips / 1441 Frames, EMR 27 Bones, kf 136, 1296 Keyframes. Paar3: 3 Clips / 210 Frames. MD1: nObj 54 = 27 Meshes, 2062 Vertices, 1427 Faces (941 Tri + 486 Quad). TIM Sektor 3261, 99.872 B, 3 CLUTs, 384x256. dir[0] ist hier 18.776 B gross (bei den meisten kinds nur 4 B). Overlay 45.780 B (groesstes Gegner-Overlay).

**Beleg:** Geparst @EMS 0x677000, dir_off 0x42d74, count 8, dir = [0x000008(18776), 0x004960(5884), 0x00605c(176564), 0x031210(4), 0x031214(4), 0x031218(856), 0x031570(15528), 0x035218(56156)]. TOC kind 0x34: (3215,45780) (3238,45780) (3261,99872) (3310,273812).

**Port-Stand:** nicht geprueft

### Modell kind 0x36 — Sonderfall, nur 2 Bones  [byte-belegt]
EMD Sektor 3533 (@0x006e6800), 112.340 B. Paar1: 11 Clips / 1360 Frames mit auffaellig runden Laengen [100,180,150,100,180,150,100,50,50,150,150]; EMR hat nur 2 Bones, kf_size 24, 1360 Keyframes (also 1 Keyframe pro Frame). Paar3: 1 Clip / 150 Frames (15 Bones = Opfer-Rig). MD1: nObj 14 = 7 Meshes, 1756 Vertices, 963 Faces (358 Tri + 605 Quad) — hohe Vertexzahl bei nur 7 Meshes. TIM Sektor 3468, 132.640 B, 3 CLUTs, 512x256 px (alle uebrigen Birkin-Formen 384x256). dir[0] = 19.112 B. parent[] = [-1,0].

**Beleg:** Geparst @EMS 0x6E6800, dir_off 0x1b6b4, count 8, dir = [0x000008(19112), 0x004ab0(5488), 0x006020(32672), 0x00dfc0(4), 0x00dfc4(4), 0x00dfc8(608), 0x00e228(12008), 0x011110(42404)]. TOC kind 0x36: (3444,23476) (3456,23476) (3468,132640) (3533,112340).

**Port-Stand:** nicht geprueft

### Laufzeit-Lader: Index-Bildung  [byte-belegt]
Der Binder FUN_8001aaa8 liest den kind aus dem Spawn-Struct (+8), bildet idx = (kind-0x10)*4 und holt TIM mit idx|2 und EMD mit idx|3 aus der TOC. Kinds 0x50..0x5A haben einen Sonderpfad.

**Beleg:** 8001aac8 lbu s0,8(s1) / 8001aad4 addiu v0,s0,-80 / 8001aad8 sltiu v0,v0,0xb (Bereich 0x50..0x5A) / 8001ab04 addiu s0,s0,-16 / 8001ab48 sll s0,s0,2 / 8001ab4c ori a0,s0,0x2 ; jal 0x8001b934 (TIM) / 8001ab7c ori a0,s0,0x3 ; jal 0x8001b934 (EMD).

**Port-Stand:** re2_ems.c:29 bildet i = ((kind-0x10)*4+rec)*2 auf dieselbe Tabelle ab

### Laufzeit-Lader: Overlay-Klemmung  [byte-belegt]
Der AI-Overlay-Lader FUN_8001b710 klemmt alle kinds 0x10..0x1F auf 0x10 (ein gemeinsames Zombie-Overlay) und laedt nach 0x80100000 + slot*0xD000.

**Beleg:** 8001b724 lw v0,-36(at) [0x800cffdc + kind*4] / 8001b738 addiu v0,t1,-16 / 8001b73c sltiu v0,v0,0x10 / 8001b740 beq v0,zero,0x8001b74c / 8001b748 addiu t1,zero,16.

**Port-Stand:** in re2_ems.h dokumentiert (RE2_EMS_REC_OVL_SLOT0/1); der Port laedt keine RE2-Overlays, er hat eigene C-Brains

### Keine Dateinamen-Bildung — Debug-Strings sind tot  [byte-belegt]
RE2 bildet zur Laufzeit KEINEN Dateinamen 'em0XX.emd'. Die im EXE vorhandenen Pfad-Strings sind unreferenziert.

**Beleg:** strings -a -t x info/re2leon/PSX.EXE: 0x8dc10 'd:/bio2/room/emd/em000.emd', 0x8dc44 'd:/bio2/room/emd/em000.tim' (= RAM 0x8009d410 / 0x8009d444 bei t_addr 0x80010000, Header-Bytes @0x18: 00 00 01 80). Eigener EXE-weiter Scan nach lui(0x800a)+addiu/lw(0xd410 bzw. 0xd444): 0 Treffer. Eigene Suche nach dem Zeigerwort 10 d4 09 80 bzw. 44 d4 09 80 in der ganzen EXE: 0 Treffer.

**Port-Stand:** nicht relevant

### SCD-Spawn: Opcode-Dispatch  [byte-belegt]
Die RE2-SCD-Opcode-Tabelle liegt @0x800a74c8 (143 Eintraege), Index = Opcode-Byte. Genau zwei Opcodes spawnen einen Gegner und rufen den Modell-/Overlay-Lader: 0x44 (FUN_8005714c, PC-Vorschub 22 B) und 0x8E (FUN_80057714, Vorschub 24 B).

**Beleg:** Dispatcher: 80053f70 lbu v0,0(a1) / 80053f78 sll v0,v0,2 / 80053f7c lui at,0x800a / 80053f84 lw v0,29896(at) [=0x800a74c8] / 80053f8c jalr v0. Tabelleneintrag 0x800a75d8 = 0x8005714c => Opcode (0x800a75d8-0x800a74c8)/4 = 68 = 0x44; Eintrag 0x800a7700 = 0x80057714 => Opcode 142 = 0x8E. Vorschub: 800576e4 addiu v1,v1,22 ; 800576e8 sw v1,28(s5) — und 80057d78 addiu v1,v1,24 ; 80057d7c sw v1,28(s5). EXE-weiter jal-Scan auf 0x8001b710 findet NUR die zwei Aufrufer @0x800571f0 (in FUN_8005714c) und @0x80057884 (in FUN_80057714).

**Port-Stand:** nicht geprueft (der Port hat einen eigenen RE1.5-SCD-VM, scd_vm.c)

### SCD-Spawn: Feldlage im Befehl  [byte-belegt]
Der geladene kind steht im Spawn-Befehl auf Byte +5, das Entity-kind-Feld (+0x1FA) wird aus Byte +9 gefuellt.

**Beleg:** FUN_8005714c: 80057170 lw v0,28(s5) ; 80057178 sw v0,0x800d5be8 (v0 = SCD-PC+2) / 800571e4 lw v0,0x800d5be8 ; 800571ec lbu a0,3(v0) ; 800571f0 jal 0x8001b710 => Befehlsbyte +5 = kind / 8005726c lw v0,0x800d5be8 ; 80057274 lbu v0,7(v0) ; sb v0,506(s0) [Entity+0x1FA] => Befehlsbyte +9. Identisch in FUN_80057714 (@0x80057880 lbu a0,3(v0); @0x80057908 lbu v0,7(v0)). Realbeispiel ROOM1140 SCD-Sektion 17 +0x001e: 44 00 00 1f 01 20 00 2b 00 5b 4a 96 00 00 92 b9 2b 0e 00 00 00 00 -> Byte+5 = 0x20 (Hund).

**Port-Stand:** nicht geprueft

### Raumbindung: was belegt ist  [byte-belegt]
In den SCD-Sektionen (RDT-Offsettabelle Index 16 = init, 17 = main) aller 495 RE2-RDTs findet der Opcode-0x44/0x8E-Scan: kind 0x31 in ROOM60C0 (Opcode 0x8E) und kind 0x3A in ROOM7040 UND ROOMG040 — den jeweils letzten Raeumen beider Szenarien. Fuer 0x30, 0x33, 0x34 und 0x36 gibt es KEINEN 0x44/0x8E-Spawn in irgendeiner RDT.

**Beleg:** SCD-Sektionsindex selbst bestimmt: fuer alle geprueften RDTs erfuellen genau die Offsets 16 und 17 die Sub-Tabellen-Form (u16[0] gerade, = 2*count, danach streng monoton steigende Offsets innerhalb der Sektion). ROOM7000 offsets[17] = 0x147c -> 14 00 4e 00 50 00 76 02 aa 02 26 04 5e 04 96 04 dc 04 64 05 (10 Subs). Treffer ROOM7040 Sektion 17 +0x084a: 44 00 32 00 02 3a 00 00 ee a3 33 00 00 00 00 08 00 00 42 22 01 04 (Byte+5 = 0x3A); byte-identisch in ROOMG040 +0x084a. Treffer ROOM60C0 Sektion 16 +0x00c0: 8e 00 46 03 04 31 03 00 00 00 ff ff 08 00 06 00 0a 00 21 04 a4 01 22 01 (Byte+5 = 0x31).

**Port-Stand:** nicht geprueft

### Modell kind 0x3A (Gegner des letzten Raums)  [byte-belegt]
EMD Sektor 3874 (@0x00791000), 43.612 B. Paar1: 11 Clips / 257 Frames, EMR 13 Bones, kf 72. Paar3: 4 Clips / 89 Frames. MD1: nObj 34 = 17 Meshes, 738 Vertices, 311 Faces. TIM Sektor 3841, 66.592 B, 2 CLUTs, 256x256 px. Overlay 20.096 B (Sektoren 3821/3831).

**Beleg:** Geparst @EMS 0x791000, dir_off 0xaa3c, count 8. TOC kind 0x3A: (3821,20096) (3831,20096) (3841,66592) (3874,43612). Spawn-Beleg siehe Befund 'Raumbindung'.

**Port-Stand:** nicht geprueft

### Namenszuordnung Birkin 1..5  [abgeleitet]
Die Gleichsetzung EM030=Birkin1, EM031=Birkin2, EM033=Birkin3, EM034=Birkin4, EM036=Birkin5 stammt aus einer Fremdquelle im Repo, nicht aus RE2-Bytes. Sie ist mit den gemessenen Daten VERTRAEGLICH (0x30/0x31/0x33/0x34 teilen sich TIM-Geometrie 384x256/3 CLUTs und steigende Bone-Zahlen 16/18/29/27), aber nicht bewiesen.

**Beleg:** info/Resident_Evil_und_Playstation_Information/BioModels-master/src/BioModels.h:197-203: { "EM030.EMD", BIRKIN_1 }, { "EM031.EMD", BIRKIN_2 }, { "EM033.EMD", BIRKIN_3 }, { "EM034.EMD", BIRKIN_4 }, // { "EM036.emd", BIRKIN_5 }, { "EM037.EMD", BIRKIN_PROJECTILE_1 }, { "EM038.EMD", BIRKIN_PROJECTILE_2 }; Zeile 231: "EM036.emd Birkin 5, wrong mesh and texture". Passend dazu misst kind 0x37 nur 4 Bones und 156 Vertices (Projektil-Groessenordnung). NB: EM038 existiert in der TOC NICHT (kind 0x38 = leerer Slot).

**Port-Stand:** nicht relevant

### RE1.5-Gegenstueck Typ 0x30  [byte-belegt]
RE1.5 hat den Typ 0x30 in re15_port/shared_assets/PSX/EMD/CDEMD0.EMS als Blob 16 @0x00276800, 226.404 B, dir_count 9 (TIM IM EMD, dir[8]). Paar1: 21 Clips / 1358 Frames, EMR 16 Bones, kf 84, 912 Keyframes. Paar3: 3 Clips / 96 Frames. MD1: nObj 34 = 17 Meshes, 1198 Vertices, 738 Faces. TIM 99.872 B, 8 bpp, 3 CLUTs, 384x256. parent[] = [-1,0,1,2,0,4,5,0,7,8,8,10,11,8,13,14].

**Beleg:** Selbst gelaufen mit der EMS-Kettenregel aus re15_port/engine/src/re15_ems.c:23-58 (dir_off = u32@cursor, len = dir_off+36, danach auf 2048 aufrunden, Null-Luecke ueberspringen) und der Reihenfolge s_ems_order (re15_ems.c:64-68) mit 0x30 an Index 16, 0x36 an Index 17. Blob 16: dir = [0x000008(4716), 0x001274(5520), 0x002804(76792), 0x0153fc(4), 0x015400(4), 0x015404(400), 0x015594(7688), 0x01739c(31364), 0x01ee20(99872)]. CDEMD1.EMS traegt denselben Blob @0x00278800.

**Port-Stand:** pc_enemy_read_re15_emd() (re15_port/platform/pc/main.c) liest genau ueber re15_ems_index_for_type/re15_ems_get_entry — Typ 0x30/0x36 sind damit heute ladbar

### RE1.5-Gegenstueck Typ 0x36  [byte-belegt]
RE1.5-Blob 17 @0x002ae000, 128.600 B, dir_count 9. Paar1: nur 5 Clips / 195 Frames, EMR 16 Bones, kf 84, 120 Keyframes; Paar2 UND Paar3 sind 4-Byte-Stubs. MD1: nObj 32 = 16 Meshes, 550 Vertices, 349 Faces. TIM 99.872 B, 3 CLUTs, 384x256. parent[] identisch mit RE1.5-0x30 => beide teilen sich dasselbe Rig, 0x36 ist die kleinere Variante.

**Beleg:** dir = [0x000008(3156), 0x000c5c(804), 0x000f80(10264), 0x003798(4), 0x00379c(4), 0x0037a0(4), 0x0037a4(4), 0x0037a8(14444), 0x007014(99872)]. Selbst geparst aus re15_port/shared_assets/PSX/EMD/CDEMD0.EMS.

**Port-Stand:** ladbar wie 0x30

### Rig-Bruch RE1.5 <-> RE2  [byte-belegt]
RE1.5-0x30 und RE2-0x30 haben zwar beide 16 Bones und kf_size 84, aber UNTERSCHIEDLICHE Hierarchie — RE2-Keyframes koennen das RE1.5-Mesh ohne Bone-Permutation nicht posieren.

**Beleg:** RE1.5 parent[] = [-1,0,1,2,0,4,5,0,7,8,8,10,11,8,13,14] gegen RE2 parent[] = [-1,0,1,2,2,4,5,2,7,8,0,10,11,0,13,14] (beide selbst aus dem jeweiligen dir[2]-EMR ueber bones_tbl/child-Listen rekonstruiert, Regel emd_common.c:164-183).

**Port-Stand:** re2_hybrid_perm() (re15_port/engine/src/re2_ems.c:169-183) kennt NUR die kinds 0x10,0x11,0x12,0x13,0x16,0x18 (15 Bones), 0x20 (17), 0x21 (13), 0x25 (20), 0x26 (1). Fuer 0x30/0x31/0x33/0x34/0x36 gibt es keine Permutation -> re2_hybrid_apply liefert -2/-3

### Datenlage im Port fuer den RE2-Boss  [byte-belegt]
Die RE2-Modelldaten liegen bereits byte-identisch im Port; der Lader ist kind-generisch und wuerde die Birkin-Bank heute parsen.

**Beleg:** md5 10137142e8d3f2f96c06afc00fd49bfe fuer BEIDE: re15_port/shared_assets/RE2/CDEMD0.EMS und info/re2leon/PL0/PLD/CDEMD0.EMS (je 11.124.736 B). Aufrufpfad: re15_port/platform/pc/main.c:514 `re2_ems_load_bank(ems, ems_sz, type, eb, &tim)` mit type == RE1.5-Typ == RE2-kind; re2_emd_parse_bank prueft nur dir_cnt==8 (re2_ems.c:100) — fuer 0x30/0x31/0x33/0x34/0x36 erfuellt (selbst gemessen).

**Port-Stand:** vorhanden und generisch; nur ueber OPTIONS->AI->RE2 erreichbar und nur fuer Typen, die ueberhaupt gespawnt werden

### RE2-Boss-KI ist im Repo NUR als undisassembliertes Overlay vorhanden  [byte-belegt]
Der ausfuehrbare RE2-Code der Birkin-Formen liegt als MIPS-Overlay-Record in CDEMD0.EMS und ist nirgends decompiliert.

**Beleg:** TOC-Overlay-Records (Sektor, Groesse): 0x30 (2709,30380)+(2724,30380); 0x31 (2863,32088)+(2879,32088); 0x33 (3039,26552)+(3052,26552); 0x34 (3215,45780)+(3238,45780); 0x36 (3444,23476)+(3456,23476). Zielbasis 0x80100000 + slot*0xD000 (FUN_8001b710). RE2_Quellcode_Overlays/ enthaelt dafuer keine Datei (nur STAGE6 hat 4 Dateien).

**Port-Stand:** Der Port hat KEIN RE2-Birkin-Brain; die RE2-KI-Option deckt laut Memory nur Zombie/Hund/Kraehe/Spinne ab

## Fehlende Daten
- info/re2leon/PL0/PLD/CDEMD1.EMS (11.196.416 B) hat KEINE Adressierungstabelle im Repo — der Port vendored nur s_re2_cdemd0_toc. Fehlt: die zweite Sektor-TOC aus der RE2-EXE (Analogon zu 0x8009adf4) bzw. der Nachweis, dass CDEMD1 dieselbe TOC mit anderem Basis-File benutzt. Ohne sie sind die Szenario-B-Modelle nicht byte-true adressierbar.
- info/re2leon/PL0/PLD/CDEMD1/EM059.EMD (8.947.712 B) ist ein Fehl-Split (praktisch das ganze Archiv-Ende in einer Datei), kein Modell. Der Verzeichnisbaum CDEMD0/ und CDEMD1/ ist insgesamt unbrauchbar (sektorverschoben) und muesste aus der TOC neu erzeugt werden.
- RE2_Quellcode_Overlays/ enthaelt KEINE Decompilate der Gegner-Overlays (nur STAGE6 mit 4 Dateien). Es fehlen die Decompilate/Disassemblate der CDEMD0.EMS-Overlay-Records fuer kinds 0x30 (Sektor 2709, 30.380 B), 0x31 (2863, 32.088 B), 0x33 (3039, 26.552 B), 0x34 (3215, 45.780 B), 0x36 (3444, 23.476 B) — sie enthielten die vollstaendige RE2-Boss-FSM (States, Clip-Indizes, Schadensfenster, HP).
- info/re2leon/COMMON/BIN/STAGE1..7.BIN (420..9984 B) sind NICHT die RE2-Raum-/Stage-Code-Overlays. Die echten RE2-Stage-Overlays fehlen im Repo vollstaendig; sie enthielten die raumseitige Event-/Boss-Inszenierung, die im RDT-Skript nicht steht.
- Es gibt keine RE2-SCD-Opcode-Laengentabelle im Repo. Mein aus den 143 Handlern automatisch abgeleiteter Vorschub deckt nur die geradlinigen Opcodes; die Kontrollfluss-Opcodes 0x01, 0x06, 0x09, 0x10, 0x11, 0x17, 0x18, 0x1A, 0x36, 0x37, 0x39, 0x40, 0x46, 0x4E haben keinen konstanten Vorschub (Handler-Adressen: 0x800537fc, 0x80053924, 0x800539dc, 0x80053e0c, 0x80053e50, 0x8005415c, 0x800541a8, 0x80054268, 0x80056428, 0x8005655c, 0x800563e8, 0x80056bd8, 0x80055154, 0x80054cd4). Ohne sie laeuft kein vollstaendiger RE2-SCD-Walker (aktuell 8 von 2568 Subroutinen vollstaendig).
- Keine RE2-Savestates/RAM-Dumps im Repo (stage_saves/ ist reines RE1.5). Damit ist keine dynamische Gegenprobe moeglich, welcher kind in welchem RE2-Raum tatsaechlich lebt — der einzige Weg, die 0x30/0x33/0x34/0x36-Raumbindung zu schliessen, ohne die Stage-Overlays.
- Kein CLUT-/Pixel-Dump der Birkin-TIMs im Repo. Die Farbwahl pro Form (3 CLUTs @(0,480) bei 0x30/0x31/0x33/0x34, ebenfalls 3 bei 0x36 aber 512x256) ist ungeprueft — es fehlt eine gerenderte Gegenprobe (z.B. PNG wie das vorhandene em_type20_tex.png fuer kind 0x20).

## Gegenpruefung — GEKIPPT

### [15] SCD-Spawn: Feldlage im Befehl — kind steht auf +3, NICHT auf +5
Die tragende Praemisse "v0 = SCD-PC+2" ist falsch. Der Ermittler hat den PC-Stand aus dem BEDINGUNGS-Evaluator FUN_80053f50 abgelesen (dort steht direkt vor dem Dispatch ein `addiu a1,a1,2`), aber der Haupt-Interpreter setzt 28(ctx) AUF das Opcode-Byte. Damit ist lbu 3(v0) = Befehlsbyte +3 (kind) und lbu 7(v0) = Befehlsbyte +7 (Entity+0x1FA) — alles um 2 verschoben. Zusaetzlich ist die Aussage in sich widerspruechlich: der Ermittler nennt selbst Vorschub 22 und zeigt eine 22-Byte-Zeile; bei PC=Opcode+2 waere der Befehl 24 Byte lang.

**Gegenbeleg:** Haupt-SCD-Schleife, selbst disassembliert: 800536fc lw v0,28(s0) / 80053704 lbu v0,0(v0) / 80053708 lui v1,0x1f80 / 8005370c sll v0,v0,2 / 80053714 lw v0,0(v0) / 8005371c jalr v0 — PC zeigt AUF das Opcode-Byte (Tabelle liegt zur Laufzeit im Scratchpad, kopiert @0x800535a0 / @0x80053674 via jal 0x80010778 mit a0=0x1f800000, a1=0x800a74c8, a2=1024). Auch im Evaluator FUN_80053f50 zeigt 28(s2) nach `80053fb0 addiu a1,a1,2 / 80053fb4 sw a1,28(s2) / 80053fb8 lbu v0,0(a1)` AUF das Opcode-Byte. Kette, die +3 beweist: 80057334 lbu v0,3(v1) ; 8005733c sb v0,8(s0) (Byte+3 -> Entity+8) und Binder 8001aac8 lbu s0,8(s1) ; 8001ab04 addiu s0,s0,-16 ; 8001ab48 sll s0,s0,2 (Entity+8 IST der kind fuer den TOC-Index). Empirisch: ROOM7040 Sektion 17 +0x008c fuenf LUECKENLOS 22-Byte-Befehle: 44 00 00 36 …, 44 00 01 37 …, 44 00 02 37 …, 44 00 03 37 …, 44 00 04 37 … — Byte+2 = 0,1,2,3,4 (= `80057204 lb v0,2(v0)` Slot) und Byte+3 = kind. Vollstaendige Feldkarte Opcode 0x44 (relativ zum Opcode-Byte): +2 Slot, +3 kind, +4 u16->Ent+0x10E, +6->Ent+0x106, +7->Ent+0x1FA, +8->Ent+0x1CF, +9->Ent+0x1CE/+0xC, +10/+12/+14 = X/Y/Z, +16 = Yaw. Gegenprobe ROOM2190: 44 00 01 30 02 00 00 1c 00 ff 44 b1 0c fe ad 97 b9 09 -> X=-20156, Y=-500, Z=-26707, Yaw=0x09b9=2489 (<4096). Opcode 0x8E identisch (80057880 lbu a0,3(v0); 80057908 lbu v0,7(v0)).

### [16] Raumbindung — die drei genannten Treffer sind Fehl-Treffer, und die Negativ-Aussage ist falsch
Alle drei zitierten Fundstellen sind Zufallsbytes an nicht-ausgerichteten Positionen; mit dem korrekten Feld-Offset (+3) steht dort kind 0x00 bzw. 0x03 = kein gueltiger TOC-kind. ROOM60C0 Sektion 16 ist ueberhaupt kein Skript, sondern eine 34-Byte-Satztabelle (`.. .. 01 31 00 00` + Koordinaten, Stride 0x22, ab +0x00 wiederholt) — die "0x31" des Ermittlers ist ein Tabellenfeld. Damit faellt auch die Behauptung "Fuer 0x30, 0x33, 0x34 und 0x36 gibt es KEINEN 0x44/0x8E-Spawn in irgendeiner RDT": mit dem korrigierten Offset finden sich saubere Spawns fuer JEDEN dieser kinds.

**Gegenbeleg:** Eigener Scan ueber alle 495 RDTs, Sektionen 16/17, Filter Byte+3 in TOC-kind-Menge, Byte+1<8, Byte+2<8: 0x30 -> ROOM2190 s17 +0x031a und +0x0334 (44 00 01 30 02 00 00 1c 00 ff 44 b1 0c fe ad 97 b9 09), ROOM3030 s17 +0x06dc (44 00 00 30 00 00 03 1c 00 62 65 b9 e8 ea 6a c8 38 04); 0x31 -> ROOM5000 s17 +0x00c2, ROOM5050 s17 +0x0508, ROOM5090 s17 +0x02c8 (44 00 00 31 00 00 00 21 …); 0x33 -> ROOM5090 s17 +0x02f6 (44 00 00 33 00 60 00 22 …); 0x34 -> ROOM6170 s17 +0x00aa (44 00 00 34 00 00 00 26 00 ff 94 ca d0 d5 52 e2 00 04) sowie per 0x8E in ROOMB040 s16 +0x018c (8e 00 00 34 02 00 00 34 …), ROOMB0A0 +0x00da, ROOMB160 +0x00a0, ROOMF020 +0x00a0, ROOMF140 +0x0190; 0x36 -> ROOM7040/ROOMG040 s17 +0x008c. Zum Gegenbeweis der zitierten Stellen: ROOM7040 s17 +0x084a = 44 00 32 00 02 3a … -> Byte+3 = 0x00 (ungueltig); ROOM60C0 s16 +0x00c0 = 8e 00 46 03 04 31 03 00 … -> Byte+3 = 0x03 (ungueltig) und Byte+2 = 0x46 (kein Slot).

### [17] "kind 0x3A = Gegner des letzten Raums" — Raumzuordnung faellt mit [16]
Die Etikettierung stuetzt sich allein auf den widerlegten ROOM7040/ROOMG040-Treffer. Der letzte Raum beider Szenarien spawnt in Wahrheit kind 0x36 plus vier Mal kind 0x37. Fuer 0x3A habe ich in keiner der 495 RDTs einen ausgerichteten 0x44/0x8E-Spawn gefunden; die beiden 0x3A-Kandidaten in ROOM11C0 (s17 +0xaf09/+0xb269) liegen an ungerader Adresse mitten in einer Datentabelle mit dem Wiederholungsmuster `.. .. 78 .. .. 80 00`. Die MESSWERTE des Records (Sektoren, Clips, Bones, MD1) sind dagegen korrekt.

**Gegenbeleg:** ROOM7040 = ROOMG040, Sektion 17, +0x008c bis +0x00e4, fuenf 22-Byte-Befehle im Abstand 22: 44 00 00 36 00 80 00 27 00 ff 00 83 00 00 00 83 00 00 00 00 00 00 | 44 00 01 37 01 00 00 27 00 ff … | 44 00 02 37 … | 44 00 03 37 … | 44 00 04 37 …. Byte+3 = kind (Beleg siehe Punkt [15]).

### [6][7][8][9][10][17][18][19][20] Vertexzahlen sind durchgaengig VERDOPPELT
Der Ermittler hat pro Mesh tri_vertex_count UND quad_vertex_count addiert. In RE2- wie RE1.5-MD1 zeigen beide Header-Felder auf DENSELBEN Vertexblock (identischer Offset UND identischer Count). Jede genannte Vertexzahl ist also genau doppelt so gross wie die tatsaechliche. Die Face-Zahlen (Tri+Quad) sind korrekt.

**Gegenbeleg:** MD1 von kind 0x30 (@EMS 0x572000, dir[7]), alle 19 Mesh-Header selbst gelesen: Mesh0 tv_off 1064 cnt 18 == qv_off 1064 cnt 18; Mesh1 1208/25 == 1208/25; Mesh2 1408/98 == 1408/98; … Mesh18 6576/24 == 6576/24 — SAME=True fuer alle 19. Korrekte Werte: 0x30 = 713 Vertices (nicht 1426), 0x31 = 684 (nicht 1368), 0x33 = 788 (nicht 1576), 0x34 = 1031 (nicht 2062), 0x36 = 878 (nicht 1756), 0x3A = 369 (nicht 738), 0x37 = 78 (nicht 156), RE1.5-0x30 = 599 (nicht 1198), RE1.5-0x36 = 275 (nicht 550).

### [9] "Overlay 45.780 B (groesstes Gegner-Overlay)"
Falsch. Das Zombie-Overlay kind 0x10 ist mit 53.068 B groesser. 45.780 B ist nur das groesste BIRKIN-Overlay.

**Gegenbeleg:** Eigener Vollausdruck der TOC @0x8009adf4 (Datei 0x8B5F4, t_addr 0x80010000), Spalten rec0/rec1 aller 55 belegten kinds: kind 0x10 (0,53068)/(26,53068) gegen kind 0x34 (3215,45780)/(3238,45780). Bestaetigt durch die Dateigroesse von info/re2leon/COMMON/BIN/EMZ0.BIN = 53.068 B.

### FEHLENDE DATEN #1: "CDEMD1.EMS hat KEINE Adressierungstabelle im Repo"
Die zweite Sektor-TOC LIEGT in der RE2-EXE und wird von derselben Funktion gelesen, die der Ermittler fuer die erste zitiert hat — er hat nur den zweiten Zweig von FUN_8001b934 nicht verfolgt. Sie steht @0x8009b880 (Groessen) / @0x8009b880-4 (Sektoren), Zweig-Auswahl ueber DAT_800cfc00 (Stage-Nummer), mit den CD-Datei-Ids 469 (CDEMD0) bzw. 470 (CDEMD1).

**Gegenbeleg:** Selbst disassembliert: 8001b940 lbu v1,-1024(v1) [0x800cfc00] / 8001b948 beq v1,13 -> 469 / 8001b95c beq v1,11 -> 469 / 8001b970 beq v1,14 -> 0x8001ba14 mit `addiu v0,zero,470` / 8001b984 andi v0,v0,0x1 ; 8001b988 bne -> 470. Zweig 470: 8001ba28 lw v0,-18300(at) [=0x8009b884 Groesse] / 8001ba40 lw t0,-18304(at) [=0x8009b880 Sektor] / 8001ba58 lbu v0,-15904(at) [=0x8009c1e0]. Validierung: alle 56 EMD-Records dieser zweiten Tabelle treffen in info/re2leon/PL0/PLD/CDEMD1.EMS auf gueltige Records (u32@+0 == size-0x20 UND u32@+4 == 8, 56/56, 0 Fehler), und der abgedeckte Bereich endet bei 0xaad0f0 = exakt 5467 Sektoren = 11.196.416 B = die Dateigroesse. kind-Belegung weicht gegenueber CDEMD0 nur bei 0x42/0x46/0x4B ab.

### [19] "CDEMD1.EMS traegt denselben Blob @0x00278800"
An 0x00278800 steht ein Blob mit demselben Header und derselben Laenge, aber NICHT derselbe Inhalt: 6.491 Bytes weichen ab, ab 0x1559e — das liegt im dir[6]-Bereich (0x015594, 7688 B), also im Keyframe-Pool von Paar 3 (Opfer-Rig). Die RE1.5-CDEMD1-Fassung von Typ 0x30 hat andere Animationsdaten.

**Gegenbeleg:** Byte-Vergleich re15_port/shared_assets/PSX/EMD/CDEMD0.EMS[0x276800:+226404] gegen CDEMD1.EMS[0x278800:+226404]: erste Abweichung 0x1559e, insgesamt 6491 abweichende Bytes. Kontext a: 08 00 0f 00 50 00 00 00 d1 fc 00 00 …  gegen b: … 3d fd 00 00 …. Identisch in info/Re1.5/PSX/EMD/CDEMD1.EMS (gleiche Groesse 4.739.072, gleicher Fundort).

### [23] / FEHLENDE DATEN: "RE2_Quellcode_Overlays/ … nur STAGE6 hat 4 Dateien"
Jedes der sieben Verzeichnisse enthaelt Dateien; STAGE1-5 und STAGE7 haben je FUN_80100000.c, STAGE6 hat vier. Zusaetzlich existieren STAGE1..7_overlay.c (je 266 B, ausser STAGE6 mit 6070 B). Die inhaltliche Schlussfolgerung (keine Decompilate der Gegner-/Birkin-Overlays) bleibt bestehen, der zitierte Bestand ist aber falsch wiedergegeben.

**Gegenbeleg:** ls RE2_Quellcode_Overlays/STAGE{1..7}: 1,1,1,1,1,4,1 Dateien. ls -la RE2_Quellcode_Overlays/*.c: STAGE1..5,7_overlay.c je 266 B, STAGE6_overlay.c 6070 B.

### [13] Beleg "Eigener Scan … 0 Treffer" fuer die lo16-Immediates
Der Scan-Beleg stimmt so nicht: es gibt VIER Instruktionen mit dem Immediate 0xd444 in der EXE. Sie gehoeren nur nicht zu lui 0x800a. Die AUSSAGE (Pfad-Strings unreferenziert) haelt — der Beleg, wie formuliert, nicht.

**Gegenbeleg:** Eigener EXE-weiter Scan: `addiu a1,a1,-11196` (0x24a5d444) @0x8006ce8c, 0x8006d1f8, 0x80071d0c, 0x80072904 — jedes Mal mit vorangehendem `lui a1,0x8007` -> Zieladresse 0x8006d444, nicht 0x8009d444. Immediate 0xd410: 0 Treffer. Zeigerworte 10 d4 09 80 / 44 d4 09 80: 0 Treffer. gp = 0x00000000 laut EXE-Header (Offset 0x14), also auch keine gp-relative Adressierung.

### [22] "der Lader … wuerde die Birkin-Bank heute parsen" — Bedingung verschwiegen
Der Parser ist kind-generisch, aber der Aufrufpfad ist es nicht: unmittelbar nach dem erfolgreichen Laden laeuft BEDINGUNGSLOS pc_enemy_hybrid_re15_models(type, eb) und tauscht Mesh/Textur/Bind-Laengen gegen RE1.5 zurueck — der reine RE2-Modell-Zweig ist laut Kommentar "tot und weg". Fuer 0x31/0x33/0x34/0x3A gibt es ausserdem gar kein RE1.5-Gegenstueck, auf das der Hybrid zurueckbauen koennte.

**Gegenbeleg:** re15_port/platform/pc/main.c:717-724: `if (pc_enemy_load_re2(type, eb)) { … pc_enemy_hybrid_re15_models(type, eb); return; }` mit Kommentar "Die Stufe ist auf Nutzer-Entscheidung entfernt, also laeuft der Hybrid BEDINGUNGSLOS — der reine RE2-Modell-Zweig ist damit tot und weg." RE1.5-Typliste s_ems_order (re15_port/engine/src/re15_ems.c:64-68) enthaelt 0x30 und 0x36, aber weder 0x31 noch 0x33/0x34/0x3A.

### [5] "dir[2] traegt die Bone-STRUKTUR" und "Paar 3 durchgaengig 15 Bones / kf 80" — abgeleitet bzw. zu absolut
(a) Der zitierte Binder FUN_8001aaa8 speichert nur acht Zeiger; dass ausgerechnet dir[2] die verbindliche Bone-Struktur fuer alle drei Paare traegt, steht dort NICHT — das ist aus dem Port-Kommentar re2_ems.c abgeschrieben, nicht disassembliert. (b) "durchgaengig" stimmt nicht: von 55 EMD-Records haben 21 dir[5]/dir[6] als 4-Byte-Stub.

**Gegenbeleg:** (a) 8001ab88 lw v1,0(s2) / 8001ab90 addu s0,s2,v1 / 8001ab94..8001ac14: acht Mal `lw v1,N(s0)` + `addu v1,s2,v1` + `sw v1,…(s1)` — reine Zeiger-Fixups, keine Struktur-Semantik. (b) Eigener Zensus ueber alle 55 EMD-Records (dir[6]-EMR, u16@+4/+6): 15/80 bei 0x10,0x11,0x12,0x13,0x15,0x16,0x17,0x18,0x1e,0x1f,0x20,0x21,0x22,0x23,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x30,0x31,0x33,0x34,0x36,0x37,0x39,0x3a,0x4a; STUB (4 B) bei 0x24,0x25,0x2f,0x3b,0x3e,0x3f,0x40,0x41,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4f,0x50,0x51,0x54,0x55,0x58,0x59,0x5a.

### [1] "EMOVL10_S1.BIN = zweite Kopie" — Slot/Record-Zuordnung vertauscht beschrieben
EMOVL10_S1.BIN ist rec0 (Sektor 0), waehrend EMZ0.BIN/EMZ0_d1.BIN/EMOVL10_S0.BIN rec1 (Sektor 26) sind. Der Lader bildet den Record-Index als 1 - slot, also S0 -> rec1 und S1 -> rec0. Bei kind 0x21 hat der Ermittler das richtig hingeschrieben, bei 0x10 nicht.

**Gegenbeleg:** md5-Vergleich: EMS[0*2048:+53068] = 6123ab3c5aee613caac08b2adbd6ea96 == EMOVL10_S1.BIN; EMS[26*2048:+53068] = 7f7a39e651064e07c95830db1bebdb89 == EMZ0.BIN == EMZ0_d1.BIN == EMOVL10_S0.BIN. Formel im Lader: 8001b870 addiu v1,t1,-16 / 8001b874 sll v1,v1,2 / 8001b878 addiu v1,v1,1 / 8001b87c subu v1,v1,a3 / 8001b880 sll a0,v1,3.

## Gesamturteil der Gegenpruefung
Der Datenteil des Strangs (TOC, Record-Identitaeten, EMD/EMR/EDD/MD1/TIM-Struktur, Lader-Disasm) haelt der Gegenpruefung stand — ich habe jede zitierte Adresse selbst disassembliert und jede Groesse selbst gemessen; nichts davon war abgeschrieben. Der SPIEL-Teil faellt komplett. Der Ermittler hat die SCD-Feldlage um 2 Bytes verschoben, weil er den PC-Stand aus dem Bedingungs-Evaluator FUN_80053f50 statt aus dem Haupt-Interpreter @0x800536fc genommen hat; daraus folgen drei falsche Raumbindungen (ROOM60C0/ROOM7040/ROOMG040 sind allesamt Byte-Zufallstreffer an nicht ausgerichteten Positionen) und die falsche Negativ-Aussage, 0x30/0x33/0x34/0x36 haetten gar keinen Spawn. Mit dem korrekten Offset (+3, bewiesen ueber Byte+3 -> Entity+8 -> Binder-kind sowie ueber fuenf lueckenlose 22-Byte-Befehle in ROOM7040) ergibt sich: 0x30 -> ROOM2190/ROOM3030, 0x31 -> ROOM5000/5050/5090, 0x33 -> ROOM5090, 0x34 -> ROOM6170 (+ 8E in B040/B0A0/B160/F020/F140), 0x36 + 4x 0x37 -> ROOM7040/ROOMG040, 0x3A -> kein ausgerichteter Spawn gefunden. Zweitens sind ALLE Vertexzahlen des Strangs verdoppelt (tri- und quad-Vertexblock sind in jedem Mesh derselbe Block). Drittens ist die wichtigste Luecken-Meldung falsch: die CDEMD1-Sektor-TOC fehlt nicht, sie steht @0x8009b880 und adressiert alle 56 Records der Datei sauber (5467 Sektoren = exakte Dateigroesse) — Szenario B ist damit sehr wohl byte-true adressierbar. Dazu kommen kleinere Fehler: 45.780 B ist nicht das groesste Gegner-Overlay (Zombie 0x10 = 53.068 B), der RE1.5-0x30-Blob in CDEMD1 ist NICHT derselbe (6.491 Byte Unterschied ab 0x1559e im Opfer-Keyframe-Pool), RE2_Quellcode_Overlays/ hat in allen sieben Stage-Verzeichnissen Dateien, Paar 3 ist bei 21 von 55 Records ein Stub, und der Port-Pfad baut die RE2-Bank unmittelbar nach dem Laden bedingungslos auf RE1.5-Meshes zurueck.

---

# Strang: boss-sound

## Kernaussage
(1) Die Sound-Baenke der Endkampf-Raeume liegen NICHT in COMMON/SOUND, sondern EINGEBETTET im RDT: Kopf +0x08=EDT, +0x0C=VH, +0x10=VB (Bank 1 bei allen fuenf Raeumen = 0); COMMON/SOUND enthaelt nur die globalen Baenke ARMS01..13 / CORE00..15, ENEMSE.VBS und 114 *.BGM — es gibt fuer die Raum-Baenke KEINE extrahierten *.wav (0 Stueck unter room70*/). (2) Die Endkampf-BGM ist MAIN23.BGM (Layer SUB_2C / SUB_2F / SUB_30, ROOM7020-Zwischenstufe MAIN11); gestartet wird sie NICHT von Sce_bgm_control — das ist in RE2 Opcode 0x51 (6 B, Handler 0x80057e20 → 0x8005b6f0, 6-Kommando-Sprungtabelle @0x80011608) und macht nur Play/Stop/Fade —, sondern von Opcode 0x57 (8 B, Handler 0x80057e98 → FUN_8005b9f4 @0x8005b9f4), der {mainId,subId} in das RAM-Array 0x800d46d0 schreibt, das der Raumwechsel FUN_8005a444 @0x8005a474-0x8005a4fc liest und daraus FUN_8005a97c (MAIN) / FUN_8005ae50 (SUB) laedt; CD-Datei-Index ueber die u16-Tabellen 0x800a8168 (MAIN) / 0x800a81e8 (SUB). (3) Gegner-SE laufen ueber den gemeinsamen EXE-Aufruf FUN_8005bd6c (ENEM-Bank, Bank 3 fest verdrahtet: 0x800dbb84/0x800d75ac/0x800d4c4b) bzw. FUN_8005ba28 (4-Bank-Variante, Bank = a0>>24); gekeyt wird auf ZWEI Arten — fester Zustands-Einstieg mit Cooldown-Byte Entity+0x239 (EMZ0 @0x8010278c/0x801027bc) UND EDD-Frame-Event-Bit (EMD0G_MOD0 @0x80100510-0x8010052c: Bit 0x40000 → SE 8). Die ENEM-Bank pro Raum steht fest: ROOM7000=Bank 4, ROOM7020=Bank 26, ROOM7040=Bank 25, ROOM7030=0xFF (keine); die Overlay-BINs der Endkampf-Gegner fehlen im Repo, die zugehoerigen Bank-Daten in ENEMSE.VBS sind aber vorhanden und von mir verifiziert. (4) Die Prioritaets-/Stimmen-Vergabe ist RE2 FUN_8005c92c @0x8005c92c-0x8005c968 — bis auf den Gleichstand-Tiebreak instruktionsgleich zu RE1.5 FUN_80045a18 @0x80045a34-58; Unterschied: Prio-Array 0x800d4ca0 mit Stride 2 statt 0x800b22cc mit Stride 1. (5) Der Port hat FUN_8005bd6c + FUN_8005c92c + den ENEMSE-TOC @0x800a7b1c bereits byte-true, aber die Bank ist auf 6 (Hund) festgenagelt und der Bank-Resolver FUN_80052b38/Paartabelle 0x800a7400 fehlt; die BGM-Seite kennt Stage 7 gar nicht (ss_bgm_entry lehnt stage>5 ab, Tabelle = RE1.5-statisch UNK_80074828) und den RE2-Opcode 0x57 als BGM-Tabellen-Setzer gibt es nicht (im Port ist 0x57 korrekt der RE1.5-Fade-Schritt).

## Befunde

### RDT-Kopf snd0/snd1 der Endkampf-Raeume  [byte-belegt]
Alle fuenf ROOM70x0.RDT fuehren GENAU EINE Sound-Bank (snd0). Kopf-Felder: +0x08=EDT, +0x0C=VH, +0x10=VB; +0x14/+0x18 (Bank 1 EDT/VH) sind 0, +0x1C spiegelt +0x08. Belegt durch Differenzrechnung gegen die bereits entpackten Dateien: 0x58ec-0x582c=0xC0=192=snd0.edt; 0x650c-0x58ec=0xC20=3104=snd0.vh.

**Beleg:** info/re2leon/PL0/RDT/ROOM7000.RDT @0x00: 00 0f 06 00 00 00 00 4e | 2c58 0000 ec58 0000 0c65 0000 0000 0000 0000 0000 2c58 0000. ROOM7010 @0x08: c462/6384/71a4; ROOM7020: 5c90/5d50/6b70; ROOM7030: ff08/ffc8/010de8; ROOM7040: 6018/60d8/6ef8 (alle u32 LE). Gegenprobe Dateigroessen room7000/snd0.{edt,vh,vb} = 192/3104/89792.

**Port-Stand:** re15_port/include/re15_rdt.h:166 kennt genau dieses Layout (snd0 EDT@0x08/VH@0x0c/VB@0x10, snd1 @0x14/0x18/0x1c) — die Struktur ist da, RE2-RDTs werden aber nicht geladen (shared_assets/PSX = RE1.5).

### VAB-Kennzahlen der fuenf Endkampf-Baenke  [byte-belegt]
snd0 je Raum: ROOM7000 nProg=1 nTone=15 nVAG=11, vh 3104 B, vb 89792 B; ROOM7010 2/21/17, 3616/81472; ROOM7020 2/22/18, 3616/100832; ROOM7030 2/18/15, 3616/105824; ROOM7040 2/24/17, 3616/72320. Summe der VAG-Groessen aus der 512-B-Tabelle == VB-Groesse in ALLEN fuenf (Konsistenzprobe bestanden).

**Beleg:** snd0.vh @0x00 'pBAV', @0x12 nProg-1, @0x14 nTone-1, @0x16 nVAG-1; VAG-Groessentabelle @ 32+2048+32*16*nProg (=0xa20 bei 1 Prog, 0xc20 bei 2). ROOM7000 vagsizes = [48,2560,4288,3360,3296,3840,3424,51840,2496,5152,9488] = 89792 = Dateigroesse snd0.vb.

**Port-Stand:** vab_common.c / re15_vab_t parst genau dieses Format (Port nutzt es fuer RE1.5-snd0/snd1 und fuer ENEMSE-Baenke).

### COMMON/SOUND enthaelt KEINE Raum-Baenke  [byte-belegt]
info/re2leon/COMMON/SOUND fuehrt nur globale Baenke: 19x ARMS01..ARMS13.{EDH,VB} und 13x CORE00,CORE01,CORE0B..CORE15.{EDH,VB}, dazu ENEMSE.VBS (6.694.912 B), 114 *.BGM und ROOM800.SND. Extrahierte wavs: ARMS 99, CORE 111, ENEMSE 980, ROOM800 5, Rest MAIN*/SUB_* (gesamt 1954 wav). Zu den ROOM70x0-Baenken gehoert davon KEINE einzige wav — unter room7000..room7040 liegen 0 wav-Dateien.

**Beleg:** ls info/re2leon/COMMON/SOUND: *.EDH = ARMS01..13, CORE00,01,0B..15 (32 Stueck); find room7000 room7010 room7020 room7030 room7040 -name '*.wav' | wc -l = 0.

**Port-Stand:** re15_port/shared_assets/RE2/ enthaelt nur CDEMD0.EMS und ENEMSE.VBS — keine RE2-RDTs, keine RE2-BGM, keine ARMS/CORE.

### EDT-Record-Format (SE-Tabelle)  [byte-belegt]
Der EDT ist ein Array aus 4-Byte-Records, Index = SE-Id (192 B = 48 Records in allen fuenf Raeumen). 0xFFFFFFFF = stumm. b0&0x80 -> VAB-Id-Override (b0&0x7f), sonst globale Id; b1&0x7f = Programm; b2>>4 = Tone, b2&0xf = Prio-Nibble; b3&0x1f = SPU-Kanal, b3>>5 = Anzahl ZUSAETZLICHER konsekutiver Tone/Kanal-Layer.

**Beleg:** FUN_8005bd6c: @0x8005bddc lbu v1,0(s0); @0x8005bde4 andi 0x80; @0x8005bdf0 andi s3,v1,0x7f; @0x8005bdf4 lbu v0,3(s0); @0x8005bdf8 lbu v1,2(s0); @0x8005bdfc andi s1,v0,0x1f; @0x8005be00 andi s5,v1,0xf; @0x8005be34 srl t0,v0,4; @0x8005be38 andi t2,v1,0x7f; @0x8005be44 srl t1,v0,5; Tone-Adresse @0x8005be40/48/4c: (t2<<9)+(t0<<5)+2080 auf VH-Basis 0x800d75ac.

**Port-Stand:** audio_pc.c:1085-1092 dokumentiert und implementiert exakt diese Dekodierung inkl. b3>>5-Layer-Schleife.

### Belegte SE-Ids in den fuenf Raum-Baenken  [byte-belegt]
ROOM7000: Ids 10,11,22,26..31,37. ROOM7010: 10..13,20,21,23..31,34,35,36. ROOM7020: 10..19,22,26..31,41,42. ROOM7030: 10..15,22,23..31. ROOM7040: 10..12,20,21,22,26..31,34..36,41,42. Ids 26..31 sind in ALLEN fuenf identisch (prog0, tone 4,5,6,1,2,3; Prio 3; Kanal 22) = der Sechser-Satz Spieler-Schritte je Bodenmaterial.

**Beleg:** info/re2leon/PL0/RDT/room7000/snd0.edt @0x68..0x7f: 00 00 43 16 | 00 00 53 16 | 00 00 63 16 | 00 00 13 16 | 00 00 23 16 | 00 00 33 16 — bytegleich in room7010/20/30/40 snd0.edt an denselben Record-Indizes 26..31.

**Port-Stand:** re15_audio_footstep (audio_pc.c, FUN_80045630-Pfad) nutzt in RE1.5 dieselbe snd0-EDT-Aufloesung; die RE2-Raum-Baenke werden nicht geladen.

### RE2 Sce_bgm_control = Opcode 0x51, 6 Byte  [byte-belegt]
Dispatch-Tabelle der SCD-VM liegt @0x800a74c8 (143 Eintraege). Eintrag [0x51] = 0x80057e20. Der Handler liest 5 u8 ab pc+1, packt sie zu a0 = (b1<<28)|(b2<<24)|(b3<<16)|(b4<<8)|b5, ruft 0x8005b6f0 und schiebt den pc um 6 weiter. RE1.5 nutzt fuer dieselbe Funktion 0x54 — der Opcode ist also verschoben, nicht identisch.

**Beleg:** 0x800a74c8[0x51] = 0x80057e20 (u32 LE). 0x80057e3c-0x80057e6c: lbu a0,1(s0); lbu v0,2(s0); lbu v1,3(s0); sll a0,a0,28; sll v0,v0,24; or; sll v1,v1,16; or; lbu v0,4(s0); lbu v1,5(s0); sll v0,v0,8; or; jal 0x8005b6f0; or a0,a0,v1. @0x80057e78 addiu s0,s0,6; @0x80057e7c sw s0,28(s1). Quergeprobt an room1000/scd/sub04.c 'Sce_bgm_control(0,0,1,120,51)' vs sub04.scd @0x0c: 51 00 00 01 78 33.

**Port-Stand:** scd_vm.c:347 mappt 0x54 -> op_sce_bgm_control (RE1.5-korrekt); 0x51 ist im Port NICHT als bgm_control belegt. Kommentar scd_vm.c:4086 nennt die RE2-Verschiebung bereits richtig.

### BGM-Kommando-Sprungtabelle  [byte-belegt]
FUN_8005b6f0 zerlegt a0: s0=a0>>28 (Bank 0=MAIN, 1=SUB), a2=(a0>>24)&0xf = KOMMANDO, s4=a0>>16, s2=a0>>8. Kommando < 6, Sprungtabelle @0x80011608 mit 6 Zielen: cmd0=0x8005b974, cmd1=0x8005b7b0, cmd2=0x8005b82c, cmd3=0x8005b898, cmd4=0x8005b910, cmd5=0x8005b948. Vorher zwei Abbruch-Gates: 0x800d75c1[bank*8]==0xff und 0x800d75c2[bank*8]==-1 (kein SEQ-Handle).

**Beleg:** 0x8005b71c srl s0,a0,28; 0x8005b720/24 srl v0,a0,24 + andi a2,v0,0xf; 0x8005b78c sltiu v0,v1,0x6; 0x8005b798-a8 lui at,0x8001 / addu / lw v0,5640(at) / jr v0. Tabelle @0x80011608 roh: 74 b9 05 80 | b0 b7 05 80 | 2c b8 05 80 | 98 b8 05 80 | 10 b9 05 80 | 48 b9 05 80.

**Port-Stand:** re15_audio_seq_ctl(slot,op) (re15_audio.h) bildet die RE1.5-Variante FUN_80044da4 ab (op 1..5); die RE2-Argumentpackung (Nibble-Feld a0>>24) und die RE2-Zieladressen sind nicht abgebildet.

### Sce_bgm_control-Aufrufe in den Endkampf-Raeumen  [byte-belegt]
Kein einziger 0x51-Aufruf laedt einen Track — alle sind Play/Stop/Fade. ROOM7000: sub07 (0,2,0,91,65) und (1,2,0,91,65). ROOM7010: KEINER. ROOM7020: sub07 (0,2,0,0,0); sub16 (0,1,0,0,0),(1,1,1,127,64),(1,0,1,127,64); sub19 fuenf Stueck (1,0,1,60/70/80/90,64) und (1,2,1,0,64). ROOM7030: sub08 (0,0,2,1,65); sub10 (0,2,0,91,65),(1,2,0,91,65); sub12 sechs; sub13 zwei. ROOM7040: sub06 drei; sub07 zwei; sub08 zwei.

**Beleg:** Rohbytes, z.B. info/re2leon/PL0/RDT/room7000/scd/sub07.scd @0x012: 51 00 02 00 5b 41 und @0x018: 51 01 02 00 5b 41; room7020/scd/sub19.scd @0x018: 51 01 00 01 3c 40. Filter: b1 in {0,1} (Bank) und b2 < 6 (Kommando-Bereich laut sltiu 0x6 @0x8005b78c).

**Port-Stand:** nicht geprueft — der Port fuehrt keine RE2-RDTs aus.

### RE2 Opcode 0x57 = Sce_bgmtbl_set (die eigentliche Track-Wahl)  [byte-belegt]
Opcode 0x57, 8 Byte, Handler 0x80057e98. a0 = (u16@+2 <<16) | u16@+4 | u16@+6, dann FUN_8005b9f4. Dort: stage=a0>>24, Basis = 0x800d46d0 + 0x800a80af[stage]*2, Offset = ((a0>>16)&0xff)*2 = room*2, gespeichert wird die untere u16 (LE: Byte0=mainId, Byte1=subId). Das Array 0x800d46d0 ist im EXE-Abbild NULL, also reines RAM.

**Beleg:** 0x800a74c8[0x57]=0x80057e98. 0x80057eb0-c8: lhu a0,2(v0); lhu v1,4(v0); lhu v0,6(v0); sll a0,a0,16; or a0,a0,v1; jal 0x8005b9f4; or a0,a0,v0; @0x80057ed4 addiu v1,v1,8. FUN_8005b9f4 @0x8005b9f4-0x8005ba24: srl v0,a0,24; lbu v1,-32593(at) [=0x800a80af]; addiu v0,v0,18128 [lui 0x800d -> 0x800d46d0]; sll v1,v1,1; addu; srl v0,a0,15; andi v0,v0,0x1fe; addu; sh a0,0(v1). Ghidra: '8005ba08 d0 46 42 24 addiu v0,v0,0x46d0' und Label DAT_800d46d0 mit XREF FUN_8005a444:8005a470.

**Port-Stand:** scd_vm.c:105/258/1683 belegt 0x57 als RE1.5-Fade-Schritt (op_fade_adjust) und widerruft den Namen 'Sce_bgmtbl_set' ausdruecklich als RE2-only — das deckt sich mit diesem Befund, heisst aber: der RE2-Mechanismus existiert im Port nicht.

### Raum -> BGM-Slot: Lese- und Ladepfad  [byte-belegt]
FUN_8005a444 (gerufen von FUN_80049e48 @0x80049ebc) berechnet ptr = 0x800d46d0 + 0x800a80b0[stage]*2 + room*2, legt ihn in 0x800dfd64 ab, liest s0=ptr[0] (MAIN-Id) und s1=ptr[1] (SUB-Id). Weicht die MAIN-Id von der geladenen (0x800d7856) ab, wird @0x8005a4fc FUN_8005a97c gerufen = MAIN laden+abspielen; FUN_8005ae50 macht dasselbe fuer SUB. Stage-Basis-Tabelle 0x800a80b0 (u8, 0-basiert) = [0,31,61,77,96,108,134,...] -> Stage 7 beginnt bei Raum-Index 134.

**Beleg:** 0x8005a474-0x8005a4bc: lh v0,0x800d481c (stage); lh v1,0x800d481e (room); lbu v0,-0x7f50(at) [=0x800a80b0]; sll v1,1; sll v0,1; addu v0,v0,a0 [a0=0x800d46d0]; sw v0,0x800dfd64; addu v0,v0,v1; sw v0,0x800dfd64; lbu s0,0(v0); lbu s1,1(v0). 0x8005a4e8 lbu v0,0x800d7856; 0x8005a4f4 beq v1,v0,0x8005a534; 0x8005a4fc jal 0x8005a97c. Tabelle 0x800a80b0 roh: 00 1f 3d 4d 60 6c 86 00.

**Port-Stand:** audio_pc.c:2102 ss_bgm_entry(stage,room): 'if (stage < 0 || stage > 5) return -1;' — Stage 7 faellt heraus. SS_STAGE_OFF[6]={0x00,0x26,0x32,0x41,0x4d,0x62} und SS_BGMTBL[106] sind die RE1.5-STATISCHE Tabelle UNK_80074828 aus PSX.EXE, kein RAM-Array.

### BGM-Dateiindex-Tabellen MAIN/SUB  [byte-belegt]
FUN_8005a97c laedt die MAIN-Datei ueber den CD-Index *(u16*)(0x800a8168 + (mainId & 0x3f)*2), FUN_8005ae50 die SUB-Datei ueber *(u16*)(0x800a81e8 + (subId & 0x3f)*2). Je 0x40 Eintraege (0x800a81e8-0x800a8168 = 0x80). Sentinel 0xFF = kein Track (Frueh-Return). MAIN wird nach 0x801f2e00 geladen, SUB nach 0x801ece00; SUB oeffnet ZWEI SsSeq-Handles (0x800d75ca und 0x800d75d2).

**Beleg:** RE2_Quellcode_V2/FUN_8005a97c.c: FUN_80012fb8(*(undefined2 *)(&DAT_800a8168 + (*DAT_800dfd64 & 0x3f) * 2), uVar2, 1, "              MAIN BGM"); RE2_Quellcode_V2/FUN_8005ae50.c: (&DAT_800a81e8 + (*(byte *)(DAT_800dfd64 + 1) & 0x3f) * 2) mit "              SUB BGM", zwei SsSeqOpen/SsSeqPlay-Paare. Dateibestand: MAIN00..MAIN3A (58, MAIN32 fehlt) + SUB_00..SUB_37 (56) = 114 in info/re2leon/COMMON/SOUND.

**Port-Stand:** audio_pc.c modelliert die zweite SUB-Sequenz korrekt (s_ss_sub2, 'SUB-Container fuehren eine ZWEITE Sequenz', Trailer-Offsets @size-8/-12/-16 nach FUN_80044564/80044774) — das ist die RE1.5-Fassung derselben Architektur.

### BGM des Endkampfs: konkrete Tracks  [byte-belegt]
Die einzigen 0x57-Zuweisungen mit stage=7 im gesamten RDT-Bestand: ROOM7020/sub16 setzt Raum 02 -> MAIN 0x11 (17) + SUB 0x2F (47) und Raum 03 -> MAIN 0x23 (35) + SUB 0x2C (44); ROOM7030/sub00 setzt Raeume 00,01,02,04 -> MAIN 0x23 + SUB 0x2C; ROOM7030/sub02 setzt Raum 03 -> MAIN 0x23 + SUB 0xFF (kein SUB) und Raum 04 -> MAIN 0x23 + SUB 0x30 (48). Also: MAIN23.BGM (26296 B) ist die Endkampf-Musik, Layer SUB_2C.BGM (110248 B), SUB_2F.BGM (130980 B), SUB_30.BGM (227836 B); MAIN11.BGM (120464 B) ist die Vorstufe in ROOM7020.

**Beleg:** room7030/scd/sub00.scd @0x014: 57 00 01 07 23 2c 40 80 -> a0 = 0x07010000 | 0x2c23 | 0x8040 = 0x0701AC63 -> ptr[0]=0x63 (&0x3f = 0x23 = 35), ptr[1]=0xAC (&0x3f = 0x2C = 44). room7020/scd/sub16.scd @0x036: 57 00 02 07 11 2f 00 80 -> low=0xAF11 -> main 0x11, sub 0x2F. room7030/scd/sub02.scd @0x076: 57 00 03 07 23 ff 00 00 -> low=0xFF23 -> sub 0xFF = keiner (FUN_8005ae50: 'if (*(byte *)(DAT_800dfd64 + 1) == 0xff) return 0xffffffff').

**Port-Stand:** Nicht abbildbar: ss_bgm_entry kennt Stage 7 nicht, und die Container MAIN23/SUB_2C/2F/30 liegen nicht unter re15_port/shared_assets.

### Gemeinsamer SE-Aufruf: FUN_8005bd6c (ENEM) und FUN_8005ba28 (4 Baenke)  [byte-belegt]
FUN_8005bd6c(a0=SE-Id, a1=Entity) ist die auf Bank 3 (ENEM) fest verdrahtete Fassung: EDT-Zeiger 0x800dbb84, VH-Zeiger 0x800d75ac, VAB-Id 0x800d4c4b. FUN_8005ba28(param_1, param_2) ist die allgemeine 4-Bank-Fassung: Bank = param_1>>24 indiziert (&DAT_800dbb78)[bank] (EDT), (&DAT_800d75a0)[bank] (VH), (&DAT_800d4c48)[bank] (VAB-Id); SE-Id = (param_1>>16)&0xff. Entity-Flagwort bit 0x2000 addiert 16 auf die SE-Id.

**Beleg:** 0x8005bd90-a4: lw v0,0(s2); andi 0x2000; beq -> addiu s4,a0,16. 0x8005bdb0-c0: sll v0,v0,2; lw s0,-17532(0x800e) = [0x800dbb84]; addu s0,s0,v0; lw v0,0(s0). RE2_Quellcode_V2/FUN_8005ba28.c: uVar10 = param_1 >> 0x18; (&DAT_800dbb78)[uVar10]; iVar11 = (&DAT_800d75a0)[uVar10] + uVar16*0x200 + uVar14*0x20 + 0x820. Init FUN_80059654: DAT_800dbb78 = &DAT_801fb700; DAT_800dbb7c = &DAT_801faa00; DAT_800dbb80 = 0; DAT_800dbb84 = 0.

**Port-Stand:** audio_pc.c:1085ff. implementiert FUN_8005bd6c byte-true (re15_audio_re2_enemy_se, inkl. flag2000 -> +0x10). Die 4-Bank-Fassung FUN_8005ba28 ist nur in Kommentaren erwaehnt (enemy_ai_common.c:703/1101), nicht als Bank-Selektor implementiert; der Port hat stattdessen den RE1.5-Selektor FUN_80045024 mit 6 Baenken (re15_audio.h).

### Keying der Gegner-SE: zwei belegte Mechanismen  [byte-belegt]
(a) FESTER ZUSTANDS-EINSTIEG mit Cooldown-Byte: der Zombie prueft Entity+0x239, spielt bei 0 die SE und setzt +0x239 = 150. (b) ANIM-FRAME-EVENT: der Hund liest das u32-Kopfwort des AKTUELLEN EDD/RBJ-Frames ueber Entity+0x178 und feuert SE 8, wenn Bit 0x00040000 gesetzt ist. Es gibt also keine separate 'Anim-Frame-Tabelle', sondern Event-Bits IM Frame-Wort plus feste FSM-Einstiege.

**Beleg:** EMZ0.BIN @0x8010278c: lbu v0,569(s1); @0x80102794 bne v0,zero,0x801027c4; @0x8010279c jal 0x80015fe8 (rand); @0x801027ac addiu a0,zero,11 / @0x801027b0 addiu a0,zero,10; @0x801027b4 jal 0x8005bd6c; @0x801027bc addiu v0,zero,150; @0x801027c0 sb v0,569(s1). EMD0G_MOD0.BIN @0x80100510: lw v0,376(s0); @0x80100518 lw v0,0(v0); @0x8010051c lui v1,0x4; @0x80100520 and v0,v0,v1; @0x80100524 beq v0,zero,0x80100534; @0x80100528 addiu a0,zero,8; @0x8010052c jal 0x8005bd6c.

**Port-Stand:** enemy_ai_re2_dog.c:208/2150-2151 implementiert genau das (re2d_frame_word(e) & 0x40000u -> re2d_se(8)) und Z.227 zusaetzlich Bit 0x08000000 -> SE = Wort>>28.

### Vollstaendiger SE-Id-Zensus der vorhandenen RE2-Gegner-Overlays  [byte-belegt]
EMZ0.BIN/EMOVL10_S0.BIN (identisch, 43 Aufrufe): Ids {2,3,4,5,7,8,9,10,11,12,13}. EMD0G_MOD0.BIN (14 Aufrufe): {1,3,7,8,10}. EMS25.BIN (9): {1,3,6,10}. EMS26.BIN (1): {7}. EMOVL21_S0.BIN (25): {1,2,3,4,5,6}. (Restliche Aufrufe setzen a0 nicht als Immediate, sondern registerabhaengig.)

**Beleg:** Bytemuster jal 0x8005bd6c = 5b 6f 01 0c (LE), Offset = addr-0x80100000. Trefferliste u.a. EMZ0 @0x801014e0 (a0=11, gesetzt @0x801014dc), @0x801022dc (a0=10), @0x80102b58 (a0=8, gesetzt @0x80102b44), @0x8010843c (a0=11 oder 13, Rand-Auswahl @0x80108434/38); EMD0G_MOD0 @0x8010052c (a0=8), @0x801033d8 (a0=1), @0x8010358c (a0=10), @0x801041cc (a0=7).

**Port-Stand:** Der Port bildet die vier RE2-Gegner ab (enemy_ai_re2_{zombie,dog,crow,spider}.c) — genau die, deren Overlays im Repo liegen.

### ENEM-Bank-Wahl je Raum (FUN_80052b38 / Paartabelle 0x800a7400)  [byte-belegt]
Die ENEM-Bank-Nummer DAT_800d424b ist der ZEILENINDEX in der 2-Byte-Paartabelle 0x800a7400, verglichen wird die SOUND-ID aus Sce_em_set-Record +7 (nicht der Gegner-kind). Zeilenauszug: 0=(03,00) 4=(2e,00) 6=(0c,00) 25=(27,00) 26=(1a,00) 31=(00,20) 72=(34,00) 73=(ff,00)=Ende. Ist die u16 der Raum-Sound-Ids 0, wird DAT_800d424b = 0xFF gesetzt und KEINE Bank geladen.

**Beleg:** 0x80052b38-0x80052bf0: lui a1,0x800e / addiu a1,a1,-29488 [=0x800d8cd0]; lhu v0,0(a1); beq v0,zero,0x80052be8; addiu a2,a2,29696 [=0x800a7400]; lbu a0,0(a2); lbu a3,0(a1); Schleife @0x80052b7c mit addiu t0,t0,1; @0x80052be8-f0 addiu v0,zero,255 / sb v0,16971(at) [=0x800d424b]; @0x80052c18 sb t0,0(v1). Tabelle 0x800a7400 roh: 03 00 05 00 06 00 24 00 2e 00 2f 00 0c 00 0d 00 0e 00 33 00 0f 00 10 00 12 00 13 00 30 00 32 00 14 00 16 00 17 00 18 00 1c 00 21 00 23 00 22 00 26 00 27 00 1a 00 28 00 00 1d ... 34 00 ff 00.

**Port-Stand:** enemy_ai_re2_dog.c:95-130 hat diese Tabelle bereits korrekt als SOUND-ID-Tabelle identifiziert (Record+7 -> Entity+0x1FA, Vergleich @0x80052C48) und daraus Bank 6 fuer den Hund abgeleitet — aber es gibt KEINEN Laufzeit-Resolver: RE2DOG_ENEMSE_BANK ist die Konstante 6 (enemy_ai_re2_dog.c:130), gesetzt in main.c:697.

### ENEM-Baenke der Endkampf-Raeume — konkret und im Repo vorhanden  [byte-belegt]
Sce_em_set-Record +7 (Sound-Id) der Stage-7-Raeume: ROOM7000 = 0x2E (5x kind 0x17) -> Zeile 4 -> Bank 4; ROOM7020 = 0x1A (1x kind 0x2B) -> Zeile 26 -> Bank 26; ROOM7040 = 0x27 (kind 0x36 + 4x kind 0x37) -> Zeile 25 -> Bank 25; ROOM7030 = 0x00 (3x kind 0x4F) -> u16 == 0 -> DAT_800d424b = 0xFF, keine ENEM-Bank; ROOM7010 = kein Sce_em_set gefunden. Alle drei Baenke sind in ENEMSE.VBS vorhanden und von mir geoeffnet: Bank 4 EDT@0x04F000 (0xCA8) / VBD@0x050000 (72592 B), nProg=1 nTone=15 nVAG=15, live SE-Ids 0..13. Bank 25 EDT@0x1F2800 (0xEA8) / VBD@0x1F3800 (96784 B), nProg=2 nTone=19 nVAG=15, live Ids 0..15. Bank 26 EDT@0x20B800 (0xCA8) / VBD@0x20C800 (55632 B), nProg=1 nTone=15 nVAG=12, live Ids 0..9 und 12.

**Beleg:** TOC 0x800a7b1c, 16 B je Bank = [u32 EDT-Groesse][u24 Sektor][u8][u32 VBD-Groesse][u24 Sektor][u8], Byte-Offset = Sektor*0x800. Bank 4 @0x800a7b5c: a8 0c 00 00 9e 00 00 00 90 1b 01 00 a0 00 00 75. Bank 25 @0x800a7cac: a8 0e 00 00 e5 03 00 00 10 7a 01 00 e7 03 00 2a. Bank 26 @0x800a7cbc: a8 0c 00 00 17 04 00 00 50 d9 00 00 19 04 00 63. Gegenprobe an der bereits belegten Hund-Bank 6 @0x800a7b7c: a8 0c 00 00 eb 00 00 00 f0 f8 00 00 ed 00 00 7e -> EDT@0x075800/0xCA8, VBD@0x076800/0xF8F0 — identisch mit dem Port-Kommentar. SCD-Rohbytes: room7000/scd/sub02.scd @0x1b4: 44 00 00 17 06 00 00 2e 00 a6 ...; room7020/scd/sub00.scd @0x0e6: 44 00 00 2b 00 00 00 1a 00 28 ...; room7040/scd/sub00.scd @0x06c: 44 00 00 36 00 80 00 27 00 ff ...; room7030/scd/sub00.scd @0x0a6: 44 00 01 4f 00 40 00 00 00 ff ... Datei ENEMSE.VBS = 6694912 B, an jedem berechneten EDT+0x80 steht 'pBAV'.

**Port-Stand:** audio_pc.c:1018 load_re2_enemy_se_pc(bank) liest exakt diesen TOC (gen/re2_ems_toc.inc) aus shared_assets/RE2/ENEMSE.VBS — die Baenke 4/25/26 waeren sofort ladbar, werden aber nie angefordert (Bank fest 6).

### Prioritaets-/Stimmen-Gate RE2 vs RE1.5  [byte-belegt]
RE2 FUN_8005c92c(a0=Kanal, a1=Prio-Nibble): Kanal-Index wird VERDOPPELT (sll a0,a0,1) und in 0x800d4ca0 gelesen; neue Prio = a1&7; ist neu < laufend -> return 1 (ABLEHNEN); sind sie ungleich -> return 0 (ZULASSEN); bei Gleichstand entscheidet Bit 0x8 von a1 (gesetzt = ablehnen). Beim Zulassen werden 0x800d4ca0[kanal*2] = prio&7 und 0x800d4ca1[kanal*2] = kanal gelatcht. RE1.5 FUN_80045a18 ist instruktionsgleich, aber Array 0x800b22cc mit Stride 1 (kein sll) und der Gleichstand-Test ist 'a2&0xff >= 8' statt 'a1 & 0x8'.

**Beleg:** RE2 0x8005c92c: andi a0,a0,0xff; sll a0,a0,1; lbu a0,19616(at) [0x800d4ca0]; andi v1,a1,0x7; sltu v0,v1,a0; beq v0,zero,0x8005c958; j 0x8005c968 / addiu v0,zero,1; 0x8005c958 bne a0,v1,0x8005c968 / addu v0,zero,zero; 0x8005c960 andi v0,a1,0x8; sltu v0,zero,v0. Latch 0x8005be8c-be a8: sll a2,s1,1; andi v0,s5,0x7; sb v0,19616(at); sb s1,19617(at). RE1.5 0x80045a18: addu a2,a1,zero; andi a0,a0,0xff; addiu at,at,8908 [0x800b22cc]; addu at,at,a0; lbu v1,0(at); andi a1,a1,0x7; sltu v0,a1,v1; ... 0x80045a50 andi v0,a2,0xff; sltiu v0,v0,0x8; xori v0,v0,0x1. Aufrufstelle RE2: 0x8005be04-14 addu a0,s1 / jal 0x8005c92c / addu a1,s5 / andi v0,v0,0xff / bne v0,zero,0x8005c018.

**Port-Stand:** audio_pc.c:128-176 fuehrt BEIDE: s_se_prio[] (== DAT_800b22cc, RE1.5) und s_re2se_prio[] (== DAT_800d4ca0[c*2], RE2), samt Kommentar 'der einzige textuelle Unterschied' zum RE1.5-Gate re15_se_prio_gate (vab_common.c). Byte-true vorhanden.

### Sce_em_set-Record-Layout (Grundlage der Bank-/Typ-Auswertung)  [byte-belegt]
Opcode 0x44, 22 Byte: [0]=Opcode, [1]=0, [2]=Slot, [3]=kind, [4]=state0, [5..8]=weitere Bytes (davon [7]=SOUND-ID -> Entity+0x1FA), [9]=Id, [10..21]=6x s16 (Position/Rotation). Kind-Werte im gesamten RE2-Leon-Bestand: 0x10,0x12,0x13,0x1F,0x20,0x21,0x22,0x24,0x29,0x2A,0x2E,0x2F,0x39,0x41,0x45,0x48,0x51 — plus die Stage-7-Kinds 0x17, 0x2B, 0x36, 0x37, 0x4F.

**Beleg:** Handler-Adresse: 0x800a74c8[0x44] = 0x8005714c. Ausrichtung an room1040/scd/main00.{c,scd}: main00.c 'Sce_em_set(0, 2, 0x1F, 6, 144, 0, 1, 0, 45, 18487, 0, -25436, -28402, 0, 0)' vs main00.scd @0x132: 44 00 02 1f 06 90 00 01 00 2d 48 37 00 00 9c a4 91 0e 00 00 00 00 (die .c-Ausgabe dreht u16 auf Big-Endian, die Rohbytes sind LE). Ebenda 0x2c = Aot_set (24 B) und 0x3b = Door_aot_set (32 B).

**Port-Stand:** nicht geprueft — der Port fuehrt RE1.5-SCD aus, dessen 0x44 ein anderer Opcode ist.

### Endkampf-Gegner-Overlays fehlen; STAGE7.BIN ist nur ein Stub  [byte-belegt]
In info/re2leon/COMMON/BIN liegen als Gegner-Code nur EMZ0.BIN == EMOVL10_S0.BIN == EMZ0_d1.BIN (bytegleich, sha256 7115c0ad4926...), EMOVL10_S1.BIN, EMOVL21_S0/S1.BIN, EMD0G_MOD0.BIN (= der Hund), EMS25.BIN, EMS26.BIN. Fuer die Stage-7-Kinds 0x17/0x2B/0x36/0x37/0x4F existiert KEIN Overlay im Repo. STAGE7.BIN ist 420 B und registriert nur drei Einsprungpunkte in einem NICHT vorhandenen Overlay.

**Beleg:** sha256: EMZ0.BIN, EMOVL10_S0.BIN, EMZ0_d1.BIN = 7115c0ad492f63c53ce1e3a0db99081c62b0a0ab3e7f7d51a71ffc99c5e2959c. STAGE7.BIN @0x80100004: addiu sp,sp,-24; lui v0,0x8012 / addiu v0,v0,-24492 -> 0x8011a054 / sw v0,-1304(at) -> 0x800cfae8; -24332 -> 0x8011a0f4 / sw -6848 -> 0x800ce540; -24252 -> 0x8011a144 / sw -6844 -> 0x800ce544; jal 0x80031fe4. Der Zensus jal 0x8005bd6c / jal 0x8005ba28 ueber alle *.BIN findet in STAGE7.BIN 0 Treffer.

**Port-Stand:** nicht relevant — der Port hat nur die vier Gegner, deren Overlays vorhanden sind.

### Port-Luecke BGM: Stage-Schranke und statische Tabelle  [byte-belegt]
Der Port kann die Endkampf-BGM heute nicht aufloesen: ss_bgm_entry lehnt stage>5 ab, und die Zuordnung kommt aus einer einkompilierten RE1.5-Tabelle statt aus einem RAM-Array, das ein SCD-Opcode fuellt.

**Beleg:** re15_port/platform/pc/src/audio_pc.c:2082 'static const uint8_t SS_STAGE_OFF[6] = {0x00,0x26,0x32,0x41,0x4d,0x62};' und :2083 'static const uint16_t SS_BGMTBL[106] = { /* UNK_80074828 @ PSX.EXE 0x80074828 */'; :2103 'if (stage < 0 || stage > 5) return -1;'. RE2-Gegenstueck: RAM-Array 0x800d46d0 + Stage-Basis 0x800a80b0 (7 Eintraege, Stage-7-Basis 134 = 0x86).

**Port-Stand:** selbst nachgelesen, Stand wie beschrieben.

### Port-Luecke Gegner-SE: Bank ist eine Konstante  [byte-belegt]
re15_audio_re2_enemy_bank() setzt nur ein Feld; die einzige Quelle fuer den Wert ist die Konstante RE2DOG_ENEMSE_BANK = 6 im Hund-Modul. Fuer ROOM7000/7020/7040 braeuchte es Bank 4/26/25, also den Resolver FUN_80052b38 gegen die Paartabelle 0x800a7400 und die Raum-Sound-Id-Liste 0x800d8cd0 (aus Sce_em_set-Record +7).

**Beleg:** re15_port/engine/src/enemy_ai_re2_dog.c:130 '#define RE2DOG_ENEMSE_BANK 6'; :140 'if (s_re2d_bank_fn) s_re2d_bank_fn(RE2DOG_ENEMSE_BANK);'; re15_port/platform/pc/main.c:697 're15_re2dog_audio_hook(re15_audio_re2_enemy_se, re15_audio_re2_enemy_bank);'; audio_pc.c:1080-1083 're15_audio_re2_enemy_bank(int bank) { s_re2se_bank_sel = bank; }'. Kein weiterer Aufrufer im Baum (grep RE2DOG_ENEMSE_BANK: 4 Treffer, alle oben).

**Port-Stand:** selbst nachgelesen, Stand wie beschrieben.

### Architektur-Unterschied RE1.5 vs RE2 bei Gegner-SE  [byte-belegt]
RE1.5 holt Gegner-/Kampf-SE aus der RAUM-Bank snd1 (RDT +0x14/+0x18/+0x1c) ueber FUN_800453d0; RE2 holt sie aus einer RESIDENTEN ENEM-Bank (Bank 3), die pro Raum anhand der Gegner-Sound-Ids aus einer einzigen grossen Datei (ENEMSE.VBS) nachgeladen wird. Der Bank-Selektor unterscheidet sich ebenfalls: RE1.5 FUN_80045024 hat 6 Baenke (0..5), RE2 FUN_8005ba28 hat 4 (Bank = a0>>24, Zeiger 0x800dbb78/7c/80/84).

**Beleg:** RE2: FUN_8005a09c laedt die ENEM-Bank ('              ENEM EDT' / '              ENEM VBD', Ziel 0x801f8e10, DAT_800dbb84 = &DAT_801f8e10, DAT_800d75ac = Basis + u32@EDT-Ende-8), Bank-Index DAT_800d424b @0x8005a0f4. RE1.5: re15_audio.h dokumentiert FUN_80045024 @0x80045028 'bank = arg>>24, dispatch @0x80010e70, sltiu 0x6' mit Bank 3 = RDT snd1.

**Port-Stand:** Der Port fuehrt beide Wege parallel (re15_audio_room_se fuer RE1.5-snd1, re15_audio_re2_enemy_se fuer RE2-ENEM) — die Architektur ist also schon zweigleisig, nur ohne Stage-7-Daten.

## Fehlende Daten
- info/re2leon/COMMON/BIN/ — die Gegner-Overlays fuer die Stage-7-Kinds 0x17 (ROOM7000), 0x2B (ROOM7020), 0x36 und 0x37 (ROOM7040) und 0x4F (ROOM7030). Vorhanden sind nur EMZ0/EMOVL10_S0/S1/EMZ0_d1 (Zombie), EMOVL21_S0/S1, EMD0G_MOD0 (Hund), EMS25, EMS26. Diese Dateien enthielten die `jal 0x8005bd6c`-Aufrufstellen mit den SE-Ids je Zustand bzw. je EDD-Frame-Event-Bit — also genau die Bruell-/Angriffs-/Treffer-/Todes-Zuordnung des Bosses.
- Das STAGE-7-Raumlogik-Overlay mit dem Code ab 0x8011A054 / 0x8011A0F4 / 0x8011A144. STAGE7.BIN (420 B) registriert diese drei Zeiger nur nach 0x800CFAE8 / 0x800CE540 / 0x800CE544; der Code selbst liegt nicht im Repo.
- RE2_Quellcode_Overlays/STAGE7/ — leer (nur STAGE6 hat 4 Dateien). Es gibt keine Decompilate der Stage-7-Overlay-Funktionen.
- info/re2leon/PL0/RDT/room7000..room7040/scd/*.c — die fuenf Endkampf-Raeume sind die EINZIGEN, fuer die der SCD-Decompiler keine .c-Dateien erzeugt hat (Raeume 1xxx..6xxx haben sie). Deshalb sind meine 0x51/0x57/0x44-Treffer musterbasiert gescannt und nicht durch einen echten SCD-Walk bestaetigt.
- Eine vollstaendige RE2-SCD-Opcode-Laengentabelle. Es gibt keine im EXE-Abbild (meine Suche nach einer Bytetabelle mit [0x51]=6,[0x02]=2,[0x13]=4,[0x14]=6,[0x1a]=2 ergab 0 Treffer); jeder der 143 Handler in 0x800a74c8 schiebt den pc selbst (`addiu rX,rX,N; sw rX,0x1c(...)`), teils zweigabhaengig. Ohne diese Tabelle ist kein sauberer SCD-Walk der Endkampf-Skripte moeglich.
- Extrahierte *.wav der ROOM70x0-snd0-Baenke: unter info/re2leon/PL0/RDT/room7000..7040 liegen 0 wav-Dateien (nur snd0.edt/.vh/.vb/.snd). Die 1954 wavs in COMMON/SOUND gehoeren zu ARMS (99), CORE (111), ENEMSE (980), ROOM800 (5) und MAIN*/SUB_*.
- Ein Savestate/RAM-Abzug eines RE2-Endkampfraums. Damit waere 0x800d46d0[134..138] (die live gesetzten MAIN/SUB-Ids), 0x800d424b (die tatsaechlich geladene ENEM-Bank) und 0x800d8cd0 (die Sound-Id-Liste des Raums) direkt ablesbar statt aus dem Skript rekonstruiert.
- Die Bedeutung der EDT-Byte0-Bit7-Overrides ist im Endkampf nicht pruefbar: in allen fuenf Raum-EDTs ist Byte0 == 0, d.h. es greift immer die globale VAB-Id 0x800d4c4b.

## Gegenpruefung — GEKIPPT

### [18]/[19] Kind-Zensus unvollstaendig — 0x17 ist NICHT stage-7-exklusiv
Die Liste 'Kind-Werte im gesamten RE2-Leon-Bestand' fehlen sechs Werte, und 0x17 wird faelschlich als 'Stage-7-Kind' etikettiert. Damit faellt auch [19]s Rahmung, fuer 0x17 gebe es kein Overlay/keine Vergleichsdaten.

**Gegenbeleg:** Eigener Zensus ueber 344 Sce_em_set-Zeilen in info/re2leon/PL0/RDT/*/scd/*.c ergibt arg3-Werte {0x10,0x11,0x12,0x13,0x17,0x1F,0x20,0x21,0x22,0x23,0x24,0x29,0x2A,0x2E,0x2F,0x39,0x3E,0x41,0x45,0x47,0x48,0x50,0x51}. Fehlend in seiner Liste: 0x11 (room1030/scd/sub15.c), 0x17 (room60A0/scd/sub16.c), 0x23 (room40A0/scd/main00.c), 0x3E (room60D0/scd/sub02.c), 0x47 (room60C0/scd/sub10.c), 0x50 (room30B0/scd/sub00.c). Roh: room60A0/scd/sub16.scd @0x00 = 44 00 00 17 00 00 00 2e 00 38 da c7 00 00 f9 c0 70 03 00 00 00 00 — kind 0x17, Sound-Id +7 = 0x2e, also derselbe Typ und dieselbe ENEMSE-Bank 4 wie sein 'ROOM7000-Boss'. Zusaetzlich in room7030/scd/sub00.scd @0x60: 44 00 00 55 ... = kind 0x55, in keiner der beiden Listen.

### [21] 'Bank ist eine Konstante — einzige Quelle RE2DOG_ENEMSE_BANK=6'
Es gibt VIER Bank-Konstanten im Port, nicht eine, und vier registrierte Hooks. Die Aussage 'Kein weiterer Aufrufer im Baum (grep RE2DOG_ENEMSE_BANK: 4 Treffer, alle oben)' stimmt nicht einmal fuer den eigenen grep: main.c:697 enthaelt das Makro gar nicht.

**Gegenbeleg:** Eigener grep in re15_port: engine/src/enemy_ai_re2_crow.c:109 '#define RE2CROW_ENEMSE_BANK 7' (:120 s_re2c_bank_fn(...)); engine/src/enemy_ai_re2_dog.c:130 '=6' (:140); engine/src/enemy_ai_re2_spider.c:133/134 'RE2SPIDER_ENEMSE_BANK 11' / 'RE2SPIDER_BABY_ENEMSE_BANK 11' (:145); engine/src/enemy_ai_re2_zombie.c:731 'RE2Z_ENEMSE_BANK 0' (:739 mit env-Override 'ov ? atoi(ov) : ...'). platform/pc/main.c:697/703/710/716 registrieren re15_audio_re2_enemy_bank fuer Hund/Kraehe/Spinne/Zombie.

### [7] Zaehlung room7030/sub12 falsch + Filter verwirft echte Opcodes
'sub12 sechs' ist unter seinem eigenen Filter (b1 in {0,1}) fuenf und roh sieben. Der Filter b1 in {0,1} ist eine Annahme, keine Code-Schranke — FUN_8005b6f0 kennt keine Bank-Obergrenze.

**Gegenbeleg:** Eigener Rohscan room7030/scd/sub12.scd (len 0x23a) findet 0x51 an 0x15a,0x18c,0x192,0x198,0x1d0,0x1d6,0x1dc. Die verworfenen 0x192 und 0x1d6 lauten '51 02 02 00 00 00' und liegen exakt 6 Byte nach 0x18c bzw. 0x1d0 und 6 Byte vor 0x198 bzw. 0x1dc — luecken- und ueberlappungsfrei konsekutive 6-Byte-Opcodes, also echte Bank-2-Aufrufe. Im Handler: @0x8005b71c 'srl s0,a0,28', @0x8005b734 'andi a1,s0,0xff', @0x8005b738 'sll a0,a1,3', Gate-Array 0x800d75c1[bank*8] — nirgends ein sltiu/slti auf die Bank (das sltiu 0x6 @0x8005b78c trifft NUR das Kommando).

### [6] Es sind DREI Abbruch-Gates, nicht zwei
Das erste und wichtigste Gate — eine globale Audio-Stummschaltung — fehlt, ebenso die bank-abhaengige SEQ-Zeigerwahl.

**Gegenbeleg:** Eigener Disasm @0x8005b6f0: 'lui v0,0x800d / lw v0,-1164(v0)' = 0x800cfb74; @0x8005b710 'andi v0,v0,0x2000'; @0x8005b714 'bne v0,zero,0x8005b9d0' — Frueh-Ausstieg VOR den beiden genannten Gates. Dasselbe 0x2000-Gate steht am Kopf von FUN_8005a444 (@0x8005a460-6c), FUN_8005a97c (@0x8005a980-a4) und FUN_8005ae50 (@0x8005ae54-80). Ebenfalls unerwaehnt: @0x8005b76c 'beq a1,zero,0x8005b784' waehlt s1 = *0x800d75b8 (bank!=0) bzw. *0x800d75b4 (bank==0).

### [16] Die Raum-Sound-Id ist ein PAAR — Bank 4 fuer ROOM7000 ist nicht gesichert
Der Befund behandelt 0x800d8cd0 als einen Wert. Der Handler fuellt ZWEI Slots und der Resolver vergleicht das Paar in beiden Reihenfolgen; Zeile 4 = (2e,00) matcht nur, wenn Slot1 == 0 bleibt. Sein Mustersuchverfahren kann einen zweiten Sound-Id-Kandidaten nicht ausschliessen.

**Gegenbeleg:** Eigener Disasm FUN_8005714c @0x80057274-0x800572b8: 'lbu v0,7(v0); sb v0,506(s0)' (Entity+0x1FA), dann 'lbu v1,0(a0)' [a0=0x800d8cd0]; 'beq v1,v0' -> nichts; 'beq v1,zero' -> 'sb v0,0(a0)'; sonst 'sb v0,-29487(at)' = 0x800d8cd1. FUN_80052b38 @0x80052b7c-0x80052bcc vergleicht row.b0 gegen room.b0 UND row.b1 gegen room.b0, jeweils mit room.b1 als zweiter Bedingung. Mein Rohscan von room7000/scd/sub08.scd findet @0x1e '44 00 09 0a 1a 00 66 22 ...' mit +7 = 0x22; in 0x800a7400 existiert weder (2e,22) noch (22,2e) (0x800a7400+2*56 = '2e 20', +2*23 = '22 00') — waere dieser Record echt, ergaebe der Resolver 0xFF statt Bank 4. Der Befund prueft das nicht.

### [1] VH-Feldbezeichnung 'nProg-1 / nTone-1 / nVAG-1' ist falsch
Die Felder @0x12/0x14/0x16 sind die Zaehlwerte selbst, nicht count-1. Er hat die Rohwerte gelesen und sie zugleich als 'minus 1' etikettiert — die gemeldeten Zahlen stimmen dadurch nur zufaellig mit der Beschreibung ueberein.

**Gegenbeleg:** room7000/snd0.vh @0x12 = 01 00, @0x14 = 0f 00, @0x16 = 0b 00. Waere das Feld nProg-1, gaebe es 2 Programme und die VAG-Tabelle laege bei 32+2048+512*2 = 0xc20; die Datei ist aber nur 0xc20 gross. Bei Tabelle @0xa20 (= 1 Programm) endet sie exakt bei 0xa20+512 = 0xc20 = Dateigroesse. Gegenprobe room7010 (@0x12 = 02 00): 0xc20+512 = 0xe20 = 3616 = Dateigroesse. Die Summenprobe habe ich selbst nachgefahren, alle fuenf MATCH.

### [13](b) 'u32-Kopfwort des aktuellen EDD/RBJ-Frames' ist abgeleitet, nicht belegt; [4] Fussschritt-Deutung ebenfalls
Die zitierten Instruktionen belegen nur eine doppelte Indirektion ueber Entity+0x178 und ein Bit-Test. Dass das Ziel ein EDD/RBJ-Frame-Kopf ist, steht in keiner zitierten Instruktion. Analog ist '= der Sechser-Satz Spieler-Schritte je Bodenmaterial' in [4] reine Spielinterpretation.

**Gegenbeleg:** Eigener Disasm EMD0G_MOD0.BIN @0x80100510-0x8010052c: 'lw v0,376(s0); lw v0,0(v0); lui v1,0x4; and v0,v0,v1; beq v0,zero,0x80100534; addiu a0,zero,8; jal 0x8005bd6c'. Kein Zugriff, der das Ziel als Animationsframe ausweist. Fuer [4]: die Records 26..31 lauten in allen fuenf Raeumen 00 00 43/53/63/13/23/33 16 — das belegt prog0, tone 4/5/6/1/2/3, Prio 3, Kanal 22, aber nichts ueber Schritte oder Bodenmaterial.

### [10] 'MAIN wird nach 0x801f2e00 geladen' — falsches Ladeziel, Beleg war ein Decompilat
0x801f2e00 ist die Basis fuer die anschliessende Offsetrechnung (VH/VB/SEQ), nicht das CD-Ladeziel. Das Ladeziel ist ein Heap-Zeiger. Ausserdem ist der Beleg RE2_Quellcode_V2/FUN_8005a97c.c, also ein Decompilat statt Disassembly.

**Gegenbeleg:** Eigener Disasm @0x8005ab94-abac: 'addiu v0,zero,-16; lui a3,0x800d; lw a3,-7380(a3) [=0x800ce32c]; addiu a3,a3,16; and s0,a3,v0' und @0x8005abd4/d8 'jal 0x80012fb8 / addu a1,s0,zero' — a1 (Ladeziel) = (*0x800ce32c + 16) & ~15. 0x801f2e00 wird erst @0x8005abe8/ec 'lui s2,0x801f / ori s2,s2,0x2e00' gebildet. Analog SUB @0x8005b014-0x8005b034 (0x801ece00) gegen dasselbe s0 aus 0x800ce32c @0x8005afdc-affc.

### [11] 'MAIN23.BGM ist die Endkampf-Musik' ist abgeleitet, nicht byte-belegt
Der Code laedt einen CD-INDEX aus 0x800a8168, keinen Dateinamen. Der Befund setzt Id 0x23 ohne jede Tabellenlesung mit MAIN23.BGM gleich.

**Gegenbeleg:** Eigene Lesung 0x800a8168 (u16, 0x40 Eintraege): Index 0..0x31 = 354..403 fortlaufend, Index 0x32 = 354 (Duplikat, passend zum im Repo fehlenden MAIN32.BGM), Index 0x33..0x3A = 404..411, Index 0x3B..0x3F = 354. MAIN-Id 0x23 -> CD-Index 389. 0x800a81e8: Index 0..0x37 = 413..468, 0x38..0x3F = 413. Die Zuordnung Index->Dateiname folgt allein aus der Sequenz, nicht aus Bytes. (Die IDs 0x23/0x11/0x2C/0x2F/0x30 selbst habe ich bestaetigt.)

### [9] Stage-Tabellen-Basis: Schreiber und Leser benutzen VERSCHIEDENE Basen
[8] nennt 0x800a80af, [9] nennt 0x800a80b0 und etikettiert sie '0-basiert'. Beides steht so im Code, aber der Befund benennt die Verschiebung nicht — mit '0-basiert' ab 0x800a80b0 waere Stage 7 = Index 7 = 0x00, nicht 134. Ausserdem fehlt die 0x3f-Maskierung des Vergleichs und das Ziel der oberen Bits.

**Gegenbeleg:** Schreiber FUN_8005b9f4 @0x8005ba00: 'lbu v1,-32593(at)' mit at=0x800b0000+ (a0>>24) -> Basis 0x800A80AF. Leser FUN_8005a444 @0x8005a48c: 'lbu v0,-32592(at)' mit at=0x800b0000+ *(s16*)0x800d481c -> Basis 0x800A80B0. Die SCD-Stage ist also 1-basiert, die RAM-Stage 0-basiert; beide landen fuer den Endkampf auf 0x800a80b6 = 0x86 = 134. Weiter @0x8005a4ec/f0: 'andi v1,s0,0x3f' und 'andi v0,v0,0x3f' — BEIDE Seiten maskiert; @0x8005a4f8 'srl v0,s0,6' und @0x8005a528 'sb v0,30145(at)' = 0x800d75c1. (Die Basis 0x800d46d0 im Leser habe ich verifiziert: @0x8005a44c 'addiu a0,a0,-1164' = 0x800cfb74, @0x8005a470 'addiu a0,a0,19292' = 0x800d46d0 — die Werkzeug-Annotation '0x800d4b5c' dort ist ein Tool-Artefakt.)

### [12] '4-Bank-Fassung' ist abgeleitet, nicht im Code geschrankt; Beleg war ein Decompilat
FUN_8005ba28 prueft die Bank nirgends gegen eine Obergrenze. Die '4' folgt allein aus den in FUN_80059654 gesetzten Zeigern — und dort wird ein FUENFTER Slot mitgenullt.

**Gegenbeleg:** Eigener Disasm 0x8005ba28-0x8005bacc: 'srl t1,a0,24' @0x8005ba30, 'lb v0,19528(at)' [0x800d4c48+t1] @0x8005ba64, 'sll a2,t1,2' @0x8005ba78, 'lw a1,-17544(at)' [0x800dbb78+a2] @0x8005ba8c, 'lw a2,30112(at)' [0x800d75a0+a2] @0x8005bac8 — kein sltiu/slti auf t1. Init FUN_80059654 @0x80059764-0x80059798: 0x800dbb78 = 0x801fb700; 0x800dbb7c = 0x801faa00; 0x800dbb8c = 0; 0x800dbb84 = 0; 0x800dbb80 = 0 — also auch Slot 0x800dbb8c (Index 5).

### [17] Der behauptete Gleichstand-Unterschied ist im tatsaechlichen Wertebereich KEIN Verhaltensunterschied
Die Instruktionen unterscheiden sich, das Verhalten nicht. Wer daraus eine Port-Divergenz ableitet, jagt ein Phantom.

**Gegenbeleg:** Der Prio-Parameter ist in beiden Engines b2&0xf, also 0..15 (RE2 @0x8005be00 'andi s5,v1,0xf', uebergeben @0x8005be0c 'addu a1,s5,zero'). Fuer 0..15 ist RE2s 'andi v0,a1,0x8; sltu v0,zero,v0' (@0x8005c960/64) bitgleich zu RE1.5s 'andi v0,a2,0xff; sltiu v0,v0,0x8; xori v0,v0,0x1' (@0x80045a50/54/58). Divergent erst ab a1>=0x10, was der Aufrufpfad ausschliesst.

## Gesamturteil der Gegenpruefung
Der Kern des Strangs haelt: die BGM-Kette (Opcode 0x51 @0x80057e20 -> FUN_8005b6f0 -> Sprungtabelle 0x80011608; Opcode 0x57 @0x80057e98 -> FUN_8005b9f4 -> RAM-Array 0x800d46d0 -> FUN_8005a444 -> FUN_8005a97c/FUN_8005ae50 ueber 0x800a8168/0x800a81e8) und die SE-Kette (EDT-Record-Dekodierung FUN_8005bd6c, Bank-3-Verdrahtung, Resolver FUN_80052b38 gegen die Paartabelle 0x800a7400, ENEMSE-TOC 0x800a7b1c) habe ich instruktions- und bytegenau nachvollzogen; ebenso die RDT-Kopf-Offsets, alle VAB-Kennzahlen mit Summenprobe, den SE-Id-Zensus der Overlays und die acht Stage-7-0x57-Zuweisungen (exhaustiv, alle acht korrekt beschrieben). Zwoelf Punkte fallen. Die drei schwersten: (1) [18]/[19] — kind 0x17 ist NICHT stage-7-exklusiv (room60A0/scd/sub16.scd @0x00, sogar mit derselben Sound-Id 0x2e), und die Kind-Liste des 'gesamten Bestands' laesst sechs Werte aus (0x11,0x17,0x23,0x3E,0x47,0x50; dazu 0x55 in room7030). (2) [21] — die Behauptung, RE2DOG_ENEMSE_BANK=6 sei die einzige Bankquelle im Port, ist schlicht falsch: crow=7, spider=11, zombie=0 plus env-Override, vier registrierte Hooks. (3) [16] — 0x800d8cd0 ist ein PAAR aus zwei Sound-Ids und der Resolver matcht das Paar in beiden Reihenfolgen; die Bank-4/25/26-Zuordnung ruht auf einem Rohmuster-Scan, der einen zweiten Id-Kandidaten (room7000/sub08 @0x1e, snd 0x22) nicht ausschliessen kann — und fuer (2e,22) gibt es keine Tabellenzeile. Dazu: [7] verzaehlt sich bei room7030/sub12 und verwirft mit dem Filter 'b1 in {0,1}' nachweislich echte Bank-2-Aufrufe; [6] verschweigt das globale 0x2000-Stummgate @0x8005b714; [10]/[12] belegen mit Decompilat statt Disasm und geben mit 0x801f2e00 die Offsetbasis als Ladeziel aus; [11]s Dateinamens-Gleichsetzung und [13](b)s EDD-Frame-Deutung sind abgeleitet, nicht belegt; [1]s Feldbezeichnung 'nProg-1' ist falsch (die Zahlen stimmen); [17]s Gleichstand-'Unterschied' ist im tatsaechlichen Wertebereich 0..15 verhaltensidentisch. Die vom Ermittler gemeldete FEHLENDE-DATEN-Lage (keine Stage-7-Gegner-Overlays, kein STAGE7-Codeoverlay, keine .c-Decompilate der fuenf Endkampf-SCDs, keine RE2-Opcode-Laengentabelle) habe ich stichprobenartig bestaetigt und sie ist der eigentliche Grund, warum die Bank-/Kind-Aussagen nicht ueber Mustersuche hinauskommen.

---

# Strang: menue-sound-effekte

## Kernaussage
Beide Spiele benutzen dieselbe UI-Sound-Bank: `SOUND/CORE00.EDH` ist zwischen RE1.5 und RE2-Leon BYTE-IDENTISCH (3176 B, sha256 7948e41ee3a08242…, ebenso CORE00.VB 40464 B) — 11 gueltige Records 0..10, Slots 11..15 = `ff ff ff ff`. Es fehlt RE1.5 also fast kein SAMPLE, es fehlen AUFRUFE. Zaehlung: RE1.5 spielt in Menue-/UI-Kontext 7 verschiedene CORE-Records ({0,4,5,6,8,9,10}) an 33 Aufrufstellen (EXE-Inventar 7, DEBUG.BIN 22, TITLE.BIN 4); RE2 spielt 9 ({0,4,5,6,7,8,9,10,16}) an ~170 Aufrufstellen (EXE-Statusschirm 56, Message-Auswahlbox 3, 0x80026530 1, TITLE.BIN 47, CONFIG.BIN 34, MEM_CARD.BIN 24, SELECT.BIN 4, DIEDEMO.BIN 1, RESULT.BIN 0). Se_on ist RE1.5 `FUN_80045024` / RE2 `FUN_8005ba28`, Argument identisch `a0=(bank<<24)|(rec<<16)|low`, Bank 4 = CORE. Die grossen Luecken auf RE1.5-Seite sind ganze SCHIRME, nicht einzelne Toene: der OPTIONS/CONFIG-Screen (RE1.5 EXE 0x8002dde4-0x8002f800: 0 Se_on im vollstaendigen jal-Zensus — RE2 CONFIG.BIN: 34), der Memory-Card/Save-Screen (RE1.5 0x80025c00-0x80028000: 0 Se_on — RE2 MEM_CARD.BIN: 24 inkl. 2 Bank-2-Kartenzugriffstoene), die YES/NO-Box im Message-System (RE1.5 FUN_80028134: 0 Se_on — RE2 FUN_80030844: rec 4/5/6), Combine/EXCHANGE (RE1.5 0x8004b37c-b404: 0 — RE2 0x8006b584/b5bc/b5d4), Game-Over (RE1.5 FUN_8001500c: 0 — RE2 DIEDEMO.BIN 1) und das Titelmenue (4 vs 47). CORE-Record 7 (`00 00 83 00`, existiert in RE1.5!) ist der FEHLER-Ton: RE1.5 ruft ihn EXE- und overlay-weit 0-mal, RE2 genau 1-mal (@0x8006b5b4, ungueltiges Combine-Paar). Record 16 dagegen existiert in RE1.5 gar nicht (RE1.5 hat nur CORE00..CORE13, alle mit nrec=16; RE2 hat zusaetzlich CORE14/CORE15, CORE15 mit nrec=48 und gueltigem Record 16 — von SELECT.BIN benutzt). Bei den EFFEKTEN ist der Befund gemischt: RE1.5 HAT eigene Menue-Animationen (Panel-Slides ±14/Frame, 30-Frame-FILE-Slide, 22-Frame-Seitenumblaettern, 17-Frame-Combine-Puls, ▼-Blink-Gate `0x18<<s1` @0x800284c8-d0) — was RE1.5 komplett FEHLT ist die 3D-Item-DREHUNG beim Untersuchen (RE2: 0x800d5c1e ±((f+1)*4+20) ueber 12 Frames @0x8006cd18/0x8006cd5c, ausgeloest von LEFT/RIGHT mit SE rec 10 @0x8006cb24/cb48; RE1.5-CHECK schiebt nur ein flaches TIM-Foto mit +22 px/Frame herein @0x800c6704-671c). Und der PORT hat zwei RE1.5-Toene NICHT: den Titel-State-3-Ton (rec 6 @0x80102ac4-c8) und den Debug-Item-Ton (rec 9 @0x8004a154-58); umgekehrt spielt der Port auf dem Save-Screen drei SEs, die RE1.5 dort nicht hat (dokumentierte RE2-Uebernahme).

## Befunde

### Se_on-Einstieg beide Seiten  [byte-belegt]
RE1.5 Se_on = FUN_80045024, RE2 Se_on = FUN_8005ba28, gleiche gepackte Argumentform a0=(bank<<24)|(rec<<16)|low, a1 = 3D-Position oder 0 fuer UI. Bank 4 = CORE (Menue-Bank) in beiden.

**Beleg:** RE1.5 FUN_80045024 Bank-Switch: `uVar7 = param_1 >> 0x18; ... case 4: if (0x20 < uVar16) return; puVar9 = &DAT_801fbd00;` (RE_15_Quellcode_V2/FUN_80045024.c:23-58). RE2 FUN_8005ba28: `uVar10 = param_1 >> 0x18; uVar5 = (char)(&DAT_800d4c48)[uVar10]; uVar18 = param_1 >> 0x10 & 0xff; pbVar9 = (byte*)((&DAT_800dbb78)[uVar10] + uVar18*4);` (RE2_Quellcode_V2/FUN_8005ba28.c:26-33). Bank-4-Sonderzweig in RE2: `if ((uVar5 == 4) && (3 < uVar18))` (ebd. Zeile 44).

**Port-Stand:** re15_port/engine/src/menu_common.c:110 `static void se4(int id){ re15_audio_core_se(id); }`; Plattform-Impl re15_port/platform/pc/src/audio_pc.c:957 re15_audio_core_se.

### CORE00-Bank ist zwischen RE1.5 und RE2 byte-identisch  [byte-belegt]
Die Menue-Sound-Bank ist in beiden Spielen dieselbe Datei. Damit ist jede 'RE2 hat einen Ton, den RE1.5 nicht hat'-Aussage eine Aussage ueber fehlende AUFRUFE, nicht ueber fehlende Samples — mit genau einer Ausnahme (Record 16, s.u.).

**Beleg:** re15_port/shared_assets/PSX/SOUND/CORE00.EDH und info/re2leon/COMMON/SOUND/CORE00.EDH: beide 3176 Byte, sha256 identisch (7948e41ee3a08242…); CORE00.VB beide 40464 Byte, sha256 7851473813d4354a…. pbav=u32 @size-8 = 0x40, Magic 'pBAV' @0x40 => nrec=16. Records (4-Byte, Datei-Offset rec*4): 0:`00 00 14 17` 1:`00 00 24 17` 2:`00 00 34 17` 3:`00 00 44 17` 4:`00 00 53 00` 5:`00 00 63 01` 6:`00 00 73 01` 7:`00 00 83 00` 8:`00 00 93 00` 9:`00 00 a3 01` 10:`00 00 b3 01` 11-15:`ff ff ff ff`. CORE0F.EDH ebenfalls identisch; CORE01/CORE0B unterscheiden sich.

**Port-Stand:** audio_pc.c:960 laedt CORE00 als Default (`if (!s_core_loaded && load_core_se_vab_pc(0) != 0) return;`).

### RE1.5 Menue-SE-Zensus (vollstaendig)  [byte-belegt]
RE1.5 hat exakt 33 Menue-/UI-Se_on-Aufrufstellen, verteilt auf EXE-Inventar (7), DEBUG.BIN (22) und TITLE.BIN (4). Distinkte CORE-Records: {0,4,5,6,8,9,10} = 7.

**Beleg:** Voller `jal 0x80045024`-Zensus ueber info/Re1.5/PSX.EXE (t_addr 0x80010000 @Header 0x18, Text ab Datei-0x800) = 41 Stellen; davon Bank 4: 0x800185ec(rec8, Spieler-Code), 0x80036184/0x80036274/0x80036340(rec1, Schritte), 0x8004a158(rec9), 0x8004a47c/0x8004a4a4/0x8004a4cc/0x8004a4f4(rec4), 0x8004a520(rec6), 0x8004a664(rec5). DEBUG.BIN (Basis 0x800c0000, Datei-Offset = addr-0x800c0000, kein Header): 0x800c62d8/632c/63ac/6420 rec4, 0x800c6488 rec6, 0x800c64ec rec10, 0x800c6550 rec5, 0x800c657c rec5, 0x800c679c rec5, 0x800c6e04 rec5, 0x800c6e54 rec4, 0x800c6e90 rec4, 0x800c6ea4 rec6, 0x800c7050 rec6, 0x800c7080 rec5, 0x800c709c rec4, 0x800c70d4 rec4, 0x800c7174 rec5, 0x800c71d8 rec8, 0x800c71f0 rec4, 0x800c7230 rec4, 0x800c7264 rec8 (+0x800c47c4 = Bank 1, kein Menue). TITLE.BIN (Basis 0x80100000, per jr-ra-Scoring 24/24 bestaetigt): 0x80102ac8 rec6, 0x80102b20 rec4, 0x80102b70 rec4, 0x80102c24 rec0.

**Port-Stand:** menu_common.c enthaelt 32 se4()-Aufrufe fuer den Statusschirm (Zeilen 261-366, 767, 1509-1615) — deckt EXE-Inventar + DEBUG.BIN ab. TITLE-Toene: main.c:2385 core_se(4), main.c:2400 core_se(0). NICHT portiert: rec6 @0x80102ac8 und rec9 @0x8004a158.

### RE2 Menue-SE-Zensus (vollstaendig)  [byte-belegt]
RE2 hat ~170 Menue-/UI-Se_on-Aufrufstellen. Distinkte CORE-Records: {0,4,5,6,7,8,9,10,16} = 9.

**Beleg:** Voller `jal 0x8005ba28`-Zensus ueber info/re2leon/PSX.EXE (t_addr 0x80010000, t_size 0x000f0800) = 138 Stellen; davon im Statusschirm-Bereich 0x8006a894-0x80072bf4: 56 (51 Bank 4, 5 Bank 2). Weitere EXE-Menuestellen: FUN_80030844 (Message-Auswahlbox) @0x80030954/0x8003096c/0x800309a0; @0x80026530. Overlays (jal-Zensus je Datei): TITLE.BIN 47, CONFIG.BIN 34, MEM_CARD.BIN 24, SELECT.BIN 4, DIEDEMO.BIN 1, RESULT.BIN 0, OPENING.BIN 0, ENDING.BIN 2 (beide Bank 2).

**Port-Stand:** Nur 3 RE2-Stellen sind im Port nachgeruestet: menu_common.c:1106 se4(4), :1109 se4(5), :1119 se4(ret?6:7) — die Combine-Triade.

### OPTIONS/CONFIG — RE1.5 komplett stumm  [byte-belegt]
RE1.5s Controller-/Sound-Config-Screen spielt NULL SEs. RE2s CONFIG.BIN spielt 34.

**Beleg:** RE1.5: vollstaendiger jal-Zensus ueber 0x8002dde4-0x8002f800 (Task FUN_8002dde4, Handler-Tabelle @0x80073d9c: [0]0x8002dfb0 [1]0x8002e418 [2]0x8002e470 [3]0x8002e774 [4]0x8002e92c [5]0x8002f0d4) — Ziele: 80013b60, 800216ec, 8002178c, 800217b0, 80028ec4, 80029ac8, 80029afc, 80029bf8, 80029c2c, 8002f3f0, 8002f518, 8002f600, 8002f714, 8002f7e8, 8002f884, 8002fb94, 8002ffb8, 8003014c, 80043850, 80043c00, 8004ee78, 80069858, 8006b310, 8006b3d8, 8006b704, 8006b754. KEIN 0x80045024, KEIN 0x800453d0. 0x80043c00 ist die CD-Audio-Volumen-Matrix (`jal 0x8005ad54`/`0x8005ad40`, 0x7f/0x3f-Bytes @0x80043c18-3c), 0x80043850 ein 3-Wort-Setter (sw a0/a1/a2 -> 0x800b8544/48/4c). RE2: CONFIG.BIN, 34 `jal 0x8005ba28` (Datei-Offsets 0x00a2c,0x00b3c,0x00f30,0x01004,0x010f4,0x01160,0x0120c,0x012c0,0x01330,0x014ac,0x01620,0x0179c,0x01b28,0x01ba0,0x01c94,0x01d70,0x01e4c,0x01f10,0x01ffc,0x02150,0x02270,0x0235c,0x02594,0x0264c,0x02768,0x029a4,0x02a5c,0x02ea8,0x02f1c,0x02f38,0x03070,0x03118,0x031dc,0x03288); Records: 22x rec4, 8x rec5, 6x rec6.

**Port-Stand:** pc_run_config() in re15_port/platform/pc/main.c — kein re15_audio_core_se-Aufruf (grep: nur Zeilen 1167/1168/1169 Save-Screen, 2385/2400 Title). Byte-true korrekt gegenueber RE1.5.

### Memory-Card / Save-Screen — RE1.5 komplett stumm  [byte-belegt]
RE1.5s (dormanter) Card-Screen FUN_80025c00 spielt NULL SEs. RE2s MEM_CARD.BIN spielt 22 CORE-SEs + 2 Bank-2-Kartenzugriffstoene.

**Beleg:** RE1.5: jal-Zensus 0x80025c00-0x80028000 (36 distinkte Ziele: 80013b60, 800216ec, 8002178c, 800217b0, 800264e8, 80026594, 80026658, 80026b30, 80026be8, 80026c34, 80026ca8, 80026dd4, 80026e4c, 80026e54, 80026f48, 80027108, 80027160, 800271a8, 80027368, 80027488, 800275f0, 800276b0, 80027724, 80027780, 80027828, 800278bc, 800279c8, 80028ec4, 80029ac8, 80043850, 8004ee38, 8004ee60, 80061fc0, 8006b538, 800c004c/00e4/0104/0158, 800c74f0) — KEIN 0x80045024. RE2: MEM_CARD.BIN 24 `jal 0x8005ba28` (Datei-Offsets 0x005e0,0x00604,0x00778,0x007bc,0x008dc,0x00938,0x009c4,0x00c2c,0x00cd0,0x01568,0x01580,0x01598,0x0166c,0x01728,0x01744,0x01768,0x01850,0x01870,0x018b8,0x018dc,0x01964,0x019ac = Bank4 (6x rec4, 9x rec5, 8x rec6) und 0x02780 = Bank2 rec34 (`0x02220000`), 0x02794 = Bank2 rec36 (`0x02240000`).

**Port-Stand:** ⛔ ABWEICHUNG: main.c:1167-1169 spielt core_se(4)/(6)/(5) auf dem Save-Screen. Der Code-Kommentar (main.c:1154-1161) benennt das ausdruecklich als RE2-MEM_CARD-Uebernahme und haelt fest, dass RE1.5s eigener Card-Screen sound-frei ist.

### YES/NO- bzw. Auswahlbox in der Message — RE1.5 stumm, RE2 mit Ton und Cursor-Blinken  [byte-belegt]
RE2s Message-Auswahlbox FUN_80030844 spielt rec4 bei Cursorbewegung (beide Richtungen), rec6 bei Option 0, rec5 bei Option 1, rec6 sonst. RE1.5s Message-FSM FUN_80028134 hat NULL Sound-Ziele.

**Beleg:** RE2 FUN_80030844: `if ((DAT_800ce310 & 0x800) != 0) { FUN_8005ba28(0x4040000,0); ... }` und `if ((DAT_800ce310 & 0x400) != 0) { FUN_8005ba28(0x4040000,0); ... }` (RE2_Quellcode_V2/FUN_80030844.c:30-42); Confirm-Zweig: `if ((*(byte*)(param_1+0x5c8c)&0xf)==0){ uVar7=0x4060000; ...} else { uVar7=0x4050000; if ((...&0xf)!=1) uVar7=0x4060000; } FUN_8005ba28(uVar7,0);` (ebd. 55-68). Roh: jal-Stellen 0x80030954 (lui a0,0x405 @0x8003093c / lui a0,0x406 @0x80030944 / @0x80030950), 0x8003096c (lui a0,0x404 @0x80030968), 0x800309a0 (lui a0,0x404 @0x8003099c, Gate `andi v0,v0,0x400` @0x80030994). RE1.5: jal-Zensus von FUN_80028134 (420 Instr.) = 0x800283f4, 0x800284f0, 0x800284f8, 0x800285a8, 0x800286 54, 0x8002867c, 0x80028750 — kein Sound-Ziel (analysis/nutzer_batch_2026-08-27/... bzw. analysis/typewriter_sounds.md §3, hier nachgeprueft).

**Port-Stand:** msg_common.c / item_prompt_common.c enthalten keinen re15_audio_*-Aufruf (Zensus in analysis/typewriter_sounds.md §1, weiterhin gueltig).

### Fehler-Ton = CORE-Record 7 — existiert in RE1.5, wird nie gespielt  [byte-belegt]
RE2 spielt Record 7 genau einmal: beim CONFIRM auf ein ungueltiges Combine-Paar. RE1.5 spielt Record 7 nirgends — weder in der EXE noch in DEBUG.BIN, TITLE.BIN oder STAGE1-6.BIN.

**Beleg:** RE2 @0x8006b5b0-c0: `beq v0,zero,0x8006b5bc` / Delay-Slot `lui a0,0x407` @0x8006b5b4 (immer ausgefuehrt) / `lui a0,0x406` @0x8006b5b8 (ueberschreibt bei gueltigem Paar) / `jal 0x8005ba28` @0x8006b5bc / `addu a1,zero,zero` @0x8006b5c0. Roh-Scan aller `lui a0,0x407` (Wort 0x3C040407) in info/Re1.5/PSX.EXE ab Datei-0x800: 0 Treffer; in DEBUG.BIN/TITLE.BIN/STAGE1-6.BIN ebenfalls 0 (Record-Aufloesung aller Se_on-Stellen dieser Dateien ergibt {0,1,3,4,5,6,8,10}). Sample vorhanden: CORE00.EDH Byte 0x1c..0x1f = `00 00 83 00` (nicht ff-leer).

**Port-Stand:** menu_common.c:1119 `se4(ret != 0 ? 6 : 7);` — bereits als RE2-Nachruestung eingebaut.

### Combine/EXCHANGE — RE1.5 komplett stumm  [byte-belegt]
RE1.5s Combine-Erfolgspfad ruft Se_on NICHT auf; RE2 spielt dort drei verschiedene Toene.

**Beleg:** RE1.5 Substate-0 `0x8004b37c` (35 Instr.): `jal 0x80048904` @0x8004b384, `jal 0x8004e900` @0x8004b3b0, `jal 0x8004e054` @0x8004b3b8, `jal 0x8004dadc` @0x8004b3cc — sonst kein jal; Cancel-Pfad `andi v0,v1,0x8000` @0x8004b398 -> `sb v0,9666(at)` @0x8004b3f4 ohne SE. Ergebnis-Walker 0x8004b408 (88 Instr.): kein jal/jalr. RE2 FUN_8006b358: `beq a2,v0,0x8006b58c` @0x8006b57c / `lui a0,0x404` @0x8006b580 / `jal 0x8005ba28` @0x8006b584 (Cursor, nur bei echter Aenderung); `jal 0x8005ba28` @0x8006b5bc mit 0x406/0x407; `lui a0,0x405` @0x8006b5d0 / `jal 0x8005ba28` @0x8006b5d4 (Cancel).

**Port-Stand:** menu_common.c:1106/1109/1119 — alle drei nachgeruestet (dokumentiert als RE2-Import).

### Cursor-Ton rec 4: RE1.5 spielt UNBEDINGT, RE2 nur bei echter Bewegung  [byte-belegt]
Verhaltensunterschied bei gleichem Ton. RE1.5 piept auch, wenn der Cursor gar nicht wandert (Randfall, blockierte Richtung); RE2 vergleicht vorher/nachher.

**Beleg:** RE1.5 Kommandostufe @0x8004a458-4fc: `andi v0,v1,0x2000` @0x8004a464 -> `sb v0,9686(at)` (0x800b25d6 := 3) @0x8004a474 -> `lui a0,0x404` @0x8004a478 -> `jal 0x80045024` @0x8004a47c; analog 0x8000->1 (@0x8004a49c/4a0/4a4), 0x4000->2 (@0x8004a4c4/4c8/4cc), 0x1000->0 (@0x8004a4ec/4f0/4f4). Der Ton haengt NUR am Pad-Bit. RE1.5 Item-Grid DEBUG.BIN @0x800c62c8-62dc: `andi v0,v1,0x2000` -> `lui a0,0x404` @0x800c62d4 -> `jal 0x80045024` @0x800c62d8 — die Grenzpruefung (`addiu v0,a0,0xfffe` / `beq v0,v1` @0x800c63e8-f4) laeuft ERST DANACH. RE2 Item-Grid @0x8006ab70-84: `lbu v0,12(s0)` (Cursor nachher) / `beq a2,v0,0x8006ab88` (a2 = Cursor vorher, gelesen @0x8006a9f4) / `lui a0,0x404` @0x8006ab7c / `jal 0x8005ba28` @0x8006ab80. Ebenso Kommandoleiste RE2 @0x8006a884-94.

**Port-Stand:** menu_common.c:261-277 und :342-345 folgen dem RE1.5-Muster (unbedingt) — byte-true.

### Seitenumblaettern rec 8 — in BEIDEN vorhanden, gleiche Semantik  [byte-belegt]
Record 8 ist in beiden Spielen der Blaetterton im Dokument-/Datei-Leser.

**Beleg:** RE1.5 DEBUG.BIN @0x800c71b4-dc: `ori t1,zero,0x6` -> `sb t1,9666(at)` (25c2:=6) -> `addiu t0,t0,0x78a4` / `ori t1,zero,0xc` / `sh zero,0(t0)` (Phase:=0) / `sh t1,2(t0)` (Text-x:=0xc) -> `lui a0,0x408` @0x800c71d4 -> `jal 0x80045024` @0x800c71d8; Gegenrichtung @0x800c7254-64 (Text-x:=0x44, `lui a0,0x408` @0x800c7260, jal @0x800c7264). RE2 @0x8006d17c-e8: `lbu v1,19(s2)` (Seite) / `sltu v0,v1,a0` / `sb v0,19(s2)` / `addiu v0,zero,15` -> `sb v0,2(s2)` + Blaetterzustand; `lui a0,0x408` @0x8006d1e0 / `jal 0x8005ba28` @0x8006d1e4; zweite Stelle `lui a0,0x408` @0x8006cf58 / `jal 0x8005ba28` @0x8006cf70.

**Port-Stand:** menu_common.c:1601 se4(8) /* @0x800c7260-68 */ und :1615 se4(8) /* @0x800c71d4-dc */ — byte-true portiert.

### Record 9 — gleiche Nummer, voellig andere Rolle  [byte-belegt]
RE1.5 benutzt Record 9 NUR als Quittung eines DEBUG-Item-Werkzeugs (SELECT im Inventar setzt einen beliebigen Item-Typ mit Menge 255 in den Cursor-Slot). RE2 benutzt Record 9 als OEFFNUNGS-Ton zweier Status-Unterschirme.

**Beleg:** RE1.5 @0x8004a134-58: `lhu a1,-14494(a1)` (0x800ac762 = Raw-Edge) / `andi v0,a1,0x100` @0x8004a140 (= SELECT im byte-swapped libpad-Layout: START=0x800, Up/Right/Down/Left=0x1000/0x2000/0x4000/0x8000) / `sh v0,9832(at)` (0x800b2668:=1) @0x8004a150 / `lui a0,0x409` @0x8004a154 / `jal 0x80045024` @0x8004a158. Folgeblock (RE_15_Quellcode_V2/FUN_8004a0cc.c:20-27): `FUN_80013b60(0xb,&DAT_801a0000,0)` + `(&DAT_800b10ac)[cursor*4] = hi; (&DAT_800b10ad)[cursor*4] = 0xff;` = Item-ID + Menge 255 setzen. RE2: `beq v0,zero,0x8006f124` / `lui a0,0x409` @0x8006f148 / `jal 0x8005ba28` @0x8006f14c am Ende der Oeffnungsroutine 0x8006efd8 (Schleife ueber `jal 0x8002c350` @0x8006f13c); identisch @0x8006f85c/0x8006f860 in 0x8006f6a8. Beide Routinen stehen im Schirm-Deskriptor @0x800a9370, Slots [8]=0x8006efd8 und [11]=0x8006f6a8. Dritte Stelle: 12-Frame-Uebergang `sltiu v0,v0,0xd` @0x8006d7d0 -> `lui a0,0x409` @0x8006d7dc -> `jal 0x8005ba28` @0x8006d7ec.

**Port-Stand:** Kein se4(9)/core_se(9) im gesamten Port (grep re15_port/engine/src + platform/pc). Also: RE1.5-Ton vorhanden, Port hat ihn nicht — aber er gehoert zu einem Debug-Werkzeug, das der Port ebenfalls nicht hat.

### Record 10 — gleiche Nummer, andere Rolle  [byte-belegt]
RE1.5 spielt Record 10 beim Bestaetigen des Sonder-/Ausruest-Slots im Item-Gitter. RE2 spielt Record 10 beim DREHEN des untersuchten Items.

**Beleg:** RE1.5 DEBUG.BIN @0x800c64d4-ec: `lbu v1,9672(v1)` (0x800b25c8 = Ausruest-Slot) / `ori v0,zero,0x80` / `beq v1,v0,0x800c659c` (0x80 = nichts ausgeruestet -> kein Ton) / `lui a0,0x40a` @0x800c64e8 / `jal 0x80045024` @0x800c64ec; erreicht nur wenn Cursor==10 (`ori v0,zero,0xa` @0x800c6480 / `beq v1,v0,0x800c64d4` @0x800c64a0). RE2 @0x8006cb08-4c: `lhu v0,8476(s0)` / `andi v0,v0,0x8000` (LEFT) -> `addiu v0,zero,4` / `sb v0,2(s2)` / `lui a0,0x40a` @0x8006cb20 / `jal 0x8005ba28` @0x8006cb24; `andi v0,v0,0x2000` (RIGHT) -> Zustand 5 / `lui a0,0x40a` @0x8006cb44 / `jal 0x8005ba28` @0x8006cb48; dritte Stelle @0x8006cca4/0x8006cca8.

**Port-Stand:** menu_common.c:306 `se4(0x0a);` (Grid-Sonderslot) — RE1.5-Semantik byte-true portiert. Die RE2-Drehung existiert im Port nicht.

### EFFEKT: Item-Drehung beim Untersuchen — RE2 hat sie, RE1.5 nicht  [byte-belegt]
RE2 dreht das 3D-Item beim Untersuchen ueber 12 Frames mit beschleunigendem Delta. RE1.5s CHECK-Schirm hat keine Drehung — er schiebt ein flaches TIM-Foto herein.

**Beleg:** RE2 Zustand 6 @0x8006cd18-58: `lbu v0,3(s2)` (Frame) / `addiu v1,v0,1` / `sltiu v0,v0,0xb` / `bne v0,zero,0x8006cd40` / dann `andi v0,v1,0xff` / `sll v0,v0,2` / `lhu v1,46(s2)` (Winkel 0x800d5c1e) / `addiu v0,v0,20` / `addu v1,v1,v0` / `sh v1,46(s2)` @0x8006cd58 => Winkel += (Frame+1)*4 + 20. Zustand 7 @0x8006cd5c-9c identisch mit `subu v1,v1,v0` @0x8006cd94. Zustandstabelle @0x80011c30 (19 Eintraege, Gate `sltiu v0,v1,0x13` @0x8006c734): [6]=0x8006cd18, [7]=0x8006cd5c. RE1.5 CHECK (DEBUG.BIN FUN_800c6630, Sprungtabelle @0x800c6864 = {0x800c6664,0x800c6704,0x800c6760,0x800c67b8,0x800c67ec}): Zustand 1 @0x800c6704-671c schiebt x um +22/Frame von -207 bis 13 (10 Frames, `ori v0,zero,0xd` @0x800c6710), Zustand 3 @0x800c67b8-67d0 mit -22 zurueck; kein Winkelregister, kein Pad-Lesen im Zustand 1/3.

**Port-Stand:** menu_common.c:715-795 (state9_check) bildet exakt die RE1.5-Foto-Schiebung ab (exam_x += 22 / -= 22). Korrekt gegenueber RE1.5.

### EFFEKT: Cursor-Blinken  [byte-belegt]
Beide Spiele blinken den Auswahlpfeil ueber ein Bitmasken-Gate auf einem laufenden Zaehler — RE1.5 mit verschiebbarer Maske, RE2 mit zwei festen Masken.

**Beleg:** RE1.5 FUN_80028134 @0x800284c8-d4: `ori v0,zero,0x18` / `sllv v0,v0,s1` / `and v1,v1,v0` / `beq v1,zero,0x80028748` -> sonst `jal 0x800c69bc` @0x800284f0 (Positions-Packer) + `jal 0x800279c8` @0x800284f8 (▼-Draw, a1=0x80, a2=&DAT_80010938). RE2 FUN_80030844 @0x800309c0-f8: `addiu v0,v0,-1` / `sb v0,23697(s3)` (Zaehler param_1+0x5c91) / `lbu v1,-998(v1)` (0x800dfc1a) / `bne v1,zero,0x800309f4` / `andi v0,v0,0x30` @0x800309e0 (Normalfall) bzw. `andi v0,v0,0x18` @0x800309f4 (Alternativfall) -> Pfeil-Draw mit &DAT_80010a40.

**Port-Stand:** nicht geprueft

### EFFEKT: Menue-Blende — RE1.5 blendet die ganze Szene schwarz, kein Hintergrund-Abdunkeln  [abgeleitet]
Der RE1.5-Statusschirm wird nicht ueber einen abgedunkelten Raum gelegt; die Engine faehrt einen vollen Fade-to-black und wieder heraus.

**Beleg:** Fade-in-Arm @0x800496c4-704: `FUN_800217b0(0x200,-0x1800,7,0)` + `FUN_800216ec`; Fade-out-Arm @0x80046544-7c: `FUN_800217b0(0x200,+0x1800,7,0)` + Kick; Halte-Schwarz bei Ausruest-Wechsel @0x80046620-50; Rueckkehr-Fade @0x8001cc00-18. (Adressen aus re15_port/engine/src/menu_common.c:1298-1358, dort als Zitat gefuehrt; Fade-Engine FUN_80021880 Integrator-Semantik.)

**Port-Stand:** menu_common.c:1300/1310/1320/1356 re15_fade_config(0,2,7,∓0x1800,0) — implementiert.

### EFFEKT: Panel-Slides und Combine-Puls — RE1.5 HAT eigene, RE2 hat andere  [byte-belegt]
Der Eindruck 'RE1.5 fehlen die Menue-Effekte' ist fuer die Slides falsch: RE1.5 hat vollstaendige, gerenderte Animationen. Nur RE2s Combine-SLIDE ist ein Ersatz, kein Zusatz.

**Beleg:** RE1.5 Inventar-Slide ±14/Frame: `*psVar3 = sVar1 + 0xe` LAB_8004a6e0 / `sVar1 + -0xe` LAB_8004a440 (FUN_8004a0cc cases 0/2/3/6/8). MAP-Tab 25 Frames: `sltiu 0x19` @0x8004c0bc, list_x +15 @0x8004c0e0-e8, ecg_y +9 @0x8004c0f4-fc, cond_x -9 @0x8004c108-110, equip_y -7 @0x8004c11c-124. FILE-Tab 30 Frames: `sltiu 0x1e` @0x800c6cdc, list_x -15 @0x800c6d08-10, ecg_y -9 @0x800c6d1c-24, arms_x/equip_x -7 @0x800c6d30-4c, tab_base_y +7 @0x800c6d58, idcard_y -8 @0x800c6d60. Combine-Puls 17 Frames: Gate `sltiu v0,v1,0x11` @0x8004b414, Tabelle @0x80010ff4 ([0..7]->0x8004b43c, [8..15]->0x8004b4a8, [16]->0x8004b524), Handler A `addiu a2,a2,1`/`addiu a0,a0,1`/`addiu a1,a1,-1`/`addiu v0,v0,-1` @0x8004b468-74 auf 0x800b25d0..d3. RE2 Combine-Slide 10 Frames: `if (9 < DAT_800d5bf5)` + `DAT_800d5c0e += DAT_800d5c0c` usw. (RE2_Quellcode_V2/FUN_8006b358.c:75-99); Deltas aus FUN_8006a38c: X in {0,2,4,0xfe,0xfc}, `DAT_800d5c0d = ((DAT_800d5bfd>>1)-(DAT_800d5bfc>>1))*3`.

**Port-Stand:** re15_inv_screen.c:1192-1197 rendert den 17-Frame-Puls (comb_d0..d3); menu_common.c:1479-1496 den FILE-Slide. Der RE2-Slide ist bewusst NICHT portiert.

### Game-Over / Tod  [byte-belegt]
RE1.5s Game-Over-FSM ist SE-frei; RE2 hat ein eigenes DIEDEMO.BIN-Overlay mit einem CORE-Ton.

**Beleg:** RE1.5: im vollstaendigen `jal 0x80045024`-Zensus der EXE liegt zwischen 0x800123d4 und 0x80017684 keine Stelle — der Game-Over-Bereich (FUN_8001500c, Tabelle @0x80071d10) enthaelt also keinen Se_on-Aufruf; die Kandidaten-Instruktion `lui a0,0x400` @0x80015220 erreicht kein `jal 0x80045024` (Vorwaertsfenster 24 Instr.). RE2: DIEDEMO.BIN (Basis 0x80190000, per jr-ra-Scoring 14/14) hat genau 1 `jal 0x8005ba28` bei Datei-Offset 0x2a64 = 0x80192a64, a0 = 0x04000000 (Bank 4, Record 0). RE1.5 besitzt kein DIEDEMO-Overlay (info/Re1.5/PSX/BIN enthaelt nur DEBUG.BIN, STAGE1-6.BIN, TITLE.BIN).

**Port-Stand:** game_step_common.c:35-54 implementiert die Game-Over-FSM (FUN_8001500c/@0x80071d10) — ohne SE, byte-true.

### Szenario-Auswahl / SELECT.BIN und CORE-Record 16  [byte-belegt]
RE2s SELECT.BIN spielt Record 16 — der EINZIGE Menue-Ton, fuer den RE1.5 wirklich kein Sample hat.

**Beleg:** SELECT.BIN: 4 `jal 0x8005ba28` (Datei-Offsets 0x01038 rec6, 0x010cc rec4, 0x0113c rec4, 0x02adc a0=`0x04100000` = Bank 4 Record 16). RE2 SOUND-Bestand: CORE00,01,0B..0F (nrec=16, leer ab 11) und CORE10..CORE14 (nrec=16) sowie CORE15.EDH mit pbav=0xC0 => nrec=48, nicht-leere Records {4,6,10,11,12,16,34,35,36}. RE1.5 SOUND-Bestand: CORE00..CORE13, ALLE mit nrec=16 => Record 16 existiert in RE1.5 nirgends. RE1.5 hat ausserdem keine Szenario-Auswahl (kein SELECT-Overlay).

**Port-Stand:** nicht geprueft (Port hat keine Szenario-Auswahl)

### Titelmenue 4 vs 47  [byte-belegt]
RE1.5s TITLE.BIN ist ein Rumpf mit 4 SE-Stellen; RE2s TITLE.BIN traegt das ganze Frontend mit 47.

**Beleg:** RE1.5 TITLE.BIN (11832 B, Basis 0x80100000): Zustandstabelle @0x801026a0 = {[0]0x80102038, [1]0x80102100, [2]0x80102140, [3]0x80102a8c, [4]0x80102b00, [5]0x801023d0, [6]0x80102594, [7]0x801025f0}. SE: FUN_80102a8c @0x80102aa0 `andi v0,v0,0x8f0` -> `lui a0,0x406` @0x80102ac4 -> `jal 0x80045024` @0x80102ac8; FUN_80102b00 @0x80102b10 `andi v0,v0,0x4000` -> `lui a0,0x404` @0x80102b1c -> jal @0x80102b20; @0x80102b60 `andi v0,v0,0x1000` -> `lui a0,0x404` @0x80102b6c -> jal @0x80102b70; @0x80102c14 `andi v0,v0,0x8f0` -> `lui a0,0x400` @0x80102c20 -> jal @0x80102c24. RE2 TITLE.BIN (28740 B, Basis 0x80190000, jr-ra-Scoring 24/24): 47 jal-Stellen in 12 verschiedenen Funktionen (0x80190438, 0x801906d8, 0x80191670, 0x80191e64, 0x80192058, 0x801924c8, 0x801926e0, 0x80192ac8, 0x80192df8, 0x801934bc, 0x801936bc, 0x80193a7c); Records 3x rec0, 22x rec4, 8x rec5, 9x rec6.

**Port-Stand:** main.c:2385 core_se(4) (UP/DOWN, zitiert @0x80102b1c-24 / @0x80102b6c-74) und main.c:2400 core_se(0) (Confirm, zitiert @0x80102c20-24). ⛔ LUECKE: rec 6 @0x80102ac4-c8 (TITLE-Zustand 3, Tabelle @0x801026ac) hat im Port keine Entsprechung.

### Message-System als Ganzes  [byte-belegt]
RE1.5s Message-/Dialog-Kette ist EXE-weit stumm; RE2s Message-System spielt Bank-2-SEs.

**Beleg:** RE1.5: `jal 0x80045024`-Zensus der EXE hat KEINE Stelle in 0x80027e68-0x80028ebc (Message-Familie FUN_80027e68/FUN_80028134/FUN_80028868/FUN_8002877c/FUN_800279c8); ebenso keine in FUN_8001db28 (Item-Get-Modal, 0x8001d…-Bereich). RE2: 0x80039e08 (a0=0x02090000), 0x80039f10 (0x02060000), 0x80039f68 (0x02070000), 0x80039fac (0x02060000), 0x80039fd8 (0x02070000) in FUN_80039b88/FUN_80039ed4; `case 2: FUN_8005ba28(0x2070000,0);` in RE2_Quellcode_V2/FUN_80039ed4.c; ausserdem 0x8003af8c/0x8003afe4/0x8003b050 (0x02120000) in FUN_8003a9f4.

**Port-Stand:** msg_common.c ohne Audio-Aufruf — byte-true gegenueber RE1.5.

### Bank-4-Records 0..3 sind KEINE Menue-Toene  [byte-belegt]
Nur die CORE-Records 4..10 (+16 in RE2) bilden die UI-Beep-Familie. Records 0..3 werden im Gameplay verwendet (Schritte/Treffer) — mit der Ausnahme, dass beide Titelschirme Record 0 als 'grossen' Bestaetigungston benutzen.

**Beleg:** EDH-Struktur: Records 0..3 haben Byte3 = 0x17 und Byte2 = 0x14/0x24/0x34/0x44; Records 4..10 haben Byte3 = 0x00/0x01 und Byte2 = 0x53/0x63/0x73/0x83/0x93/0xa3/0xb3 (aufsteigender Tone-Index derselben Programmgruppe). RE1.5-Gameplay-Stellen: rec1 @0x80036184/0x80036274/0x80036340 (im Port als Schritt-SE zitiert, game_step_common.c:480 'SE 0x04010001 @0x80036184-88'), rec8 @0x800185ec. RE2-Gameplay: rec0 @0x8003f6ec, rec1 @0x8003f968, rec2 @0x8003fa88/0x8003fcbc/0x8003fd00, rec3 @0x8003ffc8. Titel-Ausnahme: RE1.5 @0x80102c24, RE2 @0x801916d0/0x801922b8/0x8019383c und DIEDEMO 0x80192a64.

**Port-Stand:** game_step_common.c:322-323 core_se(1)/core_se(2) fuer Clips 0x08/0x09 — Gameplay, nicht Menue.

### RE2-Statusschirm-Struktur (fuer die Zuordnung der 56 EXE-Stellen)  [byte-belegt]
RE2s Statusschirm ist in mehrere getrennte Zustandsmaschinen aufgeteilt, jede mit eigener Ton-Ausstattung; RE1.5 hat dafuer eine EXE-Maschine plus zwei DEBUG.BIN-Handler.

**Beleg:** RE2: Kommandoleiste 0x8006a7f0 (Cursor 0x800d5bfe, rec4 @0x8006a894, rec6/rec5 @0x8006a918, rec5 @0x8006a934/0x8006a96c); Item-Gitter 0x8006a994 mit 5-Zustands-Tabelle @0x80011b48 = {0x8006a9e4, 0x8006ad00, 0x8006ae6c, 0x8006af48, 0x8006af80} (Gate `sltiu v0,v1,0x5` @0x8006a9bc), rec5 @0x8006ab50, rec4 @0x8006ab80, rec6 @0x8006ac44, rec5 @0x8006ac7c/0x8006acc0; Untersuchen 0x8006c2b0 (14 Zustaende, rec5 @0x8006c568); Dokument-/Karten-Schirm 0x8006c6e4 (19 Zustaende, Tabelle @0x80011c30, rec10/rec8/rec4/rec5/rec6); Schirm-Deskriptoren @0x800a9370 und @0x800a9ab4 = {0x80068f08, 0x80068f40, 0x8006a7f0, 0x8006a994, 0x8006d650, 0x8006c6e4}. RE1.5: FUN_8004a0cc mit Zustand 0x800b25c2 (Cases 0..9; case 4 = Kommandostufe mit den 6 Toenen, case 1 -> FUN_800c62a0, case 9 -> FUN_800c6630, case 7 -> FUN_8004b33c, Dispatch der Kommandostufe ueber PTR_LAB_8004a73c[0x800b25d6]).

**Port-Stand:** menu_common.c bildet die RE1.5-Struktur vollstaendig ab (Header Zeilen 11-47 mit denselben Adressen).

### Kein Screen-Shake in beiden Menues gefunden  [abgeleitet]
Weder in RE1.5s Statusschirm/DEBUG.BIN-Handlern noch in RE2s Statusschirm-Zustandsmaschinen ist ein Kamera-/Bildschirm-Ruettel-Mechanismus im Menuepfad aufgetreten.

**Beleg:** Geprueft: RE1.5 FUN_8004a0cc (Cases 0-9), FUN_800c62a0, FUN_800c6630, FUN_800c6ca0, FUN_800c7010, FUN_8004b33c/0x8004b408 — alle Positions-Deltas sind monotone Slides (±14, ±15, ±9, ±7, ±36, ±22) bzw. der 17-Frame-Combine-Puls (±1); kein Zufalls-/Oszillator-Offset. RE2 FUN_8006b358, FUN_8006c2b0, 0x8006c6e4-Zustaende 0..18 — dito (Deltas ±0xf, ∓0xc, +6, ±8, (f+1)*4+20).

**Port-Stand:** nicht geprueft

### PORT-Luecken und PORT-Zusaetze gegenueber RE1.5  [byte-belegt]
Der Port hat zwei RE1.5-Toene nicht und drei Toene zuviel.

**Beleg:** FEHLT: (a) CORE rec 6 im Titel-Zustand 3, RE1.5 `lui a0,0x406` @0x80102ac4 / `jal 0x80045024` @0x80102ac8 — re15_port/platform/pc/main.c hat im Titelpfad nur core_se(4) (Zeile 2385) und core_se(0) (Zeile 2400). (b) CORE rec 9 @0x8004a154-58 (Debug-Item-Werkzeug) — kein se4(9)/core_se(9) im ganzen Port. ZUVIEL: re15_port/platform/pc/main.c:1167-1169 core_se(4)/(6)/(5) auf dem Save-Screen, wo RE1.5 (FUN_80025c00-Kette, jal-Zensus 0x80025c00-0x80028000) stumm ist; im Code als bewusste RE2-Uebernahme dokumentiert (main.c:1154-1161). Ebenso menu_common.c:1106/1109/1119 (Combine-Triade, RE2-Nachruestung).

**Port-Stand:** siehe Beleg — selbst nachgelesen

## Fehlende Daten
- RE2_Quellcode_V2/ enthaelt KEINE Decompilate fuer den gesamten RE2-Statusschirm: FUN_8006a7f0, FUN_8006a994, FUN_8006c6e4, FUN_8006d650, FUN_8006ecb8, FUN_8006efd8, FUN_8006f6a8, FUN_8006f878, FUN_8006fbd4, FUN_8006fc58, FUN_80071ba0, FUN_80072050, FUN_8007274c, FUN_80068cd4, FUN_80068f08, FUN_80068f40, FUN_80071eb4 — alle 17 Dateien fehlen. Alles dazu ist hier roh disassembliert. Die vorhandene FUN_8006a38c.c/FUN_8006b358.c decken nur Teilbereiche ab und haben falsche Funktionsgrenzen.
- RE2_Quellcode_Overlays/ ist fuer die Menue-Overlays leer: es gibt keine Decompilate zu CONFIG.BIN, TITLE.BIN, SELECT.BIN, MEM_CARD.BIN, RESULT.BIN, DIEDEMO.BIN (nur STAGE6 hat 4 Dateien). Alle 110 Overlay-SE-Stellen sind hier aus den Rohbytes gewonnen.
- info/re2leon/COMMON/SOUND/ enthaelt nur CORE00, CORE01, CORE0B..CORE0F, CORE10..CORE15 — CORE02..CORE0A FEHLEN. Ein vollstaendiger Record-fuer-Record-Vergleich aller RE2-CORE-Baenke gegen RE1.5s CORE00..CORE13 ist damit nicht moeglich. Diese Dateien enthielten die restlichen Ingame-SE-Baenke (je 3176 B EDH + ~40 KB VB).
- Die RAM-Ladeadresse der RE2-Menue-Overlays ist NICHT aus einer EXE-Loader-Konstante belegt. Abgeleitet per jr-ra-Ausrichtungs-Scoring (Methode am RE1.5-DEBUG.BIN validiert: liefert dort exakt 0x800c0000): CONFIG/SELECT/MEM_CARD = 0x801BFA18 (SELECT 32/32, MEM_CARD 28/31, CONFIG 6/6 interne Ziele treffen einen Funktionsanfang), TITLE/DIEDEMO = 0x80190000 (24/24 bzw. 14/14), RESULT = 0x80158000 (8/8). Fehlt: die Stelle in info/re2leon/PSX.EXE, die diese Overlays laedt (Datei-ID-Tabelle + Zieladresse).
- Es fehlt eine Zuordnung 'Se_on-Bank-Index -> Datei' fuer RE2. In RE1.5 steht sie in FUN_800440c4 (`FUN_80013b60(*(u16*)(&DAT_80073a88 + (id&0xff)*2), &DAT_801fbd00, 1, "CORE EDH")`, ID-Tabellen @0x80073a88/@0x80073ab0). Fuer RE2 ist nur bekannt, dass Se_on ueber (&DAT_800d4c48)[bank] und (&DAT_800dbb78)[bank] geht; welche Datei Bank 2 in MEM_CARD.BIN/ENDING.BIN traegt (Records 34/35/36 = Kartenzugriff?), ist damit unbelegt.
- Nicht ermittelt: ob RE2 beim AUFNEHMEN eines Items (Item-Get-Modal) einen Ton spielt. Das RE2-Gegenstueck zu RE1.5s FUN_8001db28 / FUN_8004dc4c wurde nicht lokalisiert; die Bank-4-Stellen 0x8003f6ec/0x8003fa88/0x8003fd00/0x8003ffc8 gehoeren zur Gameplay-Record-Familie 0..3, nicht zum Inventar.
- Nicht ermittelt: der Schreibmaschinen-/Speicherpunkt-Ton in RE2 (der SE beim Benutzen der Schreibmaschine selbst, vor dem Card-Screen). Der liegt vermutlich im Raum-SCD/AOT der RE2-RDTs unter info/re2leon/PL0/RDT/, nicht im EXE-Menuecode — nicht geprueft.
- Nicht ermittelt: welche zwei Unterschirme die RE2-Deskriptor-Slots @0x800a9370[8]=0x8006efd8 und [11]=0x8006f6a8 sind (die mit dem rec-9-Oeffnungston). Namensgebende Daten (Datei-IDs/TIM-Namen) waeren dafuer noetig.

## Gegenpruefung — GEKIPPT

### [7] Fehler-Ton = CORE-Record 7 — 'RE1.5 spielt Record 7 nirgends'
HARTER FEHLBEFUND. RE1.5 spielt CORE-Record 7 an VIER Stellen — in STAGE3.BIN und STAGE5.BIN. Sein zentraler Beleg ('Roh-Scan aller lui a0,0x407 = Wort 0x3C040407 ... in STAGE1-6.BIN 0 Treffer') ist messbar falsch: es sind 4 Treffer. Damit faellt auch seine daraus abgeleitete RE1.5-Record-Menge {0,1,3,4,5,6,8,10} — die tatsaechliche Menge ueber EXE+DEBUG+TITLE+STAGE1-6 ist {0,1,2,3,4,5,6,7,8,9,10}.

**Gegenbeleg:** Eigener Byte-Scan nach 0x3C040407: info/Re1.5/PSX/BIN/STAGE3.BIN Datei-Offsets 0x1aa00 und 0x1acc4; STAGE5.BIN 0x1b214 und 0x1b4d8 (EXE/DEBUG/TITLE/STAGE1/2/4/6 = 0, insoweit richtig). Eigene Disasm (Overlay-Basis 0x80100000, kein Header): STAGE3.BIN @0x8011aa00 `lui a0,0x407` / @0x8011aa04 `ori a0,a0,0x1` -> a0=0x04070001, kein a0-Ueberschreiber bis @0x8011aa40 `jal 0x80045024` / Delay `addu a1,s0,zero`. STAGE3.BIN @0x8011acc4 `lui a0,0x407` / @0x8011acc8 `ori a0,a0,0x1` / @0x8011accc `jal 0x80045024`. STAGE5.BIN identisch @0x8011b214->jal @0x8011b254 und @0x8011b4d8 -> jal @0x8011b4e0. Eigener voller jal-0x80045024-Zensus der Overlays bestaetigt (4,7) x2 in STAGE3, x2 in STAGE5.

### [1] 'CORE00-Bank ist zwischen RE1.5 und RE2 byte-identisch' -> generalisiert auf 'die Menue-Sound-Bank ist dieselbe Datei'
Er hat GENAU ZWEI Dateien verglichen (CORE00, CORE0F) und daraus die Bank-Identitaet verallgemeinert. Von 11 in beiden Spielen vorhandenen CORE-Baenken sind nur 4 identisch, 7 unterscheiden sich. Schlimmer: bei CORE10/CORE11 ist der INHALT VERTAUSCHT — und genau CORE10..13 sind die Baenke mit dem menuetypischen Record-Profil {0,4,5,6}. Damit traegt sein Schluss 'jede RE2-hat-einen-Ton-den-RE1.5-nicht-hat-Aussage ist eine Aussage ueber fehlende AUFRUFE, nicht ueber fehlende Samples' nicht.

**Gegenbeleg:** sha256 (erste 16 Hex) info/Re1.5/PSX/SOUND vs info/re2leon/COMMON/SOUND, EDH+VB: CORE00 SAME, CORE0F SAME, CORE12 SAME, CORE13 SAME; CORE01/0B/0C/0D/0E/10/11 DIFF. RE1.5 CORE10 = a87e71bd31ec4401 = RE2 CORE11; RE1.5 CORE11 = e7279b053e0dc04b = RE2 CORE10 (vertauscht). Zusatzbefund: RE1.5 CORE01/0B/0C/0D sind byte-identisch zu RE1.5 CORE00 (alle 7948e41ee3a08242), in RE2 nicht. Die pro-Szene geladene Bank-4-Datei steht in RE1.5 in der ID-Tabelle @0x80073a88 (u16: 161,163,165,167,... eigenes read), ist also NICHT konstant CORE00.

### [0] 'Bank 4 = CORE (Menue-Bank) in beiden' als byte-belegt
Nur die RE1.5-Haelfte ist byte-belegt. Fuer RE2 laeuft der Bank-Index durch zwei zur Laufzeit gefuellte Tabellen, die im statischen Image komplett NULL sind — 'Bank 4 = CORE' ist fuer RE2 unbelegt. Er widerspricht sich selbst: in seiner eigenen 'FEHLENDE DATEN'-Liste steht 'Es fehlt eine Zuordnung Se_on-Bank-Index -> Datei fuer RE2'.

**Gegenbeleg:** RE1.5 belegt: Sprungtabelle @0x80010e70 (eigenes table-read) [4]=0x8004511c; dort @0x8004511c `sltiu v0,s4,0x21` / @0x80045124 `lui a0,0x801f` / @0x8004512c `ori a0,a0,0xbd00` -> Puffer 0x801fbd00; Lader FUN_800440c4 @0x80044104 `lui s0,0x801f` / @0x80044108 `ori s0,s0,0xbd00`, @0x80044130 `jal 0x80013b60` mit a3=0x80010e28, und `bytes 0x80010e28` = 43 4f 52 45 20 45 44 48 = "CORE EDH". RE2 NICHT belegt: @0x8005ba64 `lb v0,19528(at)` = DAT_800d4c48 und @0x8005ba8c `lw a1,-17544(at)` = DAT_800dbb78; eigenes `bytes 0x800d4c48 16` = alles 00, eigenes `table 0x800dbb78 8` = 8/8 Null. Beide BSS.

### [12] 'RE2 spielt Record 10 beim DREHEN des untersuchten Items'
Die zitierten Bytes tragen das Etikett nicht und widersprechen seinem eigenen Befund [13]. Die rec-10-Stellen setzen den Zustandsbyte auf 4 bzw. 5; der Handler fuer Zustand 4/5 fasst den Drehwinkel gar nicht an. Die Drehung (Winkel bei 46(s2)) liegt laut seinem eigenen [13] in den Zustaenden 6/7 — und dort wird KEIN rec10 gespielt.

**Gegenbeleg:** Eigene Disasm: @0x8006cb10 `andi v0,v0,0x8000` -> @0x8006cb18 `addiu v0,zero,4` / @0x8006cb1c `sb v0,2(s2)` / @0x8006cb20 `lui a0,0x40a` / @0x8006cb24 jal; @0x8006cb34 `andi v0,v0,0x2000` -> `addiu v0,zero,5` / `sb v0,2(s2)` / @0x8006cb44 lui 0x40a / @0x8006cb48 jal. Tabelle @0x80011c30 (eigenes table-read): [4]=[5]=0x8006cbcc. Handler @0x8006cbcc: `lbu v0,3(s2)` / `sltiu v0,v0,0xc` / dann `lbu v0,17(s2)` +/-1 (@0x8006cbfc bzw. @0x8006cc20) — kein Zugriff auf 46(s2). Der Winkel-Akkumulator `lhu v1,46(s2)` / `sh v1,46(s2)` steht nur in [6]=0x8006cd18 / [7]=0x8006cd5c.

### [6] 'rec6 bei Option 0' — verschwiegene Bedingung
Bei Option 0 wird rec6 NICHT unbedingt gespielt: ein zusaetzliches Gate ueberspringt den Se_on-Aufruf vollstaendig. Er hat das RE2-Decompilat abgeschrieben ('uVar7=0x4060000; ...'), in dem der Zweig hinter den Auslassungspunkten verschwindet.

**Gegenbeleg:** Eigene Disasm RE2 @0x80030930 `beq v1,zero,0x80030948` (Option 0) -> @0x80030948 `andi v0,s1,0x20` / @0x8003094c `bne v0,zero,0x80030b68` — Sprung AN `jal 0x8005ba28` @0x80030954 VORBEI. Nur wenn (s1&0x20)==0 faellt es auf @0x80030950 `lui a0,0x406` durch. Option 1: @0x80030938 `beq v1,v0,0x80030954` mit Delay @0x8003093c `lui a0,0x405` (korrekt). Sonst: @0x80030940 `j 0x80030954` / @0x80030944 `lui a0,0x406` (korrekt).

### [4] CONFIG.BIN Record-Aufschluesselung
Die Zahlen summieren sich auf 36 und widersprechen seiner eigenen Gesamtzahl 34. Gemessen ist die Aufteilung anders. (Die 34 Datei-Offsets selbst sind exakt richtig — ich habe sie 1:1 gegengerechnet, 0 Abweichungen.)

**Gegenbeleg:** Eigener jal-0x8005ba28-Zensus ueber info/re2leon/COMMON/BIN/CONFIG.BIN mit Rueckwaerts-Aufloesung des `lui a0,0x4rr`: n=34, {(4,4):22, (4,5):7, (4,6):5}. Behauptet: 22x rec4, 8x rec5, 6x rec6.

### [5] MEM_CARD.BIN Record-Aufschluesselung
Aufteilung falsch (und summiert auf 23 statt 22 Bank-4-Stellen). Offsets und die beiden Bank-2-Stellen sind korrekt.

**Gegenbeleg:** Eigener Zensus MEM_CARD.BIN: n=24, {(2,34):1, (2,36):1, (4,4):5, (4,5):8, (4,6):9}. Behauptet: 6x rec4, 9x rec5, 8x rec6.

### [19] RE2 TITLE.BIN Record-Aufschluesselung
Die Zahlen summieren sich auf 42, waehrend er selbst 47 Stellen nennt. Gemessen anders.

**Gegenbeleg:** Eigener Zensus info/re2leon/COMMON/BIN/TITLE.BIN: n=47, {(4,0):3, (4,4):22, (4,5):10, (4,6):12}. Behauptet: 3x rec0, 22x rec4, 8x rec5, 9x rec6.

### [22] Schirm-Deskriptor-Tabelle @0x800a9ab4
Basisadresse um 8 Byte daneben. Die 6-Eintrag-Tabelle, die er auflistet, beginnt bei 0x800a9aac; bei 0x800a9ab4 stehen nur noch 4 der 6 Zeiger, danach faengt Datenmaterial an.

**Gegenbeleg:** Eigenes table-read: 0x800a9aac -> 0x80068f08, 0x800a9ab0 -> 0x80068f40, 0x800a9ab4 -> 0x8006a7f0, 0x800a9ab8 -> 0x8006a994, 0x800a9abc -> 0x8006d650, 0x800a9ac0 -> 0x8006c6e4, 0x800a9ac4 -> 0x06040201 (Daten). Die zweite Tabelle @0x800a9370 stimmt dagegen exakt, inkl. [8]=0x8006efd8 und [11]=0x8006f6a8.

### [21] 'Nur CORE-Records 4..10 bilden die UI-Beep-Familie; 0..3 sind Gameplay' — EDH-Strukturargument
Zwei Probleme. (a) Faellt mit [7]: rec 7 wird in RE1.5 ausschliesslich in Stage-Overlay-Code benutzt, also gerade NICHT als UI-Ton — die Grenze '4..10 = UI' ist damit durchbrochen. (b) Sein EDH-Strukturbeleg trennt 0..3 von 4..10 nicht: der von ihm als 'aufsteigender Tone-Index derselben Programmgruppe' verkaufte Wert byte2>>4 laeuft ueber ALLE Records durch (1,2,3,4 fuer rec0..3 und weiter 5..11 fuer rec4..10) — eine einzige ununterbrochene Reihe. Der einzige strukturelle Unterschied ist byte3 (0x17 gegen 0x00/0x01), dessen Bedeutung unbelegt bleibt.

**Gegenbeleg:** CORE00.EDH Bytes 0x00..0x2c (eigener xxd): 0000 1417 / 0000 2417 / 0000 3417 / 0000 4417 / 0000 5300 / 0000 6301 / 0000 7301 / 0000 8300 / 0000 9300 / 0000 a301 / 0000 b301. byte2>>4 = 1,2,3,4,5,6,7,8,9,10,11 durchgehend. Se_on-Felddecode @0x80045160-84 (RE1.5) bzw. @0x8005bacc-af4 (RE2): byte2>>4 -> s2, byte3&0x1f -> s0, byte3>>5 -> s3, byte1&0x7f -> s7 — s2 ist in beiden Engines dasselbe Feld, ohne Gruppenbruch. Ausserdem eigener Overlay-Zensus RE1.5: Bank-4-Records in STAGE1-5 = rec0 x20, rec1 x9, rec2 x4, rec3 x17, rec7 x4.

### [10] 'Record 8 in beiden — RE2 zwei Stellen'
Unvollstaendig: RE2 hat in der EXE VIER Bank-4-rec8-Stellen, nicht zwei. Und die Gleichsetzung der Semantik ('Blaetterton im Dokument-/Datei-Leser') ist aus den Nachbar-Stores abgeleitet, nicht belegt — kein Zitat zeigt, dass 0x800c78a4 (RE1.5) bzw. 19(s2) (RE2) eine Dokumentseite ist.

**Gegenbeleg:** Eigener RE2-EXE-Zensus, (4,8) = 4 Stellen: 0x8006cf70, 0x8006d1e4, 0x80071df4, 0x800728f0. Er nennt nur die ersten beiden. Die RE1.5-Stellen @0x800c71d4/d8 und @0x800c7260/64 habe ich selbst gelesen und sie stimmen byte-genau — aber die dortigen Stores sind `sh zero,0(t0)` / `sh t1,2(t0)` auf 0x800c78a4 mit t1=0xc bzw. 0x44, was fuer sich genommen nur zwei Halbworte setzt.

### [13] 'ueber 12 Frames' Rotationsdauer
Off-by-one gerundet. Das Gate laesst 11 Winkel-Inkremente zu (Frame-Zaehler 0..10); der 12. Tick springt in den Folgezustand, ohne den Winkel weiterzudrehen.

**Gegenbeleg:** Eigene Disasm @0x8006cd18 `lbu v0,3(s2)` / @0x8006cd20 `addiu v1,v0,1` / @0x8006cd24 `sltiu v0,v0,0xb` / @0x8006cd28 `bne v0,zero,0x8006cd40`. Nur fuer v0<=10 wird bei 0x8006cd40 der Winkel `lhu v1,46(s2)` / `addiu v0,v0,20` / `sh v1,46(s2)` @0x8006cd58 fortgeschrieben; bei v0=11 geht es nach @0x8006cd30 `addiu v0,zero,8` / `sb v0,2(s2)` (Zustand 8) und `sb zero,3(s2)`.

## Gesamturteil der Gegenpruefung
Das Zensus-Handwerk ist ueberwiegend solide: alle Se_on-Aufrufzahlen und praktisch alle Einzeladressen, die ich stichprobenweise bis vollstaendig gegengerechnet habe (RE1.5 EXE 41/DEBUG 23/TITLE 4; RE2 EXE 138 mit 56 im Statusschirm; RE2-Overlays 34/47/24/4/1/0/0/2 inklusive der 34 CONFIG-Offsets Byte fuer Byte), reproduzieren sich exakt. Aber der HAUPTBEFUND des Strangs ist falsch: [7] behauptet, RE1.5 kenne den Fehler-/Ablehnungston (CORE-Record 7) nur als Sample und spiele ihn nie. Das ist widerlegt — STAGE3.BIN und STAGE5.BIN spielen ihn an je zwei Stellen (a0=0x04070001, jal @0x8011aa40, @0x8011accc, @0x8011b254, @0x8011b4e0). Sein 'Roh-Scan' ueber STAGE1-6.BIN, auf den sich die Aussage stuetzt, hat vier vorhandene Treffer nicht gefunden; damit ist auch die daraus abgeleitete Record-Menge falsch und die in [21] gezogene Trennlinie '0..3 Gameplay / 4..10 UI' bricht zusammen. Zweiter systematischer Fehler: [1] verallgemeinert einen Zwei-Datei-Hashvergleich (CORE00, CORE0F) zu 'die Menue-Bank ist in beiden Spielen dieselbe Datei'. Tatsaechlich unterscheiden sich 7 von 11 gemeinsamen CORE-Baenken, und ausgerechnet bei den menuefoermigen Baenken CORE10/CORE11 ist der Inhalt zwischen den Spielen VERTAUSCHT — die Kernthese 'RE2 hat keine Toene, die RE1.5 fehlen, nur Aufrufe' ist damit nicht getragen. Drittens ist [0]s 'Bank 4 = CORE in beiden' als byte-belegt ueberstuft: fuer RE1.5 ja (fester Puffer 0x801fbd00 + Ladername 'CORE EDH' @0x80010e28), fuer RE2 nein — dort laufen Bank-Byte und Puffer ueber DAT_800d4c48/DAT_800dbb78, beide im statischen Image komplett NULL; er raeumt das in seiner Fehldaten-Liste selbst ein und widerspricht damit seiner eigenen Einstufung. Dazu drei arithmetisch falsche Record-Histogramme ([4] CONFIG, [5] MEM_CARD, [19] TITLE — zwei davon summieren sich nicht einmal auf die eigene Gesamtzahl), eine um 8 Byte falsche Tabellenbasis ([22] 0x800a9aac statt 0x800a9ab4), eine verschwiegene Bedingung ([6] Option-0-Ton ist durch `andi v0,s1,0x20` @0x80030948 gegated) und ein Etikett, das den eigenen Bytes widerspricht ([12] rec10 setzt Zustand 4/5, dreht aber nichts — der Winkel liegt in Zustand 6/7). Positiv hervorzuheben: die RE1.5-Stille in Config-/Card-/Message-/Item-Get-Pfad ist richtig und laesst sich sogar HAERTER belegen, als er es tut — im gesamten EXE-Text gibt es zwischen 0x80018e40 und 0x8002c70b sowie zwischen 0x8002c980 und 0x8003338b kein einziges `jal 0x80045024`, und das Datenwort 0x80045024 kommt nirgends vor, es existiert also auch kein indirekter Pfad. Seine abgeleiteten Overlay-Basen (0x80190000 / 0x801BFA18 / 0x80158000) habe ich mit eigener Methode exakt reproduziert. Die Port-Aussagen in [24] stimmen; er hat nur die zusaetzlichen bse(4/5/6)-Stellen in re15_itembox.c uebersehen.

---

# Strang: tuer-sequenz

## Kernaussage
RE2 und RE1.5 haben DIESELBE Tuer-Architektur, nur unterschiedlich weit ausgebaut: ein DO2-Archiv (Modell+TIM+eigenes Skript+eigene VAB) wird nach 0x801a1000 geladen, die Tuer-Demo laeuft als eigene Koroutine auf Task-Slot 1, waehrend der Hauptthread den Zielraum von CD nachlaedt; beide Seiten synchronisieren ueber dasselbe Bit 0x2000000. RE2 hat 55 DO2-Archive (DOOR00..DOOR36, Datei-Indizes 0xEA..0x120 in der Tabelle @0x8009a4b0, Chunk-Tabelle @0x8009a520 mit 12 B/Eintrag) — RE1.5 hat GENAU EINEN (DOOR00.DO2), und dessen dtex->Datei-Tabelle @0x80071d2c hat nur einen gueltigen Eintrag (0x25), danach kollidiert sie unmittelbar mit der Demo-Funktionstabelle @0x80071d30. Die Sequenz selbst ist datengetrieben: das Skript liegt IM DO2 (RE2: Chunk+0x8, 9 Routinen; RE1.5 DOOR00: 4 Bytes = ein einziges END-Opcode 0x01), RE2s DOOR00-Hauptroutine schlaeft 70+50+60 = 180 Bilder (Variante 2: 70+50+110 = 230), RE1.5s DOOR00-Skript endet nach EINEM Bild. Einstieg ist bei beiden der AOT-SCE-Handler, aber mit UNTERSCHIEDLICHEM Typ-Index: RE1.5 sce=2 @0x800430bc (13 Instruktionen, KEINERLEI Schloss-/Schluessel-Pruefung), RE2 sce=1 @0x80051514 (Flag-Test, Item-Suche FUN_800696cc, drei Meldungs-/Sound-Pfade). Der Ton kommt in RE2 aus der pro-Archiv mitgelieferten VAB (Bank 0, Programm via Opcode 0x36 @0x80056428) — die 55 Archive tragen je 2-3 eigene VAG-Samples, DAS ist die klangliche Tuer-Typ-Unterscheidung; RE1.5 laedt seine DOOR00-VAB zwar (FUN_800170e0), aber die Strings "DOOR EDH"/"DOOR VBD" @0x80010e10/0x80010e1c haben KEINE Xrefs. Die Ladepause kaschiert RE2 dadurch, dass FUN_80026b7c die Demo per FUN_80031f6c(1,&LAB_80013bc4) startet und erst nach dem Raumladen bei `while (DAT_800cfb74 & 0x2000000)` blockiert — plus die Textzeilen "Please wait a minute" (dtex 0x32) bzw. "The side pack is being/equipped" (dtex 0x34) in Bildfenster 41..259. Der re15_port hat von alledem NICHTS: kein DO2-Laden im Shared-Engine (DO2 kommt nur in platform/psx als Test-Mesh und Test-VAB vor), keine door_params-Felder fuer dtex/door_type/Schloss, keine Demo-Koroutine — er teleportiert direkt und faehrt nur die Fade-FSM.

## Befunde

### DO2 RE2 — Kopf + Chunk-Tabelle  [byte-belegt]
Eine RE2-DO2-Datei besteht aus GENAU ZWEI CD-Chunks, deren Groessen/Offsets NICHT in der Datei stehen, sondern in einer EXE-Tabelle @0x8009a520 mit 12 Byte pro Tuer-Index: +0x00 u16 = Groesse des Sound-Chunks, +0x02 u16 = Groesse des Textur/Modell-Chunks, +0x04 u32 = Sektor-Offset des zweiten Chunks innerhalb der Datei, +0x08 u8 / +0x09 u8 = je eine Pruefbyte pro Chunk (nach DAT_800d531e). Der Datei-Index steht separat in DAT_8009a4b0[dtex] (u16).

**Beleg:** FUN_80014cd0 @0x80014cd0: `iVar6 = *(byte*)(DAT_800ce550+0xc) * 0xc; uVar14 = *(ushort*)(&DAT_8009a520 + iVar6); DAT_800d531e = (&DAT_8009a528)[iVar6]; DAT_800d5314 = *(u16*)(&DAT_800988a8 + uVar10*8) + (u8)(&DAT_800988aa)[uVar10*8]*0x10000; DAT_800d5308 = uVar14; FUN_80012fb8(uVar10, 0x801a1000-uVar14, 3, "DOOR SOUND")`. FUN_80015064 @0x80015064: `DAT_800d531e = (&DAT_8009a529)[iVar2]; DAT_800d5314 = fileLBA + *(int*)(&DAT_8009a524 + iVar2); DAT_800d5308 = *(u16*)(&DAT_8009a522 + iVar2); FUN_80012fb8(fileIdx, &DAT_801a1000, 3, "DOOR TEXTURE")`. Rohbytes @0x8009a520 (re2_disasm bytes): `08 4b d0 8d 0a 00 00 00 9a 42 00 00 | 78 5d 70 ba 0c 00 00 00 2e d0 00 00 | ...`. Gegenprobe DOOR00.DO2: 0x4b08 (Sound) endet vor 10*2048 = 0x5000, 0x5000+0x8dd0 = 0xDDD0 = 56784 = exakte Dateigroesse. DOOR01: 0x5d78 / 12 Sektoren = 0x6000, 0x6000+0xba70 = 0x11A70 = 72304 = exakte Dateigroesse.

**Port-Stand:** Kein DO2-Lader im Shared-Engine. `grep -rn "DO2" re15_port/engine re15_port/include` = 0 Treffer; nur re15_port/platform/psx/main.c:400 (DOOR00.md1 als rotierendes Test-Mesh) und platform/psx/src/audio_psx.c:6/39/127 (DOOR00.vh/.vb als eingebundene SFX-Bank).

### DO2 RE2 — Sound-Chunk-Layout  [byte-belegt]
Chunk 1 (ab Datei-Offset 0) ist: 16 Byte konstanter Vorspann `00 00 14 16 00 00 24 17 ff ff ff ff ff ff ff ff` (in allen 55 Dateien identisch), dann der VAB-Header ab 0x10 (Magic 'pBAV', ver=7, id=0, fsize, res0=0xEEEE), ProgAtr[128] ab 0x30, VagAtr[16] ab 0x830, vagOffsets[256] ab 0xA30 — VH-Block also [0x00,0xC30). Bei 0xC30 steht u32 = 0x10 (Offset des VAB-Headers im kopierten Block), der VB-Koerper beginnt bei 0xC38.

**Beleg:** FUN_80014cd0 kopiert genau 0xC30 Byte (`while (puVar13 != (uint*)(&UNK_801a1c30 + iVar6))`) nach &DAT_801fb700, setzt `DAT_800d75a0 = (uchar*)((int)&DAT_801fb700 + DAT_801fc330)` — 0x801fc330-0x801fb700 = 0xC30 — und ruft `SsVabOpenHeadSticky(DAT_800d75a0,0,0x3dc50)`, danach `SsVabTransBody(&UNK_801a1c38 - uVar14, vabid)` (Block+0xC38). Datei DOOR00.DO2 @0x10: `70 42 41 56 07 00 00 00 00 00 00 00 f0 4a 00 00 ee ee 01 00 03 00 03 00 7f 40 00 00 ff ff ff ff`; @0xC30: `10 00 00 00 00 00 00 00`.

**Port-Stand:** nicht geprueft (kein DO2-Pfad vorhanden)

### DO2 RE2 — Textur/Modell-Chunk-Layout  [byte-belegt]
Chunk 2 (ab dem Sektor-Offset, DOOR00: Datei 0x5000) beginnt mit ZWEI relativen u32-Offsets: [0] = Modell (MD1), [1] = TIM. Direkt danach (Chunk+0x8) liegt der Skript-Block der Tuer. Bei DOOR00: [0]=0x224 -> Datei 0x5224 (MD1), [1]=0xBB0 -> Datei 0x5BB0 (TIM, 8bpp+CLUT), Skript ab Datei 0x5008.

**Beleg:** FUN_80013c1c @0x80013c1c: `DAT_801a1004 = (int)&DAT_801a1000 + DAT_801a1004; DAT_801a1000 = (int)&DAT_801a1000 + DAT_801a1000; *DAT_800c3a80 = DAT_801a1004; FUN_80076a40(); DAT_800c3a80[1] = DAT_801a1000; ... DAT_800d8cbc = &DAT_801a1008;`. Datei DOOR00.DO2 @0x5000: `24 02 00 00 b0 0b 00 00` ; @0x5BB0: `10 00 00 00 09 00 00 00 0c 02 00 00 00 00 e0 01 00 01 ...` (TIM-Magic 0x10, Flags 9).

**Port-Stand:** nicht geprueft (kein DO2-Pfad vorhanden)

### Anzahl Tuer-Varianten RE2 vs RE1.5  [byte-belegt]
RE2 hat 55 Tuer-Varianten (dtex 0x00..0x36), abgebildet auf die CD-Datei-Indizes 0x00EA..0x0120 — lueckenlos aufsteigend. RE1.5 hat exakt EINE: info/Re1.5/PSX/DOOR/ enthaelt nur DOOR00.DO2 (57016 B), und die RE1.5-Tabelle @0x80071d2c hat nur einen gueltigen u16-Eintrag (0x0025); ab 0x80071d2e stehen 0x0000 und danach ab 0x80071d30 bereits die drei Funktionszeiger der Tuer-Demo. Ein dtex != 0 wuerde in RE1.5 also einen Funktionszeiger als CD-Datei-Index lesen.

**Beleg:** RE2 @0x8009a4b0: `ea 00 eb 00 ec 00 ... 1f 01 20 01` = 55 u16-Eintraege, danach folgt direkt @0x8009a520 die Chunk-Tabelle. RE1.5 FUN_800171f4 @0x80017214-0x80017224: `sll v0,v0,0x1 ; addiu at,at,0x1d2c ; addu at,at,v0 ; lhu a0,0(at)`. Rohbytes @0x80071d2c: `25 00 00 00 | e0 61 01 80 | c8 64 01 80 | 64 66 01 80 | 00 00 00 00`. Dateizaehlung: `ls info/re2leon/COMMON/DOOR/*.DO2 | wc -l` = 55; `ls info/Re1.5/PSX/DOOR` = DOOR00.DO2.

**Port-Stand:** Port kennt keinen dtex-Index; re15_aot_door_params_t (re15_port/include/re15_aot.h:126-147) hat nur target_cut, dest_stage, dest_room, spawn_x/y/z, spawn_yaw_4096, floor, band.

### DOOR00 ist in beiden Spielen dasselbe Asset  [byte-belegt]
Das MD1-Modell und die TIM von RE1.5 DOOR00.DO2 sind BYTE-IDENTISCH mit denen von RE2 DOOR00.DO2 — nur an anderen Datei-Offsets und mit anderem Archiv-Kopf.

**Beleg:** Python-Vergleich: RE1.5 DOOR00.DO2[0x18:0x798] == RE2 DOOR00.DO2[0x5224:0x59A4] -> True, 0 abweichende Bytes. TIM: RE1.5[0x9A8:0x8BC8] == RE2[0x5BB0:0x5000+0x8DD0] -> True, beide 33312 Byte.

**Port-Stand:** Port laedt DOOR00.md1 nur als PSX-Test-Mesh (platform/psx/main.c:400), nicht als Tuer.

### DO2 RE1.5 — Kopf-Layout (abweichend von RE2)  [byte-belegt]
RE1.5s DO2 hat einen 3x u32 Datei-Kopf {model_block, sound_block, vb_block} und der Modell-Block enthaelt seinerseits 3 relative u32 {MD1, SCD, TIM}. Der VAB-Vorspann ist 8 Byte (nicht 16 wie in RE2), der VH-Block hat dieselbe Groesse 0xC30, und bei Block+0xC28 steht u32 = 8 (analog RE2s 0x10 bei Block+0xC30).

**Beleg:** Datei DOOR00.DO2 @0x00: `0c 00 00 00 | c8 8b 00 00 | f8 97 00 00` ; @0x0C: `0c 00 00 00 | 98 09 00 00 | 9c 09 00 00`. FUN_800161e0 @0x800161e0: `DAT_801a1004 = 0x801a1000 + DAT_801a1004; DAT_801a1008 = 0x801a1000 + DAT_801a1008; DAT_800b5364 = 0x801a1000 + DAT_801a1000; FUN_800170e0(DAT_801a1008 - DAT_801a1004);` -> 0x97F8-0x8BC8 = 0xC30. FUN_800170e0 @0x800170e0: `memcpy(&DAT_801fdd00, DAT_801a1004, 0xC30); DAT_800b2598 = &DAT_801fdd00 + *(int*)(&DAT_801fdcf8 + 0xC30)` -> liest Block+0xC28; Datei @0x97F0 = `08 00 00 00`. VAB-Magic 'pBAV' bei Datei 0x8BD0 = 0x8BC8+8.

**Port-Stand:** Java-Extraktor src/main/java/de/re15/extractors/DO2Extractor.java Layout.parse liest exakt diese drei u32 (modelOffset=0, soundOffset=4, vbOffset=8) und danach modelOffset+4/+8 als scd/tim-Relativ — deckungsgleich. Sein Kommentar 'eight SCD door scripts' ist fuer DOOR00 falsch (siehe naechster Befund).

### RE1.5 DOOR00 Skript ist LEER (nur END)  [byte-belegt]
Der SCD-Bereich von RE1.5s DOOR00.DO2 ist 4 Byte gross: eine 1-Eintrag-Offsettabelle (u16 = 2) plus Opcode 0x01 = END. Die RE1.5-Tuer-Demo laeuft daher genau EIN Bild, denn FUN_80016518 deaktiviert das Objekt, sobald das naechste Opcode 0x01 ist, und FUN_800164c8 prueft die Aktiv-Flagge nach jedem Durchlauf.

**Beleg:** Datei DOOR00.DO2 @0x9A4: `02 00 01 00` (SCD-Start = modelOffset+0x998 = 0x9A4, SCD-Ende = modelOffset+0x99C = 0x9A8, wo der TIM-Header `10 00 00 00 09 00 ...` beginnt). FUN_80016518 @0x80016518: `if (**(char**)((int)&DAT_800b2b68 + iVar3) == '\x01') { (&DAT_800b2b4d)[iVar3] = 0; }`. FUN_800164c8 @0x800164c8: `if (DAT_800b39ad != 0) { do { FUN_80016518(); FUN_800166c4(); FUN_80029ac8(1); } while (DAT_800b39ad != 0); }` — DAT_800b39ad = DAT_800b2b4c+0xE60+1.

**Port-Stand:** Port hat keine Tuer-Demo; aot_common.c:aot_fire_door ruft direkt re15_room_request_change und der Wechsel wird in room_common.c:re15_room_apply_pending ausgefuehrt.

### RE2 DOOR00 Skript — 9 Routinen, 180 bzw. 230 Bilder  [byte-belegt]
RE2s DOOR00-Skriptblock (Datei 0x5008..0x5224) hat eine 18-Byte-Offsettabelle mit 9 Eintraegen (0x12, 0x2E, 0x8E, 0x104, 0x118, 0x12C, 0x1A4, 0x1D8, 0x204). Routine 0 startet je nach Bedingung Routine 1 oder 2. Routine 1 (die Tuer-Hauptbewegung) enthaelt die Schlaf-Befehle sleep(70), sleep(50), sleep(60) = 180 Bilder; Routine 2 sleep(70), sleep(50), sleep(110) = 230 Bilder. Der Schlaf-Befehl ist die 4-Byte-Folge `09 0A cc cc`.

**Beleg:** Skriptblock-Bytes Routine 1 (Datei 0x5036, Blockoffset 0x2E): `4d 00 00 00 01 00 a0 0a 10 00 d0 07 ce 0e 00 08 00 00 00 00 00 00 4d 01 00 e2 01 01 d0 00 10 00 82 00 68 f3 d4 f2 00 00 00 00 00 00 53 00 02 07 00 fe 74 00 00 1c 09 0a 46 00 18 08 36 00 00 00 00 00 00 00 00 00 00 00 04 0b 18 03 04 0c 18 04 09 0a 32 00 04 0b 18 05 09 0a 3c 00 18 06 01 00` (0x46=70, 0x32=50, 0x3C=60). Opcode 0x09 = Handler @0x800539dc: `lhu a0,2(a2) ; sh a0,0(v0)` speichert den Zaehler und setzt PC = a2+1, sodass der naechste Dispatch Opcode 0x0A trifft; Opcode 0x0A = @0x80053a24: `lhu v0,0(v1) ; addiu v0,v0,-1 ; sh v0,0(v1) ; bne v0,zero,...`. Opcode 0x01 = END @0x800537fc: `lb a2,2(a3) ; bne a2,zero,... ; sb zero,1(a3)` (deaktiviert das Objekt = DAT_800d86e9) und liefert 2.

**Port-Stand:** nicht geprueft (Port hat keinen Tuer-Skript-Interpreter)

### Zustandsmaschine der Sequenz — RE2  [byte-belegt]
Die RE2-Tuer-Demo ist keine Zustandsmaschine mit Phasenzaehler, sondern ein Init/Main/Exit-Tripel, das FUN_80013bc4 als NULL-terminierte Funktionsliste @0x8009a7b4 abarbeitet: Init = FUN_80013c1c, Main = FUN_80013eb4, Exit = FUN_8001417c. Der Main-Teil ist eine Schleife ueber DAT_800d86e9 mit einem Bildzaehler in *(u16*)(DAT_800c3a80+0x22e), der bei 0 startet und je Durchlauf um 1 erhoeht wird; pro Durchlauf laufen FUN_80014058 (Skript-VM fuer Objekte 10..13) und FUN_80014234 (GTE-Render), danach FUN_80031f94(1) = ein VSync.

**Beleg:** Pointer-Tabelle @0x8009a7b4: `1c 3c 01 80 | b4 3e 01 80 | 7c 41 01 80 | 00 00 00 00`. FUN_80013bc4 @0x80013bc4-0x80013c04: `lw v0,0(v1) ; beq v0,zero,end ; loop: lw v0,0(s0) ; jalr v0 ; addiu s0,s0,4 ; lw v0,0(s0) ; bne v0,zero,loop`. FUN_80013c1c @0x80013e5c-Umfeld: `*(undefined2*)((int)piVar1 + 0x22e) = 0;`. FUN_80013eb4 @0x80013f88: `lhu v0,558(v0)` ; @0x80014014-0x80014024: `lhu v1,558(v0) ; addiu v1,v1,1 ; jal 0x80031f94 ; sh v1,558(v0)` ; @0x8001402c-0x80014034: `lbu v0,-30999(v0)=DAT_800d86e9 ; bne v0,zero,0x80013f54`.

**Port-Stand:** Port hat nur die Fade-FSM: re15_port/engine/src/room_common.c:154 re15_room_transition_tick mit s_trans_state 4 -> 5 -> 0, zitiert @0x8001cc34/@0x8001cc70/@0x8001cc8c. Keine Demo-Schleife, kein Bildzaehler.

### Zustandsmaschine der Sequenz — RE1.5 (identische Bauart)  [byte-belegt]
RE1.5 hat dasselbe Tripel: FUN_80016188 laeuft die NULL-terminierte Liste @0x80071d30 ab = { FUN_800161e0 (Init), FUN_800164c8 (Main), FUN_80016664 (Exit) }. Main = Schleife ueber DAT_800b39ad mit FUN_80016518 (Skript-VM ueber PTR_LAB_800744a8, Objekte 10..13, Schrittweite 0x170) und FUN_800166c4 (GTE-Render), je Durchlauf FUN_80029ac8(1).

**Beleg:** FUN_80016188 @0x8001618c-0x800161c4: `lui v1,0x8007 ; addiu v1,v1,7472` = 0x80071d30 ; `lw v0,0(v1) ; beq v0,zero ; loop: lw v0,0(s0) ; jalr v0 ; addiu s0,s0,4 ; lw v0,0(s0) ; bne v0,zero,loop`. Rohbytes @0x80071d30: `e0 61 01 80 | c8 64 01 80 | 64 66 01 80 | 00 00 00 00`. FUN_800164c8 s.o.

**Port-Stand:** nicht implementiert

### Kamera der Tuer-Demo  [byte-belegt]
Die Kamera ist in BEIDEN Spielen statisch und fest verdrahtet, sie faehrt nicht. RE1.5: Auge/Ziel-Konstanten 30000 und 22000 (Abstand 8000) in die Struktur @0x800b220c, danach FUN_80053ca4. RE2: 10000 und 0 (Abstand 10000) ueber lokale Kopien von DAT_80010830.. und FUN_80076cb0. Die Bewegung im Bild entsteht ausschliesslich aus dem Skript, das das Tuer-Objekt dreht.

**Beleg:** RE1.5 @0x80016460-0x80016498: `ori v0,zero,0x7530 ; sw v0,0(v1)=0x800b2210 ; ori v0,zero,0x55f0 ; sw zero,0x800b2214 ; sw zero,0x800b2218 ; sw v0,0x800b221c ; sw zero,0x800b2220 ; sw zero,0x800b2224 ; jal 0x80053ca4 ; addiu a0,v1,-4` (0x7530=30000, 0x55F0=22000). RE2 FUN_80013c1c @0x80013c34-0x80013c64 laedt DAT_80010830..DAT_80010848 in local_24..local_c und ruft @0x80013e3c `FUN_80076cb0(&local_24,&local_18)`; Rohbytes @0x8001082c: `00 00 2d 64 | 10 27 00 00 | 00 00 00 00 ...` -> DAT_80010830 = 0x2710 = 10000, alle folgenden 0. Zusaetzlich RE1.5 @0x80016450-Umfeld: `FUN_80066d60(0xa0,0x78)` (Bildschirmmitte 160/120), `SetBackColor(0xff,0xff,0xff)`.

**Port-Stand:** nicht implementiert

### Wann der neue Raum geladen wird — RE2  [byte-belegt]
Reihenfolge in FUN_80026b7c @0x80026b7c: (1) Demo-Koroutine auf Slot 1 installieren, (2) EIN Bild yield, (3) DOOR SOUND von CD laden (FUN_80014cd0), (4) ein Bild yield, (5) Tuer-Struktur in die Zielraum-Globalen kopieren, (6) falls Stage wechselt: FUN_8004a3c0 (Stage-Overlay), (7) auf Ende der Sound-Uebertragung warten (`while (DAT_800cfbd8 & 0x20000)`), (8) den Zielraum laden: FUN_8005a714, FUN_8001633c, FUN_8001645c, FUN_80049e48, (9) auf das Ende der Tuer-Demo warten (`while (DAT_800cfb74 & 0x2000000)`). Der Raumladevorgang laeuft also WAEHREND der Tuer-Animation, und der Hauptthread wartet danach ggf. noch auf die Animation.

**Beleg:** FUN_80026b7c: `DAT_800cfb74 = DAT_800cfb74 | 0x2000000; ... FUN_8002bda8(2,0); FUN_80031f6c(1,&LAB_80013bc4); FUN_80031f94(1); FUN_80014cd0(); FUN_80031f94(1); ... while ((DAT_800cfbd8 & 0x20000) != 0) { FUN_80031f94(1); } FUN_8005a714(); FUN_8001633c(); FUN_8001645c(); FUN_80049e48(); while ((DAT_800cfb74 & 0x2000000) != 0) { FUN_80031f94(1); } FUN_8002bda8(0,0); FUN_8002c6b4(1); DAT_800dfc1b = 1; FUN_80031f94(1);`. Bit 0x2000000 wird in FUN_80013c1c gesetzt (`DAT_800cfb74 = DAT_800cfb74 | 0x2000000`) und in FUN_8001417c @0x80014218 geloescht (`and v0,v0,a1` mit a1 = 0xfdffffff).

**Port-Stand:** room_common.c:re15_room_apply_pending laedt die RDT synchron (c->load_rdt) und bricht bei Fehler ab; keine Nebenlaeufigkeit, kein 0x2000000-Handshake.

### Wann der neue Raum geladen wird — RE1.5 (gleiche Struktur, ohne Ton)  [byte-belegt]
RE1.5 FUN_8001d600 ist der direkte Zwilling: FUN_80021634(2,0); FUN_800171f4() (DOOR TEXTURE laden); FUN_80029a98(1,&LAB_80016188) (Demo als Koroutine auf Slot 1); Warte-Schleife auf 0x10000; Tuer-Struktur kopieren; ggf. FUN_80039a30 (Stage); FUN_800396fc (Raum); `while (DAT_800aca38 & 0x2000000) FUN_80029ac8(1)`; FUN_80021634(0,0); DAT_800b5457 = 1; DAT_80072ec4 = 0x3c. Es gibt KEINEN Aufruf eines DOOR-SOUND-Laders.

**Beleg:** RE_15_Quellcode_V2/FUN_8001d600.c, else-Zweig ab `FUN_80021634(2,0); FUN_800171f4(); FUN_80029a98(1,&LAB_80016188); do { FUN_80029ac8(1); } while ((DAT_800aca38 & 0x10000) != 0);` ... `while ((DAT_800aca38 & 0x2000000) != 0) { FUN_80029ac8(1); } FUN_80021634(0,0); DAT_800b5457 = 1; FUN_800444b0(); FUN_80029ac8(1); DAT_80072ec4 = 0x3c;`. Bit 0x2000000 gesetzt in FUN_800161e0 (`DAT_800aca38 = DAT_800aca38 | 0x2000000`), geloescht in FUN_80016664 @0x800166a8 (`and v0,v0,a1` mit a1 = 0xfdffffff).

**Port-Stand:** Port implementiert nur die Fade-Haelfte (@0x8001c958-Familie) und den Payload-Warp; die Zeilen FUN_800171f4 / FUN_80029a98(1,&LAB_80016188) / die 0x2000000-Warteschleife / DAT_80072ec4=0x3c fehlen komplett (Suche nach 8001d600 in re15_port/engine liefert nur Kommentare in aot_common.c:463/505/521/888 und debug_menu_common.c:115).

### Ein-/Ausblenden  [byte-belegt]
RE2 blendet am ENDE der Demo aus: FUN_8001417c wartet auf FUN_8002c350(0) != 0, dann FUN_8002c1a0(0,0,7,1) und FUN_8002c2b0(0, 0x7FFF, 0x00FFFFFF, 0), setzt DAT_800dfc1a = 2 und loescht Bit 0x2000000. RE1.5s Exit FUN_80016664 macht das NICHT — dort steht nur die Warte-Schleife auf FUN_8002178c(0), DAT_800b5456 = 2, das Loeschen von 0x2000000 und FUN_80029afc. Die Blenden liegen in RE1.5 ausserhalb der Demo, in der Uebergangs-FSM.

**Beleg:** FUN_8001417c @0x8001417c-0x80014220: `jal 0x8002c350 ; addu a0,zero,zero ; bne v0,zero,0x800141a4 ; ... jal 0x8002c1a0 (a0=0,a1=0,a2=7,a3=1) ; ... jal 0x8002c2b0 (a0=0,a1=0x7fff,a2=0x00ffffff,a3=0) ; ... addiu v1,zero,2 ; sb v1,-998(at)=DAT_800dfc1a ; and v0,v0,a1(0xfdffffff) ; jal 0x80031fe4`. FUN_80016664 @0x80016664-0x800166b0 (vollstaendig, 13 Instruktionen): kein Aufruf einer Fade-Setzfunktion.

**Port-Stand:** room_common.c:147-149 fuehrt `re15_fade_config(0, 2, 7, -0x1800, 0)` mit Zitat FUN_800217b0(0x200,-6144,7,0) @0x8001cc00 und `re15_fade_kick(0,0)` @0x8001cc18 aus — das ist die RE1.5-FSM-Blende, nicht die Demo-Blende.

### Einstieg: AOT/SCE-Typ und Tastendruck  [byte-belegt]
Der Einstieg ist bei beiden Spielen der AOT-Scan mit derselben Vorwaerts-Reichweite 0x26C = 620 Einheiten, aber der Tuer-SCE-Index UNTERSCHEIDET SICH: RE1.5 = Index 2 (Tabelle @0x8007469c, 14 Handler, Tuer @0x800430bc), RE2 = Index 1 (Tabelle @0x800a73c4, 15 Handler, Tuer @0x80051514). Beide Handler setzen denselben Endzustand: Zeiger auf die Tuer-Struktur global ablegen, ein Byte auf 1 setzen, Pauseflags |= 0xFF000000.

**Beleg:** RE1.5 FUN_80042bac @0x80042bd0: `ori v0,zero,0x26c`. RE2 FUN_80051088 @0x80051088ff: `local_68[0] = 0x26c; FUN_80077384((int)*(short*)(param_1+0x76), local_68, local_68);` und Dispatch `(*(code*)(&PTR_LAB_800a73c4)[*pbVar9])(pbVar8)`. Tabelle @0x8007469c: `[0]=0x8004305c [1]=0x80043084 [2]=0x800430bc ...`. Tabelle @0x800a73c4: `[0]=0x800514ec [1]=0x80051514 [2]=0x80051884 ...`. RE1.5-Tuerhandler @0x800430bc-0x800430ec vollstaendig: `sw a0,DAT_800ac9a8 ; addiu a0,a0,-13760 (=0x800aca40) ; sb v0,DAT_800b5359 ; lw v0,0(a0) ; lui v1,0xff00 ; or v0,v0,v1 ; sw v0,0(a0) ; jr ra ; addu v0,zero,zero`. RE2-Tuerhandler-Ende @0x800516c8-0x800516f4: `addu v0,zero,zero ; addiu v1,zero,1 ; sb v1,DAT_800df348 ; lw v1,DAT_800cfbdc ; lui a0,0xff00 ; sw s0,DAT_800ce550 ; or v1,v1,a0 ; sw v1,DAT_800cfbdc`.

**Port-Stand:** aot_common.c:1003-1011 setzt genau diese Reichweite um (Kommentar 'rotate (620,0) by yaw, ori 0x26c @0x80042bd0') plus Band-Gate; aot_fire_door (aot_common.c:468) ist der Port des RE1.5-Handlers [2] @0x800430bc. Der Port hat KEINEN Gegenpart zum RE2-Index 1.

### Einstieg: Pruefungen VOR der Sequenz — RE2 hat sie, RE1.5 nicht  [byte-belegt]
RE2s Tuerhandler @0x80051514 fuehrt eine vollstaendige Schloss-/Schluessel-Logik aus, RE1.5s @0x800430bc hat KEINE einzige Verzweigung. RE2-Ablauf: (a) Abbruch, falls DAT_800cfddc != 0; (b) Sonderfall Meldung 8, wenn (DAT_800cfbd8 & 0x90000000)==0x90000000 und der Spieler-Zustand passt; (c) Nutzlast-Byte +0x0F: Bit 0x80 = 'hat Schloss', Bits 0..5 = Flag-Index; ist das Flag gesetzt -> Tuer oeffnet sofort; (d) sonst Nutzlast-Byte +0x10 als Schluessel-Art: 0xFE -> Meldung 10 + Sound 0x226 + Flag setzen; 0xFF -> Sound 0x216 + Meldung 11 (bleibt zu); sonst Item-Suche FUN_800696cc(id) — nicht dabei: Sound 0x216 + Meldung (id-76); dabei: Meldung 5, DAT_800e873f = id, Sound 0x225, Rueckruf FUN_80051718 hinterlegen, Flag setzen.

**Beleg:** @0x80051514-0x80051694: `lw v0,DAT_800cfddc ; beq v0,zero,0x80051550 ; j 0x800516f8` — `lbu a1,15(s2) ; andi v0,a1,0x80 ; beq v0,zero,0x800516c8 ; ori a0,zero,0x884c ; addu a0,s3,a0 ; jal 0x80077360 ; andi a1,a1,0x3f ; bne v0,zero,0x800516cc` — `lbu s0,16(s2) ; addiu v0,zero,254 ; bne s0,v0,0x80051608 ; addiu v0,zero,255` — 0xFE-Zweig: `addiu a2,zero,10 ; jal 0x8002fe38` + `lui a0,0x226 ; jal 0x8005ba28` — 0xFF-Zweig: `lui a0,0x216 ; jal 0x8005ba28` + `addiu a2,zero,11 ; jal 0x8002fe38` — Item-Zweig: `jal 0x800696cc ; addu a0,s0,zero ; addu s1,v0,zero ; bltz s1,0x800516a0` ; Treffer: `addiu a2,zero,5 ; sb s0,DAT_800e873f ; lui a0,0x225 ; jal 0x8005ba28 ; addiu v0,v0,5912 (=0x80051718) ; sw v0,... ; addiu v0,s1,1 ; sb v0,...(0x800d4249)` ; Fehlschlag: `lui a0,0x216 ; jal 0x8005ba28 ; addiu a2,s0,-76 ; jal 0x8002fe38`. Flag setzen: `jal 0x8007730c` mit a0 = 0x800cc1e8+0x884c und a1 = byte15 & 0x3f.

**Port-Stand:** Port hat keine Schloss-/Schluessel-Pruefung an der Tuer — aot_fire_door prueft nur die Null-Spawn-Falle und die dest_id; das ist byte-true zu RE1.5, aber NICHT uebertragbar auf RE2.

### Tuer-Nutzlast (AOT-Payload) — gleiche Offsets in beiden Spielen  [byte-belegt]
+0x00 u16 Spawn-X, +0x02 u16 Spawn-Y, +0x04 u16 Spawn-Z, +0x06 u16 Gierwinkel, +0x08 u8 Ziel-Stage, +0x09 u8 Ziel-Raum, +0x0A u8 Ziel-Kameraschnitt, +0x0B u8 Etage (Band), +0x0C u8 dtex (Tuer-Archiv-Index), +0x0D u8 Tuer-Typ (Bit 7 gesondert). In RE2 zusaetzlich +0x0F Schloss-Flag und +0x10 Schluessel-Art. Die Etage wird in beiden Spielen mit -0x708 pro Stufe in Weltkoordinaten umgerechnet.

**Beleg:** RE2 FUN_80026b7c: `DAT_800cfc3c=*DAT_800ce550; DAT_800cfc3e=DAT_800ce550[1]; DAT_800cfc40=DAT_800ce550[2]; DAT_800cfc6e=DAT_800ce550[3]; DAT_800cfcfe=*(byte*)((int)DAT_800ce550+0xb); DAT_800cfdba=(ushort)DAT_800cfcfe * -0x708; DAT_800cfbf2=*(byte*)(DAT_800ce550+5); DAT_800d481e=(ushort)*(byte*)((int)DAT_800ce550+9); ... *(byte*)(DAT_800ce550+4) % 9`. RE1.5 FUN_8001d600: `DAT_800aca88=*DAT_800ac9a8; ... DAT_800acabe=DAT_800ac9a8[3]; DAT_800acad6=*(byte*)((int)DAT_800ac9a8+0xb); DAT_800acc0e=(ushort)DAT_800acad6 * -0x708; DAT_800afbb5=*(byte*)(DAT_800ac9a8+5); _DAT_800b0fe2=(ushort)*(byte*)((int)DAT_800ac9a8+9); ... *(byte*)(DAT_800ac9a8+4)`. dtex: RE2 FUN_80014cd0/FUN_80015064 `*(byte*)(DAT_800ce550+0xc)`; RE1.5 FUN_800171f4 @0x8001720c `lbu v0,0xc(v0)`. Tuer-Typ: RE2 FUN_80013c1c `DAT_800d4804 = *(byte*)(DAT_800ce550+0xd) & 0xff7f; DAT_800d4808 = *(byte*)(DAT_800ce550+0xd) & 0x80;`; RE1.5 @0x800164a8-0x800164b4 `lbu v0,13(v0) ; sh v0,0x800b0fde`.

**Port-Stand:** re15_port/include/re15_aot.h:126-147 deckt +0x00..+0x0B ab (target_cut, dest_stage, dest_room, spawn_*, floor, band); +0x0C dtex, +0x0D Tuer-Typ, +0x0E, +0x0F, +0x10 fehlen ersatzlos.

### Raum-ID-Kodierung unterscheidet sich  [byte-belegt]
RE2 bildet die Ziel-Raum-ID als (stage+1)*0x100 + room; RE1.5 (bzw. der Port) als ((stage+1)<<12) | (room<<4) | Variante. Die Stage-Nummer wird in RE2 vor dem Vergleich modulo 9 genommen.

**Beleg:** RE2 FUN_80026b7c: `DAT_800d4822 = DAT_800d481e + (DAT_800d481c + 1) * 0x100;` und `if ((int)DAT_800d481c != (uint)*(byte*)(DAT_800ce550 + 4) % 9) { DAT_800d481c = (short)((uint)*(byte*)(DAT_800ce550+4) % 9); FUN_8004a3c0(); }`.

**Port-Stand:** aot_common.c:517-520: `unsigned dest_id = (((unsigned)d->dest_stage + 1u) << 12) | ((unsigned)d->dest_room << 4) | (g_current_room_id & 0x000Fu);` — RE1.5-Kodierung, im Port validiert mit 649/653 Door_aot_set-Records.

### Tuer-Ton RE2: eigene VAB pro Archiv, Bank 0  [byte-belegt]
Der Tuer-Ton kommt NICHT aus einer globalen Sound-Bank, sondern aus der VAB, die im DO2-Archiv selbst liegt. Sie wird von FUN_80014cd0 nach SPU-Adresse 0x3dc50 geoeffnet und ihre VAB-ID landet in DAT_800d4c48 = Bank-Slot 0. Die Abspielfunktion FUN_8005ba28 nimmt a0 als (bank<<24)|(prog<<16)|rest, a1 = Positionszeiger. Der Skript-Opcode 0x36 (Handler @0x80056428, 12 Byte lang: +1 Bank, +2 u16 Programm, +4 u16 Modus/Offset, +6/+8/+10 Positionsversatz) erzeugt genau dieses a0. In DOOR00 lautet der Aufruf `36 00 00 00 00 00 00 00 00 00 00 00` = Bank 0, Programm 0, direkt nach sleep(70).

**Beleg:** FUN_80014cd0: `sVar5 = SsVabOpenHeadSticky(DAT_800d75a0,0,0x3dc50); DAT_800d4c48 = (char)sVar5;`. FUN_8005ba28 @0x8005ba30-0x8005ba88: `srl t1,a0,24 ; lui at,0x800d ; addu at,at,t1 ; lb v0,19528(at) (=0x800d4c48+bank) ; srl v0,a0,16 ; andi s7,v0,0xff`. Opcode-0x36-Handler @0x80056444-0x80056540: `lh a1,4(s0) ; lbu a3,1(s0) ; lh a0,2(s0) ; ... sll v1,a3,24 ; andi v0,a0,0xff ; sll v0,v0,16 ; or v1,v1,v0 ; srl a0,a0,8 ; jal 0x8005ba28 ; or a0,v1,a0` ; `addiu v1,s0,12 ; sw v1,28(s1)` (PC += 12). Nur EIN Aufruf von FUN_8005ba28 liegt im Opcode-Handler-Bereich 0x80053xxx-0x80058xxx: @0x80056530.

**Port-Stand:** Der Port erzeugt bewusst KEINEN Tuer-Ton; aot_common.c:574-585 begruendet das mit RE1.5-Belegen (Handler @0x800430bc fuehrt kein Sub aus; die Uebergangs-FSM FUN_8001c958 Zustand 1 stoppt den Ton mit FUN_80061fc0(-1)). Fuer RE1.5 stimmt das; fuer RE2 ist es falsch.

### Tuer-Ton: unterscheidet RE2 Holz/Metall/Gitter?  [byte-belegt]
Ja — aber nicht ueber eine Zuordnungstabelle im Code, sondern dadurch, dass jedes der 55 DO2-Archive seine EIGENEN 2 bis 3 VAG-Samples mitbringt. Die Auswahl geschieht allein ueber das Nutzlast-Byte +0x0C (dtex), das Archiv und Ton in einem Zug bestimmt. 40 Archive haben ts=3/vs=3, 13 haben ts=2/vs=2, DOOR25 und DOOR2A haben ts=3/vs=2. Alle 55 haben ps=1 (ein Programm).

**Beleg:** VAB-Header je Datei (Offset 0x22/0x24/0x26 = ps/ts/vs), python-Auszaehlung ueber alle 55 DO2: z.B. DOOR00 ps1 ts3 vs3 fsize 0x4af0; DOOR0E ps1 ts2 vs2 fsize 0x3a10; DOOR25 ps1 ts3 vs2 fsize 0x44c0; DOOR2A ps1 ts3 vs2 fsize 0x4010; DOOR36 ps1 ts2 vs2 fsize 0x52e0. Auswahl: FUN_80014cd0/FUN_80015064 indizieren beide mit `*(byte*)(DAT_800ce550 + 0xc)`.

**Port-Stand:** nicht implementiert

### Tuer-Schliess-Ton am Ende (RE2)  [byte-belegt]
Am Ende der Sequenz spielt RE2 zusaetzlich Bank 0 / Programm 1 als POSITIONIERTEN Ton (a1 = &DAT_800cfc30 = die eben gesetzte Ziel-Spawn-Position), aber nur wenn *(u16*)(DAT_800c3a80+0x248) != 0.

**Beleg:** FUN_8001417c @0x800141cc-0x800141f4: `lw v0,0x800c3a80 ; lhu v0,584(v0) ; beq v0,zero,0x800141fc ; lui a1,0x800d ; addiu a1,a1,-976 (=0x800cfc30) ; jal 0x8005ba28 ; lui a0,0x1` (a0 = 0x00010000 = Bank 0, Programm 1). DAT_800cfc30 wird in FUN_80026b7c mit `DAT_800cfc30 = (int)DAT_800cfc3c` (Spawn-X) gesetzt.

**Port-Stand:** nicht implementiert

### RE1.5 laedt seine Tuer-VAB, aber die Lade-Strings sind tot  [byte-belegt]
RE1.5 oeffnet die DOOR00-VAB sehr wohl (SPU-Adresse 0x1020, ID nach DAT_800b21ec), aber es gibt KEINE Funktion, die eine separate Tuer-Sound-Datei von CD holt: die Strings 'DOOR EDH' @0x80010e10 und 'DOOR VBD' @0x80010e1c haben in ghidra1_V2.txt keinen einzigen Xref. Der Ton steckt also — wie in RE2 — im DO2, nur ohne Nachlade-Pfad.

**Beleg:** FUN_800170e0 @0x800170e0: `if (-1 < DAT_800b21ec) { SsVabClose((short)DAT_800b21ec); DAT_800b21ec = -1; } do { sVar1 = SsVabOpenHeadSticky(DAT_800b2598,0,0x1020); DAT_800b21ec = (char)sVar1; } while (DAT_800b21ec == -1); SsVabTransBody(DAT_801a1008,(short)DAT_800b21ec); SsVabTransCompleted(1);`. Xrefs DAT_800b21ec: 0x8001712c/0x80017150/0x8001716c (FUN_800170e0), 0x800439e8/0x80043a00/0x80043aac, 0x80045064. Xref-Suche 'DOOR EDH'/'DOOR VBD' in ghidra1_V2.txt: nur die Datendefinition @0x80010e10, keine Code-Referenz.

**Port-Stand:** platform/psx/src/audio_psx.c bindet DOOR00.vh/.vb als SFX-Bank ein — das ist Port-Eigenbau fuer Se_on, nicht der Tuer-Ton.

### Ladepause: was passiert bei schnellem Laden  [byte-belegt]
Bei schnellem Laden gewinnt die ANIMATION: der Hauptthread haengt nach dem Raumladen in `while (DAT_800cfb74 & 0x2000000)` und kommt erst weiter, wenn die Demo ihr Skript zu Ende gespielt hat. Umgekehrt haelt bei langsamem Laden die Demo an: FUN_80013eb4 setzt DAT_800d4806 erst dann auf 0, wenn Bit 0x20000 von DAT_800cfbd8 geloescht ist (= die Sound-Uebertragung fertig), und dieses Bit wird von FUN_80014cd0 zu Beginn gesetzt und am Ende geloescht.

**Beleg:** FUN_80013eb4 @0x80013f54-0x80013f68: `lw v0,0(s0)=DAT_800cfbd8 ; lui v1,0x2 ; and v0,v0,v1 ; bne v0,zero,0x80013f6c ; sh zero,19502(s0)` (0x800cfbd8+0x4C2E = 0x800d4806). FUN_80014cd0: `DAT_800cfbd8 = DAT_800cfbd8 | 0x20000;` am Anfang, `DAT_800cfbd8 = DAT_800cfbd8 & 0xfffdffff;` am Ende. FUN_80013c1c: `DAT_800d4806 = 1;`. Hauptthread FUN_80026b7c: `while ((DAT_800cfbd8 & 0x20000) != 0) { FUN_80031f94(1); }` VOR dem Raumladen und `while ((DAT_800cfb74 & 0x2000000) != 0) { FUN_80031f94(1); }` DANACH.

**Port-Stand:** nicht implementiert (synchrones Laden, keine Kaschierung)

### Ladepause: Textkaschierung fuer zwei Spezial-Tueren  [byte-belegt]
Fuer dtex 0x32 und 0x34 zeichnet RE2 waehrend der Sequenz Text — und zwar in JEDEM Bild des Fensters 41 <= Bildzaehler <= 259 (219 Bilder). dtex 0x32: eine Zeile bei (32,200) = 'Please wait a minute'. dtex 0x34: zwei Zeilen bei (32,184) und (32,200) = 'The side pack is being' / 'equipped'.

**Beleg:** FUN_80013eb4 @0x80013f88-0x80014004: `lhu v0,558(v0) ; addiu v0,v0,-41 ; sltiu v0,v0,0xdb ; beq v0,zero,0x80014008 ; addiu v0,zero,50 ; lh v1,19506(s0) (=0x800d480a) ; bne v1,v0,0x80013fd4 ; addiu v0,zero,52` ; Zeichenaufrufe `addiu a0,zero,32 ; addiu a1,zero,200 ; lhu a2,-23448(a2) (=0x8009a468) ; addiu a3,zero,16384 ; jal 0x800313a4 ; addu a2,a2,s1 (s1=0x8009a42c)` bzw. `addiu a1,zero,184 ; lhu a2,0(s2) (=0x8009a46a)` und `addiu a1,zero,200 ; lhu a2,2(s2) (=0x8009a46c)`. Offsets: u16@0x8009a468 = 0x0000, @0x8009a46a = 0x0017, @0x8009a46c = 0x002F. Textbytes ab 0x8009a42c: `2c 48 41 3d 4f 41 00 53 3d 45 50 00 3d 00 49 45 4a 51 50 41 01 fe 00` / ab 0x8009a443: `30 44 41 00 4f 45 40 41 00 4c 3d 3f 47 00 45 4f 00 3e 41 45 4a 43 fe 00` / ab 0x8009a45b: `41 4d 51 45 4c 4c 41 40 01 fe 00`. Zeichensatz aus den Bytes selbst ableitbar: 'a'=0x3D (b=0x3E, e=0x41, l=0x48, s=0x4F, w=0x53, i=0x45, t=0x50, m=0x49, n=0x4A, u=0x51), 'A'=0x1D ('P'=0x2C, 'T'=0x30); 0x00 = Wortluecke, 0xFE = Zeilenumbruch, 0x01 = Ende.

**Port-Stand:** nicht implementiert

### Sonderfall dtex 0x28 in der VRAM-Ablage  [byte-belegt]
Genau eine Tuer, dtex 0x28 (DOOR28.DO2), legt ihre Textur an anderen VRAM-Koordinaten ab: statt (0x15, 0x1F) wird (0, 0) an FUN_80076b60 uebergeben.

**Beleg:** FUN_80013c1c: `if (*(char *)(DAT_800ce550 + 0xc) == '(') { uVar5 = 0; uVar8 = 0; } else { uVar5 = 0x15; uVar8 = 0x1f; } FUN_80076b60(1, iVar4, uVar5, uVar8, uVar9);` — '(' = 0x28.

**Port-Stand:** nicht implementiert

### Uebergeordnete Uebergangs-FSM (DAT_800df348, 7 Zustaende)  [byte-belegt]
Der Tuer-Trigger setzt DAT_800df348 = 1; die Haupt-Spielschleife fuehrt daraufhin eine 7-Zustands-Maschine ueber die Sprungtabelle @0x800109c4 aus: 1 -> 0x80025970 (wartet auf Blende, ruft FUN_80026b7c = die gesamte Tuer-/Raumwechsel-Sequenz, oder setzt Zustand 2), 2 -> 0x80025aa0, 3 -> 0x80025c5c, 4 -> 0x80025d3c, 5 -> 0x80025d64 (Blende fertig -> Zustand 0), 6 -> 0x80025d8c (Warteschleife), 7 -> 0x80025dc4. Waehrend DAT_800df348 != 0 ist Pausieren gesperrt.

**Beleg:** Sprungtabelle @0x800109c4: `0x80025970, 0x80025aa0, 0x80025c5c, 0x80025d3c, 0x80025d64, 0x80025d8c, 0x80025dc4`. Dispatch @0x8002594c-0x80025968: `addiu v1,v0,-0x1 ; sltiu v0,v1,0x7 ; beq v0,zero,LAB_80025de4 ; sll v0,v1,0x2 ; lui at,0x8001 ; addu at,at,v0 ; lw v0,0x9c4(at) ; jr v0`. Aufruf der Sequenz @0x80025a70: `jal FUN_80026b7c`. Pause-Sperre in main.c: `if (((((((DAT_800cfb74 & 0x40000000) == 0) || (DAT_800dfc1b != '\0')) || (DAT_800df348 != '\0')) || ...`.

**Port-Stand:** Port hat die RE1.5-Entsprechung teilweise: room_common.c s_trans_state (Zustaende 4/5) mit Zitaten @0x8001cc34/@0x8001cc70/@0x8001cc8c aus FUN_8001c958. Die RE2-Tabelle @0x800109c4 hat im Port kein Gegenstueck.

### Tuer-Demo laeuft als Koroutine auf Task-Slot 1  [byte-belegt]
FUN_80031f6c(slot, fn) traegt fn bei 0x800d76a8 + slot*128 ein und setzt den Zustand bei 0x800d76a4 + slot*128 auf 2; FUN_80031f94(n) ist der Yield/VSync-Schritt des Planers. Die Tuer-Demo laeuft damit echt nebenlaeufig zum CD-Laden. RE1.5 nutzt dieselbe Konstruktion mit FUN_80029a98 / FUN_80029ac8.

**Beleg:** FUN_80031f6c @0x80031f6c-0x80031f90: `sll a0,a0,7 ; addiu v0,zero,2 ; lui at,0x800d ; addu at,at,a0 ; sw a1,30376(at) ; lui at,0x800d ; addu at,at,a0 ; sh v0,30372(at) ; jr ra`. RE1.5 FUN_8001d600: `FUN_80029a98(1,&LAB_80016188);` und `FUN_80029ac8(1)` als Yield.

**Port-Stand:** nicht implementiert — der Port hat keinen Koroutinen-Planer fuer den Uebergang.

### Skript-VM der Tuer-Demo ist dieselbe wie die Raum-SCD-VM  [byte-belegt]
FUN_80014058 (RE2) und FUN_80016518 (RE1.5) dispatchen ueber PTR_LAB_800a74c8 bzw. PTR_LAB_800744a8, ueber die Objekt-Slots 10..13 (RE2 Schrittweite 0x174 ab 0x800d86e8, RE1.5 Schrittweite 0x170 ab 0x800b39ac). Die RE2-Tabelle hat mindestens 60 Eintraege. Die Tuer-Demo benutzt also keinen Sonder-Interpreter.

**Beleg:** FUN_80014058: `iVar1 = (*(code *)(&PTR_LAB_800a74c8)[**(byte **)((int)&DAT_800d787c + iVar3)])(puVar2);` mit `uVar4 = 10; iVar3 = 0xe88; ... iVar3 = iVar3 + 0x174; while (uVar4 < 0xe);` (0x800d7860+0xe88 = 0x800d86e8). FUN_80016518: `(*(code *)(&PTR_LAB_800744a8)[**(byte **)((int)&DAT_800b2b68 + iVar3)])(puVar2)` mit `iVar3 = 0xe60 ... + 0x170` (0x800b2b4c+0xe60 = 0x800b39ac). Tabellendump @0x800a74c8 liefert 60 gueltige EXE-Zeiger 0x800537e4..0x80058ce4.

**Port-Stand:** Der Port hat eine SCD-VM (re15_port/engine/src/scd_vm.c), aber sie wird nie fuer eine Tuer-Demo instanziiert.

### Was der Port gegenueber dem RE1.5-ORIGINAL fehlt  [byte-belegt]
Der Port setzt aus FUN_8001d600 nur den Nutzlast-Warp und die Blenden-FSM um. Fehlend, obwohl im RE1.5-Auslieferungsstand vorhanden und ausgefuehrt: (1) FUN_800171f4 = DOOR TEXTURE nach 0x801a1000 laden; (2) FUN_800161e0 = Modell/TIM/VAB entpacken, Kamera 30000/22000, SetBackColor(0xff,0xff,0xff), Bildschirmmitte (160,120); (3) FUN_80029a98(1,&LAB_80016188) = Demo-Koroutine; (4) die 0x2000000-Warteschleife (der Raumwechsel wartet im Original auf die Demo); (5) FUN_800170e0 = SsVabOpenHeadSticky der Tuer-VAB auf SPU 0x1020; (6) DAT_80072ec4 = 0x3c am Ende. Weil RE1.5s einziges Tuer-Skript nur ein END enthaelt, ist der SICHTBARE Unterschied klein (ein Bild), aber die Zustandsfolge ist nicht byte-true.

**Beleg:** RE_15_Quellcode_V2/FUN_8001d600.c (vollstaendiger else-Zweig, oben zitiert) gegen re15_port/engine/src/aot_common.c:468-600 (aot_fire_door) und re15_port/engine/src/room_common.c:142-232 (Transitions-FSM + re15_room_apply_pending). Suche `grep -rn "DO2\|door_demo\|door_seq" re15_port/engine re15_port/include` = 0 Treffer.

**Port-Stand:** selbst nachgelesen: aot_common.c ruft re15_room_request_change(dest_id, spawn, yaw, cut); room_common.c laedt die RDT synchron und faehrt re15_fade_config(0,2,7,-0x1800,0) + re15_fade_kick(0,0), s_trans_state 4->5->0.

### Was RE2 gegenueber RE1.5 anders macht — Kurzliste  [byte-belegt]
(1) SCE-Index der Tuer: 1 statt 2. (2) Vollstaendige Schloss-/Schluessel-Logik im Handler statt gar keiner. (3) 55 Tuer-Archive statt 1, mit funktionierender dtex-Tabelle. (4) Tuer-Sound-Chunk wird getrennt vom Textur-Chunk von CD geholt (FUN_80014cd0 mit eigener 'DOOR SOUND'-Anfrage), RE1.5 laedt die ganze Datei in einem Rutsch. (5) Ausblende + Schliess-Sound im Exit der Demo. (6) Ladekaschierungs-Text fuer dtex 0x32/0x34. (7) VRAM-Sonderfall dtex 0x28. (8) Raum-ID-Kodierung (stage+1)*0x100+room statt der RE1.5-Nibble-Form. (9) Ein Elevator-/Sonderpfad (FUN_80015128 / Koroutine LAB_800335c0, Abbruch auf DAT_800dcc1e == 4), den RE1.5 nicht hat.

**Beleg:** Alle Einzelbelege stehen in den vorherigen Befunden; zum Elevator-Pfad: FUN_80026b7c `if ((DAT_800cfb74 & 0x1000000) != 0) { FUN_80015128(); } ... if ((uVar1 == 0) || (iVar3 = FUN_80077360(&DAT_800d4a2c,0x3f), iVar3 == 0)) { ...LAB_80013bc4... } else { DAT_800dcc1e = '\0'; DAT_800cfbf2 = (byte)DAT_800d4820; FUN_80031f6c(1,&LAB_800335c0); while (((DAT_800cfb74 & 0x2000000) != 0 && (DAT_800dcc1e != '\x04'))) { FUN_80031f94(1); } }`.

**Port-Stand:** nicht implementiert

## Fehlende Daten
- info/re2leon/COMMON/BIN/STAGE1..7.BIN sind 420..9984 Byte gross und enthalten NICHT den Stage-Overlay-Code. Die RE2-Raum-Overlays, die FUN_8004a3c0 beim Stage-Wechsel nachlaedt, liegen damit nicht im Repo — ohne sie ist nicht pruefbar, ob ein Overlay die Tuer-Sequenz zusaetzlich beeinflusst.
- RE2_Quellcode_Overlays/STAGE1..7/ ist bis auf 4 Dateien in STAGE6 leer. Es fehlen die decompilierten Overlay-Funktionen, in denen RE2-Raum-Skripte Aot_set mit sce=1 aufrufen — dadurch ist die tatsaechliche Verteilung der dtex-Werte 0x00..0x36 ueber die 495 RDTs nicht aus Code belegbar, nur aus den RDT-Bytes.
- Ein RDT-AOT-Zensus fehlt: die konkreten Nutzlast-Bytes +0x0C..+0x10 aller Tuer-AOTs in info/re2leon/PL0/RDT/*.RDT wurden hier NICHT ausgelesen. Ohne ihn ist die Zuordnung dtex -> Raum/Tuer-Material (Holz/Metall/Gitter) nicht belegt, nur der Mechanismus.
- Die Bedeutungen der Skript-Opcodes 0x13, 0x18, 0x1A, 0x2E, 0x2F, 0x30, 0x31, 0x4D, 0x53 der Tuer-/Raum-VM (Tabelle @0x800a74c8) wurden nicht disassembliert. Belegt sind nur 0x01 (END, @0x800537fc), 0x09/0x0A (sleep, @0x800539dc/@0x80053a24) und 0x36 (Sound, @0x80056428). Die exakte Bild-fuer-Bild-Choreografie (wann die Tuer aufschwingt, wann durchgeschritten wird) steht in diesen Opcodes.
- Die Semantik von FUN_80053ca4 (RE1.5) bzw. FUN_80076cb0 (RE2) wurde nicht disassembliert. Damit ist zwar belegt, WELCHE Zahlen die Kamera bekommt (30000/22000 bzw. 10000/0), aber nicht, ob sie x/y/z oder Abstand/Winkel bedeuten.
- Die 5 Modi der Sprungtabelle @0x80011458 im Sound-Opcode 0x36 sind nicht aufgeloest — der Positionsbezug des Tuer-Tons (2D vs. 3D) ist damit unbelegt.
- Kein RE2-Savestate im Repo. Die tatsaechliche Bildlaenge einer Tuer-Sequenz im laufenden Spiel (inkl. der Wartezeit auf das CD-Laden) ist daher nur aus den Skript-Schlafwerten abgeleitet, nicht gemessen.

## Gegenpruefung — GEKIPPT

### [19] Verteilung ts/vs ueber die 55 DO2
Die Aggregat-Zahlen stimmen nicht. Behauptet: 40x ts=3/vs=3, 13x ts=2/vs=2, 2x ts=3/vs=2. Gemessen: 41 / 12 / 2. Die genannten Einzelbeispiele (DOOR00, 0E, 25, 2A, 36) sind alle korrekt — nur die Auszaehlung ist um je 1 daneben.

**Gegenbeleg:** Eigene Auszaehlung aller 55 Dateien, VabHdr ps/ts/vs bei Datei-Offset 0x22/0x24/0x26: ts3vs3 = DOOR00-0D,10,11,13,14,15,17-1F,22,23,24,26,27,28,29,2C,2D,2E,2F,30,31 = 41 Stueck. ts2vs2 = DOOR0E,0F,12,16,20,21,2B,32,33,34,35,36 = 12 Stueck. ts3vs2 = DOOR25,2A = 2. Summe 55. ps=1 gilt tatsaechlich fuer alle 55 (selbst geprueft), das Wort bei Datei+0xC30 ist ueberall 0x10.

### [7] "Routine 1 = 180 Bilder, Routine 2 = 230 Bilder"
Die SCHLAFWERTE sind byte-belegt, die SUMME ist abgeleitet und als byte-belegt verkauft. Beide Routinen rufen per Opcode 0x18 weitere Routinen auf (R1: 18 08, 18 03, 18 04, 18 05, 18 06), deren Opcodes 0x2E/0x2F/0x30/0x31/0x4D/0x53 der Ermittler selbst als un-disassembliert auffuehrt. Routine 5 traegt zudem einen EIGENEN Schlaf `09 0a 12 00` (=18 Bilder). Ohne Nachweis, dass keine dieser Routinen yieldet, ist "180"/"230" keine Dauer, sondern eine Addition dreier Konstanten.

**Gegenbeleg:** Eigener Dump des Skriptblocks DOOR00.DO2 @0x5008: Routine 1 (Blockoffset 0x2e) enthaelt neben `09 0a 46 00`/`09 0a 32 00`/`09 0a 3c 00` die Gosubs `18 08`,`18 03`,`18 04`,`18 05`,`18 06`. Routine 5 (Blockoffset 0x12c) enthaelt `09 0a 12 00`. Dass 0x18 ein Gosub ist, habe ich am Handler idx24 @0x800541a8 bewiesen: `lbu a2,1(v1) ; lw v1,-29508(v1)=0x800d8cbc ; sll a2,a2,1 ; addu a2,a2,v1 ; lhu v0,0(a2)` — das Argument indiziert die Routinen-Offsettabelle.

### [1] "kopiert genau 0xC30 Byte"
Es sind 0xC38 Byte. Der Ermittler hat die Schleifen-Obergrenze des Decompilats fuer die Gesamtlaenge gehalten. Nach BEIDEN Kopierschleifen steht ein unbedingter Nachlauf von 8 Byte. Genau diese 8 Byte sind der Grund, warum der anschliessende `lw` auf Block+0xC30 ueberhaupt kopierte Daten liest — bei nur 0xC30 kopierten Byte laege der gelesene Wert hinter dem Ziel.

**Gegenbeleg:** Eigener Disasm FUN_80014cd0: unaligned-Pfad @0x80014df4-0x80014e3c (16 B/Iteration, Schranke `addiu a3,s0,3120` = +0xC30), aligned-Pfad @0x80014e48-0x80014e74 (gleiche Schranke); BEIDE fallen auf 0x80014e78-0x80014e94 durch: zwei lwl/lwr-Paare + zwei swl/swr-Paare = 8 weitere Byte. Erst danach @0x80014ea4 `lw v0,-15568(0x8020)` = 0x801fc330 = Ziel(0x801fb700)+0xC30.

### [17] RE1.5-Haelfte der Raum-ID-Kodierung
Fuer "RE1.5 als ((stage+1)<<12) | (room<<4) | Variante" wird KEIN einziges RE1.5-Byte zitiert — nur die RE2-Formel. Im RE1.5-Tuerpfad existiert gar keine kombinierte Raum-ID: Stage, Raum und Cut landen als DREI getrennte Halbworte. Der Ermittler hedged mit "(bzw. der Port)" und stuft die Aussage trotzdem als byte-belegt ein. Die RE2-Haelfte ist korrekt.

**Gegenbeleg:** Eigener Disasm FUN_8001d600: @0x8001d94c `lbu v0,9(a0)` -> @0x8001d95c `sh v0,4066(at)` = 0x800b0fe2 (Raum); @0x8001d930 `lbu v1,10(a0)` -> @0x8001d948 `sh v1,4068(at)` = 0x800b0fe4 (Cut); @0x8001d970 `lbu v0,8(a0)` -> @0x8001d97c `sh v0,4064(at)` = 0x800b0fe0 (Stage). Keine Schiebe-/Oder-Kombination im gesamten Zweig. Gegenprobe RE2 @0x80026d44-0x80026d68: `addiu v1,a2,1 ; sll v1,v1,8 ; lhu v0,0x800d481e ; addu v0,v0,v1 ; sh v0,0x800d4822`.

### [10] RE1.5-Kamera "Auge/Ziel-Konstanten ... in die Struktur @0x800b220c"
0x800b220c wird vom zitierten Codeblock GAR NICHT geschrieben; die Region wird vorher pauschal genullt. Die Konstanten landen auf +4 und +0x10 der uebergebenen Struktur. Welches Feld Auge und welches Ziel ist — und ob 30000/22000 ueberhaupt auf derselben Achse liegen — folgt aus den Bytes nicht; der Ermittler raeumt selbst ein, FUN_80053ca4 nie disassembliert zu haben, behauptet aber trotzdem "Abstand 8000". Das ist ein Modell, kein Messwert.

**Gegenbeleg:** Eigener Disasm FUN_800161e0 @0x80016270-0x80016298: `addiu v1,v1,8716 (=0x800b220c) ; addiu a0,v1,46 ; sh zero,0(v1) ; addiu v1,v1,2 ; ...` — nullt 0x800b220c..0x800b223a. Erst danach @0x8001645c-0x80016498: `addiu v1,v1,8720 (=0x800b2210) ; sw 0x7530,0(v1) ; sw zero,0x800b2214 ; sw zero,0x800b2218 ; sw 0x55f0,0x800b221c ; sw zero,0x800b2220 ; sw zero,0x800b2224 ; jal 0x80053ca4 ; addiu a0,v1,-4`.

### [10] RE2-Kamera "laedt DAT_80010830..DAT_80010848"
Die Ladebasis ist 0x8001082c, nicht 0x80010830, und das erste geladene Wort ist 0x642D0000 — nicht "alle folgenden 0". Das Fazit (uebergebene Vektoren (10000,0,0) und (0,0,0)) ueberlebt, die zitierte Adressspanne nicht.

**Gegenbeleg:** Eigener Disasm @0x80013c2c: `addiu a1,a1,2092` = 0x8001082c; danach acht `lw` von a1+0..a1+28 nach sp+16..sp+44. Eigene Byte-Lesung ab 0x8001082c: `00 00 2d 64 | 10 27 00 00 | 00 00 ...`. Der Kamera-Aufruf @0x80013e38-0x80013e40 nimmt `addiu a0,sp,20` / `addiu a1,sp,32`, also die Woerter ab 0x80010830 bzw. 0x8001083c.

### [6] Mechanismus der Deaktivierung in RE1.5
Der zitierte Post-Check in FUN_80016518 ist die ZWEITE, redundante Loeschung. Primaer loescht der END-Handler selbst — genau die Konstruktion, die der Ermittler fuer RE2 (@0x800537fc) korrekt zitiert, fuer RE1.5 aber uebersieht und stattdessen durch den Nachlauf-Check ersetzt. Das Ein-Bild-Fazit bleibt richtig, die Begruendung ist falsch verortet.

**Gegenbeleg:** Eigener Disasm des RE1.5-Opcode-1-Handlers (PTR_LAB_800744a8[1] = 0x8003f1f0) @0x8003f1f0-0x8003f20c: `lb v1,2(a1) ; bne v1,zero,0x8003f210 ; addiu v1,v1,-1 ; sb zero,1(a1) ; j 0x8003f250 ; ori v0,zero,0x2` — baugleich zu RE2 @0x800537fc. Der Post-Check liegt separat @0x80016614-0x8001662c: `lw v0,28(s0) ; lbu v0,0(v0) ; bne v0,s3(=1),0x80016630 ; sb zero,1(s0)`.

### [13] "FUN_80016664 ... (vollstaendig, 13 Instruktionen)"
Die Funktion laeuft 0x80016664..0x800166c0, rund 23 Instruktionen. Die Zahl 13 ist falsch — und sie ist genau das Argument, mit dem "vollstaendig geprueft, kein Fade" belegt werden soll. Die inhaltliche Aussage haelt trotzdem.

**Gegenbeleg:** Eigener Disasm 0x80016664-0x800166b8 = 22 Instruktionen bis zum letzten sichtbaren `addiu sp,sp,24`; die einzigen jal sind 0x8002178c (@0x8001666c), 0x80029ac8 (@0x8001667c) und 0x80029afc (@0x800166ac). Kein Fade-Setzaufruf.

### [18] Tuer-Ton "direkt nach sleep(70)" + "+2 u16 Programm"
Zwischen Schlaf und Sound-Opcode steht `18 08` (Gosub Routine 8). Und das u16 bei +2 ist nicht "Programm": es wird gesplittet — nur das Low-Byte wird zum Programm (Bits 16-23), das High-Byte fliesst in die unteren 8 Bit von a0.

**Gegenbeleg:** Eigene Byte-Lesung DOOR00.DO2 Routine 1: `... 09 0a 46 00 | 18 08 | 36 00 00 00 00 00 00 00 00 00 00 00 ...`. Eigener Disasm 0x36-Handler @0x80056518-0x80056534: `sll v1,a3,24 ; andi v0,a0,0xff ; sll v0,v0,16 ; or v1,v1,v0 ; srl a0,a0,8 ; jal 0x8005ba28 ; or a0,v1,a0` mit a0 = `lh a0,2(s0)` @0x8005644c.

### [3] "55 ... lueckenlos aufsteigend" — 56. Eintrag verschwiegen
Unmittelbar vor der Chunk-Tabelle steht ein SECHSUNDFUENFZIGSTES u16 = 0x00EA. Die Aussage "55 Eintraege 0x00EA..0x0120, danach folgt direkt @0x8009a520 die Chunk-Tabelle" laesst 2 Byte unerklaert. Die 55 der Chunk-Tabelle ist dagegen bewiesen.

**Gegenbeleg:** Eigene Byte-Lesung 0x8009a510: `1a 01 1b 01 1c 01 1d 01 1e 01 1f 01 20 01 ea 00` — bei 0x8009a51e steht `ea 00`. 55 Eintraege ab 0x8009a4b0 belegen 110 Byte und enden bei 0x8009a51d. Gegenprobe fuer die Chunk-Tabelle: 0x8009a520 + 55*12 = 0x8009a7b4 = exakt die Koroutinen-Zeigertabelle aus [8].

### [0] "+0x08 u8 / +0x09 u8 = je eine Pruefbyte pro Chunk"
Der Byte-FLUSS ist belegt, die BEDEUTUNG "Pruefbyte" nicht — sie ist ein Etikett, das als Byte-Befund auftritt. Die Zelle DAT_800d531e ist nicht tuer-spezifisch: FUN_80012fb8 fuellt sie im Normalfall aus der globalen CD-Dateitabelle, und 20 Stellen im Spiel schreiben sie. Was der Vergleich prueft, wurde nicht nachgelesen. Abzustufen auf "unbenannter CD-Anforderungs-Parameter".

**Gegenbeleg:** Eigener Disasm: @0x80014d98 `lbu a3,8(s1)` -> @0x80014dc0 `sb a3,30(s5)` (s5=0x800d5300); @0x800150d0 `lbu v0,9(v1)` -> @0x800150ec `sb v0,21278(at)` = 0x800d531e. Default-Quelle derselben Zelle: FUN_80012fb8 @0x8001300c-0x80013014 `lbu v0,-30549(at)` = 0x800988ab -> `sb v0,30(s1)`. Auswertung @0x800131b8-0x800131c4: `lbu v1,0(s3) ; lbu v0,-1(s3) ; beq v1,v0,...`. Ghidra listet 20 Schreib-Xrefs auf DAT_800d531e.

## Gesamturteil der Gegenpruefung
Der Strang ist ueberwiegend solide — ungewoehnlich solide fuer diesen Umfang. Ich habe rund 35 Adressen selbst gefahren; die tragenden Mechanismen (Chunk-Tabelle 0x8009a520 mit 12 B Stride, Zwei-Chunk-Arithmetik ueber ALLE 55 Dateien, Init/Main/Exit-Tripel in beiden Spielen, SCE-Index 2 vs 1, Payload-Offsets, -0x708 pro Etage, Schlaf-Opcode 09/0A, Sound-Bank-Kodierung, dtex-0x28-Sonderfall, Ladekaschierungs-Text) haben alle gehalten, mehrere exakt bis auf die zitierten Register. Elf Punkte fallen oder muessen abgestuft werden. Zwei sind echte Substanzfehler: (a) [19] zaehlt die VAB-Verteilung falsch — 41/12/2 statt 40/13/2; (b) [17] verkauft die RE1.5-Haelfte der Raum-ID-Kodierung als byte-belegt, obwohl dafuer KEIN RE1.5-Byte zitiert wird und im Tuerpfad gar keine kombinierte ID existiert (Stage/Raum/Cut liegen als drei getrennte Halbworte @0x800b0fe0/0fe2/0fe4). Vier weitere sind Semantik-Etiketten im Byte-Gewand: [0] \"Pruefbyte\" (die Zelle DAT_800d531e ist global, 20 Schreib-Xrefs), [10] \"Auge/Ziel\" und \"Abstand 8000\" — der Ermittler raeumt selbst ein, FUN_80053ca4 nie gelesen zu haben, behauptet aber trotzdem eine Distanz —, [7] die Bildsummen 180/230 (drei Konstanten addiert, obwohl die dazwischenliegenden Gosubs unbekannt sind und Routine 5 einen eigenen Schlaf traegt). Drei sind Praezisionsfehler durch Abschreiben aus dem Decompilat statt Disassemblieren: [1] 0xC30 statt 0xC38 — der 8-Byte-Nachlauf @0x80014e78 fehlt, und genau er macht den anschliessenden lw auf +0xC30 gueltig; [10] Ladebasis 0x8001082c statt 0x80010830 mit einem verschwiegenen Nicht-Null-Wort; [13] \"13 Instruktionen\" fuer eine 23-Instruktionen-Funktion, ausgerechnet als Vollstaendigkeitsbeweis. Zwei sind Auslassungen: [3] der 56. u16-Eintrag 0x00EA @0x8009a51e, [6] der eigentliche Deaktivierungsort (END-Handler @0x8003f1f0 statt des redundanten Post-Checks). Positiv: [7]s unbelegte Behauptung \"Routine 0 startet je nach Bedingung Routine 1 oder 2\" habe ich selbst nachgewiesen — Opcode 0x13 @0x80054020 ist ein Switch auf *(s16*)(0x800d47ec + arg*2), mit arg=0x0c genau auf 0x800D4804 = Payload +0x0D & 0x7F, also den Tuer-Typ. Kein einziger Fall von \"falsche Binary\": RE2-Adressen wurden konsequent mit re2_disasm.py gefahren, RE1.5-Adressen mit re15_disasm.py.

---
