# FINDING 6 (RE1.5-KI) — Hunde-Finisher: Positionierung + „Kopf abreissen"

Nutzer-Report: *„stimmt die positionierung des Hundes noch nicht ganz beim finisher.
Ausserdem reisst er hier den Kopf nicht ab im Gegensatz zu RE 2 AI."*

Stand: 2026-08-24. Reines RE + Messung, **keine Code-Aenderung**.
Alle Adressen selbst disassembliert (`re15_disasm.py` gegen `info/Re1.5/PSX/BIN/STAGE*.BIN`,
`info/Re1.5/PSX.EXE`, `info/re2leon/COMMON/BIN/EMD0G_MOD0.BIN`), alle Clip-Zahlen selbst aus
`re15_port/shared_assets/PSX/EMD/CDEMD0.EMS` (EM020 = EMS-Blob 7 @Datei 0x152800, len 166912)
geparst.

---

## 0. Die Kette, byte-belegt

**Spielerblock-Basis** = `0x800ACA54` (Beleg: `addiu a0,a0,-13740` @0x80111A84 direkt vor dem
ad68-Aufruf auf den Spieler). Daraus: `+0x4`=cmd `0x800ACA58`, `+0x5`=Variante `0x800ACA59`,
`+0x6`=Phase `0x800ACA5A`, `+0x8f`=Blend `0x800ACAE3`, `+0x93`=hit_react `0x800ACAE7`,
`+0x94`=Clip `0x800ACAE8`, `+0x95`=Frame `0x800ACAE9`, `+0x6a`=Yaw `0x800ACABE`,
`+0xA0/+0xA2`=Anker `0x800ACAF4/F6`, `+0x188`=Part-Pool-Zeiger `0x800ACBDC`,
`+0x1A8`=**Greifer-Zeiger** `0x800ACBFC`.
`DAT_800AC784` = Zeiger auf die aktuell getickte Entity; in den Spieler-Handlern = der SPIELER
(`RE15_FUN_CATALOG.md`: FUN_80031C44 „DAT_800ac784=player").

| Stufe | Adresse | Was |
|---|---|---|
| Hund ACTIVE sub 9 / 10 (Griff) | `0x8010F80C` / `0x8010FC60`, Phasen @`0x80100204` | Latch/Fressen/Release |
| Hund ACTIVE sub 0xB (Finisher) | `0x801100B4` | Kadaver-Fressen, Clip 0x1B |
| Hund ACTIVE sub 0xC (Recover) | `0x801101E4` | nach Break-free |
| Spieler-Hook A (cmd 5) | `0x80111944` → Tabelle `0x801210D8`[aca59] → **Maschine A `0x80111984`**, Phasen @`0x801002AC` (9) | Ringkampf |
| Spieler-Hook B (cmd 6) | `0x80111CB0` → Tabelle `0x801210F0`[aca59] → **Maschine B `0x80111CF0`**, Phasen @`0x801002D4` (5) | Kollaps/Tod |

Der Griff-Commit (sub 9 Schritt 0, `0x8010F84C`) setzt byte-belegt:
```
8010f8b0/b8: lhu v1,-372(a0) ; sh v1,160(v0)   ; HUND +0xA0 := playerX  (a0 = 0x800ACBFC, -372 -> 0x800ACA88)
8010f8c8/d0: lhu v0,playerZ  ; sh v0,162(v1)   ; HUND +0xA2 := playerZ
8010f8b4:    sw v0,0(a0)                        ; 0x800ACBFC := &Hund   (Greifer-Zeiger)
8010f8ec/f4: sh v0,-13580(at) / sh v1,-13578(at); SPIELER +0xA0/+0xA2 := playerX/Z
8010f954/78: addiu a0,a0,-372 ; jal 0x8001a8f8 (a1=0x800)  ; HUND-Yaw schnappt auf bearing(Hund->Spieler)
8010f910:    sb 5,0x800ACA58                    ; Spieler cmd 5
8010f940/4c + f950/5c: 0x800ACBCC := Hund+0x178, 0x800ACBD0 := Hund+0x17C  ; Opfer-Bank (EM020 Bank 2)
```
**Beide Anker (Hund und Spieler) sind derselbe Punkt: Leons Position im Griff-Moment.**
Genau darauf baut die ganze Choreografie auf.

`FUN_8001AD68(a0=Entity, a1=Skel, a2=Anim)` — selbst disassembliert:
```
8001add8: lh a0,106(s1)        ; RotMatrixY(entity yaw)
8001adf4: lh v0,160(s1)        ; Anker X (+0xA0)
8001ae00/04: addu v0,v0,v1 / sw v0,52(s1)   ; entity.X = AnkerX + rot(rootoffset).x
8001ae08/18: lh v0,162(s1) ... sw v0,60(s1) ; entity.Z = AnkerZ + rot(rootoffset).z
```
= exakt `re15_clip_root_motion_abs` (enemy_ai_common.c:622).

---

## TEIL (a) — POSITIONIERUNG: der Hund bekommt seine Wurzelbewegung im Port NIE

### a.1 Wo das Original den Hund platziert

`jal 0x8001AD68`-Zensus ueber die gesamte Hunde-Region von `STAGE1.BIN` (0x8010D000–0x80112400,
selbst gescannt):

| Adresse | Kontext | a0 / a1 / a2 |
|---|---|---|
| `0x8010F998` | sub 9 Schritt 1 (Latch-Anim `0x8010F980`) | **Hund** / Hund+0x84 / Hund+0x16C |
| `0x8010FA34` | sub 9 Schritt 3 (Fress-Loop `0x8010FA1C`) | **Hund** / +0x84 / +0x16C |
| `0x8010FB80` | sub 9 Schritt 5 (Release-Anim `0x8010FB68`) | **Hund** / +0x84 / +0x16C |
| `0x8010FDEC` / `0x8010FE88` / `0x8010FFD4` | sub-10-Zwillinge derselben Schritte | **Hund** |
| **`0x8011018C`** | **sub 0xB Phase 1 (`0x80110174`) = DER FINISHER** | **Hund** / +0x84 / +0x16C |
| `0x80111A88` / `0x80111AD8` / `0x80111B84` | Maschine A Phasen 1/3/5 | Spieler / `0x800ACBCC` / `0x800ACBD0` |
| `0x80111D84` | Maschine B Phase 1 | Spieler / `0x800ACBCC` / `0x800ACBD0` |

Instruktions-Beleg fuer den Finisher (`FUN_801100B4`):
```
801100e4: sb 1,6(a0)          ; Hund +0x6 = 1
801100f4: sb 0x1b,148(v1)     ; Hund +0x94 = Clip 27 (0x1B)
80110104/114: sb zero,149 / sb zero,150
80110124: sb 7,143(v1)        ; +0x8f = 7
80110138: sb 6,0x800ACA58     ; Spieler cmd 6
80110140/148: sb zero,0x800ACA59 / sb zero,0x800ACA5A   ; Variante 0, Phase 0
80110150: sw v1,0x800ACBFC    ; Greifer := Hund
80110154-70: 0x800ACBCC/D0 := Hund+0x178 / Hund+0x17C
--- FAELLT DURCH in Phase 1 (kein Sprung) ---
80110184/88: lw a1,132(v0) / lw a2,364(v0)   ; Hund-EIGENE Skel/Anim (+0x84 / +0x16C)
8011018c:    jal 0x8001ad68                   ; PLATZIERUNG des HUNDES aus Anker + Clip-Wurzel
801101a8:    jal 0x8001f314 (a3=0x200)        ; DANACH erst der Frame-Advance
801101c4/c8: +0x6 += f314-Ergebnis            ; Phase 2 = ab dann NICHTS mehr (Ruecksprung @0x801100D4)
```
Der Kontrast sitzt eine Funktion weiter: `FUN_801101E4` (sub 0xC, Recover) hat **kein** ad68 —
nur `jal 0x8001f314` @`0x80110288`. Das ad68 in sub 0xB ist also gewollt, kein Muster-Artefakt.

### a.2 Was der Port macht

`re15_port/engine/src/enemy_ai_common.c`
* `:6990-6999` sub 0x0B: `re15_dog_clip(e,0x1b)` + `re15_player_victim_devour(e)` + `re15_dog_anim(e)` — **kein** `re15_clip_root_motion_abs`.
* `:6360-6455` `re15_dog_grabhold` Schritte 1/3/5: nur `re15_dog_anim(e)` — **kein** `re15_clip_root_motion_abs`.
* Gegenprobe: `grep re15_clip_root_motion_abs` liefert 8 Fundstellen (Zeilen 1094, 1713, 2053, 3409, 3426, 3489, 3548, 3737) — **keine einzige** im Hunde-Block (6300–7030).

Der Anker ist im Port korrekt gesetzt (`e->anchor_x = pl->x` :6371, Spieler-Anker :6403) — es
fehlt **nur der Konsument**. Die Spielerseite (`re15_victim_place` :1084) ist byte-true.

### a.3 Messung — die fehlenden Einheiten

Wurzel-Track = `keyframe[+6/+8/+10]` (`re15_emd_get_keyframe_speed`, emd_common.c:334), aus
EM020 selbst geparst. Frame-Zahlen decken sich 28/28 mit `s_dog_clip_len` (:6021).

**Hund, Bank 0 (eigene Anim):**

| Clip | Phase | Frames | sx-Verlauf | sz-Verlauf | Amplitude, die der Port NICHT faehrt |
|---|---|---|---|---|---|
| `0x17` (23) | Latch vorn | 29 | **−1479 → +200** | 0 → −3 | **1679 Einheiten Zustoss** |
| `0x1A` (26) | Latch hinten | 30 | **−1507 → +200** | 0 → −3 | **1707 Einheiten Zustoss** |
| `0x18` (24) | Fress-Loop | 18 | 209…244 | −4…+38 | ±44 Wippen |
| `0x19` (25) | Release | 40 | 240 → 146 | **39 → 1865** | **1826 Einheiten Rueckzug** |
| **`0x1B` (27)** | **Finisher** | **99** | **241 → 304(f27) → −903(f79) → −128(f98)** | konstant 38 | **1207 Einheiten Zerr-Bewegung** |

**Spieler, Bank 2 (Opfer-Set, 5 Clips {29,18,60,30,90}):**

| Clip | Phase | Frames | sx | sz |
|---|---|---|---|---|
| 0 | Intro vorn | 29 | 41 → 344 | 0 |
| 1 | Hold | 18 | 342 → 341 | 0 |
| 2 | Release | 60 | 337 → 251 | 0 |
| 3 | Intro hinten | 30 | 0 → 344 | −189 → 0 |
| **4** | **Kollaps** | **90** | **konstant 337** | **0** |

**Die Geometrie des Finishers (beides auf denselben Anker A):**
Maschine A Phase 0 (`0x801119C0`) schnappt Leons Yaw:
`a1 = 0x800 − (aca59<<12)` (@0x801119D4/F8/FC) → Variante 0 = +0x800 (Schritt 180° ⇒ Snap auf
`bearing(Spieler→Hund)`), danach `+0x6a += 2048` (@0x80111A2C-40); Variante 1 = −0x800
(Snap auf `bearing+0x800`, kein Nachschlag). **Beide Varianten enden bei
`bearing(Spieler→Hund)+0x800` = `bearing(Hund→Spieler)` = exakt dem Hunde-Yaw θ**
(der Hund hat den beim Latch per `FUN_8001A8F8(&Spieler.pos, 0x800)` @0x8010F978 geschnappt;
Schrittweite 0x800 ⇒ die `slt`-Klemme @0x8001A974 greift immer ⇒ harter Snap).

⇒ Hund und Spieler stehen mit **gleichem Yaw** auf **gleichem Anker**, die Trennung ist rein
`sx_Hund − sx_Spieler` entlang θ:

| Frame des Kollaps-Clips | sx Hund (27) | sx Leon (4) | Trennung |
|---|---|---|---|
| f0 | 241 | 337 | −96 |
| f27 | 304 | 337 | −33 |
| f39 | −347 | 337 | −684 |
| f49 | −549 | 337 | −886 |
| **f79** | **−903** | 337 | **−1240** |
| f98 (Ende) | −128 | 337 | −465 |

Der Hund zerrt also im Original 1240 Einheiten vom Koerper weg und kommt auf 465 zurueck —
eine sichtbare Zerr-/Fress-Bewegung. Im Port steht er **bewegungslos** dort, wo ihn der Bissprung
abgestellt hat.

**Gemessen im Port** (`re15_port/build/tests/unit/test_dog_grab_anchor.exe`, echter `game_step`,
ROOM1190-Spawns):
```
[sub 9  vorn ] commit=(9628,-19020) anker=(9628,-19020) max-dist-zum-Hund=2433
[sub 10 hinten] commit=(9628,-19020) anker=(9628,-19020) max-dist-zum-Hund=2237
```
Byte-true SOLL-Maximum ueber Griff **und** Finisher = die Latch-Frame-0-Trennung
`|−1479 − 41| = 1520` (vorn) bzw. `|−1507 − 0| + lateral 189 ≈ 1519` (hinten); waehrend des
Fressens/Kollapses nie mehr als **1240**.
⇒ **Port-Ueberschuss ≈ +913 (vorn) / +718 (hinten) Einheiten** am schlechtesten Punkt, und
**0 von 1679 / 1707 / 1826 / 1207** Einheiten der authored Wurzelbewegung werden gefahren.

### a.4 Zweiter, unabhaengiger Positions-/Posen-Defekt am Finisher-Ende

Original: sub 0xB Phase 1 zaehlt `+0x6 += f314` (@0x801101C4-C8); ab Phase 2 kehrt der
Dispatcher sofort zurueck (`slti v0,v1,2 / beq → 0x801101CC` @0x801100D4-D8) — **f314 wird nie
wieder gerufen**, der Part-Pool behaelt also die zuletzt geschriebene Pose = Keyframe **98**
(die Wrap-auf-0 in f314 betrifft nur `+0x95`, nicht die schon geschriebene Pose; dieselbe Lehre
steht schon in `re15_dog_anim_hold_last`, enemy_ai_common.c:6120-6134).

Port `:6996`: `if (re15_dog_anim(e)) e->sub_state_2 = 2;` — `re15_dog_anim` setzt
`anim_frame = 0` beim Clip-Ende, und der Renderer posiert jeden Frame neu ⇒ der Hund steht nach
dem Fressen in **Clip-27-Frame-0** (Anfangspose, aufrecht) statt in Frame 98.
Das ist derselbe Fehler, der schon fuer die Hunde-Leiche gefixt wurde — `re15_dog_anim_hold_last`
existiert bereits, wird hier aber nicht benutzt.

### a.5 URTEIL (a)

**Bestaetigt, Port-Defekt, Mechanismus vollstaendig belegt.**
Der Hund wird im Original in **jedem** Griff-, Fress-, Release- und Finisher-Frame per
`FUN_8001AD68` aus seinem gemeinsamen Anker mit dem Spieler neu platziert; der Port faehrt davon
**nichts**. Umsetzung (naechste Phase, nicht hier):
1. In `re15_dog_grabhold` Schritte 1/3/5 und in sub 0x0B Phase 1 jeweils
   `re15_clip_root_motion_abs(e, &bank->skel, &bank->anim, e->motion, e->anim_frame)`
   **VOR** dem Frame-Advance (Original-Reihenfolge ad68 → f314, @0x8011018C vs. @0x801101A8).
2. sub 0x0B Phase 1: `re15_dog_anim_hold_last` statt `re15_dog_anim` (Pin auf Keyframe 98).
3. sub 0xC (Recover) bekommt **kein** ad68 — Gegenbeleg `FUN_801101E4` hat keins.
4. Jede Konstante traegt die Adressen aus a.1/a.3.

---

## TEIL (b) — „KOPF ABREISSEN": RE1.5 tut es NICHT, und das ist korrekt

### b.1 Die RE1.5-Part-Maschine existiert (und kann Koepfe abreissen)

Spieler-Part-Pool = `*(0x800ACBDC)`, Record-Stride **0xAC (172)**, Flag-Wort bei `rec+0x00`
(Details: `analysis/konstruktion_2026-08-23/gore-model-swap.md`; Bits 0x8 Scatter, 0x40 Detach,
0x20 Wurf-Physik, 0x1062 = die kombinierte Abriss-Armierung ueber `FUN_8001F220`).

**Beleg, dass RE1.5 dem SPIELER Teile abreisst — inklusive dem Kopf:** in `STAGE4.BIN` /
`STAGE5.BIN` gibt es eine Kollaps-Maschine mit *exakt derselben Bauform* wie die Hunde-Maschine B
(Dispatcher `0x8011621C`, 5 Phasen @`0x801003AC`, Hook-Varianten-Tabelle @`0x8011A288`,
[4]/[5] = Maschine A `0x80115D00`, [12..15] = Maschine-B-Familie `0x8011621C`/`0x801165F4`/…).
Deren Phase 1 stempelt frame-getaktet auf `0x800ACAE9` (= Spieler `+0x95`):

```
801162e8: lbu v1,0x800ACAE9 ; ori v0,0x1d ; bne     ; FRAME 0x1D
80116314: jal 0x80019700 (a0=0x1500, a2=pool+2128 = Record 12 +0x40)
8011632c: jal 0x80045024 (0x04010001)
80116338-4c: lw v0,2064(pool) ; xori v0,0x1062 ; sw  ; Record 12 word0 ^= 0x1062  (ABRISS)
80116350-68: lw v0,2236(pool) ; xori 0x200 ; sw      ; Record 13 (Kind) Rot-Freeze
8011636c-84: lw v0,2408(pool) ; xori 0x200 ; sw      ; Record 14 (Kind) Rot-Freeze
80116388-a4: sb 0x14,2181 / sh -128,2194             ; rec12 +0x75 Gravity, +0x82 Spin
801163b0-e0: sh …,2120/2122/2124                     ; rec12 +0x38/+0x3A/+0x3C Wurf-Velocity

801163ec: lbu v1,0x800ACAE9 ; ori v0,0x3c ; bne      ; FRAME 0x3C
8011641c: jal 0x80019700 (a0=0x2000, a2=pool+1440 = Record 8 +0x40)
80116434: jal 0x80045024 (0x04030001)
80116448: lw  v0,1376(pool)                          ; **Record 8 = DER KOPF** (1376 = 8*172)
80116450: xori v0,v0,0x1062                          ; ABRISS-Armierung (Wurf-Physik + Detach)
80116454: sw  v0,1376(pool)
80116464: sb  0x14,1493(pool)                        ; rec8 +0x75 = 20   (Gravity)
80116474: sh  -128,1506(pool)                        ; rec8 +0x82 = -128 (Spin-Winkel)
80116484: sb  8,1494(pool)                           ; rec8 +0x76 = 8    (Spin-Rate)
80116494: sh  zero,1432(pool)                        ; rec8 +0x38 = 0    \
801164a4: sh  -100,1434(pool)                        ; rec8 +0x3A = -100  > Wurf-Velocity
801164b4: sh  zero,1436(pool)                        ; rec8 +0x3C = 0    /
```
(Der Kopf wird also mit −100 nach oben weggeschleudert — dasselbe `FUN_8001F220`-Muster
`word0 ^= 0x1062`, `+0x75 = 0x14`, `+0x76 = 8` aus gore-model-swap.md.)
(1376 = 8·172, 2064 = 12·172, 2236 = 13·172, 2408 = 14·172 — exakt Record-Grenzen.
STAGE5-Zwilling: `0x80116394` ff., Stellen `0x801164B8…0x801168FC`, byte-gleiche Offsets.)
Record 8 = Kopf ist unabhaengig belegt: derselbe Offset `pool+0x5A0` (= 8·172+0x40) ist
game-weit der **Kopf-Blut-Anker** (`re15_player_victim_bone_pos(8,…)`, enemy_ai_common.c:898;
Pose-Messung „PL00-Bone 8 = Kopf" :827).

### b.2 Die Hunde-Kette fasst den Spieler-Part-Pool NIE an

**Game-weiter Zensus** aller Instruktionen mit Immediate `0xCBDC` (= `0x800ACBDC`, der einzige
Weg an den Spieler-Part-Pool) ueber alle `STAGE*.BIN` — 105 Treffer. In **STAGE1** sind es
genau **9**, alle `lw a2,-13348(a2)`, also **Argument a2 = FX-Anker**, nie ein Ziel:

| Adresse | Kontext |
|---|---|
| `0x8010A53C`, `0x8010A568` | Zombie cmd-5-Maschine, Blut |
| `0x8010A788`, `0x8010A844` | Zombie cmd-6-Kollaps, Blut |
| **`0x80111B38`** | **Hund Maschine A Phase 4**, Blut `0x1500`, Anker `pool+0x5A0` |
| **`0x80111DD4`**, **`0x80111E28`** | **Hund Maschine B Phase 1**, Blut `0x2000`, Frames 0x3A / 0x29 |
| `0x8011C480`, `0x8011C510` | anderer STAGE1-Greifer, Blut |

Dazu die vollstaendige Roh-Disassemblierung der gesamten Hunde-Kill-Kette
(`0x8010F80C`–`0x801102D0` Hundeseite, `0x80111944`–`0x80111F08` Spielerseite): darin steht
**kein** Store auf ein Part-Record, **kein** `word0 |= / ^=`, **kein** Mesh-/Modelltausch, **kein**
Aufruf einer Part-/Gore-Funktion. Die einzigen Praesentations-Aufrufe sind
`FUN_80019700` (Blut), `FUN_80045024` (Se_on), `FUN_80045630` (Boden-Fall-SE @0x80111DA4),
`FUN_80037EDC` (Wundstempel @0x80111B60/`0x80111E78`/`0x80111E84`/`0x80111E90`),
`FUN_8001F314` (anim_set) und `FUN_8001AD68` (Platzierung).

Auch in der EXE gibt es keinen Hunde-Pfad dorthin: die 28 `0xCBDC`-Treffer in `PSX.EXE` sind
Player-Init/Allokator (`0x8003163C`, vgl. gore-model-swap.md „Player-Init @0x80031614"),
Waffen-Bindung (`0x80036BBC`/`0x80036DD4`, beide direkt neben der Waffen-Basistabelle
`0x800741E8`) und Render-/Hit-Pfade — keiner armiert Scatter/Detach.

### b.3 Und was macht RE2 wirklich?

Zensus aller Zugriffe auf `+408 (0x198 = Part-Pool-Zeiger)` in `EMD0G_MOD0.BIN` (RE2-Hund):
11 Treffer. **Zehn** davon gehen auf den **Hund selbst** (`s0`/`s1`/`a0` = Hund):
Ganz-Modell-Tinten ueber alle 17 Records (`0x801047E8` 0x00202020, `0x8010484C` 0x3F2F,
`0x8010496C` 0x3F203F), sowie `word0 |= 0x4A` @`0x801044A4` = der **Hunde-eigene** Abriss.
**Genau einer** geht auf den Spieler:

```
80101d3c-40: lui s3,0x800d ; addiu s3,s3,-1032    ; s3 = 0x800CFBF8 = RE2-SPIELERBLOCK
...  (sub 7 Biss-Latch, Clip 23)
80101f48-50: lbu v1,333(s2) ; ori v0,98  ; bne    ; FRAME 98 -> nur HUND:
80101f60-80:   lw a0,408(s2) ; sw 96,840(a0) ; sw 0x101010,844(a0) ; word0(688)|=0x80
                                                  ; = Hund-Record 4 (688 = 4*172)
80101f8c:    bne v1,102 ...                       ; FRAME 102 -> SPIELER:
80101f98:      addiu a3,s3,408                    ; a3 = &Spieler-Part-Pool
80101fa8-f0:   lbu v1,0x80105508[i]  (i=0..3)     ; Tabelle = 08 00 09 0C
80101fe4-f0:   sw 96,152(rec) ; sw 0x101010,156(rec) ; word0 |= 0x80
```
Tabelle `0x80105508` = **{8, 0, 9, 12}** (byte-gelesen: `08 00 09 0c`).
`word0 |= 0x80` ist **kein** Abriss, sondern der **Farb-Blend** (Konsument `FUN_80027434` →
`FUN_8002940C`, siehe gore-model-swap.md): die Prim-Farben faden pro Frame Richtung
`rec+0x98 = 96` (dunkelrot) und `rec+0x9C = 0x00101010` (fast schwarz).
⇒ **Auch RE2 reisst Leon den Kopf NICHT ab.** RE2 faerbt Leons Kopf (Record 8) plus Rumpf/0,
9 und 12 blutig-dunkel, waehrend der Hund am Kopf haengt. Kein `0x8` (Scatter), kein `0x40`
(Detach), kein `0x1062`, kein `word0 = 0`, kein Mesh-Tausch.

Was der Nutzer im RE2-Modus zusaetzlich sieht, ist die **Choreografie**: die RE2-Opfer-Bank des
Hundes traegt **einen** Clip mit **145 Frames**, synchron zum Hunde-Latch-Clip 23 (145 F) —
der Hund haengt Leon am Kopf/Nacken und schuettelt (enemy_ai_common.c:779-786).
RE1.5 fahrt stattdessen das Paar *Hund-Clip 0x1B (99 F, Kadaver-Fressen)* / *Leon-Bank2-Clip 4
(90 F, Kollaps, Wurzel konstant 337)* — eine andere, ruhigere Inszenierung.

### b.4 URTEIL (b)

**Kein Port-Defekt. Der Bericht beschreibt einen Unterschied, keinen Fehler.**
* RE1.5 hat die Spieler-Zerlege-Maschine (STAGE4 `0x80116338`/`0x80116440`, STAGE5
  `0x801164B8`/`0x801165C0`) und reisst dort sogar **Record 8 = den Kopf** ab — die Hunde-Kette
  benutzt sie nachweislich **nicht** (0 Part-Writes in `0x8010F80C`–`0x801102D0` und
  `0x80111944`–`0x80111F08`; 3 Zugriffe auf den Pool, alle als Blut-Anker).
  Da beide Maschinen dieselbe Bauform haben (5-Phasen-Maschine B, Frame-Gate auf `0x800ACAE9`,
  Blut + Se_on), ist die Abwesenheit beim Hund eine **Autoren-Entscheidung**, kein
  uebersehener Zweig.
* RE2 reisst ebenfalls keinen Kopf ab; sein einziger Eingriff am Spielermodell ist der
  Farb-Blend auf {8,0,9,12} @`0x80101F8C-FF8`.
* ⇒ **Der Port ist im RE1.5-Modus hier korrekt.** Nichts implementieren.

**Einziger echter, adressbelegter RE2-Rueckstand** (bereits als OPEN markiert,
`enemy_ai_re2_dog.c:1108-1114`): der Part-Blend am Frame 98 (Hund-Record 4) und Frame 102
(Spieler-Records {8,0,9,12}) → Ziel `96` / `0x00101010`, Flag `0x80`. Wenn der Nutzer die
RE2-Optik will, ist **das** die Sollseite — Blut-Verfaerbung, nicht Enthauptung.

---

## Zusammenfassung fuer die Umsetzung

| Teil | Urteil | To-do |
|---|---|---|
| (a) Positionierung | **PORT-DEFEKT, belegt** | `FUN_8001AD68` (= `re15_clip_root_motion_abs`) in die Hunde-Griff-Schritte 1/3/5 (`0x8010F998`/`0x8010FA34`/`0x8010FB80` + sub-10-Zwillinge) **und** in sub 0x0B Phase 1 (`0x8011018C`), jeweils VOR dem Advance; sub 0x0B zusaetzlich `re15_dog_anim_hold_last` (Pin Keyframe 98). Fehlende Amplituden: 1679 / 1707 / ±44 / 1826 / 1207 Einheiten. Port-Ueberschuss gemessen: +913 (vorn) / +718 (hinten). |
| (b) Kopf abreissen | **KEIN Defekt** | Nichts. RE1.5 hat hier bewusst keinen Kopfabriss; RE2 auch nicht (nur Farb-Blend). Optional/OPEN: RE2-Part-Blend {8,0,9,12} @`0x80101F8C-FF8`. |
