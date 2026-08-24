# ROOM10D0 / RE2-KI — Sitzer-Aufstehen + Bodentreffer-Blut: RE-Dossier (2026-08-24)

**Nutzer-Report (FINDING 3, RE2-KI, ROOM 10D0):** „stimmt der Zombie jetzt fast, nur beim
aufstehen wiederholt sich die Eine Animation noch einmal kurz, bis er dann letztlich steht.
Ausserdem wurde mein Schuss bei ihn auf den Boden wieder nicht mit Blut quittiert."

**Ergebnis in zwei Sätzen:**

* **(a)** Die Zustands-/Clip-Folge des Sitzers ist byte-true (Original-FSM selbst disassembliert,
  §1.1). Der Fehler sitzt im **Übergabe-Tick vom RE1.5-Aufsteher an das RE2-Gehirn**: der Port
  löscht `re2z_re15_pose` und schreibt das RE2-Wort `0x101`, **kehrt aber zurück, ohne das
  RE2-Gehirn in diesem Tick laufen zu lassen** — der Aktor trägt für genau einen Frame noch den
  RE1.5-Clip-Index 0x29 (41) in `+0x94`, und die RE2-Bank hat nur 31 Clips, also wird
  `41 % 31 = 10` posiert = eine **LIEGE-Pose**. Weil der RE2-WALK im Folgetick mit `+0x8f = 0xF`
  (Rate 0x100) startet, mischt der FRAC-Crossfade diese Liege-Pose noch **11 weitere Frames**
  ein: der Zombie klappt am Ende des Aufstehens auf den Boden und steht **ein zweites Mal auf**
  (gemessen: Brusthöhe 2266 → **179** → über 11 Ticks zurück auf 2806). Im RE1.5-Flavor
  existiert dieser Frame nicht (dort ist Clip 41 in der 43-Clip-Bank gültig; gemessen 2266 →
  2266 → 2272). **Port-Defekt, nur im RE2-Flavor.**
* **(b)** Der 10D0-Sitzer ist beim Treffer **st=1 / +0x5=0x12 / +0x6=1 (bzw. 3) / grid=0x00 /
  +0x21A=0x0000** — er ist damit **nicht** „liegend" klassifiziert, der schon nachgezogene
  Liege-Blutpfad EXEC[5] case 6 (@0x801035B0-DC) ist hier **strukturell unerreichbar**.
  Zuständig ist der **Haupt-HURT-Emitter @0x80105640-710** (`addiu a0,zero,6096` @0x8010567C,
  `jal 0x8001bf10` @0x80105708). Der Port ruft ihn — es spawnt **genau EIN** Partikel
  (Anker Part 0, Skalierung 0x1500). Derselbe Schuss im RE1.5-Flavor spawnt **NEUN** Partikel
  (1 Burst 0x2000 am Gore-Bone + 8 Splatter-Trigger). Das Blut fehlt also nicht, es ist im
  RE2-Modus **ein einzelner kleiner Sprite an der Hüfte statt Burst+Splatter an der Brust**.

⛔ **Keine Code-Änderung in dieser Phase.** Neu im Baum ist ausschliesslich die Mess-Sonde
(§5) + ihr CMake-Eintrag (kein `add_test`).

---

## 0. METHODE (STOP-GATE Schritt 1)

Sonde: `re15_port/tests/unit/probe_10d0_situp_re2.c` — echter Weg über `re15_game_step`,
echte `ROOM10D0.RDT`-Spawns (sub00 `Ck(4,247)==0`-Zweig), **echte RE2-Bank** EM010 aus
`shared_assets/RE2/CDEMD0.EMS` und die **RE1.5-Pose-Bank**, die die Plattform im Hybrid-Lauf
registriert (`platform/pc/main.c:539-548` → `re15_re2z_set_re15_pose_bank`). Ohne diese Bank
fällt sel 0x0e in den RE2-Liege-Fallback und man misst einen anderen Pfad.

