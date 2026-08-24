# FINDING 4 — „Kopf zertreten" beim kriechenden Zombie (RE2-KI, ROOM1030)

**Stand:** 2026-08-24 · **Phase:** REINES RE + MESSUNG, keine Code-Änderung.
**Nutzer-Report:** „fehlt der 'smash' sound, wenn ich einen am boden herankriechenden Zombie
der mich beisst der Kopf zertrete. Ausserdem wird der Kopf nicht zertreten und der Zombie
kriecht weiter."

> **Legende** `BYTE_BELEGT` = Instruktionen selbst gelesen (Adresse + Binary genannt) ·
> `GEMESSEN` = Sonde am echten `re15_game_step` · `OFFEN` = weder belegt noch widerlegt.

---

## 0. Executive Summary

**Es gibt in RE1.5 KEIN Spieler-Kommando „treten/stampfen".** Das ist kein Verdacht, sondern
eine vollständige Aufzählung (§1). Was der Nutzer als „Kopf zertreten" sieht, ist die
**Befreiung aus dem Bein-Biss des Kriechers** — und die ist in **beiden** Originalen eine
reine GEGNER-seitige Choreografie, die den Kriecher **tötet** und ihm dabei ein Körperteil
abreißt bzw. **den Kopf abtrennt**:

| Flavor | Original-Kette | Ergebnis | Port |
|---|---|---|---|
| RE1.5 | Grid-1-Kriech-Grab `FUN_80103B94`, Phase [5] Frame 25 → [6] | Gore-Part (Bone 14) + **SE 9**, danach `+0x4/+0x5 = 7` (CORPSE) + HP = −1 | ✅ portiert (`enemy_ai_common.c` `re15_zcrawl_grab_animate`, Z. 3480–3520) |
| RE2 | Grab-EXEC[1] `@0x801025EC`, **P5-Kriecher-Zweig `@0x80102A78–0x80102BC0`** → P6 `@0x80102BE8` | **KOPF (Part 8) ab** + Blut + **SE 2** + Hals-Stumpf-Sprühen, danach `sw 7,4` + `sh −1,342` (tot) | ❌ **FEHLT KOMPLETT** |

**Der Port-Defekt ist eine einzige fehlende Bedingung plus ein fehlender Block** (§4):
`enemy_ai_re2_zombie.c:1414` schneidet P5 bei Frame 7 nach P7 ab — **ohne** das
`(s5 & 1) == 0`-Tor, das das Original `@0x80102BC4-C8` davorsetzt. Der Kriecher verlässt P5
also schon bei Frame 7, **bevor** irgendetwas von der Gore-Choreografie (Frame 22 / 23–41 /
Frame 27) läuft, und **bevor** P6 ihn töten kann. Er landet stattdessen im
Aufrecht-Erhol-Pfad P7/P8 → Sturz `0x501` → kriecht weiter und greift erneut an.

**GEMESSEN (Sonde, 1800 Frames ROOM1030, RE2-Flavor, echte Bänke):**
```
f346  +0x6=4 frame=15   <- P4 Abwurf-Eintritt
f347  +0x6=5 frame=0    <- P5
...
f353  +0x6=5 frame=6
f354  +0x6=7 frame=7    <-- FRAME-7-SCHNITT FEUERT AUF DEM KRIECHER (im Original gesperrt)
f355  +0x6=8 frame=8    <-- P8 = der AUFRECHT-Erholpfad
...
f368  +0x6=8 frame=21   <-- ab hier weg (Frame 22 = P8-Sturzwurf -> 0x501)
f554  slot 2: +0x5 5 -> 1   <-- lebt, greift wieder an
ERGEBNIS: 6 Griff-Eintritte, 18 HP-Abfälle, 0x mal +0x6 == 6, 0x mal state 7, HP bleibt 97
```
Damit sind **alle drei** Nutzer-Symptome auf **eine** Ursache zurückgeführt.

---

## 1. „Wie wird der Stampf ausgelöst?" — GAR NICHT. Es gibt kein Stampf-Kommando.

### 1.1 Vollständige Aufzählung des Spieler-Kommandos `DAT_800ACA58` (PSX.EXE) `BYTE_BELEGT`

