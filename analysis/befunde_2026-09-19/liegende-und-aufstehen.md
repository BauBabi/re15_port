# liegende-und-aufstehen — Phase 2 (Umsetzung) zu den Dossiers `liegende-zombies.md` und `aufstehen-schuss.md`

Runde 16, 2026-09-19. Dieses Blatt buendelt die Umsetzung der beiden Fix-Plaene:

- `liegende-zombies.md` §4 — passiv liegende Zombies (Deskriptor 0x87/0x88) muessen im RE2-Flavor
  unschiessbar und unreaktiv bleiben.
- `aufstehen-schuss.md` §4 — Treffer waehrend des Boden-Aufstehers laufen ueber FUN_80107A78
  (eigener Handler `re2z_getup_hurt` mit +0x228/+0x22C-Schnappschuss).

Skeptiker-Praezisierung (uebernommen): FUN_80107A78 liest +0x1D2 in den `sltiu < 3`-Toren der
Zeilen 10 UND 11 — `lbu v0,466 / sltiu v0,v0,3 / beq` @0x80107C04-10 (Zeile 10, Brand) und
@0x80107CCC-D8 (Zeile 11, Saeure). Beide Tore stehen so im Handler.

## 6. Umsetzung (Phase 2)

### 6.1 Eigene Nachpruefung der Belege (re2_disasm.py, EMZ0.BIN RAW @0x80100000)

Vor dem ersten Edit erneut disassembliert (nicht aus dem Dossier abgeschrieben):

```
80105014: lhu v0,538(s1)        ; +0x21A
8010501c: andi v0,v0,0x10
80105020: beq v0,zero,0x8010503c ; kein Bit 0x10 -> Flinch-Tor
8010502c: jal 0x80107a78        ; AUFSTEHER-TREFFER
80105034: j   0x80105418        ; Epilog (keine Liege-Route, kein 2D-Dispatch)

8010061c: lw v0,4(s0)           ; Zustandswort nach dem Dispatch dieses Ticks
80100628: sw v0,552(s0)         ; +0x228 (Delay-Slot des jal 0x80016028)

80107aec-cfc  P0: sb 1,6 / sh 0,344 / sh 1,346 / sb 0,362 / (+0x1D0&0x20 -> sh -1,346)
              Blut 6096 @+1448 (Part 8) v={0,300,0}; sb 15,334; lw 552 -> (&0xff==1) sw 556
              HP<0: sh 0,342 / sb 1,362 / Blut 0x31F40 a1=rand<<4 / sb 15,334
              SE 12 wenn +0x239==0, +0x239=150; Zeilen 10/16/15/18/11/14 (s.o.)
80107d00-db4  P1: advance(256); fertig -> EXIT; Twist ((+0x158*+0x16B)<<3)*+0x15A; +0x158++ ; alt>=3 -> P2, +0x158=16
80107db8-e54  P2: advance(256); fertig -> EXIT; Twist ohne <<3; alt=+0x158; +0x158=alt-1; alt==0 -> P3
80107e58-6c   P3: advance(256); fertig -> EXIT
80107e70-ecc  EXIT: andi 0xffed (+0x21A); sw +0x22C -> +0x4; +0x6 += 1; andi 0xdfff (+0x10E)
```

### 6.2 Aenderungen

