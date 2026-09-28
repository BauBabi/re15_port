# 04 - Katalog der 55 RE2-Tueren und Vorbild fuer das Schwenktor in ROOM1170

Stand 2026-09-28. Werkzeug `re15_port/tools/tor/tuerkatalog.py`, Messwerte `build/tor_1170/tuerkatalog.json`,
Bilder `build/tor_1170/katalog_*.png`. Gelesen wurden die ROHEN Archive `info/re2leon/COMMON/DOOR/DOORxx.DO2`
und `info/re2leon/PSX.EXE`; die entpackten Ordner `DOORxx/` dienten nur der Gegenprobe.

## 0. Ergebnis in Kuerze

- Bestes Vorbild ist **DOOR2E** (Gittertuer im Rahmen mit gerundeten Ecken), punktgleich dahinter **DOOR0A**
  (Zellentuer). Beide teilen sich dasselbe Skript: der Skriptblock (600 B) unterscheidet sich in 11 Bytes, alle
  in den Aufstell-Saetzen der Klinke. Ein Fluegel, Drehung um die Hochachse um **570 Einheiten = 50,10 Grad**
  in **80 Bildern** (Bild 130..209), Ablauf gesamt **291 Bilder**.
- Kein einziges der 55 RE2-Modelle ist hueft-hoch. Die 32 drehenden Fluegel sind 5639..6602 Einheiten hoch,
  die 6 schiebenden 6002..7163. Fuer die Bildaufteilung eines niedrigen Tors gibt es in RE2 **kein Vorbild**
  (Abschnitt 8, OFFEN).
- In keiner der 146 Varianten hat ein um y drehendes Objekt mit Mesh-Hoehe ab 3000 ein Elternobjekt. Feste Teile
  neben dem Fluegel sind eigene Wurzelobjekte (z. B. DOOR15 Variante 2: fester zweiter Fluegel).
- Die entpackten `DOORxxNN.c` lesen 16-Bit-Felder big-endian und Opcode 0x74 mit falscher Laenge; sie sind
  als Zahlenquelle unbrauchbar (Abschnitt 1.4).
- RE2 und RE1.5 belegen das Flag-Wort des Aufstell-Opcodes VERSCHIEDEN (Eltern-Bit 0x10 gegen 0x08). RE2-
  Skriptbytes lassen sich nicht unveraendert in den RE1.5-Opcode 0x4F uebernehmen (Abschnitt 1.3).

## 1. Grundlagen mit Beleg

Alle Adressen: RE2 Leon `info/re2leon/PSX.EXE`, t_addr 0x80010000. Nachlesen mit
`python .claude/skills/re15-psx-disasm/scripts/re2_disasm.py dis <adresse> <n>`.

### 1.1 Container

| Was | Wert | Beleg |
|---|---|---|
| Tuer-Tabelle | @0x8009a520, 55 Eintraege x 12 B | `re2_disasm.py bytes 0x8009a520 48`; Eintrag 0 = `08 4b d0 8d 0a 00 00 00 9a 42 00 00` |
| Feld +0 u16 | Groesse Klangteil (DOOR00: 0x4b08) | FUN_80014cd0: Index x 12 @0x80014d40..48, Basis @0x80014d50, `lhu s4,0(s1)` @0x80014d94, Ladeadresse 0x801a1000 - Wert @0x80014d7c / @0x80014da0 `subu s0,s0,s4` |
| Feld +2 u16 | Groesse Modellteil (DOOR00: 0x8dd0) | Messung: Beginn + Groesse = Dateilaenge bei 55/55 |
| Feld +4 u32 | Beginn Modellteil in Sektoren (DOOR00: 10 -> 0x5000) | Messung: = aufgerundete Klanggroesse bei 55/55; Verwendung im Lader nicht disassembliert |
| Modellteil +0 / +4 | u32 MD1-Offset / u32 TIM-Offset | FUN_80013c1c: DAT_801a1000 und DAT_801a1004 werden um 0x801a1000 verschoben |
| Modellteil +8 | SCD-Offsettabelle, u16, relativ zu +8 | @0x8001405c `lui v1,0x801a` / @0x80014060 `ori v1,v1,0x1008` / @0x80014098 `sw v1,-29508(at)` = 0x800d8cbc |
| Skriptzahl | erster Tabelleneintrag / 2 | Messung: alle 498 Skripte der 55 Archive zerfallen lueckenlos in bekannte Opcodes |

Nachmessen: `python re15_port/tools/tor/tuerkatalog.py` (letzte Zeile `Container/Skripte mit Bruch: []`).

### 1.2 Modell und Textur

| Was | Wert | Beleg |
|---|---|---|
| MD1-Kopf | 12 B (u32 Laenge, u32, u32 Objektzahl = 2 x Meshes) | @0x80013de4 `addiu v1,v1,12` / @0x80013de8 `sw v1,4(a0)`; DOOR00 @Datei 0x5224: `70 07 00 00 00 00 00 00 04 00 00 00` |
| Mesh-Schritt | 56 B | FUN_80014b40 @0x80014b60 `sll v0,a1,3` / @0x80014b64 `subu v0,v0,a1` / @0x80014b68 `sll v0,v0,3` |
| Vierecksteil | @+0x1c | @0x80014b74 `addiu v1,v1,28` |
| Renderer zeichnet nur Dreiecke | FUN_8001468c | @0x80014710 `lw fp,16(a2)` (Dreieckszeiger), @0x80014718 `lw t0,20(a2)` (Anzahl), Schleifenende @0x80014b04 `bne t0,zero,0x8001478c`, danach Epilog |
| Dreieckssatz | 12 B: n0 v0 n1 v1 n2 v2 (s16) | @0x8001475c `lh v0,2(fp)`, @0x80014760 `lh v1,6(fp)`, @0x80014770 `lh v0,10(fp)` = Vertex-Indizes |
| Vertex | 8 B: s16 x, y, z, Fuellwort | @0x80014764 `sll v0,v0,3`, Basis @0x80014720 `lw s5,0(a2)` |
| Alle 55 Modelle | 8560 Dreiecke, 0 Vierecke | Summe ueber `md1.tris_total` / `md1.quads_total` im JSON |
| Textur | bei 55/55: 128 x 256, 8 Bit, 1 CLUT mit 256 Farben | TIM-Kopf DOOR00 @Datei 0x5bb0: `10 00 00 00 09 00 00 00 0c 02 00 00 00 00 e0 01 00 01 01 00` |
| VRAM-Lage | Bild (0,0), CLUT (0,480) | derselbe Kopf; Dreiecke tragen clut=0x7800, tpage=0x0080 |
| Aufteilung | Blatt v = 0..218, Beschlag-Streifen v = 219..255 | DOOR2E: Mesh 0 uv (0,0)..(126,218), Mesh 1 uv (0,241)..(62,255) |

Nachmessen: `python re15_port/tools/tor/tuerkatalog.py --mesh 2E`.

### 1.3 Aufstell-Opcode 0x4D (RE2) gegen 0x4F (RE1.5)

RE2-Handler @0x80014ba4 (Tabelleneintrag @0x800a75fc), Laenge 22 B (@0x80014cac `addiu v0,a1,22`).
RE1.5-Handler @0x80016f20 (Tabelleneintrag @0x800745e4 = 0x800744a8 + 0x4F x 4), Laenge 22 B (@0x80017020
`addiu v0,a2,22`). Beide Handler selbst disassembliert.

| Satz-Offset | Feld | RE2 schreibt nach | RE1.5 schreibt nach |
|---|---|---|---|
| +1 u8 | Objektnummer 0..9 | Index in Zeigertabelle 0x800d4dd8 (@0x80014bb8) | Index in 0x800b23f4 |
| +2 u8 | unbenannt | obj+8 (@0x80014bd8) | obj+8 (@0x80016f54) |
| +3 u8 | Bildnummer fuer Flag 0x400 | obj+270 (@0x80014be4) | obj+9 (@0x80016f60) |
| +4 u8 | an/aus | obj+0 (@0x80014bf0) | obj+0 (@0x80016f6c) |
| +5 u8 | Mesh-Nummer | obj+326 (@0x80014bfc) | obj+142 (@0x80016f78) |
| +6 u16 | Flags | obj+324 (@0x80014c08) | obj+140 (@0x80016f84) |
| +8 s16 | unbenannt (in allen Kandidaten 16) | obj+16 (@0x80014c14) | obj+12 (@0x80016f90) |
| +10,+12,+14 s16 | Lage x, y, z | obj+56/60/64 (@0x80014c20/2c/38) | obj+52/56/60 (@0x80016f9c/a8/b4) |
| +16,+18,+20 u16 | Drehung x, y, z (4096 = 360 Grad) | obj+116/118/120 (@0x80014c44/54/60) | obj+104/106/108 (@0x80016fc0/cc/dc) |

| Flag-Bit | RE2 | RE1.5 |
|---|---|---|
| Eltern-Bindung | 0x0010 (@0x80014c64 `andi v0,v1,0x10`) | 0x0008 (@0x80016fe4 `andi a3,v0,0x8`) |
| Eltern-Nummer | Bits 0..3 (@0x80014c6c `andi v0,v1,0xf`) | Bits 0..2 (@0x80016ff4 `andi v0,v0,0x7`) |
| Elternmatrix | Eltern-Objekt + 84 (@0x80014c80) | Eltern-Objekt + 72 (@0x80017018) |
| ohne Eltern | Zeiger auf 0x800dcba8 = Kameramatrix (@0x80014c84/88) | Zeiger 0 (@0x80017004 `sw zero,116(a1)`) |
| 0x0800 | setzt Wort +584 des Tuer-Arbeitsblocks auf 1 (@0x80014c90..a8) | im Handler nicht vorhanden |

Weitere Flag-Bits, die der RE2-Renderer auswertet (FUN_80014234 / FUN_8001468c):

| Bit | Wirkung | Beleg |
|---|---|---|
| 0x0020 | Dreiecke werden unterteilt (DivideGT3) | @0x80014820 und @0x80014a30 `andi v0,v0,0x20`, Aufruf @0x80014a90 |
| 0x00c0 | Wahl der Ordnungstabelle: 0x80 -> (otz>>7)*4 + 2044, 0xc0 -> (otz>>7)*4, 0x40 -> zweite Tabelle, (otz>>12)*4 | @0x800149bc `andi v1,v0,0xc0`, @0x80014a14 `addiu v0,v0,2044` |
| 0x0100 | Farbe pulsiert (+8 je Bild, Dreieckswelle 0..255) | @0x80014468 `andi v0,v0,0x100` |
| 0x0400 | im Bild Nummer (Feld +3) wird Bit 0x80 umgeschaltet | @0x800146c8, @0x800146fc `ori v0,a0,0x80`, @0x80014700 `andi v0,a0,0xff7f` |
| 0x1000 | Grundhelligkeit 0x88 statt 0x44 | @0x800142b4 `andi v0,v0,0x1000` |
| 0x2000 | Vertex-z wird mit einem globalen Faktor gestaucht (>>8) | @0x80014794 `andi v0,v0,0x2000` |
| 0x4000 | halbdurchsichtig (GPU-Code 0x36 statt 0x34) | @0x8001473c `srl v0,v0,13` / @0x80014740 `andi v0,v0,0x2` / @0x80014744 `ori v0,v0,0x34` |

