# Runde 37 Spur N "elzaems" — Elzas Szenario laedt jeden Typ aus CDEMD1 wie das Original

Stand: 2026-10-12, Baum `.claude/worktrees/r37_elzaems` (Zweig `r37/elzaems`, Basis 99b4b13f = Spur K 238bb610 + AUFTRAG/VERTRAG).

## Punkte (Wortlaut, AUFTRAG.md massgeblich)

1. Nachtrag 7: "Bei Elzas Szenario stimmt weder das Modell von Roy noch all seine Animationen. Außerdem wiederholen sich
   seine Animationen wieder ständig, wie wir schon ungefähr 500x vorher bei Leon gefixed haben....." — Roy durch Spur K
   erledigt; Spur N schliesst dieselbe Fehlerklasse fuer ALLE uebrigen Typen.
2. Orchestrator-Auftrag N: in Elzas Szenario laedt der Port jeden Gegner-/NPC-Typ wie das Original aus CDEMD1.EMS
   (FUN_80022300: Elza-Bit bltz @0x80022334 -> Datei 0x27 @0x800223ac, TOC @0x80073178). Leon bleibt bytegleich.

## Fortschritt (fortlaufend)

- [x] Schritt 0: Pflichtlektuere (AUFTRAG, VERTRAG, K_roy, K_abnahme_0, J_bissarm via `git show r37/bissarm:`)
- [x] Schritt 1: Zensus CDEMD0 gegen CDEMD1 je Typ/dir-Eintrag + Vorkommen in Elza-Raeumen + Port-Lesepfade (§1)
- [ ] Schritt 2: Original messen (Lader FUN_80022300 ganz, Savestates mit Elza)
- [ ] Schritt 3: Bauen (Weiche fuer alle Typen)
- [ ] Schritt 4: Messung nachher (exe, Framedumps)
- [ ] Schritt 5: Riegel probes/r37_elzaems.cmake

## 1. Zensus (Schritt 1)

### 1.1 Lader des Originals — gilt die Weiche fuer JEDEN Typ? (FUN_80022300 ganz gelesen, re15_disasm.py)
Einziger Aufrufer: Sce_em_set-Handler `80042328 jal 0x80022300` (ghidra1_V2.txt: XREF[1] 80042328; die TOCs
@0x80072f38/@0x80073178 haben je genau EINE Lese-Xref = FUN_80022300). Ablauf:

    80022324 lbu  v0,8(s2)              ; Typ
    8002232c lw   v1,-13764(v1)         ; DAT_800aca3c
    80022334 bltz v1,0x800223ac         ; Elza-Bit — KEIN Typ-Test davor oder danach
    80022338 addiu a0,v0,-16            ; Index = Typ - 0x10 (Leon UND Elza)
    Leon: 8002233c ori v0,zero,0x26 / 8002234c sh v0 -> 0x800be572 (Datei-Id) / 80022354 0x80072f3c (Laenge)
          80022370 0x80072f38 (Sektor) / 8002238c 0x80073138 (Byte je Typ) -> 800223a0 sb -> 0x8006f573
    Elza: 800223ac ori v0,zero,0x27 / 800223bc sh v0 -> 0x800be572 / 800223c4 0x8007317c / 800223e0 0x80073178
          800223fc 0x80073378 (Byte je Typ) -> 80022410 sb -> 0x8006f57b
    80022418-80022468: LBA = Dateitabelle 0x8006f440[id*8] (lhu +0 / lbu +2 <<16) + TOC-Sektor, jal 0x80013c50
    80022488-800224f8: +0x1b0/+0x16c/+0x84/+0x174/+0x170/+0x17c/+0x178/+0x10 = dir[0..7] (positionsfest)

Sonderfaelle im Lader (NICHT nach Szenario, nach Entity-Byte +0x97 = Sce_em_set pc[6], `sb v1,151(s0)` @0x8004225c):
* +0x97 == 1 (80022510-80022548): MD1 per `jal 0x800104b0` an die Stelle von dir[3] geschoben; die eigene Bank
  (+0x170..+0x17c) kommt danach vom Vorgaenger-Entity (Sce_em_set @0x80042358-800423bc, DAT_800b3f74).
* +0x97 == 2 (8002254c-800225f4): dir[1]/dir[2] verworfen, +0x16c := DAT_800acbc0, +0x84 := DAT_800acad8
  (= Spieler +0x16c/+0x84, Spieler-Entity 0x800aca54) — im ganzen RDT-Zensus kommt +0x97 = 2 NICHT vor
  (n1_typzensus.txt: nur 0 und 1).
