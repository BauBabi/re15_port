# Runde 38 — Spur B "szenenschutz" (Dossier, fortlaufend)

Baum: `.claude/worktrees/r38_szenenschutz` (Zweig `r38/szenenschutz`, Basis e2cc7673). Angelegt 2026-10-16 16:21.

## Nutzer-Punkt (woertlich, AUFTRAG.md)

> Das Leon schon in Cutscenes angegriffen werden kann, wo er sich noch nicht mal bewegen kann ist mist. So wie bei Leon
> zum Beispiel bei ROOM 1190 bei den Hunden oder 11D0 bei den Hunden. Es sei den es ist Teil der Cutscene wie bei Elza
> Room 1170.

Faelle: Leon ROOM1190 (Hunde), ROOM11D0 (Hunde). Gegenprobe (unveraendert lassen): Elza ROOM1171 (John + Kraehen).

## 1. Messung vorher (Stand e2cc7673, gebaut im Spurbaum, Standard-KI = RE2)

Messkopie `re15_pc_r38b_mess.exe`, Laeufe unter `scratchpad/b/runs/<name>` (RE15_STATE_LOG je Bild: Spieler-HP,
`pm` = g_scd.player_mode, `pf` = g_re15_pauseflags, je Gegner Zustand/Sub/grid/Abstand/Lage).
`pm=2` ist im Port GENAU "die Szene haelt den Spieler, Pad wird ignoriert": einziger Schreiber `g_scd.player_mode = 2`
ist platform/pc/main.c:5739 (cine_active = flag(1,27) || flag(2,7), plus 15 Bilder Letterbox-Countdown ohne Plc_ret).

### 1.1 ROOM1190 (Lauf m1190a)
Rezept: RE15_TITLE_SHOT (Neues Spiel), RE15_DEBUG_JUMP=1190@240, RE15_DEBUG_SUB=13@450 (Hunde-Spawn sub13),
RE15_SET_FLAG_AT=4:243,5:4,5:5@520 (Hebel-Kombination -> sub01 startet sub10 = Szene), Spieler am Hebel 0
(RE15_PLAYER_POS=-4100,-24500).

| Bild | Spieler | pm | Hunde |
|---|---|---|---|
| F521 | hp 100 | 2 | Szene sub10 beginnt (Set(2,7)=1, Set(1,27)=1) |
| F641 | hp 100 | 2 | Hund 1 grid 0x43 (Member_set(12,0x43) @0x027E6) -> Sprung |
| F675 | hp 100 | 2 | Hund 1 gelandet: st1/2 (RE2-RUN), Hund 2/3 folgen F707/F739 |
| F706 | hp 100 | 2 | Hund 1 st1/3 (RE2-Angriff/Lunge) — haengt an der Schranke z=-21300 |
| F739 | Teleport (900,-19800) | 1 | Szenenende (Plc_ret @0x02844, Set(2,7)=0 @0x0284E); Hund 3 landet d=3197 |
| F741 | | 1 | Hund 3 st1/3 (Lunge) |
| **F752** | **hp 100 -> 80** | 1 | **Biss 13 Bilder nach Szenenende** (Letterbox schliesst noch bis F753) |
| F773/F857/F864/F869 | 60/40/20/0 | 0 | Folgebisse ohne Eingabe |

### 1.2 ROOM11D0 (Lauf m11d0c)
Rezept: RE15_DEBUG_JUMP=11d0@240, Spieler im Aufnahme-AOT 9 (RE15_PLAYER_POS=-10600,-13250), RE15_FIRE_AOT=9@330#11D0
(sub03), Quadrat-Druecke ab F380 bestaetigen das Aufnahme-Modal (Item 5).

| Bild | Spieler | pm | Hunde |
|---|---|---|---|
| F331 | hp 100, pst 4 (Plc_motion Hocke) | 2 | sub03 Set(2,7)=1 @0x01892 |
| F451 | | 2 | sub02 Member_set(12,0x42) x5 @0x0172C.. -> Kaefig-Freigabe 0xC01 (st1/12, Clip 8) |
| F483 | | 2 | alle 5 Hunde st1/2 (RE2-RUN) |
| F514 | | 2 | sub02-Ende Set(2,7)=0/Set(1,27)=0 @0x01888/0x0188C; **Hund 4 st1/3 (Lunge) waehrend pm=2** |
| F525 | | 0 | Letterbox-Countdown fertig, Spieler frei |
| **F528** | **hp 100 -> 80** | 0 | **Biss 3 Bilder nach Freigabe — Angriff wurde in der Szene gestartet** |
| F601 | hp 60 | 0 | Folgebiss |