Bit 0x0200 (in den Wurzel-Flags 0x0a80/0x0aa0/0x0280 gesetzt) und 0x8000 (@0x80013f14) wurden nicht verfolgt - OFFEN.

### 1.4 Skript-Maschine

Die Tuerskripte laufen in der normalen RE2-SCD-Maschine (Dispatch-Tabelle @0x800a74c8, 0x8f Eintraege), aber auf
eigenen Ereignisplaetzen 10..13 (FUN_80014058 @0x80014068 `addiu s2,zero,10`, Schritt 0x174 B).
Ein Bild = ein Durchlauf der Schleife in FUN_80013eb4: @0x80013f6c Skripte, @0x80013f74 Zeichnen, danach
`FUN_80031f94(1)`. Rueckgabe eines Handlers: 1 = weiter im selben Bild, 2 = Bild beenden, 0 = Bedingung falsch.

| Op | Name | Laenge | Beleg PC-Vorschub | Wirkung (nur was die Bytes zeigen) |
|---|---|---|---|---|
| 0x00 | Nop | 1 | @0x800537ec addiu v0,v0,1 (Handler 0x800537e4) | nichts |
| 0x01 | Evt_end | 2 | @0x80053808 Ebene 0: sb zero,1(a3) -> Ereignis aus, sonst Ruecksprung @0x80053850 (Handler 0x800537fc) | Ebene 0: Ereignis beenden; sonst Ruecksprung aus Gosub |
| 0x02 | Evt_next | 1 | @0x80053868 addiu v0,v0,1; Rueckgabe 2 @0x80053874 (Handler 0x80053860) | Bild beenden (Rueckgabe 2) |
| 0x03 | Evt_chain | 4 | @0x80053888 lbu a1,3(v0); jal 0x800530ec (Neustart, kein Vorschub) (Handler 0x80053878) | dasselbe Ereignis mit Skript Byte+3 neu starten |
| 0x04 | Evt_exec | 4 | @0x800538bc addiu v0,v0,4 (Handler 0x800538a4) | Ereignis auf Platz Byte+1 mit Skript Byte+3 starten; laeuft noch im selben Bild, wenn der Platz hoeher liegt |
| 0x05 | Evt_kill | 2 | @0x80053914 addiu v0,v0,2 (Handler 0x800538dc) | Ereignis Byte+1 abschalten |
| 0x06 | Ifel_ck | 4 | @0x8005392c addiu a1,a2,4 (Handler 0x80053924) | Wenn-Block oeffnen, Sprungziel pc+4+u16@2 merken |
| 0x07 | Else_ck | 4 | @0x8005397c addu v1,v1,v0 (pc += u16@2) (Handler 0x80053964) | pc += u16@2, Wenn-Ebene schliessen |
| 0x08 | Endif | 2 | @0x800539b8 addiu v0,v0,2 (Handler 0x800539a0) | Wenn-Ebene schliessen |
| 0x09 | Sleep | 1 | @0x800539e4 addiu v0,a2,1; Zaehler = u16@2 @0x80053a10 (Handler 0x800539dc) | Zaehler = u16@2 setzen; die Bytes +1..+3 sind der Opcode 0x0A |
| 0x0A | Sleeping | 3 | @0x80053a6c addiu v0,v0,3 (Handler 0x80053a24) | Zaehler - 1, Bild beenden; bei 0 weiter. Sleep n kostet genau n Bilder |
| 0x0D | For | 6 | @0x80053b58 addiu t0,t0,6 (Handler 0x80053b1c) | Schleife: s16@2 Blocklaenge, u16@4 Anzahl; Anzahl 0 ueberspringt |
| 0x0E | Next | 2 | @0x80053d1c addiu v0,v0,2 (Handler 0x80053cbc) | Zaehler - 1; ungleich 0 -> Schleifenanfang |
| 0x0F | While | 4 | @0x80053da0 addiu a1,a1,4 (Handler 0x80053d3c) | Schleife mit Bedingung (Byte+1 = Laenge der Bedingung) |
| 0x10 | Ewhile | 2 | @0x80053e2c lw v0,32(v1) (Sprung zum While) (Handler 0x80053e0c) | zurueck zum While |
| 0x13 | Switch | 4 | @0x80054040 addiu a3,a3,4 (Handler 0x80054020) | Variable Byte+1 gegen die Case-Werte pruefen |
| 0x14 | Case | 6 | @0x80054100 addiu v0,v0,6 (Handler 0x800540f8) | direkt ausgefuehrt nur Vorschub (Durchfall) |
| 0x15 | Default | 2 | @0x80054118 addiu v0,v0,2 (Handler 0x80054110) | Vorschub |
| 0x16 | Eswitch | 2 | @0x8005414c addiu v0,v0,2 (Handler 0x80054128) | Blockebene schliessen |
| 0x17 | Goto | 6 | @0x80054190 addu a1,a1,t0 (pc += s16@4) (Handler 0x8005415c) | pc += s16@4 (ab Opcode), Ebenen aus Byte+1/+2 |
| 0x18 | Gosub | 2 | @0x800541b4 addiu v1,v1,2 (Handler 0x800541a8) | Unterprogramm Skript Byte+1 |
| 0x1A | Break | 2 | @0x80054290 lw v1,96(v1) (Sprung ans Blockende) (Handler 0x80054268) | ans Blockende springen |
| 0x1D | Work_copy | 4 | @0x800542c8 addiu a1,a1,4 (Handler 0x800542b4) | Variable Byte+1 in die SKRIPTBYTES bei pc+4+Byte+2 schreiben (8 oder 16 Bit) |
| 0x23 | Cmp | 6 | @0x80054484 addiu v0,v0,6 (Handler 0x80054474) | Vergleich Variable Byte+2, Operator Byte+3, Wert s16@4 |
| 0x24 | Save | 4 | @0x8005452c addiu v0,v0,4 (Handler 0x8005451c) | Variable Byte+1 = s16@2 |
| 0x25 | Copy | 3 | @0x8005455c addiu v0,v0,3 (Handler 0x8005454c) | Variable Byte+1 = Variable Byte+2 |
| 0x26 | Calc | 6 | @0x800545a4 addiu v0,v0,6 (Handler 0x8005458c) | Variable Byte+3 (Operator Byte+2) s16@4 |
| 0x2E | Work_set | 3 | @0x80055928 addiu v0,v0,3 (Handler 0x80055904) | Arbeitsobjekt waehlen (Typ 5 = Tuerobjekt Byte+2) und die 12 Geschwindigkeiten loeschen |
| 0x2F | Speed_set | 4 | @0x80055a94 addiu v0,v0,4 (Handler 0x80055a84) | Geschwindigkeit[Byte+1] = s16@2; Index 0..2 Lage, 3..5 Drehung, 6..8 Beschleunigung Lage, 9..11 Beschleunigung Drehung |
| 0x30 | Add_speed | 1 | @0x80055b1c addiu v0,v0,1 (Handler 0x80055ab0) | Lage += v[0..2], Drehung += v[3..5] |
| 0x31 | Add_aspeed | 1 | @0x80055b8c addiu v1,v1,1 (Handler 0x80055b2c) | v[0..5] += v[6..11] |
| 0x34 | Member_set | 4 | @0x80055c30 addiu v0,v0,4 (Handler 0x80055c00) | Feld des Arbeitsobjekts = s16@2 (Feld 13 = z) |
| 0x35 | Member_set2 | 3 | @0x80055c90 addiu v0,v0,3 (Handler 0x80055c50) | Feld = Variable |
| 0x36 | Se_on | 12 | @0x8005653c addiu v1,s0,12 (Handler 0x80056428) | Klang ausloesen |
| 0x3D | Member_copy | 3 | @0x80055e54 addiu v0,v0,3 (Handler 0x80055e38) | Variable = Feld |
| 0x4D | Door_model_set | 22 | @0x80014cac addiu v0,a1,22 (Handler 0x80014ba4) | Tuerobjekt aufstellen (Abschnitt 1.3) |
| 0x53 | Sce_fade_set | 6 | @0x80057fa8 addiu v0,s2,6 (Handler 0x80057ef0) | Blende setzen (Byte+1, Byte+2, Byte+3, s16@4) |
| 0x74 | Sce_fade_adjust | 4 | @0x80058004 addiu s0,s0,4 (Handler 0x80057fd8) | Blende nachstellen (Byte+1, s16@2) |
| 0x8A | Op8A | 6 | @0x80059378 addiu v1,v1,6 (Handler 0x80059348) | nicht untersucht |
| 0x8B | Op8B | 6 | @0x800593c8 addiu v1,v1,6 (Handler 0x80059394) | nicht untersucht |
| 0x8C | Op8C | 8 | @0x8005941c addiu v1,v1,8 (Handler 0x800593e4) | nicht untersucht |

Belege fuer die Wirkung: Work_set @0x80055904..0x800559c8 (Sprungtabelle @0x800111f0, Typ 5 -> @0x800559b0
`lw v0,19928(at)`); Speed_set @0x80055aa0 `sll v1,v1,1` / @0x80055aac `sh a1,344(v1)`; Add_speed @0x80055ab0..
(Lage +56/+60/+64, Drehung +116/+118/+120); Add_aspeed @0x80055b2c.. (344+2i += 356+2i); Sleep/Sleeping
@0x800539dc/@0x80053a24; For/Next @0x80053b1c/@0x80053cbc.

Variablen (s16-Feld ab 0x800d47ec), gesetzt in FUN_80013c1c:

| Variable | Inhalt | Beleg |
|---|---|---|
| 0x0C | Tuer-Variante = Byte +13 des Tuerdatensatzes (Zeiger 0x800ce550) ohne Bit 7 | @0x80013e5c `lbu v0,13(a0)`, @0x80013e6c `andi v0,v0,0xff7f`, @0x80013e74 -> 0x800d4804 |
| 0x0D | 1, bis der Klangteil geladen ist, dann 0 | @0x80013e68 -> 0x800d4806; geloescht @0x80013f68, sobald Bit 0x20000 von 0x800cfbd8 faellt |
| 0x0E | Bit 7 desselben Bytes | @0x80013e84 `andi v0,v0,0x80`, @0x80013e8c -> 0x800d4808 |
| 0x0F | Tuernummer = Byte +12 | @0x80013e90 `lbu v0,12(a0)`, @0x80013e98 -> 0x800d480a |

