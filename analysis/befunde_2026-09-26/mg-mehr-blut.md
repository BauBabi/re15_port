# Maschinengewehr: in RE2 viel mehr Blut bei Zombie-Treffern — der Port gibt dem MG den KLEINSTEN Blutstoss des ganzen Spiels, das Original den GROESSTEN

**Datum:** 2026-09-26 · **Status:** Mechanismus BELEGT (Original-Seite byte-gelesen, Port-Seite
code-verifiziert). Keine Engine-Aenderung in diesem Lauf.
**Quellen, alle selbst nachgelesen:** `info/re2leon/COMMON/BIN/EMZ0.BIN` (RE2-Zombie-Overlay,
roh @0x80100000), `info/re2leon/PSX.EXE` (RAM = 0x80010000 + off − 0x800),
`info/Re1.5/PSX/BIN/STAGE1.BIN` (RE1.5-Overlay, roh @0x80100000),
`RE2_Quellcode_V2/FUN_8001bf10.c` (nur als Hinweis; die Packung ist am Byte gegengeprueft).
Werkzeug: `.claude/skills/re15-psx-disasm/scripts/re2_disasm.py` + eigene Wort-Zensus-Skripte.

---

## 0. Kurzantwort

RE2 waehlt die Trefferreaktion des Zombies ueber eine **2D-Tabelle @0x8010C940**, deren ZEILE die
**Item-Id der Waffe** ist. SMG (Id 15) und Gatling (Id 18) sind die **einzigen** beiden Zeilen, die
auf den Handler **`0x80107EF0`** zeigen. Dieser Handler spawnt seinen Blutstoss mit

* **zufaelliger Groesse** `scale = 7000 + (rand<<3)` (bzw. `6096 + (rand<<3)`), also **7000…9040**
  statt der **festen 6096** der Pistolen-Zeile,
* **zufaelligem Sprite-Satz** `sub = rand & 1` (die Pistole nimmt immer sub 0),
* **zufaelligem Positions-Versatz** (drei RNG-Zuege, Streuung ~±256 X/Z und 1020 Y) statt des
  festen `{0, 800, 0}` der Pistole,

und weil der Applier bei JEDEM Treffer das ganze Zustandswort ueberschreibt (`sw v0,4(s1)`
@0x80047288), laeuft Phase 0 — und damit der Blutstoss — **pro Patrone** erneut; SMG/Gatling
verbrauchen alle **8 Frames** eine Patrone (@0x8006A144-58).

**Der Port macht das Gegenteil:** sein 7EF0-Handler ruft `re2z_blood_fx_at()` →
`re2z_blood_fx_scaled(…, 0x1500, 0)` — Groesse **5376**, **null** Splatter-Sprites, fester sub 0,
kein Versatz. Der Pistolen-Pfad desselben Ports ruft `re2z_blood_fx_dir()` →
`re2z_blood_fx_scaled(…, 0x2000, 8)` — Groesse **8192 plus 8 Splatter-Sprites**.
Im Port erzeugt eine MG-Kugel also **1 kleines Sprite**, eine Pistolenkugel **9 groessere** —
genau das umgekehrte Verhaeltnis zum Original. **Und `0x1500` traegt keine `@0x…`-Adresse**; der
Code flaggt das selbst als offen (`enemy_ai_re2_zombie.c:1010-1015`).

---

## 1. Was der Nutzer meldet / was ich gemessen habe

Nutzer: *"Das Maschinengewehr sorgt in Resident Evil 2 fuer viel viel mehr Blut bei den Zombies
bei treffern."*

### 1.1 Roh-Byte-Zensus: wo ueberhaupt Blut gespawnt wird

Blut-/Effekt-Spawner in RE2 ist `FUN_8001bf10`, der Zwilling von RE1.5 `FUN_80019700`. Packung
(im Decompilat `RE2_Quellcode_V2/FUN_8001bf10.c` und am Byte bestaetigt):
`id = a0>>24`, `sub = (a0>>16)&0xff`, `scale16 = a0&0xffff` (Q12).
`jal 0x8001bf10` = Wort `0x0C006FC4`; `jal 0x80019700` = Wort `0x0C0065C0`.

| Binary | Spawn-Aufrufe |
|---|---|
| `info/re2leon/COMMON/BIN/EMZ0.BIN` (RE2-Zombie) | **88** |
| `info/re2leon/PSX.EXE` | 146 (davon **kein einziger** Blut-Spawn am Gegner — s. §2.5) |
| `info/Re1.5/PSX/BIN/STAGE1.BIN` (RE1.5) | **98** |

---

## 2. Das Original — mit Adressen und Bytes

### 2.1 (a) Wo der Treffer-Effekt gestartet wird und welcher Parameter von der Waffe abhaengt

HURT-Wurzel des RE2-Zombies = `FUN_80104F40` (Zustandstabelle @0x8010C830[2]).
Selbst disassembliert (`re2_disasm.py dis 0x80104f40 … --bin EMZ0.BIN`):