### 1.3 Vergleich RE1.5-KI (RE15_AI_FLAVOR=re15, sonst identisch)
- m1190_re15: erster Biss **F850** (111 Bilder nach Szenenende, Hund 2), Hunde bellen/laufen vorher.
- m11d0_re15: Hunde bleiben an der Kaefigwand (z=-14818) und schnappen ins Leere, **kein Treffer in 300 Bildern**.

### 1.4 Gegenprobe ROOM1171 (Elza, Lauf m1171a / m1171_re15)
Rezept: RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1, RE15_SET_FLAG=3:194,
RE15_DEBUG_JUMP=1170@250 (laedt ROOM1171). sub07 spawnt John (0x49) + 2 Kraehen (0x21, grid 0x40) und startet sub02.
- RE2-KI (Standard): Kraehen st1/0, greifen nie an; Szene wartet ewig in `While Ck(5,28)==0` @0x013F4 (das ist der
  Haenger der Spur I, nicht dieser Spur).
- RE1.5-KI: Kraehe 2 st4 (FLIGHT-2) greift Elza bei **F101 waehrend pm=2** (hp 100 -> 92, gr=1) — der Angriff IST die
  Szene (sub02 wartet auf genau diesen Griff). Diese Wirkung darf der Szenenschutz NICHT aendern.

## 2. RE-Belege (Original)

### 2.1 Es gibt im Original KEINE allgemeine "Ereignis -> kein Angriff"-Sperre
- Set(2,7) = DAT_800aca40 Bit 0x01000000 (Bank-Tabelle @0x80074664[2]). Alle 14 EXE-Leser von 0x800aca40
  (ghidra1_V2.txt, lw/lhu/lbu) durchgesehen; Bit 0x01000000 wird nur gelesen von
  - der Pad-Aufbereitung FUN_80030444: @0x800304f4 `lw v0,aca40` / @0x800304f8 `lui v1,0x100` / @0x800304fc `and` /
    @0x80030500 `beq` -> Pad auf die Menuebits 0xF000 (@0x80030514) = "Spieler kann sich nicht bewegen";
  - der Statusmenue-Sperre der Hauptschleife @0x8001cd0c `lui v1,0x100` / @0x8001cd14 `lw` / @0x8001cd20 `bne`.
  Kein Gegner-Code liest es.
- Gegner-Wurzeln pruefen nur das GLOBALE Einfrieren Bit 0x20000000 (Set(2,2)): Hund @0x8010d80c `lw v0,0(s0)` /
  @0x8010d810 `lui v1,0x2000` / @0x8010d814 `and` / @0x8010d818 `bne` (+ grid&0x20 @0x8010d82c); RE2-Hund
  EMD0G_MOD0.BIN @0x80100014 `lw v0,-1060(v0)` (0x800CFBDC) / @0x80100018 `lui v1,0x2000` / @0x80100020 `bne`.
  Der Port setzt dieses Tor bereits (game_step_common.c:2342 `!(g_re15_pauseflags & RE15_PAUSE_AI)`).
- Die Szenen 1190 sub10 und 11D0 sub02/sub03 setzen NUR (2,7)/(1,27), NICHT (2,2):
  1190 @0x02784 `22 01 1b 01`, @0x02788 `22 02 07 01`; 11D0 @0x01754 `22 02 07 01`, @0x01758 `22 01 1b 01`,
  @0x01892 `22 02 07 01`. Die Hunde LAUFEN im Original also waehrend der Szene (Freigabe ist Teil der Szene).

