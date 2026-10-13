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
- [x] Schritt 2: Original messen (Nutzer-Spielstaende + eigene GDB-Laeufe; Elza-Bit = Skript-Flag (1,0), §2)
- [x] Schritt 3: Bauen (Weiche fuer alle Typen, §4)
- [x] Schritt 4: Messung nachher (exe, Framedumps, §5)
- [x] Schritt 5: Riegel probes/r37_elzaems.cmake (§7)
- [x] Schritt 6 (Fortsetzung 2026-10-13, nach zwei Limit-Abbruechen): Zombie-Griff an Elza mit RE1.5-KI, Kraehe,
      0x27, 0x42 in 11B1, Birkin 5091, RE2-KI-Standard, Wechselwirkung J/H/L (§6), gezielte Tests (§7)

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

## 2. Original gemessen (Schritt 2) — UND der entscheidende Zusatzbefund: das Elza-Bit ist ein SKRIPT-FLAG

### 2.1 Nutzer-Spielstaende (DuckStation, MZD-Disc, nur gelesen; N_belege/n2_orig_ram.py -> n2_orig_ram_nutzer.txt)
Alle 21 Staende HASH-957757946319438E_{1..10,resume}.sav/.bak: saubere EXE (@0x80026e4c = 08 00 e0 03). Je Entity die
sechs Bank-Zeiger (+0x16c/+0x84/+0x174/+0x170/+0x17c/+0x178, FUN_80022300 @0x80022498-e8) gegen die Abschnitte beider
TOC-Blobs:
* Elza (Bit 31 = 1): _6 ROOM11B1 Entity 1 **Typ 0x42: EMR2/EDD3/EMR4 == CDEMD1** (EDD1/EDD5/EMR6 in beiden gleich);
  _7.bak ROOM1141 Entity 1/2 **Typ 0x27: EDD5/EMR6 (Opfer-Bank) == CDEMD1**; _1/_2/_3/_5 Typ 0x47 == CDEMD1 (Spur K).
* Leon (Bit 31 = 0): resume ROOM1140 0x16/0x10/0x10/0x11/0x11, _9 ROOM10D0 0x10, _3.bak ROOM1200 4x 0x10, _10.bak ROOM1110
  0x11: eigene + Opfer-Bank == CDEMD0; _9.bak ROOM10C0 3x 0x21: Opfer-Bank == CDEMD0.

### 2.2 Eigene Laeufe am Original (DuckStation-GDB, N_belege/n2_gdb_elza.py; Protokolle N_belege/orig/g*.txt)
Verfahren: Kopie eines Nutzer-Spielstands direkt geladen (-batch -statefile), GDB-Server nur fuer den Lauf (settings.ini
vorher/nachher bytegleich, geprueft "settings.ini zurueck: True"; ConfirmPowerOff fuer den Lauf aus, damit das geordnete
Schliessen einen Endstand schreibt; resume.sav des Nutzers vorher kopiert und bytegleich zurueckgelegt, md5 b177ab42 vor
und nach jedem Lauf). Raumwechsel wie FUN_800430bc (@0x800430c4/@0x800430d4/@0x800430e4): 12-B-Nutzlast (x,y,z,dir,
stage,room,cut,floor — gelesen von FUN_8001d600 @0x8001d87c-0x8001d970) an der Stelle der Nutzlast eines Door_aot_set
des aktuellen Raums; Haltepunkte 0x80022300 (Typ, Elza-Bit) und 0x80022470 (nach dem CD-Lesen: Datei-Id 0x800be572,
TOC-Laenge 0x800be574, Sektor-Feld 0x800be57c, Blob im RAM ab s4 gegen beide TOC-Blobs):