```
80104fe0: lhu  a0,270(s1)          ; +0x10E
80104fe8: andi v0,a0,0x1           ; Kriecher?
80104fec: beq  v0,zero,0x80105014
80104ff4: lbu  v0,5(s1)            ; ZEILE = +0x5
80105008: lw   v0,-13336(at)       ; 1D-Kriechertabelle 0x8010CBE8[+0x5]
8010500c: j    0x8010540c          ; -> jalr @0x80105410
```
Der normale (stehende) Pfad dispatcht weiter unten dieselbe Zeile ueber die **2D-Tabelle
`0x8010C940 + Zeile*36 + Spalte*4`**.

**Die Zeile IST die Waffe.** Der Stempel sitzt im Applier der EXE, selbst gelesen
(`re2_disasm.py dis 0x80041a40 60`):
```
80041a9c: sb   v1,466(t0)          ; em+0x1D2 = Trefferzone (= SPALTE)
80041aa0: lw   t0,64(sp)
80041aa8: srl  v1,t0,16            ; Treffercode >> 16
80041ab0: addiu v1,v1,1            ; +1
80041ab4: sb   v1,5(t0)            ; em+0x5 = RE2-ITEM-ID DER WAFFE  (= ZEILE)
```

### 2.2 (b) Der Wert des MG gegen den der Pistole — die Tabelle selbst

Eigener Dump aus `EMZ0.BIN`, 19 Zeilen × 9 Spalten, Zeilenstride 36, ab **@0x8010C940**
(`-` = NULL-Wort):

```
Zeile  Adresse     Sp0    Sp1    Sp2  Sp3    Sp4    Sp5  Sp6    Sp7    Sp8     Waffe
  1..4 8010C964..  5438   5438   -    5438   5438   -    5438   5438   -       Messer / Handguns
  5,6  8010C9F4..  -      -      -    -      -      -    -      -      -       Magnum (900 Schaden = immer toedlich)
  7    8010CA3C    7438   66FC   -    7438   5BC0   -    7438   5438   -       Schrotflinte
  8    8010CA60    7438   66FC   -    7438   5BC0   -    7438   5BC0   -       Custom Schrot
  9    8010CA84    7438   5BC0   5BC0 5438   5438   5438 5438   5438   5438    GL Explosiv
 10,11 8010CAA8..  5BC0   5BC0   5BC0 5438   5438   5438 5438   5438   5438    GL Brand / Saeure
 12    8010CAF0    7438   703C   -    7438   703C   -    7438   703C   -       Bowgun
 13,14 8010CB14..  5438   5438   -    5438   5438   -    5438   5438   -       Colt SAA / Spark
 15    8010CB5C    7EF0   7EF0   -    7EF0   7EF0   -    7EF0   7EF0   -       ### SMG / Ingram
 16    8010CB80    5438   5438   -    5438   5438   -    5438   5438   -       Flammenwerfer
 17    8010CBA4    -      -      -    -      -      -    -      -      -       Rakete (immer toedlich)
 18    8010CBC8    7EF0   7EF0   -    7EF0   7EF0   -    7EF0   7EF0   -       ### GATLING
 19    8010CBEC    7888   7888   7888 7888   7888   7888 7888   7888   7888    (faellt mit der 1D-Kriechertabelle @0x8010CBE8 zusammen)
```
Kurzform: `5438` = `0x80105438`, `5BC0` = `0x80105BC0`, `66FC` = `0x801066FC`,
`703C` = `0x8010703C`, `7438` = `0x80107438`, `7EF0` = `0x80107EF0`, `7888` = `0x80107888`.

**Nur die Zeilen 15 und 18 benutzen `0x80107EF0`.** Damit ist der per-Waffe-Parameter belegt:
Die Waffen-Id waehlt die Reaktions-Funktion, und die Reaktions-Funktion bestimmt den Blutstoss.
(Diese Tabelle steht im Port bereits byte-identisch: `enemy_ai_re2_zombie.c:3760 re2z_hit_tbl`.)

### 2.3 Der Blutstoss im MG-Handler `0x80107EF0` gegen den der Pistole `0x80105438`

**Pistole / MAIN `0x80105438`**, selbst gelesen:
```
80105650: lbu  v1,466(s1)          ; +0x1D2
80105654-74: v1 = +0x1D2 % 3       ; 0xAAAAAAAB-Magic
80105678: beq  v1,zero,0x801056dc
8010567c: addiu a0,zero,6096       ; a0 = 0x17D0  -> id 0, sub 0, scale FEST 6096
801056e0: addiu v0,zero,800        ; Offsetvektor FEST {0, 800, 0}
801056ec/f0/f4: sh zero,16(sp) / sh v0,18(sp) / sh zero,20(sp)
80105708: jal  0x8001bf10
```
→ **ein** Spawn, **feste** Groesse 6096 (1.488 in Q12), **fester** sub 0, **fester** Offset.

