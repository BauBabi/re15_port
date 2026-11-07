# WS Leser B — Lebenspunkte aller Gegner im Port und D_G der Handgranate

Stand 2026-11-07. Nur gelesen: Integrationsbaum `C:/workspace/git/reAi_v2/.claude/worktrees/r41_integration` (r41/integration),
dazu die Overlays `info/Re1.5/PSX/BIN/STAGE{1..5}.BIN` (= `re15_port/shared_assets/PSX/BIN`, gegengelesen mit
`Version_40%/PSX/BIN`) und die RE2-Module `info/re2leon/COMMON/BIN/*.BIN`. Ich habe nichts gebaut, nichts im Baum geändert und nichts committet.

Werkzeuge (eigene, im Scratchpad):
* `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/ws/werkzeuge/em_zensus.py`
  zählt alle `Sce_em_set` (0x44, 20 B, Typ = Byte +2) in allen RDTs. Den Walker übernimmt es aus `re15_port/tools/scd_dump_room.py`.
  Ergebnis: `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/ws/werkzeuge/em_zensus.json`.
* `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/ws/werkzeuge/hp_tabellen.py`
  und `.../hp_tabellen2.py` lesen die HP-Tabelle aller Stages (Datei-Offset = Adresse − 0x80100000, ohne Header).
* Disassembliert habe ich mit `C:/workspace/git/reAi_v2/.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` und `re2_disasm.py`. Die Adressen stehen unten.

## 0. Ergebnis in drei Sätzen

* **D_G = 251.** Das Maximum stellt der Zombie-Typ **0x11**. Unter der RE2-KI (Vorgabe), unter der RE1.5-KI (Vorgabe:
  RE2-Zombie-Import an) und unter MIXED hat er feste **250 HP** (`addiu v1,zero,250` @0x801008C8 /
  `sh v1,342(s2)` @0x801008CC, EMOVL10_S0.BIN; RE2-EM011 = Brad Vickers). 250 − 251 = −1 < 0. Nach der Todesregel ist er damit tot
  (RE1.5 `bgez v1` @0x80013014, RE2 `bgez v1` @0x80047284).
* **0x11 ist ein Ausreißer.** Ohne ihn läge das Maximum bei 143: übrige Zombie-Familie, 128 @0x8010C674 + 15 @0x80100810.
  D_G wäre dann 144. Der nächste Typ dahinter ist Hund bzw. Spinne mit 132.
* **Die RE1.5-HP ohne Import erreicht man nur über Umgebungsvariablen.** Mit `RE15_RE15_RE2Z_IMPORT=0` oder
  `RE15_RE2_DMG_MODEL=0`, also nicht über das Menü, gilt die byte-true RE1.5-HP-Zeile. Der Typ 0x18 hat dort in Spalte 3 den Einzelwert
  **1058** (`22 04` @0x8011f33a, auch auf der 40%-Disc). D_G wäre dann 1059, ohne diese Zelle 137 (Spinne 136 @0x80117634).
  Der Port kennt **keinen Schwierigkeitsgrad**. Das RE2-Härtebit 0x40 ist immer gesetzt, siehe §1.4.

## 1. Gegnertypen im Port (Raum-Zensus) und Start-HP

### 1.1 Zensus der Sce_em_set-Records (alle RDTs unter `re15_port/shared_assets/PSX/STAGE*`)

Gezählt sind Records, nicht gleichzeitige Gegner: XXX0/XXX1 sind die Leon- und Elza-Fassung, dazu kommen bedingte Alternativen
(z. B. 1141/11C0 je 2 Szenen- und 2 Kampf-Gorillas auf denselben Slots).