### 2.2 Was die Hunde im Original von Leon fernhaelt
- Tor "Spieler angreifbar" = Spieler+0x93 (0x800acae7, der Treffer-Riegel). RE1.5-Hund:
  ATTACK-RANGE FUN_8010e568: @0x8010e578 `lh v0,470(v0)` (+0x1d6) / @0x8010e580 `bne v0,zero` (Sperre) /
  @0x8010e590 `jal 0x8001a804` (3000/0x180) / @0x8010e5ac `sb v0(8),5` (BISS) / @0x8010e5c4 `lbu v0,-13593(v0)`
  (+0x93) / @0x8010e5cc `beq v0,zero` — bei gesetztem Riegel KEIN Biss, sondern Knurren (Sub 6, Clip 0x12, kein
  Schaden FUN_8010ee90). Biss-Kontakt FUN_8010f15c prueft `DAT_800acae7 == 0` vor `acaee -= 10`.
  Schadens-Aufloeser FUN_80012d60 @0x80012e24 `lbu v0,-0x3519(v0)` (+0x93): nur bei Bit 0 = 0 Schaden.
  STAGE1-Overlay: 54 Vergleiche auf 0x800acae7 (Decompile-Zensus `DAT_800acae7 == 0|!= 0`).
- RE2-Hund (Standard-KI, EMD0G_MOD0.BIN): Angriffs-Entscheid 0x80104CD8 @0x80104D84 `lbu v0,467(s1)` (PL+0x1D3) /
  @0x80104D8C `andi v0,v0,0x80` / @0x80104D90 `bne` -> -4 ("Spieler beansprucht"); Biss-Kontakt @0x80104E5C-68
  gleiches Tor -> 0. RUN behandelt -4 mit Halbkreis-Abdrehen (P2 @0x80100E4C, Port re2d_sub2_run).
- 1190-Freigabe FUN_80111398 Phase 4: @0x801115e8 `ori v0,zero,0x78` / @0x801115ec `sh v0,470(v1)` -> +0x1d6 = 120,
  @0x8011161c `sb zero,9(v0)` (grid verbraucht), @0x80111628 `ori v0,zero,0x201` / @0x8011162c `sw v0,4(v1)` (ACTIVE,
  Sub 2). Abzug im ACTIVE-Schwanz FUN_8010dbcc @0x8010dd5c `lh v0,470(a0)` / @0x8010dd64 `beq` / @0x8010dd6c
  `addiu v0,v1,-1` / @0x8010dd70 `sh v0,470(a0)`. => **jeder 1190-Hund darf 120 Bilder nach der Landung nicht
  beissen.** Port: re15_dog_state456 schreibt dog_atk_cd = 0x78 (enemy_ai_common.c:7591), das RE2-Gehirn liest es
  NIE -> Biss 13 Bilder nach Szenenende (1.1).
- 11D0-Freigabe FUN_80111658: @0x8011170c `lbu v1,9(a0)` / @0x80111710 `ori v0,zero,0x42` / @0x80111714 `bne` /
  @0x8011171c `sb zero,9(a0)` / @0x8011172c `sw v0(0xc01),4(v1)` -> ACT[12] 0x801101E4 (Clip 8, dann Sub 2), keine
  Sperrzeit; im Original halten Kaefigwaende die Hunde (1.3: RE1.5-KI kommt nicht heraus).

### 2.3 Die Ausnahme 1171 im Original
- Kraehen-INIT: grid&0x40 -> +0x4 = 4 (FLIGHT-2, Decompile FUN_8011224c Z.42-44; Port enemy_ai_common.c:6900 mit
  @0x80112408).
- FLIGHT-2 HOVER FUN_80115130, Kontaktpfad @0x80115254 `lbu v0,464(a0)` (+0x1d0 Kontakt) / @0x8011525c `beq` /
  @0x80115268 `lbu v1,-13736(v1)` (aca58) / @0x80115270 `beq v1,5` / @0x801152a0 `sb v0(5),-13736(at)` (Spieler-
  Kommando 5 = GEGRIFFEN) / @0x801152ac `sb v0(1),-13735(at)` / @0x801152b8 `jal 0x80115d74(2)` / @0x801152c0-c4
  `jal 0x8004ef90(0x800b1028,0x1c)` = Raum-Flag (5,28), auf das sub02 @0x013F4 `While Ck(5,28)==0` wartet.
  **Dieser Pfad liest Spieler+0x93 NICHT** (kein Zugriff auf 0x800acae7 zwischen @0x80115248 und @0x801152cc).
  => Im Original unterscheidet genau das die Faelle: freie Gegner greifen nur ueber die "Spieler angreifbar"-Tore an
  (2.2), der Szenen-Griff in 1171 laeuft in einem Skript-Zustand (+0x4 = 4) ohne Tor.

## 3. Umsetzung

(offen)

## 4. Messung nachher

(offen)

## 5. Tests

(offen)

## 6. OFFEN

(offen)

## 7. Fuer den Nutzer

(offen)
