# Trefferhoehe: liegende/kriechende Zombies bei EBEN-Zielen, Trefferzonen (Runde 16, 2026-09-19)

Nutzer: „Ich kann gerade schiessen und treffe Zombies am Boden. Das kann so nicht sein, schaue
noch mal nach was ich wie treffen kann, wenn ich runter, mittel oder hoch ziele. Auch was die
Trefferzonen angeht." — KI-Flavor RE2 (Default).

Sonde: `re15_port/tests/unit/probe_r16_trefferhoehe.c` (Registrierung
`re15_port/tests/unit/probes/r16_trefferhoehe.cmake`, Build `re15_port/build_r16_trefferhoehe`),
Log: `analysis/befunde_2026-09-19/trefferhoehe_sonde.log`.
Alle @0x-Adressen: RE1.5 = `info/Re1.5/PSX.EXE` bzw. `PSX/BIN/STAGE1.BIN`; RE2 = `info/re2leon/PSX.EXE`
bzw. `info/re2leon/COMMON/BIN/EMOVL10_S0.BIN` (Zombie-Overlay). Alles selbst disassembliert/gedumpt
mit `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` / `re2_disasm.py`.

## 0. Kurzfassung

1. **Defekt (gemessen):** Im RE2-Flavor trifft EBEN-Zielen den **Kriecher** (+0x10E = 0x2001) und
   den **Aufsteher** (EXEC[5] P6/P7) auf jeder Distanz (W3: 16 Schaden, W8: 200/Crit), obwohl RE2
   beide verwirft; TIEF-Zielen trifft Stehende/Kriecher auch jenseits 4100 (RE2: nicht).
2. **Original RE2:** Der Schuss-Applier FUN_800410CC prueft je Kandidat `word0>>26&7` (Teile-Maske
   Beine/Rumpf/Kopf) gegen Prioritaetszeilen `DAT_800A6DB4`, die aus dem dy-Fenster (Waffe x
   Zielhoehe x Sub-Box, FUN_80041B20) ein Drittel waehlen. EBEN bei dy=0 laesst NUR Rumpf zu
   (`[02 00 00]`), TIEF NUR aus `[01 02 04]` (Beine zuerst).
3. **Original RE2, Zombie-Overlay:** Beim Hinlegen/Kriechen wird die Maske von `0x0C000000`
   (Beine+Rumpf, INIT @0x80100984) auf `0x04000000` (NUR Beine) geschaltet — sieben Stellen,
   u.a. Kriecher-Eintritt @0x80106B38-50/@0x80107828-38, EXEC[5] P0 @0x801032DC-EC — und erst beim
   Aufstehen zurueckgesetzt (@0x801036DC-F0 ab Clip-Bild 55, @0x80103730, @0x80103908, @0x80107EA8).