| Typ | Records | Räume (Records) |
|---|---|---|
| 0x10 Zombie | 154 | 1010/1011 (4/4), 1020/1021 (4/4), 1070/1071 (6/6), 1090/1091 (2/2), 10B0/10B1 (4/4), 10D0/10D1 (2/2), 10E0/10E1 (10/10), 1100/1101 (10/10), 1140 (4), 1180/1181 (3/3), 11E0/11E1 (4/4), 11F0/11F1 (4/4), 1200/1201 (8/8), 1250/1251 (4/4), 3000/3001 (3/3), 3010/3011 (2/2), 30D0/30D1 (4/4), 30E0/30E1 (1/1) |
| 0x11 Zombie (RE2: EM011 Brad) | 72 | 1020/1021 (4/4), 1070/1071 (4/4), 1090/1091 (2/2), 1110/1111 (1/1), 1140 (4), 11F0/11F1 (4/4), 1250/1251 (3/3), 3000/3001 (4/4), 3010/3011 (7/7), 30D0/30D1 (4/4), 30E0/30E1 (1/1) |
| 0x12 Zombie | 0 | — (Zeile vorhanden, kein Spawn) |
| 0x13 Zombie-Mädchen | 4 | 4050/4051 (2/2) |
| 0x16 Zombie | 154 | 1030/1031 (20/20), 1040/1041 (40/40), 1140 (2), 1220/1221 (10/10), 2000/2001 (2/2), 2020/2021 (4/4) |
| 0x18 Zombie | 35 | 4050/4051 (2/2), 40A0/40A1 (2/2), 40B0/40B1 (2/2), 5020 (1), 5060/5061 (5/5), 5120/5121 (2/2), 5140/5141 (4/4) |
| 0x1A Gitterhände | 20 | 1210/1211 (10/10) |
| 0x20 Hund | 53 | 1190/1191 (6/6), 11C1 (5), 11D0/11D1 (11/11), 1230/1231 (2/2), 3060/3061 (6/4) |
| 0x21 Krähe | 28 | 10C0/10C1 (3/3), 1120/1121 (3/3), 1170/1171 (7/9) |
| 0x24 FX-Emitter | 70 | 20B0/20B1 (35/35) — kein Gegner, keine HP |
| 0x25 Spinne | 62 | 2030/2031 (7/7), 2050/2051 (2/2), 2060/2061 (4/4), 2070/2071 (12/12), 2090/2091 (2/2), 20A0/20A1 (4/4) |
| 0x26 | 7 | 1090 (7) = RE1.5-Feuer-Emitter (Nullzeile, waffenimmun). RE2-Babys entstehen zur Laufzeit aus der Spinne |
| 0x27 Gorilla | 8 | 1141 (4), 11C0 (4); je 2 mit grid 0x30 (Szene) und 2 mit grid 0x10 (Kampf), z. B. 1141 sub00 @0x00B1A/@0x00B2E (0x30) und @0x00B54/@0x00B68 (0x10) |
| 0x29 „Kakerlake“ (Port R39: kleiner Affe) | 32 | 3040/3041, 4000, 4040/4041, 4080/4081, 5000/5001, 5031, 50A0/50A1, 50B0, 5100/5101 |
| 0x2B Tyrant/Wandkriecher | 28 | 4001 (4), 5040/5041 (4/4), 5070/5071 (4/4), 5110/5111 (2/2), 5130/5131 (2/2) |
| 0x2D Ivy | 3 | 4030 (1), 4031 (2) |
| 0x30 Birkin | 12 | 3070/3071 (2/2), 5080/5081, 5090/5091, 50E0 (2), 50F1 (2) |
| 0x36 | 1 | 3080 (Szene, waffenimmun) |
| 0x40–0x4D NPCs | 101 | viele, HP −1 (`addiu v0,zero,-1` @0x8011d320 / `sh` @0x8011d324, STAGE1), kein Ziel |
| 0x00 / 0x04 | 8 | 11A0/11A1, 2030/2031, 20A0/20A1: **Fehltreffer des Walkers** in Meldungsdaten (Rohbytes ASCII-artig, z. B. `44 41 00 4b 50 44 41 4e ...`), keine Spawns |