**Die entpackten `.c` sind falsch gelesen.** Beispiel DOOR00 Skript 1, Datei 0x5036:
`4d 00 00 00 01 00 a0 0a 10 00 d0 07 ce 0e 00 08 ...`. Die `.c` nennt `40970` (= 0xA00A, big-endian gelesen);
der Handler liest mit `lhu v1,6(a1)` (@0x80014c00) little-endian 0x0AA0 = 2720. Ebenso `Sleep(17920)` = 0x4600
statt 70, `Speed_set(4, 1536)` statt 6. Opcode 0x74 ist 4 B lang (@0x80058004 `addiu s0,s0,4`), nicht 5.

### 1.5 Kamera der Tuersequenz

| Was | Wert | Beleg |
|---|---|---|
| Auge | (10000, 0, 0) | @0x80010830 `10 27 00 00 00 00 00 00 00 00 00 00`; FUN_80013c1c kopiert ab @0x80013c30, Uebergabe @0x80013e38 `addiu a0,sp,20` |
| Ziel | (0, 0, 0) | @0x8001083c zwoelf Nullbytes; @0x80013e40 `addiu a1,sp,32` |
| Blickmatrix | x_cam = z, y_cam = y, z_cam = 10000 - x | aus FUN_80076cb0 mit diesen Werten gerechnet (dx = -10000, dy = dz = 0) |
| Projektionsabstand | 290 | @0x80013e30 `jal 0x8008de24` / @0x80013e34 `addiu a0,zero,290`; 0x8008de24 = `ctc2 a0,26` |
| Drehmatrix | M = Rx * Ry * Rz, m[0][2] = +sin y | RotMatrix @0x8008e1f4, @0x8008e2c8 `sh t6,4(a1)` |
| Verkettung | Welt = Eltern * (Drehung, Lage) | FUN_80014234: Spalten der lokalen Matrix durch die Elternmatrix, Lage per `rt` |

Folge fuer die Bilder: Bildschirm-rechts = +z, Bildschirm-unten = +y, Tiefe = 10000 - x. Die Bildmitte (160,120)
in den Uebersichtsbildern ist eine ANNAHME (GTE-Offset nicht nachgelesen); sie beruehrt keine Modellzahl.

## 2. Katalog der 55 Tueren

Spalten: Meshes = Dreiecke je Mesh; Groesse = Huellquader (x Dicke, y Hoehe, z Breite) des groessten aufgestellten
Meshes; Skr = Zahl der Skripte; Var = Variante (Wert von Variable 0x0C) mit Zahl der aufgestellten Objekte und
Bildzahl; Bewegung aus der Simulation der Bytes. Winkel in Grad (Einheiten x 360 / 4096).

