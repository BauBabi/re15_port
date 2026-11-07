# WS Leser A — RE2-Schadenstabellen (Rakete, Granatwerfer) und Schadenswege im Port

Runde 41, Spur WS (Waffenschaden), 2026-11-07. NUR GELESEN: Integrationsbaum
`C:/workspace/git/reAi_v2/.claude/worktrees/r41_integration` (Zweig r41/integration), RE2-EXE
`C:/workspace/git/reAi_v2/info/re2leon/PSX.EXE` (bytegleich mit `SLUS_007.48`, `cmp` ohne Ausgabe), RE1.5-EXE
`C:/workspace/git/reAi_v2/info/Re1.5/PSX.EXE`, `C:/workspace/git/reAi_v2/info/Re1.5/PSX/BIN/STAGE1.BIN`.
Nichts geaendert, nichts gebaut, kein Test gestartet.

Werkzeuge (eigene, nur lesend):
- `C:/workspace/git/reAi_v2/.claude/skills/re15-psx-disasm/scripts/re2_disasm.py` (dis/read auf der RE2-EXE)
- `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/ws/werkzeuge/re2_tab.py` (Zeiger + Zeilen 9/10/11/16/17 je Typ)
- `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/ws/werkzeuge/re2_tab_md.py` (erzeugt Tabelle 1.4)
- `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/ws/werkzeuge/re15_tab.py` (RE1.5-Typzeilen @0x8006e0d0, DAT_8006f418/430)

---

## 1. RE2 Retail (Leon): Schadensrechnung und Tabellen

### 1.1 Der Applier FUN_800470C0 — so wird der Schaden angewendet (selbst disassembliert)

Ein einziger Applier fuer alle Projektil-/Explosionstreffer. Argumente a0 = &P, a1 = Gier, a2 = Box, a3 = Hitcode.

| Schritt | Instruktion(en) | Wirkung |
|---|---|---|
| Klammer K | `srl s6,s5,28` @0x80047114 | K = Hitcode >> 28 (0/1/2) |
| ALLE oder ERSTER | `lui v0,0x1` / `and v0,s5,v0` / `beq v0,zero,0x80047434` @0x80047208-10 | Bit 0x10000 = alle Kandidaten in der Box, sonst nur der erste (Einzelzweig ab 0x80047434) |
| Zeile | `andi v1,s5,0xffff` @0x80047214 | Zeile = Hitcode & 0xFFFF |
| Typzeiger | `lbu v0,8(s1)` @0x80047218 / `sll v0,v0,2` / `lui at,0x800a` / `lw a1,27272(at)` @0x8004722c | Tabelle = *(0x800A6A88 + Typ*4) |
| Datensatz | `sll/addu/sll` / `addiu v0,v0,-20` @0x8004723c / `addu a1,a1,v0` | Satz = Tabelle + (Zeile-1)*20 (5 Woerter je Zeile, 19 Zeilen je Typ = 380 Byte) |
| Schaden | `lw v1,0(a1)` @0x8004724c / `sll v0,v0,1` (10*K) / `srlv v1,v1,v0` @0x80047254 / `andi v1,v1,0x3ff` @0x8004725c | dmg = (w0 >> 10*K) & 0x3FF |
| HP | `lhu v0,342(s1)` @0x80047258 / `subu v0,v0,v1` @0x80047260 / `sh v0,342(s1)` @0x80047268 | +0x156 -= dmg |
| Tod | `lh v1,342(s1)` @0x8004727c / `bgez v1,0x80047294` @0x80047284 | Zustand 3 nur bei hp < 0; 0 HP lebt |
| Einzelzweig | `lw a1,27272(at)` @0x80047450 / `srlv` @0x80047478 / `andi 0x3ff` @0x80047480 / `sh v0,342(s1)` @0x80047488 / `bgez v1,0x800474b8` @0x800474a8 | dieselbe Rechnung |
| Sperre | (w1 >> 9) & 0x7F nach +0x1D3 (@0x8004731c-4c, Port-Beleg re15_damage.c:4152 und :4442) | Gate 2 `lbu v0,467(s0)` / `bne` @0x80047138-40: gesperrter Gegner ist kein Kandidat |
| Kandidat | `lh v0,342(s0)` / `bltz` @0x80047148-50 | hp < 0 ist kein Ziel, hp = 0 bleibt Ziel |

**Was NICHT im Applier steht** (gelesen @0x80047208-0x8004727c und Einzelzweig @0x80047434-0x800474b4):
- **keine Schwierigkeit** (kein Lesen von 0x800CFB74 im Schadenspfad; die Schwierigkeit wirkt in RE2 nur ueber
  die INIT-HP der EM-Module, z. B. G4 1000/900/1100 @0x80100678/@0x80100690/@0x8010069c, Beleg GR_granate.md N3.1;
  G5 600 / leicht 400 @0x801003fc-418, Beleg enemy_ai_boss_g5.c:1869),
- **kein Zufall** (kein jal auf den RNG im Schadenspfad),
- **kein Abstandsabfall**: Treffer oder kein Treffer ist der Box-Test FUN_80041EF8; der Schaden ist die Klammer K
  des Hitcodes, unabhaengig vom Abstand.
- **Trefferzone**: die Zone (Bit 0x20000, Kopf-Bit) bestimmt nur +0x1D2 (Spalte fuer Todes-/Trefferanimation),
  nicht den Schaden. Die Klammer K kommt allein aus dem Hitcode.

### 1.2 Die Rakete in RE2 (Op 24 / Op 47 Sub 13)

| Stelle | Instruktion(en) | Bedeutung |
|---|---|---|
| Flug Op 24 = FUN_8001F6E0, Box | `read 0x80010908` = {-800, 0, 400, 200} | Flugbox der Rakete |
| Gegnerkontakt | `lui a3,0x3` @0x8001f7b0 / `jal 0x800470c0` @0x8001f7bc / `ori a3,a3,0x11` @0x8001f7c0 (Delay) | Hitcode **0x00030011**: Zeile 17, K = 0, Bit 0x10000 = ALLE in der Box, Bit 0x20000 = Zonenregel |
| Bodenkontakt | `lui a3,0x2` @0x8001f820 / `ori a3,a3,0x11` @0x8001f82c / `jal 0x800470c0` @0x8001f83c | Hitcode **0x00020011**: Zeile 17, K = 0, nur der ERSTE |
| Explosion Op 47 = FUN_80020C3C | `addiu v0,zero,12` / `bne v1,v0,0x80020dc0` @0x80020d34-38 | nur Sub 12 (GL-Explosivrunde) ruft den Applier (`jal 0x800470c0` @0x80020d78 und @0x80020db0); Sub 13 (Rakete) springt nach 0x80020dc0: nur SE `lui a0,0x114` @0x80020d3c / `jal 0x8005ba28` @0x80020dc4 und Kinder `jal 0x8001cbe8` @0x80020e1c/40/60 — **kein Flaechenschaden** |

