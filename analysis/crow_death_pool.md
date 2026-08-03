# Tote Krähen (0x21): Anim-Stopp + Blutlache — Death-Kette komplett, Corpse-Maschine, Port-Fix-Plan

**Datum:** 2026-08-03 · **Status:** Original-Seite **byte-belegt** (Roh-MIPS-Disasm `STAGE1.BIN`,
off = addr−0x80100000, via `re15_disasm.py`), Port-Seite **dynamisch gemessen**
(`re15_port/tests/unit/probe_crow_death.c`, neu — Krähe erschießen, 300 Corpse-Ticks loggen).
**Nutzer-Reports:** (a) abgeschossene Krähen animieren im Port am Boden weiter (Original: still),
(b) die Blutlache unter der abgeschossenen Krähe fehlt (Original hat sie).
**Schwester-Dossiers:** `analysis/crow_1170.md`, `analysis/crow_shot_attack.md` (Lane-Map F3),
`analysis/crow_victim_anim.md`. **Port:** `re15_port/engine/src/enemy_ai_common.c`
(`re15_crow_death` Z. 3933-3995, `re15_crow_special` Z. 4001-4004), Render
`re15_port/platform/pc/main.c` (NPC-Schatten Z. 5459-5528).

---

## 0. Executive Answer

1. **(a) Die Animation stoppt im Original mit dem letzten f314-Call des DEATH-FINISH-Steps**
   (`jal 0x8001f314` @0x8011493c, Argumente `[+0x84]/[+0x16c]` = Krähen-Bank). FINISH tickt
   f314 GENAU 12× (Land-Timer `+0x1d5=11` @0x80114908-10, unconditional-Dec @0x80114958-60),
   dann `+0x4=7` @0x80114978. **Der Corpse-State 7 enthält KEINEN f314-Call** (weder
   Dispatcher 0x801157e8 noch Handler 0x80115830/0x80115910) → die tote Krähe friert auf
   **Frame 12 des Land-Clips 0x0a** ein und hält ihn für immer. Der Port tickt
   `re15_crow_anim` in `re15_crow_special` weiter (enemy_ai_common.c:4003) → der Land-Clip
   (40 Frames) looped endlos. **Dynamisch belegt:** Probe = 300 frame-CHANGES in 300
   Corpse-Ticks, Spanne 0..39 (Original-Erwartung: 0, eingefroren bei 12 — genau der Frame,
   mit dem der Port in state 7 EINTRITT).
2. **(b) Die Blutlache ist der umgefärbte, wachsende BODEN-SCHATTEN der Krähe — kein
   ESP-Effekt.** Corpse-Handler `[0] = 0x80115830` (Dispatch `jalr @0x80121234[+0x5]`
   @0x801157f8-818): Schatten-Halbausdehnungen **`+0xbc/+0xbe += 10` pro Tick**
   (@0x8011589c-ac) für **51 Ticks** (Timer `0x32` @0x8011585c-60, unconditional-Dec
   @0x801158e0-e8), Farbwörter der beiden double-buffered Schatten-Prims
   **`+0xc4/+0xec = (alt & 0xff000000) | 0x00ffff38`** (@0x80115884-c8) = R=0x38, G=0xff,
   B=0xff — **subtraktiv gezeichnet = dunkelrote Lache** (identischer Mechanismus wie der
   bereits portierte Spieler-Todes-Pool LAB_8003694c @0x8003699c-b8 und der
   Zombie-Corpse-Settle FUN_80109554: derselbe +0xbc/+0xbe-Kanal, aber eigene
   Krähen-Maschine mit +10/Tick statt +8). Gezeichnet wird sie vom Root-Post-Pass
   `jal 0x8001b064(a1=+0x1ba)` @0x8011221c-234, der für ALLE States (auch 7) läuft.
   **FUN_80019700 spielt für die Lache KEINE Rolle** — der einzige ESP-Spawn im
   Death-Komplex ist die Waffen-14-Lane (fx-Wort `0x08032000` @0x80114c94-ac).