Ohne Sce_em_set im RDT setzt der Port selbst:
* **Gator-Boss 0x23** (2090/2091, `re15_gator_boss_active`, enemy_ai_boss_gator.c:334)
* **G5 0x36** (5090/5091)
* **Riesenform** (Bahnsteig, birkin_bahnsteig.c)
* **G5-Tentakel 0x37** (HP −1 @0x80100530/34, kein Ziel)
* **RE2-Babyspinnen**
* **zusätzliche Zombies 0x10/0x16** in elzazombies_r39.c, irons_tod_1150.c und kanister2080_r41.c. Sie tragen dieselben Typen und damit dieselben HP-Regeln.

Einen RE1.5-Alligator 0x23 gibt es in keinem RDT. Die 300 HP (@0x801175dc) dienen im Port nur als Messhilfe (GR_granate.md N3.3).

### 1.2 RE1.5-KI: HP-Herkunft (byte-true Overlay)

Die gemeinsame Zeilentabelle ist in jeder Stage gleich: `HP = *(u16*)(Basis + Typ*0x20 + (rng&0xf)*2)`.
Basen: STAGE1 0x8011f034, STAGE2 0x8011717c, STAGE3 0x8011d1c8, STAGE4 0x801185a0, STAGE5 0x8011e108. Ich habe alle Zeilen
in allen fünf Stages gelesen, sie sind bytegleich, auch mit `Version_40%`.

| Typ | Leser (selbst disassembliert) | Zeile | min..max (Adresse des Max) |
|---|---|---|---|
| 0x10 | STAGE1 `addiu a0,a0,-4044` @0x801007c8, `lbu v1,8(a1)` @0x801007d8, `sll v1,v1,5` @0x801007e0, `sh v0,154(a1)` @0x801007f4 | @0x8011f234 | 61..101 (@0x8011f242) |
| 0x11 | dito | @0x8011f254 | 71..107 (@0x8011f262) |
| 0x13 | eigene Wurzel FUN_8010a8c8: `andi v0,v0,0x1f` @0x8010ac0c, `addiu v0,v0,50` @0x8010ac18, `sh v0,154(v1)` @0x8010ac1c | — | 50..81 |
| 0x16 | dito Zombie-Leser | @0x8011f2f4 | 71..99 (@0x8011f304) |
| 0x18 | dito | @0x8011f334 | 71..**1058** (@0x8011f33a, 1 von 16 Spalten); ohne diese Zelle 109 (@0x8011f34a) |
| 0x1A | Original: kein Store auf +0x9a im Arm-Baum 0x8010c1ec..0x8010d774. PORT-NACHRÜSTUNG (Nutzerauftrag 2026-08-26, enemy_ai_common.c:12949) nimmt dieselbe Routine wie Hund-INIT | @0x8011f374 | 72..99 |
| 0x20 | STAGE1 `addiu a0,a0,-4044` @0x8010dae8 … `sh v0,154(a1)` @0x8010db14 | @0x8011f434 | 65..111 (@0x8011f448) |
| 0x21 | Zeile @0x8011f454 = 16 × 1. Das Schwanz-Latch setzt die HP jedes Bild: `slti v0,v0,5200` @0x80115f88, −1 @0x80115f98 bzw. `sh zero,154` @0x80115f9c | — | **0** (hoch fliegend −1 = kein Ziel) |
| 0x25 | STAGE2 `addiu a0,a0,29052` (=0x8011717c) @0x80110da4 … `sh v0,154(a1)` @0x80110dd0 | @0x8011761c | 76..136 (@0x80117634) |
| 0x27 | STAGE1 `addiu a0,a0,-4044` @0x801170d8 … `sh v0,154(a1)` @0x80117104 | @0x8011f514 | 180 (alle 16) |
| 0x29 | STAGE3 `addiu a0,a0,-11832` (=0x8011d1c8) @0x80110ee4 … `sh v0,154(a1)` @0x80110f10 | @0x8011d6e8 | 81..121 (@0x8011d700) |
| 0x2B | STAGE4 `addiu a0,a0,-31328` (=0x801185a0) @0x80111bd8 … `sh v0,154(a1)` @0x80111c04 | @0x80118b00 | 86..126 (@0x80118b1a) |
| 0x2D | STAGE4 `ori v0,zero,0x64` @0x80116948 / `sh v0,154(a1)` @0x80116954 | — | 100 |
| 0x23 | STAGE2 `addiu a0,a0,29052` @0x8010c6a4 … `sh v0,154(a1)` @0x8010c6d4 | @0x801175dc | 300 (nur Messhilfe) |
| 0x30 | STAGE3 `ori v0,zero,0x12c` @0x8011690c / `sh v0,154(v1)` @0x80116910 (Tabellenzeile 350 @0x8011f634 wird nicht gelesen) | — | 300 |

