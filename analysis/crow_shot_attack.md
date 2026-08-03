# Krähe (0x21): Schuss-Reaktion + Attack-Commit — zwei Live-Reports nach 2446139c/61848ff6

**Datum:** 2026-08-03 · **Methode:** Raw-MIPS-Disasm (`re15_disasm.py`) aus `STAGE1.BIN`
(file = addr−0x80100000) und `PSX.EXE` (file = addr−0x80010000+0x800), Decompile-Gegenlese
(`RE_15_Quellcode_V2/FUN_8003dcc4.c`, `FUN_80011f50.c`, `FUN_8003703c.c`), SCA-Byte-Parse
ROOM1170/1140, dynamische Reproduktion per erweiterter Sonde
`re15_port/tests/unit/probe_crow_1170.c` (run_live_variant, A/B mit/ohne Kollisionsband).
Baut auf `analysis/crow_1170.md` auf; dessen §2.2-Etikett „Death STAGE1-unerreichbar" wird
hier teilkorrigiert (§2).

## 0. Ergebnis in zwei Sätzen

- **Report (a) „Krähen greifen NOCH IMMER nicht an":** Der Port-LOS-Stand-in
  (`re15_collision_on_floor`) meldet auf dem offenen 1170-Pad unter Live-Bedingungen
  (Spieler-Band 4 gesetzt) JEDEN Sample-Punkt als „blockiert" → `crow_parity` (+0x1d2) wird
  nie 1 → das steer[9]-Attack-Commit-Gate (@0x8011330c-14 `lbu +0x1d2; beq zero → exit`)
  fällt IMMER durch. Die alte Sonde lief ohne Band (`s_coll_band = −1` → `return 1`) und hat
  genau das maskiert (D12 war deshalb „grün"). Dynamisch bewiesen: **Band 4 → 0
  parity-Ticks / 0 Attack-Ticks in 2400 Ticks; Band unset → 113 / 376.** Im Original ist der
  LOS-Ray für die FLIEGENDE Krähe strukturell IMMER frei (§3).
- **Report (b) „angeschossene Krähen verhalten sich falsch":** Der Task-Verdacht
  (state-2-Freeze wegen hp=100) ist für den aktuellen HEAD **widerlegt** — die Port-Krähe
  spawnt mit hp=0 (scd_vm.c:2920, Audit wf_555f18eb) und stirbt beim Schuss über die
  Death-Lane (dyn belegt §5). Die ECHTEN Schuss-Divergenzen sind: fehlende
  GIB-Lane-Zuordnung (nur weapon 7 statt 7-11/13/15-18), fehlender +0x9a-0/−1-Latch
  (0x80115f70), fehlender Elevation-Band-Stempel (UP/DOWN/LEVEL aus +0x1ec) und der
  fehlende vertikale Winner-Filter des Schuss-Resolvers (§4).

---

## 1. Schuss-Reaktion im Original — der vollständige Mechanismus

### 1.1 Root-Tabelle @0x8012111c: [2]/[5]/[6] sind ECHTE Leer-Stubs

```
[0] 0x8011224c INIT     [1] 0x80112420 ACTIVE   [2] 0x80114e4c   [3] 0x801146d0 DEATH
[4] 0x80114e54 FLIGHT-2 [5] 0x80114e4c          [6] 0x80114e4c   [7] 0x801157e8 CORPSE
@0x80114e4c: jr ra ; nop        ← states 2/5/6 = Leer-Stub auf Dispatch-Ebene
```

Byte-Scan ALLER +0x4-Writes im Krähen-Overlay (0x80111a4c–0x80116400, sb/sh/sw imm=4):
genau 6 Treffer — `@0x80112088` (Promotion → 3), `@0x80112388` (INIT-State-Word → 1),
`@0x8011240c` (grid&0x40-Override → 4), `@0x80114978` / `@0x80114b90` / `@0x80114e00`
(die drei Death-Lane-Enden → 7). **Es gibt KEINEN Writer, der 2 setzt oder 2→1
zurückschreibt: die Krähe hat KEINEN Hit-Flinch.** State 2 wird ausschließlich EXE-seitig
geschrieben (FUN_80011f50 `+0x4=(HP>=0)?2:3` @0x80012520; FUN_80012d60 @0x80013018/20) und
wäre ein permanenter Freeze — er ist aber unerreichbar, weil:

### 1.2 Krähen-HP ist per Konstruktion 0 oder −1 — nie positiv

- **INIT FUN_80111a4c:** `@0x80112298 sh zero,154(v0)` → **+0x9a = 0** (im selben Block:
  +0x93=0 @0x80112288, +0x1ba=0 @0x801122ac). `Sce_em_set` (FUN_800420a0) schreibt +0x9a
  nicht.
- **ACTIVE-Tail-Latch 0x80115f70 (jal @0x801125cc, JEDER State-1-Tick):**
  ```
  80115f7c: lh v0,492(v1)      ; +0x1ec vert_err (player.y − crow.y)
  80115f88: slti v0,v0,5200
  80115f8c: bne → 80115f9c     ; vert<5200
  80115f90: addiu v0,zero,-1
  80115f98: sh v0,154(v1)      ; vert>=5200 → +0x9a = −1
  80115f9c: sh zero,154(v1)    ; sonst        +0x9a = 0
  ```
  (danach Schatten-Skalierung +0xbc/+0xbe aus (y−+0x1ba)>>4+400 min 100 @0x80115fa0-e4 und
  Tint aus >>5+128 @0x80115fe8ff — render-seitig.)
- **Es gibt KEINE per-Typ-HP-Tabelle für die Krähe** (die Zombie-Row @0x8011f034+type*0x20
  gehört zur Zombie-Familie; kein Krähen-Code liest sie).
- Savestate-Gegenprobe: `orig_1170_gp.sav`/`gp2.sav` enthalten nur Elliot (0x47) — ein
  Re-Entry-Roster-Save (z3/125) existiert nicht; Live-Bestätigung bleibt OFFEN (§7), die
  statische Doppel-Evidenz (INIT-Null + Tick-Latch) ist eindeutig.

### 1.3 Damage-Row Typ 0x21 und die Schuss-Folge

Per-Typ-Tabelle @0x8006e0d0, Row 0x21 @0x8006ec28 (= base + 0x21·0x58), u16 je Waffe:

```
[0,2, 2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2, 0, 100]
 w0 w1……………………………………………………………………………w19 w20 w21
```

→ **2 Schaden für jede reale Waffe** (w21=100, w0/w20=0). FUN_80011f50 auf dem Gewinner
(@0x800124b0-0x80012520): `+0x5 = weapon_id` (@0x800124bc), `+0x9a −= dmg`, `+0x93 |= 1`,
`+0x7 = 0` (@0x80012428), `+0x4 = (HP>=0) ? 2 : 3` (@0x80012520). Mit HP∈{0,−1} und dmg≥2:
**hp wird immer negativ → jeder Treffer = Sofort-Kill → state 3.** (Einzige theoretische
state-2-Quelle wären die 0-Damage-Slots w0/w20 — keine ausgerüstete Waffe.)

### 1.4 Death-Lane beim Schuss-Kill: Lane = weapon_id, drei Handler, GIB ab w7

Death-Dispatcher 0x801146d0 (state 3), jeden Tick:
```
801146e0: lbu a1,454(v0)          ; +0x1c6 = kill-Flag-Index (em_set pc[7], 1170: 50-56)
801146ec: jal 0x8004ef90          ; Flag-Set in 0x800b1038 → Krähe bleibt bei Re-Entry tot
80114700: lbu v0,5(v0)            ; +0x5 (= weapon_id nach Schuss!)
80114708-24: sll 2; lw @0x801211cc[+0x5]; jalr    ; KEIN Bounds-Check
```
Lane-Tabelle @0x801211cc (24 gelesene Einträge):
- **[0-6], [12], [19], [20] → 0x80114738** — Normal-Fall (Step-Router auf +0x7: INIT
  0x80114784 Se(3)/grav 0x26 → FALL 0x80114828 → Land floor−400 clip 0x0a → FINISH
  0x80114934 → +0x4=7 @0x80114978).
- **[7], [8], [9], [10], [11], [13], [15], [16], [17], [18] → 0x801149c4** — **GIB-Lane**
  (13 Feder-Children @0x80114a50, +0x4=7 @0x80114b90). Also nicht nur die Crit-Waffe 7:
  auch Shotgun (8) & alle schweren Waffen zerlegen die Krähe in Federn.
- **[14] → 0x80114ba4** — dritte Lane (die @0x80114c68-c90-Flock-Writes aus crow_1170.md D6
  gehören HIERHER; Ende `sb 7,+0x4` @0x80114e00).
- [21-23] laufen in die FLIGHT-2-Subtabelle @0x80121220 (Tabellen-Aliasing; w21=Selbstlauf-
  Slot, praktisch nicht erreichbar).

Crit-Regel (@0x800124fc-1c): w7 bzw. w8<3000 setzen +0x93|=0x40; das HP=−1-Instakill gilt
nur `type<0x20` — für die Krähe irrelevant (sie ist ohnehin tot), die Lane-Wahl über +0x5
macht den sichtbaren Unterschied.

**Korrektur zu crow_1170.md §2.2:** „Krähen-Tod in STAGE1 unerreichbar" gilt NUR für die
ROOT-Promotion (state 4 + grid&0x40 + bit-0x1f). Der SCHUSS-Pfad schreibt +0x4=3 direkt
(@0x80012520) — **die Death-Lanes sind in STAGE1 per Waffe jederzeit erreichbar** (und die
D6-Flock-Re-Arm-Broadcasts damit auch: erschießt man eine armed Krähe, re-armiert
aca50|0x800 eine disarmte).

### 1.5 Treffbarkeit: Bänder statt HP-Filter

FUN_80011f50-Kandidatenschleife: aktiv (word0&1) + **Band-Match
`word0 & DAT_800aca54 & 0xe0000000`** + `(+0x93&3)!=3` + Cone-Tester — **kein HP-, kein
State-Filter**. Winner-Nachfilter: `type>0x3f → return 0`; vertikal
`|enemy.y − aim.y| < 0x1f5` → Hit, sonst FUN_8001bafc (Projektion 0x8004f008 gegen die
+0x1b4-Blockerzelle), sonst Kamera-Geometrie-Block @0x80012280-0x80012370.

Die Krähen-Bänder stempelt der ACTIVE-Tail selbst (@0x80112560-c8):
```
word0 &= 0x1fffffff
vert>=4001 → jal 0x80012a0c(0x1770)   ; |=0x80000000 (UP)   nur bei dist<6000
vert< 800  → jal 0x80012974(0x1770)   ; |=0x20000000 (DOWN) nur bei dist<6000
sonst      → word0 |= 0x40000000      ; LEVEL (@0x801125bc-c8)
```
→ Eine Krähe ≥4001 über Leon trifft nur ein UP-Schuss (innerhalb 6000), eine
sitzende/niedrige (<800) nur ein DOWN-Schuss, die kreisende (800..4000) LEVEL. Der
+0x9a=−1-Latch (≥5200) wirkt zusätzlich im AUTO-AIM FUN_8003703c: der bucketed Kandidaten
nach dem Vorzeichen von +0x9a — hp<0-Ziele nur als letzter Fallback → das Fadenkreuz
klebt nicht an unerreichbar hohen Krähen.

---

## 2. Attack-Commit im Original — steer[9] vollständig (Disasm)

Weg dorthin (wer setzt sub 9): patrol[0-3] → Dive-Decide 0x80112628 (Ring grid<0x80:5000 /
≥0x80:10000 + vert<5400 @0x801126ac-f8) → sub 4; move[4] Anim-Wrap → sub 5 (@0x80112b94);
**steer[5] @0x80112bbc-d8: `lh +0x1ea; lw +0x38; addiu −3600; slt` → y < perch−3600 →
`jal 0x80115d74(9)`**; alternativ move[6]-Timer → sub 9 (@0x80112dbc). steer[4]/[6]
(@0x80112a28) sind `jr ra`-Stubs.

steer[9] @0x8011325c:
```
8011326c: lbu +0x1d3 (pturn); beq zero → 0x80113300   ; pturn==0 → DIREKT zum Commit-Block
8011327c: sb zero,+0x1d3
80113294: mode&0xf != 0        → sub 9  (@0x801132a0-a4)
801132a8: sonst mode&0x80      → jal 0x80115f00 (perch-return)
801132b0: sonst +0x5 != 6      → sub 6
801132c0: sonst                → jal 0x80115f00
801132e4-f8: (NUR im pturn-Pfad!) +0x1d6 >= 3 → jal 0x80115f00
; Commit-Block @0x80113300:
8011330c: lbu +0x1d2 ; beq zero → exit      ← LOS-PARITY-GATE
8011331c: lbu +0x1db ; beq zero → exit      ← armed
8011332c: lhu +0x1dc ; sltiu 0x2710 miss → exit   ← dist < 10000
80113340: lbu +0x1d6 ; sltiu 3 miss → exit  ← atk_ctr < 3
80113354-6c: grid<0x80 → a0=0xc (sub 12) sonst a0=0xa (sub 10); jal 0x80115d74
```
Port (enemy_ai_common.c:3293-3311) = zeilengleich bis auf EIN Detail: der atk_ctr≥3-
Perch-Return läuft im Port JEDEN Tick (Z. 3303), im Original nur im pturn-Pfad (F7, LOW).

### 2.1 Das Parity-Gate: FUN_8001bc08 → FUN_8001b84c → FUN_8003dcc4

- **FUN_8001bc08** (EXE, @0x8001bc1c-2c): Zähler +0x1f0&0x1f, Steps 0-3 rufen
  `FUN_8001b84c(0x5e8, &playerX)`, Ergebnis-Bit → +0x1f0|=(ret<<5); Step 3: Bit 0x20 clear
  → Snapshot +0x1bc/+0x1be=player, return 1; sonst return 0; beide clearen 0x20
  (@0x8001bd04); Steps 4-15 return 2; Wrap bei ≥0x10 (@0x8001bd40-44). Port-Kadenz
  (enemy_ai_common.c:1403-1433) stimmt.
- **FUN_8001b84c:** FOV `(atan2(enemy→player) − rot_y + 0x5e8) & 0xfff` vs `2·0x5e8`
  (@0x8001b898-b0) — außerhalb → return 1 (blockiert). Sonst Ray-Vektor
  **{dx, 0, dz}** (`sw zero,20(sp)` @0x8001b8d8 — **Y hart 0, der Strahl läuft horizontal
  auf Krähen-Höhe**) und `jal 0x8003dcc4(&vec, +0x1f0, 0xf00, 0x300)` (@0x8001b8e8-ec).
- **FUN_8003dcc4** (Decompile + Disasm-Kernzeilen): scannt pro Aufruf die
  QUADRANTEN-Region `step&0x1f` der SCA (`region_ptr[step]` @ sca_base+8+step·4 — Steps
  0-3 ⇒ Regionen 0-3, Region 4 wird NIE ge-LOS-t). Zellen-Filter:
  ```
  (cell.word5 >> 12) == *(byte*)(entity+0x82)     ; Band-Nibble == ENEMY-Band-Byte!
  && (cell.word5 & 0xf00) == 0x300                ; Typ-Nibble 3 = Sicht-Blocker
  ```
  dann 2D-Segment-Kreuzungstest (alle Koordinaten /0x12, OuterProduct0 gegen beide
  Zell-Diagonalen); Kreuzung → `entity+0x1b4 = cell; return 1` (blockiert).

### 2.2 Warum die fliegende 1170-Krähe im Original NIE geblockt wird

ROOM1170-SCA (RDT@0x20→0xa80, 150 Zellen, word5-Histogramm aus den Datei-Bytes):
`0x0300×30 (Band 0), 0x2300×20 (B2), 0x3300×10 (B3), 0x4300×85 (B4), 0x430e×5 (B4)` —
**höchstes Band = 4.** Die Krähe im Commit-Korridor (sub 9, y ∈ [perch−5400, perch−1800] =
[−12600, −9000]) hat `+0x82 = −(y/1800) = 5..7` (Root-Refresh @0x80112128-54). **Keine
Zelle matcht → der Ray ist frei → parity = reiner FOV-Test** → Commit feuert, sobald die
Krähe grob Richtung Spieler fliegt (move[9] slewt yaw auf den Spieler). Gegenprobe
ROOM1140: alle 95 Zellen word5=0x0300 (Band 0, Typ 3) — für Band-0-Zombies echte
Wand-Blocker; deshalb funktionierte der Port-Stand-in dort.

### 2.3 Der Port-Defekt (Report a) + dynamischer Beweis

`re15_enemy_los_probe` (enemy_ai_common.c:1415-1421) ersetzt den Ray durch 4 Punktproben
mit `re15_collision_on_floor(&g_room_rdt, sx, sz)` — der prüft „Punkt IN einer
`s_coll_band`-Zelle" (re15_collision.c:236-250, **Spieler-Band, nicht Gegner-Band**). In
1170 sind die Band-4-Zellen WÄNDE, der Spieler läuft im band-FREIEN Komplement
(re15_collision.c:13-23) → jeder Pad-Punkt liefert 0 → `s_los_blocked=1` → parity nie 1.
Ohne Band (`s_coll_band<0`) returned die Funktion pauschal 1 — die alte Sonde lief so.

Sonde `probe_crow_1170.c` um `run_live_variant` erweitert (g_room_rdt publiziert +
`re15_collision_set_band(4)` wie room_common.c:218; Spieler bei (530,−7200,−5150) neben
der armed Spawn-5-Krähe):
```
band=4 (wie live):   parity1-Ticks=0    Attack-Ticks=0    ss1-Maske=0x3f1 (0,4-9)  player-hp=100
band unset (D12):    parity1-Ticks=113  Attack-Ticks=376  ss1-Maske=0x73f1 (+12/13/14) player-hp=60
```
→ Report (a) reproduziert und auf das Parity-Gate isoliert; alle übrigen Commit-Gates
(armed=1, dist 3453<10000, atk_ctr<3) waren erfüllt.

---

## 3. Schuss-Verhalten des Ports heute (dyn gemessen)

Sonden-Schuss (`re15_player_weapon_fire(3)`, beide Varianten identisch):
```
Krähe vor Schuss: hp=0 (em_set-Spawn, scd_vm.c:2920)
fire(3)=hit → hp=−2, st=3, ss1=3 (=weapon_id), ss3=0
Death-Lauf:  st=7 corpse, y=−7600 (=floor−400-Clamp), motion=10 (Land-Clip 0x0a), hp=−1
```
→ **Kein state-2-Freeze** (der Task-Verdacht basierte auf dem alten hp=100-Nominal, das
Audit wf_555f18eb bereits entfernt hat; hp=100 in enemy_ai_common.c:8365 gehört zur Ivy
0x2d). Kill-Persistenz ist generisch abgedeckt (run_all: state-7-Corpse →
`re15_game_flag_set(zone, em_flag_id)` enemy_ai_common.c:8481-82 ≙ jal 0x8004ef90
@0x801146ec). Was am Schuss-Pfad WIRKLICH divergiert: F3-F6 (§4).

---

## 4. Divergenzen (Findings)

| # | Sev | Kurz | Original-Beleg | Port-Stelle |
|---|-----|------|----------------|-------------|
| F1 | HIGH | LOS-Stand-in blockt 1170 permanent → kein Attack-Commit (Report a) | steer[9]-Gate @0x8011330c-14; Ray-Kette @0x8001bc40 → @0x8001b8d8/e8 (Y=0, dcc4) → dcc4-Filter `(w5&0xf00)==0x300 && w5>>12==+0x82`; 1170-SCA max Band 4 vs Krähe Band 5-7 | enemy_ai_common.c:1415-1421 + re15_collision.c:236-250 |
| F2 | — (Befund) | Kein Hit-Flinch; state 2/5/6 = `jr ra` @0x80114e4c; +0x4-Census = 6 Writer, keiner setzt 1/2 | Scan §1.1 | Port-Default-Hold (Z. 4034) ist byte-true |
| F3 | MED | GIB-Lane-Map unvollständig: Original GIBt Lanes 7-11/13/15-18, Lane 14 = dritte Lane 0x80114ba4 | Tabelle @0x801211cc; Dispatch @0x80114700-24 (+0x5=weapon @0x800124bc) | re15_crow_death: GIB nur `ss1==7` (Z. 3809) |
| F4 | MED | +0x9a-0/−1-Latch fehlt (HP-Ground-Truth + Aim-Demotion + Schattenfelder) | 0x80115f70 @0x80115f88-9c, Call @0x801125cc; Konsument FUN_8003703c (Bucket nach +0x9a-Vorzeichen) | ACTIVE-Tail als OPEN markiert (Z. 3983-87); Port-Aim skippt hp<0 ganz (re15_damage.c:421) |
| F5 | MED | Elevation-Band-Stempel fehlt: UP≥4001 (dist<6000) / 0x20000000<800 (dist<6000) / LEVEL sonst | @0x80112560-c8, Helfer 0x80012a0c/0x80012974 mit a0=0x1770 | re15_damage.c:624 `eband = LEVEL` für alle Nicht-Downed |
| F6 | LOW/OFFEN | Vertikaler Winner-Filter des Schuss-Resolvers fehlt: `|Δy|<0x1f5` sonst FUN_8001bafc sonst Geometrie-Block | FUN_80011f50 Winner-Pfad (Decompile Z. 76-84) | re15_player_weapon_fire hat keinen Δy-Filter |
| F7 | LOW | steer[9]: atk_ctr≥3-Perch-Return nur im pturn-Pfad | `beq +0x1d3,zero → 0x80113300` @0x80113274 überspringt @0x801132e4-f8 | Z. 3303 läuft jeden Tick |
| F8 | Doku | crow_1170.md §2.2 „Death STAGE1-unerreichbar" gilt nur für die Root-Promotion; Schuss-Pfad erreicht die Lanes direkt | +0x4=3 @0x80012520 | Doku ergänzen |

## 5. Fix-Plan (Port, mit @0x-Zitaten)

1. **F1 — byte-true LOS-Ray statt on_floor-Stand-in** (behebt Report a):
   in `re15_enemy_los_probe` die Punktprobe durch den dcc4-Zellentest ersetzen —
   pro Step k (0-3) Quadranten-Region k der SCA scannen (`sca_rgn`-Prefixsummen; Region 4
   nie), Zellen-Filter `(w5 & 0xf00) == 0x300 && (w5 >> 12) == e->floor` (**+0x82 des
   GEGNERS**, beim Krähen-Root jeden Tick = −(y/1800) @0x80112128-54; beim Zombie =
   em_set-pc[4] @0x800421d0), dann 2D-Segment-Kreuzung Ray (enemy→player, /0x12) gegen
   beide Zell-Diagonalen (OuterProduct0-Vorzeichen, FUN_8003dcc4). Damit bleibt ROOM1140
   identisch (Zellen 0x0300 = Band-0-Blocker für Band-0-Zombies) und 1170 wird frei für
   Band-5-7-Flieger. Verifikation: `probe_crow_1170` run_live_variant(band=4) muss danach
   parity>0 + Attack-Ticks>0 zeigen; test_room1140_combat unverändert grün.
2. **F3 — Death-Lane-Map @0x801211cc:** `re15_crow_death` auf die echte Zuordnung heben:
   ss1 ∈ {7,8,9,10,11,13,15,16,17,18} → GIB 0x801149c4; ss1==14 → dritte Lane 0x80114ba4
   (portieren; Ende +0x4=7 @0x80114e00, Flock-Writes @0x80114c68-c90); Rest → Normal-Fall.
3. **F4 — Latch nachziehen:** im ACTIVE-Tail (nach dem Re-Arm-One-Shot):
   `e->hp = (e->crow_vert_err >= 5200) ? −1 : 0;` (@0x80115f88-9c) + Kommentar auf den
   703c-Konsumenten; Port-Aim-Verhalten (skip hp<0) dokumentieren oder auf Fallback-Bucket
   umbauen (703c: hp<0 nur wenn kein hp≥0-Ziel existiert).
4. **F5 — Band-Stempel:** Krähen-Tail schreibt ein Aim-Band-Feld (UP/DOWN/LEVEL nach
   vert_err 4001/800 + dist<6000 @0x80112560-c8); `re15_player_weapon_fire` liest eband aus
   dem Feld statt pauschal LEVEL (re15_damage.c:624).
5. **F6 — OFFEN halten** bis FUN_8001bafc + Geometrie-Block @0x80012280-370 RE'd sind;
   dann `|Δy| < 0x1f5`-Gate + bafc-Nachtest im Winner-Pfad.
6. **F7 — Mini-Fix:** Z. 3303 in den pturn-Block ziehen (`@0x80113274 beq → 0x80113300`).

## 6. Beantwortete Task-Fragen (Kurzindex)

- „Ist state 2 wirklich leer?" — Ja: [2]/[5]/[6] → `jr ra` @0x80114e4c; kein Rückweg (§1.1).
- „Wie stirbt eine getroffene Krähe?" — Immer: HP 0/−1 minus ≥2 → hp<0 → +0x4=3
  (@0x80012520) → Lane = weapon_id @0x801211cc (§1.3/1.4). FUN_80011f50 setzt +0x4=3 — die
  alte „unerreichbar"-Etikettierung betraf nur die Root-Promotion (F8).
- „Schaden pro Waffe?" — Row @0x8006ec28: 2 für w1-w19, 100 für w21, 0 für w0/w20.
- „Krähen-HP?" — 0 (INIT @0x80112298) bzw. −1 (Latch @0x80115f90-98, vert≥5200); keine
  HP-Tabelle. Port aktuell ebenfalls 0 (scd_vm.c:2920).
- „Hit-Flinch?" — Existiert nicht (+0x4-Census §1.1).
- „steer[9]-Commit-Bedingungen?" — +0x1d2≠0 ∧ +0x1db≠0 ∧ +0x1dc<10000 ∧ +0x1d6<3
  (@0x8011330c-70); Port zeilengleich; das Gate, das live nie öffnet, ist +0x1d2 (F1).
- „Funktioniert der LOS-Sensor für Flieger?" — Im Original ja, weil der Ray horizontal auf
  Krähen-Y läuft (`sw zero,20(sp)` @0x8001b8d8) und dcc4 nach dem GEGNER-Band filtert —
  über Band 4 gibt es in 1170 keine Blocker. Der Port-Stand-in nutzt das SPIELER-Band mit
  invertierter Zellen-Semantik → Dauerblock (F1).
- „Wer setzt sub 9?" — steer[5] @0x80112bd4-d8 (y<perch−3600) und move[6]-Timer
  @0x80112dbc; Hänge-Punkt im Port ist NICHT der Weg nach 9, sondern das Parity-Gate (dyn:
  ss1-Maske 0x3f1 enthält 9).

## 7. Offen

1. **Live-Parity-Capture:** DuckStation-Re-Entry-Roster für 1170 (z3/125 per
   `re15_ss_patch.py` setzen, Raum neu betreten, `re15_enemy_state.py` mit +0x9a/+0x1db je
   0x21-Slot) — bestätigt HP-0/−1 und die parity-Kadenz am echten Gerät.
2. **FUN_8001bafc** (Projektion 0x8004f008 vs +0x1b4-Zelle) + Geometrie-Block
   @0x80012280-370 zu Ende disassemblieren (für F6).
3. **FUN_8003703c** Bucket-Masken exakt (Decompile zeigt 64-bit-Artefakte um das
   +0x9a-Vorzeichen) — Raw-Disasm vor dem F4-Aim-Umbau.
4. Runtime-Layout der SCA-Regionpointer (FUN_8003aea0 wandelt die 5 Datei-Counts in
   Region-Pointer; Steps 0-3 ⇒ Regionen 0-3): beim F1-Fix mit den Prefixsummen des
   Port-Parsers abgleichen (rdt_common.c:263-268).