**Folge:** Der Raketenschaden ist in RE2 genau **Zeile 17, Klammer 0** des Typ-Datensatzes, einmal je Gegner, der im
Kontaktbild in der Flugbox steht (Box um +0x1EE >> 2 je Kandidat erweitert, @0x800471bc-ec). Kein Explosionsradius, kein
Abfall, keine Schwierigkeit, kein Zufall. In **allen** 48 Typen 0x10..0x3F sind die drei Klammern der Zeile 17 gleich
(Tabelle 1.4) — selbst ein anderer K-Wert aenderte nichts. Sperre nach dem Treffer (w1 >> 9) & 0x7F = 15 Bilder in
jeder Zeile 17 (Tabelle 1.4).

### 1.3 Granatwerfer (Vergleich): Hitcodes je Munition

| Munition | Zeile | Flug (Direkttreffer) | Aufschlag | Belege |
|---|---|---|---|---|
| Spreng | 9 | 0x30009 + Art = K0, ALLE (Op 15 FUN_8001ED9C, `ori s2,s2,0x9` + `lb a3,27` @0x8001ee90-dc; Port re2_fx.c:1316) | **0x10020009 = K1**, ALLE, Box {-2000,0,1000,500} (`read 0x80010918`) an P und P+900 (`jal 0x800470c0` @0x80020d78 / @0x80020db0) | Spalte K1 = Splitterschaden |
| Brand | 10 | 0x3000A = K0 (gleicher Flug-Op) | **0x0002000A = K0**, ALLE, an P und P+1800: `lui a3,0x2` @0x80021058 / `ori a3,a3,0xa` @0x80021064 / `jal` @0x80021060, zweiter Aufruf @0x8002108c (`addiu v0,v0,1800` @0x80021080); Nachbrenner Op 40 0x2002000A = K2 (re2_fx.c:682) | Op 48 FUN_80020F3C |
| Saeure | 11 | 0x3000B = K0 | **0x1002000B = K1**, ALLE, an P und P+1800: `lui a3,0x1002` @0x800216e4 / `ori a3,a3,0xb` @0x800216f0 / `jal` @0x800216ec, zweiter @0x80021718 (`addiu v0,v0,1800` @0x8002170c) | Op 49 FUN_800215C8 |

Beispiel Zombie (Tabelle 0x800A412C): Spreng direkt 200 / Splitter 50; Brand 200 (+ Nachbrenner 5); Saeure direkt 200 /
Aufschlag 50. Hund: Spreng 300/50, Brand 300, Saeure 300/50.

RE2 hat **keine Handgranate** (BAUPLAN Runde 34 E1, Musterscan 0 Treffer, zitiert re15_damage.c:3121); fuer die Handgranate
gibt es in RE2 deshalb keinen eigenen Wert.

### 1.4 RE2-Tabelle je Typ (Zeiger *(0x800A6A88 + Typ*4); Zeilen 9/10/11/17, Bytes aus info/re2leon/PSX.EXE)

Namen nach `C:/workspace/git/reAi_v2/info/Resident_Evil_und_Playstation_Information/BioModels-master/src/BioModels.h:166-204`
(EM0xx.EMD = Typ). Typen ohne eigenes EMD zeigen in RE2 auf die Zombie-Tabelle 0x800A412C (Vorgabezeiger).