| Lauf | Spieler / Raum | Typ | Elza-Bit beim Laden | Datei-Id | Sektor-Feld | Blob im RAM |
|---|---|---|---|---|---|---|
| g1 | Elza ROOM1101 | **0x10 (Zombie)** | 1 | 0x27 | 3627 (= LBA 3627 + 0) | **== CDEMD1** (214816 B), != CDEMD0 |
| g1 | Elza ROOM3071 | 0x30 Birkin | **0** | 0x26 | 2577 (= 1316 + 1261) | **== CDEMD0** (226404 B) |
| g3 | Elza ROOM50F1 | 0x30 Birkin | **0** | 0x26 | 2577 | **== CDEMD0** |
| g3/g4 | Elza ROOM5081 | 0x30 Birkin | 1 | 0x27 | 5642 (= 3627 + 2015) | **== CDEMD1** (228460 B) |
| g2 | **LEON** ROOM5080 | 0x30 Birkin | **1** | 0x27 | 5642 | **== CDEMD1** (228460 B) |
| g5 | Elza ROOM3091 | **0x42 (Sherry)** | 1 | 0x27 | 5132 (= 3627 + 1505) | **== CDEMD1** (144032 B), != CDEMD0 |
| g5 | Elza ROOM3091 | 0x49 John | 1 | 0x27 | 5336 | == beiden (Blob bytegleich) |

Bilder des Originals (VRAM-Anzeigepuffer des geordnet geschriebenen Endstands, N_belege/n2_vram_bild.py): fuer die
Aufnahme wurde NUR die Lage des einen Aktors per GDB neben Elza gesetzt (g6: 0x42 +0x34/+0x40/+0x6a; g7: Birkin) —
reiner Bildbeleg (Modell/Textur), kein Verhalten. Gegenueberstellung mit dem Port in §4.

### 2.3 Warum: Flag-Bank 1 Bit 0 IST das Elza-Bit, und vier Raumskripte schalten es um
Diagnose g0 (N_belege/n2_gdb_bit31.py, Elza 1171 -> 3071): DAT_800aca3c = 0x80000000 noch beim Raumlader
`srl a0,a0,31` @0x800397e4 (RDT-Variante 1 = ROOM3071), aber **0x00000000 beim Modell-Lader 0x80022300**. Ursache im Raumskript:
* SCD-Set (Opcode 0x22, Handler LAB_8003fdd0) schreibt ueber die Bank-Tabelle @0x80074664: [1] = **0x800aca3c**
  (re15_disasm.py table), Maske 0x80000000 >> (bit & 0x1f) (@0x8003fdd4 `lui t0,0x8000`, `srlv`) -> **Set(1,0,v) = Bit 31
  von DAT_800aca3c = genau das Bit, das FUN_80022300 per `bltz` @0x80022334 liest.**
* Zensus aller 240 RDTs (N_belege/n1_bit31_zensus.py -> n1_bit31_zensus.txt): genau 12 Set(1,0,v), alle als Klammer um
  einen Birkin-Sce_em_set, jede Klammer stellt den Wert des Szenarios im selben Abschnitt wieder her; kein Ck(1,0):

      Elza ROOM3071 sub00: Set(1,0,0) @0x03430, Sce_em_set 0x30 @0x03434, Set(1,0,1) @0x03448   (und @0x03476/7A/8E)
      Elza ROOM50F1 sub00: Set(1,0,0) @0x009A8, Sce_em_set 0x30 @0x009AC, Set(1,0,1) @0x009C0   (und @0x009D0/D4/E8)
      Leon ROOM5080 sub00: Set(1,0,1) @0x00742, Sce_em_set 0x30 @0x00746, Set(1,0,0) @0x0075A
      Leon ROOM5090 sub00: Set(1,0,1) @0x01246, Sce_em_set 0x30 @0x0124A, Set(1,0,0) @0x0125E

  (Der Port-Kommentar in game_state.c fuehrte "Zone 1 Bit 0 (0x80000000, 12x)" bereits als ungebunden.)