Dispatcher `FUN_80031C44`:
```
80031c8c  lbu v1,-13736(v1)      ; v1 = DAT_800ACA58 (Haupt-Kommando)
80031c94  sll v1,v1,2
80031ca4  addiu at,at,16272      ; at = PTR_LAB_80073F90
80031cac  lw  v0,0(at)
80031cb4  jalr v0
```
Tabelle `@0x80073F90` (selbst gedumpt): `[0]=0x800318F8 [1]=0x80031DE8 [2]=0x80035AF0
[3]=0x800366BC [4]=0x80030660 [5]=0x80036834 [6]=0x800368C0 [7]=0x8003694C`.

Sub-Zustands-Tabellen (Zwei-Hälften, `+0x05`): ENTRY `@0x80073FB0` (16), UPDATE `@0x80073FF0` (16),
dispatcht `@0x80031EEC` / `@0x80031F14`.

**ALLE 28 Schreiber von `DAT_800ACA58` in der EXE** (`grep DAT_800aca58 ghidra1_V2.txt`) und die
unmittelbar davor geladenen Immediates — selbst nachdisassembliert:

| Adresse | Wert | Bedeutung |
|---|---|---|
| `@0x80031F88` | `0x101` | Substate 1 = Vorwärtsgehen (Pad-Bit 0x1) |
| `@0x80032020` | `0x201 / 0x401 / 0x301 / 0x701` | Rennen / Rückwärts / Drehen / **R1-Zielen** |
| `@0x80032454/80` | `0x201 / 0x401 / 0x301` | dito aus Substate 1 |
| `@0x80032760` | `0x101 / 0x401 / 0x301` | aus Substate 2 |
| `@0x80032958` | `0x1` | zurück nach Idle |
| `@0x80032AA4/ACC` | `0x101 / 0x201 / 0x401` | aus Substate 3 |
| `@0x80032CD8/D08` | `0x101 / 0x201 / 0x301` | aus Substate 4 |
| `@0x8003247C` | `0x801` | Substate 8 = **Kiste schieben** (Kontaktbit 0x2000) |
| `@0x8002D604/618` | `aca59 = 9`, `aca5a = 1/0x81` | Substates 9–13 = **Klettern/Vaulten** (SQUARE-Kante) |
| `@0x8003191C/2C` | `0x1` | Idle-Prolog |
| `@0x80035ABC` | `0x1` | Schiebe-Ende |
| `@0x80035C80/DB8/F00/80036084` | Register `a0` | Hit-/Knockdown-Handler (2/3) |
| `@0x800362F4/8003640C/8003667C` | `0x1` | Death-/Recover-Rückkehr |
| `@0x80036814` | `0x7` (`sh`) | Grabbed-Death |
| `@0x80012EBC/EF4` | `2` / `3` | Damage-Resolver: Hurt / Death |
| `@0x8004672C` | `0x1` | Menü-Rückkehr |
| `@0x80031518`, `@0x8001CBDC` | `0` | Raum-/Boot-Reset |

**Ergebnis: die Kommando-Menge ist `{0, 1, 2, 3, 5, 6, 7} × Substate {0…13}` — kein Tritt, kein
Stampf, kein unbewaffneter Angriff.** Zusatz-Beleg: Substate 7 (Zielen/Angriff) wird
`@0x80032008-18` explizit gesperrt, wenn `DAT_800ACA5D` (Waffe) == 0:
```
80032008  lbu v0,-13731(v0)   ; DAT_800ACA5D = ausgerüstete Waffe
80032014  beq v0,zero,0x80032024   ; keine Waffe -> KEIN Angriffs-Kommando
80032018  ori v0,zero,0x701
```

### 1.2 Vollständige Aufzählung der Spieler-Schadensquelle `FUN_80011F50` `BYTE_BELEGT`

`ghidra1_V2.txt` XREF[9] auf `FUN_80011F50`: `0x80012418` (Selbst-Rekursion nach dem
`+0x93`-Latch) und **acht** Aufrufer:
`0x80033554`, `0x80033880`, `0x8003396C`, `0x80033A34`, `0x80033B10`, `0x80033C50` (alle in der
**Schusswaffen**-Sub-FSM `@0x800740F4[aca5a]`, Klassen-Weiche `@0x8003403C`/`@0x80032E9C`),
`0x800349EC` (Dauerfeuer-Klasse `@0x80074150`) und `0x800353CC` (**Nahkampf**-Sub-FSM
`@0x80074164[aca5a]`, Phase 2 = der Messer-Schwung `@0x80035314`).