### 1.3 RE2-KI (Vorgabe seit 2026-08-22): HP-Herkunft

Die RE2-Gehirne besitzen 0x10/0x11/0x12/0x13/0x16/0x18 (`re15_re2z_owns_type`, enemy_ai_re2_zombie.c:214) sowie 0x20, 0x21,
0x25 und 0x26 (`re15_re2_owns_type`, enemy_ai_re2_dog.c:81). `re15_re2_hp_sync` stempelt deren INIT-HP aus
`re15_re2_init_hp` (re15_damage.c ~950). Für 0x1A in 1210 läuft das RE2-Zellenarm-Gehirn. Alle anderen Typen fahren
auch unter der RE2-KI ihr RE1.5-Gehirn mit den RE1.5-HP aus §1.2.

Die Werte habe ich selbst gegengelesen (`re2_disasm.py read/dis`):

| Typ | RE2-Quelle | Wert |
|---|---|---|
| Zombie-Familie | EMOVL10_S0.BIN (S1 und EMZ0.BIN gleiche Tabelle): `andi v0,v0,0x40` @0x801006d4 (gesetzt → Tabelle @0x8010C670; die Tabelle @0x8010C600 ist 10 niedriger und unerreichbar), `lhu v0,-14736(at)` @0x80100708, `sh v0,342(s2)` @0x80100710; Tabelle @0x8010C670 = {80,94,128,75,60,95,58,75,50,83,79,66,80,65,82,65}; +15 wenn Gegnerzahl < 4: `sltiu v0,v0,0x4` @0x801007fc, `addiu v0,v0,15` @0x80100810 | 50..143 (128 @0x8010C674 + 15) |
| 0x11 | `bne v1,v0(=17)` @0x801008bc, `addiu v1,zero,250` @0x801008c8, `sh v1,342(s2)` @0x801008cc (überschreibt Wurf und +15) | **250 fest** |
| 0x13 | RE2-Zombie-Stempel wie oben (0x13 ∈ re2z_owns_type) | 50..143 |
| 0x20 | EMD0G_MOD0.BIN: `andi s1,v0,0x3` @0x80100184, `lhu v0,21424(at)` @0x801001c4 (Tabelle @0x801053B0 = {129,95,95,95,129,80,95,95,80,95,69,80,69,95,69,80}), `addu v0,v0,s1` @0x801001cc, `sh v0,342(s0)` @0x80100234 | 69..132 |
| 0x21 | EMOVL21_S0.BIN `addiu v0,zero,10` @0x80100324, `sh v0,342(s1)` @0x80100348 | 10 |
| 0x25 | EMS25.BIN `lhu v0,25396(at)` @0x80100298 (Tabelle @0x80106334 = {109,109,109,109,129,109,109,129,109,109,109,129,109,99,109,109}), `addu v0,v0,s0` @0x801002a0 (s0 = rng&3 @0x80100258), `sh v0,342(s2)` @0x80100308 | 99..132 |
| 0x26 Baby | EMS26.BIN `addiu v0,zero,1` @0x801000f0 / `sh v0,342(s0)` @0x801000f8 (bzw. −1 unverwundbar @0x80100204) | 1 |
| 0x1A Zellenarm (1210) | RE2 em2d: `+0x156 = 250` @0x80100168-6C; der DEATH-Zweig setzt **wieder 250** (`sh 250,342` @0x80101090-94) und zieht den Arm zurück (enemy_ai_re2_zellenarm.c:825) | 250, stirbt nie |