| Tuer | Meshes (Dreiecke) | Groesse x,y,z | Skr | Varianten: Objekte / Bilder | Bewegung (Variante 0 bzw. erste) | Textur 128x256 |
|---|---|---|---|---|---|---|
| DOOR00 | 12, 33 | 288, 6602, 3599 | 9 | V0: 2 / 301; V1: 3 / 301 | Drehen Hochachse einfluegelig (90.53 Grad); Klinke/Anbauteil dreht (1 Obj.); Kamerafahrt x (9450) | Dunkelrote Holztuer mit Kreuzfries und Messingbeschlag (Standardtuer; MD1 bytegleich mit RE1.5 DOOR00). |
| DOOR01 | 12, 100, 100, 33 | 288, 6602, 3599 | 15 | V0: 2 / 301; V1: 3 / 301; V2: 5 / 281; V3: 4 / 281 | Drehen Hochachse einfluegelig (90.53 Grad); Klinke/Anbauteil dreht (1 Obj.); Kamerafahrt x (9450) | Rotbraune Holztuer mit drei liegenden Kassetten. |
| DOOR02 | 12, 68 | 288, 6602, 3599 | 9 | V0: 2 / 301; V1: 3 / 301 | Drehen Hochachse einfluegelig (90.53 Grad); Klinke/Anbauteil dreht (1 Obj.); Kamerafahrt x (9450) | Schwarzgraue Holztuer mit vier hohen Kassetten. |
| DOOR03 | 12, 33 | 288, 6602, 3599 | 10 | V0: 3 / 301; V1: 3 / 301 | Drehen Hochachse einfluegelig (90.53 Grad); Klinke/Anbauteil dreht (2 Obj.); Kamerafahrt x (9450) | Braune Holztuer mit sechs Feldern und Rautenfuellung. |
| DOOR04 | 12, 196 | 288, 6602, 3599 | 13 | V0: 2 / 301; V1: 3 / 301; V2: 5 / 246; V3: 5 / 296 | Drehen Hochachse einfluegelig (90.53 Grad); Kamerafahrt x (9450) | Blau gestrichene Holztuer mit sechs Kassetten und zwei Messingplaettchen. |
| DOOR05 | 12, 33, 100, 100 | 288, 6602, 3599 | 10 | V0: 2 / 301; V1: 3 / 301 | Drehen Hochachse einfluegelig (90.53 Grad); Klinke/Anbauteil dreht (1 Obj.); Kamerafahrt x (9450) | Dunkelbraune Holztuer mit drei grossen Querfeldern. |
| DOOR06 | 12, 36 | 288, 6602, 3599 | 13 | V0: 2 / 311; V1: 3 / 311; V2: 4 / 291; V3: 5 / 311 | Drehen Hochachse einfluegelig (90.53 Grad); Kamerafahrt x (9450) | Stahltuer mit aufgesetztem X-Kreuz und Riegelkasten. |
| DOOR07 | 12, 16 | 288, 6602, 3599 | 9 | V0: 2 / 301; V1: 2 / 301 | Drehen Hochachse einfluegelig (51.68 Grad); Klinke/Anbauteil dreht (1 Obj.); Kamerafahrt x (7140) | Graue, fleckige Blechtuer mit Lueftungsschlitzen unten. |
| DOOR08 | 12, 68 | 288, 6602, 3599 | 10 | V0: 3 / 301; V1: 3 / 296 | Drehen Hochachse einfluegelig (90.53 Grad); Klinke/Anbauteil dreht (2 Obj.); Kamerafahrt x (9450) | Genietete Stahltuer mit zwei vertieften Feldern, angerostet. |
| DOOR09 | 12, 68 | 288, 6602, 3599 | 10 | V0: 2 / 301; V1: 3 / 301 | Drehen Hochachse einfluegelig (90.53 Grad); Klinke/Anbauteil dreht (1 Obj.); Kamerafahrt x (9450) | Olivbraune Holztuer mit sechs Kassetten. |
| DOOR0A | 300, 116 | 329, 6000, 3000 | 9 | V0: 2 / 291; V1: 3 / 291 | Drehen Hochachse einfluegelig (50.1 Grad); Klinke/Anbauteil dreht (1 Obj.); Kamerafahrt x (7140) | Rostige Gittertuer (Zellentuer): senkrechte Staebe, Querband mit Schlosskasten. |
| DOOR0B | 12, 36 | 288, 6602, 3599 | 9 | V0: 2 / 301; V1: 3 / 301 | Drehen Hochachse einfluegelig (90.53 Grad); Kamerafahrt x (9450) | Holztuer mit zwei Glasfenstern oben und Rautenkassetten unten. |
| DOOR0C | 12, 36 | 288, 6602, 3599 | 7 | V0: 6 / 307; V1: 4 / 307 | Drehen Hochachse zweifluegelig (-90.53/90.53 Grad); Kamerafahrt x (3960/3960) | Dunkelgruene Holztuer mit Glasfeld oben und Stangengriff. |
| DOOR0D | 12, 33 | 288, 6602, 3599 | 14 | V0: 2 / 311; V1: 3 / 311; V2: 6 / 327; V3: 4 / 327 | Drehen Hochachse einfluegelig (90.53 Grad); Klinke/Anbauteil dreht (1 Obj.); Kamerafahrt x (9450) | Helle Eichentuer mit drei Querkassetten. |
| DOOR0E | 258 | 63552, 18000, 11200 | 4 | V4: 1 / 169; V5: 1 / 165 | Heben/Senken y (1360); Kamerafahrt x (5820) | Treppenlauf aus grauem Stein mit Metallwange (kein Tuerblatt). |
| DOOR0F | 48 | 19440, 12960, 13500 | 4 | V4: 1 / 169; V5: 1 / 165 | Heben/Senken y (1360); Kamerafahrt x (5820) | Treppenlauf mit rotbraunen Holzdielen (kein Tuerblatt). |
| DOOR10 | 26, 12 | 125, 7163, 761 | 11 | V0: 10 / 301; V1: 10 / 301 | Schieben z (-495/495/495); Kamerafahrt x (5830) | Schmiedeeisernes Scherengitter: Rankenornament, Lanzenstab, Scherenglieder auf Schwarz. |
| DOOR11 | 12, 68 | 288, 6602, 3599 | 13 | V0: 4 / 291; V2: 4 / 291; V1: 5 / 311; V3: 5 / 311 | Drehen Hochachse einfluegelig (90.53 Grad); Klinke/Anbauteil dreht (1 Obj.); Kamerafahrt x (9450/9450) | Rote Holztuer mit Lueftungslamellen unten und Drueckergarnitur. |
| DOOR12 | 48 | 19440, 12960, 13500 | 4 | V4: 1 / 169; V5: 1 / 165 | Heben/Senken y (1360); Kamerafahrt x (5820) | Treppenlauf aus gruenlich verwittertem Stein (kein Tuerblatt). |
| DOOR13 | 12, 68 | 288, 6602, 3599 | 9 | V0: 3 / 286; V1: 3 / 286 | Drehen Hochachse einfluegelig (90.53 Grad); Klinke/Anbauteil dreht (2 Obj.); Kamerafahrt x (9450) | Dunkle Stahltuer mit Gitterfenster oben. |
| DOOR14 | 114, 12 | 168, 6061, 5857 | 9 | V0: 2 / 301; V1: 2 / 301 | Schieben z (-3875); Kamerafahrt x (9450) | Maschendraht-Tor im Stahlrahmen, zwei Felder mit Rautengitter. |
| DOOR15 | 12, 96 | 288, 6602, 3599 | 14 | V0: 3 / 286; V1: 3 / 286; V2: 4 / 291; V3: 5 / 311 | Drehen Hochachse einfluegelig (90.53 Grad); Kamerafahrt x (9450) | Bretterverschlag mit X-Streben und braun-schwarz schraffiertem Blechfeld. |
| DOOR16 | 280 | 156, 10820, 1434 | 5 | V4: 1 / 331; V5: 1 / 331 | Heben/Senken y (300); Klinke/Anbauteil dreht (1 Obj.); Kamerafahrt x (6950) | Leiter aus Stahlrohr vor dunklem Grund. |
| DOOR17 | 12, 24, 96 | 288, 6602, 3599 | 9 | V0: 3 / 286; V1: 5 / 286 | Drehen Hochachse einfluegelig (90.53 Grad); Klinke/Anbauteil dreht (1 Obj.); Kamerafahrt x (9450) | Alte Bohlentuer mit Eisenbaendern und Ringgriff. |
| DOOR18 | 12, 33 | 288, 6602, 3599 | 9 | V0: 2 / 301; V1: 3 / 301 | Drehen Hochachse einfluegelig (90.53 Grad); Klinke/Anbauteil dreht (1 Obj.); Kamerafahrt x (9450) | Olivgruene Holztuer mit vier Feldern. |
| DOOR19 | 12, 137, 24, 137 | 288, 6602, 3599 | 10 | V0: 3 / 301; V1: 3 / 301 | Schieben z (4000); Klinke/Anbauteil dreht (1 Obj.); Kamerafahrt x (8550) | Beige Blech-Schiebetuer mit Griffmulde und Schlitzen. |
| DOOR1A | 12, 60 | 288, 6602, 3599 | 9 | V0: 2 / 301; V1: 3 / 301 | Drehen Hochachse einfluegelig (90.53 Grad); Klinke/Anbauteil dreht (1 Obj.); Kamerafahrt x (9450) | Graue Stahlrahmentuer mit drei Feldern und Drehknauf. |
| DOOR1B | 12, 144 | 288, 6602, 3599 | 7 | V0: 6 / 307; V2: 6 / 307; V1: 4 / 307; V3: 4 / 307 | Drehen Hochachse zweifluegelig (-90.53/90.53 Grad); Kamerafahrt x (3960/3960) | Braune Blechtuer mit kleinem Sichtfenster. |
| DOOR1C | 12, 68 | 288, 6602, 3599 | 9 | V0: 2 / 301; V1: 3 / 301 | Drehen Hochachse einfluegelig (90.53 Grad); Klinke/Anbauteil dreht (1 Obj.); Kamerafahrt x (9450) | Beige Blechtuer mit vergittertem Sichtfenster und Klappe unten. |
| DOOR1D | 12, 16 | 288, 6602, 3599 | 17 | V0: 2 / 301; V1: 3 / 301; V2: 4 / 291; V3: 5 / 311 | Drehen Hochachse einfluegelig (90.53 Grad); Klinke/Anbauteil dreht (1 Obj.); Kamerafahrt x (9450) | Rotbraune Blechtuer mit Lueftungsgitter oben. |
| DOOR1E | 304, 32 | 6000, 1200, 1800 | 10 | V0: 2 / 301; V1: 2 / 301; V6: 2 / 301; V7: 1 / 301; V8: 4 / 301 | Klinke/Anbauteil dreht (1 Obj.); Kamerafahrt x (4320/4320) | Schott: Gitterstaebe oben, genietete Stahlplatten unten. |
| DOOR1F | 58, 62, 12, 2 | 600, 4200, 5100 | 6 | V0: 4 / 301; V1: 4 / 301 | Heben/Senken y (-3120/6240); Kamerafahrt x (3800) | Rostige genietete Stahlplatte mit Schild "XD-R". |
| DOOR20 | 12, 33 | - | 2 | V0: 0 / 271 | keine Bewegung | Textur wie DOOR00; das Skript stellt kein Objekt auf. |
| DOOR21 | 12, 33 | - | 2 | V0: 0 / 271 | keine Bewegung | Textur wie DOOR00; das Skript stellt kein Objekt auf. |
| DOOR22 | 12 | 288, 6602, 3599 | 9 | V0: 1 / 286; V1: 1 / 286 | Drehen Hochachse einfluegelig (90.53 Grad); Kamerafahrt x (9450) | Rostrote genietete Stahltuer mit Riegel. |
| DOOR23 | 12, 118 | 288, 6602, 3599 | 9 | V0: 2 / 286; V1: 3 / 286 | Drehen Hochachse einfluegelig (90.53 Grad); Klinke/Anbauteil dreht (1 Obj.); Kamerafahrt x (9450) | Schwere graue Panzertuer mit achteckigem Rahmenprofil. |
| DOOR24 | 12, 184 | 288, 6602, 3599 | 9 | V0: 2 / 286; V1: 3 / 286 | Drehen Hochachse einfluegelig (90.53 Grad); Klinke/Anbauteil dreht (1 Obj.); Kamerafahrt x (9450) | Beige Labortuer mit Sichtfenster, Aushang und Blechkasten. |
| DOOR25 | 12 | 288, 6602, 3599 | 7 | V0: 1 / 286 | Heben/Senken y (-5375); Kamerafahrt x (9450) | Aufzugtuer "SHAFT TYPE-L", graues Blech mit Warnschild. |
| DOOR26 | 344, 48, 48, 48, 2 | 908, 5639, 2990 | 18 | V0: 4 / 286; V1: 4 / 236; V2: 2 / 286; V3: 2 / 286 | Drehen Hochachse einfluegelig (-77.96 Grad); Klinke/Anbauteil dreht (1 Obj.); Kamerafahrt x (9450/9450) | Rostrote Schotttuer "TYPE-P" mit Handrad, zweiteilig. |
| DOOR27 | 180, 259, 8 | 486, 6002, 3236 | 10 | V0: 2 / 271; V1: 2 / 271; V2: 2 / 271; V3: 2 / 271 | Schieben z (-2240/2240); Kamerafahrt x (9450/9450) | Graugruene Labor-Schiebetuer, zweiteilig, mit Hinweisschildern. |
| DOOR28 | 232, 1, 1 | 1793, 1406, 2153 | 6 | V0: 3 / 281; V2: 3 / 281; V4: 3 / 281; V1: 3 / 301; V3: 3 / 301; V5: 3 / 301 | Kamerafahrt x (3400) | Rostige Bodenluke mit gelbem Schaltkasten und Riffelblech. |
| DOOR29 | 12, 144, 144, 2 | 288, 6602, 3599 | 9 | V0: 2 / 286; V1: 4 / 286 | Drehen Hochachse einfluegelig (90.53 Grad); Kamerafahrt x (9450) | Aufzugtuer "SHAFT TYPE-M" mit Bedientafel und Warnaufkleber. |
| DOOR2A | 12 | 288, 6602, 3599 | 11 | V0: 1 / 301; V1: 1 / 301; V2: 2 / 301; V3: 2 / 301 | Schieben z (4000); Kamerafahrt x (8550) | Dunkle Stahltuer mit Sichtschlitz, Drehriegel und Lueftungslamellen. |
| DOOR2B | 96 | 60, 6000, 9000 | 4 | V0: 1 / 301 | Heben/Senken y (-7080); Kamerafahrt x (3800) | Rostige Klappe mit Aufschrift "DUMPING AREA-B2". |
| DOOR2C | 12, 2, 60 | 288, 6602, 3599 | 13 | V0: 6 / 301; V1: 4 / 351; V2: 6 / 301; V3: 4 / 351 | Schieben z (3040/-3040); Kamerafahrt x (9450/9450) | Graue Stahltuer mit abgerundetem Drahtglasfenster. |
| DOOR2D | 340, 30, 60 | 5970, 4703, 11900 | 10 | V0: 6 / 301; V2: 6 / 301; V4: 6 / 301; V1: 6 / 451; V3: 6 / 451; V5: 6 / 451 | Kamerafahrt x (11800) | Hubbuehne: Streckmetallgitter mit gelb-schwarzem Warnrand, Bedientafel, rote Lampe. |
| DOOR2E | 448, 26 | 240, 5976, 3046 | 9 | V0: 2 / 291; V1: 3 / 291 | Drehen Hochachse einfluegelig (50.1 Grad); Klinke/Anbauteil dreht (1 Obj.); Kamerafahrt x (7140) | Gittertuer im Rahmen mit gerundeten Ecken: senkrechte Staebe, Mittelblech mit Schlosskasten, Baender. |
| DOOR2F | 12, 36 | 288, 6602, 3599 | 7 | V0: 2 / 301; V1: 3 / 301 | Drehen Hochachse einfluegelig (90.53 Grad); Kamerafahrt x (9450) | Tuerkisgruene Tuer mit ornamentalen Fuellungen und Mittelfuge. |
| DOOR30 | 196 | 180, 5943, 1216 | 13 | V0: 2 / 301; V1: 2 / 301 | Drehen Hochachse zweifluegelig (90.53/-90.53 Grad); Kamerafahrt x (4830/4830) | Schmaler Metallrahmen mit zwei dunklen Glasfeldern (ein Fluegel einer Doppeltuer). |
| DOOR31 | 344, 48, 48, 48, 2 | 908, 5639, 2990 | 18 | V0: 4 / 286; V1: 4 / 236; V2: 2 / 286; V3: 2 / 286 | Drehen Hochachse einfluegelig (-77.96 Grad); Klinke/Anbauteil dreht (1 Obj.); Kamerafahrt x (9450/9450) | Wie DOOR26, weiss ueberkrustet. |
| DOOR32 | 12, 33 | - | 2 | V0: 0 / 321 | keine Bewegung | Textur wie DOOR00; das Skript stellt kein Objekt auf. |
| DOOR33 | 304, 32 | 6000, 1200, 1800 | 10 | V0: 2 / 301; V1: 2 / 301; V6: 2 / 301; V7: 1 / 301; V8: 4 / 301 | Klinke/Anbauteil dreht (1 Obj.); Kamerafahrt x (4320/4320) | Wie DOOR1E (Gitter oben, Stahlplatten unten). |
| DOOR34 | 12, 33 | - | 2 | V0: 0 / 321 | keine Bewegung | Textur wie DOOR00; das Skript stellt kein Objekt auf. |
| DOOR35 | 304, 32 | 6000, 1200, 1800 | 10 | V0: 2 / 301; V1: 2 / 301; V6: 2 / 301; V7: 1 / 301; V8: 4 / 301 | Klinke/Anbauteil dreht (1 Obj.); Kamerafahrt x (4320/4380) | Wie DOOR1E (Gitter oben, Stahlplatten unten). |
| DOOR36 | 12, 33 | - | 2 | V0: 0 / 281 | keine Bewegung | Textur wie DOOR00; das Skript stellt kein Objekt auf. |

Verteilung der Hauptbewegung (erste Variante):