Die Waffen-Klassenweiche `@0x80032E70-84` liest `DAT_800ACA5D` und indiziert `@0x80074030[22]`:
```
80032e60  lbu v1,-13731(v1)   ; Waffe
80032e74  addiu at,at,16432   ; at = 0x80074030
80032e84  jalr v0
```
Tabelle `@0x80074030` (gedumpt): `[0..2]=0x80034E70` (Nahkampf), `[12,14,19]=0x80034014`
(Dauerfeuer), Rest `0x80032E9C` (Schuss).
Die Tester-Tabelle `@0x8006E548[22]` (gedumpt) verteilt auf
`0x80012574` (Schuss-Streifen) / `0x800127FC` (Nahkampf-Kegel) / `0x800128A0`.

**Ergebnis: der Spieler kann einen Gegner AUSSCHLIESSLICH über die Ziel-/Feuer-FSM
(Kommando `0x701`) verletzen. Es gibt keinen Fuß-, Tritt- oder Zertret-Pfad.**

### 1.3 Gegenprobe: „steppt der Gegner sich selbst tot?" (das RE2-Muster) `BYTE_BELEGT`

RE2 kennt so etwas — aber **nur bei der Baby-Spinne**. Deren Zertret-Tor `EMS26.BIN
@0x801003D4-460` liest das Spieler-Kommando **absolut**:
```
801003dc  sltiu v0,v0,0x1f4      ; dist < 500
801003ec  lhu   v0,-1034(v0)     ; 0x800CFBF6 = Schritt-Bits
801003f4  andi  v0,v0,0x6
80100410  bne   v1,v0            ; +0x106 == Spieler+0x106 (gleiche Etage)
8010041c  lhu   v1,-1028(v1)     ; 0x800CFBFC = Spieler-Kommando
80100424..3c  == 257/513/769/1025 (0x101/0x201/0x301/0x401)
80100448  sw 3,4(s0)             ; -> DEATH
```
Voll-Scan **aller** RE2-Overlays nach Lesern von `0x800CFBFC` / `0x800CFBF6`
(Skript über `EMOVL10_S0/S1`, `EMD0G_MOD0`, `EMOVL21_S0`, `EMS26`):
* `EMOVL10_S0/S1` (**Zombie**): 6 Treffer, **alle** `0x800CFBF6` und **alle** Aufwach-Tore
  (`@0x80101338/13A8/1874/18BC/1F18/20DC`, jeweils `sw 3073,4` = `0x0C01`). **Kein** Zertret-Tor.
* `EMD0G_MOD0` (Hund): 8 Treffer, Aggro-Radius (rennt der Spieler? → 5000 statt 3500).
* `EMOVL21_S0` (Krähe): 3 Treffer, ebenfalls Wach-Tore.
* `EMS26` (Baby-Spinne): die einzige echte Zertret-Regel.

Analoger Scan über **alle** RE1.5-Overlays nach Lesern von `DAT_800ACA58` (`imm == 0xCA58`):
`STAGE1.BIN` hat 9 Leser — `@0x8010DE18/DE48/E120/E160` (Hund), `@0x801120BC` (Krähe),
`@0x80115268` (Krähen-Opfer), `@0x801174F4/1176E4/80118098` (Gorilla-Boss). **Kein Zombie,
kein Kriecher.**

→ **Weder RE1.5 noch RE2 hat ein Zertreten von Zombies.** Die Frage „welche Eingabe / welcher
Spielerzustand / welche Bedingung am Ziel" hat damit die Antwort: *keine* — der Auslöser ist §2.

---

## 2. Was der Nutzer wirklich auslöst: die Befreiung aus dem Kriech-Bein-Biss

### 2.1 RE1.5 (Flavor RE15) — `STAGE1.BIN`, Grid-Wurzel 1, ANIMATE[1,2] = `FUN_80103B94` `BYTE_BELEGT`

Kette: P3 Kampf `@0x80103E88` → Mash-Budget `+0x9C -= 1 + 5·mash`, `bgez @0x80103EC4` → P4.