| Datei | Was |
|---|---|
| `re15_port/include/re15_actor.h` | zwei neue Felder `re2z_word228` (+0x228, Produzent @0x8010061C-28) und `re2z_word22c` (+0x22C, Produzent @0x80107B4C, Leser @0x80107E70) |
| `re15_port/engine/src/enemy_ai_re2_zombie.c` | (1) `re15_re2z_hit_filter_apply`: `passive_lyer = (grid_id & 0x80) && Nibble 7/8` nimmt die Spawn-Pose-Ausnahme zurueck (Gates (2) @0x80047138-40 / (4) @0x80047158-64 gelten wieder; RE1.5-Zwilling @0x80103AAC-AB8). Der Latch-Nachzug (`andi 0xbfff`/`andi 0x7f`) haengt jetzt an `in_pose`, nicht an der Ausnahme — sonst haette er die Latches des passiven Liegenden geloescht. (2) `re15_re2z_tick`: Schnappschuss +0x228 nach dem Zustands-Dispatch. (3) neuer Handler `re2z_getup_hurt` = FUN_80107A78 (P0..P3 + EXIT, Twist ueber `re2z_lean_pair`/`re2z_lean_angle`). (4) `re2z_hurt`: Weiche `+0x21A & 0x10` zwischen Kriecher-Zweig und Flinch-Tor (@0x80105014-38); der irrefuehrende OPEN-Kommentar ("Kriecher-Umbau, unerreichbar") ist ersetzt. (5) Diagnose-Wert `RE2ZH_GETUP7A78` fuer `re15_re2z_last_hit_handler()`. |
| `re15_port/tests/unit/test_r16_liegende_unschiessbar.c` | Pin (1), echter Weg |
| `re15_port/tests/unit/test_r16_aufstehen_schuss.c` | Pin (2), echter Weg |
| `re15_port/tests/unit/probes/p2_liegende-und-aufstehen.cmake` | Registrierung beider Pins (add_test) |
| `re15_port/tests/unit/probe_r16_liegende_zombies.c`, `probe_r16_aufstehen_schuss.c` + `probes/r16_*.cmake` | die Messsonden der Dossiers, unveraendert versioniert (Vorher/Nachher reproduzierbar) |

Nicht angefasst: `re15_damage.c`, `enemy_ai_common.c`, `main.c`, EXEC[5]/EXEC[8], Liege-Route,
Flinch-Tor, Naehe-Wecker, D15.2, Spawn-Remap (HP bleibt RE1.5-Zeile).

Blut-Auftritt in P0: Id 6096 @0x80107B14, Anker +1448 = Part 8 (8*172+72, derselbe Anker wie
der Kopf-Emitter der Verkohlung @0x801061AC — damit ist das "Offen"-Item 1 des Dossiers
aufgeloest). Dargestellt nach dem im Repo verankerten Nutzer-Mandat (RE2-Entscheidung, RE1.5-
Praesentation) ueber `re2z_blood_fx_scaled(e, 8, rot_y, 0x2000, 8)`, wie die drei Treffer-
Emitter der HURT-Wurzel.

### 6.3 Messwerte vorher / nachher (Sonden der Dossiers, unveraenderter Code der Sonden)

`probe_r16_liegende_zombies <raum> both`, Pistole, Abstand 2600, 120 Bilder Feuer:

| Lauf | vorher | nachher |
|---|---|---|
| RE2 ROOM1140 DOWN | **7 Treffer**, hp 79 -> 2, steht im ersten Bild auf (topY -373 -> -2766), Ende 1/5/1 | **0 Treffer**, hp 79, Ende 1/7/1, topY -373 |
| RE2 ROOM10E0 DOWN | **5 Treffer**, hp 79 -> -1, Ende 7/7/0 (Leiche) | **0 Treffer**, hp 79, Ende 1/7/1, topY -373 |
| RE2 1140/10E0 LEVEL | 0 Treffer | 0 Treffer |
| RE2 B/B2 Sweep bis 849 | keine Abweichung | keine Abweichung (Pin: bis 399) |
| RE1.5 ROOM1140 LEVEL/DOWN | 0 Treffer, +0x93=0x01 | 0 Treffer, +0x93=0x01 |
| RE2 (Pin P) 0x86-Fresser slot 2, DOWN | — | 4 Treffer, hp 50 -> -14 (Ausnahme lebt) |
| RE2 (Pin N) Skript-Bump 0x89 | — | aufgestanden bei Bild 68, danach 4 Treffer hp 79 -> 35 |

`probe_r16_aufstehen_schuss` (ROOM1140, RE2, drei Schuesse im Abstand 24):