- Drehen Hochachse einfluegelig: 29 (00, 01, 02, 03, 04, 05, 06, 07, 08, 09, 0A, 0B, 0D, 11, 13, 15, 17, 18, 1A, 1C, 1D, 22, 23, 24, 26, 29, 2E, 2F, 31)
- Heben/Senken y: 7 (0E, 0F, 12, 16, 1F, 25, 2B)
- Schieben z: 6 (10, 14, 19, 27, 2A, 2C)
- keine Bewegung: 5 (20, 21, 32, 34, 36)
- Drehen Hochachse zweifluegelig: 3 (0C, 1B, 30)
- Klinke/Anbauteil dreht: 3 (1E, 33, 35)
- Kamerafahrt x: 2 (28, 2D)

Oeffnungswinkel aller drehenden Fluegel (Fluegel-Instanzen ueber alle Varianten):

| Winkel Einheiten | Grad | Dauer Bilder | Anzahl | Tueren |
|---|---|---|---|---|
| 1030 | 90.53 | 141 | 80 | 27 Tueren |
| 887 | 77.96 | 115 | 8 | 26, 31 |
| 570 | 50.10 | 80 | 4 | 0A, 2E |
| 588 | 51.68 | 120 | 2 | 07 |

Weitere Messwerte ueber alle Archive: 36 von 55 Tueren benutzen als Mesh 0 dieselbe Platte (8 Vertices, 12
Dreiecke, 288 x 6602 x 3599; Vergleich der Vertex- und Indexlisten). Die Tuer unterscheidet sich dann nur in
Textur und Beschlag. Aufgestellte Dreiecke je Variante: kleinster Wert 12, groesster 612.

## 3. Rangliste der Vorbilder

Wertung nur aus Messwerten (`python re15_port/tools/tor/tuerkatalog.py --rang`):

- B1 Bewegung: genau ein Fluegel dreht um die Hochachse = 3, zwei gegenlaeufige = 1, sonst 0
- B2 Fluegel durchbrochen: Deckungsgrad der Frontansicht unter 0,90 = 2
  (Dreiecke orthografisch entlang x gefuellt, Raster 20 Einheiten, Anteil am Huellrechteck)
- B3 Staebe: mindestens eine Komponente ist ein deckelloses Prisma (Laenge >= 5 x Dicke, Vertices = Dreiecke) = 1
- B4 Aussenkontur nicht rechteckig: konvexe Huelle der Frontansicht hat mehr als 4 Ecken = 1
- B5 Pfosten: ruhendes stabfoermiges Wurzelobjekt mit bewegten Kindern = 1
- Gleichstand: mehr Huell-Ecken zuerst, dann der duennere Stab

| Platz | Tuer | Punkte | B1 B2 B3 B4 B5 | Fluegel x,y,z | Dreiecke | Deckung | Huell-Ecken | Staebe | Seiten je Stab | Stabdicke | Winkel Grad |
|---|---|---|---|---|---|---|---|---|---|---|---|
| 1 | DOOR2E | 7 | 3 2 1 1 0 | 240, 5976, 3046 | 448 | 0.619 | 17 | 8 | 6 | 60 | 50.1 |
| 2 | DOOR0A | 7 | 3 2 1 1 0 | 329, 6000, 3000 | 300 | 0.531 | 8 | 12 | 8 | 139 | 50.1 |
| 3 | DOOR28 | 4 | 0 2 1 1 0 | 1793, 1406, 2153 | 232 | 0.202 | 13 | 1 | 6 | 91 | - |
| 4 | DOOR10 | 4 | 0 2 0 1 1 | 125, 7163, 761 | 26 | 0.711 | 9 | 0 | - | - | - |
| 5 | DOOR2D | 4 | 0 2 1 1 0 | 5970, 4703, 11900 | 340 | 0.705 | 9 | 6 | 6 | 120 | - |

Dahinter mit 3 Punkten 28 Tueren: 27 einfluegelige Drehtueren mit GESCHLOSSENEM Fluegel (B1 allein; 00, 01, 02, 03, 04, 05, 06, 07, 08, 09, 0B, 0D, 11, 13, 15, 17, 18, 1A, 1C, 1D, 22, 23, 24, 26, 29, 2F, 31) und
DOOR16 (Leiter: durchbrochen mit Staeben, aber ohne Drehung). Weniger als 3 Punkte haben 22 Tueren.

Lesart:

1. **DOOR2E** und **DOOR0A** sind die einzigen Tueren, die zugleich einfluegelig drehen UND einen durchbrochenen
   Fluegel aus echten Staeben haben. DOOR2E liegt vorn, weil seine Aussenkontur 17 Huell-Ecken hat: jede der
   vier Ecken ist durch 2 gerade Abschnitte angenaehert, z. B. oben an der freien Kante die Huellpunkte (z,y)
   (-3017,-5790) (-2946,-5909) (-2835,-5977); Eckmass 179..212 Einheiten. DOOR0A hat 8 Huell-Ecken. Ausserdem
   sind die Staebe von DOOR2E duenner (60 x 52 gegen 139 x 136).
2. Die drei Tueren mit 4 Punkten (DOOR28 Bodenluke, DOOR10 Scherengitter, DOOR2D Hubbuehne) drehen keinen
   Fluegel. Nur **DOOR10** ist ein aufrechtes Tor und das einzige Archiv mit einem Pfosten als eigenem Objekt (B5).
3. Gesuchte Gruppen und was es davon gibt: Gittertore = DOOR0A, DOOR2E (drehen), DOOR10 (Scherengitter,
   schiebt); Zauntueren = DOOR14 (Maschendraht, schiebt 5515 seitlich; die Maschen sind Textur auf einer
   geschlossenen Flaeche, Deckung 1,000); niedrige Tueren = KEINE; Rahmen aus Rohren = DOOR2E (Rahmen 352
   Dreiecke + 8 Staebe), DOOR16 (Leiter, 10 Staebe, faehrt senkrecht).
4. Warnschraffur: Einen Anteil kraeftig gelber Pixel (R>=150, G>=110, B<=70, R-B>=100) ueber 1 Prozent im
   Blattbereich (Zeilen 0..219) haben nur DOOR28 (1,2 Prozent) und DOOR2D (1,4 Prozent). Das Schild des Tors
   laesst sich aus keiner RE2-Tuertextur uebernehmen.

Uebersichtsblatt: `build/tor_1170/katalog_kandidaten.png` (je Kandidat Textur, Frontansicht zu, Fluegel am
Anschlag, Draufsicht). Einzelblaetter je Variante: `katalog_DOOR2E_v0.png`, `katalog_DOOR2E_v1.png`, ebenso
0A, 10, 14, 15, 30. Alle 55: `katalog_texturen.png`, `katalog_texturen_reihe0..4.png`, `katalog_drahtgitter.png`.

## 4. DOOR2E vollstaendig

Datei `info/re2leon/COMMON/DOOR/DOOR2E.DO2`, 75772 B. Modellteil @0x5000, SCD-Tabelle @0x5008, MD1 @0x5260, TIM @0xa5dc.
SCD-Offsettabelle @0x5008: `12 00 2e 00 8e 00 04 01 4e 01 64 01 8e 01 b2 01 04 02` = 9 Skripte.

### 4.1 Modell

| Mesh | Eintrag @Datei | Vertices | Dreiecke | Huellquader min | max | UV min..max |
|---|---|---|---|---|---|---|
| 0 | 0x526c | 272 | 448 | (-120, -5977, -3044) | (120, -1, 2) | (0, 0)..(126, 218) |
| 1 | 0x52a4 | 16 | 26 | (0, -43, -30) | (240, 47, 480) | (0, 241)..(62, 255) |

Mesh 0 (Fluegel) zerfaellt in 9 Komponenten:

| Komponente | Dreiecke | Vertices | min x,y,z | max x,y,z | Masse |
|---|---|---|---|---|---|
| Rahmen mit Mittelblech und Baendern | 352 | 176 | (-120, -5977, -3044) | (120, -1, 2) | (240, 5976, 3046) |
| Stab, 6 Seiten | 12 | 12 | (-30, -5421, -2294) | (30, -3252, -2242) | (60, 2169, 52) |
| Stab, 6 Seiten | 12 | 12 | (-30, -2699, -2294) | (30, -984, -2242) | (60, 1715, 52) |
| Stab, 6 Seiten | 12 | 12 | (-30, -5421, -1796) | (30, -3252, -1744) | (60, 2169, 52) |
| Stab, 6 Seiten | 12 | 12 | (-30, -2699, -1796) | (30, -984, -1744) | (60, 1715, 52) |
| Stab, 6 Seiten | 12 | 12 | (-30, -5421, -1286) | (30, -3252, -1234) | (60, 2169, 52) |
| Stab, 6 Seiten | 12 | 12 | (-30, -2699, -1286) | (30, -984, -1234) | (60, 1715, 52) |
| Stab, 6 Seiten | 12 | 12 | (-30, -5421, -788) | (30, -3252, -736) | (60, 2169, 52) |
| Stab, 6 Seiten | 12 | 12 | (-30, -2699, -788) | (30, -984, -736) | (60, 1715, 52) |

- Ursprung des Fluegels = Drehachse: z reicht von +2 bis -3044, die Angel liegt bei z = 0; y = 0 ist die
  Unterkante, oben ist -y; die Dicke liegt mittig um x = 0 (-120..+120).
- Ein Stab ist ein sechsseitiges Prisma OHNE Deckel: 12 Vertices, 12 Dreiecke. Querschnitt des Stabs bei
  z = -2268: (-30,-2268) (-15,-2294) (15,-2294) (30,-2268) (15,-2242) (-15,-2242), also 60 breit in x, 52 in z.
  DOOR0A baut dieselben Staebe achtseitig (16 Vertices, 16 Dreiecke, 139 x 136).
- Mesh 1 ist der Riegel (26 Dreiecke, 240 x 90 x 510), als Kind an den Fluegel gebunden.

### 4.2 Skripte Zeile fuer Zeile

Spalten: Offset im Skript, Datei-Offset, rohe Bytes, entschluesselte Anweisung.