**P4 `@0x80103F38`** (Abwurf):
```
80103f40  ori v0,zero,0x1d ; sb v0,148(v1)    ; +0x94 = Clip 0x1D (Abschüttel-Clip)
80103f50  ori v0,zero,0x5  ; sb v0,6(v1)      ; +0x06 = 5
80103f64  sb  zero,149(v0)                    ; +0x95 = 0
80103f74  ori v0,zero,0x7  ; sb v0,143(v1)    ; +0x8F = 7
80103f84  ori v0,zero,0x5a ; sb v0,469(v1)    ; +0x1D5 = 0x5A (Gnadenfrist)
80103f94  jal 0x800453d0  (a0 = 7)            ; Raum-SE 7
```
**P5 `@0x80103FA4`** (Abspielen + GORE bei Frame 25):
```
80104000  lbu v0,-13732(v0)   ; DAT_800ACA5C (Charakter-Byte)
80104008  andi v0,v0,0x4
8010400c  bne v0,zero,0x800040c4    ; &4 != 0 -> Elza-Zweig
80104028  bne v1,0x19               ; +0x95 (Frame) == 25 ?
80104030  sb  v0(=1),440(a0)        ; +0x1B8 = 1
80104040  lbu v0,8(a2)              ; Typ
8010404c  addiu at,at,-2172         ; at = 0x8011F784  (GORE-BONE-Tabelle)
80104054  lbu v1,0(at)              ; Bone-Index  (Zombie 0x10/0x11/0x12/0x16 -> 14)
80104074  lw  v1,392(a2)            ; Part-Pool = entity+0x188
8010407c  addu s0,v0,v1             ; s0 = pool + 172*bone
80104080  jal 0x80019700 (a0 = 0x2000, a1 = rot_y, a2 = s0+64)
80104088  ori a0,zero,0x9
80104094  sw  0x6f,104(s0)          ; part+0x68 = 0x6F
801040b0  ori v1,v1,0x4a            ; part Flagwort |= 0x4A  (= eigene Matrix + Drift-Physik)
801040b4  jal 0x800453d0            ; RAUM-SE 9  <-- „Abriss"-Geräusch
```
Elza-Zweig `@0x800040C4-124`: Frame 25, `+0x1D8 |= 2`, SE 9, dann `rng&1 ? SE 5 : SE 8`.

**P6 (default) `@0x80104128`** (Tod):
```
8010413c  ori v0,v0,0x2  ; sw v0,0(v1)     ; word0 |= 2
8010414c  addiu v0,zero,-1 ; sh v0,154(v1) ; +0x9A (HP) = -1
8010415c  ori v0,zero,0x7  ; sh v0,4(v1)   ; +0x04/+0x05 = 7 = CORPSE
```
Der Bone kommt aus `LAB_8011F784` (STAGE1.BIN, per Typ): Zombies `0x10-0x12/0x15-0x1A` = **14**,
Zombie-Girl/Writher `0x13/0x14/0x1B-0x1E` = **8**, Hund `0x20` = 0, Krähe `0x21` = 7,
Gorilla `0x27` = 13. (Dieselbe Tabelle nutzt der Port bereits als `s_gore_bone`,
`re15_damage.c:1640`.)

**Port-Stand:** vollständig da — `enemy_ai_common.c` `re15_zcrawl_grab_animate`, `case 4/5/default`
(Z. 3461–3517), inkl. `re15_audio_room_se(9)` und `e->state = 7`.

### 2.2 RE2 (Flavor RE2) — `EMOVL10_S0.BIN`, Grab-EXEC[1] `@0x801025EC` `BYTE_BELEGT`

`s5 = (+0x10E & 1) + (Typ == 0x11 ? 2 : 0)` (`@0x80102674-90`) — **`s5 & 1` = „Kriecher"**.
P3 Biss-Schleife `@0x80102838`: `+0x158 -= 2 + 5·mash`, Ablauf oder „one-save"-Bit → P4.
P4 `@0x80102968`: Abwurf-Clip = `grabclip + 2`, `+0x6 = 5`.

**P5 `@0x801029A4` — hier sitzt alles.** Ablauf im Original:

```
80102a50  jal 0x80015cb8              ; Pose
80102a58..68  Advance (a3 = 512)
80102a6c  lbu v1,6(s1)
80102a74  addu v1,v1,v0               ; +0x6 += Advance-Rückgabe   <-- ZUERST advancen
80102a78  andi v0,s5,0x1
80102a7c  beq  v0,zero,0x80102bd0     ; NICHT-Kriecher -> Frame-7-Schnitt
80102a80  sb   v1,6(s1)               ; (Delay-Slot: der advancte +0x6 wird IMMER gespeichert)
```