| Lauf | vorher | nachher |
|---|---|---|
| B P7 Bild>=10 EBEN | 3 Treffer, **3 Neustarts P7->P6**, aufgestanden 81 Bilder nach dem letzten Treffer | 1 Treffer (EBEN erreicht nur die tiefen Bilder), **0 Neustarts**, Clip 8 laeuft 13 -> 79 durch |
| B P7 Bild>=10 TIEF | 3 Treffer, **3 Neustarts** | 3 Treffer, **0 Neustarts**, hnd=GETUP7A78 je Treffer, +0x22C=0x00070501 bleibt ueber alle drei, Aufstehen Bild 228 (= Clip-Ende) |
| B P7 Bild>=40 EBEN | 3 Treffer, **3 Neustarts** | 2 Treffer, **0 Neustarts** (2. Treffer erst nach dem Aufstehen -> MAIN5438, korrekt) |
| C Liegen TIEF | HIT -> 0x60501 -> P6 (byte-true), Folgetreffer im Aufsteher: **Neustart** | HIT -> 0x60501 -> P6 (unveraendert), Folgetreffer: **kein Neustart**, GETUP7A78 |
| E Fresser-Aufsteher EXEC[8] P4 | 2 Treffer ueber **MAIN5438**, Clip 0x15 abgebrochen, `+0x21A & 0x10` **klebt** (0x0010 am Ende) | 2 Treffer ueber **GETUP7A78**, Clip 0x15 laeuft 12 -> 53 durch, +0x22C=0x00040801, Bit 0x10 weg, Aufstehen ueber P5 (Bild 57) |

Der lange Nachlauf der Pins zeigt nach dem Aufstehen einen regulaeren Griff -> Abschuetteln ->
0x501-Sturz (Zeilen f298..f449 im Log) — Spiel-Logik nach dem Aufstehen, nicht Gegenstand; die
Pins messen deshalb am Aufsteh-Bild und brechen 30 Bilder danach ab.

### 6.4 Pins

- `unit_r16_liegende_unschiessbar` — RE2 ROOM1140 + ROOM10E0: C(DOWN) 0 Treffer / HP / Zustand /
  Pose / +0x93 Bit 0 / Latches +0x1D3 0x80 und +0x10E 0x4000 stehen; B2 Sweep 600 Bilder ohne
  Abweichung; (P) 0x86-Fresser muss mit DOWN treffen; (N) Skript-Bump 0x89 -> aufstehen -> treffbar;
  RE1.5 ROOM1140 C(DOWN) als Regressionswache. Vorher: 7/5 Treffer -> Pin rot; nachher gruen.
- `unit_r16_aufstehen_schuss` — B(EBEN,10) / B(TIEF,10) / B(EBEN,40): 0 Neustarts, 0 Neustuerze,
  alle Treffer vor dem Aufstehen ueber GETUP7A78, kein Clipwechsel, Bildnummer monoton,
  +0x22C == 0x00070501, im Aufsteh-Bild +0x21A & 0x12 == 0 und +0x10E & 0x2000 == 0, Zustand 1/1;
  C: Liege-Route bleibt (1/5/6, +0x16B=1), Folgetreffer kein Neustart; E: kein MAIN5438,
  Clip 0x15 ganz, +0x22C == 0x00040801, Bit 0x10 weg. Vorher: 3/3 Neustarts -> Pin rot.

### 6.4a Neu verankerte Bestands-Pins (Szenario-Verschiebung durch den Fix, Memory reai-v2-pin-fixture-verschiebung)

Gegenprobe: alle drei sind auf dem Basis-Stand (6692c9cd, Engine ohne die zwei Fixes, im selben
build_p2 gebaut) gruen; rot wurden sie erst durch den Fix.