* Gleicher Typ wie der zuletzt geladene (DAT_800b3f78, Sce_em_set `beq v1,v0,0x80042478` @0x800422f0): KEIN
  Laden, alle Banken vom Vorgaenger kopiert (@0x80042478-8004251c) — also aus dem Archiv des Vorgaengers.

=> Die Archivwahl gilt fuer JEDEN Typ gleich; es gibt keinen Typ- oder Raum-Sonderfall.

### 1.2 Blob-Vergleich je Typ und dir-Eintrag (N_belege/n1_zensus.py -> n1_zensus.txt)
Blob-Lage aus den TOCs der EXE. Zusatzbefund: die Blobkette des Ports (re15_ems.c `re15_ems_get_entry`, Reihenfolge
s_ems_order) liefert fuer CDEMD0 bei allen 25 Typen GENAU den TOC-Blob (Leon-Weg des Ports == Original), fuer CDEMD1
aber bei **0x30 NICHT**: Kettenplatz 16 liegt @0x278800 (226404 B = die Leon-Laenge), die TOC zeigt auf Sektor 2015
(@0x3EF800, 228460 B, ans Archivende angehaengt). Ein Ketten-Lader waere fuer Elzas Birkin falsch -> der Elza-Weg
liest wie FUN_80022300 ueber die TOC.

| Typ | Sektor CDEMD0 / CDEMD1 | Unterschied (alles andere bytegleich) |
|---|---|---|
| 0x10, 0x11, 0x12, 0x16 | 0, 105, 210, 427 (beide) | dir3/dir4 eigene Bank (43 Clips; Bilder Clip 1 26->25, 2 30->18, 4 21->25, 5 30->18, 9 65->70, 10 65->70, 29 29->41) + dir5/dir6 Opfer-Bank (14 Clips; 1 26->25, 2 20->36, 4 21->25, 5 20->34, 6 65->70, 7 65->70, 10 49->46, 13 49->46); MD1/TIM/Loco gleich |
| 0x13 | 316 | dito (42 Clips; 0,1,3,4,5,9,10,29) + Opfer-Bank (0..7,10,13) |
| 0x18 | 532 | nur Opfer-Bank dir5/dir6 (wie 0x10) |
| 0x20 Hund | 677 / 678 | dir2 Keyframes (Bind + Hierarchie gleich) + dir6 Opfer-Keyframes |
| 0x21 Kraehe | 759 / 760 | Opfer-Bank (3 Clips; 1 36->31, 2 20->30) |
| 0x25 Spinne | 812 / 813 | dir6 Opfer-Keyframes |
| 0x27 | 892 / 893 | Opfer-Bank (2 Clips; 0 70->63) |
| 0x29 | 994 / 995 | Opfer-Bank 4 Clips {70,39,20,51} -> 2 Clips {63,110} |
| 0x2B | 1103 / 1104 | Opfer-Bank 8 -> 11 Clips (8..10 = 6/30/30 neu) |
| **0x30 Birkin** | 1261 / **2015** | **MD1 31364 -> 37028 B, TIM anderer Inhalt**, Loco-Clip 1 68 -> 27, dir2 Keyframes (Bind/Hierarchie gleich), dir6 |
| **0x42** | 1501 / 1505 | **MD1 19868 -> 28164 B, TIM 99872 -> 66592 B, Bind-Lagen dir2 anders**, eigene Bank 6 -> 14 Clips |
| 0x47 | 1644 / 1639 | Roy (Spur K) |
| 0x1A, 0x24, 0x26, 0x2D, 0x36, 0x40, 0x45, 0x49, 0x4B, 0x4D | — | **ganzer Blob bytegleich** (md5 je Typ in n1_zensus.txt) |

0x42 bei Elza ist **Sherry** (nicht Ada): ROOM3091 sub07 (Elza) fuehrt Gegner 1 = Typ 0x42 (Sce_em_set @0x014F4)
durch eine Dialogszene mit John (Gegner 0, 0x49), Texte msg 15/20/22/24/28/29 "Sherry: ..." (rdt_msgdump.py);
Ada (CDEMD0) und CDEMD1-0x42 haben dieselbe Wurzelhoehe -1709, aber andere Glied-Lagen (n1: Bind-Lagen VERSCHIEDEN).