**KRIECHER-ZWEIG (Charakter-Weiche) `@0x80102A84-AB8`:**
```
80102a84  lbu v1,8(s3)                ; s3 = SPIELER-Block; +0x8 = Charakter-/Modell-Id
80102a90  bne (v1&1),zero,0x80102aa4
80102a9c  bne (v1<11),zero,0x80102abc      ; gerade && < 11        -> ZWEIG A
80102aa8  bne ((v1-12)<2),zero,0x80102abc  ; v1 == 12 oder 13      -> ZWEIG A
80102ab4  bne v1,11,0x80102b38             ; v1 == 11              -> ZWEIG A, sonst ZWEIG B
```
(RE1.5-Zwilling dieser Weiche: `DAT_800ACA5C & 4` @0x80104008. Welche RE2-Ids genau in
Zweig A fallen, ist `OFFEN` — für den Port ist die Zuordnung „Leon → A" die
naheliegende Übersetzung, s. §4.)

**ZWEIG A — DER KOPF AB, `@0x80102ABC-B34`, Auslöser Frame `+0x14D == 22`:**
```
80102ac0  addiu v0,zero,22
80102ac4  bne v1,v0,0x80102b70        ; nur in Frame 22
80102ac8/cc  a1 = 0x00101040          ; Farbwort
80102ad0  lw  v1,408(s1)              ; v1 = Part-Pool (entity+0x198)
80102ad8  lw  v0,1376(v1)             ; Part 8 = KOPF (8*172 = 1376) Flagwort
80102ae0  ori v0,v0,0x4a
80102ae4  sw  v0,1376(v1)             ; Kopf abgetrennt: |= 0x4A
80102ae8  lhu v0,118(s1)              ; rot_y
80102af0  sh zero,1536(v1)            ; Part8 +0xA0 = 0
80102af4  sh zero,1532(v1)            ; Part8 +0x9C = 0   (KEIN Vortrieb — anders als der
80102af8  sh zero,1530(v1)            ; Part8 +0x9A = 0    fliegende Magnum-Kopf!)
80102afc  sh zero,1534(v1)            ; Part8 +0x9E = 0
80102b00  sh zero,1540(v1)            ; Part8 +0xA4 = 0
80102b04  sw a1,1488(v1)              ; Part8 +0x70 (Tint) = 0x00101040
80102b08  sh v0,1528(v1)              ; Part8 +0x98 (Kurs) = rot_y
80102b10  addiu v0,zero,-600
80102b14/18/1c  ofs = (0, -600, 0)
80102b20  lh  a1,118(s1)              ; Winkel = rot_y
80102b24  jal 0x8001bf10              ; Blut/FX, a0 = 0x00801000
80102b28  addiu a2,a2,72              ;   Anker = Part 0 + 72 (Hüft-Matrix)
80102b2c  addiu a0,zero,2
80102b58  jal 0x8005bd6c              ; >>> SE 2 = DAS „SMASH"-GERÄUSCH <<<
80102b60/64/68  jal 0x80039514 (4,250,0)   ; Rumble
```
**ZWEIG B `@0x80102B38-54`, Auslöser Frame `+0x14D == 27`:**
```
80102b40  bne v1,27,0x80102b70
80102b50  ori v0,v0,0x80 ; sh v0,538(s1)   ; +0x21A |= 0x80
80102b58  jal 0x8005bd6c  (a0 = 8)         ; SE 8
```
**GEMEINSAM — Hals-Stumpf-Sprühen `@0x80102B70-BC0`:**
```
80102b78  addiu v0,v1,-23
80102b7c  sltiu v0,v0,0x13            ; Frame 23..41
80102b88  beq (v1&1),zero,0x80102bc4  ; nur UNGERADE Frames
80102b94/98/a0  ofs = (0, +100, 0)
80102b9c  jal 0x80015fe8              ; rand
80102ba4  addiu a0,zero,4096          ; FX-Id 0x1000
80102bac  lh a1,118(s1)
80102bb0-b8  Winkel = rot_y + rand*4 - 512
80102bbc  jal 0x8001bf10
80102bc0  addiu a2,s0,1448            ; Anker = Part 8 + 72 = KOPF/HALS-Matrix
```
**DAS TOR, DAS IM PORT FEHLT — `@0x80102BC4-E4`:**
```
80102bc4  andi v0,s5,0x1
80102bc8  bne  v0,zero,0x80102ebc     ; <<< KRIECHER: RAUS, KEIN Frame-7-Schnitt >>>
80102bd0  lbu  v1,333(s1)             ; (nur Aufrechte:)  +0x14D
80102bd8  bne  v1,7,0x80102ebc
80102be4  sb   v0(=7),6(s1)           ; Frame 7 -> +0x6 = 7 (P7)
```
**P6 `@0x80102BE8` — der Kriecher stirbt** (erreichbar NUR, weil P5 ihn nicht bei Frame 7
wegschneidet und `+0x6` per Clip-Ende von 5 auf 6 läuft):
```
80102bf4  sw  7,4(s1)                 ; +0x04 = 7 (CORPSE)
80102bfc  sh  -1,342(s1)              ; +0x156 (HP) = -1
80102c08  andi v0,v0,0xfffb           ; +0x21A &= ~4
80102c20  sw  v1,0(s1)                ; word0 = (word0 & 0xF3FFFFFF) | 0x04000000
80102c24  ori v0,v0,0x80 ; sb v0,467   ; +0x1D3 |= 0x80
```