Pro Tick protokolliert: `state/+0x5/+0x6/+0x7/+0x9`, `re2z_re15_pose`, `+0x94` (Clip),
`+0x95` (anim_frame), `re15_actor_clip_len` (die Uhr), der **Render-Slot** (`frame % len`),
der **aufgelöste Keyframe** und die **Weltposition von Bone 8 (Brust) / Bone 0 (Hüfte)** — aus
**genau der Bank, aus der `platform/pc/main.c:6804-6816` posiert**, inkl. der Wahl
`clip_override`. Zwei Betriebsarten:

* **QUERY** (`g_anim_pose_actor = NULL`) — reine Keyframe-Pose ohne Crossfade.
* **RENDERER-SEMANTIK** (`g_anim_pose_actor = e`, genau EIN Aufruf/Tick) — mit dem
  FRAC-Crossfade `FUN_8001f3bc` (Gewicht `+0x8f * rate`, `skeleton_common.c:199-209`), also
  das, was der Nutzer wirklich sieht. **Der Defekt in §1.4 ist NUR in dieser Betriebsart in
  voller Länge sichtbar** — im State-Log ist er ein einziger Frame.

Läufe: `probe_10d0_situp_re2 0` (RE2, QUERY) · `4` (RE1.5, QUERY) · `6` (RE2, RENDERER) ·
`7` (RE1.5, RENDERER) · `1/2/3` (Schuss, Aim LEVEL/DOWN/UP) · `5` (Schuss, RE1.5-Flavor).

Zusatzlauf am echten Spiel (`RE15_DEBUG_JUMP="10d0@gp"`, `RE15_ANIM_TRACE`) bestätigt nur den
Aufbau (`[debug-menu] AUTO-JUMP -> ROOM10D0 (Frame 1768)`, `[enemy] Hybrid EM10: RE1.5-Geometrie
(15 Meshes) unter RE2-Rig (15 Bones, 31 Clips)`, `[esp] room 10D0: … id 0x05/0x07`,
`[esp] global bank CORE00.ESP: 5 effects (incl effect-id 0 hit fx)`); die Trace-Datei bricht
beim `taskkill` im Puffer ab (kein `fflush` im Trace-Writer) und ist deshalb **nicht** als
Beleg verwendet.

---

## 1. (a) AUFSTEH-WIEDERHOLUNG

### 1.1 ORIGINAL — die komplette Sitz-/Aufsteh-Kette (selbst disassembliert, STAGE1.BIN)

**Spawn-Decoder sel 0x0E** (in `FUN_80100688`, Zitat aus `zombie_lyer_10d0.md` §2.1 hier
nur referenziert, nicht neu gezogen): `+0x94 = 0x2A`, `+0x95 = 0`, `+0x8f = 0`,
`anim_set(+0x170,+0x174, a2=0, a3=0x200)` @0x80100FB0, `word +0x4 = 0x00001201` @0x80100FC4
(→ State 1, `+0x5 = 0x12` SLEEPING-LYING), `+0x9 = 0` @0x80100FD4.

**Schlaf-Automat `FUN_801054f4`** (`@0x8011f890[0x12]`) — Sprungtabelle @0x801000e4, 5 Einträge
(`sltiu v0,v1,0x5` @0x8010550c). Tabelle selbst gedumpt:

```
[0] 0x801000e4 -> 0x80105534     [1] 0x801000e8 -> 0x8010560c (= EPILOG, KEIN Rumpf)
[2] 0x801000ec -> 0x80105554     [3] 0x801000f0 -> 0x80105594     [4] 0x801000f4 -> 0x801055d0
```

```
; case 0
80105534-40: ori v1,zero,0x1 ; sb v1,440(v0)      ; +0x1B8 = 1  (Liege-Latch)
8010554c-50: j 0x8010560c ; sb v1,6(v0)           ; +0x6 = 1    -> WARTEN
; case 1 = 0x8010560c = Epilog  -> KEIN f314  -> +0x95 friert  (= die SITZ-Pose)
; case 2  (fällt in case 3)
8010555c-60: ori v0,zero,0x2a ; sb v0,148(v1)     ; +0x94 = 0x2A
80105570   : sb zero,149(v0)                      ; +0x95 = 0
8010557c-80: ori v0,zero,0x3  ; sb v0,6(v1)       ; +0x6 = 3
; case 3
80105590   : sb zero,143(v0)                      ; +0x8f = 0   (harter Pose-Cut)
8010559c-a8: addu a2,zero,zero ; lw a0,368(v0) ; lw a1,372(v0) ; jal 0x8001f314
801055ac   : ori a3,zero,0x200                    ; a2 = 0 (vorwärts), a3 = 0x200 (Blend-Rate)
801055c4-cc: addu v1,v1,v0 ; sb v1,6(a0)          ; +0x6 += (Clip fertig)
; case 4
801055d8-dc: ori v0,zero,0xd01 ; sw v0,4(v1)      ; word +0x4 = 0xD01 -> +0x5 = 0x0D STANDUP
801055ec   : sb zero,440(v0)                      ; +0x1B8 = 0
80105604-08: andi v0,v0,0xfe ; sb v0,147(v1)      ; +0x93 &= 0xFE
```