| RE2-Typ | Name (BioModels.h) | Zeiger @ -> Tabelle | Rakete Z17 w0 @Adresse = K0/K1/K2 | Sperre | Spreng Z9 K0/K1/K2 | Brand Z10 K0/K1/K2 | Saeure Z11 K0/K1/K2 |
|---|---|---|---|---|---|---|---|
| 0x10 | Zombie Polizist (EM010) | @0x800a6ac8 -> 0x800a412c | **900** (`384e1384` @0x800a426c) = 900/900/900 | 15 | 200/50/10 (`00a0c8c8` @0x800a41cc) | 200/50/5 (`0050c8c8` @0x800a41e0) | 200/50/10 (`00a0c8c8` @0x800a41f4) |
| 0x11 | Brad Vickers (EM011) | @0x800a6acc -> 0x800a412c | **900** (`384e1384` @0x800a426c) = 900/900/900 | 15 | 200/50/10 (`00a0c8c8` @0x800a41cc) | 200/50/5 (`0050c8c8` @0x800a41e0) | 200/50/10 (`00a0c8c8` @0x800a41f4) |
| 0x12 | Zombie 2 (EM012) | @0x800a6ad0 -> 0x800a412c | **900** (`384e1384` @0x800a426c) = 900/900/900 | 15 | 200/50/10 (`00a0c8c8` @0x800a41cc) | 200/50/5 (`0050c8c8` @0x800a41e0) | 200/50/10 (`00a0c8c8` @0x800a41f4) |
| 0x13 | Zombie-Frau (EM013) | @0x800a6ad4 -> 0x800a412c | **900** (`384e1384` @0x800a426c) = 900/900/900 | 15 | 200/50/10 (`00a0c8c8` @0x800a41cc) | 200/50/5 (`0050c8c8` @0x800a41e0) | 200/50/10 (`00a0c8c8` @0x800a41f4) |
| 0x14 | (kein EMD) | @0x800a6ad8 -> 0x800a412c | **900** (`384e1384` @0x800a426c) = 900/900/900 | 15 | 200/50/10 (`00a0c8c8` @0x800a41cc) | 200/50/5 (`0050c8c8` @0x800a41e0) | 200/50/10 (`00a0c8c8` @0x800a41f4) |
| 0x15 | Zombie Patient (EM015) | @0x800a6adc -> 0x800a42a8 | **900** (`384e1384` @0x800a43e8) = 900/900/900 | 15 | 80/50/10 (`00a0c850` @0x800a4348) | 80/50/5 (`0050c850` @0x800a435c) | 200/50/10 (`00a0c8c8` @0x800a4370) |
| 0x16 | Zombie Wissenschaftler (EM016) | @0x800a6ae0 -> 0x800a42a8 | **900** (`384e1384` @0x800a43e8) = 900/900/900 | 15 | 80/50/10 (`00a0c850` @0x800a4348) | 80/50/5 (`0050c850` @0x800a435c) | 200/50/10 (`00a0c8c8` @0x800a4370) |
| 0x17 | Zombie enthaeutet (EM017) | @0x800a6ae4 -> 0x800a42a8 | **900** (`384e1384` @0x800a43e8) = 900/900/900 | 15 | 80/50/10 (`00a0c850` @0x800a4348) | 80/50/5 (`0050c850` @0x800a435c) | 200/50/10 (`00a0c8c8` @0x800a4370) |
| 0x18 | Zombie 1 Dupl. (EM018) | @0x800a6ae8 -> 0x800a412c | **900** (`384e1384` @0x800a426c) = 900/900/900 | 15 | 200/50/10 (`00a0c8c8` @0x800a41cc) | 200/50/5 (`0050c8c8` @0x800a41e0) | 200/50/10 (`00a0c8c8` @0x800a41f4) |
| 0x19 | (kein EMD) | @0x800a6aec -> 0x800a412c | **900** (`384e1384` @0x800a426c) = 900/900/900 | 15 | 200/50/10 (`00a0c8c8` @0x800a41cc) | 200/50/5 (`0050c8c8` @0x800a41e0) | 200/50/10 (`00a0c8c8` @0x800a41f4) |
| 0x1A | (kein EMD) | @0x800a6af0 -> 0x800a412c | **900** (`384e1384` @0x800a426c) = 900/900/900 | 15 | 200/50/10 (`00a0c8c8` @0x800a41cc) | 200/50/5 (`0050c8c8` @0x800a41e0) | 200/50/10 (`00a0c8c8` @0x800a41f4) |
| 0x1B | (kein EMD) | @0x800a6af4 -> 0x800a412c | **900** (`384e1384` @0x800a426c) = 900/900/900 | 15 | 200/50/10 (`00a0c8c8` @0x800a41cc) | 200/50/5 (`0050c8c8` @0x800a41e0) | 200/50/10 (`00a0c8c8` @0x800a41f4) |
| 0x1C | (kein EMD) | @0x800a6af8 -> 0x800a412c | **900** (`384e1384` @0x800a426c) = 900/900/900 | 15 | 200/50/10 (`00a0c8c8` @0x800a41cc) | 200/50/5 (`0050c8c8` @0x800a41e0) | 200/50/10 (`00a0c8c8` @0x800a41f4) |
| 0x1D | (kein EMD) | @0x800a6afc -> 0x800a412c | **900** (`384e1384` @0x800a426c) = 900/900/900 | 15 | 200/50/10 (`00a0c8c8` @0x800a41cc) | 200/50/5 (`0050c8c8` @0x800a41e0) | 200/50/10 (`00a0c8c8` @0x800a41f4) |
| 0x1E | Zombie 1 (EM01E) | @0x800a6b00 -> 0x800a412c | **900** (`384e1384` @0x800a426c) = 900/900/900 | 15 | 200/50/10 (`00a0c8c8` @0x800a41cc) | 200/50/5 (`0050c8c8` @0x800a41e0) | 200/50/10 (`00a0c8c8` @0x800a41f4) |
| 0x1F | Zombie 1 Dupl. (EM01F) | @0x800a6b04 -> 0x800a412c | **900** (`384e1384` @0x800a426c) = 900/900/900 | 15 | 200/50/10 (`00a0c8c8` @0x800a41cc) | 200/50/5 (`0050c8c8` @0x800a41e0) | 200/50/10 (`00a0c8c8` @0x800a41f4) |
| 0x20 | Hund (EM020) | @0x800a6b08 -> 0x800a4424 | **300** (`12c4b12c` @0x800a4564) = 300/300/300 | 15 | 300/50/10 (`00a0c92c` @0x800a44c4) | 300/50/5 (`0050c92c` @0x800a44d8) | 300/50/10 (`00a0c92c` @0x800a44ec) |
| 0x21 | Kraehe (EM021) | @0x800a6b0c -> 0x800a45a0 | **300** (`12c4b12c` @0x800a46e0) = 300/300/300 | 15 | 60/60/15 (`00f0f03c` @0x800a4640) | 60/60/15 (`00f0f03c` @0x800a4654) | 60/60/15 (`00f0f03c` @0x800a4668) |
| 0x22 | Licker rot (EM022) | @0x800a6b10 -> 0x800a471c | **200** (`0c8320c8` @0x800a485c) = 200/200/200 | 15 | 60/60/20 (`0140f03c` @0x800a47bc) | 85/60/5 (`0050f055` @0x800a47d0) | 130/60/10 (`00a0f082` @0x800a47e4) |
| 0x23 | Alligator (EM023) | @0x800a6b14 -> 0x800a4a14 | **110** (`06e1b86e` @0x800a4b54) = 110/110/110 | 15 | 30/30/30 (`01e0781e` @0x800a4ab4) | 31/31/5 (`00507c1f` @0x800a4ac8) | 25/25/25 (`01906419` @0x800a4adc) |
| 0x24 | Licker grau (EM024) | @0x800a6b18 -> 0x800a4898 | **200** (`0c8320c8` @0x800a49d8) = 200/200/200 | 15 | 45/45/10 (`00a0b42d` @0x800a4938) | 50/30/5 (`00507832` @0x800a494c) | 65/65/15 (`00f10441` @0x800a4960) |
| 0x25 | Spinne (EM025) | @0x800a6b1c -> 0x800a4b90 | **200** (`0c8320c8` @0x800a4cd0) = 200/200/200 | 15 | 60/60/20 (`0140f03c` @0x800a4c30) | 130/130/5 (`00520882` @0x800a4c44) | 90/60/10 (`00a0f05a` @0x800a4c58) |
| 0x26 | Babyspinne (EM026) | @0x800a6b20 -> 0x800a4b90 | **200** (`0c8320c8` @0x800a4cd0) = 200/200/200 | 15 | 60/60/20 (`0140f03c` @0x800a4c30) | 130/130/5 (`00520882` @0x800a4c44) | 90/60/10 (`00a0f05a` @0x800a4c58) |
| 0x27 | Maden-Baby/G-Embryo (EM027) | @0x800a6b24 -> 0x800a4d0c | **201** (`0c9324c9` @0x800a4e4c) = 201/201/201 | 15 | 60/60/60 (`03c0f03c` @0x800a4dac) | 80/80/5 (`00514050` @0x800a4dc0) | 121/121/10 (`00a1e479` @0x800a4dd4) |
| 0x28 | Made/G-Adult (EM028) | @0x800a6b28 -> 0x800a4e88 | **201** (`0c9324c9` @0x800a4fc8) = 201/201/201 | 15 | 60/60/60 (`03c0f03c` @0x800a4f28) | 80/80/5 (`00514050` @0x800a4f3c) | 121/121/10 (`00a1e479` @0x800a4f50) |
| 0x29 | Kakerlake (EM029) | @0x800a6b2c -> 0x800a5004 | **300** (`12c4b12c` @0x800a5144) = 300/300/300 | 15 | 60/60/15 (`00f0f03c` @0x800a50a4) | 60/60/10 (`00a0f03c` @0x800a50b8) | 60/60/10 (`00a0f03c` @0x800a50cc) |
| 0x2A | Mr. X (EM02A) | @0x800a6b30 -> 0x800a52fc | **500** (`1f47d1f4` @0x800a543c) = 500/500/500 | 15 | 50/50/50 (`0320c832` @0x800a539c) | 60/60/5 (`0050f03c` @0x800a53b0) | 81/81/10 (`00a14451` @0x800a53c4) |
| 0x2B | Tyrant (EM02B) | @0x800a6b34 -> 0x800a5478 | **500** (`1f47d1f4` @0x800a55b8) = 500/500/500 | 15 | 50/50/50 (`0320c832` @0x800a5518) | 60/60/5 (`0050f03c` @0x800a552c) | 101/101/10 (`00a19465` @0x800a5540) |
| 0x2C | Tyrant-Arm (EM02C) | @0x800a6b38 -> 0x800a412c | **900** (`384e1384` @0x800a426c) = 900/900/900 | 15 | 200/50/10 (`00a0c8c8` @0x800a41cc) | 200/50/5 (`0050c8c8` @0x800a41e0) | 200/50/10 (`00a0c8c8` @0x800a41f4) |
| 0x2D | Zombie-Arme (EM02D) | @0x800a6b3c -> 0x800a5180 | **40** (`0280a028` @0x800a52c0) = 40/40/40 | 15 | 60/60/20 (`0140f03c` @0x800a5220) | 60/60/10 (`00a0f03c` @0x800a5234) | 60/60/10 (`00a0f03c` @0x800a5248) |
| 0x2E | Ivy (EM02E) | @0x800a6b40 -> 0x800a55f4 | **300** (`12c4b12c` @0x800a5734) = 300/300/300 | 15 | 30/30/30 (`01e0781e` @0x800a5694) | 70/30/5 (`00507846` @0x800a56a8) | 40/40/10 (`00a0a028` @0x800a56bc) |
| 0x2F | Ivy-Arm (EM02F) | @0x800a6b44 -> 0x800a55f4 | **300** (`12c4b12c` @0x800a5734) = 300/300/300 | 15 | 30/30/30 (`01e0781e` @0x800a5694) | 70/30/5 (`00507846` @0x800a56a8) | 40/40/10 (`00a0a028` @0x800a56bc) |
| 0x30 | Birkin 1 / G1 (EM030) | @0x800a6b48 -> 0x800a5770 | **200** (`0c8320c8` @0x800a58b0) = 200/200/200 | 15 | 60/60/60 (`03c0f03c` @0x800a5810) | 70/70/5 (`00511846` @0x800a5824) | 101/101/10 (`00a19465` @0x800a5838) |
| 0x31 | Birkin 2 / G2 (EM031) | @0x800a6b4c -> 0x800a58ec | **300** (`12c4b12c` @0x800a5a2c) = 300/300/300 | 15 | 60/60/60 (`03c0f03c` @0x800a598c) | 100/100/5 (`00519064` @0x800a59a0) | 80/80/10 (`00a14050` @0x800a59b4) |
| 0x32 | (kein EMD; G1-Tabelle) | @0x800a6b50 -> 0x800a5770 | **200** (`0c8320c8` @0x800a58b0) = 200/200/200 | 15 | 60/60/60 (`03c0f03c` @0x800a5810) | 70/70/5 (`00511846` @0x800a5824) | 101/101/10 (`00a19465` @0x800a5838) |
| 0x33 | Birkin 3 / G3 (EM033) | @0x800a6b54 -> 0x800a5a68 | **400** (`19064190` @0x800a5ba8) = 400/400/400 | 15 | 60/60/60 (`03c0f03c` @0x800a5b08) | 101/101/5 (`00519465` @0x800a5b1c) | 80/80/10 (`00a14050` @0x800a5b30) |
| 0x34 | Birkin 4 / G4 (EM034) | @0x800a6b58 -> 0x800a5be4 | **301** (`12d4b52d` @0x800a5d24) = 301/301/301 | 15 | 90/90/90 (`05a1685a` @0x800a5c84) | 70/70/5 (`00511846` @0x800a5c98) | 60/60/10 (`00a0f03c` @0x800a5cac) |
| 0x35 | (kein EMD; G1-Tabelle) | @0x800a6b5c -> 0x800a5770 | **200** (`0c8320c8` @0x800a58b0) = 200/200/200 | 15 | 60/60/60 (`03c0f03c` @0x800a5810) | 70/70/5 (`00511846` @0x800a5824) | 101/101/10 (`00a19465` @0x800a5838) |
| 0x36 | Birkin 5 / G5 (EM036) | @0x800a6b60 -> 0x800a5edc | **200** (`0c8320c8` @0x800a601c) = 200/200/200 | 15 | 80/80/80 (`05014050` @0x800a5f7c) | 70/70/5 (`00511846` @0x800a5f90) | 70/70/10 (`00a11846` @0x800a5fa4) |
| 0x37 | Birkin-Projektil 1 (EM037) | @0x800a6b64 -> 0x800a412c | **900** (`384e1384` @0x800a426c) = 900/900/900 | 15 | 200/50/10 (`00a0c8c8` @0x800a41cc) | 200/50/5 (`0050c8c8` @0x800a41e0) | 200/50/10 (`00a0c8c8` @0x800a41f4) |
| 0x38 | Birkin-Projektil 2 (EM038) | @0x800a6b68 -> 0x800a412c | **900** (`384e1384` @0x800a426c) = 900/900/900 | 15 | 200/50/10 (`00a0c8c8` @0x800a41cc) | 200/50/5 (`0050c8c8` @0x800a41e0) | 200/50/10 (`00a0c8c8` @0x800a41f4) |
| 0x39 | Ivy lila (EM039) | @0x800a6b6c -> 0x800a55f4 | **300** (`12c4b12c` @0x800a5734) = 300/300/300 | 15 | 30/30/30 (`01e0781e` @0x800a5694) | 70/30/5 (`00507846` @0x800a56a8) | 40/40/10 (`00a0a028` @0x800a56bc) |
| 0x3A | Riesenmotte (EM03A) | @0x800a6b70 -> 0x800a6058 | **300** (`12c4b12c` @0x800a6198) = 300/300/300 | 15 | 45/45/45 (`02d0b42d` @0x800a60f8) | 80/40/5 (`0050a050` @0x800a610c) | 50/50/10 (`00a0c832` @0x800a6120) |
| 0x3B | Larve (EM03B) | @0x800a6b74 -> 0x800a61d4 | **300** (`12c4b12c` @0x800a6314) = 300/300/300 | 15 | 40/40/40 (`0280a028` @0x800a6274) | 40/40/10 (`00a0a028` @0x800a6288) | 40/40/10 (`00a0a028` @0x800a629c) |
| 0x3C | (kein EMD) | @0x800a6b78 -> 0x800a412c | **900** (`384e1384` @0x800a426c) = 900/900/900 | 15 | 200/50/10 (`00a0c8c8` @0x800a41cc) | 200/50/5 (`0050c8c8` @0x800a41e0) | 200/50/10 (`00a0c8c8` @0x800a41f4) |
| 0x3D | (kein EMD) | @0x800a6b7c -> 0x800a412c | **900** (`384e1384` @0x800a426c) = 900/900/900 | 15 | 200/50/10 (`00a0c8c8` @0x800a41cc) | 200/50/5 (`0050c8c8` @0x800a41e0) | 200/50/10 (`00a0c8c8` @0x800a41f4) |
| 0x3E | Labor-Greifarm (EM03E) | @0x800a6b80 -> 0x800a412c | **900** (`384e1384` @0x800a426c) = 900/900/900 | 15 | 200/50/10 (`00a0c8c8` @0x800a41cc) | 200/50/5 (`0050c8c8` @0x800a41e0) | 200/50/10 (`00a0c8c8` @0x800a41f4) |
| 0x3F | Labor-Geraet (EM03F) | @0x800a6b84 -> 0x800a412c | **900** (`384e1384` @0x800a426c) = 900/900/900 | 15 | 200/50/10 (`00a0c8c8` @0x800a41cc) | 200/50/5 (`0050c8c8` @0x800a41e0) | 200/50/10 (`00a0c8c8` @0x800a41f4) |