**Quergegenprobe:** der Port hat den **identischen** Kopf-Abriss schon einmal implementiert —
in der Liege-Todes-Zerlegung (`enemy_ai_re2_zombie.c` Z. 4690–4706, Original `@0x80109460-98`):
gleiches `re2z_part_flags[8] |= 0x4A`, gleiches `re2z_part_tint[8] = 0x00101040`, gleiches
`re2z_se(2)`. Die Port-Felder für alle Schreiber existieren also bereits
(`re2z_part_flags/_tint/_yaw98/_w9a/_w9c/_w9e/_life/_wa4`, `re15_actor.h:347-394`).

---

## 3. Der Port heute — GEMESSEN

### 3.1 Wo die Weiche steht `BYTE_BELEGT` (Port-Quelle)

`enemy_ai_common.c:4788`: sobald `re15_ai_re2_for_type(type) && re15_re2z_owns_type(type)`,
übernimmt **`re15_re2z_tick` den ganzen Dispatch** — die RE1.5-Grid-1-Kriech-Maschine mit
ihrem SE 9 + Corpse-Ende läuft im RE2-Flavor **nie**.

### 3.2 Der Defekt in `enemy_ai_re2_zombie.c` (`re2z_exec_grab`, `case 5`, Z. 1402–1428)

```c
re15_re2z_grab_rootmotion(e);                              /* @0x80102A50-68 */
if (re2z_frame_slot(e) == 7) { e->sub_state_2 = 7; break; }/* upright cut @0x80102BD0-E4 */
e->sub_state_2 += re2z_clip_done(e);                       /* @0x80102A6C-80 */
if (e->sub_state_2 == 6) { if (s5 & 1) { e->hp = -1; set_state_word(e,7); break; } ... }
```
Drei Abweichungen vom Original:
1. **⛔ HAUPTFEHLER:** Der Frame-7-Schnitt trägt das Original-Tor `andi v0,s5,0x1 / bne`
   (`@0x80102BC4-C8`) **nicht**. Der Kriecher wird bei Frame 7 nach P7 geschnitten und
   erreicht P6 (Tod) nie.
2. **Der ganze Kriecher-Zweig `@0x80102A78-0x80102BC0` fehlt:** kein Kopf-Abriss (Part 8),
   kein Tint, kein Blut, **kein SE 2 / SE 8**, kein Hals-Sprühen, kein Rumble.
3. **Reihenfolge:** das Original advancet `+0x6` VOR der Kriecher/Aufrecht-Weiche
   (`@0x80102A6C-80`, Store im Delay-Slot `@0x80102A80`); der Port advancet danach.

### 3.3 Sonde `GEMESSEN`

Werkzeug: Kopie von `re15_port/tests/unit/probe_re2z_crawl_attack.c` (echte `ROOM1030.RDT` +
SCD-Spawns + `re15_game_step` + geladene RE2-Bänke), erweitert um eine Pro-Frame-Zeile
für jeden Kriecher im Griff. Gebaut gegen `libre15_engine.a` (Scratchpad, **keine
Repo-Änderung**), Lauf `RE15_AI_FLAVOR=re2 probe_stomp 1800 2`.