**Aufsteher `FUN_80104a50`** (`@0x8011f890[0x0d]`), Verzweigung über `+0x6`:

```
80104a60-68: lbu v1,6(a0) ; ori v0,zero,0x1 ; beq v1,v0,0x80104ae8   ; +0x6 == 1 -> NUR advancen
80104a78   : beq v1,zero,0x80104a9c                                  ; +0x6 == 0 -> Eintritt
80104a88-90: ori v0,zero,0x2 ; beq v1,v0,0x80104b24 ; ori v0,zero,0x201 ; +0x6 == 2 -> AUSGANG
; Eintritt (+0x6 == 0), fällt danach in den Advance-Block:
80104a9c   : sb v0,6(a0)                          ; +0x6 = 1
80104aa8-ac: ori v0,zero,0x29 ; sb v0,148(v1)     ; +0x94 = 0x29  (AUFSTEH-CLIP)
80104abc   : sb zero,149(v0)                      ; +0x95 = 0
80104ac8-d0: ori v0,zero,0x7 ; jal 0x8001af20 ; sb v0,143(v1)  ; +0x8f = 7  (Crossfade-Saat)
80104ad4-e4: andi v0,v0,0x3 ; bne -> skip ; jal 0x800453d0 ; ori a0,zero,0x5  ; 1/4 SE 5
; Advance-Block (von Eintritt UND +0x6 == 1 erreicht):
80104af0-b00: addu a2,zero,zero ; lw a0,368 ; lw a1,372 ; jal 0x8001f314 ; ori a3,zero,0x200
80104b18-20: addu v1,v1,v0 ; sb v1,6(a0)          ; +0x6 += (Clip fertig)
80104b24   : sw v0,4(a0)                          ; +0x4 = 0x201  -> ENGAGE (+0x5 = 2)
```

Damit ist die Original-Folge **belegt**: `0x2A` einmal ab Frame 0 (28 Frames), dann `0x29`
einmal (59 Frames), dann Engage. `a3 = 0x200` ist die **Blend-Rate** (Gewicht der Vor-Pose =
`+0x8f * a3`, s. `skeleton_common.c:199-209`), **nicht** die Frame-Rate: der Frame-Schritt ist
`+1` pro `f314`-Aufruf (`8001f610: lbu v0,149(v1) / 8001f618: addiu v0,v0,1`).

### 1.2 ORIGINAL — was das RE2-Gehirn beim Übergang tut

RE2-ACTIVE-Wurzel (`EMOVL10_S0.BIN`), **DECISION und EXECUTOR im SELBEN Tick**:

```
801011a8: lbu  v0,5(s0)            ; +0x5
801011bc: lw   v0,-14196(at)       ; DECISION-Tabelle 0x8010c88c
801011c4: jalr v0                  ; <- schreibt ggf. ein neues +0x4/+0x5
801011d0: lbu  v0,5(s0)            ; +0x5 ERNEUT gelesen
801011e4: lw   v0,-14132(at)       ; EXECUTOR-Tabelle 0x8010c8cc
801011ec: jalr v0                  ; <- der Executor des NEUEN Substates läuft SOFORT
```

EXECUTOR[1] (WALK) P0 setzt das Clip-Wort:

```
80101a74-78: addiu v0,zero,1 ; sb v0,6(s1)        ; +0x6 = 1
80101a7c   : lbu  v0,536(s1)                      ; +0x218 = Walk-Clip-Index
80101a80-84: lui  v1,0xf ; addu v0,v0,v1          ; | 0x000F0000
80101a8c   : sw   v0,332(s1)                      ; +0x14C  -> Clip + FRAC-Saat 0xF
```

→ Im Original ist der Frame, in dem `+0x4 = 0x101` geschrieben wird, **derselbe** Frame, in dem
der Walk-Clip in `+0x94` landet. Es gibt keinen Tick mit „neuer Zustand, alter Clip".

### 1.3 PORT-IST — die gemessene Folge (RE2-Flavor, Sonde Modus 0/6)

Bänke (Sondenausgabe):

```
AKTIONS-BANK (RE2 EM010): clips=31   len: [0]=59 [1]=60 … [10]=50 … [30]=3
RE1.5-POSE-BANK (Sitz-Import): clips=43   [0x29]=59  [0x2A]=28
```

| Phase | Ticks | st/+0x5/+0x6 | Clip | anim_frame | Keyframe | Brusthöhe ü. Boden |
|---|---|---|---|---|---|---|
| Sitzen (halten) | ∞ | 1 / 0x12 / 1 | 0x2A (42) | **0 gepinnt** | 951 | 863 |
| Wecken (dist 1693 < 0xBB8) | t0…t27 | 1 / 0x12 / 3 | 0x2A | 0→27 | 951→965 | 863 → **1041** (t16) → 685 (t24) → 700 |
| Übergabe-Tick Schlaf→Aufsteher | t28 | 1 / **0x0d** / 0 | 0x2A | 27 (Hold) | 965 | 700 |
| Aufstehen | t29…t87 | 1 / 0x0d / 1 | **0x29 (41)** | 0→58 | 921→950 | 700 → **2266** |
| **Übergabe-Tick Aufsteher→RE2** | **t88** | 1 / **0x01** / 0 | **41 (ungültig!)** | 58 | **505** | **179** ⛔ |
| RE2-WALK | t89… | 1 / 0x01 / 1 | 0 | 0→ | 0→ | (s. §1.4) |

Der `+0x95`-Halt in der Wartephase ist im Port der Pin `player_common.c:1034-1038`
(`mo == 0x2A && +0x5 == 0x12 && +0x6 <= 1`) und deckt sich mit dem Original (case 1 = Epilog
ohne `f314`, §1.1).

### 1.4 ⛔ DER DEFEKT — Übergabe-Tick t88 und der 11-Frame-Nachlauf

**Ursachenkette (alles gemessen + im Port zitierbar):**

1. `enemy_ai_common.c:4807-4817` (Sitz-Import-Delegation in `re15_enemy_ai_live_tick`):
   ```c
   if (e->state == 1 && e->sub_state_1 == 0x0d) {
       re15_enemy_ai_standup_animate(e);            /* [0xd] = FUN_80104a50 */
       if (e->state == 1 && e->sub_state_1 == 2) {
           re15_ai_set_state_word(e, 0x101);
           e->re2z_re15_pose = 0;
       }
       return 1;                                    /* <- re15_re2z_tick läuft NICHT */
   }
   ```
   `+0x94` bleibt dabei **41 (RE1.5-Clip 0x29)**. Das widerspricht §1.2: im Original schreibt der
   Executor sein Clip-Wort im selben Tick (@0x80101a8c).
2. Der Clip-Index wird jetzt gegen die **RE2**-Bank aufgelöst:
   * Uhr: `re15_actor_clip_len` (`enemy_ai_common.c:4586-4600`) nimmt die RE1.5-Pose-Bank **nur
     bei `re2z_re15_pose != 0`** — das Flag ist gerade gefallen → `41 >= 31` → **len = 0**.
   * Pose: `re15_compute_actor_kf` (`anim_select_common.c:73-75`):
     `if (clip_idx >= anim->clip_count) clip_idx = clip_idx % anim->clip_count;` → `41 % 31 = 10`
     → RE2-Aktions-Clip 10 (50 Frames), `cur = 58` → Hold-Last-Slot 49 → **Keyframe 505**.
   * `re15_enemy_hold_last_frame` (`enemy_ai_common.c:1576-1582`) hätte klemmen können, tut es
     aber nicht: es fragt `re15_enemy_find(e->type)` = die RE2-Bank und steigt bei
     `(int)e->motion >= hb->anim.clip_count` sofort aus.