=> Der Birkin hat zwei Formen: **CDEMD0-0x30 in 3070/3071 und 50E0/50F1, CDEMD1-0x30 (Sektor 2015, ans Archivende
angehaengt) in 5080/5081 und 5090/5091** — fuer BEIDE Szenarien. Die Lesart des Auftrags "0x30 anderes Modell in
3071/5081/5091/50F1" stimmt nur fuer 5081 (und 5091); in 3071/50F1 zeigt das Original Elza denselben Birkin wie Leon.
**Und Leons ROOM5080 laedt im Original den CDEMD1-Birkin** (g2) — der Port laedt dort heute CDEMD0. In 5090/5091 tauscht
der Port 0x30 seit Runde 7/8 gegen den RE2-EM36 (NUTZER-VORGABE "finaler Birkin", scd_vm.c op_sce_em_set) -> dort wirkt
das Archiv nicht.

### 2.4 Folgerung fuer "Leon bleibt bytegleich"
Der Auftrag nahm an, Leon lade nur CDEMD0. Gemessen (g2): Leons ROOM5080 laedt den CDEMD1-Birkin. Der Original-Mechanismus
(ein Bit, vom Skript umgeschaltet, beim Sce_em_set gelesen) wird hier EINMAL fuer beide Figuren nachgebaut; das aendert bei
Leon genau einen Raum (5080: Birkin-Modell = CDEMD1 wie im Original). Jeder andere Leon-Raum laedt weiter bytegleich
CDEMD0 (Zensus: kein weiteres Set(1,0,1) bei Leon ausser 5090, wo der RE2-EM36 laedt). Im Dossier "Fuer den Nutzer".

## 3. Messung vorher (Port, Bau aus 99b4b13f = Stand Spur K; exe gesichert als scratchpad re15_pc_vorher_99b4b13f.exe, md5 7f9af479)

Laeufe N_belege/n3_port_lauf.sh (Messkopie neben der exe, Arbeitsordner im scratchpad): Elza bzw. Leon Neuspiel
(RE15_PSELECT_AUTO[_SWITCH]), RE15_GOTO_ROOM, RE15_ENEMY_DIAG, RE15_FRAMEDUMP (WINDOW_SCALE=1); 3091 zusaetzlich
RE15_SUBSTART=7@40#3091 + Kreuz alle 30 Bilder (Dialog), 5081 zusaetzlich RE15_SUBSTART=3@60#5081 + RE15_FORCE_CUT=1
(Birkin faellt vor den Generator). debug.log-Auszuege N_belege/port/vorher_*.txt:
* JEDER Raum laedt aus CDEMD0 (keine Archiv-Zeile; "[enemy] EM30 loaded: 17 meshes, 16 bones, 21 clips" in Elza 3071,
  Elza 5081, Elza 50F1 und Leon 5080 gleich; "[enemy] EM42 loaded: 15 meshes, 15 bones, 24 clips" in Elza 3091).
* Bild: Elza 3091 zeigt als Typ 0x42 **Ada** (weisse Bluse, Rock) im Sherry-Dialog ("Sherry: It's Sherry..."); Elza 5081
  und Leon 5080 zeigen die **erste Birkin-Form** (graue Haut, Jeans) — das Original zeigt dort die zweite (§2.2 g2/g4).
* Abweichung je Raum gegen das Original (§2.2): 3091 0x42 falsch, 5081 0x30 falsch, Leon 5080 0x30 falsch, 1101/alle
  Elza-Zombies eigene + Opfer-Bank falsch (unsichtbar im RE2-KI-Standard, siehe §6), 3071/50F1 richtig (zufaellig:
  das Skript verlangt dort CDEMD0).

## 4. Umsetzung

Ein Weg fuer jeden Typ, wie FUN_80022300 (keine Typ- oder Raumliste):