**MG / Gatling `0x80107EF0`**, selbst gelesen (Delay-Slots aufgeloest):
```
80107fc8-ff0: a0 = +0x1D2 % 3
80107ff4: beq  a0,zero,0x80108068           ; Zone%3 == 0 -> Zweig B

; ---- Zweig A (Zone%3 != 0; das ist der RUMPFTREFFER) ----
80108000: jal RNG   / 80108004: addiu s0,zero,256
80108008/0c:  v0 = 256 - 2*rand              -> sh 16(sp)   (X)
80108010: jal RNG
80108018/1c/20: v1 = 312 - 4*rand            -> sh 18(sp)   (Y)
80108024: jal RNG
8010802c/30: s0 = 256 - 2*rand               -> sh 20(sp)   (Z)
80108034: jal RNG
8010803c: jal RNG   / 80108040: addu s0,v0,zero
80108044: andi s0,s0,0x1
80108048: sll  s0,s0,16                      ; sub = rand & 1     <-- ZWEI Sprite-Saetze
8010804c: sll  v0,v0,3
80108050: addiu v0,v0,7000                   ; scale = 7000 + (rand<<3)
80108054: or   a0,s0,v0
80108058: addiu a3,sp,16                     ; a3 = ZUFALLS-Offsetvektor
80108064: addiu a2,s1,72                     ; Anker = Part 0
; ---- Zweig B (Zone%3 == 0; Beintreffer) ----
8010806c..80108098: X = 127-rand, Y = 927-rand, Z = 127-rand  (drei RNG-Zuege)
801080a4/a8: sub = rand & 1
801080ac/b0: scale = 6096 + (rand<<3)
801080c0: addiu a2,s1,244                    ; Anker = Part 1
801080c4: jal  0x8001bf10
```
RNG `FUN_80015fe8` liefert **0..255** (`andi v0,v0,0xff` @0x80016004, `jr ra` @0x80016020) —
selbst disassembliert. Also:

| | Groesse (Q12-Rohwert) | sub | Positions-Versatz | Anker |
|---|---|---|---|---|
| Pistole (Zeilen 1-4,13,14,16) | **6096** fest | 0 fest | `{0,800,0}` fest | Part 0 bzw. Part 1 |
| **MG Zweig A (Rumpf)** | **7000 … 9040** | **rand&1** | **zufaellig** ±256 X/Z, 1020 Y | Part 0 |
| **MG Zweig B (Beine)** | **6096 … 8136** | **rand&1** | **zufaellig** | Part 1 |

Mittelwert MG-Rumpf ≈ 8020 = **1,32×** die Pistolen-Kantenlaenge ≈ **1,73×** die Flaeche —
und zusaetzlich gestreut, statt an einem Punkt.

**Wichtiger Nebenbefund zur Menge:** `FUN_8001bf10` spawnt pro Aufruf NICHT ein Sprite, sondern
`uVar1` Slots — die Klon-Schleife `iVar13 = uVar1 - 1; … while (iVar13 != 0)` im Decompilat
(gegengeprueft an der Slot-Suchschleife `uVar12 = 0x60` = 96 Slots). `uVar1` kommt aus dem
**sub-Record** der ESP-Bank (`puVar14 = base + tbl[(a0>>16)&7]*4`, `uVar1 = *puVar14`). Da das MG
`sub` per RNG zwischen 0 und 1 wechselt, wechselt es damit auch zwischen **zwei Sub-Records mit
je eigener Sprite-Zahl**; die Pistole nimmt immer denselben.

### 2.4 Warum es sich pro Patrone wiederholt (die eigentliche „viel viel mehr"-Ursache)

Der Applier schreibt bei jedem Treffer das **ganze Zustandswort**, nicht nur das Zustandsbyte
(selbst gelesen, `re2_disasm.py dis 0x80047240 42`):
```
8004727c: lh   v1,342(s1)          ; HP
80047280: addiu v0,zero,2
80047284: bgez v1,0x80047294
80047288: sw   v0,4(s1)            ; +0x4 = 2  UND  +0x5 = 0, +0x6 = 0, +0x7 = 0
8004728c: addiu v0,zero,3
80047290: sw   v0,4(s1)            ; (HP<0) +0x4 = 3
80047294: addiu v0,zero,1
80047298: sb   v0,466(s1)          ; +0x1D2 = 1 (Grundzone RUMPF)
```
`+0x6` ist im RE2-HURT die **Reaktions-PHASE** (`0x80107EF0` verzweigt @0x80107F40 auf sie und
setzt sie in Phase 0 auf 1 @0x80107F98). Das Wort-`sw` setzt sie also **bei jeder Patrone auf 0
zurueck** → Phase 0 laeuft erneut → **ein neuer Blutstoss pro Patrone**.

