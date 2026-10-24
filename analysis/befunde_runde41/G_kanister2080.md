# Runde 41 — Spur G "kanister2080" (Dossier, fortlaufend)

Baum: `.claude/worktrees/r41_kanister2080`, Zweig `r41/kanister2080`, Basis `2adca3bf` (= r41/integration 9f10bdb0
master + A D I + B, plus AUFTRAG/VERTRAG Stand Nachtrag 16 / FG-E1).

Zuteilung (VERTRAG 1.1/1.2, FG-E1): Bits 114-119, Nachrichten-Ids 2080/2081 42-47 (42 = "Hey, Leon/Elza, try with this!").
G besitzt ROOM2080/2081; F besitzt 2090-Szene (Bit 110 = Szene gesehen). Kanister im 2090-Wasser ueber B's API
(`re15_gr2_kanister_setzen`), Treffer-Zaehler aus B's API.

Punkte (Wortlaut AUFTRAG.md):
1. Nach dem Start der Alligator-Fress-Szene: in 2080 oben Metalltuer-Schliess-SE, dann 8 gespawnte Zombies im Hauptbereich.
2. Gaskanister (RE2) am Rand der Reling in 2080 (Gas_Can.bmp).
3. Nach genug Treffern auf den Alligator: Szene — Tuer-SE, Marvin (Leon)/John (Elza) in 2080, 8 Kopfschuesse, Kanister
   aufnehmen (beide Haende), "Hey, Leon, try with this!", Wurf ueber die Reling (Kamera wie Adas Raketenwerfer-Wurf RE2,
   Nachtrag 5), Kanister in 2090 im Wasser, Alligator nimmt ihn auf (B), Schuss -> RE2-Tod (B).

## 0. Stand / Fortschritt
- 2026-10-24: Pflichtlektuere (AUFTRAG/VERTRAG/UEBERGABE, B-Dossier §2.5/§7/§9), Raum-RE 2080/2090, RE2-Zensus Raketenwerfer.

## 1. RE-Belege (selbst gelesen)

### 1.1 ROOM2080/2081 (RDT = shared_assets/PSX/STAGE2/ROOM2080.RDT, 173608 B; 2081 SCD bytegleich bis auf den Namen)
- Kameras (RID @0x60, 32 B je Cut, nCut 9): Cut 5 pos (-12826,-18695,-18043) -> (-5172,-15001,-17158) (Balkon, Blick
  nach Osten auf "SEPTIC POOL"); Cut 6 (-7315,-17332,-17514) -> (449,-10461,-16726) (vom Balkon hinab in die Grube);
  Cut 7 (8095,-18819,-9867) -> (-4415,-11874,-13977) (Totale von Osten: Grube unten, Laufgang/Kontrollraum oben);
  Cut 8 flag 1. Zonen (RVD @0x188): Laufgang-Gruppe Cut 2/3/4 x -17500..9500, Cut 5/6/8 x -11000..9500 (= dieselben
  Vierecke wie die Zonen von ROOM2090 @0x220) -> 2080 und 2090 teilen x/z.
- SCA @0x4BC (5 x 28 Saetze): Band 8 (floor-Byte 0x88) = Waende des oberen Laufgangs x -14300..-9200 / z -26900..-6750;
  Balkon nach Osten: Waende @0x588 (x -9200..-5700, z -16550..-15550), @0x594 (x -9200..-5700, z -19600..-18600),
  @0x5A0 (x -6700..-5700, z -19600..-15550 = Reling am Balkonende); Band 7 (0x78, u1 1): @0x510 Laufgang-Rechteck,
  @0x528 Balkon x -9200..-6700 z -18600..-16550; Band 5 (0x55/0x45): Treppenabsatz an der 2030-Tuer.
- Tueren (main00): Slot 0 @0x0BA2 -> ROOM2030 (Band 5, rect (-24400,-17700,1000,2000)); Slot 1 @0x0BC2 -> ROOM2090
  (Band 8, rect (-9650,-26150,1000,1000), Ziel (-8200,0,-25550) Cut 0 = Tuernische in 2090; DOOR16 V5 "hinab").
- sub00 @0x0BFE: (3,245)=0 -> Wasser-Objekt obj 0 Typ 3 Band 9 bei y -10800 + Tuer 2090 gesperrt (Aot_reset Slot 1);
  sonst bei y -3420 (@0x0C5E). sub02 @0x0C86 = Rechner (Cut 7 + Wasser senken, Cut 3, Cut_auto).