| Datei | Inhalt |
|---|---|
| NEU include/re15_elzaems_r37.h, engine/src/elzaems_r37.c | `re15_elzabit()` = Zwilling von DAT_800aca3c Bit 31: zuletzt per Set(1,0,v) geschriebener Wert, solange dieselbe Figur gespielt wird, sonst die Figur (re15_char_variant; Spiegel TITLE.BIN @0x801024a8-ec). `re15_ems_sce_em_set_merken(typ)` merkt beim Sce_em_set (jal 0x80022300 @0x80042328) das Archiv je Typ (Index Typ-0x10 @0x80022338) mit dem Raum; `re15_ems_archiv_fuer_typ(typ)` liefert es (sonst das aktuelle Bit). |
| engine/src/roy_r37.c (Spur K) | `re15_ems_archiv_aktiv()` liest jetzt `re15_elzabit()`; `re15_typ47_ist_elliot` / `re15_roy_aus_elza_archiv` lesen das gemerkte Archiv des Typs 0x47 (Elliot/Roy unveraendert: in 1011/1021/1170 schaltet kein Skript). |
| platform/pc/src/roy_pc.c (Spur K) | `re15_roy_pc_emd(typ)` gilt fuer JEDEN Typ: gemerktes Archiv == Elza -> den einen Blob aus EMD/CDEMD1.EMS lesen (fseek TOC-Sektor*2048, TOC-Laenge; NICHT die Blobkette, §1.2), debug.log-Zeile mit Sektor, Laenge, CRC32; sonst NULL -> der bisherige CDEMD0-Weg (Port-Kette == TOC fuer alle 25 Typen). Bei Elza + Leon-Archiv (Skriptklammer 3071/50F1) eine Zeile "Elza-Bit 0 beim Sce_em_set". |
| engine/src/game_state.c (1 Zeile + 2 Kommentar) | `re15_game_flag_set(1, 0, v)` -> `re15_elzabit_skript_set(v)` (Bank-Tabelle @0x80074664[1] = 0x800aca3c, Maske 0x80000000). |
| engine/src/scd_vm.c op_sce_em_set (2 Zeilen) | nach `a->type = type`: `re15_ems_sce_em_set_merken(type)` (der Port laedt die Bank erst beim ersten Zeichnen, wenn die Skriptklammer schon zu ist — @0x03448 folgt @0x03434 im selben Lauf). |
| platform/pc/main.c (nur Kommentar der Spur-K-Zeilen) | der Haken der Spur K ruft dieselbe Funktion, jetzt fuer jeden Typ. |

Keine neue Konstante ohne Adresse: Archiv/Sektor/Laenge kommen aus der EXE-TOC (gen/ems_toc_r37.inc, Spur K), 2048 =
CD-Sektor (Einheit der TOC), Set(1,0,v) = Bank 1 Bit 0 (Tabelle @0x80074664). Gleichwertigkeit des Zwillings mit dem
Original-Bit: jede der zwoelf Skriptklammern stellt im selben Abschnitt den Figurenwert wieder her (n1_bit31_zensus);
ausserhalb einer Klammer ist das Bit also immer die Figur, innerhalb der geschriebene Wert. Leon: unveraendert ausser
ROOM5080 (Birkin aus CDEMD1 wie im Original, §2.4) und ROOM5090 (0x30 wird dort zum RE2-EM36, Archiv wirkungslos).
Speicher/Entladen: die Banken fallen weiter an jeder Raumgrenze (re15_enemy_reset); der Merkwert traegt den Raum und gilt
nur in diesem Raum; der CDEMD1-Leser haelt nichts resident (je Typ ein Blob, eb->buf wie bisher).

## 5. Messung nachher (gleiche Laeufe, Bau r37/elzaems; N_belege/port/nachher_*.txt)

    Elza 3071: [elzaems] Typ 0x30 aus EMD/CDEMD0.EMS Sektor 1261 (226404 B) - Elza-Bit 0 beim Sce_em_set (Skript-Flag (1,0), @0x8002233c)
    Elza 50F1: [elzaems] Typ 0x30 aus EMD/CDEMD0.EMS Sektor 1261 (226404 B) - Elza-Bit 0 beim Sce_em_set ...
    Elza 5081: [elzaems] Typ 0x30 aus EMD/CDEMD1.EMS Sektor 2015 (228460 B) crc 1145F206 - Elza-Archiv (@0x800223ac)
    Leon 5080: [elzaems] Typ 0x30 aus EMD/CDEMD1.EMS Sektor 2015 (228460 B) crc 1145F206 - Elza-Archiv (@0x800223ac)
    Elza 3091: [elzaems] Typ 0x49 aus EMD/CDEMD1.EMS Sektor 1709 (137200 B) crc 285C67D3 ...
               [elzaems] Typ 0x42 aus EMD/CDEMD1.EMS Sektor 1505 (144032 B) crc 91415329 ...
               [enemy-diag] actor2 type=0x42 model=enemy-bank slot=12 ...
