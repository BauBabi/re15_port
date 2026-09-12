# BEFUND B — Gruppe kopfschuss-nah: „nahe Headshots nach oben funktionieren nicht bei Zombies"

Nutzer 2026-09-12, Schrotflinte (w8), Zielen HOCH, kurze Distanz, Zombie. Analyse 2026-09-12.

## Kurzfassung (Wurzelursache)

Der Port wendet das RE1.5-Elevations-Band-Gate (`enemy.word0 & player.word0 & 0xE0000000`,
FUN_80011f50 @0x800120d0-ec) auf **alle** Ziele an — auch auf RE2-owned Zombies. Ein stehender
Zombie auf gleicher Bodenhoehe traegt nach dem byte-true ACTIVE-Tail-Stempel (FUN_80012aa4
@0x80012aa4, Aufrufer @0x80101600 mit R=0xbb8) **nie das UP-Band** (UP verlangt
`(playerY-enemyY)/1800 > 0`, d.h. Gegner >=1800 Einheiten ueber dem Spieler, @0x80012b9c).
Der Spieler-Aim ist exklusiv EIN Bit (UP=0x8000 @0x8003322c-38). Zielen HOCH auf einen nahen
Zombie => `pband & eband == 0` => Kandidat verworfen, `re15_player_weapon_fire` liefert 0 —
**der Schuss existiert fuer den Resolver gar nicht** (re15_damage.c:1236/1401).

Fuer RE1.5-owned Zombies ist das **byte-true** (das Original verhaelt sich identisch, Belege
unten). Fuer **RE2-owned Zombies ist es eine Port-Divergenz**: RE2s Trefferpfad hat **kein**
Hoehen-Band-Gate; seine Hoehen-Selektivitaet ist ein **vertikales Fenster pro Waffe und
Elevation** aus dem Schadensrecord — und das UP-Fenster der Schrotflinte ist
**[-5000, +500]**, schliesst also einen Zombie auf gleicher Hoehe (dy=0) EIN
(info/re2leon/PSX.EXE, Record @0x800A41A4). Der beruehmte RE2-Nah-Kopfschuss nach oben ist
mechanisch genau das: UP-Fenster-Unterkante +500 statt -2000, nur im Nah-Sub-Box-Bereich,
Zone = KOPF aus dem unteren Fenster-Drittel, Schaden 200 >= jede Zombie-HP => Kopf-Tod.

---

## 1. IST-Zustand des Ports

### 1.1 Das Band-Gate in `re15_player_weapon_fire` (re15_port/engine/src/re15_damage.c)

- `re15_player_weapon_fire` (re15_damage.c:1176). Elevation-Gate:
  - re15_damage.c:1236: `pband = (elev>0) ? 0x80000000 : (elev<0) ? 0x20000000 : 0x40000000`
    — **exklusiv ein Bit**, wie das Original (Producer-Zitate §2.1).
  - Zombie-Familie (0x10/11/12/13/16/18, re15_damage.c:1145): `eband =
    re15_band_stamp_aa4(pl, e, bdist, 0xbb8)` (re15_damage.c:1256), Implementierung
    re15_damage.c:1129-1143 = byte-true FUN_80012aa4:
    * `s0 = (pl->y - e->y) / 1800` (@0x80012ad8/b0c-b1c)
    * `(uint)s0 < 2` -> LEVEL 0x40000000 (@0x80012b20-44)
    * Ring `ring < dist < R+ring` -> `s0>0 ? UP : DOWN` (@0x80012b88-b8)
  - **Gleiche Bodenhoehe => s0=0 => eband = LEVEL (immer) + DOWN (dist<3000). UP NIE.**
  - re15_damage.c:1401: `if ((pband & eband) == 0) continue;` => bei Zielen-HOCH faellt der
    nahe Zombie aus der Kandidatenmenge, `best` bleibt -1, Rueckgabe 0. Kein Resolver-Lauf,
    kein Crit, kein Stempel, nichts.