3. **Gemessene Pose** dieses Frames: Brust **179**, Hüfte **251** über dem Boden — gegenüber
   2266 / 1727 im Tick davor. Der Zombie liegt flach.
4. Im Folgetick lädt EXEC[1] P0 den Walk-Clip mit **`+0x8f = 0xF`, Rate `0x100`**
   (`enemy_ai_re2_zombie.c:1103`, `re2z_clip(e, walkclip, 0, 0xF, 0x100, 1)`, Original
   @0x80101a7c-8c). Der FRAC-Crossfade (`skeleton_common.c:199-209`,
   `wp = anim_frac * rate`, Klemme 0x1000) mischt damit **93,75 % der zuletzt GERENDERTEN
   Pose** ein — und das ist die Liege-Pose aus 3. Der Crossfade ist rekursiv, also zieht sich
   das über die volle `+0x8f`-Rampe.

**Messung mit Renderer-Semantik (Modus 6), Brusthöhe über Boden:**

```
t87 2266   (Aufsteher fertig)
t88  179   <- Übergabe-Frame, Liege-Pose
t89  354   frac=15 rate=256  (93,75 % Vor-Pose)
t90  680   frac=14
t91 1107   frac=13
t92 1559   frac=12
t93 1966   frac=11
t94 2286   frac=10
t95 2509   frac=9
t96 2648   frac=8
t97 2727   frac=7
t98 2767   frac=6
t99 2785   frac=5   … ab t101 stabil ~2800
```

→ **Der Zombie fällt am Ende des Aufstehens für einen Frame flach auf den Boden und richtet
sich über 11 weitere Frames erneut auf.** Bei 30 fps sind das ~0,4 s — genau das gemeldete
„wiederholt sich die Eine Animation noch einmal kurz, bis er dann letztlich steht".

### 1.5 GEGENPROBE — RE1.5-Flavor hat den Frame NICHT (Modus 7, gleiche Semantik)

```
t87 2266   clip=41 af=58 kf=950
t88 2266   clip=41 af=58 kf=950   <- Clip 41 ist in der 43-Clip-RE1.5-Bank GÜLTIG, Hold-Last
t89 2272   clip=4  af=29 kf=155   (RE1.5-Engage, Loco-Bank)
t90 2280 … glatt weiter
```

Damit ist der Defekt eindeutig **RE2-Flavor-spezifisch** und liegt an der Clip-Index-Kollision
zwischen RE1.5-Clip-Nummern und der 31-Clip-RE2-Bank, nicht an der Aufsteh-FSM.

### 1.6 Was ausdrücklich **NICHT** der Defekt ist (damit die Umsetzung nicht daneben greift)

Innerhalb von Clip **0x2A** geht die Brust erst hoch (863 → 1041, t0…t16) und dann wieder
runter (1041 → 685, t17…t24), bevor 0x29 sie auf 2266 zieht. Das sieht wie ein „Ansetzen und
Zurücksinken" aus, **ist aber die Original-Animationsdaten**: die beiden Clips sind als Kette
authored — Clip 0x2A Endframe (kf 965) und Clip 0x29 Startframe (kf 921) liefern **exakt
dieselbe Pose** (Brust 700, Hüfte 763). Der Ablauf ist Wandsitz → auf die Knie → Oberkörper
nach vorn → Aufstehen. Beide Flavor messen hier identisch (§1.5 zeigt denselben Verlauf).
**Hier nichts „glätten".**

---

## 2. (b) BODENTREFFER OHNE BLUT

### 2.1 MESSUNG — welchen Zustand trägt der Sitzer beim Treffer

Sonde Modus 1 (Aim LEVEL), Pistole (Waffe 3), Schuss in Tick 5; Pass 0 = dist 3425 (Sitzer
schläft, `+0x6 = 1`), Pass 1 = dist 1693 (Sitzer ist schon in der Weck-Phase `+0x6 = 3`).
**Beide Pässe identisch:**