Zusammenfassung Rakete (Zeile 17, K0) über alle RE2-Typen: **900** Zombie-Familie und Vorgabezeiger
(0x10-0x1F, 0x2C, 0x37, 0x38, 0x3C-0x3F) | **500** Mr. X / Tyrant (0x2A, 0x2B) | **400** G3 (0x33) | **301** G4 (0x34)
| **300** Hund, Krähe, Kakerlake, G2, Ivy/Ivy-Arm/Ivy lila, Motte, Larve (0x20, 0x21, 0x29, 0x31, 0x2E, 0x2F, 0x39, 0x3A,
0x3B) | **201** G-Embryo/G-Adult (0x27, 0x28) | **200** Licker rot/grau, Spinne/Baby, G1-Tabelle, G5 (0x22, 0x24, 0x25, 0x26,
0x30, 0x32, 0x35, 0x36) | **110** Alligator (0x23) | **40** Zombie-Arme (0x2D).

### 1.5 Nebenbefund RE1.5 (Kontext, nicht das Vorbild)

RE1.5 hat eine Raketen-SPALTE in der Typzeile (@0x8006e0d0 + Typ*0x58 + 18*4): **400** für 0x10-0x13/0x16/0x18/0x1A
(z. B. @0x8006e698), 0x20 (@0x8006ec18), 0x22, 0x23 (@0x8006ed20), 0x25, 0x27 (@0x8006ee80), 0x29, 0x2B, 0x30 (@0x8006f198);
**2** Krähe 0x21 (@0x8006ec70); **0** für 0x24, 0x26, 0x2D, 0x36 (@0x8006f3a8). Sie hat aber keinen Schützen: die
Entlade-Handler 15..18 sind NULL (@0x8007413c-4b, Beleg include/re15_werfer.h:7). Die Explosionsart 9 (DAT_8006f418[9] = 1000,
`e8 03` @0x8006f42a) hat ebenfalls keinen Aufrufer. Beides ist unfertig, also gilt RE2 (Beta -> Retail), wie vom Orchestrator gelesen.
DAT_8006f418 gelesen: [10, 20, 1000, 1000, 1000, 50, 100, 200, 300, 1000, 0]; DAT_8006f430: [3, 3, 9, 10, 11, 14, 15, 16, 17, 18, 20].