```
Skript 0  rel 0x012..0x02e  Datei 0x0501a  28 B
  +0x000 @0x0501a  13 0c 16 00                                                       Switch var0x0c (Block 22 B)
  +0x004 @0x0501e  14 00 04 00 00 00                                                 Case 0 (Block 4 B)
  +0x00a @0x05024  18 01                                                             Gosub 1
  +0x00c @0x05026  1a 00                                                             Break 00
  +0x00e @0x05028  14 00 04 00 01 00                                                 Case 1 (Block 4 B)
  +0x014 @0x0502e  18 02                                                             Gosub 2
  +0x016 @0x05030  1a 00                                                             Break 00
  +0x018 @0x05032  16 00                                                             Eswitch 00
  +0x01a @0x05034  01 00                                                             Evt_end 00

Skript 1  rel 0x02e..0x08e  Datei 0x05036  96 B
  +0x000 @0x05036  4d 00 00 00 01 00 80 0a 10 00 c4 09 b8 0d b2 06 00 00 00 00 00 00 Door_model_set obj=0 mesh=0 an=1 flags=0x0a80(Eltern=Kamera) b2=0 bild=0 w8=16 pos=(2500,3512,1714) rot=(0,0,0)
  +0x016 @0x0504c  4d 01 00 00 01 01 d0 00 10 00 82 00 fc f4 4c f5 00 00 00 00 00 00 Door_model_set obj=1 mesh=1 an=1 flags=0x00d0(Eltern=Obj0) b2=0 bild=0 w8=16 pos=(130,-2820,-2740) rot=(0,0,0)
  +0x02c @0x05062  53 00 02 07 00 fe                                                 Sce_fade_set 0,2,7,-512
  +0x032 @0x05068  74 00 00 1c                                                       Sce_fade_adjust 0,7168
  +0x036 @0x0506c  09                                                                Sleep 70
  +0x037 @0x0506d  0a 46 00                                                          Sleeping (Zaehler 70)
  +0x03a @0x05070  04 0b 18 05                                                       Evt_exec platz=11 skript=5
  +0x03e @0x05074  09                                                                Sleep 30
  +0x03f @0x05075  0a 1e 00                                                          Sleeping (Zaehler 30)
  +0x042 @0x05078  18 04                                                             Gosub 4
  +0x044 @0x0507a  36 00 00 00 01 00 00 00 00 00 00 00                               Se_on vab=0 se=0x0000 work=0x0001 pos=(0,0,0)
  +0x050 @0x05086  09                                                                Sleep 30
  +0x051 @0x05087  0a 1e 00                                                          Sleeping (Zaehler 30)
  +0x054 @0x0508a  04 0c 18 03                                                       Evt_exec platz=12 skript=3
  +0x058 @0x0508e  09                                                                Sleep 70
  +0x059 @0x0508f  0a 46 00                                                          Sleeping (Zaehler 70)
  +0x05c @0x05092  18 07                                                             Gosub 7
  +0x05e @0x05094  01 00                                                             Evt_end 00

Skript 2  rel 0x08e..0x104  Datei 0x05096  118 B
  +0x000 @0x05096  4d 00 00 00 01 00 80 0a 10 00 c4 09 b8 0d a2 fa 00 00 00 08 00 00 Door_model_set obj=0 mesh=0 an=1 flags=0x0a80(Eltern=Kamera) b2=0 bild=0 w8=16 pos=(2500,3512,-1374) rot=(0,2048,0)
  +0x016 @0x050ac  4d 01 00 00 01 01 d0 00 10 00 7e ff fc f4 4c f5 00 10 00 00 00 08 Door_model_set obj=1 mesh=1 an=1 flags=0x00d0(Eltern=Obj0) b2=0 bild=0 w8=16 pos=(-130,-2820,-2740) rot=(4096,0,2048)
  +0x02c @0x050c2  4d 02 00 00 01 01 50 00 10 00 82 00 2e f5 4c f5 38 07 00 08 00 08 Door_model_set obj=2 mesh=1 an=1 flags=0x0050(Eltern=Obj0) b2=0 bild=0 w8=16 pos=(130,-2770,-2740) rot=(1848,2048,2048)
  +0x042 @0x050d8  53 00 02 07 00 fe                                                 Sce_fade_set 0,2,7,-512
  +0x048 @0x050de  74 00 00 1c                                                       Sce_fade_adjust 0,7168
  +0x04c @0x050e2  09                                                                Sleep 70
  +0x04d @0x050e3  0a 46 00                                                          Sleeping (Zaehler 70)
  +0x050 @0x050e6  04 0b 18 05                                                       Evt_exec platz=11 skript=5
  +0x054 @0x050ea  09                                                                Sleep 30
  +0x055 @0x050eb  0a 1e 00                                                          Sleeping (Zaehler 30)
  +0x058 @0x050ee  18 04                                                             Gosub 4
  +0x05a @0x050f0  36 00 00 00 01 00 00 00 00 00 00 00                               Se_on vab=0 se=0x0000 work=0x0001 pos=(0,0,0)
  +0x066 @0x050fc  09                                                                Sleep 30
  +0x067 @0x050fd  0a 1e 00                                                          Sleeping (Zaehler 30)
  +0x06a @0x05100  04 0c 18 03                                                       Evt_exec platz=12 skript=3
  +0x06e @0x05104  09                                                                Sleep 70
  +0x06f @0x05105  0a 46 00                                                          Sleeping (Zaehler 70)
  +0x072 @0x05108  18 08                                                             Gosub 8
  +0x074 @0x0510a  01 00                                                             Evt_end 00

Skript 3  rel 0x104..0x14e  Datei 0x0510c  74 B
  +0x000 @0x0510c  2e 05 00                                                          Work_set typ=5 id=0
  +0x003 @0x0510f  00                                                                Nop
  +0x004 @0x05110  2f 00 00 00                                                       Speed_set [0=vx] = 0
  +0x008 @0x05114  2f 0a 01 00                                                       Speed_set [10=awy] = 1
  +0x00c @0x05118  2f 04 07 00                                                       Speed_set [4=wy] = 7
  +0x010 @0x0511c  0d 00 06 00 0f 00                                                 For n=15 (Block 6 B)
  +0x016 @0x05122  30                                                                Add_speed
  +0x017 @0x05123  02                                                                Evt_next
  +0x018 @0x05124  30                                                                Add_speed
  +0x019 @0x05125  02                                                                Evt_next
  +0x01a @0x05126  0e 00                                                             Next 00
  +0x01c @0x05128  0d 00 08 00 05 00                                                 For n=5 (Block 8 B)
  +0x022 @0x0512e  31                                                                Add_aspeed
  +0x023 @0x0512f  30                                                                Add_speed
  +0x024 @0x05130  02                                                                Evt_next
  +0x025 @0x05131  30                                                                Add_speed
  +0x026 @0x05132  02                                                                Evt_next
  +0x027 @0x05133  00                                                                Nop
  +0x028 @0x05134  0e 00                                                             Next 00
  +0x02a @0x05136  2f 0a ff ff                                                       Speed_set [10=awy] = -1
  +0x02e @0x0513a  0d 00 14 00 05 00                                                 For n=5 (Block 20 B)
  +0x034 @0x05140  31                                                                Add_aspeed
  +0x035 @0x05141  30                                                                Add_speed
  +0x036 @0x05142  02                                                                Evt_next
  +0x037 @0x05143  30                                                                Add_speed
  +0x038 @0x05144  02                                                                Evt_next
  +0x039 @0x05145  30                                                                Add_speed
  +0x03a @0x05146  02                                                                Evt_next
  +0x03b @0x05147  30                                                                Add_speed
  +0x03c @0x05148  02                                                                Evt_next
  +0x03d @0x05149  31                                                                Add_aspeed
  +0x03e @0x0514a  30                                                                Add_speed
  +0x03f @0x0514b  02                                                                Evt_next
  +0x040 @0x0514c  30                                                                Add_speed
  +0x041 @0x0514d  02                                                                Evt_next
  +0x042 @0x0514e  30                                                                Add_speed
  +0x043 @0x0514f  02                                                                Evt_next
  +0x044 @0x05150  30                                                                Add_speed
  +0x045 @0x05151  02                                                                Evt_next
  +0x046 @0x05152  0e 00                                                             Next 00
  +0x048 @0x05154  01 00                                                             Evt_end 00

Skript 4  rel 0x14e..0x164  Datei 0x05156  22 B
  +0x000 @0x05156  06 00 10 00                                                       Ifel_ck (sonst -> 0x162)
  +0x004 @0x0515a  23 00 0d 00 01 00                                                 Cmp var0x0d == 1
  +0x00a @0x05160  02                                                                Evt_next
  +0x00b @0x05161  00                                                                Nop
  +0x00c @0x05162  17 ff ff 00 f4 ff                                                 Goto -12 -> 0x14e
  +0x012 @0x05168  08 00                                                             Endif 00
  +0x014 @0x0516a  01 00                                                             Evt_end 00

Skript 5  rel 0x164..0x18e  Datei 0x0516c  42 B
  +0x000 @0x0516c  2e 05 01                                                          Work_set typ=5 id=1
  +0x003 @0x0516f  00                                                                Nop
  +0x004 @0x05170  2f 03 00 00                                                       Speed_set [3=wx] = 0
  +0x008 @0x05174  2f 09 ff ff                                                       Speed_set [9=awx] = -1
  +0x00c @0x05178  0d 00 06 00 19 00                                                 For n=25 (Block 6 B)
  +0x012 @0x0517e  30                                                                Add_speed
  +0x013 @0x0517f  02                                                                Evt_next
  +0x014 @0x05180  31                                                                Add_aspeed
  +0x015 @0x05181  00                                                                Nop
  +0x016 @0x05182  0e 00                                                             Next 00
  +0x018 @0x05184  2f 09 06 00                                                       Speed_set [9=awx] = 6
  +0x01c @0x05188  0d 00 06 00 05 00                                                 For n=5 (Block 6 B)
  +0x022 @0x0518e  30                                                                Add_speed
  +0x023 @0x0518f  02                                                                Evt_next
  +0x024 @0x05190  31                                                                Add_aspeed
  +0x025 @0x05191  00                                                                Nop
  +0x026 @0x05192  0e 00                                                             Next 00
  +0x028 @0x05194  01 00                                                             Evt_end 00

Skript 6  rel 0x18e..0x1b2  Datei 0x05196  36 B
  +0x000 @0x05196  2e 05 02                                                          Work_set typ=5 id=2
  +0x003 @0x05199  00                                                                Nop
  +0x004 @0x0519a  2f 03 f2 ff                                                       Speed_set [3=wx] = -14
  +0x008 @0x0519e  2f 09 f6 ff                                                       Speed_set [9=awx] = -10
  +0x00c @0x051a2  0d 00 06 00 02 00                                                 For n=2 (Block 6 B)
  +0x012 @0x051a8  30                                                                Add_speed
  +0x013 @0x051a9  02                                                                Evt_next
  +0x014 @0x051aa  31                                                                Add_aspeed
  +0x015 @0x051ab  00                                                                Nop
  +0x016 @0x051ac  0e 00                                                             Next 00
  +0x018 @0x051ae  0d 00 04 00 1d 00                                                 For n=29 (Block 4 B)
  +0x01e @0x051b4  30                                                                Add_speed
  +0x01f @0x051b5  02                                                                Evt_next
  +0x020 @0x051b6  0e 00                                                             Next 00
  +0x022 @0x051b8  01 00                                                             Evt_end 00

Skript 7  rel 0x1b2..0x204  Datei 0x051ba  82 B
  +0x000 @0x051ba  2e 05 00                                                          Work_set typ=5 id=0
  +0x003 @0x051bd  00                                                                Nop
  +0x004 @0x051be  2f 02 00 00                                                       Speed_set [2=vz] = 0
  +0x008 @0x051c2  2f 00 00 00                                                       Speed_set [0=vx] = 0
  +0x00c @0x051c6  2f 06 02 00                                                       Speed_set [6=ax] = 2
  +0x010 @0x051ca  2f 08 02 00                                                       Speed_set [8=az] = 2
  +0x014 @0x051ce  0d 00 06 00 14 00                                                 For n=20 (Block 6 B)
  +0x01a @0x051d4  30                                                                Add_speed
  +0x01b @0x051d5  31                                                                Add_aspeed
  +0x01c @0x051d6  02                                                                Evt_next
  +0x01d @0x051d7  00                                                                Nop
  +0x01e @0x051d8  0e 00                                                             Next 00
  +0x020 @0x051da  2f 08 00 00                                                       Speed_set [8=az] = 0
  +0x024 @0x051de  0d 00 06 00 14 00                                                 For n=20 (Block 6 B)
  +0x02a @0x051e4  30                                                                Add_speed
  +0x02b @0x051e5  31                                                                Add_aspeed
  +0x02c @0x051e6  02                                                                Evt_next
  +0x02d @0x051e7  00                                                                Nop
  +0x02e @0x051e8  0e 00                                                             Next 00
  +0x030 @0x051ea  2f 08 fe ff                                                       Speed_set [8=az] = -2
  +0x034 @0x051ee  0d 00 06 00 14 00                                                 For n=20 (Block 6 B)
  +0x03a @0x051f4  30                                                                Add_speed
  +0x03b @0x051f5  31                                                                Add_aspeed
  +0x03c @0x051f6  02                                                                Evt_next
  +0x03d @0x051f7  00                                                                Nop
  +0x03e @0x051f8  0e 00                                                             Next 00
  +0x040 @0x051fa  53 00 02 07 00 04                                                 Sce_fade_set 0,2,7,1024
  +0x046 @0x05200  0d 00 04 00 1e 00                                                 For n=30 (Block 4 B)
  +0x04c @0x05206  30                                                                Add_speed
  +0x04d @0x05207  02                                                                Evt_next
  +0x04e @0x05208  0e 00                                                             Next 00
  +0x050 @0x0520a  01 00                                                             Evt_end 00

Skript 8  rel 0x204..0x258  Datei 0x0520c  84 B
  +0x000 @0x0520c  2e 05 00                                                          Work_set typ=5 id=0
  +0x003 @0x0520f  00                                                                Nop
  +0x004 @0x05210  2f 02 00 00                                                       Speed_set [2=vz] = 0
  +0x008 @0x05214  2f 00 00 00                                                       Speed_set [0=vx] = 0
  +0x00c @0x05218  2f 06 02 00                                                       Speed_set [6=ax] = 2
  +0x010 @0x0521c  2f 08 fe ff                                                       Speed_set [8=az] = -2
  +0x014 @0x05220  0d 00 06 00 14 00                                                 For n=20 (Block 6 B)
  +0x01a @0x05226  30                                                                Add_speed
  +0x01b @0x05227  31                                                                Add_aspeed
  +0x01c @0x05228  02                                                                Evt_next
  +0x01d @0x05229  00                                                                Nop
  +0x01e @0x0522a  0e 00                                                             Next 00
  +0x020 @0x0522c  2f 08 00 00                                                       Speed_set [8=az] = 0
  +0x024 @0x05230  0d 00 06 00 14 00                                                 For n=20 (Block 6 B)
  +0x02a @0x05236  30                                                                Add_speed
  +0x02b @0x05237  31                                                                Add_aspeed
  +0x02c @0x05238  02                                                                Evt_next
  +0x02d @0x05239  00                                                                Nop
  +0x02e @0x0523a  0e 00                                                             Next 00
  +0x030 @0x0523c  2f 08 02 00                                                       Speed_set [8=az] = 2
  +0x034 @0x05240  0d 00 06 00 14 00                                                 For n=20 (Block 6 B)
  +0x03a @0x05246  30                                                                Add_speed
  +0x03b @0x05247  31                                                                Add_aspeed
  +0x03c @0x05248  02                                                                Evt_next
  +0x03d @0x05249  00                                                                Nop
  +0x03e @0x0524a  0e 00                                                             Next 00
  +0x040 @0x0524c  53 00 02 07 00 04                                                 Sce_fade_set 0,2,7,1024
  +0x046 @0x05252  0d 00 04 00 1e 00                                                 For n=30 (Block 4 B)
  +0x04c @0x05258  30                                                                Add_speed
  +0x04d @0x05259  02                                                                Evt_next
  +0x04e @0x0525a  0e 00                                                             Next 00
  +0x050 @0x0525c  01 00                                                             Evt_end 00
  +0x052 @0x0525e  00                                                                Nop
  +0x053 @0x0525f  00                                                                Nop

```