Die Kadenz: `FUN_8006A0CC` (Abzug/Munition), selbst gelesen:
```
8006a124: addiu v0,zero,15
8006a128: beq  s1,v0,0x8006a138     ; Waffe 15 = SMG
8006a12c: addiu v0,zero,18
8006a130: bne  s1,v0,0x8006a184     ; Waffe 18 = Gatling
8006a13c: lhu  v0,23580(v0)         ; Zaehler @0x800D5C1C
8006a144: addiu v0,v0,1
8006a158: slti v0,v0,8
8006a15c: bne  v0,zero,0x8006a218    ; < 8 -> KEIN Schuss
8006a168: sh   zero,23580(at)        ; Zaehler zuruecksetzen
8006a178: jal  0x800694b8            ; eine Patrone verbrauchen
```
→ **eine Patrone alle 8 Frames**, dauerhaft, solange der Abzug gehalten wird. Bei 30 Hz sind das
3,75 Blutstoesse/s — und jeder davon ist der grosse, gestreute Stoss aus §2.3, waehrend die
Pistole pro Abzug einen einzigen festen Stoss macht.

### 2.5 Ausgeschlossene Fehlspuren (damit niemand sie nochmal graeben muss)

* Die 18 EXE-Funktionen `0x8004418C … 0x80045648` mit bis zu 13 Spawns, die ueber
  `jalr *(0x800A6FDC + (+0x10E & 0xFFF)*4)` (@0x800431BC-CC und @0x80043CF8-08) per **Waffen-Id**
  angesprungen werden, sind **NICHT** die Blutmenge: `0x8004418C` liest `lbu v1,333(s1)` (+0x14D
  = Rueckstossphase) und spawnt `0x030A0E00` / `0x01000E00` am **Spieler**-Part
  (`addiu s0,s0,1964` = +0x198 + 11*172 + 72) — das sind **Muendungsfeuer und Rauch**.
* Der **Poise-Bruch** in der HURT-Wurzel (@0x801050A0-0x80105160, zwei weitere Blut-Spawns
  6096 / 4096) ist fuer das MG **unerreichbar**: die Kosten-Tabelle @0x8010CC33, eigener Dump
  `8010cc30: 88 78 10 80 | 09 0f 0f 23 | 00 00 00 00 | 00 00 00 00 | 14 00 …`, gibt
  Zeile 1 = 9, Zeile 2/3 = 15, Zeile 4 = 35, Zeile 13 = 20 und **Zeile 15/18 = 0**. Das MG
  bricht die Standfestigkeit also nie.
* Das RE1.5-Ein-Treffer-Schloss `+0x93` existiert in RE2 gar nicht (Voll-Scan nach
  `sb/lbu rt,147(rs)` in `EMZ0.BIN` und `re2leon/PSX.EXE`: 0 Treffer — bereits im Port-Kommentar
  `enemy_ai_re2_zombie.c:1185-1191` dokumentiert und hier nachgeprueft). In RE2 kann also jede
  Patrone treffen.

### 2.6 Spawn-Zensus je Handler (EMZ0.BIN, Funktionsgrenzen ueber `addiu sp,sp,-N`)

| Handler | Rolle | `jal 0x8001bf10` |
|---|---|---|
| `0x80104F40` | HURT-Wurzel (Poise-Bruch + Gliedmassen) | 4 |
| `0x80105438` MAIN | Messer/Pistolen/Colt/Spark/Flammenwerfer | 1 |
| `0x80105BC0` STAGGER | Granaten, Schrot Sp4/7 | 5 |
| `0x801066FC` | Schrot Sp1 | 5 |
| `0x8010703C` | Bowgun Sp1 | 1 |
| `0x80107438` | Knockdown (Schrot/Bowgun Sp0/3/6) | 7 |
| **`0x80107EF0`** | **SMG (15) + Gatling (18)** | **1** (aber gross + gestreut + 2 Sub-Saetze) |
| `0x80107888` | Kriecher | 1 |

(Die uebrigen 63 Aufrufe liegen in Tod, Zerlegung, Feuer/Saeure und Kriech-Pfaden:
0x801025EC 2, 0x80103188 1, 0x80103B74 1, 0x80104D74 1, 0x80106128 5, 0x80106310 3,
0x8010640C 2, 0x80106510 7, 0x80107A78 2, 0x80108250 1, 0x80108530 3, 0x80108A14 1,
0x80108BEC 5, 0x801092C4 3, 0x80109610 5, 0x801099E4 2, 0x8010A05C 1, 0x8010A240 3,
0x8010A9B8 2, 0x8010AF58 2, 0x8010B464 3, 0x8010BCD0 2, 0x8010C0F8 4, 0x8010C2A4 2.)

### 2.7 (c) Wie es in RE1.5 ist — es gibt eine EIGENE, andere Tabelle

