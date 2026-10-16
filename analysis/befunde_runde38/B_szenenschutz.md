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
- RE2 Retail genauso: 28 EXE-Leser von Stop_flg 0x800CFBDC (ghidra_re2_Leon.txt); Bit 0x01000000 lesen nur die
  Hauptschleifen-Tore @0x8002641c-30 (FUN_80021a48) und @0x8002b030-40 (Menue-Oeffnen, neben ClearOTagR) und die
  Pad-Aufbereitung @0x800391f8 `lw v1,-0x424(v1)` / @0x800391fc `lui a0,0x100` / @0x80039200 `and` / @0x8003921c
  `andi v0,v0,0x3c00` (nur Menuebits). Die RE2-Gegnermodule pruefen nur 0x20000000 (Hund EMD0G_MOD0.BIN @0x80100014-20).
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

### 2.4 "Kann sich nicht bewegen" auch ohne (2,7): der Plc-Executor
- Plc_motion @0x80041ba4 `ori v1,zero,0x4` / @0x80041bb0 `sb v1,4(v0)` und Plc_dest @0x80041c10 `ori v0,zero,0x4` /
  @0x80041c14 `sb v0,4(a1)` setzen Spieler+0x4 (DAT_800aca58) = 4; erst Plc_ret @0x80041f8c `ori v1,zero,0x1` /
  @0x80041f90 `sb v1,4(v0)` gibt ihn frei. Der Spieler-Dispatcher FUN_80031c44 (@0x80031c8c `lbu v1,-13736(v1)` /
  @0x80031cac `lw v0,0(at)` / @0x80031cb4 `jalr v0`) springt dann ueber @0x80073f90[4] = 0x80030660 (Executor) statt
  [1] = 0x80031de8 (Pad-Zweig).
- Zensus (alle 240 RDTs, tools/scd_dump_room.py): 15 Subs fuehren den Spieler (Work_set(1,0) + Plc_motion/Plc_dest)
  OHNE Set(2,7)/(1,27) in derselben Sub, z.B. **ROOM1230 sub21** (Hocke @0x014B4 `3f 01 0b 00`, Plc_ret @0x014CE) —
  im Hunderaum 1230. Auch dort kann sich Leon nicht bewegen.

### 2.5 Zensus "Szene haelt den Spieler" x Gegner (wer ist betroffen)
Raeume mit Set(2,7)=1-Fenster UND Gegner-Spawns (Typ < 0x40), Auszug: Zombies 0x10/0x11/0x16/0x18 (1011, 1021, 1030,
1040, 1090, 10D0, 1100, 1140, 11E0, 11F0, 1200, 2000, 2020, 3000, 30E0, 5060), Hunde 0x20 (1190, 11D0, 1230, 3060),
Kraehen 0x21 (1170 frei, 1171 Skript), Gorilla 0x27 (1141, 11C0), Spinne 0x25 (2030, 2050), Kakerlake 0x29 (3040,
4000, 5031), Birkin 0x30/0x36 (3070, 3080, 5080, 5090), Gitterarm 0x1A (1211), 0x2b (4001), 0x2d (4031), 0x24 (20B0).
Darum gilt der Schutz fuer JEDEN Gegner im freien Gehirn, nicht nur fuer Hunde.

## 3. Umsetzung

Neue Dateien: `re15_port/engine/src/szenenschutz_r38.c`, `re15_port/include/re15_szenenschutz_r38.h`.
Haken (gemeinsame Dateien, je 1-2 Zeilen, Kommentar "Runde 38 Spur B"):
- `enemy_ai_common.c`: `#include`; `re15_enemy_ai_run_all` Kopf `re15_szenenschutz_durchlauf_beginn()`, je aktivem
  Akteur `re15_szenenschutz_gegner(s)` (direkt nach `if (!e->active) continue;`), nach der Schleife
  `re15_szenenschutz_durchlauf_ende()`; `re15_player_is_grabbed()` `|| re15_szenenschutz_sicht()`.
- `enemy_ai_re2_dog.c`: `#include`; Sperrzeit-Tor in `re2d_attack_decide` (nach dem hp<0-Test), am STALK-Commit
  (@0x801009C8) und am RUN-P3-Commit (@0x80101174-7C); Abzug `re15_r38_hund_sperrzeit_takt(e)` im ACTIVE-Schwanz
  (hinter dem Sub-Dispatch, wie @0x8010dd5c hinter @0x8010dcf0).

(A) SZENENSCHUTZ — NUTZER-VORGABE, Form aus dem Original (2.1-2.4):
- Halten = `g_scd.player_mode == 2` (einziger Schreiber platform/pc/main.c cine_active = (1,27)||(2,7)) ODER
  Spieler+0x4 == 4 (Plc-Executor, 2.4).