```
Kriecher: 2 (Slots 1/typ0x16 2/typ0x16)
f261  slot 2: +0x5 0 -> 1                       Griff beginnt (EXEC[1])
[grab] f261..f269  +0x6=1   frame 0..8          P1
[grab] f270        +0x6=2                       P2
[grab] f271..f345  +0x6=3   frame 0..29 (loop)  P3 Biss-Schleife
[grab] f346        +0x6=4   frame=15            P4 Abwurf
[grab] f347..f353  +0x6=5   frame 0..6          P5
[grab] f354        +0x6=7   frame=7             <-- FRAME-7-SCHNITT (im Original gesperrt)
[grab] f355..f368  +0x6=8   frame 8..21         P7/P8 = AUFRECHT-Erholung
f554  slot 2: +0x5 5 -> 1                       war in EXEC[5] (Sturz 0x501), greift wieder an
ERGEBNIS: Griff-Eintritte = 6 ; HP-Abfälle (Spieler) = 18 ; Kriech-Frames = 3600
```
* `+0x6 == 6` (P6) in 1800 Frames: **0 mal**. `state == 7`: **0 mal**. Gegner-`hp` bleibt **97**.
* Frame 22 (Kopf ab) / Frames 23–41 (Hals-Sprühen) / Frame 27 (Zweig B) werden in P5
  **nie** erreicht — P5 endet bei Frame 7.
* Beobachtung nebenbei (`OFFEN`, eigener Punkt): dieser Skript-Kriecher trägt `grid=0x0D`
  und `+0x21A = 0x0000` — also **weder** `grid & 0x80` **noch** das RE2-Liege-Bit `+0x21A & 2`.
  Für den Treffer-Resolver (`re15_damage.c`, Liege-Klassifikation) ist er damit **nicht
  liegend**, ein Abwärts-Schuss würde ihn also nicht als Tiefband-Ziel sehen. Das ist ein
  ANDERER Befund als der w20-Fix (der galt dem Abwurf-Flop-Kriecher aus EXEC[11]) und
  gehört separat geprüft — hier nur protokolliert, nicht bewertet.

### 3.4 Der Ton `BYTE_BELEGT` (Port-Quelle)

`platform/pc/main.c:590` `pc_re2z_se_re15` (der Sound-Mandat-Mapper) kennt die Ids
`0,1,3,4,5,9,10,11,12,13`. **Id 2 (Kopf-Zerstörung) und Id 8 sind NICHT gemappt** und fallen
in `re15_audio_re2_enemy_se()` = RE2-Sample. Für den Umbau ist also zusätzlich eine
Mapping-Entscheidung nötig (Vorschlag, mit RE1.5-Beleg: **ENEMSE 2 → RE1.5-Raum-SE 9**, das
ist der byte-belegte RE1.5-Zwilling des Abriss-Geräuschs `@0x80104088/@0x800040B4`;
**ENEMSE 8 → RE1.5-SE 8**, der Zwilling aus dem Elza-Zweig `@0x80104118-1C`).

---

## 4. Die Port-Lücke, kompakt