(Die Laeufe vor der ASCII-Umstellung der Zeile trugen einen Gedankenstrich statt "-".) CRC32 = CRC32 des TOC-Blobs in
der Datei (unit_r37_elzaems_toc) = Blob im Original-RAM (§2.2): Port und Original laden dieselben Bytes, je Raum.

Bilder (Original-Endstand gegen Port, gleicher Raum):
* N_belege/n5_sherry3091_original_vorher_nachher.png — ORIGINAL 3091: Typ 0x42 = blonde Figur in Rot/Weiss; PORT VORHER:
  Ada (weisse Bluse, Rock); PORT NACHHER: dieselbe blonde Figur in Rot/Weiss wie das Original, im Dialog mit John.
* N_belege/n5_birkin5081_original_vorher_nachher.png — ORIGINAL 5081: zweite Birkin-Form (dunkelroter, wuchernder
  Oberkoerper, lange Klaue, nackte Unterschenkel); PORT VORHER: erste Form (graue Haut, Jeans); PORT NACHHER: zweite
  Form wie das Original. (Leon 5080 laedt denselben Blob, crc 1145F206.)
* Elza 3071/50F1: unveraendert erste Form (Original g1/g3: CDEMD0) — vorher wie nachher.

## 6. Die uebrigen Punkte des Auftrags (Fortsetzung 2026-10-13)

Stand vorher fuer alle exe-Vergleiche dieses Abschnitts: die gesicherte exe des Baus 99b4b13f (md5 7f9af479, §3) als
Messkopie neben der exe; nachher: Bau dieses Zweigs. Spielstand per probe_r36_elza_karte + CONTINUE (Lade-Weg des
Spiels), RE15_AI_FLAVOR=re15 wo die RE1.5-KI verlangt ist, RE15_ANIM_TRACE (anim_select_common.c: je aufgeloester
Pose Bild, Slot, Typ, Clip, Bildzahl des Clips = das, was gezeichnet wird).

### 6.1 Zombie-Griff an Elza mit RE1.5-KI — die Hand liegt auf IHRER Schulter
* **Vorher** (J_bissarm.md 2.3 / OFFEN 2): der RE1.5-Flavor gab Elzas Zombies Leons CDEMD0-Bank; die Zombie-Hand lag
  229..284 ueber ihrer Schulter.
* **Original:** FUN_80022300 laedt Elzas Zombie aus Datei 0x27 (`ori v0,zero,0x27` @0x800223ac, kein Typ-Test, §1.1);
  gemessen g1: Elza ROOM1101 Typ 0x10, Blob im RAM == CDEMD1 (§2.2). CDEMD0/CDEMD1 unterscheiden sich bei 0x10-0x16
  GENAU in der Aktions-Bank dir3/dir4 (Griff-Clips) und der Opfer-Bank dir5/dir6 (n1_zensus). Griff = RE1.5
  FUN_80102548, Biss-Schleife +0x6 == 3 @0x801027dc (J 2.3).
* **Nachher, Sonde** `unit_r37_elzaems_griff` (Aufbau der J-Sonde re15_kontakt: ROOM1140, Opfer (-1000,-19600), Greifer
  900 davor/dahinter, Hand 10/13 gegen Schulter 12/9 des Opfer-Skeletts PL04.EMR bzw. PL00.EMR mit den Keyframes der
  Opfer-Bank wie main.c s_victim_skel) — EIN Unterschied zu J: die Bank kommt aus der Port-Weiche
  (re15_ems_archiv_fuer_typ nach dem Raum-Init, Blob per TOC wie roy_pc.c), nicht aus einer fest gewaehlten Datei:

| Lauf (101 Biss-Bilder je Zeile) | Bank | vorn Hand10-S12 / Hand13-S9 | hinten Y |
|---|---|---|---|
| Elza, Port-Weiche (NACHHER) | CDEMD1 | (-31,-65,-181) / (40,-20,22) | 0 / -30 |
| Elza, Leons Bank erzwungen (Stand VORHER) | CDEMD0 | (-16,-229,-77) / (-22,-284,20) | -255 / -255 |
| Leon, Port-Weiche | CDEMD0 | (-64,-127,-2) / (-63,-165,-56) | -138 / -133 |
| Leon, CDEMD0 fest (Stand vorher) | CDEMD0 | bitgleich zur Zeile darueber | bitgleich |

  Die Zahlen sind Wert fuer Wert die der J-Tabelle (J 2.3, gemessen im Zweig r37/bissarm mit fest gewaehlter Datei):
  die Port-Weiche liefert Elza jetzt genau die Bank, mit der J "Original Elza" gemessen hat. Y negativ = oben: die
  Hand liegt 0..65 ueber IHRER Schulter (vorher 229..284), Leon unveraendert.
* **Nachher, exe** (`integration_r37_elzaems_elza1181griff`; Elza ROOM1181 (-3758,-7436) Blick 2048, ein Zombie
  (-3758,-8936) greift von vorn, RE1.5-KI; Belege N_belege/port/{vorher,nachher}_griff1181.txt):

| | vorher (99b4b13f) | nachher |
|---|---|---|
| Lader | `[enemy] EM10 loaded` (CDEMD0-Kette) | `[elzaems] Typ 0x10 aus EMD/CDEMD1.EMS Sektor 0 (214816 B) crc AE292DA8` |
| Elza im Griff: Opfer-Clip 1 / 6 (Bildzahl) | 26 / 65 | **25 / 70** (= CDEMD1 dir5/dir6) |
| Greifer: Aktions-Clip 1 / 9 | 26 / 65 | **25 / 70** (= CDEMD1 dir3/dir4) |

  Bild `N_belege/n6_griff1181_elza_vorher_nachher.png` (RE15_FRAMEDUMP, WINDOW_SCALE=1, Cut 1 nur fuer die Aufnahme
  erzwungen, Bilder 70/100/130 je vorher|nachher): vorher ragen Kopf und Arme des Zombies ueber Elzas Kopf, nachher
  liegt sein Arm auf ihrer Schulter. Urteilstragend ist die Sonde (Zahlen), das Bild zeigt dieselbe Lage.

### 6.2 Kraehe 0x21 — Opfer-Bank (Elza ROOM10C1)
* Original: Lader ohne Typ-Test (§1.1); Leon-Gegenstueck am Original: Nutzer-Stand _9.bak ROOM10C0, 3x 0x21, Opfer-Bank
  == CDEMD0 (§2.1). CDEMD1-Kraehe: Opfer-Clip 1 36 -> 31, Clip 2 20 -> 30 Bilder (n1_zensus, unit_r37_elzaems_toc).
* exe (Elza (3000,0) zwischen den drei Kraehen, RE1.5-KI, 900 Bilder; N_belege/port/{vorher,nachher}_kraehe10c1.txt):
  vorher Spieler-Opfer-Clip 1 = 36 / Clip 2 = 20 Bilder (Leon-Bank), nachher **31 / 30**, Lader
  `[elzaems] Typ 0x21 aus EMD/CDEMD1.EMS Sektor 760 (70364 B) crc 921598F6`. Leon ROOM10C0: 36 / 20, keine Elza-Zeile
  (`integration_r37_elzaems_leon10c0kraehe`). Die Opfer-FSM liest die Bildzahl aus der Bank des Greifers
  (enemy_ai_common.c ~Z.1720/1749, `vb->anim_victim.clips[clip].frame_count`), keine feste Tabelle.