RE1.5 hat dieselbe Bauart, aber eine andere Belegung: 2D-Tabelle **@0x8011FB90**
(Zeile = `+0x5` = RE1.5-Waffen-Id, Zeilenstride **0x20**, 8 Spalten). Eigener Dump aus
`info/Re1.5/PSX/BIN/STAGE1.BIN`:

```
Zeile  Adresse     Sp0        Sp1        Sp2..7   RE1.5-Waffe
  1    8011FBB0    80105B7C   80105B7C   0        Combat Knife
  2    8011FBD0    0          0          0        Pipe
  3    8011FBF0    80105B7C   80105B7C   0        Browning HP
  4    8011FC10    80105B7C   80105B7C   0        SIG P228
  5    8011FC30    80106290   80106290   0        Beretta M93R (3-Schuss)
  6    8011FC50    80106290   80106290   0        Glock 18 (Vollauto)
  7    8011FC70    0          80106624   0        Super Redhawk .44
  8    8011FC90    0          80106624   0        Remington M870
 12    8011FD10    0          0          0        ### INGRAM M10 — KOMPLETT NULL
 19    8011FDF0    80106048   80106048   0        H&K MC51 (Vollauto)
 (Zeile 21 @0x8011FE30 ist keine Zeile, sondern die Poise-Step-Tabelle)
```
Blut-Spawns je RE1.5-Handler (eigener Zensus, `jal 0x80019700` in STAGE1.BIN):
`0x80105B7C` (Pistolen) **2**, `0x80106290` (Beretta/Glock) **4**, `0x80106624` (Magnum/Schrot)
**2**, `0x80106048` (MC51) **2**.

**Aussage dazu:** auch RE1.5 kennt „schnelle Waffe = mehr Blut" — der Beretta-/Glock-Handler
`0x80106290` spawnt doppelt so viel wie der Pistolen-Handler. Die **Ingram-Zeile 12 ist aber
leer**, d.h. im RE1.5-Auslieferungsstand hat ausgerechnet die MP **gar keine** eigene
Trefferreaktion und damit auch kein eigenes Blut.

**Welcher Bestand ist massgeblich?** Klar: **der RE2-Bestand.** Die Port-Option OPTIONS→AI steht
seit 2026-08-22 auf Default RE2 (Memory `reai-v2-re2-ai-option`), der ausgelieferte Zombie faehrt
also `re2z_hurt` = `FUN_80104F40` und die Tabelle @0x8010C940. Der Nutzer vergleicht mit RE2, und
der Port laeuft im Default mit RE2 — die Referenz fuer den Fix ist `0x80107EF0`. Fuer den
RE1.5-Modus ist @0x8011FB90 die Referenz, und dort ist die korrekte Antwort fuer die Ingram
„keine eigene Reaktion" (Zeile 12 = NULL), nicht „mehr Blut".

---

## 3. Was der Port tut — mit datei.c:zeile

### 3.1 Die Kette bis zum Blut

1. `re15_port/engine/src/game_step_common.c:1793-1841` — Dauerfeuer fuer `eq == 12 | 14 | 19`;
   pro Schuss `re15_player_weapon_fire(eqa)` (`:1831`).
   Takt: `re15_port/engine/src/player_common.c:423` `*schuss = ((f & 4) == 0)` — der RE1.5-Takt
   (Bit 2 des In-Clip-Bildzaehlers, @0x800349ac-d0), also **4 Schuesse je 8 Frames**, nicht
   RE2's 1 je 8 Frames.
2. `re15_port/engine/src/re15_damage.c:2128-2168` — Treffer: `e->state = 2`, `sub_state_3 = 0`,
   dann `re15_re2_stamp_hit(e, 0, weapon_id)` (`:2163`).
3. `re15_port/engine/src/re15_damage.c:2371` — `e->sub_state_2 = 0u;` (Beleg `sw v0,4(s1)`
   @0x80047288) und `:2373-2375` `e->sub_state_1 = re15_re2z_row_for_weapon(...)`.
   Waffe 12 (Ingram) → Zeile **15**, Waffe 19 (MC51) → Zeile **18**
   (`enemy_ai_re2_zombie.c:3736 re2z_row_from_weapon`).
4. `re15_port/engine/src/enemy_ai_re2_zombie.c:6890` — `case RE2ZH_7EF0: re2z_hit_light(e, pl);`
5. `re15_port/engine/src/enemy_ai_re2_zombie.c:4942` —
   `re2z_blood_fx_at(e, 1, (int16_t)e->rot_y);`
6. `re15_port/engine/src/enemy_ai_re2_zombie.c:1016-1019` —
   `static void re2z_blood_fx_at(...) { re2z_blood_fx_scaled(e, part, yaw, 0x1500, 0); }`

### 3.2 Der Vergleich im Port selbst

| Port-Pfad | Aufruf | Groesse | Splatter-Sprites | Versatz | sub |
|---|---|---|---|---|---|
| Pistole (MAIN, `:5068`) | `re2z_blood_fx_dir` → `:1124` | **0x2000 = 8192** | **8** | keiner | 0 |
| **MG/Gatling (7EF0, `:4942`)** | `re2z_blood_fx_at` → `:1018` | **0x1500 = 5376** | **0** | keiner | 0 |