- Waehrend des Ticks eines Gegners im FREIEN Gehirn (+0x4 == 1, Typ < 0x40, nicht Kraehe mit Skript-Marke grid&0x40)
  sieht er den Spieler durch die Original-Tore als nicht angreifbar: Spieler+0x93 Bit 0 (RE1.5, 2.2) und
  PL+0x1D3&0x80 (RE2, Port `re15_player_is_grabbed`). Danach steht wieder der echte Wert; Szenen-Rollen (+0x4 != 1)
  ticken mit dem echten Wert, ihr Treffer wird uebernommen.
- Sicherheitsnetz: zieht ein geschuetzter Tick trotzdem HP ab (ungatterter Pfad), gibt das Modul sie zurueck und
  protokolliert `[szenenschutz] F.. Treffer abgefangen ...`; eine Aenderung der Opfer-FSM wird als WARNUNG protokolliert.
  Protokoll-Flanken `[szenenschutz] F.. an` / `aus (N geschuetzte Gegner-Ticks)` stehen im debug.log.
- Konstanten: keine Zahl — nur die Original-Tore (Bit 0 von +0x93, Bit 0x80 von +0x1D3), Zustandswert 1 (ACTIVE,
  Wurzeltabellen [1]), Executor 4 (@0x80041bb0/@0x80041c14), Skript-Marke 0x40 (@0x80112408).

(B) FREIGABE-SPERRZEIT 1190 — byte-true Uebernahme: der Port fuehrte den RE1.5-Sprung-Ausgang auch unter dem RE2-Gehirn
(`dog_atk_cd = 0x78`, enemy_ai_common.c:7591), las den Wert dort aber nie. Jetzt: kein Angriffs-Commit solange
`dog_atk_cd != 0` (Tor @0x8010e578-0x8010e580), Abzug je ACTIVE-Bild (@0x8010dd5c-0x8010dd70). Konstante 0x78 bleibt
die vorhandene Zeile @0x801115e8-ec. Nur der 1190/1191-Sprung-Ausgang schreibt den Wert unter dem RE2-Gehirn (RE2-INIT
fasst ihn nicht an, Raumaufbau nullt alle Akteure, scd_room_setup.c memset).

Folgeanpassung eines fremden Riegels: `tests/integration/test_r36_kleidung_hundeweg.cmake` (Runde 36 Spur B) mass die
Bisse 13 Bilder nach dem Wecken; mit der Sperrzeit beissen die Hunde 120 Bilder nach der Landung (gemessen
F664/F674/F685 statt F582-F587). Save-Haken 600 -> 700 (gleicher Stand: drei Bisse, hp 40); Biss 2 trifft jetzt Panel 5
(vorn, Tabelle @0x80121014 dir 0 -> 0x05) statt Panel 4 -> Laeufe 2/3 drehen R4 statt L4, Kasten x150..200/y70..150,
Schwelle 25 (gemessen 50 Pixel). Der Zweck des Riegels (zwei Stempel je 20-HP-Biss, Decal vor Danger, Spielstand traegt
es, CONTINUE zeigt es) bleibt unveraendert.

## 4. Messung nachher (gleiche Rezepte wie 1.)

| Lauf | Szene (pm=2) | HP-Verlust in der Szene | Hund st1/3 in der Szene | erster Biss | vorher |
|---|---|---|---|---|---|
| n1190a (RE2) | F521..F738 | 0 | 0 (vorher Hund 1 F706-F738) | **F805** (66 nach Szenenende) | F752 (13 danach) |
| n11d0c/d (RE2) | F331..F524 | 0 | 0 (vorher Hund 4 F514-F524) | **F550** (25 nach Freigabe) | F528 (3 danach) |
| n1190_re15 | F521..F738 | 0 | — | F850 | F850 (state.log 1100 Zeilen bytegleich) |
| n11d0_re15 | F331..F524 | 0 | — | kein Biss | kein Biss (bytegleich) |
| n1171a (RE2, Elza) | ab F1 | 0 | — | — | state.log ab Raumeintritt **bytegleich** (md5) |
| n1171_re15 (Elza) | ab F1 | Kraehen-Griff F101 hp 92, gr=1 | — | — | **bytegleich** (erste 590 Bilder md5) |
| n1170crow (Leon sub14) | F301..F731 | 0 | — | — | 3017 geschuetzte Kraehen-Ticks, keine im Angriff |
| n1191elza (Elza, RE2; Hebel-Kombination B: 3:112,5:6,5:7) | F521..F738 | 0 | 0 | F805 | — (SCD sub10/sub13 opcode-gleich zu 1190) |
| n11d1elza (Elza, RE2) | F331..F524 | 0 | 0 | F550 | — (SCD opcode-gleich zu 11D0) |

1190 Sperrzeit im Detail (n1190a): Hund 1 landet F675 -> erster Commit F796 (= +121), Hund 3 landet F739 -> F860
(+121); RE1.5-Referenz (m1190_re15) erster Biss F850.

## 5. Tests