```
t4  st=1 s1=0x12 s2=1 grid=0x00 pose15=1 clip=42 af=0  kf=951  Brust 863   21A=0000 1D2=0
t5  st=2 s1=0x03 s2=1 grid=0x00 pose15=0 clip=0  af=0  kf=0    Brust 2726  21A=0000 1D2=1
    hp 90 -> 74, fx-Spawns = 1, letzter Hit-Handler = 1
```

Also beim Treffer: **State 1, `+0x5 = 0x12` (importierte RE1.5-SLEEPING-Maschine),
`+0x6 = 1` bzw. `3`, `+0x9 (grid) = 0x00`, `+0x21A = 0x0000`, `re2z_re15_pose = 1`.**

### 2.2 Warum der schon nachgezogene Liege-Blutpfad hier **nicht** greifen kann

Die Liege-Klassifikation im Resolver (`re15_damage.c:1267-1271`) ist
`(grid & 0x80) || (RE2-Typ && (+0x21A & 0x2))` — RE1.5-Zwilling @0x80101614-20, RE2-Bit
@0x80105168-70. Beim Sitzer ist **beides 0** (gemessen oben; `grid` wird vom sel-0x0E-Decoder
gelöscht, `@0x80100FD4`, und der Sitz-Import setzt kein `+0x21A`-Bit,
`enemy_ai_re2_zombie.c:6820-6835`). Der Sitzer ist also für Zielband **und** Treffer-Routung
ein **stehendes** Ziel. Der EXEC[5]-case-6-Blutspawn (@0x801035B0-DC, Gate `+0x16B == 1`) hängt
am Liege-Pfad und ist hier unerreichbar — die Nachziehung von 2026-08-23 kann diesen Report
gar nicht betreffen.

Nebenbefund: der Treffer landet in der Sonde bei **allen drei** erzwungenen Aim-Elevationen
(LEVEL/DOWN/UP, Modus 1/2/3, jeweils hp 90 → 74). Das Bandtor ist hier also nicht die Ursache.
(Die Elevation wurde über den Testhaken `re15_player_set_aim_elevation_for_test` gesetzt —
ob der echte Aim-FSM sie im selben Frame hält, ist **nicht** gemessen, s. §4.)

### 2.3 ORIGINAL — welcher Blut-Spawn zuständig ist (selbst disassembliert, EMOVL10_S0.BIN)

Der Treffer stempelt (`re15_damage.c:1548-1555`, RE2-Applier-Zwilling @0x80047294/@0x80047324):
`+0x1D2 = 1`, `+0x6 = 0`, `+0x5 = Zeile(Pistole) = 3`, `+0x4 = 2`. Damit läuft der
**HURT-Master @0x80104F40**, Zeile 3, Phase 0 — und dessen Blut-Block ist:

```
80105640: lbu  v1,5(s1)              ; +0x5 (Zeile)
80105644: addiu v0,zero,16
80105648: beq  v1,v0,0x80105714      ; Zeile 16 (Flammenwerfer) -> KEIN Blut
80105650: lbu  v1,466(s1)            ; +0x1D2
80105654-6c: multu 0xAAAAAAAB …      ; v1 % 3   (ZONE)
80105678: beq  v1,zero,0x801056dc    ; Zone 0 -> Anker Part 1
8010567c: addiu a0,zero,6096         ; <- DELAY-SLOT: BEIDE Zweige nehmen Effekt-Id 6096
; Zone != 0 (unser Fall, 1 % 3 == 1): Anker Part 0
80105158-Zwilling: addiu a2,s0,72    ; +0x198 + 0*172 + 72   (Part-0-MATRIX)
; Zone 0:
801056e0-f0: addiu v0,zero,800 ; sh zero,16(sp) ; sh v0,18(sp) ; sh zero,20(sp)  ; v = {0,800,0}
80105704: addiu a2,a2,244            ; +0x198 + 1*172 + 72   (Part-1-MATRIX)
801056e8-700: sll a1,s3,16 ; sra a1,a1,16 ; subu a1,v0,a1    ; a1 = +0x76 - s3
80105708: jal 0x8001bf10
```