### 6.3 0x27 — Opfer-Bank (Elza ROOM1141)
* Original: Nutzer-Stand _7.bak ROOM1141 Entity 1/2 Typ 0x27, Opfer-Bank (EDD5/EMR6) == CDEMD1 (§2.1).
* Spawn: ROOM1141 sub00 `Ifel_ck` + Ck(4,242) @0x00B4A -> Sce_em_set 0x27 @0x00B54/@0x00B68 (rdt-Dump).
* exe (Karte mit Flag 4:242, `integration_r37_elzaems_elza1141`): `[elzaems] Typ 0x27 aus EMD/CDEMD1.EMS Sektor 893
  (207236 B) crc 2A8D48B1`. Im 2600-Bilder-Lauf griffen die 0x27 mehrfach (Wurf = Opfer-Clip 1, 83 Bilder, in beiden
  Archiven gleich); der Finisher (Sprung-Treffer B[8]: hp -= 600 @0x80119198-ac, re15_player_victim_devour
  @0x801191c8-cc) kam nicht vor.
* Deshalb Sonde `unit_r37_elzaems_opfer27` mit demselben Eintritt wie B[8]: der Finisher (Hook 0x8011c414,
  engine/src/affen_11c0.c) spielt Clip 0 der Opfer-Bank (`pl->motion = 0` @0x8011c490) und meldet den Tod bei Bild
  fc-1 (`k + 1 >= fc` @0x8011c548-50): Elza/Port-Weiche **63 Bilder, Tod bei Bild 62**; Leons Bank (Stand vorher)
  70 / 69.

### 6.4 0x42 in ROOM11B1 und ROOM3091
* 11B1: Sce_em_set @0x01058 setzt 0x42 (wie 0x40 @0x0106C) mit Verhalten 0x40 auf (-30000,0,-30000); kein Skript des
  Raums waehlt Gegner 1 an (Work_set nur kind 1 = Spieler und kind 2 idx 0 = 0x4B; rdt-Dump) — die Figur ist im
  Original wie im Port nie im Bild. Geladen wird sie trotzdem: Original Nutzer-Stand _6 EMR2/EDD3/EMR4 == CDEMD1;
  Port nachher `[elzaems] Typ 0x42 aus EMD/CDEMD1.EMS Sektor 1505 (144032 B) crc 91415329`
  (`integration_r37_elzaems_elza11b1`; 0x40 und 0x4B ebenfalls aus CDEMD1, beide Blobs in beiden Archiven gleich).
* 3091: Modell §5 (Sherry statt Ada). Animationen: 6.4b.

@@3091@@

### 6.5 Birkin 0x30 in ROOM5091
* Port: in 5090/5091 wird 0x30 beim Sce_em_set zu 0x36 umgetypt (scd_vm.c op_sce_em_set, NUTZER-VORGABE 2026-09-12
  "finaler Birkin aus RE2") und main.c laedt fuer 0x36/0x37 in diesen Raeumen das RE2-EM36/EM37 VOR jedem
  RE1.5-Archiv (`[enemy] FINALER Birkin: RE2 EM036 geladen`). Messung Elza 5091 (`integration_r37_elzaems_elza5091`):
  genau diese Zeile, keine `[elzaems] Typ 0x30/0x36`-Zeile.
* Original: ROOM5091 hat kein Set(1,0,v) (n1_bit31_zensus) -> Bit 31 = Elza -> Datei 0x27, der CDEMD1-Birkin; Leon
  5090 laedt per Klammer Set(1,0,1) @0x01246 denselben. Beide Szenarien zeigen im Original also dieselbe Form; der
  Port zeigt in beiden den RE2-EM36 (Nutzer-Vorgabe, unveraendert).