3. **GIB-Tod (Waffen 7-11/13/15-18) hat KEINE Lache:** die GIB-Lane endet in
   **state 7 sub 1** (`ori a0,zero,0x1` @0x80114b7c → 0x80115d74, delay-slot `sb 7,+0x4`
   @0x80114b90) = Handler `[1] 0x80115910` = **Pool-WIPE**: `+0xbc/+0xbe = 1`
   (@0x80115938-3c), Farben `&= 0xff000000` (schwarz, @0x80115948-54) → unsichtbarer
   1×1-Schatten. Der Port geht nach GIB fälschlich in **sub 0** (enemy_ai_common.c:3955).

---

## 1. Original — die Death-Kette vollständig (Disasm)

### 1.1 Dispatcher state 3 = 0x801146d0 (jeden Tick)

```
801146e0: lbu a1,454(v0)      ; +0x1c6 Kill-Flag-Index (em_set pc[7])
801146ec: jal 0x8004ef90      ; Flag in 0x800b1038 setzen (Persistenz)
80114700: lbu v0,5(v0)        ; +0x5 = weapon_id (Schuss-Resolver @0x800124bc)
80114708-24: sll 2; lw @0x801211cc[+0x5]; jalr   ; Lane-Dispatch OHNE Bounds-Check
```
Lane-Map (Tabelle @0x801211cc, crow_shot_attack.md F3): [0-6,12,19,20] → Normal 0x80114738;
[7,8,9,10,11,13,15,16,17,18] → GIB 0x801149c4; [14] → dritte Lane 0x80114ba4.

### 1.2 Normal-Lane 0x80114738 — Step-Router auf +0x7

Router @0x80114748-7c: step 0 → 0x80114784 (INIT, **fällt in FALL durch**), 1 → 0x80114828
(FALL), 2 → 0x80114934 (FINISH); a1 = g_entity bleibt über den Branch erhalten
(FINISH-f314 nutzt es).