- Hintergruende: BSS/ROOM2080/BG00..BG09.BSS; Bildvergleich Gas_Can.bmp gegen extracted/PSX/STAGE2/ROOM208/ROOM20800..09.bmp
  (Hauptbaum): ROOM20805 mittlere Abweichung 0,00 und 100 % gleiche Pixel ausserhalb der roten Marke (alle anderen
  >= 54,7) -> **Gas_Can.bmp = Cut 5**. Rote Marke: x 159..175, y 149..163, Mitte (167,156) = Balkonspitze vor der Reling.

### 1.2 RE2-Vorbild des Wurfs (Nachtrag 5): ROOM7020 (info/re2leon/PL0/RDT/ROOM7020.RDT)
- Zensus aller 495 RE2-Leon-RDTs (re2_scd_walk.py, Item_aot_set 0x4E / _4p 0x69, Item 0x11 = Raketenwerfer): genau
  ROOM7020 SCD-main @0x00DA `4e 05 02 31 00 00 a8 99 60 a5 48 0d c4 09 11 00 02 00 28 00 00 01` (Aufnahme-AOT 5
  x -26200 z -23200 3400x2500) und ROOMD090 (Sondermodus, nicht Leons Szenario).
- sub00 @0x00F8: Ck(4,0x97)=0 -> Sce_em_set Ada `44 00 ff 41 00 02 06 00 00 ff a0 92 c8 ce 14 ba 40 06 ..` (Modell 0x41,
  (-28000,-12600,-17900) = obere Ebene), Tyrant `44 00 00 2b ..` .
- Ausloeser: sub10 @0x06AC `Sleep 0x0708` (1800 Bilder) -> Set(5,15); sub11 @0x06B6: Work_set(3,0) + Member_cmp(0x19==1)
  + Member_cmp(0x1a==1) -> Set(5,13) + Evt_exec sub12, ODER Ck(5,15)=1 + Member_cmp(0x1a==1) + Ck(5,13)=0 -> dito.
- sub12 @0x06F2 (der Wurf): Set(2,7,1) Set(1,27,1) @0x06F2/6; Lagen sichern @0x0706-0725; **Cut_chg 7 @0x0726**;
  Work_set(4,0) + Super_set `48 00 81 8a 00 00 58 02 ce ff 80 fc 00 00 00 04` @0x072C (Werfer an Ada);
  Ada Work_set(2,0) + Pos_set (-28000,-7500,-17900) @0x0740 + Dir_set 0x0e74 @0x0748 + Plc_motion `3f 0a 12 40` @0x0750,
  Sleep 1, Plc_dest `40 00 14 ..` @0x0758, **Xa_on 0 + Message_on 5 @0x0760/0x0764**, **Sleep 30 @0x076A**, Pos/Dir +
  Plc_motion erneut @0x0772-0782, **Sleep 20 @0x0786**, Work_set(4,0) + **Super_reset** `49 00 40 fb 40 02 00 04` @0x078E
  (loslassen), Evt_exec sub13 @0x0796, warten auf (5,0x0e) @0x079C-07A4, **Sleep 15 @0x07A8**, Leon Pos_set
  (-18600,0,-21700) @0x07B0, Tyrant Pos_set (-8000,0,-25000) @0x07C4, **Cut_chg 11 @0x07D4**, Xa_on 1 + Leons Zeile
  @0x07E2-07F4, Ada Plc_dest weg @0x0812, Wsleep, **Cut_chg 0 @0x0820**, Aot_reset 5 (Aufnahme frei) @0x0822, Gosub
  sub18 (Plc_ret, Set(2,7,0), Set(1,27,0), Cut_auto 1 @0x0A2C-0A38).
- sub13 @0x088A (der Fall): Work_set(4,0), Speed_set 2/1/0 = 10/20/25 @0x088E-0896, 8/7/6 = 0/20/5 @0x089A-08A2,
  Do { Evt_next, Add_speed, Add_aspeed } while Member_cmp(0x0c op 3 -800) @0x08A6-08B0, Set(5,0x0e) @0x08B6,
  **Se_on Satz 0x0e @0x08BA** (Aufprall), Pos_set (-24650,0,-21998) + Dir_set (0xfc00,0,0x0400) @0x08CA/08D2 (liegt).