### 6.6 RE2-KI-Standard (Voreinstellung)
Im RE2-Flavor zeichnet der Port 0x10-0x13/0x16/0x18/0x20/0x21/0x25 als WELLE-G-Hybrid (RE2-Clips/-Opfer-Bank,
RE1.5-MD1/-TIM/-Bind-Lagen, fuer 0x10 zusaetzlich die Sitz-Clips 0x29/0x2A der RE1.5-Aktions-Bank). Gemessen
(`unit_r37_elzaems_toc` 3b): Bind-Lagen + Hierarchie + MD1 aller neun Typen in beiden Archiven gleich, die Sitz-Clips
59/28 Bilder mit bytegleichen Posen; MD1/TIM/Loco der Zombies laut n1_zensus bytegleich. => Im Standard aendert die
Spur fuer diese neun Typen NICHTS am Bild (der Hybrid liest jetzt den CDEMD1-Blob, dessen gelesene Teile gleich sind).
Sichtbar wird sie im Standard bei den Typen ohne RE2-Gegenstueck: Birkin 5081 (und Leon 5080), Sherry 3091, 0x27
(Opfer-Clip 0), 0x29 (Opfer-Bank 4 -> 2 Clips), 0x2B (8 -> 11 Clips), Roy (Spur K). Im RE1.5-Flavor zusaetzlich alle
Zombie-Griffe (6.1) und die Kraehe (6.2).

### 6.7 Wechselwirkung mit J (bissarm), H (zombieaktiv), L (re2zombie)
* **J** (r37/bissarm, nur gelesen): J OFFEN 2 ("RE1.5-Flavor Elza laedt die Leon-Bank") ist mit 6.1 geschlossen — die
  Datei-Id-Auswahl, die J als "naechsten Weg" nannte, ist FUN_80022300 @0x80022334/@0x800223ac, nicht die PLD-Tabelle.
  Der Opfer-Bezug in bissarm_r37.c (Schulter-Differenz Elza PL04 gegen Leon PL00, aufgetragen auf den RE2-Leon-
  Kontaktpunkt) wirkt NUR im RE2-Griff (Handkontakt, solange 0x801025EC laeuft, Zustand 1 / +0x5 == 3); dort kommen
  Griff- und Opfer-Clips aus RE2 (shared_assets/RE2/CDEMD0.EMS, Leon-Daten) und die Hybrid-Eingaenge sind in beiden
  Archiven gleich (6.6) -> der Opfer-Bezug BLEIBT, unveraendert (RE2-Claire-Daten fehlen weiter, J OFFEN 3). Im
  RE1.5-Griff laeuft J's Handkontakt nicht; dort liefert jetzt die CDEMD1-Bank die Hand auf Elzas Schulter — ohne
  Schulter-Differenz. J's Wandklemme (Startwert/EIN Durchgang in re15_victim_place) ist bankunabhaengig; J's
  Elza-Zahlen (J 2.3) und die Sonde 6.1 (ohne J-Code im Baum) sind wertgleich -> keine Wechselwirkung im Zahlenwert.
* **H** (Spawn-Haken re2z_init, enemy_ai_re2_zombie.c / enemy_ai_common.c): N aendert keinen Spawn; der Merk-Haken
  sitzt in op_sce_em_set NACH `a->type = type` und liest nur Typ und Bit.
* **L** (RE2-Zombie-KI): RE2-Banken, kein RE1.5-Archiv -> keine Wechselwirkung (6.6).
* Zusammenfuehrung: `git merge-tree --write-tree r37/integration HEAD` (2026-10-13): Code (scd_vm.c, main.c) ohne
  Konflikt; Konflikte nur in AUFTRAG.md/VERTRAG.md (Dokument-Stand 99b4b13f gegen den neueren Orchestrator-Stand).
* Gemeinsame Dateien dieser Spur: scd_vm.c (2 Zeilen, Merk-Haken), game_state.c (1 Zeile + 2 Kommentar, Set(1,0,v);
  in VERTRAG 1.4 nicht namentlich gelistet, Haken-Groesse wie dort verlangt), main.c (nur Kommentar der Spur-K-Zeilen),
  roy_r37.c/roy_pc.c (Spur-K-Dateien, laut VERTRAG 1.4 fuer N erweiterbar).