---

## 2. Port heute: wo Raketen- und Handgranatenschaden entsteht (Integrationsbaum)

Alle Pfade relativ zu `C:/workspace/git/reAi_v2/.claude/worktrees/r41_integration/re15_port/`.

### 2.1 Rakete (Waffe 18)

1. **Schuss:** RE1.5 hat für 18 keinen Entlade-Handler; der Port spawnt den RE2-Effekt-Handler [17] im Rückstoßbild 1
   (`engine/src/werfer_r35.c:8`, :165-190; RE2 @0x8004559c-a4).
2. **Flug:** `engine/src/re2_fx.c:1338` `op_24` = RE2 FUN_8001F6E0. Box {-800,0,400,200}, Applier mit **0x30011**
   (re2_fx.c:1349) und **0x20011** (re2_fx.c:1359). Kein Flächenschaden (Op 47 Sub 13 nur SE, re2_fx.c:1369-1395).
3. **Applier:** `engine/src/re15_damage.c:4501` `re15_re2_gl_apply` (Port von FUN_800470C0). Zwei Wege je Kandidat:
   - **RE2-Kandidat** (`re2_gl_typ`: Zombie-Familie 0x10/0x11/0x12/0x13/0x16/0x18 mit RE2-Gehirn, Hund 0x20, Krähe 0x21,
     Spinne 0x25, RE2-Baby 0x26, Zellenarm 0x1A, G5 0x36 nur in 5090/5091): Zeile 17 aus `s_re2gl_rec16_*`
     (re15_damage.c:4233-4239, `re2_gl_rec16_17` :4240), dmg = (w0 >> 10K) & 0x3FF (Zweig `zeile == 16u || zeile == 17u`
     im Applier): Zombie 900 (`384e1384` @0x800A426C), 0x16 900 (@0x800A43E8), Hund 300 (@0x800A4564), Krähe 300 (@0x800A46E0),
     Spinne/Baby 200 (@0x800A4CD0), Arm 40 (@0x800A52C0), G5 200 (@0x800A601C). **= RE2-Wert.**
   - **RE1.5-Kandidat** (alle anderen): Zeile 17 -> Art 9 (re15_damage.c:4603), `re15_resolver_gegnerzweig(e, 9, p, &gl)`
     (re15_damage.c:4605 -> :3669 -> `re15_enemy_take_damage_at` :3174): dmg = `re15_damage_table[9]` = **1000**
     (DAT_8006f418[9] @0x8006f42a, Tabelle re15_damage.c:63). Nur wenn der Typ unter dem RE2-Modell steht (`re15_e4_modell`,
     re15_damage.c:3105-3110: RE2-Modell oder Import-Zombie), wird daraus die RE2-Zeile (:3209 `re15_enemy_dmg_row(e)[18]`,
     dann :3239 Datensatz Zeile 17) = 900 für Import-Zombies.
   - Ergebnis heute: **1000** für Birkin 0x30 (alle Formen, auch Riesenform und Hangar), Gorilla 0x27, kleiner Affe 0x29,
     Tyrant 0x2B, Ivy 0x2D, Alligator 0x23 (RE1.5-KI und Gator-Boss 2090), RE1.5-KI-Hund/-Krähe/-Spinne, 0x36 außerhalb
     5090, G5-Tentakel 0x37 (falls Kandidat, s. offen).
