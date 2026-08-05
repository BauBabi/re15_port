# ROOM1070 — die 5 Zombies wachen nicht auf: RE-Dossier (2026-08-05)

**Nutzer-Report (BUG 5):** „Die Zombies in ROOM1070 sollten ab einem gewissen Punkt zum
Aufwachen triggern, tun sie aber nicht."

**Ergebnis in einem Satz:** Der Trigger ist ein **SKRIPT-Wake** — ein AUTO-AOT (Slot 14,
sce=3) feuert beim Durchqueren eines Korridor-Streifens `sub_scd[2]`, das per
`Member_set(12, …)` das Behavior-Byte `+0x9` jedes Zombies von **0x88→0x8A** bzw.
**0x87→0x89** umschreibt (Grid-Nibble **+2**). Genau dieser Nibble-Wechsel schaltet die
Sub-Modus-Tabelle `@0x8011f80c` vom **Schlaf-Paar [7]/[8]** (decide = `jr ra` = *kein*
Wake) auf das **Wach-Paar [9]/[10]** (decide `0x801039fc` = `+0x6 = 2`, d.h. sofort
aufstehen). **Der Port dispatcht die Nibbles 9/10 gar nicht** (`switch (e->grid_id & 0xf)`
in `enemy_ai_common.c` hat nur 0/2/5/6/7/8 → `default: break`), und selbst mit Dispatch
würde der Aufsteh-Clip nicht laufen, weil der globale Anim-Advancer die Clips 0x12/0x13
hart auf Frame 0 pinnt. Ergebnis im Port: nach dem Trigger sind die 5 Zombies **komplett
eingefroren UND dauerhaft unverwundbar** (`+0x93` Bit 0 bleibt gesetzt).

SCD-/AOT-Seite des Ports ist **korrekt** (gemessen: AOT feuert, Flag wird gesetzt, Grids
werden auf 8A/8A/89/8A/89 umgeschrieben) — der Bug sitzt **ausschließlich** in der
Gegner-KI + im Anim-Advancer.

Dynamisch reproduziert + Fix-Simulation: `re15_port/tests/unit/probe_room1070_wake.c`
(Diagnose-Probe, kein ctest; Logs unten wörtlich).

---

## 1. MESSEN — RDT-Spawns + Trigger + Port-Ist

### 1.1 ROOM1070.RDT — Spawns und Trigger (Datei-Byte-Offsets, SCD-Walker)

`sub_scd`-Basis = RDT+0x44 → **0x15A8**; 3 Subs: sub00 @0x15AE, sub01 @0x169A, sub02 @0x169C.

**`sub_scd[0]` (Raum-Skript):**

| Datei-Offset | Bytes | Bedeutung |
|---|---|---|
| 0x15AE | `06 00 80 00` | `If` (Block 0x80 Bytes) |
| 0x15B2 | `21 04 C5 00` | `Ck(zone=4, bit=0xC5=197, ==0)` — Erstbesuch |
| 0x15B6 | `2C 0E 03 41 00 00 10 27 18 FC 78 05 08 52 FF 00 18 02 00 00` | `Aot_set` **slot 14, sce=3, flags=0x41, band=0**, rect x=10000 z=−1000 w=1400 d=21000, **eventId = pc[17] = 0x02** |
| 0x15CA | `44 00 10 88 …` | `Sce_em_set` slot0 type=0x10 **beh=0x88** kill=0xC6 pos=(13444,0,4327) dir=500 |
| 0x15DE | `44 01 10 88 …` | slot1 type=0x10 **beh=0x88** kill=0xC7 pos=(13594,0,9374) dir=2767 |
| 0x15F2 | `44 02 10 87 …` | slot2 type=0x10 **beh=0x87** kill=0xC8 pos=(18934,0,7124) dir=1767 |
| 0x1606 | `44 03 11 88 …` | slot3 type=0x11 **beh=0x88** kill=0xC9 pos=(19944,0,9824) dir=3767 |
| 0x161A | `44 04 11 87 …` | slot4 type=0x11 **beh=0x87** kill=0xCA pos=(844,0,14124) dir=1791 |
| 0x162E | `07 00 6A 00` | `Else` |
| 0x1632…0x1682 | `44 xx 1x 00 …` | dieselben 5 Spawns mit **beh=0x00** (Wiederbetreten: schon wach) |

**`sub_scd[2]` (= das AOT-Event, 0x169C…0x16D2) — DER WAKE:**