### 1.4 Schwierigkeit, Zufall, Port-Sonderskalen

* **Schwierigkeit:** Der Port hat keine Schwierigkeitswahl. Das Menü OPTIONS→AI wählt nur RE1.5, RE2 oder MIXED
  (enemy_ai_re2_zombie.c:107ff). Das RE2-Spielwort-Bit 0x40 von 0x800CFB74 setzt der System-Init bedingungslos
  (`ori v0,v0,0x40` @0x8002b4ac), und kein Schreiber löscht es (Zensus re15_damage.c ~800). Erreichbar ist also nur der
  „0x40-gesetzt“-Zweig: Zombie-Tabelle @0x8010C670, G5 600 statt 400 (enemy_ai_boss_g5.c:1869), G4 1000 @0x80100678.
* **Zufall:** RE1.5 zieht `rng&0xf` (`andi v0,v0,0xf` @0x801007d4). RE2 zieht zwei Würfe aus dem PRNG @0x80015FE8, beim Zombie
  `(r1 >> (r2&3)) & 0xf` @0x801006f0-fc. Das Maximum ist in allen Tabellen erreichbar. Die 1058 trifft bei RE1.5 ohne Import 1/16 der 0x18-Spawns.
* **Port-Sonderskalen:**
  * Gorilla ×2 = 360 (`RE15_R39_AFFEN_HP_FAKTOR` 2, re15_gegner_r39.h:64, Haken `s_an = 1` Vorgabe, gegner_r39.c:33)
  * Birkin ×2: INIT 600, Mutation 300, Revive 100 (re15_birkin_formen.h, @0x8011690c/@0x80118094/@0x8011a534 × 2)
  * Riesenform 1000 (RE2-G4-Ctor @0x80100678/7c, Nutzerentscheid F2)
  * Gator-Boss 25300, Schwäche bei hp < 25000 (`slti v0,v0,25000` @0x80101ec4)
  * G5 600 (@0x801003fc)
  * Gitterhände RE1.5 mit nachgerüsteter HP 72..99
  * Für Standard-Gegner gibt es sonst keine Skala.
* **Gegner, die nicht sterben:**
  * RE2-Zombie als Kriecher mit HP 10 (@0x80106ACC-E8, @0x801077B0). Spur GR schaltet ihn für Zeile 9 ab, eine Granate gibt also Leiche statt Kriecher (GR_granate.md, Abschnitt Zerfetzen).
  * Der RE2-Zellenarm (s. o.).

## 2. Einstufung Standard-Gegner vs. Boss