4. **Bosse, eigene Nachbehandlung:**
   - Gator-Boss `engine/src/enemy_ai_boss_gator.c:794` `gb_absorb_hit`: übernimmt den Schaden aus re15_enemy_take_damage;
     in den Phasen GBP_FLINCH / GBP_RE2_TOD wird er zurückgenommen (`e->hp = g->prev_hp`, :801-802; RE2 +0x1D3 |= 0x80
     @0x80102c20-28 / @0x801022ec-f4). Schwäche bei hp < 25000 (`RE15_GR2_HP_SOCKEL`, include/re15_gator_re2_r41.h:27,
     `slti v0,v0,25000` @0x80101ec4), Start 25300 (:28). Heute schwächt eine Rakete ihn sofort: 25300 -> 24300.
   - Riesenform: `engine/src/birkin_bahnsteig.c:601` `e->hp = RE15_BB_FORM_HP` (1000, include/re15_birkin_formen.h:73,
     G4-Ctor `addiu v0,zero,1000` @0x80100678). Heute bleibt sie nach einer Rakete mit 0 HP am Leben (gemessen,
     Gegenprüfung Punkt 5: F592 1000 -> F604 0, Zustand 2).
   - G5 5090: RE2-Kandidat -> 200 (s. o.); HP 600 (`enemy_ai_boss_g5.c:1869`, @0x801003fc).

### 2.2 Handgranate (Waffe 9, Explosionsart 2)

1. **Zünder/Explosion:** Routine 31 bei Zünder 7, `engine/src/re15_esp.c:1076` -> `engine/src/granate_r35.c:363`
   `re15_granate_r35_explosion`. Sie ersetzt RE1.5 FUN_80012d60(500,&P,2) @0x80018598-bc durch die RE2-Reichweite Op 47.
   Der Flugkontakt (granate_r35.c:243-255) löst nur aus und macht keinen Schaden.
2. **Reichweite (Zentrum/Rand):** Box `RE15_GRANATE_R35_BOX_EXPLOSION` {-2000,0,1000,500} (include/re15_granate_r35.h:38, RE2
   @0x80010918), geprüft an P und P + 900 (`RE15_GRANATE_R35_EXPLOSION_DY`, :41, RE2 `addiu v0,v0,900` @0x80020d98). Je
   Kandidat laufen Band- und Boxtest `re15_re2_gl_kandidat` (Band @0x8004716c-a4, Radius +0x1EE>>2 @0x800471bc-ec,
   Box FUN_80041EF8); jeder Slot zählt höchstens einmal. **Es gibt keinen Abfall: jeder Kandidat in der Box bekommt denselben
   vollen Schaden, außerhalb 0.** Der Port rechnet nirgends Zentrum gegen Rand, und RE2 Op 47 tut es auch nicht (dort K1 für alle).
