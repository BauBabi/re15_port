# FINDING 2 — „Die Finisher-Animation, wenn man von Krähen getötet wird, stimmt noch nicht"

**Datum:** 2026-08-24 · **Status:** Original-Seite **byte-belegt** (eigener Roh-MIPS-Disasm dieser
Session: `info/Re1.5/PSX/BIN/STAGE1.BIN` off = addr−0x80100000, `info/Re1.5/PSX.EXE` off = 0x800+addr−t_addr,
via `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`), Port-Seite **dynamisch gemessen**
(neue Sonde `re15_port/tests/unit/probe_crow_kill.c`, voller `re15_game_step`, ROOM1170 Re-Entry).
**KEINE Engine-Änderung in dieser Phase.** Einzige Repo-Änderung: die Sonde + ihre CMake-Zeile
(kein ctest); `bash re15_port/tools/local_build.sh` = **224/224 grün**.

**Schwester-Dossiers:** `analysis/crow_victim_anim.md` (Hook A/B, Opfer-FSM), `analysis/crow_death_pool.md`
(tote Krähe), `analysis/konstruktion_2026-08-23/crow-throw.md` (cmd-4-Wurf), `analysis/player_hit_chain.md`.

---

## 0. Executive Answer

**Es gibt keinen krähen-EIGENEN Finisher. Die Krähe tötet über das GENERISCHE Spieler-Todes-Kommando
cmd 3, und das spielt `PL00.EDD` **Clip 7** (113 Frames, Sturz aus dem Stand zu Boden) aus der
Spieler-BASIS-Bank — nicht aus einer Krähen-Bank.** Der Port spielt **überhaupt keinen Todes-Clip**:
gemessen bleibt Leon im Idle-Sentinel `motion=200` stehen, während Blutlache, Todes-Kamera und
„YOU DIED" über ihm ablaufen. Beim Grab-Tod spielt der Port zusätzlich **fälschlich den
Krähen-Release-Clip 2**, den das Original an dieser Stelle nie erreicht (cmd 3 ersetzt cmd 5 sofort).