- Es gibt **kein** UP-Band-Distanz-Gate, das nahe Ziele verfehlt — das UP-Band existiert fuer
  bodengleiche Zombies schlicht nicht. Das Gate gilt fuer **alle Flavors** — die im
  Kriech-Root-Kommentar selbst zitierte RE2-Tatsache ("RE2s EIGENER Kandidatenfilter
  FUN_800470C0 hat ueberhaupt kein Hoehen-Band", vier Gates @0x8004712c/38/48/60) wurde nur
  fuer die Liege-/Kriech-Ausnahme angewendet, nicht fuer das UP-Band stehender Zombies.

### 1.2 Was NACH dem Gate passieren wuerde (der Rest der Kette ist intakt)

- Crit: re15_damage.c:1444 u. 1483 — `weapon 8 && best_dist < 3000` => `hit_react |= 0x40`,
  `type < 0x20` => `hp = -1` (byte-true LAB_80012370 / @0x800124fc-2c) => `state = 3` DEATH.
- `re15_re2_stamp_hit` (re15_damage.c:1655, RE2-owned only): re15_damage.c:1700 stempelt
  `+0x1D2 = elev>0 ? 2 : elev<0 ? 0 : 1` — Zielen HOCH => **Zone 2 = KOPF**. (Seit 2026-09-12
  aus der Ziel-Elevation; die Byte-Herleitung, dass UP-nah wirklich Kopf ist, liefert §3.3.)
- Schrot-Zeile: w8 -> RE2-Attacken-Id **7** (`re2z_row_from_weapon[8]=7`,
  enemy_ai_re2_zombie.c:3604ff). Zelle der Tabelle @0x8010C940 fuer Zone 2/Bracket 0 =
  Spalte 2: **@0x8010CA44 (0x8010C940 + 7*36 + 2*4) = NULL** — im Original-Dump
  (enemy_ai_re2_zombie.c:3416ff, Zeile 7: `7438 66FC - | 7438 5BC0 - | 7438 5438 -`).
  * **Nah (<3000): unkritisch** — der RE1.5-Crit toetet (`hp=-1`, state=3), `survived=0`,
    `re2z_row_guard` (enemy_ai_re2_zombie.c:3676) gibt Zeile 7 ungeprueft zurueck, die
    DEATH-Wurzel liest +0x1D2=2 => Kopf-Tod. Die NULL-Zelle wird nie dispatcht.
  * **Fern (>3000), falls UP kuenftig durchgelassen wird und der Zombie UEBERLEBT**: Zelle
    [7][2] NULL => Fallback Zeile 8 (enemy_ai_re2_zombie.c:3630) — **deren Spalte 2 ist AUCH
    NULL** (Zeile 8: `5 3 0 | 5 2 0 | 5 2 0`) => stummer Treffer. Im RE2-Original ist diese
    Kombination **unerreichbar** (Beleg §3.4: UP-Mitte/Fern-Fenster schliessen dy=0 aus);
    der Port-Fix muss dieselbe Unerreichbarkeit herstellen (Plan §4).

**Fazit IST**: Der einzige Blocker ist das Band-Gate in der Kandidatenschleife. Die
Kopf-Kill-Kette dahinter (Crit + Zone-2-Stempel + Death-Root) steht bereits.

---

## 2. Original RE1.5 — FUN_80011f50 und die Producer

### 2.1 Spieler-Elevation = exklusives Bit (kein Mehrfach-Band)

Aim-Update (EXE, Waffen-FSM, ghidra1_V2.txt):
```
8003322c  ori v0,v0,0x8000     ; PAD & 0x10 (HOCH):  acaec = (acaec & 0x1fff) | 0x8000
80033238  sh  v0,DAT_800acaec
80033274  ori v0,v0,0x2000     ; PAD & 0x20 (TIEF):  ... | 0x2000
80033284  sh  v0,DAT_800acaec
800332c0  ori v0,v0,0x4000     ; sonst (EBEN):       ... | 0x4000
800332c8  sh  v0,DAT_800acaec
```
FUN_80011f50 uebernimmt: `DAT_800aca54 = acaec<<16 | aca54 & 0x1fffffff`
(RE_15_Quellcode_V2/FUN_80011f50.c:38) => Bit31 UP / Bit30 LEVEL / Bit29 DOWN. Bestaetigt
durch den Feuer-Clip-Selektor `acae8 = 7 + (acaec>>15)*2 + ((acaec>>11)&4)` @0x800334a8-c8
(7 eben / 9 hoch / 11 tief). Die 3 Schrot-Zusatz-Resolves laufen im selben Resolver
(acae9==3/5/7 -> `jal FUN_80011f50` @0x80033508-58) — gleiche Band-Logik, keine Ausnahme.

### 2.2 Das Kandidaten-Gate (die gesuchte "Hoehenzeile" ist ein BIT-Schnitt, keine Distanz)

```
800120d0  lw  v0,0(s0)             ; enemy word0
800120d8  lw  v1,DAT_800aca54      ; player word (Bits 29-31 = Aim)
800120e0  and v0,v0,v1
800120e4  lui v1,0xe000
800120e8  and v0,v0,v1
800120ec  beq v0,zero,LAB_80012128 ; kein gemeinsames Band -> Kandidat verworfen
```

### 2.3 Das Gegner-Band des Zombies: UP nur >=1800 ueber dem Spieler

ACTIVE-Tail STAGE1 @0x801015c0-40: `jal 0x80012aa4 / a0=0xbb8` @0x80101600-04 (immer),
Downed-Nachlauf @0x80101628-3c. FUN_80012aa4 @0x80012aa4:
`s0 = (playerY - enemyY)/1800`; `(uint)s0<2` => LEVEL; Ring `|s0|*1000 < dist < R+|s0|*1000`
=> `s0>0 ? UP(0x80000000) @0x80012b9c : DOWN(0x20000000) @0x80012bb8`.
**Bodengleich (s0=0): LEVEL + (dist<3000: DOWN). UP unerreichbar.**

### 2.4 "Zeile +0x58" der Reichweiten-Tabelle @0x8006e5a0 ist KEINE Elevations-Zeile

`uVar3 = *(u16*)(0x8006e5a0 + w*4 + ((DAT_800aca5c & 4)!=0)*0x58)` (FUN_80011f50.c:54).
`DAT_800aca5c` ist die **Charakter-Id** (per-Char-Tabelle `table[DAT_800aca5c]` @0x800c00d4;
Nibble-Store @0x8003978c). Beide Baenke selbst gedumpt (info/Re1.5/PSX.EXE, t_addr
0x80010000): **bank0 == bank1** = {1000,1100,1000,1000,1100,1000,1200,1000,**1500**,...} —
byte-identisch. RE1.5 hat also **keine** Reichweiten-/Hoehenzeilen je Elevation; die gesamte
vertikale Selektivitaet ist das Band-Gate aus 2.2/2.3. Der Gun-Tester FUN_80012574 ist ein
reiner XZ-Streifen ohne Vertikalanteil (re15_gun_wedge_inside, re15_damage.c:1070ff).

### 2.5 RE1.5-Crit (zur Einordnung)

LAB_80012370: `+0x93 &= 1; ((w==8 && DAT_8008f5e0 < 3000) || w==7) -> |= 0x40`; spaeter
`(bit0x40 && type<0x20) -> hp=0xffff` (FUN_80011f50.c:145-172). Der Crit haengt NUR an
Distanz+Waffe, nicht an der Elevation — in RE1.5 macht man den Nah-Insta-Kill mit **EBEN**
gehaltener Waffe. **Nahe Kopfschuesse nach OBEN gibt es im RE1.5-Original nicht** (Band-Gate).
Der vom Nutzer erwartete Move ist die RE2-Mechanik.

---

## 3. Original RE2 — der Nah-Kopfschuss nach oben, vollstaendig hergeleitet

Alle Adressen info/re2leon/PSX.EXE (t_addr 0x80010000), Dumps selbst erstellt 2026-09-12.

### 3.1 Kein Band-Gate; Elevation waehlt einen GEOMETRIE-Record

- Kandidatengates des Hitscan-Appliers FUN_800410cc: aktiv (word0&1), +0x1D3==0, HP>=0,
  +0x10E&0xC000==0 — **kein Hoehen-Band** (RE2_Quellcode_V2/FUN_800410cc.c; deckungsgleich
  FUN_800470C0 @0x8004712c-64).
- FUN_80047C6C: `s2 = player.word0 >> 0x1d` @0x80047d18 (gleiches Bit-Layout wie RE1.5:
  1=DOWN/2=LEVEL/4=UP), `grp = DAT_800a6f8c[s2]` @0x80047ee0 — Dump: `[FF,0,1,FF,2,0,0,0]`
  => DOWN->0, LEVEL->1, UP->2. `a2 = 0x800A68E8 + item_id*24 + grp*8` @0x80047ecc-f8
  (Item-Id aus +0x10E; RE1.5-w8 <-> RE2-Id 7 = Schrotflinte, Mapping enemy_ai_re2_zombie.c).
- Schrot-Geometrie @0x800A6990: DOWN=0x800a6724, LEVEL=0x800a6740, **UP=0x800a675c**,
  Keyframes gemeinsam 0x800a6778 (`00 01 ff ...` = nur Frame-Record 0).

### 3.2 Der UP-Record + das vertikale Fenster: nah JA, fern NEIN

UP-Record @0x800a675c (0x1c Bytes): Sub-Box-Flags **0x10 / 0x20 / 0x40**, XZ-Boxen
`[100,0,750,500] / [3100,0,1500,750] / [9100,0,2500,1000]` (Nah/Mitte/Fern; Box-Masse werden
in FUN_800410cc um Gegner-Radius `+0x9a>>2` bzw. `+0x1EE>>2` erweitert).
FUN_80041b20 uebersetzt das Flag in ein Fenster ueber `dy = enemy_y - player_y` aus dem
**Schadensrecord** (pro Gegnertyp x Waffe):

Zombie-Record `PTR_DAT_800a6a88[0x10] = 0x800A412C`, Schrot Id 7 @0x800A41A4:
Schaden 200/60/40 (Bracket = Sub-Box, `(word0 >> bracket*10) & 0x3ff`), Fenster-Paare
+8/+0xc/+0x10 = **UP [-5000,+500] / LEVEL [-3000,+2000] / DOWN [-500,+3000]**
(PSX-Y: negativ = oben).

- Flag 0x10 (Nah-Box, FUN_80041b20 case 4) -> Fenster **voll**: dy in [-5000, +500] =>
  **dy=0 (gleiche Hoehe) LIEGT DRIN** — der bodengleiche Zombie ist beim Zielen-HOCH nah
  treffbar.
- Flag 0x20 (Mitte, case 5) -> Fenster um 1 Drittel (1833) nach oben verschoben:
  [-6833, -1333] => dy=0 **draussen**.
- Flag 0x40 (Fern, case 6): [-8666, -3166] => dy=0 **draussen**.

**Antwort auf "Reichweiten des UP-Bands beim Zombie"**: Zielen-HOCH trifft bodengleiche
Zombies in RE2 **nur im Nah-Sub-Box-Bereich** (XZ-Box ab z=100, Tiefe 750 + Radius-Anteil;
die Mitte-Box beginnt erst bei 3100). Auf Mittel-/Ferndistanz trifft UP nur Ziele
1333-8666 Einheiten **ueber** dem Spieler (Kraehen, Licker an der Decke).

**Die Waffenklassen-Pointe** (Dump aller 19 Zeilen @0x800A412C): das UP-Fenster-Unterende
ist **+500 nur fuer die Ids 7/8 (Schrotflinten) und 15/18 (SMG/Gatling)**; Handguns
(2/3/4/13/19), Magnums (5/6), Bowgun (12) haben UP=[-5000,**-2000**], Messer (1)
[-3000,-500] => mit der Pistole nach oben zielen verfehlt den bodengleichen Zombie AUCH in
RE2. Der Nah-Kopfschuss-nach-oben ist eine **Schrot-Klassen-Eigenschaft**, keine allgemeine
Regel.

### 3.3 Warum der Nah-UP-Treffer KOPF ist (Zone byte-hergeleitet)

FUN_800410cc partitioniert das Fenster in Drittel (`iVar8 = 6/3/0`, Vergleiche gegen
`lo + n*Drittel`) und liest Masken `DAT_800a6db4 + iVar8 (+9 bei Flag&8)` gegen
`enemy.word0 >> 26 & 7` (Teile-Maske des Gegners; Bit28 = 0x10000000 = das dokumentierte
"Kopftreffer erlaubt"-Bit, Konsument auch @0x800472E0). Dump DAT_800a6db4 (2 Saetze x 3
Zeilen x 3 Prioritaets-Bytes, Teil = Byte>>1, Prioritaet pbVar6[0] > [1] > [2]):
```
Satz0: row0 (unteres Drittel)  [4,2,1]  -> Prioritaet KOPF(2), Rumpf(1), Beine(0)
       row3 (Mitte)            [2,1,4]  -> Rumpf zuerst
       row6 (oberes Drittel)   [1,2,4]  -> Beine zuerst
Satz1 (+9, Flag&8=LEVEL-Records): [4,2,0] / [2,0,0] / [1,2,0]
```
UP-Fenster [-5000,+500], dy=0 => unteres Drittel (0 >= +500-1833 = -1333) => row0 =>
**Teil 2 = KOPF**. LEVEL-Fenster [-3000,+2000], dy=0 => Mitte => Satz1-row3 [2,0,0] =>
**Rumpf**. DOWN, dy=0 => oberes Drittel => **Beine**. Der Port-Stempel "elev->Zone"
(UP->2/LEVEL->1/DOWN->0, re15_damage.c:1700) ist fuer den bodengleichen Fall damit
**byte-hergeleitet korrekt**. `+0x1D2 = 3*bracket + zone` und die Poise-Abzuege
+0x153/+0x152/+0x151 je Teil stehen im selben Applier (FUN_800410cc-Tail).

### 3.4 Warum die NULL-Zellen [7][c2/c5/c8] im Original unerreichbar sind

Kopf-Zone + Schrot: nur Nah-Sub-Box (3.2) => Bracket 0 => Schaden 200 >= max. Zombie-HP 118
(@0x8010C600) => `+0x4 = 3` DEATH (@0x8004727C-90) => die HURT-Wurzel dispatcht nie =>
`+0x1D2 = 0*3+2 = 2` erreicht nur die DEATH-Wurzel (Kopf-Tod/Enthauptung). c5/c8 (Kopf in
Bracket 1/2) sind durch die verschobenen Fenster ausgeschlossen. Der `jalr`-Dispatch
@0x801053E0-410 kann die NULL-Zellen also nie laden — sie sind Beweis, nicht Loch.

---

## 4. Fix-Plan (Ort + Beleg je Schritt)

1. **re15_damage.c, Band-Gate-Block (1233-1401)**: fuer `re2_owned` Zombie-Familie
   (`re15_ai_re2_for_type && re15_re2z_owns_type` — dieselbe Bedingung wie die
   Kriech-Root-Ausnahme) das aa4-Band-Intersect ersetzen durch den RE2-Vertikalfenster-Test:
   `dy = e->y - pl->y` gegen das Fenster der Elevation aus einer neuen statischen Tabelle
   (Quelle: Zombie-Record @0x800A412C + (id-1)*20, Paare +8/+0xc/+0x10; id via
   `re2z_row_from_weapon[weapon]`; fuer w8->7: UP[-5000,500]/LEVEL[-3000,2000]/
   DOWN[-500,3000]). RE1.5-owned Zombies behalten das byte-true aa4-Gate unveraendert.
2. **UP-Distanzstruktur (nah-only)**: die Sub-Box-Fenster mitfuehren — UP-Mitte/Fern um je
   ein Drittel verschoben (FUN_80041b20 case 5/6, Flags 0x10/0x20/0x40 @0x800a675c),
   Bracket = erste Sub-Box, deren XZ-Bereich UND Fenster passen. Die XZ-Sub-Box-Grenzen
   (Nah ab z=100, Mitte ab 3100) erst nach dem 41ce4-Decode festnageln (offen, §5) —
   KEINE geratene Grenze einbauen.
3. **Zone**: Stempel re15_damage.c:1700 bleibt (UP->2 jetzt byte-hergeleitet, §3.3);
   optional die Drittel-Formel (dy gegen lo+n*Drittel) statt roher Elevation, dann traegt
   auch der Fall "Gegner oberhalb, LEVEL-Aim" die richtige Zone.
4. **Wache**: Distanz-Sweep-Probe nach dem Muster probe_re2z_bandlock.c (ROOM1140,
   `re15_ai_flavor_set(RE15_AI_FLAVOR_RE2)`, `re15_player_set_aim_elevation_for_test(+1/0)`,
   `re15_player_set_equipped_weapon(8)`, `re15_player_weapon_fire(8)`; Distanzen 400..6000):
   UP-nah => Treffer, `re2z_hits1d2==2`, `state==3`; UP-fern => 0 Treffer; LEVEL
   unveraendert; RE1.5-Flavor: UP-nah weiterhin 0 (byte-true Negativ-Pin).
5. **Doku**: am `re2z_row_guard`-Fallback (enemy_ai_re2_zombie.c:3630) vermerken, dass
   Spalte-2-NULL nach diesem Fix wieder original-unerreichbar ist (Fallback 8 hat c2=NULL).

## 5. Offen (kein Blocker fuer den Kern-Fix)

- Exakter Decode der XZ-Quad-Pruefung FUN_80041ce4 (Semantik b[0..3] + Radius-Anpassungen
  `iVar10+8/+0xa/+0x12/+0x1a` in FUN_800410cc) — noetig fuer die byte-true Nah/Mitte-Grenze.
- Producer der RE2-Zombie-Teile-Maske word0 Bits 26-28 (bisher nur konsumenten-belegt:
  FUN_800410cc `>>0x1a&7`, FUN_800470C0 `&0x10000000`).
- Gleichbehandlung der RE1.5-Waffen w12/w19 (-> RE2-Ids 15/18, UP-Fenster ebenfalls
  [-5000,+500]): Dauerfeuer nach oben nah — gleiche Fix-Schiene, eigener Abnahme-Fall.

## 6. Mess-Weg (Reproduktion vor dem Fix)

`re15_port/tests/unit/probe_re2z_bandlock.c` ist die Vorlage (setup + frame() +
Elevations-Hook). Neuer Treiber `probe_schrot_up_sweep.c`:
Spieler vor stehenden Zombie (state 1, sub<=2 — Fixture-Anker wie test_re2_livepath.c:315ff),
`rot_y` auf den Gegner, Sweep `dist in {400,800,...,6000}` x `elev in {+1,0}` x Flavor
{RE2, RE15}; Messgroessen: Rueckgabe `re15_player_weapon_fire(8)`, `e->hp`, `e->state`,
`e->re2z_hits1d2`, `re15_re2z_last_hit_handler()`. Erwartung VOR dem Fix: RE2-Flavor,
UP-Spalte durchgehend 0 (die Luecke), LEVEL-Spalte trifft mit Crit <3000. Erwartung NACH dem
Fix: UP-nah = Treffer/Kopf/DEATH, UP >= Nah-Grenze = 0, RE15-Flavor unveraendert.