3. **Anwendung:** `re15_resolver_gegnerzweig(e, 2, P, NULL)` (granate_r35.c:389) -> `re15_enemy_take_damage_at`
   (re15_damage.c:3174). Es gelten Gate B (`(hit_react & 3) == 3`, :3683, @0x80012f54-60) und der Ein-Treffer-Riegel
   (`hit_react & 1` -> kein Schaden, nur |= 2, :3184, @0x80012fb4-cc). dmg = `re15_granate_schaden(e)` (:3223 -> :3152):
   - **Vorgabe 1000** = `re15_damage_table[2]` (DAT_8006f418[2] `e8 03` @0x8006f41c) für jeden Typ, außer:
   - **Gorilla 0x27:** `GR_HP_GORILLA` 180 (@0x8011f514) x `RE15_R39_AFFEN_HP_FAKTOR` 2 (include/re15_gegner_r39.h:64,
     Nutzervorgabe) - `GR_REST_GORILLA` 40 = **320** (ohne Affenregel 140).
   - **Alligator 0x23 RE1.5-KI:** `GR_HP_ALLIGATOR` 300 (@0x801175dc) - `GR_REST_ALLIGATOR` 50 = **250**;
     **Gator-Boss:** 25300 - 25000 - 50 = **250**.
   - **Birkin 0x30:** `re15_bf_granate_form` (engine/src/birkin_formen.c:98) liefert Form-HP und Ende; Schaden = Form-HP - Ende -
     `GR_REST_BIRKIN` 110: 600-Form **490**, Form 1 (Ende 200) **290**, Mutation/nach der Mutation (300) **190**, Riesenform
     (1000) **890**.
   - **G5 0x36 in 5090/5091:** Der E16-Block danach (re15_damage.c:3247-3249) überschreibt auf **80** (RE2 Zeile 9 K0
     `05014050` @0x800A5F7C; Säure/Brand 70).
   - GR_REST_* (40/110/50) sind PORT-WAHL der Spur GR (Nutzerentscheid N2, Rechnung „danach genau N Pistolenschüsse“),
     keine Original-Adressen. Laut Nutzerentscheid 2026-11-07 entfallen sie.
4. **Werfer-Munition** (Art 6..9, Zeilen 9/10/11 über die RE2-Ops 15/47/48/49): RE2-Kandidaten bekommen den Datensatz K0/K1,
   RE1.5-Kandidaten DAT_8006f418[6..8] = 100/200/300 bzw. die RE2-Zeile unter dem Modell (re15_damage.c:4596-4605, :3209).
   Nicht Gegenstand von WS, nur zum Vergleich.

### 2.3 Wo es heute vom RE2-Raketenwert abweicht (Übersicht)

| Port-Typ | RE2-Wert | Port heute | Abweichung |
|---|---|---|---|
| Zombie-Familie, Hund, Krähe, Spinne, Baby, Arm, G5 (RE2-KI) | 900/300/300/200/200/40/200 | gleich | keine |
| RE1.5-KI-Hund/-Krähe/-Spinne (Import gilt nur für Zombies) | 300/300/200 | 1000 | Art 9 statt Datensatz |
| Birkin 0x30, Riesenform, Gator-Boss, Alligator, Gorilla, Affe, Tyrant, Ivy | s. Tabelle 3 | 1000 | kein Datensatz (`re2_gl_rec16_17` -> NULL) |

---

## 3. Zuordnung Port-Gegnertyp -> RE2-Gegnertyp

Roster laut Registrierungs-Scan (Memory reai-v2-enemy-roster-complete): STAGE1 0x10-0x1F, 0x1A, 0x20, 0x21, 0x26, 0x27;
STAGE2 0x22, 0x23, 0x24, 0x25; STAGE3 0x29, 0x30, 0x36; STAGE4 0x29, 0x2B, 0x2D; STAGE5 0x2B, 0x30. Port-Zusätze aus RE2:
RE2-Baby 0x26, G5-Tentakel 0x37, Gator-Boss (0x23 in 2090/2091), Riesenform (0x30 mit G4-Bank), G5 (0x36 in 5090/5091).
Spalte „Port heute“: Rakete unter RE2-KI (Vorgabe) bzw. RE1.5-KI; Granate.