4. **Port-Ist:** re15_damage.c fuehrt die Maske nicht (kein Feld; enemy_ai_re2_zombie.c:1858
   „OPEN"), nimmt den Kriecher ausdruecklich von der Liege-Regel aus (:1460 `!(f10e&1)`) und
   schickt ihn durch die STEH-Fenster; den Aufsteher schickt `re2_rising` (:1449-1455) in das
   RE1.5-Band (LEVEL+DOWN<3000). Die Zone (+0x1D2) kommt aus der Zielhoehe statt aus der Maske.
5. **Fix:** Feld `re2z_parts` (3 Bit) an allen 11 Original-Stellen fuehren; fuer RE2-owned Zombies
   den Band-/Fenster-Block durch den nachgerechneten Applier ersetzen (Fenster x Sub-Box x Drittel
   x Maske → Treffer, Zone, Klammer); `+0x1D2 = Zone + 3*Klammer`; `re2_rising`/Liege-Sonderfall
   fuer RE2 streichen. Sonde als Pin (Kriecher EBEN = 0 Treffer, TIEF = Zone 0).

## 1. Reproduktion/Messung

Sonde `probe_r16_trefferhoehe`: ROOM1140, RE2-Flavor, RE2-Bank EM010 geladen, ein Zombie
(Slot 2, Typ 0x10) + der 0x88-Liege-Spawn (Slot 1). Orakel = ein direkter Aufruf von
`re15_player_weapon_fire(w)` bei Zielhoehe {HOCH, EBEN, TIEF} (Hook
`re15_player_set_aim_elevation_for_test`), Spieler auf der +x-Seite in Distanz d, per atan2
ausgerichtet (Aufstellung wie `probe_re2z_bandlock::oracle_hits`), Spieler-y = Zombie-y (dy = 0),
danach Aktor byte-genau zurueckgesetzt. Latch/Pausenfilter (+0x93) wird NICHT angefasst.
Daneben rechnet die Sonde RE2s Applier aus den gedumpten Tabellen nach („SOLL", §2.2-2.5).

Lagen: **STEHEND** (Fresser-Spawn-Pose durch Spielernaehe verlassen, dann st=1/1/0, +0x1D3=0),
**KRIECHER** (echter Pfad: W8 EBEN aus 3600 → Ragdoll 0x801066FC → P2 → +0x10E=0x2001 nach 44
Bildern), **LIEGEND-P3** (echter EXEC[5]-Lauf ab dem 0x501-Eintrag, Phase 3 = Boden-Zucken seit
20 Bildern), **AUFSTEHEN-P6** (EXEC[5] Phase 6), **LIEGESPAWN** (0x88, EXEC[7], ohne Ticks).

Ergebnis (W3 = Pistole/RE2-Id 3, W8 = Pump-Schrot/RE2-Id 7; d in {1500,2500,3600,5500,9000};
Zone = gestempeltes +0x1D2; „x" = Divergenz zum RE2-Soll):

| Lage (Maske RE2)         | Zielhoehe | IST Port                                   | SOLL RE2                                 |
|--------------------------|-----------|--------------------------------------------|------------------------------------------|
| STEHEND (3)              | W3 HOCH   | MISS alle                                  | MISS alle                                |
|                          | W3 EBEN   | HIT alle, Zone 1                           | HIT alle, Zone 1, Klammer 0/0/0/1/2      |
|                          | W3 TIEF   | HIT alle, Zone 0                           | HIT ≤3600 Zone 0; **MISS 5500/9000** x   |
|                          | W8 HOCH   | HIT 1500/2500 (Crit), MISS ≥3600           | HIT ≤3100 Zone 1, MISS ≥3100             |
|                          | W8 EBEN   | HIT alle, Zone 1 (Crit <3000, sonst 200)   | HIT alle, Zone 1                         |
|                          | W8 TIEF   | HIT alle, Zone 0                           | HIT ≤3600; **MISS 5500/9000** x          |
| **KRIECHER (1)**         | W3 HOCH   | MISS alle                                  | MISS alle                                |
|                          | **W3 EBEN** | **HIT alle (16 Schaden, Zone 0)**        | **MISS alle** x5                         |
|                          | W3 TIEF   | HIT alle                                   | HIT ≤3600 Zone 0; MISS 5500/9000 x2      |
|                          | W8 HOCH   | HIT 1500/2500 (Crit), MISS ≥3600           | HIT 1500/2500 Zone 0 (Beine!)            |
|                          | **W8 EBEN** | **HIT alle (Crit/200, Zone 0)**          | **MISS alle** x5                         |
|                          | W8 TIEF   | HIT alle                                   | HIT ≤3600; MISS 5500/9000 x2             |
| LIEGEND-P3 (1)           | W3/W8 EBEN| MISS alle                                  | MISS alle                                |
|                          | W3/W8 TIEF| HIT ≤3600 Zone 0, MISS ≥5500               | HIT ≤3600 Zone 0, MISS ≥5500             |
|                          | W8 HOCH   | MISS                                       | HIT 1500/2500 Zone 0 x2                  |
| **AUFSTEHEN-P6 (1)**     | **W3/W8 EBEN** | **HIT alle**                          | **MISS alle** x10                        |
|                          | W3/W8 TIEF| HIT 1500/2500, **MISS 3600** (RE1.5-Ring)  | HIT ≤3600 x2                             |
|                          | W8 HOCH   | MISS                                       | HIT 1500/2500 Zone 0 x2                  |
| LIEGESPAWN 0x88 (3)      | TIEF      | HIT ≤3600 (hp 79 → 68 / Crit)              | **kein Ziel** (HP=-1, +0x1D3=0x80) x6    |
|                          | HOCH/EBEN | MISS                                       | kein Ziel                                |

40 Divergenzen; die 20 fett gedruckten sind exakt der Nutzer-Befund („gerade schiessen trifft
Zombies am Boden"): Kriecher und Aufsteher. Logauszug (`trefferhoehe_sonde.log`):

```
--- Lage KRIECHER: slot 2 st=1/0/1/0 grid=00 f10e=2001 21a=0001 1d3=00 93=00 y=0 hp=3800 box r=400 h=1440 ofs_y=-1440 | RE2-Maske=1 ---
  KRIECHER   W3 EBEN d=1500 | IST HIT  dhp=  16 1d2=0 zeile=3 st=2 hnd=3 | SOLL(RE2) MISS             <-- DIVERGENZ
  KRIECHER   W8 EBEN d=3600 | IST HIT  dhp= 200 1d2=0 zeile=7 st=2 hnd=3 | SOLL(RE2) MISS             <-- DIVERGENZ
  KRIECHER   W3 TIEF d=2500 | IST HIT  dhp=  16 1d2=0 zeile=3 st=2 hnd=3 | SOLL(RE2) HIT zone=0 br=0
--- Lage AUFSTEHEN-P6: slot 2 st=1/5/6/0 grid=80 f10e=2000 21a=0203 1d3=00 ...              | RE2-Maske=1 ---
  AUFSTEHEN-P6 W3 EBEN d=1500 | IST HIT  dhp=  16 1d2=0 zeile=3 st=2 hnd=3 | SOLL(RE2) MISS          <-- DIVERGENZ
  AUFSTEHEN-P6 W3 TIEF d=3600 | IST MISS dhp=   0 1d2=1 zeile=5 st=1 hnd=3 | SOLL(RE2) HIT zone=0 br=0 <-- DIVERGENZ
```

Gemessene Boxen: Spieler r=450 h=1530 ofs_y=-1530 (PSX.EXE @0x80073e94); Port-Zombie r=400 h=1440
(STAGE1.BIN @0x8011f778) — die RE1.5-Box, auch fuer RE2-owned Zombies (RE2 haette 500/-1500/+1500
stehend, 200/-350/+350 liegend, §2.3).

## 2. Original-Mechanismus

### 2.1 RE1.5 — FUN_80011F50 (PSX.EXE): Zielhoehe = Bandschnitt, keine Zonen

Zielhoehe des Spielers: `DAT_800acaec` Bit 0x8000 HOCH / 0x4000 EBEN / 0x2000 TIEF (exklusiv,
@0x8003322c-38 / @0x800332c0-c8 / @0x80033274-84), in FUN_80011F50 als
`DAT_800aca54 = acaec<<16 | aca54 & 0x1fffffff` (Bit31 UP / 30 LEVEL / 29 DOWN).

Kandidatengate (der einzige Hoehentest vor der Auswahl):
```
800120d0  lw  v0,0(s0)            ; enemy word0 (Bits 29-31 = gestempeltes Band)
800120d8  lw  v1,DAT_800aca54     ; player word (Bits 29-31 = Zielhoehe)
800120e0  and v0,v0,v1
800120e4  lui v1,0xe000
800120e8  and v0,v0,v1
800120ec  beq v0,zero,LAB_80012128 ; kein gemeinsames Band -> Kandidat verworfen
```
Das Gegner-Band stempelt der ACTIVE-Tail des Zombies (STAGE1.BIN @0x801015c0-40): immer
`jal 0x80012aa4 / a0=0xbb8` (s0=(playerY-enemyY)/1800; |s0|<2 → LEVEL; Ring → UP/DOWN), bei
`+0x9 & 0x80` (liegend/downed) danach `word0 &= ~0x40000000` + `jal 0x80012974 / a0=0x1388`
(DOWN, wenn dist<5000) @0x80101614-3c. **RE1.5-Liegende sind also nur mit TIEF (<5000) treffbar,
bodengleich Stehende mit EBEN und TIEF (<3000).**

Nach der Auswahl (@0x80012160ff): Typ<0x40 (@0x80012178-84), dann der Y-Test
```
80012194  lw  v1,56(s1)           ; enemy y (+0x38)
80012198  lw  a0,DAT_800aca8c     ; player y
800121a0  subu v0,v1,a0
800121a4  bgtz v0 -> 800121b8 ; slti v0,v0,501   (|dy| < 0x1F5 -> direkt LAB_80012370 = Treffer)
800121c0  jal 0x8001bafc          ; sonst Sichtlinie/Neigung: FUN_8001bf04-Sektor, 0x9c4/0xd5c-
                                  ; Schraegen-Rechnung @0x800122d4-368 (bgtz/mult/slt)
```
Der vertikale „Zonen"-Wert +0x6 (Richtung fuer den Hurt-Clip, KEINE Koerperzone):
```
80012430  lw  v0,player_word      80012438  srl v0,v0,29
80012440  addiu at,at,-3056       ; DAT_8006f410 = [07 00 01 07 02 00 00 00]
80012448  lbu v0,0(at)            80012450  sb v0,6(s1)     ; DOWN->0, LEVEL->1, UP->2
80012458-7c  playerY < enemyY && +0x6==0 -> +0x6=1
80012480-ac  enemyY < playerY && +0x6==2 -> +0x6=1
```
Die Box +0x78 wird von RE1.5-Liege-Handlern NICHT veraendert: Vollscan STAGE1.BIN nach
`sw/sh/sb …,0x78/0x7a/0x7c/0x7e(rX)` = 20 Treffer, davon 18 die per-Typ-INIT-Installation
`sw v0,0x78(v1)` (0x80100778, 0x8010ac34, … 0x8011e3b8), 2 Stack-Saves (`0x7c/0x78(sp)`
@0x80107ef4/ef8) und ein Yaw-Flip `+0x7c += 0x800` @0x801087e8-808 (kein Boxfeld). Die Reichweiten-
Baenke @0x8006e5a0 (+0x58) sind byte-identisch (kopfschuss-nah.md §2.4). **RE1.5 kennt weder Zonen
noch eine Liege-Box — nur das Band.** (Fuer RE1.5-owned Zombies ist der Port hier byte-true.)

### 2.2 RE2 — Schuss-Applier FUN_800410CC (info/re2leon/PSX.EXE)

Aufruf aus FUN_80047C6C: `srl s2,v0,29` @0x80047d18 (Zielhoehe-Bits des Spieler-word0),
`lbu a2,0x800a6f8c(at)` @0x80047ee0 (Dump `[FF 00 01 FF 02 00 00 00]`: DOWN→Gruppe 0,
LEVEL→1, UP→2), `a2 = 0x800A68E8 + item*24 + grp*8` @0x80047ecc-f8, `jal 0x800410cc` @0x80047ef4.
Geometrie-Zeiger (selbst gelesen): Item 7 (Schrot) @0x800A6990 = DOWN 0x800A6724 / LEVEL
0x800A6740 / UP 0x800A675C (Keyframes 0x800A6778); Item 3 (Pistole) @0x800A6930 = 0x800A6618 /
0x800A6634 / 0x800A6650; Item 1 (Messer) @0x800A6900 = 0x800A657C / 0x800A6348 / 0x800A6480.

Kandidaten-Gates (RE2_Quellcode_V2/FUN_800410cc.c, deckungsgleich FUN_800470C0 @0x8004712c/38/48/60):
`word0&1` aktiv, `+0x1D3 == 0`, `+0x156 (HP) >= 0`, `+0x10E & 0xC000 == 0`. **Kein Hoehenband.**

Kern je Kandidat (Decompile-Zeilen, Feldnamen nach RE2-Entity):
```
uVar5 = *puVar9 >> 0x1a & 7;                 ; TEILE-MASKE Bits 26..28 (1=Beine,2=Rumpf,4=Kopf)
iVar3 = puVar9[0xf] - player[+0x3c];         ; dy = enemyY - playerY (PSX: negativ = oben)
if (uVar5 != 0) {                            ; OHNE Maske: kein Treffer moeglich
  fuer Sub-Box b in 0..2 (Flag rec[1+b], Box rec+4+8b):
    third = FUN_80041b20(dmgrec, flag, &lo, &hi)     ; Fenster + Drittel (negativ)
    if ((uint)(dy-lo) <= (uint)(hi-lo))              ; dy im Fenster
      row = 6; if (hi+2*third <= dy) row = 3; if (hi+third <= dy) row = 0;
      if (FUN_80041ce4(player+0x10, enemy+0x10, box)) ; XZ-Sub-Box
        pb = DAT_800a6db4 + row + ((flag&8)?9:0)
        pb[2]&mask -> part=pb[2]>>1 ; pb[1]&mask -> part=pb[1]>>1 ; pb[0]&mask -> part=pb[0]>>1
        (letzter Treffer gewinnt = pb[0] hat Vorrang) ; uVar7 = part*8 + (1<<b)
```
Naechster Kandidat (SquareRoot0-Distanz) gewinnt; Schaden `(rec.word0 >> bracket*10) & 0x3ff`
(bracket aus `uVar7&2→1, &4→2`), Poise `+0x153/+0x152/+0x151` fuer Teil 0/1/2, `+0x1D2 = Teil +
3*Klammer` (@0x800413CC-D4, kopfschuss-nah.md §3.3).

Tabellen (selbst gedumpt 2026-09-19):
```
DAT_800a6db4: 04 02 01 | 02 01 04 | 01 02 04 || +9: 04 02 00 | 02 00 00 | 01 02 00
              row0(nah hi)  row3(Mitte)  row6(nah lo)      Satz fuer Flag&8 (LEVEL-Records)
Zombie-Record @0x800A412C+(id-1)*20, +8/+0xC/+0x10 = UP/LEVEL/DOWN (lo,hi):
  id3 Pistole @0x800A4154: UP[-5000,-2000] LEVEL[-3000,2000] DOWN[-500,3000]
  id7 Schrot  @0x800A41A4: UP[-5000, +500] LEVEL[-3000,2000] DOWN[-500,3000]
Geometrie-Records (0x1C B: 00, Flag0, Flag1, Flag2, dann 3 x {start, 0, depth/4, halfw/4}):
  Schrot  DOWN @0x800A6724: 04 02 01 | {100,0,1000,500} {4100,0,1000,750} {8100,0,2500,1000}
          LEVEL@0x800A6740: 08 08 08 | {200,0,1000,625} {4200,0,1000,875} {8200,0,7500,1125}
          UP   @0x800A675C: 10 20 40 | {100,0,750,500}  {3100,0,1500,750} {9100,0,2500,1000}
  Pistole DOWN @0x800A6618: 04 02 01 | {100,0,1000,375} {4100,0,1000,500} {8100,0,2500,625}
          LEVEL@0x800A6634: 08 08 08 | {200,0,1000,375} {4200,0,1000,500} {8200,0,7500,625}
          UP   @0x800A6650: 10 20 40 | {100,0,1000,375} {4100,0,1000,500} {8200,0,2500,625}
```
FUN_80041B20 (RE2_Quellcode_V2/FUN_80041b20.c): Flag 0x04 = DOWN-Paar unverschoben, 0x02 = um ein
Drittel, 0x01 = um zwei Drittel nach unten (dy positiver) verschoben; 0x08 = LEVEL-Paar; 0x10 = UP
unverschoben, 0x20/0x40 = ein/zwei Drittel nach oben; Rueckgabe `third = (lo-hi)/3` (negativ).

### 2.3 RE2 — die Teile-Maske wird beim Hinlegen/Kriechen umgeschaltet (EMOVL10_S0.BIN)

INIT (stehend): `lui v1,0xc00 / or v0,v0,v1 / sw v0,0(s2)` @0x80100984-998 → word0 |= 0x0C000000
(Beine+Rumpf; KEIN Kopf-Bit — Kopfzone am Zombie original-unerreichbar, kopfschuss-nah.md). Dazu die
Steh-Box `+0x98 = -1500` @0x80100958-5c, `+0x9E = 1500` @0x80100960-64, Radius `+0x9A/+0x9C = 500`
@0x8010096c-74.

Vollscan aller `lui` mit 0xf3ff/0x0c00/0x0400 im Overlay (53068 B) — **NUR BEINE**
(`word0 = (word0 & 0xF3FFFFFF) | 0x04000000`), jeweils mit Liege-Box r=200/+0x98=-350/+0x9E=+350:
| Stelle | Handler | Instruktionen |
|---|---|---|
| @0x801032B8-EC | EXEC[5] Knockdown P0 (Port re2z_exec_knockdown:1987) | `lui a2,0xf3ff / ori a2,0xffff / and v1,v1,a2 / lui v0,0x400 / or v1,v1,v0` (+ `ori 0x202` +0x21A) |
| @0x80104068-AC | EXEC[8] Getup P0 (Port :2704) | `lui a2,0xf3ff … lui v0,0x400 / or / sw v1,0(s1)` |
| @0x8010441C-54 | EXEC[11] P0 (Port :2789/2894) | `lui a0,0xf3ff / … / and v1,v1,a0 / lui a0,0x400 / or / sw` |
| @0x80106AE0/B38-50 | Ragdoll 0x801066FC P2 = Kriecher-Eintritt (Port :5938) | `lui t1,0xf3ff` … `lui v0,0x400 / and v1,v1,t1 / or v1,v1,v0 / sw v1,0(a0)`; davor `sh 350,158` |
| @0x801077D8/0x80107828-38 | Knockdown 0x80107438 P2 = Kriecher (Port :6290) | `addiu v1,8193 / sh v1,270` (+0x10E=0x2001) `lui v1,0x400 / and v0,v0,a0 / or / sw v0,0(s2)` |
| @0x80100AF0-B44 | INIT-Variante Kriecher (`+0x10E & 1`) | Box 200/-350/350 @0x80100B00-20, `ori 0x2000`, `and a0,a0,v1 / lui v0,0x400 / or / sw` |
| @0x80100BB4-C0C | INIT-Variante `sw 0xF01,4` (Liege-/Sitz-Spawn) | `lui t0,0xf3ff … and v1,v1,t0 / lui t0,0x400` |
| @0x80102BE8-C20 | Tod → Leiche `sw 7,4` (Port :1858 „OPEN") | `lui a0,0xf3ff … and v1,v1,a0 / lui a0,0x400 / or / sw` |
| @0x80102D38-88 | Todeszweig (Clip-Wort 0xA0A02 + 0x2000·n) | dito |
| @0x80108974-BC | Leiche → Kriecher (`sh 1,342` HP=1, +0x10E=0x2001) | dito |

**Rueckbau auf Beine+Rumpf** (`lui rX,0xc00 / or / sw …,0(sY)`), jeweils mit Steh-Box 500/-1500/1500:
| Stelle | Handler | Bedingung |
|---|---|---|
| @0x801036D0-F0 | EXEC[5] P7 (Aufsteh-Clip laeuft) | `lbu v1,333(s2) / addiu v0,55 / bne` → **nur bei Clip-Bild +0x14D == 55** |
| @0x801036F4-738 | EXEC[5] P8 `sw 0x101,4` (er steht) | unbedingt, mit `+0x1D3 &= 0x7f` @0x80103718-28 |
| @0x80103900-2C | EXEC[7] Liege-Spawn P4 `sw 0x101,4` (Port :2481) | `lui a0,0xc00` @0x80103908, `or/sw` @0x80103928-2C |
| @0x80107E70-B0 | Aufsteh-Tail hinter dem Knockdown-Handler (Zweige @0x80107D14/@0x80107DCC; Port :7416 „not RE'd") | Box 500/-1500/1500, `andi 0xffed`, `lui v1,0xc00 / or / sw` |
| @0x801049F0 | Voll-Re-INIT (`lui a0,0xc00 / ori a0,a0,0x1`, HP aus 0x8010C600) | Neuaufbau |

Damit gilt in RE2 fuer bodengleiche Zombies (dy = 0) — hergeleitet aus §2.2 + §2.3, in der Sonde
als SOLL nachgerechnet:
| Lage (Maske) | HOCH Pistole | HOCH Schrot | EBEN | TIEF |
|---|---|---|---|---|
| stehend (Beine+Rumpf) | MISS (UP-Fenster [-5000,-2000] schliesst 0 aus) | HIT Rumpf nur Nah-Box <3100 | HIT **Rumpf** (row3 Satz1 `[02 00 00]`), Klammer nach Box 200/4200/8200 | HIT **Beine** nur Nah-Box <4100 (row6 `[01 02 04]`, Beine zuerst); ab 4100 Fenster um 1166 verschoben → MISS |
| liegend/kriechend/aufstehend bis Bild 55 (NUR Beine) | MISS | HIT **Beine** <3100 (row0 `[04 02 01]` enthaelt Beine) | **MISS** (`[02 00 00]` & 1 = 0) | HIT **Beine** <4100 |
| Liege-Spawn 0x88 (EXEC[7]) | kein Ziel | kein Ziel | kein Ziel | kein Ziel (HP=-1 @0x80100A3C-40 `addiu v0,-1 / sh v0,342`; +0x1D3\|=0x80 @0x80103804-14; Wecker @0x80104EE0-F1C setzt HP NICHT) |

Der gleiche Maskenwechsel betrifft das Messer (Item 1, Fenster id 1: LEVEL[-1900,1000] → dy=0 =
row3 → nur Rumpf; DOWN[-300,2500] → row6 → Beine) — es laeuft durch denselben Applier.
Die Liege-Box +0x98/+0x9E (-350/+350 statt -1500/+1500) speist NICHT den Schuss-Applier, sondern
den Angriffs-Volumen-Applier FUN_800470C0 (`(y + +0x98 + 100 + +0x9E) - atkY < (+0x9E+100)*2`,
Zone `y + +0x98/2 < atkY → 0`, Kopf-Bit && `atkY < y + 1.5·+0x98 → 2` @0x800472AC-30C) — das ist
die Zonenrechnung aus dem Nutzer-Kontext (FUN_80047294-334), sie gilt fuer Angriffs-Volumen
(Gegner→Spieler, Skript-Waffen), nicht fuer den Pistolen-/Schrot-Schuss.

### 2.4 Sub-Box-Distanzen: Viertel-Skalierung von FUN_80041CE4 (hergeleitet)

FUN_80041CE4 (RE2_Quellcode_V2) baut `local_60.t = (b0, 0, -4·b3 - b1)`, komponiert mit der
Spieler-Matrix (+0x24, FUN_8002CE94 = gte-CompMatrix: `t = R·t2 + T`) und nimmt `t>>2`; die Kanten
`R·(b2,0,0)` und `R·(0,0,2·b3)` bleiben UNskaliert, die Gegnerposition `+0x38/+0x40 >> 2`. Ein
Kanten-Wert zaehlt also 4 Welt-Einheiten: Box = Streifen ab `b0` (Welt), Tiefe `4·b2`, halbe
Breite `4·b3`. Beleg = Luecken-Freiheit ALLER sechs Records: 100+750·4=3100, 3100+1500·4=9100
(Schrot UP), 100+1000·4=4100, 4100+1000·4=8100 (DOWN), 200+1000·4=4200 → 8200 (LEVEL), Pistole UP
100→4100→8200; die Radius-Erweiterung `+0x9A>>2` auf b3 und `+0x1EE>>2` auf b2 der Nah-Box
(FUN_800410CC) passt zur /4-Skala. Nicht per Savestate gemessen (§5).

## 3. Port-Ist (re15_port/engine/src/re15_damage.c, Stand 6692c9cd)

* `:1344` `pband` aus `re15_player_aim_elevation()` (exklusiv UP/LEVEL/DOWN) — byte-true RE1.5.
* `:1449-1455` **`re2_rising`** = RE2-owned && state 1 && (sub1==5 && sub2∈{6,7} || sub1==8 &&
  sub2∈{3,4}) → weder „lying" noch Fenster → **faellt auf das RE1.5-aa4-Band** (LEVEL + DOWN nur
  im 3000er-Ring). Gemessen: Aufsteher P6 mit EBEN treffbar (10 Divergenzen), TIEF bei 3600 NICHT
  (RE2: bis 4100 treffbar). Im Original ist dieser Zustand durch die Maske geregelt (nur Beine bis
  Clip-Bild 55 @0x801036D8), nicht durch ein Band.
* `:1457-1466` **`lying`** = `grid&0x80 || (RE2-owned && 21A&2 && !(f10e&1))` → LEVEL weg, DOWN
  wenn dist<5000 (= die RE1.5-Regel @0x80101624-3c/@0x800129cc-f0, auf RE2-owned uebertragen).
  Der **Kriecher ist ausdruecklich ausgenommen** (`!(e->re2z_f10e & 1u)`, Begruendung im Kommentar:
  „RE2s Kandidatenfilter hat kein Hoehenband … Ein RE2-Kriecher ist dort IMMER Kandidat") — das
  ist richtig fuer die GATES, uebersieht aber die Maske im Applier. Folge: der Kriecher laeuft in
  den Steh-Zweig.
* `:1467-1527` `re2_fenster` fuer RE2-owned, nicht liegend, nicht rising: dy-Fenster aus `FEN[rid]`
  (Werte korrekt, §2.2), Drittel-Verschiebung **nur fuer UP** und mit den Schrot-Boxen 3100/9100
  fuer alle Waffen (Pistole: 4100/8200 — bei dy=0 folgenlos, weil das Pistolen-UP-Fenster 0
  ausschliesst); **DOWN ohne Sub-Box-Verschiebung** (RE2: ab 4100 MISS) und **ohne Teile-Maske**.
  `:1670-1676` der Fenster-Test `fdy < lo || fdy > hi → continue`.
* `:1996-1999` Zone: `re2z_hits1d2 = (liegt || elev < 0) ? 0 : 1` mit
  `liegt = (f10e&1) || (21A&2)` — Zone aus der Zielhoehe, Klammer immer 0; `+0x6 = 0`.
* `re15_re2z_hit_filter_apply` (enemy_ai_re2_zombie.c:8184-8228): Gates aktiv/+0x1D3/HP/+0x10E als
  +0x93-Bit-0-Neuberechnung — mit der bewussten Port-Ausnahme `spawn_pose` (EXEC[7]/[8] treffbar,
  Nutzer-Auftrag 2026-08-27). Der Liege-Spawn hat im Port hp=79 (RDT) statt RE2 `-1`
  (@0x80100A3C-40) — Thema der Nachbar-Sonde `probe_r16_liegende_zombies`.
* **Kein Feld fuer word0 Bits 26-28:** `re15_actor_t.flags` ist `uint8_t` = nur das niedrigste Byte
  (re15_actor.h:43). Die sieben Schalt-Stellen und vier Rueckbau-Stellen stehen im Port als
  Kommentar/„OPEN": enemy_ai_re2_zombie.c:1858 (Tod), :2704 (EXEC[8] P0), :2789+2894 (EXEC[11] P0),
  :5732+5911 (Ragdoll P2), :6167+6290 (Knockdown P2), :1987 (EXEC[5] P0, dort nicht erwaehnt),
  :2158-2200 (P7/P8), :2480 (EXEC[7] P4).
* Warum der Nutzer es sieht: Kriecher (haeufigster Boden-Zustand nach Beinschuss/Ragdoll) und
  Aufsteher werden mit EBEN getroffen (Pistole 16, Schrot 200 bzw. Crit) und stempeln dabei Zone 0
  — im Original sind beide fuer EBEN unsichtbar, nur TIEF (Beine) und Schrot-HOCH nah (Beine)
  treffen.

## 4. Fix-Plan (Phase 2)

Reihenfolge = Abhaengigkeit. Jede Konstante traegt ihre Adresse aus §2.

1. **Feld `uint8_t re2z_parts`** in `re15_actor_t` (re15_actor.h, RE2-Gruppe neben `re2z_self1d3`
   :248): Bit0 Beine (0x04000000), Bit1 Rumpf (0x08000000), Bit2 Kopf (0x10000000) = word0>>26&7.
   Reset 0 in `re15_enemy_reset`/Spawn.
   Setzen (enemy_ai_re2_zombie.c):
   * INIT `= 3` (@0x80100984-998), im RE2-INIT (Funktion um :7660, dort wo `re2z_prev_hp` gesetzt
     wird); Kriecher-INIT-Variante `= 1` (@0x80100B38-44; Port-Zweig `re15_re2z_enter_crawler`
     :7696ff) und `sw 0xF01`-Variante `= 1` (@0x80100C0C).
   * `= 1` in: EXEC[5] P0 (:1987ff, @0x801032E8), EXEC[8] P0 (:2704, @0x80104098), EXEC[11] P0
     (:2894, @0x80104444), Ragdoll P2 (:5938, @0x80106B38), Knockdown P2 (:6290, @0x80107828),
     Tod→Leiche (:1858, @0x80102C10) und Todeszweig (@0x80102D80), Leiche→Kriecher (@0x801089B4,
     falls der Port den Pfad fuehrt — `re15_re2z_enter_crawler`).
   * `= 3` in: EXEC[5] P7 bei Clip-Bild 55 (:2158ff; `+0x14D == 55` @0x801036D4-D8 — im Port die
     Aktor-Clip-Frame-Quelle nutzen, die `re2z_clip_done` liest), EXEC[5] P8 (:2186ff, @0x80103730),
     EXEC[7] P4 (:2481, @0x80103908), Knockdown-Aufsteh-Tail (@0x80107EA8 — Port-Zwilling erst
     RE'en, s. §5; bis dahin an der Stelle setzen, an der der Port `0x101` nach dem Knockdown
     committet), Voll-Re-INIT (@0x801049F0).
2. **RE2-Applier in re15_damage.c** als Funktion `re15_re2_gun_probe(rid, elev, dist, dy, parts,
   int *zone, int *bracket)` mit den Tabellen aus §2.2: `FEN[20][6]` (vorhanden :1476ff),
   Geometrie-Records je RE2-Id (mindestens 3 Pistole @0x800A6618/34/50 und 7 Schrot
   @0x800A6724/40/5C; fuer die uebrigen Hitscan-Ids 2/4/5/6/8/13/15/18/19 die Zeiger @0x800A68E8 +
   id*24 dumpen — nicht raten), `PRIO[18]` (DAT_800A6DB4), Fenster/Drittel wie FUN_80041B20,
   Zeile `row = 6/3/0` wie @FUN_800410CC, Teil per `pb[2]→pb[1]→pb[0]` (letzter gewinnt), Sub-Box
   nach Distanz `start[b] <= dist < start[b] + 4*depth4[b]` (§2.4; Radius-Erweiterung `+0x1EE>>2`
   offen). Rueckgabe 0 = kein Treffer.
   Im Kandidaten-Block (:1449-1527, :1670-1676) fuer `re2_owned` Zombie-Familie: `re2_rising`,
   `lying` (nur der RE2-Term) und `re2_fenster` durch EINEN Aufruf ersetzen (`dy = e->y - pl->y`,
   `dist = bdist`, `parts = e->re2z_parts`); Ergebnis in Lokale fuer den Stempel merken. RE1.5-owned
   (Flavor RE15/MIXED) und `grid&0x80`-Regel fuer Nicht-RE2 unveraendert (byte-true RE1.5, §2.1).
   Gates HP<0 / +0x1D3 bleiben im Pausenfilter (`re15_re2z_hit_filter_apply`), NICHT in die
   Schleife (Fixpunkt-Falle, Kommentar :1316-1331).
3. **Stempel** `re15_re2_stamp_hit` (:1996-1999): `re2z_hits1d2 = zone + 3*bracket` aus dem
   Applier-Ergebnis (`+0x1D2 = Teil + 3·Klammer` @0x800413CC-D4) statt `(liegt||elev<0)?0:1`;
   Spalten-Klemme (`re2z_row_guard`) bleibt. Damit tragen Fern-Treffer erstmals Klammer 1/2 (=
   die 60/40-Schadensspalten und die Tabellenspalten 3..8 @0x8010C940) — vorher prueft der
   Tabellen-Dump, welche Zellen fuer Zeile 3 (Pistole) und 7 (Schrot) in Spalte 3/4/6/7 belegt sind
   (kopfschuss-nah.md §3.4 hat nur Spalte 2/5/8 als NULL nachgewiesen).
4. **Pin** `probe_r16_trefferhoehe pin` (ctest `unit_r16_trefferhoehe_pin`, Registrierung in
   `probes/r16_trefferhoehe.cmake`): Zusicherungen aus §1-SOLL: STEHEND EBEN → HIT Zone 1 (alle
   d), TIEF → HIT Zone 0 nur d≤3600; KRIECHER EBEN → 0 Treffer (W3+W8, alle d), TIEF → HIT Zone 0
   d≤3600, W8 HOCH 1500/2500 → HIT Zone 0; LIEGEND-P3 EBEN → 0; AUFSTEHEN-P6 EBEN → 0, TIEF 3600
   → HIT. Bestehende Pins, die gruen bleiben muessen: `unit_re2z_bandlock_pin` (misst LEVEL-
   Treffer auf AUFRECHTE Zombies mit haengenden Liege-Bits — nach dem Fix entscheidet die Maske,
   nicht 21A&2; Fixture ggf. neu verankern, Memory reai-v2-pin-fixture-verschiebung),
   `unit_re2_weapon_rows`, `unit_re2_hp_model`, `unit_re2_hit_repeat`, `unit_re2z_pushoff_crawl`,
   `unit_re2_crawler`, `unit_schrot_trennung` (probe_schrot_bauch_y feuert EBEN aus 3600 auf einen
   Stehenden — bleibt Treffer), `probe_spinne_elevation` (Spinnenzweig unberuehrt).
5. **Risiken / sichtbare Aenderungen:** (a) TIEF auf Stehende trifft nur noch <4100 (RE2-Byte-
   Stand, §2.2/2.4) — dem Nutzer nennen; (b) Aufsteher sind mit EBEN erst ab Clip-Bild 55 von P7
   treffbar, mit TIEF durchgehend — das erfuellt den Auftrag vom 2026-08-27 („bereits anschiessen")
   auf dem RE2-Weg; (c) der `re2_rising`-Sonderfall entfaellt ersatzlos; (d) Liege-Spawn:
   RE2-Bytes sagen „kein Ziel bis zum Aufstehen" (HP=-1, +0x1D3=0x80) — Entscheidung mit der
   Nachbar-Sonde `liegende-zombies` abstimmen, hier NICHT aendern; (e) MIXED-Flavor: Zombies sind
   RE1.5-owned → unveraendert.
6. **Doku**: die „OPEN"-Kommentare an den elf Stellen auf das neue Feld umschreiben; :1460-Kommentar
   („IMMER Kandidat") um „…aber Maske = nur Beine" ergaenzen.

## 5. Offen / nicht belegt

* **Sub-Box-Achse/Skala (§2.4)** ist aus Luecken-Freiheit und der /4-Radiuserweiterung hergeleitet,
  nicht per Savestate/PCSX gemessen; die Vorwaerts-Achse der Matrix +0x24 (lokal X) ist nicht
  einzeln belegt. Die Radius-Erweiterung `+0x1EE>>2` (Nah-Box-Tiefe) und `+0x9A>>2` (Breite) ist
  im Fix-Plan noch nicht modelliert (Port hat +0x9A nicht als Feld).
* Bit→Teil-Zuordnung der Maske (1 Beine / 2 Rumpf / 4 Kopf) ist konsumentenseitig belegt
  (Poise `+0x153/+0x152/+0x151`, Kopf-Bit `0x10000000` @0x800472E0), nicht ueber einen Namen.
* Eigentuemer des Aufsteh-Tails @0x80107E70-EC0 (Zweige @0x80107D14/@0x80107DCC) im Port nicht RE'd
  (:7416 „not RE'd").
* Liege-Spawn-Lebenslauf (HP=-1 am INIT, Wecker ohne HP-Schreiber; 19 HP-Schreiber im Overlay
  gescannt, keiner hebt -1 auf ausser dem Voll-Re-INIT @0x801049C8 aus 0x8010C600) — gehoert zur
  Nachbar-Sonde `probe_r16_liegende_zombies`.
* Messer (RE2-Id 1) und die uebrigen Hitscan-Ids: Geometrie-Records nur als Zeiger gelesen
  (@0x800A6900/…), Inhalt nicht gedumpt.
* Keine RE1.5-Flavor-Messung (dort ist das Band byte-true, §2.1); keine DuckStation-Gegenprobe
  (die RE2-Seite ist statisch vollstaendig, RE2 laeuft hier nicht im Emulator).