### 1.3 Wo die Typen bei Elza vorkommen (N_belege/n1_typzensus.py -> n1_typzensus.txt, Sce_em_set aller 240 RDTs)
Elza-Raeume (Endziffer 1) mit einem Typ, dessen CDEMD1-Blob abweicht:
* 0x10 (75 Saetze): 1011, 1021, 1071, 1091, 10B1, 10D1, 10E1, 1101, 1181, 11E1, 11F1, 1201, 1251, 3001, 3011, 30D1, 30E1
* 0x11 (34): 1021, 1071, 1091, 1111, 11F1, 1251, 3001, 3011, 30D1, 30E1 | 0x13 (2): 4051 | 0x16 (76): 1031, 1041, 1221, 2001, 2021
* 0x18 (17): 4051, 40A1, 40B1, 5061, 5121, 5141 | 0x20 (28): 1191, 11C1, 11D1, 1231, 3061 | 0x21 (15): 10C1, 1121, 1171
* 0x25 (31): 2031, 2051, 2061, 2071, 2091, 20A1 | 0x27 (4): 1141 | 0x29 (14): 3041, 4041, 4081, 5001, 5031, 50A1, 5101
* 0x2B (16): 4001, 5041, 5071, 5111, 5131
* **0x30 (6): 3071 @0x03434/@0x0347A, 5081 @0x00742, 5091 @0x01232, 50F1 @0x009AC/@0x009D4**
* **0x42 (2): 11B1 @0x01058 (geparkt bei -30000/-30000), 3091 @0x014F4 (Sherry, Szene sub07)** | 0x47 (3): 1011, 1021 (Spur K)

### 1.4 Welche Port-Pfade welchen dir-Eintrag lesen (Stand 99b4b13f)
EIN Lesepunkt fuer RE1.5-Gegnerdaten: `pc_enemy_read_re15_emd` (platform/pc/main.c ~Z.852; Haken der Spur K davor).
Ausser ihm liest nur `pc_cdemd()` (CDEMD0.EMS resident) — und nur fuer diesen Leser. Verbraucher:
* RE1.5-Flavor + alle nicht RE2-eigenen Typen (`pc_enemy_load_ex`, ~Z.1247): Haupt-Bank = groesste EDD-Bank
  (re15_emd_parse_container), Loco dir1/dir2 (re15_emd_parse_loco_bank), Opfer dir5/dir6 + Struktur dir2
  (re15_emd_parse_victim_bank), eigene Bank dir3/dir4 (re15_emd_parse_own_bank), MD1 dir7, TIM dir8.
* RE2-Flavor (Standard) fuer 0x10-0x13/0x16/0x18/0x20/0x21/0x25: RE2-Bank (shared_assets/RE2/CDEMD0.EMS) +
  WELLE-G-Hybrid `pc_enemy_hybrid_re15_models` -> derselbe Leser: RE1.5-MD1 + TIM + Bind-Laengen (dir2) via
  re2_hybrid_apply; Typ 0x10 zusaetzlich SITZ-IMPORT (RE1.5-Haupt-Bank = dir3/dir4, Clips 0x29/0x2A).
* NPC-Executor (Elliot-Sonderweg nur Leons 0x47, Spur K), Marker-Binder FUN_8001b3f8 (Raum-RBJ, nicht CDEMD),
  Opfer-FSM des Spielers (anim_victim/skel_victim des Greifers; Bildzahlen aus der Bank, keine Tabelle).
* Feste CDEMD0-Tabellen im Port: s_crow_clip_len (0x21 dir1), s_dog_clip_len (0x20 dir1), s_maggot_clip_len (0x27 dir1),
  s_irons_clip_len/s_em040_own_clip_len (0x40), s_roach_clip_len (0x29 dir1) — alle Quell-Abschnitte sind in CDEMD1
  BYTEGLEICH (1.2) -> fuer Elza richtig, keine Aenderung noetig. Birkin (re15_birkin_anim) liest die Bildzahl aus
  der Bank.
* Unabhaengig vom Archiv (Port-Konstrukte, bleiben): 0x36/0x37 in 5090/5091 = RE2-EM36 rein (finaler Birkin,
  NUTZER-VORGABE fruehere Runden), 0x23 = RE2-EM23, 0x1A = RE2-EM2D (RE2-Flavor), Elliot = PLD/ELLIOT.* (Leon).
* Banken fallen an jeder Raumgrenze (entladen_pc.c alles_entladen -> re15_enemy_reset) -> die Archivwahl beim
  Laden genuegt, kein Cache ueber Szenario-Grenzen.

## Messung vorher

(folgt)

## RE-Belege

(folgt)

## Umsetzung

(folgt)

## Messung nachher

(folgt)

## Tests

(folgt)

## OFFEN

(folgt)

## Fuer den Nutzer

(folgt)