| Port-Typ | RE2-Typ | Begründung | RE2-Rakete | Port heute (Rakete / Granate) |
|---|---|---|---|---|
| 0x10/0x11/0x12/0x13/0x18 Zombies | 0x10/0x11/0x12/0x13/0x18 | gleiche Id, Port fährt das RE2-Gehirn EMZ0 (re2z_owns_type) | 900 @0x800A426C | 900 (RE1.5-KI mit Import 900, ohne 1000) / 1000 |
| 0x16 Zombie | 0x16 Wissenschaftler | gleiche Id, eigene RE2-Tabelle 0x800A42A8 | 900 @0x800A43E8 | 900 / 1000 |
| 0x14, 0x15, 0x17, 0x19 Zombie-Ids (RE1.5-HP-Zeilen @0x8011f2b4/2d4/314/354 belegt) | gleiche Id (0x14/0x19 -> 0x800A412C, 0x15/0x17 -> 0x800A42A8) | gleiche Id; im Port nicht in re2z_owns_type | 900 @0x800A426C bzw. @0x800A43E8 | 1000 / 1000 (falls überhaupt gespawnt, s. offen) |
| 0x1B-0x1F | — | RE1.5-HP-Zeilen alle 0 (@0x8011f394-414), unbenutzt | (900) | nicht geroutet |
| 0x1A Writher/Zellenarm | 0x2D Zombie-Arme | Port fährt in jedem Flavor das RE2-EM2D-Gehirn (enemy_ai_re2_zombie.c:182); RE2-Id 0x1A wäre nur der Vorgabezeiger | 40 @0x800A52C0 | 40 / 1000 |
| 0x20 Hund | 0x20 Cerberus | gleiche Id und Kreatur | 300 @0x800A4564 | 300 (RE1.5-KI 1000) / 1000 |
| 0x21 Krähe | 0x21 Krähe | gleich | 300 @0x800A46E0 | 300 (RE1.5-KI 1000) / 1000 |
| 0x22 Stub (EM022) | 0x22 Licker rot | gleiche Id und EMD-Name; in RE1.5 inert, nicht geroutet, keine Trefferbox | 200 @0x800A485C | kein Ziel |
| 0x23 Alligator RE1.5-KI (Messhilfe) | 0x23 Alligator | gleiche Id und Kreatur | 110 @0x800A4B54 | 1000 / 250 |
| 0x23 Gator-Boss 2090/2091 | 0x23 Alligator | RE2-Gator-Modul (gator_re2_r41.c), HP 25300 und Schwäche < 25000 aus dem RE2-INIT | 110 @0x800A4B54 (= 3 Raketen bis zur Schwäche: 25300 - 330 = 24970) | 1000 (1 Rakete = Schwäche) / 250 |
| 0x24 FX-Emitter | — | RE1.5-Partikel-Emitter ohne HP und Kollision (Memory roster); RE2-0x24 = Licker grau ist eine andere Kreatur | (200 @0x800A49D8, nicht anwendbar) | kein Ziel |
| 0x25 Spinne | 0x25 Spinne | gleich | 200 @0x800A4CD0 | 200 (RE1.5-KI 1000) / 1000 |
| 0x26 RE2-Baby (Laufzeit-Spawn der Spinne) | 0x26 Babyspinne | gleich, gleiche Tabelle 0x800A4B90 | 200 @0x800A4CD0 | 200 / 1000 |
| 0x26 Feuer-Emitter ROOM1090 | — | RE1.5-Typ (Wurzel 0x80116288), kein Gegner, RE1.5-Zeile 0 (@0x8006ede0) | — | 1000, falls Kandidat (s. offen) |
| 0x27 Gorilla | **kein Gegenstück** | RE2 hat keinen Affen; RE2-0x27 ist der G-Embryo (andere Kreatur, gleiche Id). Nächste vergleichbare: Licker 0x22/0x24 (agiler Springer und Nahkämpfer) 200, G-Embryo/-Adult 0x27/0x28 201, G1 200; nur Mr. X (500) fällt heraus, ist aber ein Humanoid | Vorschlag **200** @0x800A485C (Licker rot); Alternative 201 @0x800A4E4C (gleiche Id). HINWEIS: Entscheid Orchestrator | 1000 / 320 |
| 0x29 kleiner Affe (EM029 = Menschenaffe, re15_affen3_r38.h:7-10) | **kein Gegenstück**; gleiche Id in RE2 = Kakerlake | jeder kleine Tier-Gegner in RE2 trägt 300 (Hund @0x800A4564, Krähe @0x800A46E0, Kakerlake @0x800A5144), der Wert ist also unabhängig von der Wahl eindeutig | **300** @0x800A5144 | 1000 / 1000 |
| 0x2B Tyrant | 0x2A Mr. X / 0x2B Tyrant | gleiche Kreatur; beide RE2-Typen tragen denselben Wert | 500 @0x800A543C (0x2A) = @0x800A55B8 (0x2B) | 1000 / 1000 |
| 0x2D Ivy (EM02D in RE1.5) | 0x2E Ivy | gleiche Kreatur; RE2-Id 0x2D sind die Zombie-Arme (anderer Typ) | 300 @0x800A5734 (0x2E; 0x2F/0x39 gleiche Tabelle) | 1000 / 1000 (RE1.5-Typzeile @0x8006f048 ist waffen-immun, die Explosionsarten lesen sie aber nicht) |
| 0x30 Birkin (3070, 5080, Hangar 50A0, Labor 50E0/50F1, Form 1 in 3071/5090) | 0x30 G1 | gleiche Id und Kreatur (EM030 = Birkin 1) | 200 @0x800A58B0 | 1000 / 490 bzw. 290 |
| 0x30 Birkin während/nach der Mutation (3071) | 0x30 G1 (Alternative 0x31 G2) | RE1.5 führt beide Formen als EINEN Typ mit EINER Zeile (@0x8006f150); in RE2 wechselt die Mutation den Typ (G1 -> G2) | 200 @0x800A58B0 (Alternative 300 @0x800A5A2C). HINWEIS | 1000 / 190 |
| Riesenform (0x30 mit G4-Bank, 5090) | 0x34 G4 | Modul = RE2-G4-Overlay (birkin_bahnsteig.c:4-6), HP 1000 aus dem G4-Ctor | 301 @0x800A5D24 (4 Raketen: 1000 - 903 = 97) | 1000 (-> 0 HP) / 890 |
| 0x36 G5 (5090/5091) | 0x36 G5 | RE2-G5-Modul em36 | 200 @0x800A601C | 200 / 80 |
| 0x36 in ROOM3080 (Szene) | 0x36 | Szene, Spieler gesperrt (GR N3.4) | 200 (nicht anwendbar) | nicht werfbar |
| 0x37 G5-Tentakel | 0x37 Birkin-Projektil 1 | gleiche Id; der RE2-Zeiger zeigt auf die Zombie-Tabelle (Vorgabezeiger) | 900 @0x800A426C | 1000, falls Kandidat (s. offen) |
| 0x40-0x4D NPCs | — | HP -1 = kein Kandidat (RE2 Gate 3 @0x80047148-50) | — | kein Ziel |

---

## 4. Hinweise (Lesung nicht eindeutig, mit Beleg entschieden bzw. zur Entscheidung)

1. **Gorilla 0x27 hat kein RE2-Gegenstück.** Vorschlag 200 (Licker rot 0x22, @0x800A485C). Die plausiblen Nachbarn liegen
   alle bei 200/201 (Licker 0x22/0x24, G-Embryo/-Adult 0x27/0x28, G1 0x30); nur Mr. X 0x2A/0x2B (500) weicht ab. Mit 200 oder 201 braucht der
   Gorilla (360 HP, 180 @0x8011f514 x 2) zwei Raketen; mit 500 eine.
2. **Kleiner Affe 0x29:** 300 ist eindeutig, weil alle kleinen RE2-Tiere (Hund, Krähe, Kakerlake) 300 tragen.
3. **Birkin nach der Mutation (3071):** RE1.5 kennt nur Typ 0x30 und eine Zeile (@0x8006f150), also gilt nach Id G1 = 200.
   Wer der RE2-Mutation G1 -> G2 folgt, bekommt 300 (@0x800A5A2C).
4. **Riesenform = G4 (301):** eindeutig, weil das Modul das G4-Overlay ist und die HP aus dem G4-Ctor stammt. Ergebnis wie
   in RE2: 4 Raketen. Die „0 HP lebt“-Lage entfällt damit (1000 - 3 x 301 = 97).
5. **Gator-Boss 2090:** RE2-Alligator 110 heißt 3 Raketen bis zur Schwäche (statt heute 1).
6. **Ivy 0x2D:** In RE1.5 ist die Typzeile waffen-immun (alle 0 @0x8006f048). Die Explosionsarten (DAT_8006f418) und die
   Rakete (Art 9) lesen diese Zeile aber nicht, deshalb nimmt sie im Port heute 1000 von Granate und Rakete. RE2-Ivy 0x2E ist
   verwundbar (300).
7. **Handgranate in RE2:** gibt es nicht. Der Explosionsschaden hat in RE2 keinen Abstandsabfall (Op 47: ein K für die ganze
   Box), und der Port rechnet keinen. Ein D_G wirkt also überall in der Box gleich.