Bedeutung je Skript:

| Skript | Rolle | Rechnung |
|---|---|---|
| 0 | Verteiler | Variable 0x0C = 0 -> Skript 1, = 1 -> Skript 2 |
| 1 | Aufbau Variante 0 | Fluegel Obj 0 (Mesh 0, Flags 0x0a80, Eltern Kamera) bei (2500,3512,1714), Drehung 0; Riegel Obj 1 (Mesh 1, Flags 0x00d0, Eltern Obj 0) bei (130,-2820,-2740) |
| 2 | Aufbau Variante 1 | Fluegel bei (2500,3512,-1374), Drehung y = 2048 (180 Grad); Riegel Obj 1 bei (-130,-2820,-2740) Drehung (4096,0,2048); zweiter Riegel Obj 2 (Flags 0x0050) bei (130,-2770,-2740) Drehung (1848,2048,2048) |
| 3 | Fluegel schwenkt (Obj 0, Drehung y) | 30 Bilder x 7 = 210; dann 10 Bilder mit 8,8,9,9,10,10,11,11,12,12 = 100; dann 40 Bilder mit je viermal 11,10,9,8,7,6,5,4,3,2 = 260; Summe 570 in 80 Bildern |
| 4 | Warten auf den Klang | solange Variable 0x0D = 1: Bild beenden und von vorn |
| 5 | Riegel dreht (Obj 1, Drehung x) | 25 Bilder mit 0,-1,...,-24 = -300; dann 5 Bilder mit -25,-19,-13,-7,-1 = -65; Summe -365 (-32,08 Grad) in 30 Bildern, davon das erste ohne Aenderung |
| 6 | Riegel Obj 2 (Drehung x) | 2 Bilder mit -14,-24, dann 29 Bilder mit -34; Summe -1024; wird von keinem anderen Skript aufgerufen (weder Gosub noch Evt_exec nennt 6) |
| 7 | Kamerafahrt Variante 0 | Obj 0: x-Beschleunigung 2, z-Beschleunigung +2 / 0 / -2 je 20 Bilder, danach 30 Bilder gleichfoermig; x gesamt 3540 + 3600 = 7140, z gesamt 380 + 800 + 420 = 1600; Ausblenden im 61. Bild |
| 8 | Kamerafahrt Variante 1 | wie 7 mit z-Beschleunigung -2 / 0 / +2; z gesamt -1600 |

### 4.3 Bild-Zeitachse

Beide Varianten laufen zeitgleich. Angenommen ist, dass der Klang in Bild 100 schon geladen ist (Skript 4
kostet dann 0 Bilder); laedt er spaeter, verschiebt sich alles ab Bild 100 um die Wartezeit.

| Bild | Platz | Ereignis | Obj 0 Lage | Obj 0 Drehung y V0 / V1 | Obj 1 Drehung x V0 / V1 |
|---|---|---|---|---|---|
| 0 | 10 | Aufbau, Blende (0,2,7,-512) und Nachstellen (0,7168), Sleep 70 | V0 (2500,3512,1714) / V1 (2500,3512,-1374) | 0 / 2048 | 0 / 4096 |
| 70 | 10 -> 11 | Evt_exec Skript 5: Riegel beginnt | V0 (2500,3512,1714) / V1 (2500,3512,-1374) | 0 / 2048 | 0 / 4096 |
| 71 | 11 | erste sichtbare Riegeldrehung | V0 (2500,3512,1714) / V1 (2500,3512,-1374) | 0 / 2048 | -1 / 4095 |
| 95 | 11 | Riegel: Beschleunigung wechselt auf +6 | V0 (2500,3512,1714) / V1 (2500,3512,-1374) | 0 / 2048 | -325 / 3771 |
| 99 | 11 | Riegel am Ende | V0 (2500,3512,1714) / V1 (2500,3512,-1374) | 0 / 2048 | -365 / 3731 |
| 100 | 10 | Gosub 4 (Klang bereit?), Se_on vab 0, se 0, Bezug 1; Sleep 30 | V0 (2500,3512,1714) / V1 (2500,3512,-1374) | 0 / 2048 | -365 / 3731 |
| 130 | 10 -> 12 | Evt_exec Skript 3: Fluegel beginnt, +7 je Bild | V0 (2500,3512,1714) / V1 (2500,3512,-1374) | 7 / 2055 | -365 / 3731 |
| 159 | 12 | Ende gleichfoermige Phase | V0 (2500,3512,1714) / V1 (2500,3512,-1374) | 210 / 2258 | -365 / 3731 |
| 169 | 12 | Ende Beschleunigung (bis 12 je Bild) | V0 (2500,3512,1714) / V1 (2500,3512,-1374) | 310 / 2358 | -365 / 3731 |
| 199 | 12 | Fluegel bremst | V0 (2500,3512,1714) / V1 (2500,3512,-1374) | 542 / 2590 | -365 / 3731 |
| 200 | 10 | Gosub 7 bzw. 8: Kamerafahrt beginnt (erstes Bild ohne Weg) | V0 (2500,3512,1714) / V1 (2500,3512,-1374) | 546 / 2594 | -365 / 3731 |
| 209 | 12 | Fluegel am Anschlag | V0 (2590,3512,1804) / V1 (2590,3512,-1464) | 570 / 2618 | -365 / 3731 |
| 220 | 10 | seitliche Beschleunigung 0 | V0 (2920,3512,2134) / V1 (2920,3512,-1794) | 570 / 2618 | -365 / 3731 |
| 240 | 10 | seitliche Beschleunigung umgekehrt | V0 (4140,3512,2934) / V1 (4140,3512,-2594) | 570 / 2618 | -365 / 3731 |
| 260 | 10 | Sce_fade_set (0,2,7,1024): Ausblenden; Fahrt gleichfoermig 120 je Bild | V0 (6160,3512,3314) / V1 (6160,3512,-2974) | 570 / 2618 | -365 / 3731 |
| 289 | 10 | letztes Bild mit Bewegung | V0 (9640,3512,3314) / V1 (9640,3512,-2974) | 570 / 2618 | -365 / 3731 |
| 290 | 10 | Evt_end: Ereignis 10 aus, Schleife endet nach diesem Bild | V0 (9640,3512,3314) / V1 (9640,3512,-2974) | 570 / 2618 | -365 / 3731 |