| # | Lücke | Original-Beleg | Port-Ort |
|---|---|---|---|
| L1 | Frame-7-Schnitt in P5 **ohne** Kriecher-Tor → P6 (Tod) unerreichbar → „Zombie kriecht weiter" | `@0x80102BC4-C8` `andi v0,s5,0x1 / bne v0,zero,0x80102EBC` | `enemy_ai_re2_zombie.c:1414` |
| L2 | Kopf (Part 8) wird nicht abgetrennt → „Kopf wird nicht zertreten" | `@0x80102ABC-B08` (Frame 22, `flags\|=0x4A`, Tint `0x00101040`, Kurs = rot_y, Drift-Felder = 0) | fehlt in `case 5` |
| L3 | Kein **SE 2** („smash") / kein SE 8 | `@0x80102B2C`+`@0x80102B58` (`jal 0x8005BD6C`, a0 = 2) bzw. `@0x80102B44`+`@0x80102B58` (a0 = 8) | fehlt in `case 5`; **zusätzlich** ungemappt in `pc_re2z_se_re15` |
| L4 | Kein Blut am Abriss + kein Hals-Stumpf-Sprühen | `@0x80102B10-28` (Anker Part 0+72, ofs (0,−600,0), Id 0x00801000) und `@0x80102B70-C0` (Frames 23–41 ungerade, Anker Part 8+72, Id 0x1000, Winkel rot_y+rand·4−512) | fehlt |
| L5 | Charakter-Weiche A/B fehlt | `@0x80102A84-AB8` (`Spieler+0x8`); RE1.5-Zwilling `DAT_800ACA5C & 4` `@0x80104008` | fehlt |
| L6 | P6 setzt im Port nur `hp=-1` + `state=7`; `+0x21A &= ~4`, `word0`-Gruppe, `+0x1D3 \|= 0x80` fehlen | `@0x80102C00-2C` | `enemy_ai_re2_zombie.c:1420-1422` |
| L7 | Reihenfolge: `+0x6`-Advance gehört VOR die Kriecher/Aufrecht-Weiche | `@0x80102A6C-80` (Store im Delay-Slot) | `enemy_ai_re2_zombie.c:1413-1415` |

Alles, was L2 braucht, existiert im Port bereits als Feld und ist an der Liege-Todes-Zerlegung
(`enemy_ai_re2_zombie.c:4690-4706`) einmal vollständig vorexerziert — der Umbau ist ein
Spiegel dieses Blocks mit den Kriecher-Parametern (Drift-Felder **0** statt 400/−100/10/−50,
Kurs **rot_y** statt Peilung+2048, **kein** Stumpf-Marker auf Part 0).

---

## 5. Offene Punkte (ehrlich vermerkt)

1. **`Spieler+0x8` in RE2** (`0x800CFC00`) — welche Charakter-/Modell-Ids in Zweig A fallen,
   ist nicht byte-belegt (die RE2-Spieler-FSM liegt registerbasiert in `re2leon/PSX.EXE`;
   ein Scan nach absoluten Zugriffen auf `0x800CFBFC` liefert nur einen Store `@0x80059324`).
   Für den Port ist es der Zwilling von `DAT_800ACA5C & 4`.
2. **Welcher Knochen `LAB_8011F784[type] = 14` in der RE1.5-Bank ist** — die RE2-Bank EM010
   hat 15 Knochen mit `14 = L-Fuß` (`enemy_ai_re2_zombie.c:3426-3431`, an der geladenen Bank
   gemessen). Ob die RE1.5-Bank dieselbe Reihenfolge hat, ist hier **nicht** nachgemessen; die
   Bezeichnung „ARM-ABRISS" im Port-Kommentar (`enemy_ai_common.c:3492`) ist damit unbelegt.
   Für den RE2-Zweig ist es irrelevant — dort steht `1376 = 8·172` = Part 8 = Kopf **hart im
   Code**, das ist eindeutig.
3. **Liege-Klassifikation des Skript-Kriechers** (`grid=0x0D`, `+0x21A=0x0000`, §3.3) —
   eigener Befund, eigener Prüfauftrag.
4. Rumble `0x80039514(4,250,0)` hat im Port keinen Kanal (wie überall).

---

## 6. Werkzeug-Reproduktion

```bash
# Disasm (RE2-Overlay, RAW @0x80100000)
S=.claude/skills/re15-psx-disasm/scripts/re2_disasm.py
python $S dis   0x801029a4 120 --bin EMOVL10_S0.BIN     # Grab-P5 inkl. Kriecher-Gore
python $S dis   0x80102be8  20 --bin EMOVL10_S0.BIN     # P6 = der Tod des Kriechers
python $S table 0x80100204  12 --bin EMOVL10_S0.BIN     # RE2-Kriech-Opfer-Phasen
# Disasm (RE1.5)
T=.claude/skills/re15-psx-disasm/scripts/re15_disasm.py
python $T dis   0x80103fa4 100 --bin STAGE1.BIN         # RE1.5-Abwurf-Gore + SE 9
python $T dis   0x80104128  20 --bin STAGE1.BIN         # RE1.5-Kriecher-Tod
python $T table 0x80073f90   8                          # Spieler-Kommando-Tabelle
python $T table 0x8006e548  22                          # Waffen-Tester
# Sonde (Scratchpad-Build, kein Repo-Eingriff)
export PATH="/c/msys64/mingw64/bin:$PATH"     # sonst cc1 0xC0000139 (Git-mingw-DLL)
gcc -O1 -o probe_stomp.exe probe_stomp.c -Ire15_port/include \
    -DRE15_ASSET_PSX_DIR='"…/re15_port/shared_assets/PSX"' \
    -Wl,--start-group re15_port/build/tests/libre15_test_support.a \
    re15_port/build/engine/libre15_engine.a -Wl,--end-group -lm
RE15_AI_FLAVOR=re2 ./probe_stomp.exe 1800 2
```