| Pin | Symptom mit Fix | Ursache | Neuverankerung |
|---|---|---|---|
| `unit_re2_zombie_abc` (C) | 64/64 Seeds "UNSTERBLICH": `[C-VORFALL] slot=1 st=1 s1=7 1D3=0x80 10E=0x4002` | das ist der passive 0x88-Liegende — im Original dauerhaft gesperrt bis zum Skript-Wecker (RE1.5 @0x80103AAC-AB8, RE2 @0x80103804-14 / @0x80100A34-38); ROOM1140 hat keinen | der passive Liegende in EXEC[7] (grid&0x80, Nibble 7/8, 1/7) zaehlt nicht als haengender Latch |
| `unit_re2_zombie_abc` (A) | 89 "FALLEN UND SOFORT AUFSTEHEN", kuerzestes DOWN = 1 | Treffer im Aufsteher -> Zustand 2 (FUN_80107A78) beendete das DOWN-Fenster; der Exit stellt 1/5/8 wieder her, P8 committet ein Bild spaeter 0x101 = Fenster der Laenge 1 "aufrecht" | ein OFFENES DOWN-Fenster laeuft durch Zustand 2 mit +0x21A&0x10 hindurch; P2/P3-Struktur nur aus EXEC[5] selbst gezaehlt |
| `unit_re2z_bandlock_pin` Fall 5 | LEVEL = 0 fuer seed 50 slot 2 | der Kandidat liegt beim Orakel in EXEC[5] (st=1 s1=5, 21A&2) — LEVEL = 0 ist dort byte-true (Liege-Band), DOWN-Aim = 30; die Kampffolge in seed 50 hat sich durch den nicht mehr neu startenden Aufsteher verschoben | Orakel-Skip deckt "nicht aufrecht" (s1 > 3) wie der Sweep, statt nur Leichen; 4 Faelle bleiben geprueft (LEVEL = 30) |
| `unit_re2z_rise_hittable` | 80 Treffer < Schranke 100 (Basis 2026-08-27: 400 mit / 8 ohne Band-Fix) | die 400 stammten zum Grossteil aus Clip-Neustarts (jeder Treffer = neue tiefe Pose); jetzt laeuft der Clip einmal durch, Treffer im Aufsteher = Trefferpausen-Takt je Durchlauf | Zustand-2-Bilder mit +0x21A&0x10 zaehlen als Aufstehen (sind es: Clip laeuft ungebrochen) -> 104 Treffer / 5992 Bilder, deterministisch 13 je Seed; Schranke 50 (zwischen 8 und 104) |

### 6.5 Offen

- Sichtpruefung (Dossier liegende-zombies §4 Schritt 4, gdigrab/FRAMEDUMP in ROOM1140/10E0) ist
  in dieser Phase NICHT gelaufen: der Debug-Sprung setzt den Spieler an die DEBUG.BIN-Position,
  die Ausrichtung/Entfernung zum Liegenden dort ist nicht bekannt, ein Input-Skript dorthin ist
  nicht gebaut. Die Messung laeuft ueber den echten `re15_game_step`-Weg (RDT, sub00, echte
  Baenke, Pad-Feuer), also denselben Code-Pfad, den der Nutzer spielt.
- Zeile-10/11-Tore (`hits1d2 < 3`) sind im Port nur mit Spalte 0/1 erreichbar (Stempel-Bruecke
  in re15_damage.c), also immer offen — wie im Dossier vermerkt, ohne Wirkung auf den Defekt.
- Der HP<0-Zweig von P0 (@0x80107B80-BC) ist im Port wie im Original praktisch unerreichbar
  (der Applier stempelt bei HP<0 Zustand 3 -> DEATH); er ist byte-nah mitgenommen, aber nicht
  durch einen Pin gedeckt.
- Nicht portiert (Praesentation/Kollision ohne Zwilling): Hitbox-Reset @0x80107E78-98,
  word0 |= 0x0C000000 @0x80107EA8-B0, Emitter-Geschwindigkeit {0,300,0} @0x80107B20-24.
- EBEN-Zielen erreicht den Aufsteher nur in seinen tiefen Bildern (RE1.5-Hitscan-Geometrie in
  re15_damage.c) — nicht Gegenstand dieser Runde, hier nur als Beobachtung.

### 6.6 ctest

build_p2 (Worktree, mingw64 GCC + Ninja, RE15_BUILD_TESTS=ON), Komplettbau + `ctest --timeout 120`:

```
100% tests passed, 0 tests failed out of 307
Total Test time (real) = 331.38 sec
```

Erster Lauf nach den Fixes: 3 rot (unit_re2_zombie_abc, unit_re2z_bandlock_pin, unit_re2z_rise_hittable) -> §6.4a; nach der Neuverankerung 307/307.