- **INIT 0x80114784:** `Se(3)` (jal 0x800453d0, a0=3 @0x80114784-88); `+0x9a=−1`
  @0x80114798; `+0x1e4=0` (vvel) @0x801147a8; `+0x1e8=0x26` (grav) @0x801147b8; `+0x7++`
  @0x801147d4; `+0x8c=0` @0x801147e4; `+0x6c=0` @0x801147f8; Flock `aca50++`
  @0x801147fc-80c + armed-Re-Arm-Broadcast `(aca50&0xf0ff)|0x800` @0x80114810-24;
  **fällt ohne Break in 0x80114828 durch** (Fall beginnt im selben Tick; Port-Case-0
  break't → 1 Tick Versatz, kosmetisch).
- **FALL 0x80114828:** `+0x6c += 140`, Clamp 1024 (@0x8011483c-68); `+0x8c=0x3c`
  @0x80114878; `+0x1e4 += +0x1e8` @0x80114894-98; `y += vvel` @0x801148a8-bc (Delay-Slot
  von `jal 0x800245d8` pos_advance a0=0). **KEIN f314.**
  Land-Check @0x801148cc-d8: `y >= +0x1ba − 400` →
  `y = floor−400` @0x801148e4; **`+0x6c = 0`** (rot_z-Reset! @0x801148f8, Delay-Slot);
  Clip 0x0a (jal 0x80115d94, a0=0xa @0x801148f0-f4; Setter: `+0x94=a0; +0x95=0`
  @0x80115da0-b0); **`+0x1d5=0xb`** @0x80114908-10 (Delay-Slot); `Se(5)` @0x801148fc/90c;
  `+0x7++` @0x80114928-30.
- **FINISH 0x80114934:** **`jal 0x8001f314(a0=[ent+0x84], a1=[ent+0x16c], a2=0, a3=0x200)`**
  @0x80114934-40 — der LETZTE Anim-Tick-Ort der toten Krähe (Argument-Muster identisch
  INIT @0x801122f8-304 und move[6] @0x80112e2c-38: **das Krähen-Anim-Paar liegt in
  +0x84/+0x16c**, nicht +0x170/+0x174). Timer unconditional-Dec @0x80114950-60
  (`addiu v1,a0,255; bne a0,zero; sb v1` — Wrap-Muster); bei Timer==0:
  `addu a0,zero,zero` @0x80114964 → `jal 0x80115d74` @0x80114974 (Delay-Slot
  **`sb 7,+0x4`** @0x80114978; Setter schreibt `+0x5=a0=0; +0x6/+0x7=0` @0x80115d80-90)
  → **state 7 sub 0**; danach `word0 |= 0x2` @0x80114988-94 und `word0 |= 0x40`
  @0x801149a4-b0 (aec4-Kontakt-Skip-Bits; Port-Semantik `e->flags`,
  enemy_ai_common.c:2233).
  **f314-Bilanz:** Clip 0x0a startet bei Frame 0 (Land-Tick); FINISH läuft mit Timer
  11..0 = 12 Ticks à ein f314 → **Freeze-Frame = 12** (von 40). Port-Probe bestätigt:
  state-7-Eintritt exakt bei `motion=10 frame=12`.

### 1.3 CORPSE state 7 = 0x801157e8 — die Lachen-Maschine (KEIN f314)

Dispatcher: `lbu +0x5; jalr @0x80121234[+0x5]` @0x801157f8-818. Tabelle @0x80121234
(on-disc): **[0] 0x80115830, [1] 0x80115910**, [2] 0x801159bc, [3] 0x80115b80,
[4] 0x80115d6c. ⚠ [2..4] sind **Tabellen-ALIASING** mit der Hook-A-Victim-Tabelle
@0x8012123c (= 0x80121234+8, crow_victim_anim.md §1.1) — in state 7 unerreichbar:
vollständiger `jal 0x80115d74`-Census über 0x80112020-0x80116400 zeigt als
Corpse-sub-Setter NUR a0=0 (@0x80114964 Normal, @0x80114dec Lane 14) und a0=1
(@0x80114b7c GIB); kein state-7-Handler ruft den Setter.

**[0] = 0x80115830 — POOL-GROWER (Normal- und Lane-14-Corpse):** Step-Router auf +0x6
(@0x8011583c-58): step≥2 → `j 0x80115908` = `jr ra` (Halt).
```
step 0 @0x8011585c: ori v0,0x32; sb v0,469(a0)   ; +0x1d5 = 50
        @0x80115870-7c: +0x6++                    ; → fällt in step 1 durch
step 1 @0x80115880-84: a2 = 0x00ffff38 ; a1 = 0xff000000
        @0x80115894-9c: lhu +0xbc; addiu +10; sh  ; Schatten-Halbbreite  += 10
        @0x80115898-ac: lhu +0xbe; addiu +10; sh  ; Schatten-Halbtiefe   += 10
        @0x801158a4-c4: +0xc4 = (+0xc4 & 0xff000000) | 0x00ffff38   ; Prim-Farbe Buf A
        @0x801158b0-c8: +0xec = (+0xec & 0xff000000) | 0x00ffff38   ; Prim-Farbe Buf B
        @0x801158d8-e8: Timer unconditional-Dec (0→255-Wrap-Muster)
        @0x801158f8-904: bei Timer==0 → +0x6++    ; → step 2 = Halt für immer
```
→ **51 Wachstums-Ticks × +10** auf beiden Achsen. **Kein f314, kein ESP-Call, kein
weiterer Store** — Antwort auf Task-Frage: die Lache läuft über den
**Corpse-Pool-Kanal +0xbc/+0xbe wie beim Zombie-Corpse-Settle** (dort FUN_80109554,
+8/Tick, Budget 0x5a), nicht über FUN_80019700.

**[1] = 0x80115910 — POOL-WIPE (GIB-Corpse):** step 0 @0x8011592c-70: `+0xbc=1; +0xbe=1`
(`ori v0,zero,0x1` @0x80115928, sh @0x80115938-3c); `+0xc4 &= 0xff000000`,
`+0xec &= 0xff000000` (@0x80115940-54); `+0x6++` @0x8011596c-70; step≥1: `jr ra`
@0x80115924→0x80115974. → unsichtbarer Punkt-Schatten, keine Lache.

### 1.4 Warum der Schatten die Lache IST (Feld-Beweis)

- **Renderer:** Root-Post-Pass ruft für ALLE States (auch 7, auch im Freeze)
  `0x8001b064(a0=ent+0xb0, a1=+0x1ba)` @0x8011221c-234. `FUN_8001b064`
  (RE_15_Quellcode_V2): param_1 = Schatten-Deskriptor — **`param_1+0xc/+0xe` = Halbmaße
  (= ent+0xbc/+0xbe)**, Quad-Ecken (±w, 0, ±h), RotMatrixY(ent+0x6a), Position
  ent+0x34/+0x3c, y = a1 (Floor), Prim @param_1+0x10 (0x28 Bytes = POLY_FT4, texturiertes
  Schatten-Blob) double-buffered über `DAT_800aca34*0x28` → **Farbwort Buf A = ent+0xc4,
  Buf B = ent+0xec** — exakt die vier Felder, die die Corpse-Maschine schreibt.
- **Subtraktiv = rot:** RGB (0x38,0xff,0xff) subtrahiert Grün/Blau voll und Rot leicht →
  dunkelrote Lache. Identische Konstante ist im Port für den SPIELER-Todes-Pool bereits
  live-verifiziert portiert (LAB_8003694c @0x8003699c-b8; `re15_render_shadow_quad_c(...,
  0x38, 0xff, 0xff)` main.c:4841).
- **Basisgröße:** die Lache wächst auf dem LETZTEN State-1-Wert von +0xbc/+0xbe — der
  ACTIVE-Tail schreibt sie jeden State-1-Tick: **Halbmaße =
  `((y − +0x1ba) >> 4) + 400`, bei <50 → 100** (@0x80115fa0-e4: `sra 4; addiu 400;
  slti 50; ori 0x64`), **Grau-Tint = `((y − +0x1ba) >> 5) + 128`, bei <32 → 32**
  (@0x80115fe8-6058, Grauwort v|v<<8|v<<16 in +0xc4/+0xec, Top-Byte erhalten).
  Am Boden gestorbene Krähe (y=floor−400): Basis 375 → Lache wächst auf 885.
  States 3/7 aktualisieren die Basis nicht mehr (Tail ist State-1-only, jal @0x801125cc).

### 1.5 GIB-Lane 0x801149c4 — Feder-Burst = die 13 BONE-PARTS, Corpse ohne Lache

Step-Router auf +0x7 (@0x801149d4-ec): 0 → 0x801149f4, 1 → 0x80114b20.
- **step 0:** Flock `aca50++` @0x801149fc-08 + armed-Broadcast @0x80114a0c-20; `Se(3)`
  @0x80114a24-28; `+0x9a=−1` @0x80114a4c; **Schleife i=0..12 (sltiu 0xd @0x80114af4)**
  über die Records `*(ent+0x188) + i*0xac` (Index-Berechnung ×172 @0x80114a54-7c):
  `+0x68=0x8f`, `+0x94=0`, `+0x96=−50`, `+0x98=0`, `+0x9a=3`, `+0x9c=0`, `+0x9e=0`,
  `word0 |= 0x4a` (@0x80114a84-a4) — **Scatter-Armierung**. `+0x1d5=0x32` @0x80114ab4;
  crow `word0 |= 0x2` @0x80114acc, `|= 0x40` @0x80114ae8; `+0x7++` @0x80114b14-18.
  **+0x188 identifiziert:** `FUN_8001e56c` (Setter, RE_15_Quellcode_V2):
  `*(ent+0x188)=buf; return buf + (+0x83)*0xac` — beim Spawn allozierter
  **per-Bone-Part-Puffer, bone_count(+0x83) Records à 0xac**; Caller = Player-Init
  @0x80031614 und **Sce_em_set @0x80042524** (ghidra1_V2.txt XREF). 13 Records = die 13
  Krähen-Bones (EM021 dir[2] bone_count=13, crow_victim_anim.md §1.5) — der „Feder-Burst"
  ist das Auseinanderfliegen der 13 Bone-Parts des Körpers.
- **step 1 @0x80114b20:** Timer unconditional-Dec; bei 0: Schleife i=0..12
  **`sw zero,0(part)`** @0x80114b78 (alle 13 Parts deaktiviert → Körper weg), dann
  `ori a0,zero,0x1` @0x80114b7c → `jal 0x80115d74` @0x80114b8c, Delay-Slot `sb 7,+0x4`
  @0x80114b90 → **state 7 SUB 1 = Pool-WIPE**. **Kein f314 in der ganzen GIB-Lane** — die
  eigene Anim friert am Todes-Frame ein (irrelevant, Körper zerlegt), KEIN Fall, KEINE
  Lache.

### 1.6 Lane 14 = 0x80114ba4 — Spin-Fall + der einzige ESP-Spawn

step 0 @0x80114bf0: `Se(3)`; `+0x9a=−1`; `+0x1e4=0`; `+0x1e8=0x26`; `+0x7++`; `+0x8c=0`;
`+0x6c=0`; Flock-Writes @0x80114c68-90 (die aus crow_1170.md D6); dann
**`jal 0x80019700(a0=0x08032000, a1=+0x6a rot_y, a2=&ent+0x20, a3=0x8012110c)`**
@0x80114c94-ac — FUN_80019700 = ESP-Slot-Allokator (Pool @0x800a73b8, 0x60 Slots):
a0-Encoding Bank=(a0>>24)=0x08, Gruppe=(a0>>16)&7=3, low16=0x2000 → Slot+0x72;
a2 = Quellblock ent+0x20..+0x3f (enthält Position +0x34/+0x38/+0x3c); a3 = 4 Null-Wörter
@0x8012110c (die Daten direkt vor der Root-Tabelle @0x8012111c). Fällt in
step 1 @0x80114cb0 durch: identischer Spin-Fall wie Normal (Clamp @0x80114ce0-f0,
Land @0x80114d54-64: `+0x6c=0` @0x80114d80, Clip 0xa @0x80114d78-7c, `+0x1d5=0xb`
@0x80114d90-98, Se(5)); step 2 @0x80114dbc: f314 + Timer → **a0=0** @0x80114dec →
state 7 **sub 0** (`sb 7,+0x4` @0x80114e00) + word0-Bits @0x80114e18/34 → **hat die
Lache** wie Normal.

### 1.7 Root-Tabelle (Verify)

`table @0x8012111c`: [0] 0x8011224c INIT, [1] 0x80112420 ACTIVE, [2/5/6] 0x80114e4c Stub,
[3] 0x801146d0 DEATH, [4] 0x80114e54 FLIGHT-2, **[7] 0x801157e8 CORPSE** — der
Corpse-Handler läuft jeden Root-Tick, der Post-Pass (inkl. Schatten-Draw) danach.

---

## 2. Port-IST (statisch + dynamisch gemessen)

Probe: `re15_port/tests/unit/probe_crow_death.c` (ROOM1170 Re-Entry, Band 4; Krähe
anvisieren, `re15_player_weapon_fire(3)` bzw. `(7)`, dann 300 Corpse-Ticks loggen):

```
== weapon 3 (Normal): hp=−2 st=3 ss1=3 → state 7 nach 36 Ticks: ss1=0 motion=10 frame=12 y=floor−400
   Corpse: frame 13,14,15,… wrap … — frame-CHANGES=300/300, Spanne 0..39   ← Bug (a)
== weapon 7 (GIB):    hp=−2 st=3 ss1=7 → state 7 nach 1 Tick: ss1=0 (Original: 1!) motion=4 y=−10522
   Corpse: frame looped 0..9 (Flug-Clip 4), Körper bleibt sichtbar in der LUFT
```

| # | Port-Stelle | IST | Original |
|---|---|---|---|
| P1 | `re15_crow_special` enemy_ai_common.c:4001-4004 | tickt `re15_crow_anim` jeden State-7-Tick | state 7 hat KEINEN f314 (§1.3) → Freeze bei Frame 12 |
| P2 | Render main.c:5471-5490 | Corpse-Pool nur für Zombie-Typen (`nis_zombie`, Audit wf_246147e3); Krähe = normale 500/600-Schatten | Krähe hat EIGENE Pool-Maschine (§1.3/1.4): +10/Tick, 51 Ticks, 0x38/0xff/0xff |
| P3 | GIB-Ende enemy_ai_common.c:3955 | `re15_crow_sub(e, 0)` → sub 0 | a0=1 @0x80114b7c → **sub 1** (Pool-WIPE) |
| P4 | GIB-Körper | bleibt gerendert (Probe: Flug-Clip looped in der Luft) | 13 Bone-Parts scatter-armiert (@0x80114a50-aa4) und nach 50 Ticks getötet (`sw zero` @0x80114b78) → Körper weg |
| P5 | Land case 1 (Z. 3980-3986) | rot_z bleibt auf dem Fall-Wert (bis 1024) | `+0x6c=0` beim Land @0x801148f8 (Lane 14: @0x80114d80) — Corpse liegt ungerollt |
| P6 | Krähen-Schattengröße/-Tint | fix 500/600, fixes Grau | ACTIVE-Tail: Maße `((y−floor)>>4)+400` min-Klausel 100, Tint `((y−floor)>>5)+128` min 32 (@0x80115fa0-6058) — zugleich die Pool-BASIS |
| P7 | `e->flags` an den Lane-Enden | nicht gesetzt | `word0 |= 0x2/0x40` (Normal @0x80114988-b0, GIB @0x80114acc/ae8, Lane 14 @0x80114e18/34); Port-Konsument = aec4-Skip enemy_ai_common.c:2233 |
| P8 | Lane 14 | auf Normal-Lane gemappt (dokumentiert) | eigene Lane §1.6 inkl. ESP 0x08032000 |

Positiv verifiziert: Port-FINISH (Z. 3989-3993) == Original (f314 + Timer + state 7 sub 0;
state-7-Eintritt exakt bei frame=12 wie die Original-Bilanz §1.2); Death-Lane-Map (F3-Fix)
und Flock-Writes stimmen; Land-Timer 11 und GIB-Timer 0x32 stimmen.

---

## 3. Fix-Plan (jede Konstante zitiert)

1. **(a) Anim-Stopp — `re15_crow_special` ersetzen** durch die byte-true Corpse-Maschine
   (Dispatch @0x80121234[+0x5], §1.3), OHNE `re15_crow_anim`:
   ```c
   switch (e->sub_state_1) {
   case 0:                                   /* 0x80115830 POOL-GROWER */
       if (e->sub_state_2 == 0) { e->crow_timer = 0x32;            /* @0x8011585c-60 */
                                  e->sub_state_2 = 1; }             /* @0x80115870-7c, fällt durch */
       if (e->sub_state_2 == 1) {
           e->crow_shadow_w += 10; e->crow_shadow_h += 10;          /* @0x8011589c-ac */
           e->crow_pool = 1;               /* +0xc4/+0xec = top|0x00ffff38 @0x80115884-c8 */
           uint8_t t = e->crow_timer; e->crow_timer = (uint8_t)(t-1); /* @0x801158d8-e8 */
           if (t == 0) e->sub_state_2 = 2;                          /* @0x801158f8-904 */
       }
       break;                                /* step>=2: Halt (j 0x80115908) — KEIN f314 */
   case 1:                                   /* 0x80115910 POOL-WIPE (GIB) */
       if (e->sub_state_2 == 0) { e->crow_shadow_w = e->crow_shadow_h = 1; /* @0x80115938-3c */
                                  e->crow_pool = 0;                 /* &=0xff000000 @0x80115948-54 */
                                  e->sub_state_2 = 1; }             /* @0x8011596c-70 */
       break;                                /* step>=1: jr ra @0x80115974 */
   }
   ```
2. **(b) Lache rendern:** neue per-Actor-Felder `crow_shadow_w/h` (Port von +0xbc/+0xbe)
   + `crow_pool`-Flag (Port des Farbzustands +0xc4/+0xec):
   - ACTIVE-Tail (State-1-Tick): `w = h = clamp(((y − crow_floor)>>4)+400, <50→100)`
     @0x80115fa0-e4; Tint `clamp(((y−crow_floor)>>5)+128, <32→32)` @0x80115fe8-6058.
   - Render main.c NPC-Loop: für Typ 0x21 `nhx/nhz` aus `crow_shadow_w/h`;
     `crow_pool` → `re15_render_shadow_quad_c(..., 0x38, 0xff, 0xff)` (dieselbe Farbe wie
     der Spieler-Pool @0x8003699c-b8; Krähen-Wort 0x00ffff38 @0x80115880-84); Tint des
     Normal-Schattens aus dem Tail-Wert. Der Zombie-Gate-Kommentar (wf_246147e3) wird
     korrigiert: die Krähe gehört NICHT zu „keine Pool-Maschine", sie hat eine eigene.
3. **GIB → sub 1:** enemy_ai_common.c:3955 `re15_crow_sub(e, 1)` (`ori a0,zero,0x1`
   @0x80114b7c). Zusätzlich Körper ab GIB-step-0 verbergen (Render-Flag) — Original:
   Parts scatter-armiert @0x80114a50-aa4, getötet `sw zero,0(part)` @0x80114b78; der
   ESP-Feder-Stand-in (`re15_esp_fx_splatter`, 13) bleibt als dokumentierte Annäherung,
   bis der Part-Scatter-Mover RE'd ist (§4.2).
4. **Land-Detail:** `e->rot_z = 0` beim Land (@0x801148f8; Lane 14 @0x80114d80).
5. **Flags:** `e->flags |= 0x02|0x40` an den drei Lane-Enden (Adressen P7) — Konsument
   aec4-Skip existiert bereits (enemy_ai_common.c:2233).
6. **Optional Lane 14 byte-true** (statt Normal-Mapping): Spin-Fall §1.6 + ESP-Spawn
   0x08032000 (sobald die fx-Identität geklärt ist, §4.3).

**Verifikation:** `probe_crow_death` nach dem Fix: weapon 3 → state 7 sub 0,
frame-CHANGES=0, eingefroren bei motion=10/frame=12, `crow_shadow_w/h` wächst Basis+510
über 51 Ticks, danach konstant; weapon 7 → state 7 **sub 1**, `crow_shadow_w/h==1`.
Visuell per Skill `re15-port-visual-verify` (gdigrab): liegende Krähe still + dunkelrote
Lache; GIB: Federn, kein Körper, keine Lache.

---

## 4. Offen (ehrlich)

1. **word0-Bits 0x2/0x40 — Konsumenten-Census unvollständig:** Port modelliert sie als
   aec4-Kontakt-Skip (Z. 2233, aus dem Zombie-RE); ein vollständiger EXE-Census (wer liest
   Bit 1/6 noch — Targeting? Draw?) steht aus. Hund-Corpse (@0x8010ecfc/ed18) und
   Krähen-Corpse setzen sie identisch und bleiben sichtbar → sie verbergen NICHT.
2. **Part-Scatter-Mover/-Renderer:** welcher EXE-Code integriert die armierten
   Part-Records (+0x94/+0x96/+0x98 = Velocity?, +0x9a=3, +0x68=0x8f, word0|=0x4a) pro Tick
   und zeichnet sie — nicht RE'd (Kandidat: der Skelett-Draw-Pass, der +0x188 pro Bone
   konsumiert). Bis dahin ist der Port-Feder-Stand-in eine dokumentierte Annäherung.
3. **fx 0x08032000 (Lane 14):** ESP-Bank 8 / Gruppe 3 — welcher visuelle Effekt das ist
   (und welche Waffe id 14 trägt), ist nicht gemappt.
4. **PSX-Live-Capture der Krähen-Lache** (DuckStation, Krähe im echten Spiel erschießen,
   VRAM/Frame-Vergleich der Lachen-Größe über die 51 Ticks) — statisch ist die Kette
   vollständig; der Hardware-Beweis der Subtraktiv-Optik stützt sich bisher auf den
   live-verifizierten Spieler-Pool mit identischer Farbkonstante.
5. **Initialwert +0xbc/+0xbe vor dem ersten ACTIVE-Tick** (af5c-Spawn-Default): für die
   Krähe irrelevant (Tail überschreibt ab Tick 1), beim Port-Feld-Init dokumentieren.