`FUN_8001bf10` selbst (re2leon/PSX.EXE) belegt **einen** Slot: `t2 = 96` Slots, Stride 124,
`t6 = a0>>24`, `t5 = (a0>>16)&0xff`, erster freier Slot wird initialisiert
(@0x8001bf10-bfac). Ein Aufruf = **ein Emitter-Slot** (dessen Row-VM danach die Partikel treibt).

### 2.4 PORT-IST — der Spawn passiert, ist aber ein Zehntel des RE1.5-Auftritts

| | Spawns | Anker | Skalierung | gemessene Höhe des Ankers |
|---|---|---|---|---|
| **RE2-Flavor** (`enemy_ai_re2_zombie.c:931-943`, aufgerufen @`:4374`) | **1** | Part 0 (Hüfte, `re2z_blood_anchor` → Zone 1) | **0x1500** | dy 1995 |
| **RE1.5-Flavor** (`re15_damage.c:1732-1744`) | **9** | Gore-Bone `@0x8011f784[type]` (Brust) | 0x2000 + Splatter | dy 2318 |

Sondenausgabe (Modus 1 vs. Modus 5, gleicher Schuss, gleicher Zombie):

```
RE2  : hp 90 -> 74, fx-Spawns=1, FX-Anker part=0 pos=(5785,-1995,25725)
RE15 : hp 80 -> 64, fx-Spawns=9, FX[0..8] pos=(5767,-2318,25731)
```

Die 8 zusätzlichen Partikel im RE1.5-Pfad kommen aus
`re15_esp_fx_splatter(bank, 0, 8, …)` (`re15_damage.c:1743`). Die **Stream-Daten** dieses
Splatters sind byte-true aus `CORE00.ESP` (`re15_esp.c:635-645`, Zeilen @0x94C/0x9A0/0x9F4);
die **Trigger-Zahl 8** an dieser Aufrufstelle trägt **kein** `@0x…`-Zitat. Ebenso trägt die
RE2-Skalierung `0x1500` kein Zitat — das Original übergibt in `a0` low16 den Wert
**0x17D0** (= 6096, @0x8010567C).

Der Effekt ist auflösbar: `[esp] global bank CORE00.ESP: 5 effects (incl effect-id 0 hit fx)`
(Effekt-Id 0 kommt aus der GLOBAL-Bank; ROOM10D0 selbst lädt nur 0x05/0x07). Es fehlt also
**kein Sheet** — es fehlt die Menge/Position.

### 2.5 KEIN Divergenz-Befund: der Sitzer springt beim Treffer auf

Gemessen springt die Brust im Treffer-Tick von 863 auf 2726 (RE2) bzw. 2312 (RE1.5) — der
Sitzer wird also in **beiden** Flavors sofort in die stehende Trefferreaktion gerissen (RE2:
HURT-P0 lädt `re2z_walkclip`; RE1.5: Stagger-Clip 4 der Loco-Bank). Das ist **kein**
RE2-spezifischer Fehler und gehört nicht in diesen Fix.

---

## 3. FIX-RICHTUNGEN (mit Belegen — Umsetzung ist NICHT Teil dieser Phase)

**(a)** Der Übergabe-Tick muss so aussehen wie im Original (§1.2: DECISION und EXECUTOR im
selben Tick, @0x801011a8-ec; der Executor schreibt sein Clip-Wort @0x80101a8c). Zwei belegte
Varianten:
* im Sitz-Import nach `re15_ai_set_state_word(e, 0x101)` **nicht** `return`en, sondern in
  `re15_re2z_tick(slot)` durchfallen — dann lädt EXEC[1] P0 den Walk-Clip im selben Frame und
  der `0xF/0x100`-Crossfade startet von der stehenden Aufsteh-Endpose (genau das, was der
  RE1.5-Pfad de facto tut, §1.5);
* alternativ `re2z_re15_pose` erst fallen lassen, **nachdem** das RE2-Gehirn `+0x94`
  überschrieben hat.
  Beide brauchen **keine** neue Konstante. Zusätzlich sollte `re15_enemy_hold_last_frame`
  (`enemy_ai_common.c:1576`) dieselbe Bankregel benutzen wie `re15_actor_clip_len` — heute
  klemmt es beim Sitzer nie (RE2-Bank, Clip 41 out of range).