Damit ist die frühere Beobachtung aus `crow_victim_anim.md` §0.2 („Hook B [0x21] = `jr ra`-Stub,
also KEIN Devour-/Kill-Pfad") **richtig und trotzdem kein Widerspruch**: der Kill läuft nicht über
Hook B (cmd 6), sondern über cmd 3. Das ist der Weg, den die Aufgabenstellung gesucht hat.

---

## 1. WER setzt den Todes-Zustand — vollständiger Zensus (nicht nur die Krähe)

Eigener Byte-Scan über `STAGE1.BIN` (alle `sb/sh/sw` mit imm `0xca58`, d.h. jeder Schreiber des
Spieler-Kommando-Registers `0x800aca58`), Wert per Rücklauf auf das ladende `ori/addiu rt,zero,imm`:

| Adresse | Instr. | Wert | Bedeutung |
|---|---|---|---|
| 0x80113b00 | `sb v1,-13736(at)` | **2** | Dive-Connect → Standard-Flinch |
| **0x80113b48** | `sb v0,-13736(at)` | **3** | **Dive lethal → TOD** |
| 0x80113e48 | `sb v0,…` | 5 | Front-Grab → Opfer-FSM |
| **0x80113f20** | `sb v0,…` | **3** | **Grab lethal → TOD** |
| **0x80114518** | `sb v0,…` | **3** | **Strike lethal → TOD** |
| 0x801152a0 | `sb v0,…` | 5 | Rück-Peck-Grab (nur ROOM1171-Szene) |
| 0x80115b1c / 0x80115b40 | `sb`/`sw` | 1 | Opfer-FSM-Exit (Kontrolle zurück) |
| 0x80115d04 | `sb v0,…` | 4 | Wurf → Plc-Mode 6 |

Im gesamten Krähen-Bereich `0x80111a4c–0x80116400` kommt **kein Wert 6 und kein Wert 7** vor.
(Zum Kontrast im selben Scan: der HUND schreibt 6 @0x80110138 und 7 @0x80111ea0 — der hat einen
echten eigenen Finisher, die Krähe nicht.) Damit ist unabhängig re-bestätigt: **Hook B [0x21]
(`0x80115d2c`, Tabelle [0] = `jr ra`-Stub @0x80115d6c) ist unerreichbarer toter Code.**

### 1.1 Die drei Kill-Sites — identisches Byte-Muster

Grab-Kill (Halte-Step `move[13]`, roh):
```
80113f00: lh   v0,-13586(v0)        ; player.hp (+0x9a)
80113f08: bgez v0,0x80113f54        ; hp >= 0 -> kein Tod
80113f18: ori  v0,zero,0x3          ;  03 00 02 34
80113f20: sb   v0,-13736(at)        ;  58 ca 22 a0   -> 0x800aca58 := 3   (cmd 3 TOD)
80113f14/30: aca50 = (aca50 & 0xfff) | 0x2000        ; Flock-KILL-Broadcast
80113f44: sb   zero,-13735(at)      ; 0x800aca59 := 0  (Richtung 0)
80113f4c: sh   zero,-13734(at)      ; 0x800aca5a := 0  (Phase 0, +aca5b)
80113f50: sb   v0(=1),472(v1)       ; Krähe +0x1d8 := 1 (Self-Exempt)
```
Dive-Kill @0x80113b34-78 und Strike-Kill @0x80114504-3c sind **instruktionsgleich**
(`03 00 02 34` / `58 ca 22 a0` an 0x80113b44-48 bzw. 0x80114514-18; aca59:=0 @0x80113b6c /
@0x8011453c; aca5a:=0 @0x80113b74).

**Wichtig für die Reihenfolge:** der Dive schreibt in DERSELBEN Funktion zuerst cmd 2
(@0x80113b00) und überschreibt es bei `hp<0` mit cmd 3 (@0x80113b48) — **beim tödlichen Dive gibt
es also KEINEN Flinch, sondern sofort die Todes-Animation.** Beim Grab ersetzt cmd 3 das laufende
cmd 5: **die Krähen-Opfer-FSM (Clips 0/1/2 aus Paar C) bricht sofort ab, der Release-Clip 2 wird
NIE gespielt.**

---

## 2. WELCHE Animation — cmd-3-Handler `0x800366bc` (EXE), voll disassembliert

Dispatch: `PTR_LAB_80073f90[cmd]` (Dump dieser Session) — [0]=0x800318f8 [1]=0x80031de8
[2]=0x80035af0 Hit **[3]=0x800366bc TOD** [4]=0x80030660 Plc [5]=0x80036834 Grab
[6]=0x800368c0 (Devour-Tod) [7]=0x8003694c (Blutlache).

### 2.1 Erst-Frame-Präambel `0x800366bc` (nur wenn `aca5a == 0`)
```
800366bc: lui  v0,0x800b
800366c0: lbu  v0,-13734(v0)        ; aca5a (Phase)
800366c8: bne  v0,zero,0x80036700   ; Phase != 0 -> direkt in die FSM
800366d4: lbu  v1,-13610(v1)        ; player.floor (+0x82)
800366dc: sh   zero,-13564(at)      ; 0x800acb04 (= player +0xb0, Schatten-Deskriptor) := 0
800366e0-f4: v0 = -(floor * 1800)   ; ((f<<3)-f)<<5 +f <<3, negiert
800366fc: sh   v0,-13298(at)        ; 0x800acc0e := -floor*1800   (Boden-Referenz)
80036700: jal  0x80036718           ; die eigentliche FSM
```
(Identische Präambel wie der cmd-5-Grab-Handler @0x80036854-80 — im Port für cmd 5 bereits
modelliert, enemy_ai_common.c:458-463.)

### 2.2 Die Todes-FSM `0x80036718` — Phasen-Router auf `aca5a`
```
80036728: lbu  v1,0(a2)             ; a2 = 0x800aca5a
80036730: beq  v1,1 -> 0x800367c8   ; Phase 1
80036740: beq  v1,0 -> 0x80036764   ; Phase 0   (Delay-Slot: lui a0,0x403)
80036754: beq  v1,2 -> 0x80036804   ; Phase 2   (Delay-Slot: ori a0,zero,2)
```

**Phase 0 @0x80036764 — der Anim-Seed (das gesuchte „Finisher"-Setup):**
```
80036764: ori  a0,a0,0x1            ; a0 = 0x04030001      (SE-Wort)
8003676c: ori  v0,zero,0x1
8003676c/6c: sb v0,0(a2)            ; aca5a := 1
80036774: lbu  v0,-13593(v0)        ; player.hit_react (+0x93)
80036778: ori  v1,zero,0x7          ;  07 00 03 34
80036780: sb   v1,-13592(at)        ;  e8 ca 23 a0  -> 0x800acae8 = player+0x94  := CLIP 7
80036788: sb   zero,-13591(at)      ;  e9 ca 20 a0  -> +0x95  := Frame 0
80036790: sb   v1,-13597(at)        ;  -> +0x8f     := Blend 7 (Crossfade-Seed)
80036798: sh   zero,-13600(at)      ;  -> +0x8c     := 0      (Speed/Root-Akkumulator)
800367a4: sb   v0|1,-13593(at)      ;  hit_react |= 1
800367a8: jal  0x80045024           ;  Se_on(0x04030001, a1 = a2+46 = 0x800aca88 = player+0x34)
800367bc/c4: 0x800aca3c |= 0xC0     ;  Steuer-/Kamera-Gate-Bits (wie in der Opfer-FSM Ph0)
                                    ;  -> FÄLLT OHNE BREAK in Phase 1 (derselbe Tick advanct schon)
```

**Phase 1 @0x800367c8 — der Clip läuft:**
```
800367c8: addu a2,zero,zero         ; a2 = 0  (kein Frame-Skip = 1 Frame/Tick)
800367d0: lw   a0,-13608(a0)        ; a0 = [0x800acad8]
800367d8: lw   a1,-13376(a1)        ; a1 = [0x800acbc0]
800367dc: jal  0x8001f314           ; anim_set/advance
800367e0: ori  a3,zero,0x200        ; Blend-Rate 0x200
800367f0/f8: aca5a += v0            ; Rückgabe 1 beim Clip-Wrap -> Phase 2
```

**Phase 2 @0x80036804 — Abschluss:**
```
80036804: jal  0x80045630           ; FUN_80045630(a0=2, a1=0)
80036808: addu a1,zero,zero         ;   = boden-MATERIAL-abhängiger Aufschlag-SE + Staub-FX
80036814: sh   v0(=7),-13736(at)    ; 0x800aca58 := 7 (und aca59 := 0)  -> cmd 7 Blutlache
```

**Jeder Tick endet mit** `jal 0x800369f8(a0=0, a1=1)` @0x8003681c-20 = die Wurzelbewegung/
Fußlock, **Kanal 1** (`FUN_800369f8` Modus 0 verschiebt player+0x34/+0x3c um die Bone-Chain-Delta;
Decompile `RE_15_Quellcode_V2/FUN_800369f8.c`).

### 2.3 Welche BANK ist „Paar A"? — belegt, nicht angenommen
`FUN_800314b0` (Spieler-Init, `RE_15_Quellcode_V2/FUN_800314b0.c`) lädt die PLD nach `0x801bd814`:
`DAT_800acbc0 = base + dir[0]`, `DAT_800acad8 = base + dir[1]`.
Byte-Gegenprobe an `shared_assets/PSX/PLD/PL00.PLD` (Dir-Tabelle @word0 = 0x2e2a4):
`dir[0] = 0x8` → beginnt mit `22 00 60 00 22 00 e8 00 …` = **exakt die Clip-Tabelle von PL00.EDD**;
`dir[1] = 0xc60` = **PL00.EMR**; dir[2] = MD1, dir[3] = TIM.
⇒ **Paar A = (PL00.EMR-Keyframe-Pool, PL00.EDD)** — die Spieler-BASIS-Bank, **nicht** die
Waffen-Bank PLW (`+0x170/+0x174` = 0x800acbc4/0x800acbc8) und **nicht** die Krähen-Opfer-Bank
Paar C (`0x800acbcc/0x800acbd0`). Der Todes-Clip ist also **waffen-unabhängig**.

### 2.4 Clip 7 — Datei-Beleg + objektiver Inhalts-Nachweis
`PL00.EDD` Clip-Tabelle (u16-Paare `(frame_count, data_offset)` @ i*4), gelesen aus
`re15_port/shared_assets/PSX/PLD/PL00.EDD`:

| Clip | 0 | 1 | 2 | 3 | 4 | 5 | 6 | **7** | 8 | 9 | 10 | 11 | … | 22 | 23 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Frames | 34 | 34 | 50 | 26 | 20 | 20 | 50 | **113** | 22 | 22 | 20 | 25 | … | 30 | 30 |

**Clip 7 = 113 Frames (Daten @EDD 1032) — der längste Clip der Bank.**
Inhaltsbeleg (rein datengetrieben, KEINE Sichtprüfung): Root-Translation Y der referenzierten
EMR-Keyframes (`PL00.EMR` kf_off=176, bones=15, kf_size=80; PSX-Y ist nach oben negativ):

| Clip | Y am Anfang | Y in der Mitte | Y am Ende |
|---|---|---|---|
| 0 (Walk) | −1804 | −1783…−1794 | −1802 |
| 8 (Flinch vorn) | −1746 | −1742…−1755 | −1807 |
| 22 (Injured-Idle) | −1776 | −1759 | −1776 |
| **7** | **−1807** | **−901 / −915** | **−245 … −256** |

⇒ Clip 7 fährt den Wurzelpunkt von Hüfthöhe (−1807) über 113 Frames **auf Bodenhöhe (≈ −250)** —
der Zusammenbruch. Die übrigen Clips bleiben aufrecht. Damit ist „Clip 7 = Todes-Animation"
belegt, ohne sich auf „sieht richtig aus" zu stützen.

### 2.5 Zeitachse des Originals (aus den obigen Adressen abgeleitet, keine geratene Zahl)
| Tick | Ereignis |
|---|---|
| T+0 | Krähe schreibt cmd 3 (@0x80113f20 / b48 / 4518); Handler Ph0: Clip 7, Frame 0, Blend 7, `hit_react\|=1`, `Se_on(0x04030001)`, `aca3c\|=0xC0`; fällt in Ph1 und advanct bereits |
| T+0 … T+112 | Ph1: `anim_set(PL00.EMR, PL00.EDD, 0, 0x200)` 1 Frame/Tick; jeden Tick zusätzlich `FUN_800369f8(0,1)` (Wurzel/Fußlock) |
| ≈T+113 | Clip-Wrap → `aca5a := 2` |
| ≈T+114 | Ph2: `FUN_80045630(2,0)` = Boden-Material-Aufschlag-SE + Staub; `aca58 := 7` |
| T+114 … +235 | cmd 7 `0x8003694c`: Lachen-Farbwort `(alt & 0xff000000) \| 0x00ffff38` @0x8003699c-b8, Halbmaße `+0xbc/+0xbe += 12` @0x800369c4/d8, Timer `+0x9e = 0x78` @0x8003697c-88 herunter, dann `aca59 := 2` = Halt |
| parallel ab T+0 | `FUN_8001500c` (Fade/White/„YOU DIED"/Death-Cam) — gegated auf **cmd 3/6/7** (`beq v1,6` @0x8001501c, `beq v1,3` @0x80015028, `bne v1,7` @0x80015030), läuft also schon **während** der Todes-Animation |

---

## 3. PORT-IST — gemessen, nicht gelesen

**Sonde:** `re15_port/tests/unit/probe_crow_kill.c` (neu; Diagnose, kein ctest — CMake-Ziel
`probe_crow_kill`). ROOM1170 Re-Entry-Zweig (z3/125), Band 4, echte RDT, EM021-Opfer-Bank geladen,
`PL00.EDD/EMR` an `ctx.pl00_anim/pl00_skel`, voller `re15_game_step`. Zwei Läufe: Tod durch
erzwungenen DIVE (hp=3, −4) und Tod durch GRAB (hp=5, −8). Protokolliert werden 200 Ticks nach
`hp<0`.

Sonden-Kopf bestätigt die Datenlage: `PL00.EDD: clips=24 … [7]=113 …`

### Lauf B — Tod durch DIVE
```
t=   9 KILL  hp=-1 motion=0xc8(200) frame=16 hit_react=1 victim=0 grabbed=0 | pool=0 cam=0 flyin=-1
t=   0 dead  hp=-1 motion=0xc8(200) frame=16 ... pool=1
t=  50 dead  hp=-1 motion=0xc8(200) frame=16 ... pool=51 cam=0 flyin=-1
t=  80 dead  hp=-1 motion=0xc8(200) frame=16 ... pool=81 cam=1 flyin=3
t= 199 dead  hp=-1 motion=0xc8(200) frame=16 ... pool=122 cam=1 flyin=50
== DIVE-KILL: motion-Wechsel nach dem Tod = 0, motion==7 je gesehen = NEIN, Endmotion = 0xc8 frame=16
```
**Leon friert im Idle-Sentinel 200 auf Frame 16 ein und bleibt 200 Ticks lang aufrecht stehen.**
Position x/z konstant (−2306/−8030).

### Lauf A — Tod durch GRAB
```
t= 167 KILL  hp=-3 motion=0x00 frame=0 frac=7 victim=1 grabbed=1 | pool=0
t=   0 dead  hp=-3 motion=0x02 frame=0  frac=6 victim=3 grabbed=1 | pool=1     <- Krähen-RELEASE-Clip 2
t=  20 dead  hp=-3 motion=0x02 frame=19 frac=0 victim=3 grabbed=1 | pool=21
t=  30 dead  hp=-3 motion=0xc8(200) frame=0 hit_react=0 victim=0 grabbed=0 | pool=31
t= 199 dead  hp=-3 motion=0xc8(200) frame=0 ... pool=122 cam=1 flyin=50
== GRAB-KILL: motion-Wechsel nach dem Tod = 2, motion==7 je gesehen = NEIN, Endmotion = 0xc8 frame=0
```
**Der Port spielt nach dem tödlichen Grab den Krähen-Release-Clip 2 (20 Frames aus Paar C) und
stellt Leon danach wieder aufrecht in den Idle.**

### 3.1 Divergenz-Tabelle

| # | Original (Adresse) | Port-IST (gemessen/Code) | Wirkung |
|---|---|---|---|
| **D1** | cmd 3 Ph0 setzt **Clip 7** (`+0x94:=7` @0x80036780), Frame 0 (@0x80036788), Blend 7 (@0x80036790), Rate 0x200 (@0x800367e0); 113 Frames Zusammenbruch | **kein Todes-Clip überhaupt**; `re15_game_step` überspringt beim Tod `re15_player_tick` (game_step_common.c:818-836) und setzt `motion` nie — gemessen `motion=200`, 0 Wechsel | **Leon stirbt im Stehen.** Das ist der Nutzer-Report |
| **D2** | cmd 3 ersetzt cmd 5 sofort (@0x80113f20) → Opfer-FSM aus, **kein Release-Clip** | `re15_player_victim_tick` läuft ungegatet weiter (game_step_common.c:1466) → `g_player_victim 1→3`, Krähen-Clip 2 spielt 20 Frames | falsche Zwischen-Animation vor dem (fehlenden) Tod |
| **D3** | `Se_on(0x04030001, &player+0x34)` @0x800367a8 im Todes-Tick | kein Gegenstück auf dem Spieler-Todespfad — die einzigen `0x4030001`-Stellen im Port sind der HUND-Devour (enemy_ai_common.c:1202/1282, @0x80111df4) | Todes-SE fehlt |
| **D4** | `FUN_80045630(2,0)` @0x80036804 am Clip-Ende = Boden-Material-Aufschlag-SE + Staub-FX | fehlt (der Port kennt den Helfer, nutzt ihn aber nur für Schritte/Hund-Devour, enemy_ai_common.c:1203) | Aufprall-Sound + Staub fehlen |
| **D5** | Blutlache startet **erst mit cmd 7**, also ~114 Ticks NACH dem Todes-Tick (@0x80036814) | `g_death_pool` wächst ab dem ersten `hp<0`-Tick (game_step_common.c:328; Render main.c:5835/5892) — gemessen `pool=1` bereits bei t=0 | Lache erscheint ~113 Frames zu früh, **unter dem noch stehenden Leon** |
| **D6** | jeder cmd-3-Tick ruft `FUN_800369f8(0,1)` @0x8003681c-20 (Wurzel-/Fußlock, **Kanal 1**) | Port hat nur Kanal 0 (`push_root_step`, player_common.c:506); Kanal 1 ist explizit als „deferred" markiert (enemy_ai_common.c:8579) — gemessen: x/z bleiben konstant | Betrag unbekannt → **OPEN** (§5.1) |
| **D7** | Ph0-Präambel: `+0xb0`-Halbwort := 0 (@0x800366dc), `0x800acc0e := −floor·1800` (@0x800366fc); `aca3c \|= 0xC0` (@0x800367bc) | für cmd 5 modelliert, für cmd 3 nicht | Detail; Wirkung von `+0xb0`/`acc0c` unklar (§5.2) |

**Nicht divergent (positiv verifiziert):** dass beim tödlichen Dive KEIN Flinch spielt (Original
überschreibt cmd 2 mit cmd 3; Port zeigt gemessen ebenfalls keinen Flinch — allerdings aus dem
falschen Grund: er spielt gar nichts). Der Game-Over-Präsentations-Start am Todes-Tick ist
byte-richtig (`FUN_8001500c` ist auf cmd 3 gegated, @0x80015028) — nur die LACHE hängt im Port
am falschen Ereignis (D5).

### 3.2 RE2-KI-Modus
Im RE2-Flavor übernimmt `re15_re2crow_tick` (enemy_ai_re2_crow.c, Root @0x8010013C) den ganzen
Krähen-Dispatch. Der RE2-Peck ruft `re15_re2_player_damage_mode(pl, 5, alive)` (@0x8010265C-64) —
**Modus 1 tötet nie direkt** (One-Save-Schwanz, `pl->hp = 0` + Latch, enemy_ai_re2_zombie.c:764-772);
der RE2-Grab entlässt den toten Spieler nur (`re2c_grab_release` @0x80102510-18). Die
Spieler-Todes-PRÄSENTATION ist in beiden Modi dieselbe Port-Route (`re15_player_is_dead()` →
game_step_common.c:818) — **D1/D3/D4/D5 gelten in beiden Modi unverändert.** Nach dem
SOUND-MANDAT bleiben die SEs ohnehin RE1.5-seitig; für D3/D4 gibt es also keine Modus-Verzweigung.

---

## 4. Was die Umsetzung braucht (nur Fakten, kein Fix-Code)

Alles, was für den Einbau nötig ist, mit Adresse:

1. **Todes-Kommando einführen** (der Port hat kein cmd-Register): beim `hp<0`-Übergang
   `motion := 7`, `anim_frame := 0`, `anim_frac := 7`, `anim_blend_rate := 0x200`
   (@0x80036780 / @0x80036788 / @0x80036790 / @0x800367e0), `hit_react |= 1` (@0x800367a4).
   `7` ist ein **echter PL00.EDD-Index**, kein Sentinel — `anim_select` überspringt damit das
   W01/Idle-Remap (dokumentiert player_common.c:110-113), was genau der Paar-A-Wahl des Originals
   entspricht (§2.3).
2. **Victim-FSM beim Tod abwürgen** (D2): `re15_player_victim_tick` darf nach `hp<0` nicht mehr in
   den Release-Zweig gehen — im Original ist cmd 5 in dem Moment schon durch cmd 3 ersetzt
   (@0x80113f20).
3. **Clip-Ende-Handoff** (113 Frames, EDD-gelesen, nicht hartkodieren — aus
   `pl00_anim.clips[7].frame_count`): dann `FUN_80045630(2,0)`-Äquivalent (@0x80036804) und
   **erst dann** die Lache starten (@0x80036814) → D5 auflösen, indem `g_death_pool` an diesen
   Handoff statt an `hp<0` gehängt wird. Die Game-Over-Kette (`re15_gameover_fsm_tick`) bleibt am
   Todes-Tick, weil `FUN_8001500c` auf cmd 3 gegated ist (@0x80015028).
4. **SE** `0x04030001` an den Player-Positions-Kanal (@0x800367a8, a1 = 0x800aca88 = player+0x34).
5. **Nicht erfinden:** kein eigener Krähen-Todes-Clip (existiert nicht, §1), kein cmd-6-Pfad
   (Hook B ist ein `jr ra`-Stub @0x80115d6c), keine Timeout-/Rundungszahl — die 113 stehen in der
   EDD, die 0x78 in @0x8003697c.

---

## 5. OFFEN (ehrlich)

1. **`FUN_800369f8(0,1)` Kanal-1-Betrag** (D6): dass der Todes-Tick ihn jeden Frame ruft, ist
   belegt (@0x8003681c-20); WIE WEIT Leon dadurch beim Zusammenbruch verschoben wird, ist nicht
   gemessen (Port-XZ bleibt konstant). Nächster Weg: `FUN_800369f8` Kanal 1 gegen die
   Clip-7-Keyframes rechnen oder DuckStation-Savestates über den Sturz (Skill `re15-room-capture`).
2. **`0x800acb04`-Halbwort := 0 (@0x800366dc) und `0x800acc0c`** (letzteres setzt die Opfer-FSM,
   die Todes-FSM nicht): Semantik weiter offen (= `crow_victim_anim.md` §7.6).
3. **Waffen-Mesh während des Todes:** Clip 7 kommt aus Paar A, die Waffen-Bindung
   (`FUN_80036b68`, `+0x170/+0x174`) wird von der Todes-FSM nicht angefasst — ob die Waffe
   weiter in der Hand gerendert wird, ist nicht verifiziert.
4. **`FUN_80045630(2,0)`-Sample:** der Helfer ist im Port vorhanden (vab_common.c:161ff), der
   konkrete Boden-Material-Record für den Helipad-Boden ist nicht gegengeprüft.
5. **Hardware-Gegenprobe** (Krähe tötet Leon im echten Spiel, DuckStation): steht aus. Die
   Original-Seite hier ist rein statisch belegt (Disasm + EDD/EMR-Bytes) — was ausreicht, um D1-D5
   zu benennen, aber die Frame-Genauigkeit des Handoffs (T+113 vs. T+114) sollte am Ende gegen
   einen Savestate laufen.