| Datei-Offset | Bytes | Bedeutung |
|---|---|---|
| 0x169C | `22 04 C5 01` | `Set(4,197,1)` — Trigger verbraucht |
| 0x16A0 | `46 0E 00 …` | `Aot_reset(14)` — Zone sofort tot (erste, yield-freie Opcode-Gruppe) |
| 0x16AA/0x16AE | `2E 02 00` / `34 0C 8A 00` | `Work_set(2,0)` + **`Member_set(12, 0x8A)`** (slot0: 0x88→0x8A) |
| 0x16B2/0x16B6 | `2E 02 01` / `34 0C 8A 00` | slot1: 0x88→**0x8A** |
| 0x16BA/0x16BE | `2E 02 02` / `34 0C 89 00` | slot2: 0x87→**0x89** |
| 0x16C2/0x16C6 | `2E 02 03` / `34 0C 8A 00` | slot3: 0x88→**0x8A** |
| 0x16CA/0x16CE | `2E 02 04` / `34 0C 89 00` | slot4: 0x87→**0x89** |

→ Die Abbildung ist **exakt Nibble +2**: 0x88→0x8A, 0x87→0x89, slotweise passend zum
jeweiligen Spawn-Behavior.

### 1.2 Port-Messung (`probe_room1070_wake.exe`, wörtlich gekürzt)

```
EM010 bank: 43 clips  fc[0x12]=98 fc[0x13]=120
EM011 bank: 43 clips  fc[0x12]=98 fc[0x13]=120
-- H1: Spawn-Roster nach sub00 --
   slot=1 type=0x10 grid=0x88 st=0 s1=0x00 s2=0 mo=19  fr=0   pos=(13444,4327)
   slot=2 type=0x10 grid=0x88 st=0 s1=0x00 s2=0 mo=19  fr=0   pos=(13594,9374)
   slot=3 type=0x10 grid=0x87 st=0 s1=0x00 s2=0 mo=18  fr=0   pos=(18934,7124)
   slot=4 type=0x11 grid=0x88 st=0 s1=0x00 s2=0 mo=19  fr=0   pos=(19944,9824)
   slot=5 type=0x11 grid=0x87 st=0 s1=0x00 s2=0 mo=18  fr=0   pos=(844,14124)
   aot=14 type=6(AUTO_EVENT) ev=2 band=0x00 flags=0x41 c=(10700,9500) half=(700,10500)
== Phase B: Spieler betritt AOT 14 (x=10700, z=5000) ==
H2 grid-Rewrite durch sub_scd[2]: JA (Tick 1), Flag(4,197)=1
   slot=1 grid=0x8A | slot=2 grid=0x8A | slot=3 grid=0x89 | slot=4 grid=0x8A | slot=5 grid=0x89
   aot14 danach: type=11 (NONE) -> Zone tot, feuert genau EINMAL
== Phase C: 600 Ticks nach dem Rewrite ==
  slot1 grid 0x88->0x8A  Wake(+0x6>=2): NIE  Clip laeuft(fr>0): NIE  ENGAGE: NIE   (hitr=0x01)
  slot2 grid 0x88->0x8A  Wake: NIE  Clip: NIE  ENGAGE: NIE                         (hitr=0x01)
  slot3 grid 0x87->0x89  Wake: NIE  Clip: NIE  ENGAGE: NIE                         (hitr=0x01)
  slot4 grid 0x88->0x8A  Wake: NIE  Clip: NIE  ENGAGE: NIE                         (hitr=0x01)
  slot5 grid 0x87->0x89  Wake: NIE  Clip: NIE  ENGAGE: NIE                         (hitr=0x01)
== Phase D: H4-Isolation — grid=0x88, +0x6=2 (Phase 2) von Hand ==
   t=  0 … t= 90  grid=0x88 s2=2 mo=19 fr=0 clip_len=120
H4: nach 120 Ticks s2=2 fr=0 -> CLIP-PIN BESTAETIGT (Frame haengt auf 0)
```

Das ist exakt das Nutzer-Symptom: der Trigger *feuert* (Skript-Seite byte-true), aber die
Zombies bleiben in ihrer Liege-/Anlehn-Pose stehen — und sind zusätzlich unverwundbar
(`hit_react=0x01`, `re15_damage.c:761` blockt jeden Treffer).

---

## 2. ORIGINAL — die komplette Wake-Kette (Disasm-Belege)