`re2z_blood_fx_scaled` (`:993-1008`) spawnt `re15_esp_fx_spawn_ex(bank, 0, 0, scale16, …)` plus,
wenn `splatter_n > 0`, `re15_esp_fx_splatter(bank, 0, splatter_n, …)`.

**Wichtig — die 0x2000 der Pistole ist KEIN Fehler, sondern eine dokumentierte Entscheidung.**
`:1121-1123`: *„scale16 = 0x2000 (@0x80105c54 / @0x80105cbc, RE1.5-Praesentation) statt der
RE2-eigenen 6096 = 0x17D0 (@0x8010567C) — Mandats-Entscheidung, beide Werte oben zitiert."*
Der Port stellt Blut also bewusst in der RE1.5-Praesentation dar (Grundsprite + Splatter-Faecher),
weil die RE1.5-Bank kein Ein-Sprite-Aequivalent zur RE2-Bank hat. **Der Fehler ist, dass der
MG-Pfad diese Entscheidung nie mitbekommen hat**: er blieb auf dem alten, unbelegten Stand-in
`0x1500 / 0 Splatter` stehen. Der Praesentations-Faktor der Pistole ist
`0x2000 / 6096 = 8192/6096 = 1.3438`; auf den MG angewandt muesste der Stoss bei
`7000…9040` Basis **9406…12146** gross sein und ebenfalls den Splatter-Faecher tragen — statt
5376 ohne Faecher.

### 3.3 Die Konstante `0x1500` ist selbst-deklariert unbelegt

`enemy_ai_re2_zombie.c:1010-1015`:
> `⛔ OPEN (RESTBESTAND, seit Runde 4 stark geschrumpft): die 0x1500 traegt KEINE @0x…-Adresse.`
> `… hier haengen nur noch die Aufrufer OHNE ausgelesene Id … Wer eine davon fixt: erst die Id am
> Original auslesen.`

Genau das ist mit §2.3 jetzt getan — fuer den 7EF0-Aufrufer.

### 3.4 Zweiter Port-Fehler an derselben Stelle: der falsche Anker

Der Port haengt den Stoss fest an **Part 1** (`re2z_blood_fx_at(e, 1, …)`). Das Original waehlt
**nach `+0x1D2 % 3`**: `!= 0` → **Part 0** (`addiu a2,s1,72` @0x80108064), `== 0` → Part 1
(`addiu a2,s1,244` @0x801080C0). Der Port stempelt fuer den stehenden Rumpftreffer
`+0x1D2 = 1` (`re15_damage.c:2360-2368`) → `1 % 3 = 1` → **Zweig A = Part 0**. Der Port nimmt
also im Normalfall den falschen Knochen **und** die falsche Groessen-Basis (6096 statt 7000).

---

## 4. Der Unterschied, in einem Satz

Im Original ist der Blutstoss des MG der **groesste und einzige zufaellig gestreute** des Spiels
(`scale 7000+(rand<<3)`, `sub = rand&1`, drei RNG-Offsets, Anker Part 0, einmal **pro Patrone**),
im Port ist er mit `0x1500 = 5376` ohne Splatter, ohne Streuung, ohne Sub-Wechsel und am falschen
Knochen der **kleinste** — kleiner sogar als der Pistolenstoss desselben Ports
(`0x2000` + 8 Splatter).

---

## 5. Umsetzungsplan (konkrete Dateien/Zeilen, jede Konstante mit @0x)

Alle Aenderungen in **`re15_port/engine/src/enemy_ai_re2_zombie.c`**. Keine neue Datei noetig.

**Leitentscheidung:** die Praesentations-Entscheidung des Ports (RE1.5-Sprite + Splatter-Faecher,
`:1121-1123`) bleibt, weil sie bereits vom Nutzer abgenommen ist. Geaendert wird nur, was das
Original **relativ** anders macht: Groessen-Basis, Zufall, Streuung, Anker, Sub-Wechsel.

### Schritt 1 — den 7EF0-Blutstoss byte-true machen (`:4942`)

Ersetze in `re2z_hit_light` (Phase 0) den Aufruf
`re2z_blood_fx_at(e, 1, (int16_t)e->rot_y);` durch die Original-Rechnung. Vorlage, jede Zahl
mit Beleg:

```c
/* MG/Gatling-Blutstoss, byte-true FUN_80107EF0 @0x80107FC8-0x801080C4.
 * Zone = +0x1D2 % 3 (Magic-Multiply 0xAAAAAAAB @0x80107FCC-FF0, Verzweigung
 * `beq a0,zero,0x80108068` @0x80107FF4).
 * RNG = FUN_80015FE8, Rueckgabe 0..255 (`andi v0,v0,0xff` @0x80016004). */
{
    unsigned zone3 = (unsigned)e->re2z_hits1d2 % 3u;
    int32_t  ox, oy, oz; uint32_t r_sub, r_sc; int anker; uint16_t basis;
    if (zone3 != 0u) {                       /* Zweig A @0x80107FFC-80108064 */
        ox = 256 - 2*(int32_t)re2z_rand();   /* @0x80108004/08/0C */
        oy = 312 - 4*(int32_t)re2z_rand();   /* @0x80108018/1C    */
        oz = 256 - 2*(int32_t)re2z_rand();   /* @0x8010802C/30    */
        r_sub = re2z_rand();                 /* @0x80108034 -> s0 @0x80108040 */
        r_sc  = re2z_rand();                 /* @0x8010803C       */
        basis = 7000;                        /* @0x80108050       */
        anker = 0;                           /* addiu a2,s1,72  @0x80108064 */
    } else {                                 /* Zweig B @0x80108068-801080C0 */
        ox = 127 - (int32_t)re2z_rand();     /* @0x80108070/74    */
        oy = 927 - (int32_t)re2z_rand();     /* @0x80108080/84    */
        oz = 127 - (int32_t)re2z_rand();     /* @0x80108090       */
        r_sub = re2z_rand();                 /* @0x80108094       */
        r_sc  = re2z_rand();                 /* @0x8010809C       */
        basis = 6096;                        /* @0x801080B0       */
        anker = 1;                           /* addiu a2,s1,244 @0x801080C0 */
    }
    re2z_gore_fx_ex(e, anker,
                    ((r_sub & 1u) << 16) | (uint32_t)(basis + (r_sc << 3)),
                    (int16_t)e->re2z_bearing158 /* lh a1,344(s2) @0x8010805C/@0x801080BC */,
                    ox, oy, oz);
}
```
* **Die RNG-WURFZAHL ist Verhalten** (Regel aus `reai-v2-gore-vollausbau`): es muessen genau
  **fuenf** Zuege pro Stoss bleiben, in der Reihenfolge X, Y, Z, sub, scale.
* `re2z_gore_fx_ex` (`:1041`) ist bereits der Dekoder fuer die gepackte Id und nimmt
  `ofs_x/y/z` — Id 0 (Blut) geht unveraendert durch (`case 0: break;` `:1049`).
* `a1` ist **nicht** `e->rot_y`: das Original uebergibt `lh a1,344(s2)` = **+0x158**, den in
  Phase 0 @0x80107FD8 frisch aus `FUN_800154AC` geschriebenen Winkel zum Spieler
  (`sh v0,344(s2)` @0x80107FD8). ⛔ Das Port-Feld `re2z_t158` (`re15_actor.h:313`) traegt
  +0x158 heute als **Budget/Timer** (Grab-Wehr 148 @0x80102828-2C, ACTIVE-Timer
  `enemy_ai_re2_zombie.c:1277/1295/1308/1321/1567/1606`). Die Ueberladung im 7EF0-Handler ist
  im Port also **nicht modelliert**; bis sie es ist, bleibt `e->rot_y` eine ausdruecklich
  **gekennzeichnete Port-Naeherung**, keine byte-true Zeile.
* Die Groessen-Basis wird zusaetzlich mit dem Praesentations-Faktor der Pistole multipliziert
  (`0x2000 / 6096`, beide Werte belegt: @0x80105C54 und @0x8010567C) — als **[PORT-MAPPING]
  gekennzeichnet**, damit MG und Pistole im Port dasselbe Verhaeltnis tragen wie im Original.
* Der Splatter-Faecher muss beim MG **mitkommen** (heute `0`): siehe Schritt 2.

### Schritt 2 — die Splatter-Menge je Handler an die ESP-Daten binden (nicht raten)

`re15_esp_fx_splatter(bank, 0, n, …)` ist der Port-Ersatz fuer die Klon-Schleife in
`FUN_8001BF10` (`iVar13 = uVar1 - 1`, `uVar1 = *puVar14`, `puVar14 = tbl[(a0>>16)&7]`).
`n` darf deshalb **nicht** per Hand gesetzt werden: es ist die Zeilenzahl des Sub-Records.
Konkret:
* `re2z_blood_fx_dir` (`:1124`) setzt `n = 8` fest — das ist eine **ererbte, unbelegte** Zahl
  und gehoert in denselben Umbau.
* Richtig: `re15_esp_fx_spawn_rows(bank, id, sub, scale, …)` benutzen (existiert bereits,
  `re15_esp.h:241`, und ist laut `reai-v2-gore-vollausbau` genau der Zwilling von
  `FUN_8001BF10`) — dann kommt die Sprite-Zahl aus der Bank statt aus einer Konstante, und der
  `sub = rand&1`-Wechsel des MG liefert automatisch die zweite Sprite-Zahl.

### Schritt 3 — Regressionsschutz