- `unit_r38_szenenschutz` (neu, tests/unit/test_r38_szenenschutz.c, Registrierung tests/unit/probes/r38_szenenschutz.cmake):
  T1 Praedikat (Halten = pm 2 oder Executor 4; frei/Skript/NPC/Skript-Kraehe), T2 Haken-Folge + Sicherheitsnetz,
  T3 ROOM1190 RE2-Hund Sperrzeit (Landung schreibt 120, Commit 121 Bilder danach; NEGATIVKONTROLLE ohne Tor: 2 Bilder
  danach = Messlauf F741), T4/T5 gehalten 400 Bilder kein Treffer und 0 Angriffs-Bilder (NEGATIVKONTROLLE ohne Schutz:
  RE2 64 Angriffs-Bilder + 2 Bisse hp 60, RE1.5 49 Biss-Bilder), danach Biss; T4b Spieler im Plc-Executor ohne (2,7)
  kein Angriff, nach Plc_ret Biss; T6 Gegenprobe ROOM1171 Szenen-Kraehe greift den gehaltenen Spieler (Bild 95, hp 92);
  T7 Zombie 0x10 RE2 + RE1.5 frei greift an (Bild 23 / 5), gehalten 600 Bilder nicht.
- `integration_r38_szenenschutz` (neu, tests/integration/test_r38_szenenschutz.cmake): echte exe, Laeufe 1190 / 11D0
  (Leon, Standard-KI) / 1171 (Elza, RE1.5-KI) / 1171s (Elza, Standard-KI). Gemessen 2x reproduzierbar:
  `[1190] Szene F521..F738 hp-Verlust=0 Angriffs-Bilder=0 erster Biss F805; [szenenschutz] F739 aus (94 geschuetzte
  Gegner-Ticks)`, `[11D0] Szene F331..F524 hp-Verlust=0 Angriffs-Bilder=0 erster Biss F550`, `[1171] Szenen-Kraehe
  greift bei F101 (hp 92, pm=2)`, `[1171s] kein geschuetzter Akteur in ROOM1171`. Gegen den alten Stand schluege der
  Riegel an (1190 Angriffs-Bilder F706-F738 und Abstand 13 < 30; 11D0 Angriffs-Bilder F514-F524).
- angepasst: `integration_r36_kleidung_hundeweg` (s. 3.).

(ctest-Summenzeilen: Abschnitt 5.1 unten.)

## 6. OFFEN

- Die RE2-Kraehe (Standard-KI) hat am Strike-/Griff-Commit KEIN Spieler-Tor (EMOVL21_S0.BIN liest +0x1D3 nur am
  eigenen Akteur, @0x80100160/@0x80100fbc); eine freie Kraehe an einem gehaltenen Spieler wuerde nur vom
  Sicherheitsnetz (HP zurueck) gebremst, ein Griff wuerde als WARNUNG protokolliert. Im ausgelieferten Spiel trifft das
  nie zu: freie Kraehen gibt es nur in 1170 (sub14 teleportiert Leon 23000+ weg, n1170crow), 10C0/1120 haben keine
  (2,7)-Fenster (Zensus 2.5). Kein gemessener Defekt -> kein Eingriff.
- Die Szenen-Kraehe 1171 haengt (Szene wartet auf (5,28)) und ihr HOVER-Griff traegt im Port ein +0x93-Tor und -8 HP,
  die das Original @0x80115248-0x801152c4 nicht hat — Spur I (heliport1171). Dieser Spur aendert daran nichts
  (state.log bytegleich), der Szenenschutz nimmt die Rolle per Zustand 4 / grid 0x40 aus.

- RE1.5-KI (nicht Standard): die Sicht "+0x93 gesetzt" fuehrt den RE1.5-Hund im ATTACK-RANGE in seine Original-Reaktion
  auf einen gerade getroffenen Spieler (Knurren Sub 6; bei Spieler-HP < 50 Ausholen Sub 5/7, @0x8010e5c4-0x8010e624).
  In 1190 verhindert die Sperrzeit den Entscheid ohnehin, in 11D0 kommen die RE1.5-Hunde nicht aus dem Kaefig
  (state.log bytegleich, 4.); in einem anderen Raum koennte ein Hund nach einer Szene bei HP < 50 bereits ausgeholt
  haben. Kein gemessener Fall im Standard-Flavor.

## 7. Fuer den Nutzer

- Keine neuen Assets, keine Sprachdateien, keine Bank-/Message-Ids.
- Verhalten: Solange eine Szene Leon festhaelt (Letterbox/Skript oder eine Skript-Bewegung wie Hocken), greifen
  Gegner ihn nicht an — sie laufen, kreisen oder knurren. Ist der Angriff selbst Teil der Szene (Elza ROOM1170: die
  Kraehe packt Elza, John schiesst), bleibt er. Nach der Szene greifen die Gegner normal an; die Zwingerhunde in
  ROOM1190 beissen wie im Original erst 4 Sekunden (120 Bilder) nach ihrer Landung.
- Im debug.log zeigen `[szenenschutz] ... an/aus` die geschuetzten Fenster.