**(b)** Nutzer-Mandat „Präsentation = RE1.5" trifft hier genau: der RE2-Treffer-Emitter ist
byte-true **ein** Slot (§2.3), sieht im Port aber wie ein einzelner Sprite an der Hüfte aus,
während der RE1.5-Pfad Burst + Splatter an der Brust zeigt. Die Entscheidung, ob der RE2-Pfad
denselben Auftritt bekommt (Splatter + Gore-Bone-Anker), ist eine **Präsentations-Entscheidung
unter dem Mandat**, keine byte-true-Frage — und sie muss dann für **alle** RE2-Treffer gelten,
nicht nur für 10D0. Die uncitierte `0x1500` gegen die Original-low16 `0x17D0` (@0x8010567C)
ist unabhängig davon ein sauber belegbarer Ein-Zeilen-Abgleich.

---

## 4. OFFEN (ehrlich)

1. **Der Beweis am echten Fenster fehlt.** Der Defekt in §1.4 ist über die exakte
   Renderer-Bank-/Keyframe-/Crossfade-Kette gemessen, aber nicht per gdigrab gefilmt. Für die
   Abnahme des Fixes gehört ein `re15-port-visual-verify`-Lauf dazu (Skill), weil genau diese
   0,4 s der sichtbare Teil sind.
2. **`RE15_ANIM_TRACE` verliert beim `taskkill` den Puffer** (kein `fflush` im Writer,
   `anim_select_common.c:126-133`) — der Live-Trace-Weg war deshalb nicht auswertbar. Ein
   `fflush` dort würde diese Messklasse am echten Spiel freischalten.
3. Ob der **echte Aim-FSM** die Elevation im Schuss-Frame so hält, wie der Testhaken sie setzt,
   ist nicht gemessen (§2.2). Für „Schuss auf den am Boden liegenden Zombie" (nicht Sitzer)
   gilt weiterhin der EXEC[5]-Pfad; der wurde hier nicht erneut geprüft.
4. `+0x1B8` (Liege-Latch, `@0x80105540`/`@0x801055ec`) hat im Port weiterhin keinen Konsumenten
   — unverändert offen wie in `zombie_lyer_10d0.md` §5.2.
5. Die **Trigger-Zahl 8** des RE1.5-Splatters (`re15_damage.c:1743`) und die **Skalierung
   0x1500** des RE2-Stand-ins (`enemy_ai_re2_zombie.c:941`) sind beide ohne `@0x…`.
6. `re15_enemy_bone_world_pos` posiert für den Blut-Anker immer `b->anim/b->skel`; für einen
   Aktor mit `re2z_re15_pose` wäre das die falsche Bank. Im gemessenen Fall fällt es nicht auf
   (der HURT-P0 setzt den RE2-Clip vor dem Spawn), bleibt aber eine latente Kante.

## 5. ARTEFAKTE

* Sonde: `re15_port/tests/unit/probe_10d0_situp_re2.c` (+ Eintrag in
  `re15_port/tests/unit/CMakeLists.txt`, **kein `add_test`**).
  Modi: `0` RE2/QUERY · `4` RE1.5/QUERY · `6` RE2/RENDERER · `7` RE1.5/RENDERER ·
  `1|2|3` Schuss (Aim LEVEL/DOWN/UP) · `5` Schuss im RE1.5-Flavor.
* Disasm (selbst gezogen, `.claude/skills/re15-psx-disasm/scripts`):
  `re15_disasm.py dis 0x801054f4` / `table 0x801000e4 5` / `dis 0x80104a50` (STAGE1.BIN);
  `re2_disasm.py --bin info/re2leon/COMMON/BIN/EMOVL10_S0.BIN dis 0x801011a8|0x80101a74|
  0x80105640|0x80105670|0x801056dc`, `re2_disasm.py dis 0x8001bf10` (re2leon/PSX.EXE).
* Aufbau-Beleg aus dem echten Lauf: `debug.log` mit `AUTO-JUMP -> ROOM10D0 (Frame 1768)`,
  Hybrid-EM10-Zeile, `[esp] room 10D0` und `[esp] global bank CORE00.ESP`.