| Typ | Einstufung | Beleg |
|---|---|---|
| 0x10, 0x16 | Standard | 154 Records je Typ, 18 bzw. 6 Raumpaare |
| 0x11 | **Standard (Hinweis)** | 72 Records in 11 Raumpaaren, STAGE1 und STAGE3. Im RE2-Original ist EM011 der einmalige zähe Brad (BioModels.h:186). Im Port ist er ein regulärer Typ, darum Standard. Er stellt das Maximum. |
| 0x13 | Standard (Grenzfall) | nur 4050/4051, aber ein normaler Zombie (gemeinsame FSM, Wurzel FUN_8010a8c8). HP-Maximum 143, ohne Einfluss auf D_G |
| 0x18 | Standard | 35 Records, 7 Räume in STAGE4/5 |
| 0x20, 0x21, 0x25, 0x29, 0x2B | Standard | mehrere Räume bzw. Gruppen (Tabelle 1.1) |
| 0x2D Ivy | Standard (Grenzfall) | nur 4030/4031, 1 bzw. 2 Records. In RE2 ist die Ivy ein Standard-Gegner. 100 HP, ohne Einfluss auf D_G. Mit Feuerwaffen nicht verwundbar (Nullzeile), die Granate trifft über DAT_8006f418 |
| 0x1A Gitterhände | **Gefahrenquelle, kein Gegner im Sinne von D_G** | RE1.5 im Original unkillbar (kein +0x9a-Store). RE2-KI: DEATH setzt 250 zurück und zieht zurück (@0x80101090). Selbst als Standard gezählt bliebe D_G = 251 (250 = Brad) |
| 0x27 Gorilla | **Boss (Grenzfall)** | Nutzer wörtlich (GR_granate.md N2): „Der Gorilla ist ein Endgegner, wenn der mit einer Granate stirbt ist das etwas schwach.“ Aber: 2 Räume (1141, 11C0) mit je 2 kampfaktiven Tieren (grid 0x10). **Als Standard gezählt wäre D_G = 361** (360 = 180 @0x8011f514 × 2) |
| 0x30 Birkin (alle Formen, Riesenform) | Boss | Szenen- und Endkämpfe 3070/3071, 5080/5081, 5090/5091, 50A0, 50E0/50F1 |
| 0x36 | 3080 Szene (immun, Spieler gesperrt, GR N3.4); 5090 G5 = Boss | — |
| 0x23 Gator-Boss | Boss | Nutzer-Design-Bosskampf 2090/2091 (enemy_ai_boss_gator.c Kopf) |
| 0x24, 0x26 (RDT), 0x37, 0x40–0x4D | kein Ziel | keine HP bzw. HP −1, Gate hp < 0 (`bltz` @0x80047150) |

## 3. Maximum der Standard-Gegner und D_G

| Modus | max HP Standard | Typ | D_G = max + 1 |
|---|---|---|---|
| RE2-KI (Vorgabe) | 250 | 0x11 (@0x801008C8) | **251** |
| RE1.5-KI (Menü; Import an = Vorgabe) | 250 | 0x11 (RE2-HP über den Import) | **251** |
| MIXED (RE2 nur Hund) | 250 | 0x11 (Import) | **251** |
| nur per Umgebungsvariable: RE1.5-KI + `RE15_RE15_RE2Z_IMPORT=0`, oder `RE15_RE2_DMG_MODEL=0` | 1058 | 0x18 Spalte 3 (@0x8011f33a) | 1059 (ohne die Zelle 137, Spinne @0x80117634) |
| gesamt über die Menü-Modi | 250 | 0x11 | **251** |

Verteilung der Maxima (Menü-Modi), absteigend:

| Typ | max HP | Abstand zu D_G 251 |
|---|---|---|
| 0x11 | 250 (fest, jeder 0x11) | 1 |
| 0x1A (Gefahrenquelle, RE2) | 250 (stirbt nie) | — |
| 0x10/0x13/0x16/0x18 RE2-HP | 143 | 108 |
| 0x25 Spinne RE2 / RE1.5 | 132 / 136 | 119 / 115 |
| 0x20 Hund RE2 / RE1.5 | 132 / 111 | 119 / 140 |
| 0x2B Tyrant | 126 | 125 |
| 0x29 | 121 | 130 |
| 0x2D Ivy | 100 | 151 |
| 0x21 Krähe RE2 / RE1.5 | 10 / 0 | — |
| Bosse zum Vergleich: Gorilla 360, Birkin 600, Riesenform 1000, G5 600, Gator-Boss 25300 | | |

**Ausreißer:** 0x11 allein hebt D_G von 144 auf 251. Ohne Brad wäre der Abstand zwischen den Typen klein: 143 / 136 / 132 / 126 / 121.

Rechnung für die Bosse, falls sie D_G wie jeder andere Treffer nehmen (nicht gemessen, nur Arithmetik):