Die volle Zeitachse (jedes Bild, in dem sich etwas aendert: 190 Eintraege je Variante) steht im JSON unter
`doors[46].variants[k].zeitachse`, der Ablauf der Anweisungen mit Bildnummer unter `.ablauf`.
Nachmessen: `python re15_port/tools/tor/tuerkatalog.py --trace 2E 0` und `--trace 2E 1`.

Richtung: Mit m[0][2] = +sin y wandert die freie Kante (lokal z = -3044) in Variante 0 bei wachsendem Winkel
nach -x, also VON der Kamera WEG (x = 2500 - 3044 * sin 50,1 Grad = 165). In Variante 1 steht der Fluegel um 180
Grad gedreht, die Angel liegt links (z = -1374), und derselbe Zuwachs fuehrt die freie Kante ZUR Kamera
(x = 2500 + 2335). Variante 0 = aufdruecken, Angel rechts; Variante 1 = aufziehen, Angel links.

## 5. Die zwei naechstbesten in Kurzform

### 5.1 DOOR0A (Zellentuer)

- Datei 77932 B, Modellteil @0x6000, MD1 @0x6260, TIM @0xae4c, 9 Skripte mit denselben Offsets wie DOOR2E.
- Skriptblock 600 B, 11 Bytes anders als DOOR2E, alle in Door_model_set-Saetzen des Riegels:
  Block-Offsets 0x50 0x52 0x55 (Skript 1, Obj 1: y -3040 statt -2820, z -2660 statt -2740, Drehung x 2048 statt 0),
  0xb0 0xb2 0xb5 (Skript 2, Obj 1) und 0xc6..0xc9, 0xcb (Skript 2, Obj 2).
- Bewegung deshalb identisch: Fluegel 570 Einheiten = 50,10 Grad in Bild 130..209, Riegel -365, Fahrt 7140, 291 Bilder.
- Fluegel Mesh 0: 256 Vertices, 300 Dreiecke, (329, 6000, 3000); 14 Komponenten = Rahmen 92 + Querblech 16 + 12 Staebe zu je 16
  Dreiecken (achtseitig, deckellos, 139 x 136). Deckung 0,531, Huelle 8 Ecken. Riegel Mesh 1: 116 Dreiecke.

### 5.2 DOOR10 (Scherengitter mit Pfosten)

- Datei 56728 B, 11 Skripte. Mesh 1 = Pfosten (8 Vertices, 12 Dreiecke, 120 x 6900 x 120), Mesh 0 = Gitterglied
  (24 Vertices, 26 Dreiecke, 125 x 7163 x 761: ein Kastenstab 120 x 6298 x 120 mit 10 Dreiecken auf 8 Vertices
  und vier flache Rechtecke mit je 4 Dreiecken auf 4 Vertices fuer Spitze, Ornament und Scherenglieder).
- Aufbau Skript 1 (Datei 0x503a, 272 B): Obj 0 = Pfosten als Wurzel bei (400,3880,-2800), Flags 0x2a80; Obj 1..9 =
  neun Gitterglieder als KETTE, jedes Kind des vorigen (Flags 0x2290, 0x2291 ... 0x2298), Abstand z = -650
  (Obj 1: +650 bei Drehung y = -2048).
- Bewegung Skript 6 (Datei 0x52b4): 100 Bilder lang schreibt Work_copy die Objektnummer und den Wert in die
  folgenden Anweisungen; Member_set Feld 13 (= z, Sprungziel 0x80055d3c) setzt je Bild den Abstand neu: 650 -> 155
  bzw. -650 -> -155, Schritt 5. Das Gitter schiebt sich zusammen, je Glied 495. Bild 121..219.
- Skript 5 (Datei 0x5282) schiebt die Wurzel 100 Bilder lang um +10 in x (= 1000) und setzt dabei Variable 0x04;
  der Rest der Fahrt (gesamt 5830) kommt aus Skript 9.
- Gesamt 301 Bilder, Klang in Bild 120, Ausblenden in Bild 270.
- Uebertragbar ist der AUFBAU (Pfosten als eigenes Wurzelobjekt, Glieder als Kinder), nicht die Bewegung.

Zum Vergleich die Normaltuer DOOR00 (und 79 weitere Fluegel-Instanzen): 1030 Einheiten = 90,53 Grad in 141
Bildern (Bild 120..260 bei DOOR00), Kamerafahrt 9450 (Variante 0) bzw. 4830 (Variante 1), 301 Bilder.

## 6. Was ein Modellbau daraus uebernehmen kann (alles belegt)

| Nr | Festlegung | Wert | Herkunft |
|---|---|---|---|
| 1 | Nur Dreiecke | 0 Vierecke in 55 Modellen; der Renderer hat keinen Vierecks-Durchlauf | Abschnitt 1.2 |
| 2 | Fluegel-Ursprung | Drehachse bei z = 0, Unterkante y = 0, oben -y, Fluegel erstreckt sich nach -z, Dicke mittig um x | DOOR2E Mesh 0 Huellquader (-120,-5977,-3044)..(120,-1,2) |
| 3 | Rundstab | Prisma mit 6 (duenn) oder 8 (dick) Seiten, ohne Deckel, 2 Dreiecke je Seite | DOOR2E 8 Staebe, DOOR0A 12 Staebe |
| 4 | Gerundete Ecke | 2 gerade Abschnitte je Ecke (3 Huellpunkte), Eckmass 179..212 Einheiten | Huellpunkte DOOR2E, JSON `doors[46].md1.meshes[0].front.huelle` |
| 5 | Textur | 128 x 256, 8 Bit, 1 CLUT; Blatt oben, Beschlag im Streifen ab v = 219 | 55/55 Archive |
| 6 | Objektbaum | Fluegel = Wurzel; Anbauteile = Kinder; feste Teile = eigene Wurzeln | 146 Varianten, kein drehender Fluegel als Kind |
| 7 | Zwei Varianten | 0 = aufdruecken, 1 = aufziehen mit gespiegelter Angel und um 180 Grad gedrehtem Fluegel | DOOR2E Skript 1 / 2 |
| 8 | Zeitgeruest | 70 Bilder Stand, Riegel 30, Klang in Bild 100, Fluegel ab 130, Fahrt ab 200, Ausblenden 260, Ende 290 | Abschnitt 4.3 |
| 9 | Kamerafahrt | wird am Wurzelobjekt gefahren (x waechst zur Kamera), nicht an der Kamera | Skript 7/8, Work_set Typ 5 Objekt 0 |
| 10 | Flag-Uebersetzung | RE2 0x0010 (Eltern) entspricht RE1.5 0x0008; Eltern-Nummer RE2 4 Bit, RE1.5 3 Bit | Abschnitt 1.3 |

## 7. Nachmessen

```
python re15_port/tools/tor/tuerkatalog.py --sheets 2E,0A,10,15,30,14   # JSON + alle Bilder, rund 1 Minute
python re15_port/tools/tor/tuerkatalog.py --rang                        # Rangliste
python re15_port/tools/tor/tuerkatalog.py --dis 2E                       # Skripte zeilenweise
python re15_port/tools/tor/tuerkatalog.py --dis 2E --script 3            # ein Skript
python re15_port/tools/tor/tuerkatalog.py --trace 2E 0                   # Bild-Zeitachse Variante 0
python re15_port/tools/tor/tuerkatalog.py --mesh 2E                      # Mesh-Masse
python re15_port/tools/tor/tuerkatalog.py --vergleich 0A 2E              # 11 abweichende Bytes
python re15_port/tools/tor/tuerkatalog.py --platte                       # 36 Tueren mit derselben Platte
python re15_port/tools/tor/tuerkatalog.py --gelb                         # Gelbanteil je Textur
python .claude/skills/re15-psx-disasm/scripts/re2_disasm.py dis 0x80014ba4 80    # Aufstell-Handler RE2
python .claude/skills/re15-psx-disasm/scripts/re15_disasm.py dis 0x80016f40 70   # Aufstell-Handler RE1.5
```

## 8. OFFEN

1. Dauer eines Bildes in Sekunden: `FUN_80031f94(1)` gibt an den Aufgabenplaner ab; wie viele Bildwechsel das
   sind, wurde nicht nachgelesen. Alle Bildzahlen sind Durchlaeufe der Tuerschleife.
2. Wartezeit in Skript 4 (Klang laden) haengt vom Laufwerk ab; hier mit 0 Bildern gerechnet.
3. Bedeutung der Blend-Parameter (0,2,7,-512), (0,7168), (0,2,7,1024): nur die Felder sind belegt
   (@0x80057ef0, @0x80057fd8), nicht die Wirkung der aufgerufenen Routinen 0x8002c1a0 / 0x8002c2b0.
4. Flag-Bits 0x0200 und 0x8000 sowie das Feld +8 (Wert 16) und Feld +2 des Aufstell-Satzes.
5. Bildaufteilung fuer ein hueft-hohes Tor: RE2 hat kein niedriges Tuerblatt; Lage des Wurzelobjekts
   (bei DOOR2E x = 2500, y = 3512) laesst sich nicht aus einem Vorbild ableiten.
6. Welche Flag-Bits der RE1.5-Renderer ausser dem Eltern-Bit auswertet, wurde hier nicht untersucht.
7. Ob RE1.5 die Opcodes der Bewegung (Work_set, Speed_set, Add_speed, Add_aspeed) mit denselben Feldern
   fuehrt wie RE2, wurde hier nicht geprueft; belegt ist nur der Aufstell-Opcode.
8. Klang: Se_on nennt vab 0 / se 0; welcher Ton der Tuer-Klangbank das ist, wurde nicht aufgeloest.
9. Opcodes 0x8A, 0x8B, 0x8C (nur DOOR28, DOOR2D, DOOR33): Laenge belegt, Wirkung nicht untersucht.
10. Bildmitte (160,120) der Uebersichtsbilder ist angenommen.
11. Lader: Der Klangteil wird nach 0x801a1000 - Groesse geladen, der Modellteil liegt in der Datei aber erst am
    naechsten Sektoranfang (DOOR00: Klang endet 0x4b08, Modell beginnt 0x5000). Wie FUN_80012fb8 / FUN_80015064
    die Luecke ueberbruecken, wurde nicht disassembliert; die Datei-Offsets des Katalogs sind davon unabhaengig
    strukturell geprueft (55/55).
12. Ein drehender Fluegel als KIND eines ruhenden Pfostens kommt in RE2 nicht vor; ob die Verkettung dafuer
    taugt, ist aus dem Code zu erwarten (Abschnitt 1.5), aber an keinem Archiv gemessen.