Alle Overlay-Adressen aus `info/Re1.5/PSX/BIN/STAGE1.BIN` (lädt @0x80100000, **kein**
0x800-Header → Datei-Offset = addr − 0x80100000); EXE-Adressen aus `info/Re1.5/PSX.EXE`
(Datei-Offset = 0x800 + addr − 0x80010000).

### 2.1 `Member_set(12, …)` schreibt entity `+0x9` — der Linchpin (@0x800411f4)

`FUN_8004116c` (`sltiu v0,a1,0x14`, Sprungtabelle **@0x80010c8c**, 20 Einträge):

```
8004116c: sltiu v0,a1,0x14        ; id < 20 ?
8004117c: addiu at,at,3212        ; Tabelle 0x80010c8c
8004118c: jr    v0
...
80010cbc: -> 0x800411f4           ; Eintrag [12]
800411f4: j 0x80041230
800411f8: sb a2,9(a0)             ; ENTITY +0x9 = value   <-- Member 12 == das Behavior/Grid-Byte
```

→ `Member_set(12, 0x8A)` ist **identisch** zu „Sce_em_set-behavior = 0x8A".

### 2.2 AOT sce=3 = „führe sub aus" (@0x800430f0, Handler-Tabelle @0x8007469c[3])

```
800430f0: lhu a0,0(v0)     ; payload u16@0  (= pc[14..15] = 0x00FF)
800430fc/80043100: lbu a1,3(v0)  ; payload+3  = pc[17]  = 0x02   <-- die SUB-Nummer
80043104: jal 0x8003ee3c   ; FUN_8003ee3c(cond, sub) = Sub starten
```

Flags `pc[3] = 0x41`: Bit 0x10 **CLEAR** → AUTO-Scan (`FUN_800436a8`, param_3=0, kein
Tastendruck), Bit 0x40 = CENTRE-Test, Bit 0x01 = Player-Pool. → **Durchlaufen genügt.**

### 2.3 Der Sub-Modus-Dispatch `@0x8011f80c[+0x9 & 0xf]` (Tabellen-Dump STAGE1.BIN)

```
[0]=0x8010168c  [1]=0x80101708 [2]=0x80101784 [3]=0x80101800 [4]=0x8010187c
[5]=0x801018f8  [6]=0x801018f8            <- FEEDING-Paar
[7]=0x80101974  [8]=0x80101974            <- SCHLAF-Paar   (behavior 0x87/0x88)
[9]=0x801019f0  [10]=0x801019f0           <- WACH-Paar     (behavior 0x89/0x8A)
[11]=0x80101a6c [12]=0x80101ae8 [13]=0x80101b64 …
```

Jeder dieser Handler ist ein **Doppel-Dispatcher** `decide_row[+0x5]()` dann
`animate_row[+0x5]()` mit unterschiedlichen Tabellen-Basen (verschobene Fenster derselben
Funktionsliste **@0x8011f9c8**):

```
0x801018f8 (Nibble 5/6):  decide @0x8011f9d0[+0x5]   animate @0x8011f9d4[+0x5]
0x80101974 (Nibble 7/8):  decide @0x8011f9d8[+0x5]   animate @0x8011f9d4[+0x5]
0x801019f0 (Nibble 9/10): decide @0x8011f9dc[+0x5]   animate @0x8011f9d4[+0x5]
```
(Belege: `addiu at,at,-1584/-1576/-1572/-1580` @0x80101918/0x80101948, @0x80101994/
0x801019c8, @0x80101a10/0x80101a44 — lui 0x8012 ⇒ 0x8011F9D0/D8/DC/D4.)

Funktionsliste **@0x8011f9c8** (Dump):
```
0x8011f9c8 -> 0x80103980   0x8011f9cc -> 0x80103a58
0x8011f9d0 -> 0x80103980   0x8011f9d4 -> 0x80103a58
0x8011f9d8 -> 0x801039f4   0x8011f9dc -> 0x801039fc
0x8011f9e0 -> 0x80109e44   0x8011f9e4 -> 0x80109e4c
0x8011f9e8 -> 0x80104b38   0x8011f9ec -> 0x80104b40
```

⇒ Für `+0x5 == 0` (der einzige Zustand, den diese Sub-Modi je haben) gilt:

| Nibble | behavior | decide | animate |
|---|---|---|---|
| 5/6 | 0x85/0x86 | **0x80103980** = Näherungs-Wake `dist<0xFA0` | 0x80103a58 |
| 7/8 | 0x87/0x88 | **0x801039f4** = `jr ra` (**KEIN** Auto-Wake) | 0x80103a58 |
| 9/10 | 0x89/0x8A | **0x801039fc** = Sofort-Wake | 0x80103a58 |