| Boss | HP danach | Pistolenschüsse danach |
|---|---|---|
| Gorilla 360 | 109 | 10 (12 @0x8006ee44) |
| Birkin 600 | 349 (Form 1 mutiert erst bei < 200) | 24 (15 @0x8006f15c) |
| Birkin in/nach der Mutation 300 | 49 | 4 |
| Riesenform 1000 | 749 | — |
| G5 600 | 349 (heute E16 = 80) | — |
| Gator-Boss 25300 | 25049 | 8 bis zur Schwäche (25049 − 7·7 = 25000 ist nicht < 25000) |

## 4. Explosionsabfall der Handgranate im Port

**Einen Abfall gibt es nicht.** Ein Wurf löst genau eine Explosion aus: Routine 31, Zünder 7, `FUN_80012d60(500,&P,2)`
@0x80018598-bc → Port `re15_granate_r35_explosion(p, gier, 2)`, re15_esp.c:1076. Jeder Kandidat erhält über
`re15_resolver_gegnerzweig(…, art, …, NULL)` (granate_r35.c:386-389) den **vollen** Schaden `re15_granate_schaden(e)`
(re15_damage.c:3222-3223). Ein Klammer- oder Abstandsfaktor wirkt dort nicht (gl = NULL). Wer im Kasten liegt, bekommt D_G, wer draußen liegt, 0.

Die Wirkungsfläche ist der RE2-Kasten von Op 47 (`addiu a2,a2,2328` @0x80020c5c → @0x80010918 `30 f8 00 00 e8 03 f4 01` =
{−2000, 0, 1000, 500}). Ihn prüft `re2gl_box_test` (FUN_80041EF8, re15_damage.c:4362) mit Ecke P + RotY(Gier)·(−2000, −2000).
Die Kanten sind 4·box[2] und 8·box[3] Einheiten lang. Dazu kommt der Zielradius: `box[2] += r1ee>>2`, `box[3] += r1ee>>2`
(@0x800471bc-ec, re15_damage.c:4704-4706). In lokalen Koordinaten zur Wurf-Gier ergibt das:
**a ∈ [−2000, 2000 + r1ee], b ∈ [−2000 − r1ee, 2000 + r1ee]**, Abstand jeweils vom Treffer-Mittelpunkt (x + Versatz).
Senkrecht gilt `0 ≤ y + b98 + 100 + h9e − P.y < 2(h9e + 100)` (@0x8004716c-a4). Geprüft wird bei P.y und bei P.y + 900
(@0x80020d98, `RE15_GRANATE_R35_EXPLOSION_DY`). Nach jedem Treffer bleibt der Puffer erweitert (keine Rücknahme
@0x800473dc-408). Spätere Kandidaten sehen also einen größeren Kasten.

Für D_G = 251 heißt das: **jeder Standard-Typ stirbt aus voller Gesundheit, solange er im Kasten liegt.**

| Typ / KI | r1ee (Beleg) | +a | ±b | −a | Ecke (+a) | Ecke (−a) | D_G tötet im Kasten? |
|---|---|---|---|---|---|---|---|
| Zombie RE2 | 500 (`addiu v1,zero,500` @0x8010096C, EMZ0) | 2500 | 2500 | 2000 | 3536 | 3202 | ja (max 250) |
| Zombie RE1.5-KI | 400 (Kasten r 400, re15_damage.c ~3918) | 2400 | 2400 | 2000 | 3394 | 3124 | ja |
| Hund RE2 | 600 (@0x80100290), Versatz +0x94 = 500 | 2600 | 2600 | 2000 | 3677 | 3280 | ja (132) |
| Hund RE1.5-KI | 500 | 2500 | 2500 | 2000 | 3536 | 3202 | ja (111) |
| Krähe RE2 / RE1.5 | 300 (@0x801003f4) / 200 | 2300 / 2200 | 2300 / 2200 | 2000 | 3253 / 3111 | 3048 / 2973 | ja (10 / 0). Hohe RE1.5-Krähe (hp −1) ist kein Ziel |
| Spinne RE2 / RE1.5 | 800 (@0x80100414) / 1000 (Kasten @0x80118dd4) | 2800 / 3000 | 2800 / 3000 | 2000 | 3960 / 4243 | 3441 / 3606 | ja (132 / 136) |
| 0x29 | 1100 | 3100 | 3100 | 2000 | 4384 | 3689 | ja (121) |
| 0x2B Tyrant | 800 (Kasten @0x8011a094) | 2800 | 2800 | 2000 | 3960 | 3441 | ja (126) |
| 0x2D Ivy | Null-Kasten → 1 (re15_damage.c:4693) | 2000 | 2000 | 2000 | 2828 | 2828 | ja (100), aber senkrechtes Band nur ±101 um y + oy (s. offen) |
| Baby 0x26 RE2 | 20 (@0x80100188) | 2020 | 2020 | 2000 | 2857 | 2843 | ja (1) |
| Gorilla (Boss) | 1600 | 3600 | 3600 | 2000 | 5091 | 4118 | nein (360 → 109) |