`re15_port/tests/unit/` hat bereits `test_re2_gore_decoder` (Fixture aus Runde 4) und die
Diagnosehaken `re15_re2z_last_fx_part()` / `re15_re2z_last_fx_pos()` (`:986-991`).
Neuer Fall: „MG-Treffer bei `+0x1D2 = 1` → Anker Part 0, Skalierung im Band 7000..9040,
fuenf RNG-Zuege" — pruefbar ohne Grafik.

### Schritt 4 — NICHT anfassen

* Die Feuerkadenz (`player_common.c:423`) bleibt der RE1.5-Takt; RE2's 8-Frame-Takt
  (@0x8006A144-58) gilt fuer RE2-Waffen-Ids, der Port schiesst mit dem RE1.5-Resolver. Ein
  Umbau hier waere eine Spielbalance-Aenderung ohne Auftrag.
* Die Poise-Kosten-Zeile 15/18 bleibt **0** (@0x8010CC33 + 15/18, Dump in §2.5) — das MG bricht
  im Original die Standfestigkeit nicht, und der Port bildet das schon korrekt ab.
* Die uebrigen drei `re2z_blood_fx_at`-Aufrufer (`:5954`, `:6086` @0x80106D7C, `:6174`
  @0x801071A0) bleiben in diesem Auftrag unberuehrt — ihre Ids sind noch nicht ausgelesen
  (s. §6).

---

## 6. Offen / nicht belegt

1. **Die restlichen `0x1500`-Aufrufer.** `:4888` (Stagger-P1, Ids laut Kommentar 8000/5096 je
   `+0x222` @0x80105FF4), `:5954`, `:6086` (@0x80106D7C), `:6174` (@0x801071A0) tragen weiter
   die unbelegte 0x1500. Dieses Dossier hat nur den MG-Aufrufer (`:4942`) am Original gelesen.
   Naechster Weg: dieselbe Methode (Disasm der vier Adressen, a0/a2/a3 auslesen).
2. **`re2z_blood_fx_dir`s `splatter_n = 8`** ist ebenfalls unbelegt. Beleg waere die Zeilenzahl
   `uVar1` des ESP-Sub-Records 0 der Blut-Bank — messbar am Asset
   (`shared_assets/PSX/DATA/CORE00.ESP`), nicht am Code. Nicht gemessen.
3. **`a1` = +0x158 statt rot_y** (§5 Schritt 1): dass `+0x158` in Phase 0 @0x80107FD8 aus
   `FUN_800154AC` (atan2) gefuellt wird, steht im Disasm; ob das Port-Feld `re2z_bearing158`
   heisst und zum Zeitpunkt des Spawns denselben Wert traegt, ist **nicht geprueft**.
   ⛔ Erinnerung `reai-v2-zitierte-adresse-ist-kein-beleg`: 0x800154AC ist der atan2, nicht der
   Blut-Spawner — die Adresse steht hier nur fuer die Winkelberechnung.
4. **Keine Laufzeitmessung.** Dieser Lauf durfte nicht bauen (Regel 2), also gibt es keinen
   Zaehler „Sprites pro Sekunde im Port gegen Sprites pro Sekunde im Original". Die
   Gegenueberstellung in §3.2 ist aus dem Quelltext gelesen, nicht gemessen. Vor der Abnahme
   gehoert ein Lauf mit `RE15_ESP_FX`-Zaehlung bzw. `re15-port-visual-verify` (gdigrab) dazu.
5. **RE2-Kadenz vs. Port-Kadenz.** Dass `FUN_8006A0CC` @0x8006A144-58 die Munition alle 8 Frames
   verbraucht, ist belegt; dass in RE2 **jeder** dieser Schuesse auch einen Treffer aufloest
   (und nicht etwa mehrere Kugeln pro Tick), ist **nicht** nachgelesen. Naechster Weg: die
   Aufrufer von `FUN_800410CC` (@0x80043AFC in `FUN_80043908`, @0x80047EF4 in `FUN_80047C6C`,
   …) entlangverfolgen.
6. **Irrefuehrender Kommentar im Port.** `enemy_ai_re2_zombie.c:5068` schreibt
   `re2z_blood_fx_dir(e, bdir); /* Id 6096 @0x8010567C */`, die Funktion uebergibt aber
   `0x2000` (`:1124`). Die Begruendung steht 3500 Zeilen weiter oben (`:1121-1123`) — beim
   Umbau mit korrigieren, sonst liest der naechste die 6096 als aktive Konstante.
7. **Kriecher/Liegende.** Die 1D-Tabelle @0x8010CBE8 schickt ALLE Waffen (auch das MG) auf
   `0x80107888` mit einem einzigen festen Spawn (Id 6000 @0x8010792C-3C). Der Port hat das
   (`re2z_crawl_hurt`); dass die MG-Brutalitaet dort im Original also **nicht** gilt, ist
   belegt, aber im Port nicht gegen eine Messung geprueft.