### 2.4 Der Wach-Decide `FUN_801039fc` (Nibble 9/10) — der eigentliche Trigger

```
801039fc: lui v1,0x800b ; lw v1,-14460(v1)   ; v1 = aktuelle Entity
80103a0c: lbu v0,6(v1)                        ; +0x6 (Phase)
80103a14: bne v0,zero,0x80103a48              ; nur wenn +0x6 == 0
80103a18: ori v0,zero,0x2                     ; (Delay-Slot)
80103a1c: sb  v0,6(v1)                        ; +0x6 = 2      <-- WAKE (ueberspringt Phase 1!)
80103a28: ori v0,zero,0xf
80103a2c: jal 0x8001af20                      ; rng
80103a30: sb  v0,143(v1)                      ; Delay-Slot: +0x8f = 0x0F (Crossfade)
80103a34: andi v0,v0,0x3
80103a38: bne v0,zero,0x80103a48
80103a40: jal 0x800453d0                      ; room-SE
80103a44: ori a0,zero,0x5                     ; SE 5 (Aechzen), Chance 1/4
```

### 2.5 Zum Vergleich der Feeding-Decide `FUN_80103980` (Nibble 5/6)

```
80103990: lw    v0,464(v1)      ; +0x1d0 Dist-Cache
80103998: sltiu v0,v0,0xfa0     ; dist < 4000 ?
801039a4: lbu   v0,6(v1)  ; bne != 0 -> ret
801039b4/b8: jal rng ; sb 1,6(v1)     ; +0x6 = 1  (Countdown-Phase)
801039c8/cc: jal rng ; sh (rng&0xf),156(v1)  ; +0x9c = rng & 0xF
801039dc/e0: jal 0x800453d0 ; a0=5    ; 1/4 SE 5
```

**Damit ist die offene Frage #2 aus `analysis/zombie_hit_1140.md` §5 beantwortet:**
`+0x6=1` und `+0x9c` setzt **der Feeding-Decide 0x80103980** (Näherung < 0xFA0); die
Nibble-7/8-Lyer haben *überhaupt keinen* Auto-Wake — sie werden **nur** per Skript-
Nibble-Bump geweckt.

### 2.6 Die geteilte Phasen-Maschine `FUN_80103a58` (animate, Switch über `+0x6`)

```
case 0 @0x80103aac: +0x93 |= 1 (unverwundbar, JEDEN Tick), +0x1b8 = 1     -> passiv liegen
case 1 @0x80103ad0: lhu +0x9c ; sh (t-1) ; wenn t war 0: +0x6 = 2, +0x8f = 0xF
case 2 @0x80103b08: a2=0 ; a0=+0x170 ; a1=+0x174 ; jal 0x8001f314 ; a3=0x100
                    +0x6 += ret          ; AUFSTEH-CLIP laeuft (+0x94 wird NICHT gesetzt!)
case 3 @0x80103b3c: sb zero,9(a1)        ; +0x9 = 0   (Nibble weg -> Standard-Combat)
                    sw 0x201,4(v1)       ; word +0x4 = 0x201 -> State 1 / +0x5 = 2 ENGAGE
                    +0x93 &= 0xfe        ; wieder verwundbar
                    sb zero,440(v0)      ; +0x1b8 = 0
```

**Wichtig:** Phase 2 schreibt **kein** `+0x94` — gespielt wird der beim Spawn gesetzte
Pose-Clip. Der INIT-Decoder (`FUN_80100688`, Pose-Block gated auf `+0x9 & 0x80`
@0x80100cac-b0) setzt:

```
80100cbc-e0: sel 4/7/9  -> +0x94 = 0x0C
80100cfc-24: sel 5/8/0xA-> +0x94 = 0x0E
80100d78/88: +0x95 = 0 ; +0x8f = 0
80100da0:    jal 0x8001f314 (a2=0, a3=0x200)
80100dc0-e8: sel 4/7/9  -> +0x94 = 0x12     <-- Endstand behavior 0x87
80100e04-28: sel 5/8/0xA-> +0x94 = 0x13     <-- Endstand behavior 0x88
```
Kein `+0x5`-Write, kein `+0x9`-Clear → Nibble 7/8 bleibt stehen (der Port macht das schon
richtig, `enemy_ai_common.c` INIT-Kommentar „sel 8 … pose-only").

EM010/EM011-Bank (gemessen): **Clip 0x12 = 98 Frames, Clip 0x13 = 120 Frames** = die
Aufsteh-Animationen; Frame 0 ist die Liege-/Anlehn-Pose.

### 2.7 Sequenz im Original (ROOM1070, Erstbesuch)

| Phase | grid | +0x6 | Clip | sichtbar |
|---|---|---|---|---|
| Spawn/Schlaf | 0x88 / 0x87 | 0 | 0x13 / 0x12 **auf Frame 0 gehalten** | liegt/lehnt, unverwundbar |
| Spieler betritt AOT-14-Streifen | → **0x8A / 0x89** | 0 | — | (Skript, 1 Frame) |
| Wake (nächster Tick) | 0x8A / 0x89 | 0→2 | +0x8f=0xF, 1/4 SE 5 | Aufsteh-Clip startet |
| Aufstehen | 0x8A / 0x89 | 2 | 0x13 (120f) / 0x12 (98f) durchspielen | richtet sich auf |
| Fertig | **0x00** | — | word 0x201 | normaler ENGAGE-Zombie, verwundbar |

### 2.8 Spielweite Reichweite des Mechanismus (RDT-Zensus, 240 RDTs, SCD-gegatet)

`Sce_em_set` behavior **0x87** (20×) / **0x88** (33×) und `Member_set(12, 0x89)` (20×) /
`Member_set(12, 0x8A)` (26×):

| Raum | 0x88 / 0x87 Spawns | Wake-Sub mit `Member_set(12,0x8A/0x89)` |
|---|---|---|
| ROOM1020 / 1021 | 3 / 1 | sub03 bzw. sub07 — 3×0x8A + 1×0x89 ✔ |
| **ROOM1070 / 1071** | **3 / 2** | **sub02 — 3×0x8A + 2×0x89 ✔** |
| ROOM1100 / 1101 | 3 / 2 | sub02 — 3×0x8A + 2×0x89 ✔ |
| ROOM11F0 / 11F1 | 2 / 2 | sub18 — 2×0x8A + 2×0x89 ✔ |
| ROOM1200 / 1201 | 1 / 2 | sub03 — 1×0x8A + 2×0x89 ✔ |
| ROOM3010 / 3011 | 1 / 1 | sub01 — 1×0x8A + 1×0x89 ✔ |
| ROOM10E0/10E1, 1110/1111, 1140 | 0x88, **kein** Member-Write | → OFFEN, s. §5.1 |

⇒ Der Fix repariert **12 Räume** (6 Paare, STAGE1 + STAGE3), nicht nur 1070.
**Kein einziger** `Sce_em_set` im ganzen Spiel benutzt ein behavior mit Low-Nibble 9 oder
0xA (Histogramm über alle 240 RDTs) — Nibble 9/10 entsteht **ausschließlich** durch diesen
Skript-Bump.

---

## 3. PORT-IST — zwei Defekte (statisch + dynamisch belegt)

### D1 (Ursache) — Grid-Nibble 9/10 wird nicht dispatcht (`enemy_ai_common.c:2508 ff.`)

`re15_enemy_ai_live_active` hat `switch (e->grid_id & 0xf)` mit `case 0`, `case 2`,
`case 5/6` (:2588, feeding), `case 7/8` (:2591, Lyer-Phasenmaschine) und **`default: break`** (:2633, Kommentar „`[1..4],[9..15]` … deferred —
für **9 und 10 gibt es nichts**. Nach dem Skript-Bump landet jeder der 5 Zombies für
immer im `default`. Probe Phase C: 600 Ticks, `+0x6` bleibt 0, kein Clip, kein ENGAGE.

**Nebenwirkung (2. Symptom):** Der Port setzt in `case 7/8` Phase 0 byte-true
`e->hit_react |= 1` (@0x80103aac). Nach dem Bump wird der Bit nie mehr gelöscht (das täte
erst Phase 3 @0x80103b64), also bleiben die Zombies **unverwundbar** —
`re15_enemy_take_damage` (`re15_damage.c:761`) verwirft jeden Treffer. Gemessen:
`hitr=0x01` nach 600 Ticks.

Der Port ist an der Stelle **ehrlich dokumentiert** („Rows [5]/[6]=0x80104b38/40 … remain
OPEN; die phase-0->1 wake TRIGGER … ist OPEN"), aber die Zeile darüber
(„`[9..12]=…`") wurde nie aufgelöst.

### D2 (Folge) — der globale Advancer pinnt den Aufsteh-Clip auf Frame 0 (`player_common.c:725-726`)

```c
int getup = (a->sub_state_1 == 0x11 && a->sub_state_2 >= 4);
if ((mo == 0x0C || mo == 0x0E || mo == 0x12 || mo == 0x13) && !getup) { a->anim_frame = 0; continue; }
```
Der Lyer weckt mit `sub_state_1 == 0` (nicht 0x11), also greift `getup` nicht → `+0x95`
bleibt 0 → `re15_enemy_clip_done` (`enemy_ai_common.c:3031-3047`, `anim_frame >= fc-1`)
liefert nie 1 → Phase 2 hängt ewig. **Isoliert gemessen** (Probe Phase D: grid künstlich
0x88 + `+0x6=2` gesetzt): `fr=0` über 120 Ticks bei `clip_len=120`.

Byte-true-Begründung für die Freigabe: Im Original läuft der Frame **nur**, wenn ein
State-Handler `FUN_8001f314` ruft. In `FUN_80103a58` gibt es genau **einen** solchen Call
— in Phase 2 (@0x80103b14). Phasen 0/1 rufen ihn nicht (= der Pin ist dort byte-true
korrekt).

### D3 (KEIN Defekt — geprüft) — SCD/AOT-Seite ist byte-true

AOT 14 wird als `AUTO_EVENT` (`flags & 0x10 == 0`, `scd_vm.c` op_aot_set) mit
`ev = pc[17] = 2` installiert, feuert beim Betreten (`fire = inside`, `aot_common.c`),
`game_step_common.c:781` startet `sub_scd[2]`, `op_work_set(2,N)` bindet Actor-Slot N+1
(`SCRIPT_SLOT_TO_ACTOR`), `op_member_set` id 12 → `actor_common.c:153 case 12: grid_id`.
`Aot_reset(14)` setzt den Slot auf `TYPE_NONE` (gemessen `type=11`) → **genau ein** Fire.
Gemessen: Tick 1, Flag(4,197)=1, Grids 8A/8A/89/8A/89.

---

## 4. FIX-PLAN (mit `@0x`-Zitaten; in dieser Diagnose wurde KEIN Engine-Code geändert)

### F1 — Nibble 9/10 dispatchen (`enemy_ai_common.c`, im `switch (e->grid_id & 0xf)`)

Die Phasenmaschine ist bereits als `case 7: case 8:` vorhanden — 9/10 teilen sie sich
(`animate @0x8011f9d4[0] = 0x80103a58` in BEIDEN Fällen); der einzige Unterschied ist der
**decide**:

```c
case 9: case 10:   /* @0x8011f80c[9]/[10] = 0x801019f0: decide-Row @0x8011f9dc[0] =
                    * FUN_801039fc (SKRIPT-WAKE, byte-true), animate-Row @0x8011f9d4[0] =
                    * FUN_80103a58 (dieselbe +0x6-Maschine wie Nibble 7/8).
                    * Erzeugt wird der Nibble AUSSCHLIESSLICH per Member_set(12, 0x89/0x8A)
                    * (= entity +0x9, FUN_8004116c Tabelle @0x80010c8c[12] -> sb a2,9(a0)
                    * @0x800411f8) aus dem AOT-sce3-Sub eines Raums — 12 Raeume, s. Dossier
                    * §2.8. Kein Sce_em_set im Spiel spawnt mit Nibble 9/0xA. */
    if (e->sub_state_1 == 0) {
        if (e->sub_state_2 == 0) {              /* decide FUN_801039fc @0x80103a0c-44 */
            e->sub_state_2 = 2;                 /* sb 2,6(v1)  @0x80103a1c  (Phase 1 uebersprungen) */
            e->anim_frac   = 0x0f;              /* sb 0xf,143(v1) @0x80103a30 (Delay-Slot, v0=0xf) */
            if ((re15_engine_rand8() & 3) == 0) /* andi 3; bne  @0x80103a34-38 */
                re15_audio_room_se(5);          /* jal 0x800453d0, a0=5 @0x80103a40-44 */
        }
        /* … danach EXAKT der bestehende case-7/8-Rumpf (FUN_80103a58, +0x6-Phasen 0..3) … */
    }
    break;
```

Um Duplikat-Code zu vermeiden: den vorhandenen `case 7/8`-Rumpf in eine statische
Funktion `re15_enemy_ai_lyer_phase(re15_actor_t*)` ziehen und aus 7/8 sowie 9/10 aufrufen;
9/10 ruft davor den Wake-Decide. **Reihenfolge im selben Tick beibehalten** (erst decide,
dann animate — `0x801019f0` ruft beide hintereinander, @0x80101a20 / @0x80101a54).

### F2 — Anim-Pin für die Aufsteh-Phase freigeben (`player_common.c:725-726`)

```c
/* LYER-RISE (byte-true FUN_80103a58): der einzige f314-Call der Liege-Maschine steht in
 * Phase 2 (@0x80103b14, a3=0x100) — Phase 0/1 rufen ihn NICHT (@0x80103aac/0x80103ad0),
 * dort ist der Pin korrekt. Ab +0x6 >= 2 MUSS der Spawn-Pose-Clip 0x12/0x13 laufen, sonst
 * erreicht re15_enemy_clip_done nie 1 und Phase 2 haengt (Dossier D2). Sub-Modi 7/8/9/10
 * (@0x8011f80c) teilen sich diese Maschine; +0x5 ist dort immer 0. */
int lyer_rise = (a->state == 1 && a->sub_state_1 == 0 && a->sub_state_2 >= 2 &&
                 ((a->grid_id & 0x0f) >= 7 && (a->grid_id & 0x0f) <= 10));
if ((mo == 0x0C || mo == 0x0E || mo == 0x12 || mo == 0x13) && !getup && !lyer_rise) {
    a->anim_frame = 0; continue;
}
```

### F3 (optional, Konvention) — `anim_blend_rate = 0x100` in Phase 2 setzen (f314 a3
@0x80103b18). Der Port-Advancer wertet die Rate nicht aus (port-weite bestehende
Konvention, s. §5.3) — rein dokumentarisch.

### Verifikation (bereits vorab simuliert)

Probe Phase E baut den Fix ohne Engine-Änderung nach (Nibble-9/10-Maschine + Pin-Freigabe
per Schatten-Frame) — Ergebnis wörtlich:

```
SIM Tick   5: grid=0x8A s2=2 mo=19 fr=4      (Wake sofort nach dem Bump)
SIM Tick  60: grid=0x8A s2=2 mo=19 fr=59     (Aufsteh-Clip laeuft)
SIM Tick 130: grid=0x00 s1=0x02 mo=4         (ENGAGE, normale Lauf-Clips)
  SIM slot1/2/4 (Clip 0x13, 120f) ENGAGE nach 121 Ticks
  SIM slot3/5   (Clip 0x12,  98f) ENGAGE nach  99 Ticks
```

Nach dem echten Fix: dieselbe Probe erwartet `Wake JA / Clip laeuft JA / ENGAGE JA` und
`hitr` zurück auf 0x00 — und danach Live-Verifikation per gdigrab (Skill
`re15-port-visual-verify`) im echten Flow (`RE15_DEBUG_JUMP="1070@gp"` + `RE15_INPUT_SCRIPT`,
über den Streifen x∈[10000,11400] laufen).

---

## 5. OFFEN (ehrlich)

1. **ROOM10E0/10E1, ROOM1110/1111, ROOM1140/1141 spawnen behavior 0x88 OHNE jeden
   Member-Write** (Scan über Opcodes 0x34/0x35/0x3E/0x5A/0x5B in diesen RDTs: 0 Treffer
   auf Member 12 außer 1141, das nur 0x10/0x30 schreibt). Ihr Wake-Trigger ist damit
   **nicht** dieser Mechanismus — Kandidaten: Schaden (`+0x93`-Pfad), ein Overlay-seitiger
   Sonderfall des Raums, oder sie sind byte-true dauerhaft passiv. Nächster Schritt:
   DuckStation-Savestate in ROOM1140 vor/nach dem Cutscene-Ende und `+0x9`/`+0x6` diffen.
2. **Decide/Animate-Rows [1..6]** der geteilten Liste @0x8011f9c8 (0x80109e44/0x80109e4c =
   Death-Paar, 0x80104b38/0x80104b40 = „Lying get-up pair") sind für diese Sub-Modi nur
   erreichbar, wenn `+0x5 != 0` wird. In den Phasen 0–3 passiert das nie (Phase 3 setzt
   `+0x5=2` und `+0x9=0` im selben Tick). Ob ein TREFFER während des Aufstehens `+0x5`
   umbiegt (und dann in diese Rows läuft), ist NICHT gemessen. Der Fix übernimmt die
   bestehende Port-Konvention (`if (e->sub_state_1 == 0)`), also identisch zu 7/8.
3. **`+0x1b8`** (Phase 0 = 1, Phase 3 = 0) hat im Port weiterhin keinen Konsumenten —
   dokumentierte Lücke, identisch zu 1140/10D0.
4. **f314-Rate `a3=0x100`** wird vom Port-Advancer nicht modelliert (flach 1 Frame/Tick) —
   port-weite bestehende Konvention, nicht Teil dieses Bugs.
5. **SE 5 / `+0x8f=0xF`** sind belegt (@0x80103a30/0x80103a40), aber der akustische
   1/4-Roll ist im Port RNG-deterministisch (Memory `reai-v2-rng-determinism`) — Hör-
   Verifikation offen.
6. **ROOM1071/1021/1101/11F1/1201/3011** (Alternativ-Spieler-Varianten) sind nicht
   dynamisch geprobt — identische Bytes, identischer Pfad.

## 6. Artefakte

- Probe: `re15_port/tests/unit/probe_room1070_wake.c` (+ CMake-Eintrag, **kein** ctest) —
  Phasen A–E inkl. Fix-Simulation.
- Disasm: `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` gegen STAGE1.BIN
  (0x80101974/0x801019f0/0x80103980/0x801039f4/0x801039fc/0x80103a58/0x80100c20, Tabellen
  @0x8011f80c/@0x8011f9c8) und PSX.EXE (0x800430f0, 0x8004116c, Tabelle @0x80010c8c).
- SCD-Walker + spielweiter RDT-Zensus (Session-Skripte im Scratchpad; Ergebnis §1.1/§2.8).
- ctest-Baseline vor der Diagnose: **110/110 grün**.

---

## STATUS 2026-08-06 — BEHOBEN

### Eingebaut (beide Blocker, der Pruefer hat den Fix vorab selbst gebaut und getestet)

**R1070-1** — `enemy_ai_common.c`: die case-7/8-Phasenmaschine (FUN_80103a58) in
`re15_enemy_ai_lying_phase()` gezogen (reine Code-Bewegung) und `case 9: case 10:` ergaenzt, die
davor den Sofort-Wake-Decide `FUN_801039fc` ausfuehrt: `+0x6 = 2` (@0x80103a1c),
`+0x8f = 0xf` (@0x80103a28-30), 1/4-Wurf auf SE 5 (@0x80103a34-44). Reihenfolge
decide-dann-animate im selben Tick (@0x80101a20 / @0x80101a54).

**R1070-2** — `player_common.c`: der globale Anim-Advancer gibt den Pin auf Clip 0x12/0x13 fuer
die Aufsteh-Phase frei. Gate so eng wie moeglich: Zombie-Typen (0x10/0x11/0x12/0x16/0x18 — der
vom Pruefer angemahnte Typ-Check ist drin), `state==1`, `sub_state_1==0`, `sub_state_2>=2`,
Nibble 7..10. Belegt durch den einzigen f314-Call der Funktion @0x80103b14 (a3=0x100); Phase 0
(@0x80103aac-cc) und Phase 1 (@0x80103ad0-b04) rufen ihn nicht, der Pin ist dort also byte-true
korrekt und NUR ab Phase 2 falsch.

**R1070-3** faellt damit weg (Phase 3 @0x80103b5c-68 loescht `+0x93 & 1`), **R1070-4** ist als
Kommentar hinterlegt.

### Verifikation

`probe_room1070_wake` ist jetzt ctest `unit_room1070_wake` und prueft pro Zombie: aufgewacht,
Clip wirklich abgespielt, ENGAGE erreicht, Nibble auf 0, `hit_react` Bit 0 geloescht.

| | vorher | nachher |
|---|---|---|
| Wake nach dem Skript-Bump | NIE (600 Ticks default:break) | JA, Tick 0 |
| Aufsteh-Clip | NIE (Frame-0-Pin) | laeuft |
| ENGAGE | NIE | 116 Ticks (Clip 0x13) / 94 (0x12) |
| beschiessbar | nein, dauerhaft `hitr=0x01` | ja, `hitr=0x00` |

Voller `ctest`: 113/113 gruen; `probe_zlyer_10d0`, `probe_zreentry_10d0`, `probe_zombie_hit`,
`probe_marvin_spawn` unveraendert.

**Reichweite (R1070-5):** derselbe Mechanismus steckt in 12 Raeumen (1020/1021, 1070/1071,
1100/1101, 11F0/11F1, 1200/1201, 3010/3011) — die sind damit ebenfalls repariert, dynamisch
gemessen wurde nur ROOM1070.