Außerhalb dieser Grenzen: 0 Schaden. Ausgeschlossen bleiben außerdem:
* Gegner mit hp < 0 (Gate 3)
* RE1.5-KI-Gegner mit gesetztem Ein-Treffer-Riegel +0x93 Bit 0 oder `re2_gl_sperre` (Gate 2, re15_damage.c:4681-4682)
* RE2-KI-Gegner mit +0x1D3 ≠ 0 oder +0x10E & 0xC000 (Gates 2 und 4)

## 5. Offen / Hinweise

1. **0x11 = Brad treibt D_G.** Mit 251 tötet eine Granate jeden 0x11. Gilt 0x11 nicht als Standard-Gegner (in RE2 ist er einmalig), dann wäre D_G = 144.
   Das braucht einen Entscheid. Ich habe mit Beleg auf Standard entschieden (72 Records in 11 Raumpaaren).
2. **Gorilla 0x27:** Nach dem Nutzerzitat ist er Boss. Als Standard gezählt wäre D_G = 361.
3. **Die Umgebungs-Modi** (`RE15_RE15_RE2Z_IMPORT=0`, `RE15_RE2_DMG_MODEL=0`) bringen die RE1.5-Zeile 0x18 mit 1058 (@0x8011f33a). Ein solcher
   0x18 überlebt D_G = 251. Die Zelle ist Original (40%-Disc bytegleich), aber im Menü nicht erreichbar.
4. **Ivy 0x2D hat keinen Trefferkasten.** Der Kandidatentest nimmt deshalb 1/1/1 (re15_damage.c:4693). Das senkrechte Band wird dadurch ±101 um
   y + oy schmal. Ob die Explosion bei P.y bzw. P.y + 900 die Ivy überhaupt erreicht, ist **nicht gemessen**.
5. **Eine Folge der Lesung, nicht mein Auftrag:** Der G5 im Endkampf bekommt heute für Art 2 fest 80 (E16-Block,
   re15_damage.c:3247-3249). Gilt „Bosse nehmen D_G“, muss auch dieser Block weg.
6. **Fehltreffer im Zensus:** Die Records 0x00/0x04 sind Fehltreffer des Walkers in Meldungsdaten (11A0, 2030, 20A0). Spawns, die der Port selbst
   setzt (Elza-Zombies, 1150, 2080), zählt der RDT-Zensus nicht. Sie haben nur die Typen 0x10/0x16 (HP-Regeln wie oben).
7. **Port-Fund, ohne Einfluss auf D_G:** Unter der RE1.5-KI (Import aus) hat die Zombie-Live-Init `z_hp` keine Zeile für 0x13
   (enemy_ai_common.c:2106-2118). 0x13 läuft aber über die eigene Mädchen-Wurzel mit (rng&0x1f)+50 @0x8010ac0c.
   Das ist die richtige Quelle, die Lücke ist nur kosmetisch. Die Original-Zeile @0x8011f294 (61..88) wird für 0x13 nie gelesen.
