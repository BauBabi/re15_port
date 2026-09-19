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

## 6. Umsetzung (Phase 2, 2026-09-19)

Branch `worktree-wf_074e2f88-24e-2` (auf aaf06c9b), Build `re15_port/build_p2`.
Commits: `182453a0 fix(gegner): RE2-Trefferzonen - Teile-Maske und Schuss-Applier FUN_800410CC
fuer RE2-Zombies`, `e34f6094 test(gegner): Pin unit_r16_trefferhoehe_pin und Bandlock-Pin auf den
RE2-Applier verankert`. Die Skeptiker-Korrekturen (1)-(7) gelten und sind unten je Punkt belegt.

### 6.1 Was gebaut wurde

* **`re15_actor_t.re2z_parts`** (word0>>26&7) und **`re2z_rad9a`** (+0x9A) — re15_actor.h, jeder
  Setzer mit Adresse im Feld-Kommentar. +0x1EE = 500 ist im Overlay ein EINZIGER Schreiber
  (INIT `sh v1,494(s2)` @0x80100980, eigener sh-Scan `re15_port/tools/re2_zombie_mask_scan.py`)
  und deshalb Konstante `RE2Z_RAD1EE` in re15_damage.c, kein Feld.
* **Setzer in enemy_ai_re2_zombie.c** (Vollscan aller `lui 0xc00/0x400/0xf3ff` + `sw ...,0(rX)`:
  16 Maskenschreiber, davon 2 Bindeflag-Argumente @0x8010AAE8/@0x8010B048 = SE-Aufrufe, kein
  word0):
  | Maske | Stelle | Port |
  |---|---|---|
  | 3 | INIT @0x80100984-998 (+0x9A 500 @0x8010096C-70) | re2z_init |
  | 1 | Kriecher-INIT @0x80100B38-44 (+0x9A 200 @0x80100B00-04) | re15_re2z_enter_crawler |
  | 1 | EXEC[5] P0 @0x801032E8-FC | re2z_exec_knockdown case 0 |
  | – | EXEC[5] P1 +0x9A -= 10 solange >= 21 @0x80103388-A0 | case 1 |
  | – | EXEC[5] P2 +0x9A = 0 @0x80103478 | case 2 |
  | – | EXEC[5] P7 +0x9A += 10 bis 500 @0x80103628-3C | case 7 |
  | 3 | EXEC[5] P7 NUR bei +0x14D == 55 @0x801036D0-F0 | case 7, `re2z_frame_slot(e) == 55` |
  | 3 | EXEC[5] P8 @0x80103730-38 (+0x9A 500 @0x801036FC-700) | default |
  | 3 | EXEC[7] P4 @0x80103908-2C | re2z_exec_lying default |
  | 1 | EXEC[9] Sturzzweig @0x80104098-AC | re2z_exec_getup (Zeile ~2704, war OPEN) |
  | 1 | EXEC[11] P0 @0x80104444-54 | re2z_exec_eleven |
  | 1 | Ragdoll P2 @0x80106B38-50 (+0x9A 200 @0x80106B14-18) | re2z_hit_ragdoll default |
  | 1 | Knockdown P2 @0x80107828-38 (+0x9A 200 @0x801077F8-FC) | Kriecher-Ausgang |
  | 1 | Tod->Leiche @0x80102C10-20 | re2z_death_main (war OPEN) |
  | 1 | Todeszweig @0x80102D80-98 | Zeile ~1899 |
  | 3 | Re-INIT @0x801049F0 (+0x9A 500 @0x801049FC-A00) | re2z_exec_restyle |
  | \|=1 | Root @0x8010039C-A8 (+0x10E&1 \|\| +0x21A&2) | re15_re2z_tick Prolog |
  Ohne Port-Zwilling (dokumentiert, nicht erfunden): INIT-Variante `sw 0xF01` @0x80100C0C-1C
  (kein Port-Zustand 0xF01), Leiche->Kriecher @0x801089B4-C4, Kriech-Umbau-Tail
  FUN_80107A78 @0x80107EA8-B0 (Port ruft den Handler nicht, enemy_ai_re2_zombie.c "OPEN
  (unveraendert): +0x21A & 0x10 -> Kriecher-Umbau").
  **Skeptiker (2)**: EXEC[8] (Fresser) setzt NICHT auf 1 — Exit `sw 0x101` @0x80103D94 ohne
  Maskenwechsel; der Pin prueft nach dem Verlassen der Fresser-Pose `Maske == 3`.
  **Skeptiker (3) geklaert**: der EXEC[9]-Sturzzweig committet `sw v0,4(s1)` @0x80104028
  (Delay-Slot des `jal 0x80015fe8` @0x80104024, v0 = 1281 = 0x501) VOR dem Maskenwechsel
  @0x80104068-AC und `sb 1,6` @0x80104084 — er endet in EXEC[5] P1, der Rueckbau ist P7/P8.
  EXEC[9]s eigener `sw 0x101` @0x80104148 (P2 @0x80104144) ist nur aus dem Nicht-Sturz-Zweig
  @0x8010411C-40 erreichbar (`beq s0,zero,0x8010411c` @0x8010401C), der die Maske nicht anfasst.
* **Applier `re15_re2_gun_probe`** (re15_damage.c, Block am Funktionskopf): Gruppe aus
  DAT_800A6F8C, Geometrie-Records fuer die Hitscan-Ids 2/3/4/5/6/7/8/13/15/18/19 aus den
  Zeigern @0x800A68E8 + item*24 + grp*8 (**alle selbst gedumpt**, `re15_port/tools/
  re2_gun_tables_dump.py`; Pistole 2/3/4/13/19 -> @0x800A6618/6634/6650, 15 -> @0x800A6848/
  6864/6880 (inhaltsgleich), Magnum 5 -> @0x800A6670/668C/66A8, 6 -> @0x800A66C8/66E4/6700,
  Schrot 7 -> @0x800A6724/6740/675C, 8 -> @0x800A6788/67A4/67C0, Sparkshot 18 -> @0x800A68A4/
  68C0/68DC — dessen LEVEL-Record hat Flags 08/04/10 ueber EINER Box (200, 7500, 500)),
  Fenster FUN_80041B20, Zeile 6/3/0, DAT_800A6DB4, Radien (**Skeptiker (1)**: rec+8 +=
  +0x1EE>>2 nur Nah-Box, rec+10/+0x12/+0x1A += +0x9A>>2), XZ-Box FUN_80041CE4 im /4-Raum,
  Sub-Box-Reset (**Skeptiker (5)**). Der Applier ersetzt fuer RE2-owned Zombies Band, dy-Fenster
  UND den RE1.5-Keil `re15_gun_wedge_inside`; `re2_rising` ist gestrichen. Nicht ueber den
  Applier: das Messer (Id 1, Nahkampf-Kegel des Ports; seine Records @0x800A657C/63A8/64E0
  tragen b1 = 500/800 als seitlichen Versatz zur Klingenhand — Seite im Port nicht belegbar,
  s. §6.4), die Bruecken-Ids 9/10/11/16/17 (NULL-Record @0x800A6350) und der Liege-Spawn 0x88
  (EXEC[7], s. 6.3).
  **Skeptiker (6) Achse/Skala**: (a) Skala — +0x9A = 500 wird als 500>>2 = 125 auf halfw4
  addiert = 4*125 = 500 Welt-Einheiten, also exakt der Radius (nur die /4-Skala liefert das);
  Luecken-Freiheit 100+4*1000 = 4100 = Start Sub-Box 2. (b) Achse — der byte-true RE1.5-Keil
  im selben Port (re15_gun_wedge_inside, gegen Savestate 4 bewiesen) rechnet mit derselben
  PsyQ-RotMatrixY-Konvention: V5 = lokal (650,0) -> Welt (c,-s), lokal X = Blickrichtung; die
  Messer-Records versetzen die Box ueber t[2] = -4*b3 - b1 seitlich (lokal -Z) — nur mit X =
  vorwaerts sind die Starts 100/4100/8100 ein Schuss-Streifen. Der Pin bestaetigt: STEHEND
  trifft bis 4599 und verfehlt ab 4600 (TIEF), Schrot-HOCH 3599/3600.
  Rundung: FUN_80041CE4 rundet Ecke `(R*v + T) >> 2` und Gegner `+0x38 >> 2` beide ab — die
  ferne Kante liegt auf einem 4er-Raster, das von den absoluten Positionen abhaengt (Gegner-x
  = 4k+3: 4599 faellt wie 4600 nach aussen). Der Port bildet das ab; der Pin stellt den Zombie
  fuer die Grenzzellen auf ein Vielfaches von 4.
* **Schaden nach Klammer** `(rec.w0 >> (Klammer*10)) & 0x3ff` — Id 3 @0x800A4154 w0 =
  0x00E03C10 = 16/15/14, Id 7 @0x800A41A4 w0 = 0x0280F0C8 = 200/60/40; Typ 0x15/0x16/0x17 aus
  @0x800A42A8 (Id 3: 11/10/9, Id 7: 60/40/30). Klammer 0 ist byte-identisch mit der bisherigen
  Zeile s_re2_wpn_dmg_zombie (Pin: 16 / 200).
* **Stempel** `+0x1D2 = Teil + 3*Klammer` (@0x800413CC-D4) aus dem Applier-Ergebnis; der
  Trefferbox-Pfad (row_src 1) behaelt die Elevations-Naeherung, jetzt mit Maske (Beine-only ->
  Zone 0). Die Poise-Reserve (re2z_stamp_hit_row) liest die Klammer bereits aus +0x1D2/3.
  **Skeptiker (7)** — Tabellen-Dump @0x8010C940 (Stride 36, eigener Dump VOR dem Umschalten):
  ```
  Zeile 3 @0x8010C9AC: 80105438 80105438 00000000 | 80105438 80105438 00000000 | 80105438 80105438 00000000
  Zeile 7 @0x8010CA3C: 80107438 801066FC 00000000 | 80107438 80105BC0 00000000 | 80107438 80105438 00000000
  ```
  = Port-Zeilen 3 {1,1,0,1,1,0,1,1,0} und 7 {5,3,0,5,2,0,5,1,0}; beide Zeilen lagen bereits
  byte-identisch in `re2z_hit_tbl` (enemy_ai_re2_zombie.c), der Dump bestaetigt sie. Folge:
  Schrot-Rumpftreffer in Sub-Box 2 (ab 4200) dispatcht 0x80105BC0 (Taumel) statt 0x801066FC
  (Ragdoll), in Sub-Box 3 (ab 8200) 0x80105438; Beintreffer bleiben in allen Klammern
  0x80107438. **Das ist jetzt GEMESSEN, nicht nur aus der Tabelle gefolgert** — Pin-Abschnitt
  [E] (s. 6.2) feuert dreimal W8 EBEN auf denselben stehenden Zombie und liest den tatsaechlich
  dispatchten Handler EIN Bild nach dem Schuss (der Dispatch @0x801053E0-410 liegt im KI-Tick,
  nicht im Resolver; die Spalte `hnd` der Zeilenausgabe traegt darum den Stand des VORIGEN
  Ticks und taugt nicht als Beleg).

### 6.2 Messwerte vorher/nachher (`probe_r16_trefferhoehe pin`, Log build_p2/probe_r16_pin.log)

| Zelle | vorher (Dossier §1) | nachher | RE2-SOLL |
|---|---|---|---|
| KRIECHER W3/W8 EBEN, alle d | HIT (16 / 200 bzw. Crit), Zone 0 | **MISS** | MISS |
| KRIECHER W3 TIEF 4599 / 4600 | HIT / HIT | HIT Zone 0 / **MISS** | HIT / MISS |
| KRIECHER W8 HOCH 1500 / 3599 / 3600 | HIT Crit / MISS / MISS | HIT Zone 0 / HIT Zone 0 / MISS | dito |
| LIEGEND-P3 W3 EBEN | MISS | MISS | MISS |
| LIEGEND-P3 W8 HOCH 1500 | MISS | HIT Zone 0 | HIT Zone 0 |
| AUFSTEHEN-P6 W3/W8 EBEN, alle d | HIT | **MISS** | MISS |
| AUFSTEHEN-P6 W3 TIEF 3600 | MISS (RE1.5-Ring) | HIT Zone 0 | HIT |
| STEHEND W3 TIEF 4599 / 4600 / 5500 | HIT / HIT / HIT | HIT / **MISS** / MISS | HIT / MISS / MISS |
| STEHEND W8 HOCH 3599 / 3600 | MISS / MISS | **HIT** Zone 1 / MISS | HIT / MISS |
| STEHEND W3 EBEN 4599 / 5500 / 9000 | 16 / 16 / 16, +0x1D2 = 1 | 15 / 15 / 14, +0x1D2 = 4 / 4 / 7 | Klammer 1 / 1 / 2 |
| STEHEND W8 EBEN 3600 / 5500 / 9000 | 200 / 200 / 200, Ragdoll | 200 Ragdoll / **60 Taumel** / 40 | 200 / 60 / 40 |
| P7 Rueckbau | – (Maske gab es nicht) | Clip 8 (80 Bilder), Wechsel bei Zaehler 55 (davor 54); EBEN aus 1500 davor MISS, danach HIT | +0x14D == 55 |

Abschnitt [E] (Klammer-Dispatch, Skeptiker-Punkt 7) — derselbe stehende Zombie, W8 EBEN,
Handler EIN Bild nach dem Schuss:

```
  [E] W8 EBEN d=3600 | HIT  dhp=200 zone=1 klammer=0 zeile=7 -> Handler 3 (0x801066FC)
  [E] W8 EBEN d=5500 | HIT  dhp= 60 zone=1 klammer=1 zeile=7 -> Handler 2 (0x80105BC0)
  [E] W8 EBEN d=9000 | HIT  dhp= 40 zone=1 klammer=2 zeile=7 -> Handler 1 (0x80105438)
```


192 Zellen, 0 Abweichungen; Divergenzen vorher: 40 (§1) — jetzt 0 in den gepinnten Lagen. Die
12 Zellen des Liege-Spawns 0x88 (TIEF <= 4600 trifft) sind unveraendert gegenueber vorher (nur
Messung, s. 6.3).
**Skeptiker (4)**: der Port-Advancer laeuft im Game-Step VOR `re15_enemy_ai_run_all`
(game_step_common.c:1936 vs :1956); der P7-Tick sieht deshalb den schon advancten Zaehler —
gemessen faellt der Wechsel auf den Tick mit anim_frame 55 (P7-Tick 56 ab Bild 0 in P6), das
Bild davor ist 54; die Zusicherung gilt dem Bild-Wert (das ist die Instruktion `addiu v0,zero,55
/ bne v1,v0` @0x801036D4-D8), nicht der Tick-Nummer.

### 6.3 Pins

* NEU `unit_r16_trefferhoehe_pin` (probes/r16_trefferhoehe.cmake): 192 Zellen + 13 Zusicherungen
  (INIT-Maske, Fresser-Exit, Kriecher/P3/P6-Maske und +0x9A, P7 Bild 55, P8) + 3 Zusicherungen
  im Abschnitt [E] (Klammer-Dispatch je Sub-Box gegen Zeile 7 @0x8010CA3C).
* `unit_re2z_bandlock_pin` NEU VERANKERT (Fixture-Verschiebung): LEVEL-Treffer auf den
  Kriech-Root = 0 (Maske NUR BEINE, EBEN-Zeile `02 00 00`), DOWN > 0 (der Kriecher ist NICHT
  unverwundbar = der Nutzer-Befund 2026-08-27 bleibt erfuellt, jetzt auf dem RE2-Weg).
  Zweite Verschiebung im selben Pin: das Orakel spielte den Sweep bis zum Fund-Bild nach; das
  war schon VOR dem Fix nicht reproduzierbar (Lauf 19.09. auf dem Hauptbaum: Fall 3 als
  st=1/s1=1 gefunden, im Nachspiel st=2/s1=3; Fall 5 st=1/s1=5) und lief nach dem Fix in
  0 pruefbare Faelle (Sweep-Funde landeten im Nachspiel bei st=0). Der Fall traegt jetzt einen
  Schnappschuss von Aktor + Spieler aus dem Fund-Bild, das Orakel misst genau diesen Zustand:
  2 Faelle (Seeds 24/26), LEVEL 0 / DOWN 30 / ohne Liege-Bit 0 — die dritte Spalte zeigt, dass
  jetzt die Maske entscheidet, nicht +0x21A&2. Der Sweep selbst findet weniger Funde
  (223 statt 737 Bilder mit Liege-Bit), weil EBEN-Schuesse den Kriecher nicht mehr treffen und
  er den Spieler erreicht.
* `re2z_parts == 0` (Aktor ohne RE2-INIT, z.B. umgetypte Test-Slots in test_re2_baby_spider_dmg /
  test_re2_hp_model) zaehlt im Applier als INIT-Wert 3 / Radius 500 — benanntes Port-Mapping:
  in RE2 laeuft der INIT (@0x80100984-998) im Spawn-Bild, das Fenster existiert dort nicht.
  Der Klammer-Schaden folgt dem Schadensmodell-Schalter (nur wenn re15_enemy_dmg_row die
  RE2-Zeile liefert; RE2 AUS = RE1.5-Zeile, Wache test_re2_hp_model Abschnitt 7).
* Unveraendert gruen: `unit_re2_weapon_rows`, `unit_re2_hp_model`, `unit_re2_hit_repeat`,
  `unit_re2z_pushoff_crawl`, `unit_re2_crawler`, `unit_schrot_trennung` (EBEN aus 3600 = Nah-Box
  [200, 4700) -> Klammer 0 -> Ragdoll), `probe_spinne_elevation`.
* ctest (`re15_port/build_p2`, Worktree-Basis aaf06c9b): **306/306 gruen** — zweimal gelaufen,
  vor dem Abschnitt [E] (206,7 s) und mit ihm (183,7 s), beide Male 0 Fehlschlaege.

### 6.4 Offen

* Liege-Spawn 0x88 (EXEC[7]): in RE2 kein Ziel (HP = -1 @0x80100A3C-40, +0x1D3 |= 0x80
  @0x80103804-14), Maske bleibt 3. Der Port laesst ihn per spawn_pose-Ausnahme treffen; mit
  Maske 3 traefe der Applier ihn dann mit EBEN auf jeder Distanz (gemessen: 18 Zellen). Deshalb
  bleibt EXEC[7] auf der bisherigen Liege-Regel (grid&0x80: TIEF < 5000) — Entscheidung beim
  Nachbar-Dossier liegende-zombies.
* Messer (Id 1): laeuft weiter ueber Nahkampf-Kegel + dy-Fenster ohne Maske; in RE2 verwirft
  die EBEN-Zeile den Kriecher auch fuer das Messer (LEVEL[-1900,1000] -> row 3 -> `02 00 00`).
  Die seitliche Lage der Messer-Box (b1 = 500/800, lokal -Z) ist ohne RE2-Emulator nicht
  belegbar; UP-Record Flag 0x80 hat in FUN_80041B20 keinen Fall (lo/hi undefiniert).
* Kriech-Umbau-Tail FUN_80107A78 (@0x80107E70-EC0, Rueckbau @0x80107EA8) und Leiche->Kriecher
  @0x801089B4 haben keinen Port-Zwilling; INIT-Variante `sw 0xF01` @0x80100C0C ebenso.
* RE1.5-Crit-Regel (Schrot < 3000 -> hp = -1 fuer Typ < 0x20) wirkt weiter auch auf RE2-owned
  Zombies (Port-Bestand, nicht Teil dieses Themas).
* Der 16-Bit-Vorzeichentest von FUN_80041CE4 (MulMatrix0 >> 12 in shorts) ist im Port ein
  exakter Vergleich; Ueberlauf tritt erst jenseits ~32k Einheiten auf.

### 6.5 Nachpruefung (Fortsetzung nach API-Abbruch, selbst disassembliert)

Alle Skeptiker-Korrekturen wurden vom Fortsetzungs-Agenten noch einmal unabhaengig gegen
`info/re2leon/COMMON/BIN/EMOVL10_S0.BIN` und `RE2_Quellcode_V2/FUN_800410cc.c` geprueft:

* **(1) Radien** — `dis 0x80100960`: `addiu v1,zero,500` @0x8010096C, `sh v1,154(s2)` (+0x9A)
  @0x80100970, `sh v1,156` @0x80100974, `sh v1,144/146` @0x80100978-7C, `sh v1,494(s2)` (+0x1EE)
  @0x80100980, `lui v1,0xc00 / or / sw v0,0(s2)` @0x80100984-998. Kriecher-INIT
  `addiu v0,zero,200 / sh v0,154(s2)` @0x80100B00-04, Maske `lui v0,0x400` @0x80100B38;
  Ragdoll `addiu v0,zero,200 / sh v0,154(a0)` @0x80106B14-18, Maske `lui v0,0x400` @0x80106B38.
  Im Applier: `*(short*)(iVar10+8) += *(short*)(puVar9+0x1ee) >> 2` (NUR die Nah-Box-Tiefe) und
  `+10 / +0x12 / +0x1A += (short)(+0x9A >> 2)` (alle drei Breiten) — beides je einmal vor und
  einmal nach der Schleife, also ein Leihwert, kein dauerhafter Schreibzugriff.
* **(2) Fresser** — `dis 0x80103B74 182` gefiltert auf `lui …,0x(c00|400|f3ff)` und `sw …,0(rX)`:
  **ein einziger Treffer**, `sw v0,4(s1)` @0x80103D94 mit `addiu v0,zero,257` @0x80103D90.
  EXEC[8] aendert die Maske also nicht. Phasentabelle @0x801000B4 = {0x80103BE8, 0x80103C18,
  0x80103CB0, 0x80103CD8, 0x80103D60, 0x80103D90} — P5 ist der Exit.
* **(3) EXEC[9]** — `dis 0x80103E48`: Phasenverteiler `beq v1,v0(1),0x80103FB8` @0x80103EB8 und
  `beq v1,v0(2),0x80104144 / addiu v1,zero,257` @0x80103EE0-E4. Der Sturzzweig
  `beq s0,zero,0x8010411C` @0x8010401C nimmt den Fall-Pfad nur bei s0 != 0; dort steht
  `addiu v0,zero,1281` @0x80104020 und `sw v0,4(s1)` @0x80104028 im Delay-Slot von
  `jal 0x80015FE8` — der Zustandswechsel auf 0x501 (EXEC[5] P1) passiert VOR dem Maskenwechsel
  `lui a2,0xf3ff / … / lui v0,0x400 / sw v1,0(s1)` @0x80104068-AC. Der Nicht-Sturz-Pfad
  @0x8010411C-40 faehrt ueber `j 0x80104154` am Handler-Ende vorbei und fasst die Maske nie an.
  Der Rueckbau liegt damit wie beschrieben in EXEC[5] P7/P8 — **keine offene Luecke**.
* **(4) P7-Bildwert** — `lbu v1,333(s2) / addiu v0,zero,55 / bne v1,v0,0x80103754 / lui v1,0xc00`
  @0x801036D0-DC; P8 `addiu v0,zero,500 / sh v0,154(s2)` @0x801036FC-700 und `lui v1,0xc00`
  @0x80103730. Der Port liest +0x14D ueber `re2z_frame_slot`, dieselbe Abbildung, die
  enemy_ai_re2_zombie.c an 12 weiteren byte-belegten Stellen benutzt.
* **(5) Sub-Box-Reset** — im Decompilat steht `uVar7 = 0;` nur in den Bloecken fuer Box 2 und
  Box 3, jeweils INNERHALB von `if (iVar4 != 0)` (also erst nach bestandenem XZ-Test). Der Port
  macht es an derselben Stelle (`if (b > 0) res = 0;` nach `re15_re2_box_inside`).
* **(7) Zeilen 3 und 7** — eigener `table`-Dump @0x8010C9AC / @0x8010CA3C, byte-identisch zu
  §6.1; zusaetzlich jetzt am laufenden Port gemessen (Abschnitt [E]).

Aufraeumung in diesem Schritt: im Applier folgte auf die Port-Mapping-Zeile
`if (mask == 0u) { mask = 3u; … }` noch das Original-Gate `if (mask == 0u) return 0;` — nach der
Zuweisung unerreichbar. Es steht jetzt als Zitat im Kommentar statt als toter Zweig.

## 7. Nacharbeit (Phase 3, 2026-09-19, Thema re-restposten)

Offener Punkt aus §5: *„Messer (RE2-Id 1) und die uebrigen Hitscan-Ids: Geometrie-Records nur als
Zeiger gelesen (@0x800A6900/…), Inhalt nicht gedumpt."* — erledigt.

### 7.1 Der Geometrie-Zeiger ist ein PAAR, und das Messer hat eine Record-REIHE

`re15_port/tools/re2_gun_tables_dump.py` ist erweitert: es dumpt jetzt nicht nur den ERSTEN Record
je Zielhoehe, sondern das ganze Record-ARRAY samt der Auswahl-Liste. Der Eintrag
@0x800A68E8 + item*24 + grp*8 ist `{rec_base, pattern}`; `pattern` ist eine Liste von
`{recIdx, count}`, die FUN_800410CC pro Angriffsbild abarbeitet (alles selbst disassembliert):

```
80041128  lbu v0,492(t0)      ; +0x1EC Restbilder   / 80041130 bne v0,zero,0x80041168
80041138  lbu v0,493(t0) / addiu v0,v0,1 / sb v0,493(t0)          ; +0x1ED Listenindex++
8004114c  lw v1,4(a2) / sll v0,v0,1 / addu / lbu v0,1(v0) / sb v0,492(t0)   ; count
8004116c  lbu v0,493(t0) / lw v1,4(a2) / sll / addu / lbu a1,0(v0)          ; recIdx
80041180  addiu a0,zero,255 / bne a1,a0 -> ...  ; recIdx 0xFF: `lbu v1,492; addiu v1,-1;
          sb v1,492; j 0x80041aec` mit v0 = 0 = DIESES BILD TRIFFT NICHTS (kein Listenende)
800411a0  addiu v0,zero,254 / bne a1,v0 -> ...  ; recIdx 0xFE: Ruecksprung an den Listenanfang
```

Ergebnis des Dumps (Protokoll `re-restposten_gun_records.log`):

| Item | Pattern (recIdx/count) | Records |
|---|---|---|
| **1 Messer** | `ff/6 00/1 01/1 02/1 03/1 04/1 00/255` | **fuenf** je Gruppe (DOWN @0x800A657C+n*0x1C, LEVEL @0x800A63A8+n*0x1C, UP @0x800A64E0+n*0x1C) |
| 2/3/4/13/19 Pistolen | `00/1 00/255` | einer |
| 5/6 Magnum | `00/1 …` | einer |
| **7 Schrot** | `00/1 ff/1 00/1 ff/1 00/1 ff/1 00/255` | einer — **vier treffende Bilder**, exakt die vier Resolve-Aufrufe, die der Port aus RE1.5 @0x80033508-58 fuehrt |
| 8 Custom-Schrot | `00/1 ff/1 …` (fuenf treffende Bilder) | einer |
| 15 SMG | `00/1 ff/2 fe/255` | einer |
| 18 | `00/2 ff/1 fe/255` | einer |
| 9/10/11/12/16/17 | `00/255` auf dem NULL-Record @0x800A6350 (Flags 00 00 00) | **keiner** |

Damit ist die §5-Frage beantwortet: **ausser dem Messer steht jede Waffe in jedem treffenden Bild
auf Record 0** — die Tabelle in re15_damage.c war also vollstaendig, nur das Messer fehlte. Und die
sechs Bruecken-Ids haben im Original *nachweislich keinen Record*; sie bleiben belegt auf dem
bisherigen Weg (Granaten 9/10/11, Bowgun 12, Flammenwerfer 16, Rakete 17).

Die fuenf Messer-Records sind der KLINGENBOGEN (LEVEL, `{start, seitlich, Tiefe/4, Halbbreite/4}`):

```
[0] @0x800A63A8 Flag 08  { 250, 800, 426, 121}      [3] @0x800A63FC Flag 08  {1250,    0, 426, 121}
[1] @0x800A63C4 Flag 08  { 700, 800, 426, 121}      [4] @0x800A6418 Flag 08  {1050, -200, 426, 121}
[2] @0x800A63E0 Flag 08  {1150, 300, 426, 121}
```

### 7.2 Port: das Messer laeuft jetzt durch den Applier

`re15_damage.c` traegt die Reihe als `s_re2z_geo1[3][5]` (jeder Record mit seiner `@0x…`-Adresse)
und waehlt den Schritt ueber `re15_re2_knife_step()`. **PORT-ABBILDUNG, als solche gekennzeichnet:**
der Port ruft den Resolver im Schlag nur im RE1.5-Schadensfenster `anim_frame 6..11`
(byte-true @0x80035388-cc) auf — und genau bei Bild 6 endet RE2s Ausholphase `ff/6`. Beide
Originale legen den ersten Schadensframe auf 6; die Zuordnung ist Bild 6 → Record 0 … Bild 10 →
Record 4, Bild 11 → Record 0 (Listeneintrag `00/255`).

Zwei Entscheidungen sind ausdruecklich benannt, nicht geraten:

* **Flag 0x80** (UP-Record [0] @0x800A64E0) hat in FUN_80041B20 KEINEN switch-Fall; `*param_3` /
  `*param_4` bleiben unbeschrieben, der Aufrufer sieht den Wert des VORIGEN Aufrufs (die Locals
  `local_80`/`local_7c` sind funktionsweit). Das Original hat dort also kein definiertes Fenster.
  Der Port VERWIRFT diese Sub-Box. Messbare Folge: keine — die Bilder 7..10 tragen Flag 0x10
  (UP-Fenster [-3000,-500] fuer Id 1), das dy = 0 ohnehin ausschliesst.
* **Nur die echten Nahkampfwaffen** bekommen die Messer-Records. `re2z_row_from_weapon` bildet auch
  w0 (unbewaffnet) und w21 (keine Waffe) auf RE2-Id 1 ab — beides [PORT-ZUORDNUNG]en fuer die
  Schadenszeile. Welcher Tester eine Waffe fuehrt, sagt @0x8006E548: [0] = [21] = 0x80012574
  (Streifen), [1] = [2] = 0x800127FC (Kegel). Ohne diese Schranke verfehlte w0 auf der Pin-Distanz
  3500 jeden Schuss (die Messer-LEVEL-Box reicht bis ~2450) und verschob damit die ganze
  RNG-Folge von `unit_re2_weapon_rows` — 9 Pins fielen. Mit der Schranke: 319/319.

### 7.3 Messwerte (`probe_p3_restposten`, Abschnitt [A], Log `re-restposten_sonde.log`)

Distanzen 400/900/1400/2000/2600/3400, je Schlag-Bild 6..11:

| Lage (Maske) | EBEN | TIEF |
|---|---|---|
| STEHEND (3) | trifft, **Zone 1 = Rumpf** in jedem Bild (Reichweite wandert mit dem Bogen: Bild 6 bis 2000, Bild 9 bis 3400) | Bild 6 Rumpf (Record [0] traegt Flag 0x08 = LEVEL-Fenster!), Bilder 7..10 **Zone 0 = Beine** |
| KRIECHER (1) | **0 Treffer in allen 6 Bildern x 6 Distanzen** | Bilder 7..10 treffen, **Zone 0** |

Vorher (Stand §6): der Schlag lief am Applier vorbei durch den RE1.5-Nahkampfkegel und traf den
Kriecher mit EBEN auf jeder Distanz im Kegel. Jetzt entscheidet dieselbe Teile-Maske wie beim
Schuss: EBEN waehlt Zeile 3 Satz 2 `02 00 00` (nur Rumpf) und verwirft Maske 1.

### 7.4 Offen (unveraendert / neu)

* Die SEITE des Messer-Versatzes (`b1` = 500/800 nach lokal −Z) bleibt ohne RE2-Emulator
  unbelegt. Fuer ein Ziel GENAU VORAUS ist sie folgenlos: alle fuenf Records enthalten die
  seitliche 0 (Intervalle [-1784,184) … [-784,1184)) — nur seitlich versetzte Ziele haengen daran.
* Item 14 hat einen echten Record (@0x800A67F0/680C/6828, Box (200,0,3500,500)) und steht jetzt in
  der Tabelle, hat aber keinen Port-Konsumenten (`re2z_row_from_weapon` bildet auf 14 nicht ab).
* Die Kommentar-Beschriftung „Sparkshot" an `s_re2z_geo[18]` ist irrefuehrend: laut der
  Item-Definitionstabelle @0x800A9E1C und den PLW-Dateien (Block in enemy_ai_re2_zombie.c) ist
  Item 14 der Spark Shot und Item 18 die Gatling. Nur ein Name, kein Wert — nicht angefasst, um den
  Diff dieses Auftrags klein zu halten.
