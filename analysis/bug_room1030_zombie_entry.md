# ROOM1030 — „Nur sechs Zombies" + „kriechen unter dem Tor durch"

Nutzer-Report 2026-08-06. Synthese aus fünf parallelen RE-Bahnen (spawn-zensus, behavior-0d,
tor-und-einlauf, port-ist-zustand, render-sichtbarkeit), jede adversarial gegengeprüft.
Alle unten mit `@0x…` zitierten Instruktionen habe ich beim Schreiben dieses Dossiers
**selbst noch einmal disassembliert** (`re15_disasm.py` gegen `info/Re1.5/PSX.EXE` und
`info/Re1.5/PSX/BIN/STAGE1.BIN`) bzw. als RDT-Bytes gedumpt.

**Kurzantwort:** Die beiden Symptome haben **NICHT** dieselbe Ursache. Sie sind zwei
unabhängige Lücken im selben Raum-Design.
* „Nur sechs" = ein **datengetriebenes Spawn-Limit** (`Save(0x12,6)`), das der Port nicht liest.
* „Kriechen" = eine **fünfgliedrige Zustandskette** (AOT-Stempel → `anim_flags` Bit 0x1000 →
  Sub-Modus 0x10 → Grid-Nibble 1 → Kollisionsmaske 8), von der im Port **vier Glieder fehlen**.

Die Arbeitshypothese („behavior 0x0d ist die Einlaufmaschine") ist **widerlegt**.

---

## 1. BELEGT — Warum im Original nur SECHS Zombies existieren

### 1.1 Der Mechanismus

`ROOM1030.RDT` führt in `main00` **unmittelbar vor** seinen 20 `Sce_em_set`-Records einen
Opcode aus, der das globale Gleichzeitig-Limit auf 6 setzt:

```
ROOM1030.RDT  Datei-Offset 0x1de2:   24 12 06 00      = Save(work_var 0x12, 6)
ROOM1030.RDT  Datei-Offset 0x1de6..0x1f62:            = 20x Sce_em_set, Stride 20
                                     44 <slot> 16 0d 00 00 <b> <slot> …
```
(selbst gedumpt; ROOM1031 @0x1e58 identisch; ROOM1040 @0x11f0 / ROOM1041 @0x1218 = `24 12 05 00`)

**Save-Handler** — Dispatch-Tabelle `0x800744a8[0x24] = 0x80040018` (selbst gelesen):
```
80040020: lbu v1,1(v0)          ; work-var index = pc[1]
80040024: lh  a1,2(v0)          ; wert = pc[2..3]
80040030: sll v1,v1,1
80040038: addiu at,at,4048      ; = 0x800b0fd0  (Basis der work_vars)
80040040: sh  a1,0(at)
```
→ `0x800b0fd0 + 0x12*2 = ` **`0x800b0ff4`**.

**Genau diese Adresse ist das Spawn-Maximum**, das `Sce_em_set` (`FUN_800420a0`) prüft:
```
800421dc: addiu a1,a1,4082      ; a1 = 0x800b0ff2   (LEBEND-ZAEHLER)
80042214: lh  v0,0(a1)          ; count
8004221c: lh  v1,4084(v1)       ; = 0x800b0ff4      (MAXIMUM)
80042224: slt v0,v0,v1          ; count < max ?
80042228: bne v0,zero,0x8004223c
8004222c: addiu v0,a0,1         ; (delay) count+1
80042230: ori v0,zero,0x8000    ; ABLEHNUNG:
80042234: j   0x8004261c        ;   entity+0x00 = 0x8000  (Bit0 AKTIV = 0)
80042238: sw  v0,0(s0)          ;   kein Modell, kein INIT, kein g_active_count++
8004223c: sh  v0,0(a1)          ; ANNAHME: count = count+1
```
Beide Pfade landen im gemeinsamen Epilog, der den PC weiterschaltet:
```
80042620: lw  v1,28(s5)
8004262c: addiu v1,v1,20
80042630: sw  v1,28(s5)          ; PC += 20 auch im Ablehnungs-Pfad
```

**Default und Reset** kommen aus der Raum-SCD-Init `FUN_8003ecec`, die **vor** `main00` läuft
(einziger Aufrufer `FUN_8003ef6c @0x8003ef84`):
```
8003ed58: ori v0,zero,0xff
8003ed60: sh  v0,4084(at)        ; 0x800b0ff4 = 255  (kein Limit)
8003ed7c: sh  zero,4082(at)      ; 0x800b0ff2 = 0    (Zähler)
```

**Reihenfolge ist byte-relevant:** das Kill-Flag-Gate (`pc[7]`, `@0x80042120-58`) liegt **VOR**
dem Zähler-Gate. Ein bereits getöteter Gegner verbraucht also **kein** Kontingent — die 14
übrigen Records sind eine **Nachschub-Warteschlange** beim Wieder-Betreten.

**Ergebnis:** Records 0..5 spawnen, Records 6..19 werden verworfen. **Exakt SECHS.**

### 1.2 Game-weiter Zensus

Drei Bahnen haben unabhängig (instruktions-alignter SCD-Walk mit Desync-Guard über alle
RDTs) dasselbe Ergebnis: `Save` auf work_var 0x11/0x12 existiert in **genau vier Dateien** —
ROOM1030/1031 (=6, 20 Records) und ROOM1040/1041 (=5, 40 Records). Kein `Copy`/`Calc` auf
diese Indizes. Ohne Desync-Guard produziert der Walk Falschtreffer (ROOM2030 sub06,
ROOM2060 sub20) — wer den Zensus wiederholt, MUSS abbrechen, sobald ein ungültiger Opcode kommt.

### 1.3 Die Messung, die die Kausalität zeigt

Zwei unabhängig gebaute Live-Sonden (`probe_lanea_1030_cap.c`, `probe_adv_1030_verify.c`,
`probe_gate_1030.c`, `probe_laned_1030_spawn.c` — jeweils Raum wirklich geladen, main00/sub00
durch, VM getickt, EM016-Bank geladen, Live-AI gelaufen):

| Messgröße | Port | Original |
|---|---|---|
| aktive Gegner in ROOM1030 | **15** | **6** |
| `work_vars[0x12]` (Cap) | 6 (korrekt geschrieben) | 6 |
| `work_vars[0x11]` (Zähler) | **0 — nie inkrementiert** | 0→6 |
| Kontrollraum ROOM1140 `work_vars[0x12]` | **0** (memset-Default) | 0xFF |

**Beleg für „kein Konsument":** `grep` über `re15_port/engine`, `include`, `platform` findet
**null** Treffer für `0x800b0ff2` / `0x800b0ff4` / `work_vars[17]` / `work_vars[18]`.
`op_sce_em_set` (`scd_vm.c:2974`) hat kein Zähler-Gate.

Damit ist die Kausalkette geschlossen: der Save läuft byte-true, der Wert liegt im Array,
und **niemand liest ihn**.

### 1.4 Was NICHT die Ursache ist (widerlegt, nicht erneut untersuchen)

* **`RE15_ACTOR_MAX = 16`** (`re15_actor.h:22`) verwirft die Script-Slots 15..19 — das ist eine
  echte Divergenz, zeigt aber in die **falsche Richtung**: der Port hat 15 statt 20, das Symptom
  ist „zu viele". Nach dem Cap-Fix ist es für ROOM1030 gegenstandslos (6 < 15).
  Anheben würde das Symptom kurzfristig **verschlimmern**.
* **Fehlende `sprite.pri`-Vordergrundmasken.** `shared_assets/PSX/BSS/ROOM1030/` enthält nur
  `BG00..BG13.BSS`, keine `PRI##.TIM` — der Port zeichnet in ROOM1030 **null** Masken
  (`bg_pc.c:82` → `main.c:3369` → `render_pc.c:711` `mask_n=0`). Das ist ein **echter,
  separater Defekt** mit validiertem Fix-Pfad (der Python-Port von `SldDecoder.findBlock/
  decompress` reproduziert ROOM1090s 13 vorhandene `PRI##.TIM` **byte-identisch** und findet in
  ROOM1030 SLD-Blöcke an 12 Offsets, passend zu den 12 Cuts mit nicht-NULL `pri_offset`).
  **Aber er erklärt die Anzahl nicht:** eine Occlusion-Messung mit den Cut-0-Kameraparametern
  ergibt Abdeckungen von 0/11/18/22/24/25/40/53/78/100 % — **genau einer** von 10 ist voll
  verdeckt. Die Lamellen liegen in y 88..154, die Zombies projizieren mit Füßen bis y~175.
* **Region-Quad-Cull.** Byte-identisch zum Original: 13 Cuts × 20 Positionen, **null Divergenz**
  gegen den Nachbau von `FUN_80014368`. Es gibt im Original **kein** Etagen-Gate, **kein**
  Distanz-Cull, **kein** Draw-Limit für Gegner (Zeichen-Schleife `@0x8001d0a8-0x8001d168` hat
  nur `andi 0x1` und die Obergrenze `DAT_800aca4e`; OT = 1024 Wörter, einziges Verwerfen ist
  das Near-Gate `otz<64 @0x80025654`). **Nicht anfassen.**
* **Per-Frame-sub01-Reseed.** Gemessen: Aktorzahl über 1200 Ticks konstant. Die 20
  `Sce_em_set` stehen in `main00`, in keinem Sub.
* **Opcode 0x53** ist im Port zwar `op_sce_fade_set` **benannt**, der Rumpf (`scd_vm.c:3775ff`)
  ist aber bereits byte-true `Work_set`-by-work_var mit korrekten `@0x80040e18/e40/e44/e58`-Zitaten.
  Gemessen: mit `flag(4,15)=1` setzt der Port exakt die sechs Nord-Positionen aus `sub00`.
  **Reine Umbenennung, kein Verhaltensfix.**

---

## 2. BELEGT — Was „unter dem Tor durchkriechen" technisch ist

### 2.0 Vorab: behavior 0x0d ist KEINE Kriech-Maschine

```
80100f20: lbu v0,9(a0)          ; behavior-Byte
80100f24: ori v1,zero,0xd
80100f28: andi v0,v0,0x1f
80100f2c: bne v0,v1,0x80100f60
80100f34: sb v0,148(a0)         ; +0x94 = 0x27   (Start-Clip)
80100f44: sw v0,4(v1)           ; +0x04 = 0x00000201  (state 1 / Sub 2 = ENGAGE)
80100f54: sb zero,9(v0)         ; +0x09 = 0      ← behavior LÖSCHT SICH SELBST
```
0x0d = **„spawnt bereits wach"**, ein Einmal-Selektor. Weil `+0x9` danach 0 ist, kann der
Sub-Modus-Dispatch `@0x80101654 andi v0,v0,0xf` den Index 13 **nie** sehen — der vermutete
Tabellenüberlauf (`0x8011f80c` hat 13 Einträge, `[13]` fällt physisch auf `0x8011f840[0]`) ist
gegenstandslos. Der Port macht das bereits byte-true (`enemy_ai_common.c:1020`).
**Diesen Code nicht anfassen** — 0x0d wird game-weit von weiteren Räumen benutzt (u.a. ROOM1040,
ROOM1200), wo gar kein Tor existiert.

### 2.1 Das „Tor" ist keine Tür und kein Objekt

ROOM1030 hat `nDoor=0`, `nOmodel=1`; das einzige Prop ist ein **Dauer-Rotator**
(`sub11 @0x2878`: `Work_set(3,0); Speed_set(4,-32); Add_speed; Goto -2`, Handler
`@0x80040f3c sh a1,344(v1)` / `@0x80040fa4 sh v0,106(a1)` = `rot_y` −32/Frame). Wer eine
Tor-Animation sucht, sucht falsch.

Das Tor ist **eine Kollisionszelle plus eine Bitmaske**:
```
SCA rgn2/idx6 == rgn3/idx6:  type=1  u0=0xFF  floor=0
                             x[-20144..4120]  z[-24420..-22771]
```
Die einzigen beiden `floor=0`-Zellen des Raums, über die volle Raumbreite. Alle 20 Zombies
stehen südlich davon (z ≤ −25655).

`Sca_id_set` öffnet sie — Bytes selbst gedumpt:
```
ROOM1030.RDT 0x2000: 37 02 06 f7      (sub00, Zweig Ck(4,15)==1)
ROOM1030.RDT 0x2004: 37 03 06 f7
ROOM1030.RDT 0x278e: 37 02 06 f7      (sub08 = die Cutscene)
ROOM1030.RDT 0x2792: 37 03 06 f7
```
Handler `LAB_8004175c`, `@0x8004179c sb a1,9(v0)` = das `u0`-Byte des 12-Byte-Eintrags.
**0xFF → 0xF7 löscht GENAU Bit 0x08.**

### 2.2 Die Maske ist PRO ENTITY, nicht global

```
80100620: lw  v0,120(a0)         ; box
80100624: lbu a2,471(a0)         ; = entity+0x1D7  ← DIE KOLLISIONSMASKE
80100628: lhu a1,6(v0)           ; radius
8010062c: jal 0x8003b0a4         ; Resolver
```
Der Resolver hält eine Zelle nur solid, wenn `(maske & u0) != 0` (`@0x8003b244-58`).
Zombie-INIT setzt `+0x1D7 = 4` (`@0x80100828`). Der Spieler übergibt das Literal 1
(`@0x80031d74` / `@0x800384c8` in PSX.EXE — es gibt in der EXE **kein einziges**
`sb …,471`). Von 13 `jal 0x8003b0a4`-Aufrufern in STAGE1.BIN lesen **nur zwei** `+0x1D7`:
`FUN_80100688` (Zombie 0x16) und `FUN_8010a8c8` (Zombie Girl 0x13); die übrigen 11 setzen
`ori a2,zero,0x4`.

Rechnung:
| Entity | Maske | `& 0xF7` | Tor |
|---|---|---|---|
| Spieler | 1 | 1 | **zu** |
| aufrechter Zombie | 4 | 4 | **zu** |
| **kriechender Zombie** | **8** | **0** | **OFFEN** |

Das ist wörtlich „unter dem Tor durchkriechen".

### 2.3 Die vollständige Zustandskette

**(1) AOT-Stempel `entity+0x0B` (= SCD-Member 15).**
`FUN_800436a8` fährt den AOT-Scan pro Frame dreimal (Spieler `a1=1`, Gegner `a1=2`, Objekte).
Vor jedem Gegner wird `+0x0B` genullt:
```
80043714: addiu at,at,-13257     ; = 0x800acc37 = enemy_pool + 0x0B
8004371c: sb zero,0(at)
80043720: ori a1,zero,0x2        ; Pool-Maske GEGNER
80043724: jal 0x80042bac
80043728: addu a2,zero,zero      ; a2 = 0  → AUTO-Scan
80043730: addiu s2,s2,500        ; Stride 0x1F4
```
Der Stempel selbst in `FUN_80042bac`:
```
80042f40: beq s6,zero,0x80042fc4   ; s6 = a2 = 0 für ALLE drei Pässe → AUTO
80042f44: addiu v0,s2,255          ; (delay) = Slot-Index (s2 = index+1)
80042fc4: sb v0,11(s1)             ; entity+0x0B = AOT-Slot
…
80043010: jalr v0                  ; Handler
8004301c: bne v0,zero,0x80042c50   ; ← SCHLEIFE LÄUFT WEITER
```
⚠️ **LAST-WINS, nicht first-wins.** Der frühe Rücksprung `j 0x80043028 @0x80042fbc` liegt
ausschließlich im ACTION-Pfad (`a2 != 0`), der zusätzlich bei `sce==0` aussteigt
(`@0x80042f50`). Das ist in ROOM1030 nicht akademisch: **AOT 4 ist vollständig in AOT 6
enthalten** und AOT 5 überlappt es. Eine „erster Treffer gewinnt"-Implementierung stempelt 4,
wo das Original 6 stempelt.
Member 15 → `+0x0B`: `FUN_80041358`, Tabelle `0x80010cfc[15] = 0x80041438 lbu v0,11(a0)`.

Die drei relevanten AOTs (alle `flags=0x42` = Gegner-Pool):
| AOT | Rect | Rolle |
|---|---|---|
| 5 | x[−12900..−3700] z[−25300..−24200] | Tor-Mund, **südlich** |
| 4 | x[−14800..−2800] z[−22500..−20300] | **nördlich**, direkt hinter dem Tor |
| 6 | x[−27300..3300] z[−24500..−700] | raumgroßer Zähl-Marker (sce=0) |

**(2) Das Skript pollt den Stempel und setzt das Kommando-Bit.**
```
sub03 @0x220e   3e 00 0f 00 06 00    Member_cmp(15, ==, 6)  → zählt nach work_var 7
sub04 @0x223e   3e 00 0f 00 04 00    Member_cmp(15, ==, 4)  → Gosub sub05 (aufstehen)
sub06 @0x24e0ff 3e 00 0f 00 05 00    Member_cmp(15, ==, 5)  → Gosub sub07 (hinlegen)
sub07 @0x2754   3d 04 10 | 26 00 05 04 00 10 | 35 10 04      anim_flags |= 0x1000
sub05 @0x24b2   3d 04 10 | 26 00 06 04 ff 0f
                         | 26 00 05 04 00 20 | 35 10 04      anim_flags = (x & 0x0FFF) | 0x2000
```
`0x3D` = `Member_copy` (`0x800744a8[0x3D] = 0x80041238`; `lb` dst=pc[1], `lb` mid=pc[2],
`lw a0,340(a0)` = Work-Entity, `sh v0,0(0x800b0fd0+dst*2)`, PC+=3).
Member 16 → `+0x1C4`: Tabelle `0x80010cfc[16] = 0x80041444 lhu v0,452(a0)`.

**Der Governor:** `sub02` (Endlosschleife, gestartet aus `sub00 @0x2176` per
`Evt_exec(0xff,0x18,2)`) schaltet AOT 6 für einen Frame auf `sce=5/flags 0x42` um, lässt
`sub03` zählen und gibt über `Cmp(work_var[7] < 4)` (`@0x21cc`, Opcode 0x23 = Work-Var-Vergleich,
Handler `@0x8003ff68`, Operator-Tabelle `@0x80010c3c`, `[3] = <`) nur dann den nächsten
Zombie frei. **Es dürfen nie mehr als 3 gleichzeitig nördlich stehen** — das ist die Staffelung.
Zusätzlich reißt die Cutscene `sub08` über 4× `Gosub sub09` vier auf einmal los.

**(3) Bit 0x1000 → Sub-Modus 0x10.** Drei Entscheider lesen es identisch:
```
80101ec4: lhu v0,452(v1)  /  andi v0,v0,0x1000  /  ori v0,zero,0x1001  /  sw v0,4(v1)
801021cc: (identisch)
80105790: (identisch)
```
`0x1001` = state 1, `+0x5 = 0x10`.

**(4) Die Prone-Maschine.** `0x8011f840[16] = 0x80104f78` (`jr ra`-Stub) /
`0x8011f890[16] = 0x80104f80`:
```
Phase 0:  +0x93 |= 1
          80104fe4: ori v0,zero,0x12
          80104fec: sb  v0,148(v1)      ; +0x94 = Clip 0x12 (98 Frames, „hinlegen")
          80104fe8: jal rng  /  80104ff8: andi v0,v0,0x3  /  80104ffc: sb v0,149  ; +0x95
          +0x6 = 1 ; +0x8f = 0x0f
          80105044: andi v0,v0,0x80     ; (+0x9 & 0x80)
          80105050: sb  a0,159(v1)      ; +0x9f = 1  NUR wenn NICHT bereits downed
          → FÄLLT DURCH nach 0x80105054 (kein eigener Tick!)
Phase 1:  anim_set(+0x170, +0x174, (s8)+0x9f, 0x100) ; +0x6 += Rückgabe
Phase 2:  80105094: sb zero,9(a0)       ; +0x9 = 0
          801050a4: sw v0,4(v1)         ; +0x4 = 0x201
          801050b0: ori v0,zero,0x4
          801050b4: sb  v0,471(v1)      ; +0x1D7 = 4
          801050c4: lb v0,159(v1)  / 801050cc: beq
          801050d0: ori v0,zero,0x81
          801050d4: sb  v0,9(v1)        ; +0x9 = 0x81   ← ZUWEISUNG, nicht |=
          801050e4: sw  a1,4(v0)        ; +0x4 = 1      (state 1 / Sub 0)
          801050f0: ori v0,zero,0x8
          801050f4: sb  v0,471(v1)      ; +0x1D7 = 8    ← TOR WIRD DURCHLÄSSIG
          +0x93 &= 0xfe
```

**(5) Grid-Nibble 1 = der Kriech-Brain.** `+0x9 = 0x81` → `& 0xf = 1` →
`0x8011f80c[1] = 0x80101708`; Decide-Tabelle `0x8011f8e0` (`@0x80101728`),
Animate-Tabelle `0x8011f920` (`@0x8010175c`).
```
Animate[0] = 0x801036dc :  +0x8c = 0x1e (Speed 30)
                           80103718: ori v0,zero,0x1a
                           8010371c: sb  v0,148(v1)   ; +0x94 = Clip 0x1A = DAS KRIECHEN
                           +0x95 = 0 ; +0x8f = 0x0f
                           80103748: ori v0,zero,0x8
                           8010374c: sb  v0,471(v1)   ; +0x1D7 = 8 gehalten
                           steer(+0x1bc, +0x1be, 0x10) @0x80103780
                             — GEGATET auf *(u32*)(+0x168) & 0x10000 (@0x8010375c-70)
                           anim_set(..., 0x100) = LOOP
Decide[0]  = 0x801035f8 :  Ausstieg @0x8010369c-c4
                           lhu +0x1C4 / andi 0x2000 / beq
                           lhu +0x1D8 / andi 0x80  / bne
                           ori v0,zero,0x601 / sw v0,4(v1)   ; → Sub 6
Animate[6] = 0x80104f80 :  dieselbe Prone-Maschine, jetzt mit +0x9f = 0
                           → +0x9 = 0, +0x4 = 0x201, +0x1D7 = 4 = wieder aufrecht
```
Die Einträge `0x8011f8e0[7..14]` und `0x8011f920[7..14]` sind **`0x00000000`** — dort darf
nicht dispatcht werden.

### 2.4 Warum es im Port nicht passiert — fünf Lücken, alle gemessen

| # | Lücke | Port-Ort | Messung |
|---|---|---|---|
| K1 | AOT-Scan stempelt `member_0b` **nur für Props**, kein Per-Frame-Clear für Aktoren | `aot_common.c:324` (einziger Write) | Aktoren exakt in AOT-5/AOT-4-Mitte gesetzt, 16–240 Ticks: `member_0b` bleibt **0** |
| K2 | Opcode **0x3D nicht registriert** (fällt auf `op_unknown`; Größe [0x3D]=3 → kein PC-Desync) | `scd_vm.c` — `grep -c "s_op_table\[0x3D\]"` = **0** | sub07-Äquivalent schreibt `anim_flags = 0x1234` statt `0x1007` |
| K3 | Kein ANIMATE-Zweig für `sub_state_1 == 0x10` | `enemy_ai_common.c:2577-2626` (0..0x0d, 0x11, 0x12, 0x13) | Eintritt existiert (`:333`, `:407`), Ziel nicht → Zombie würde einfrieren |
| K4 | Kein `case 1:` im Grid-Root-Switch | `enemy_ai_common.c:2549`, `default: break` @`:2669` | `+0x9 = 0x81` ⇒ **gar keine AI** |
| K5 | Kollisionsmaske hart `4u` statt `+0x1D7` | `re15_collision.c:617` | Port setzt `u0` korrekt auf 0xF7, blockt aber weiter: `4 & 0xF7 = 4` |

Live-Endzustand ohne Fix (`probe_laned_1030_spawn`, `probe_gate_1030`, echte AI, publiziertes
`g_room_rdt`): **15 von 15 Zombies stauen sich über die volle Raumbreite direkt an der
Torlinie** (z ≈ −24838, x −12917..−4268), 0 im Raum, `anim_flags` konstant 0x0004,
`Flags(5,0..19)` leer, `work_var[7] = 0`.

**BLOCKER vor K1:** `member_0b` ist im Port **doppelt belegt** — Hund und Adult-Spider
missbrauchen es als LOS-Latch (`enemy_ai_common.c:7062-7063` und `:7278-7279`:
`if (p != 2) e->member_0b = (uint8_t)(p & 1); los = e->member_0b & 1;`). Ein AOT-Stempel
würde die Sichtlinie jeden Frame zerschießen — und umgekehrt. Der Kommentar
`re15_actor.h:51 "stored, no consumer yet"` ist **stale/falsch** (Guess-Tell).

---

## 3. BELEGT — Haben beide Symptome dieselbe Ursache?

**NEIN.** Zwei unabhängige Komplexe:

* **Anzahl** = ein einzelner ungelesener Wert (`work_vars[0x12]`) im SCD/Spawn-Pfad.
  Fix ohne jede Berührung der Gegner-AI.
* **Kriechen** = eine Kette aus AOT-Scan, SCD-Opcode, zwei Gegner-Sub-Maschinen und der
  Kollisions-Signatur. Fix ohne jede Berührung des Spawn-Pfads.

Sie sind **im Raum-Design gekoppelt** (die sechs gecappten Zombies sind genau die, die durch
das Tor gestaffelt hereinkommen sollen), aber im Code vollständig disjunkt.
**Praktische Folge:** der Cap-Fix (§4.1) behebt Symptom 2 **allein** und sofort.

---

## 4. Fix-Plan — minimal, byte-true, in dieser Reihenfolge

### 4.1 S1 — Spawn-Cap (behebt Symptom „nur sechs" allein)

**Ändern:**
1. `re15_port/engine/src/scd_room_setup.c:112` — direkt **nach** dem `memset(&g_scd,0,…)` und
   **vor** `scd_thread_start(0, main_scd)`:
   ```c
   g_scd.work_vars[0x12] = 0x00FF;  /* DAT_800b0ff4 = 0xff  @0x8003ed58-60 (FUN_8003ecec) */
   g_scd.work_vars[0x11] = 0;       /* DAT_800b0ff2 = 0     @0x8003ed7c   — läuft vor main00, @0x8003ef84 */
   ```
2. `re15_port/engine/src/scd_vm.c:2974` (`op_sce_em_set`) — **nach** dem bestehenden
   `suppress`-Gate (Kill-Flag, `@0x80042120-58`) und **vor** jeder Aktor-Aktivierung:
   ```c
   /* @0x80042214-3c: count(0x800b0ff2) < max(0x800b0ff4) ? sonst entity+0x00 = 0x8000, kein Spawn */
   int16_t cnt = g_scd.work_vars[0x11], cap = g_scd.work_vars[0x12];
   if (!(cnt < cap)) { t->pc += 20; return 1; }   /* PC-Advance = gemeinsamer Epilog @0x8004262c */
   g_scd.work_vars[0x11] = (int16_t)(cnt + 1);    /* @0x8004223c */
   ```
   Der bestehende `re15_enemy_spawn_count_inc()` (`DAT_800aca4e`, `@0x80042558-60`) muss
   **hinter** dieses Gate (im Original wird er im Ablehnungs-Pfad nicht erreicht).

**Fasst NICHT an:** `RE15_ACTOR_MAX`, `op_save`, `op_sca_id_set`, den Gegner-AI-Code,
den Region-Cull, `behavior 0x0d`, die em-Status-Bank-Wahl.

**Regressions-Gate:**
* ⚠️ **Schritt 1 ist zwingend.** `g_scd` wird bei jedem Raum-Load genullt; gemessen ist
  `work_vars[0x12] = 0` in ROOM1140. Mit `cnt < 0` würde **jeder** Raum ohne `Save(0x12,…)`
  **null** Gegner spawnen.
* Volle 115 ctests, speziell `test_room1140_spawn`, `test_room1140_combat`,
  `test_em_status_persist`, `test_scd_opcodes`, `probe_marvin_spawn`, `probe_zreentry_10d0`,
  `probe_room1070_wake`.
* Positiv: `probe_lanea_1030_cap` / `probe_gate_1030` muss **6** statt 15 melden,
  `work_vars[0x11] == 6`.
* Gegenprobe **ROOM1040** (Cap 5, 40 Records → 5) und **ROOM20B0** (35 Records, **kein** Cap →
  darf sich nicht ändern, bleibt bei der `RE15_ACTOR_MAX`-Kappung).
* Nachschub-Test: Kill-Flags für Record 0/1 setzen, Raum neu betreten → Records 6/7 rücken
  nach, Gesamtzahl bleibt 6 (Kill-Flag-Gate vor Zähler-Gate, `@0x80042120-58` vs `@0x80042214`).

**Kein Dekrement beim Töten einbauen ohne Beleg** — siehe §6 (WAHRSCHEINLICH).

---

### 4.2 S2 — `member_0b` entflechten (Blocker, reines Feld-Umhängen)

`enemy_ai_common.c:7062-7063` und `:7278-7279`: LOS-Latch auf ein **eigenes** Aktorfeld
umziehen; `member_0b` danach exklusiv für den AOT-Stempel. Kommentar
`re15_actor.h:51 "stored, no consumer yet"` korrigieren.

**Fasst NICHT an:** Verhalten (Wert-für-Wert identisch), Prop-Notch (`aot_common.c:324`,
`scd_vm.c:2843/2857` — Props behalten ihren eigenen `member_0b`).

**Regressions-Gate:** vorher/nachher eine Sonde, die den heutigen `member_0b`-Verlauf für
Hund/Spider protokolliert (Werte müssen identisch bleiben); `test_dog_ai`, `test_crow_ai`,
`test_crow_fixes`, `probe_crow_1170`, `probe_crow_flinch`, `probe_crow_death`, 115 ctests.

⚠️ **BELEG FEHLT:** die Original-Adresse des LOS-Latches (`+0x1d0` Bit 0) ist **nicht**
verifiziert — `@0x80110e70-bc` disassembliert als `+0x5`-indizierter Vtable-Dispatch, nicht als
Latch-Write. Die Zitatquelle ist ein stale Port-Kommentar (zirkulär). Für ein **reines
Umhängen** genügt das (es ändert kein Verhalten), aber das neue Feld darf **nicht** mit einem
erfundenen `@0x`-Kommentar versehen werden.

---

### 4.3 S3 — Opcode 0x3D `Member_copy` registrieren

`scd_vm.c` (Registrierung bei ~`:283-350`), neuer Handler byte-true nach `@0x80041238`:
```c
/* Member_copy (0x3D) @0x80041238: work_vars[pc[1]] = get_member(work_entity, pc[2]); PC += 3
 * @0x8004124c lb s0,1(v0) / @0x80041250 lb a1,2(v0) / @0x80041254 addiu v0,v0,3
 * @0x8004125c lw a0,340(a0) / @0x80041260 jal 0x80041358 / @0x8004126c addiu at,at,4048 */
```
Beide Operanden werden mit `lb` (**signed**) gelesen. Prop-Pfad analog zu `op_member_set2`.

**Fasst NICHT an:** die Größentabelle (`s_op_table`-Größe `[0x3D]=3` ist bereits korrekt, es
gibt heute keinen PC-Desync), Opcode 0x35, 0x53, 0x2E.

**Regressions-Gate:** Zensus ist eng — 0x3D existiert nur in ROOM1030/1031 (je 5×) und
ROOM1040/1041 (je 40×); ein Kandidat in ROOM3020 sub06 (`3d 00 8a`, Member-ID 0x8a außerhalb
0..0x13, ohne Gegenstück in ROOM3021) ist mit hoher Wahrscheinlichkeit ein Walker-Artefakt und
muss vor dem Commit einzeln geprüft werden. `test_scd_opcodes` + neue Sonde + ROOM1040-Lauf.

---

### 4.4 S4 — AOT-Stempel `entity+0x0B` für Aktoren

`re15_port/engine/src/aot_common.c` (`re15_aot_scan`):
1. Am Anfang für jeden aktiven Aktor mit `flags & 1`: `member_0b = 0`
   (`@0x8004371c sb zero,0(0x800acc37 + i*500)`), für Props analog (`@0x80043788`).
2. Beim Durchlaufen der AOT-Slots **aufsteigend**, für jede Entity, die Pool-Maske
   (`@0x80042c8c lbu rec[1]; and a3`, Gegner = **Maske 2** laut `@0x80043720`), AUTO-Gate
   (`@0x80042ca0 andi 0x10 == a2`), Band-Gate (`@0x80042cac`) und Geometrie passiert:
   `g_actors[es].member_0b = (uint8_t)slot;` (`@0x80042fc4`).
   **LAST-WINS, kein `break`** (`@0x8004301c bne v0,zero,0x80042c50`).
   **Auch für `sce == 0`-Records** (ROOM1030 AOT 6 ist genau so ein Marker) — der Port filtert
   heute auf `sce == 5`; das muss auf das Flags-Bit umgestellt werden.

**Fasst NICHT an:** den Prop-Notch-Pfad, den Action-Scan (`a2 != 0`, dort gilt `sce != 0`
und der frühe Rücksprung `@0x80042fbc`), die `work_vars[0]/[1]`-Schreiber (`@0x80042f3c`/`@0x80042ee4`).

**Regressions-Gate:** ⚠️ **MITTEL–HOCH.** `Member_cmp(15,…)` kommt game-weit in ~37–68 RDTs
vor (ROOM1030/31 und 1040/41 je 42×, 11E0/11F0/10D0/1230 je ~11–12×). Heute liest jeder dieser
Vergleiche konstant 0 → `==0` ist spurios wahr, `==N>0` immer falsch. Der Stempel dreht beides
um und weckt schlafende Skript-Zweige. Erforderlich: S2 zuerst; `test_aot_sce_census`,
`test_aot_edge`, `test_flag_gate`, `probe_zreentry_10d0`, `probe_zlyer_10d0`,
`probe_marvin_10d0`, `probe_room1070_wake`, `unit_room1040_switch`; plus eine Zähl-Sonde je
betroffenem Raumpaar.

---

### 4.5 S5 — Kollisionsmaske aus `entity+0x1D7`

1. Aktorfeld `solid_mask` (= `+0x1D7`), Default **4** (`@0x80100828`).
   ⚠️ `re15_actor.h:360` belegt `+0x1D7` derzeit als `crow_bank`. **Kein Feld-Split nötig:**
   im Original ist `+0x1D7` ebenfalls typabhängig — die Krähe (`FUN_80112020`), der Hund, der
   Gorilla übergeben alle das **Literal 4** an `0x8003b0a4`; nur Zombie 0x16 und Zombie Girl
   0x13 lesen das Feld. Also nur umbenennen/dokumentieren, nicht aufteilen.
2. `re15_port/engine/src/re15_collision.c:611-618` — `re15_collision_constrain_enemy` bekommt
   die Maske als **Parameter**; **nur** die Zombie-/Zombie-Girl-Aufrufer übergeben
   `e->solid_mask` (`@0x8010062c`, `@0x8010aad0`), alle übrigen behalten das Literal `4u`.

**Fasst NICHT an:** `op_sca_id_set` (`scd_vm.c:3672-3679` ist byte-true), `u0`-Semantik,
den Spieler-Clamp (Literal 1).

**Regressions-Gate:** `test_collision_layers`, `test_enemy_body_push`, `unit_room1140_spawn`,
`probe_zreentry_10d0`, alle Krähen-Tests; Sonde: „für alle Nicht-Zombie-Typen ist
`solid_mask == 4`". Zusätzlich der Re-Entry-Test **1030 → 1040 → 1030**: `Sca_id_set` mutiert
den residenten RDT-Puffer in-place (`sca_entry_at` castet `const` weg) — `u0` von rgn2/idx6
muss nach jedem Load wieder 0xFF sein.

---

### 4.6 S6 — Sub-Modus 0x10 (Prone-Transition)

`enemy_ai_common.c:2577-2626`: `else if (e->sub_state_1 == 0x10)` → neuer Handler als
1:1-Port von `FUN_80104f80` (`0x8011f890[16]`, Decide `0x8011f840[16] = 0x80104f78` bleibt
bewusst ein leerer `jr ra`-Stub). Konstanten mit den Adressen aus §2.3(4).

**Zwei Fallen:**
* Phase 0 **fällt durch** in den anim-Block `@0x80105054` — eine dreiwegige
  `if(phase==0)…else if(phase==1)` verliert einen `anim_set`-Tick.
* Phase 2 macht `+0x9 = 0x81` (**Zuweisung**, `@0x801050d0/d4`), nicht `|= 0x80`.
* Clip 0x12 hat **98 Frames** (gemessen). Die im Port existierende „Faithful-line"-Abkürzung
  („phase 2→3 direkt") darf hier **nicht** wiederholt werden.

**Fasst NICHT an:** die Eintritts-Gates (`:333`, `:407` — bereits byte-true), die übrigen
Sub-Modi.

**Regressions-Gate:** heute unerreichbar (0x1000 wird nie gesetzt) → geringes Risiko, aber
`+0x9 = 0x81` steuert game-weit die Hurt-/Death-Clip-Umleitung (`enemy_ai_common.c:1659/3092/3126`):
`test_zombie_hit`, `probe_zombie_hit`, `test_enemy_ai`, `test_enemy_shootable`,
`probe_zreentry_10d0`.

---

### 4.7 S7 — Grid-Root 1 (Kriech-Brain)

`enemy_ai_common.c:2549`: neuer `case 1:` = Port von `FUN_80101708` —
Decide `0x8011f8e0` / Animate `0x8011f920`, Minimal-Umfang **Sub 0 und Sub 6**.
Sub 0 Animate = `FUN_801036dc` (Clip 0x1A, Speed 0x1E, Maske 8, Steer-Rate 0x10 **gegatet auf
`*(u32*)(+0x168) & 0x10000`**), Sub 0 Decide = `FUN_801035f8` (Prone-Grab + 0x2000-Ausstieg →
`0x601`), Sub 6 = derselbe 0x10-Handler aus S6.
**Sub-Modi 7..14 hart ausschließen** (Tabellen-Einträge sind `0x00000000`).

Der `0x2000`-Ausstieg (`@0x8010369c-c4`) liest zusätzlich `+0x1D8 & 0x80`. Welches Port-Feld
das ist, ist **nicht** verifiziert — solange nicht belegt, mit 0 modellieren und den Zustand
dokumentieren (nicht als byte-true verkaufen).
Hilfreich: `FUN_801035f8` ist im Port bereits als `re15_zgirl_overflow_row11`
(`enemy_ai_common.c:6765-6780`) vorhanden, inkl. `&0x2000 → 0x601`.

**Fasst NICHT an:** Grid-Root 0/2/5/6/7/8/9/10, `behavior 0x0d`.

**Regressions-Gate:** kein `Sce_em_set` erzeugt Nibble 1 direkt (entsteht nur als Commit von
`FUN_80104f80`) → kein bestehender Raum betroffen. `test_enemy_ai`, `test_zombie_hit`,
`probe_zreentry_10d0`, 115 ctests; Zielbild in der Sonde:
`grid 0x00 → (AOT-5) s1=0x10 → grid 0x81/s1=0 (Clip 0x1A, bewegt sich) → (AOT-4) s1=6 → grid 0x00/s1=2`.

---

### 4.8 Separat, NICHT Teil dieses Fixes

* **PRI-Vordergrundatlanten extrahieren** (`SldDecoder.findBlock/decompress` über alle 156
  BSS-Räume → `shared_assets/PSX/BSS/ROOM<id>/PRI<cut>.TIM`; für ROOM1030 entstehen genau 12
  Dateien, Cut 8 bleibt korrekt ohne). Kein Engine-Code. **Aber:** ROOM1030 Cut 0 enthält drei
  **quadratische** Masken (#41/#44/#48, `size_b=0x10` → 8×8); der Quadrat-Pfad
  `pri_common.c:113` trägt „⚠️ Untested at runtime" — er läuft damit zum ersten Mal live.
  Absicherung: byte-Vergleich der neu erzeugten gegen die 418 vorhandenen `PRI##.TIM`
  (müssen identisch sein), plus **gdigrab**-Verifikation (nicht Autoshot) in ROOM1030,
  ROOM1170, ROOM1090. Nebenbei: `render_pc.c:1600-1603` gibt beim Clear die alte Atlas-Textur
  nicht frei — beim Aktivieren mitfixen.
* **`RE15_ACTOR_MAX`** — **nicht** anheben. Für ROOM1030 durch S1 gegenstandslos; die
  Pool-Kapazität des Originals ist ungeklärt (siehe §6).

---

## 5. WAHRSCHEINLICH (nicht von mir re-verifiziert, aber von je einer Bahn belegt)

* **Kein Dekrement beim Töten.** Bahn A: Ghidra-XREFs auf `DAT_800b0ff4`/`DAT_800b0ff2` zeigen
  nur Init + `Sce_em_set` → das Limit gilt pro Raum-**Load**. Bahn D findet dagegen ein
  Dekrement `@0x80039af8-0x80039b10` (`lhu v1,4082; addiu v1,v1,-1; sh v1,4082`) im Despawn.
  **Widerspruch, für ROOM1030 folgenlos** (nichts despawnt im Einlauf), aber vor einem
  Dekrement-Einbau zu klären.
* **Die Staffelung ist eine „max 3 nördlich"-Regel** (`sub02 @0x21cc Cmp(work_var[7] < 4)`,
  gefüllt von `sub03`s Zählschleife über AOT 6). Statisch vollständig belegt, dynamisch nicht.
* **ROOM1040/1041 = dieselbe Maschinerie im großen Maßstab** (Cap 5, 40 Records, 40×
  `Member_copy`). Jeder Fix wirkt dort sofort mit; der Port kappt heute bei 15.
* **`sub00` vergibt genau sechs Positionen** (For(20) + Switch mit Cases 0..5). Vor der
  Cutscene byte-identisch zu den Spawn-Positionen der Records 0..5 (alle **hinter** dem Tor,
  z −25655..−30000), nach der Cutscene (`Ck(4,15)==1`) sechs Innenraum-Punkte
  (z −11381..−21599). Gemessen: der Port führt das bereits korrekt aus.
* **`+0x9f`** = 3. Parameter von `anim_set` (`FUN_8001f314`, nicht disassembliert) — als
  Abspielrichtung/Variante gedeutet, weil derselbe Clip 0x12 hin mit 1 und zurück mit 0 läuft.
* **`sub10`** (`Z += 3600` für alle mit Bit 0x1000) ist **totes Skript** — kein `Gosub`/
  `Evt_exec` in ROOM1030 zielt darauf. **Nicht implementieren.**

---

## 6. OFFEN — was fehlt, bevor Teile davon „fertig" heißen dürfen

1. **Symptom 2 ist im laufenden Port nie visuell reproduziert.** Alle Zahlen stammen aus
   Sonden. Der Port zeichnet in Cut 0 aktuell **9** Gegner, nicht „Dutzende". Ein
   **gdigrab**-Lauf (Skill `re15-port-visual-verify`, plain exe, echter Flow) vor und nach S1
   fehlt — Autoshot/Software-Render maskieren nachweislich.
2. **Keine Original-Referenz für ROOM1030.** `stage_saves/` enthält keinen sauberen
   ROOM1030-Savestate (`PATCHED-EXE_*` sind unzulässig). Die gesamte Kette ist statisch
   belegt, aber **nie live gegen das Original gemessen**. Nächster Schritt: Skill
   `re15-room-capture` → Savestate nach der Cutscene → `+0x9/+0x5/+0x94/+0x1D7/+0x1C4/+0x0B`
   aller Entities lesen.
3. **LOS-Latch-Adresse** (`+0x1d0` Bit 0) unbelegt — siehe S2.
4. **`+0x1D8` Bit 0x80** (Aufsteh-Gate) hat keine belegte Port-Entsprechung — siehe S7.
5. **Pool-Kapazität des Originals ungeklärt.** Drei widersprüchliche Rechnungen:
   „20 Slots" (Clear-Schleife `@0x8001a4c0 ori a0,zero,0x13`, 20 Iterationen über
   `0x800ACC2C` Stride `0x1F4`), „~24" (`0x800afbb4` ist ein echtes Fremd-Global — der
   AOT-Zähler, gelesen `@0x80042c30` — und liegt in Slot 24), „34/36" (arithmetisch falsch:
   `0x800b0fd0 − 0x800acc2c = 0x43A4 = 34,6` Slots). **Gleichzeitig** hat ROOM20B0 laut zwei
   Bahnen **35** Records mit Slots 0..34. Das ist ein ungelöster Widerspruch —
   `RE15_ACTOR_MAX` darf auf dieser Grundlage **nicht** verändert werden.
6. **Zensus fehlt:** wie viele `Sce_em_set`-Records game-weit `pc[1] & 0x80` bzw. `pc[5] != 0`
   setzen. Ohne ihn ist das inverse Spawn-Gate (`@0x800420fc` / `@0x80042154`) und der
   `0x2001`-Zweig (`@0x800422a4-b0`) nicht risikofrei nachziehbar (beide fehlen im Port,
   für ROOM1030 latent).
7. **Roh-Byte-Zensus über RDTs ist unbrauchbar.** Mehrere Bahnen haben mit Muster-Suchen
   (`44 xx yy 0d`, `3e 00 0f`) massiv kontaminierte Zahlen produziert (207/73 statt 142/12;
   „Slot 65"-Records in Textdaten). Alle solchen Zahlen in diesem Dossier sind bewusst nicht
   als Beleg zitiert. Ein sauberer Zensus muss instruktions-aligned aus `main00`/den Subs
   laufen, mit Abbruch bei ungültigem Opcode.
8. **Sichtbarkeits-Frage.** Belegt ist die **Zahl** 6. Ob der Nutzer „sechs gleichzeitig im
   Bild" meint, ist offen — ROOM1030 hat 13 Cuts, die sechs stehen vor der Cutscene bei
   z −25655..−30000, danach bei z −11381..−21599.

---

## 7. Widerlegte Hypothesen — nicht erneut verfolgen

| Hypothese | Status | Grund |
|---|---|---|
| `behavior 0x0d` ist die Einlauf-/Kriech-Maschine | **WIDERLEGT** | `@0x80100f24-54`: Einmal-Selektor, löscht `+0x9` selbst; Port bereits byte-true |
| Tabellen-Überlauf `0x8011f80c[13]` | **gegenstandslos** | Index 13 unerreichbar, weil `+0x9` vorher 0 wird |
| `RE15_ACTOR_MAX` erklärt „zu viele" | **falsche Richtung** | Port hat 15 statt 20 — Anheben verschlimmert |
| Fehlende PRI-Masken verdecken 14 Zombies | **WIDERLEGT** | Gemessen: 1 von 10 voll verdeckt; nur 10 der 20 sind überhaupt im Cut-0-Quad (Doppelzählung mit dem Region-Cull) |
| Region-Quad-Cull / Etagen- / Distanz-Gate | **WIDERLEGT** | 13 Cuts × 20 Positionen: null Divergenz; im Original existiert kein solches Gate |
| sub01-Reseed spawnt nach | **WIDERLEGT** | Aktorzahl über 1200 Ticks konstant; `Sce_em_set` nur in `main00` |
| Opcode 0x53 ist als Fade fehl-gemappt | **WIDERLEGT** | Nur der **Name** ist veraltet; Rumpf `scd_vm.c:3775ff` ist byte-true `Work_set`-by-var |
| em-Status-Bank-Widerspruch `0x800b48bc/dc` | **falscher Alarm** | `@0x800420b4 addiu a3,a2,18620` mit `a2 = 0x800ac77c` → **0x800B1038** / **0x800B1058** = genau das, was der Port zitiert |
| „erste Treffer-AOT-Zone gewinnt" | **WIDERLEGT** | AUTO-Pfad setzt fort (`@0x8004301c`) → **LAST-WINS** |
| `sub10` (`Z += 3600`) ist die Tor-Durchquerung | **WIDERLEGT** | totes Skript, kein Aufrufer |

---

## 8. Guess-Tells / Verdachtsstellen im berührten Port-Code (Meldung, kein Auftrag)

* `re15_actor.h:51` — `member_0b … "stored, no consumer yet"` → **stale/falsch**, zwei Konsumenten.
* `enemy_ai_common.c:4217` — `e->anim_flags = 0x04; /* PORT-SHIM (kein Original-Write) */` →
  beschreibt genau den Kommando-Kanal `+0x1C4`; kollidiert mit den Bits 0x1000/0x2000.
  Live gemessen: alle ROOM1030-Zombies tragen nach 200/1500 Ticks `anim_flags = 0x0004`.
* `enemy_ai_common.c` (~`:1120`) — „Faithful-line: … the port advances 2→3 directly" →
  dieselbe Abkürzung würde in S6 das 98-Frame-Hinlegen verschlucken.
* `enemy_ai_common.c:2650` — „no `Sce_em_set` in the whole game uses low nibble 9/0xA" →
  unverifiziert (Roh-Byte-Zensus untauglich).
* `scd_vm.c` `op_sce_em_set` — „hp=100 is harmless nominal init"; `op_sce_fade_set` —
  „sel==5 … left unbound (faithful residual)".
* `re15_collision.c:578-579` — `push_caps8/9` „⚠️ best-effort, unverified" (ROOM1030 hat nur
  `type=1`, aber der Clamp-Umbau in S5 läuft durch dieselbe Funktion).
* `pri_common.c:109-113` — Quadrat-Masken „⚠️ Untested at runtime".
* `camera_common.c:47-51` — H-Clamp `if (H > 208) H -= 128` explizit unbelegt.
* `render_pc.c:1600-1603` — `set_pri_atlas(NULL)` gibt die alte Textur nicht frei.
* `CLAUDE.md` Build&Run — `re15_pc.exe --headless` existiert nicht mehr: `main.c:1642`
  `(void)argc; (void)argv;`, stderr geht per `freopen` nach `debug.log`.

---

## 9. Artefakte

Sonden (alle **Mess**-Sonden, kein Engine-/Plattform-Code verändert):
`re15_port/tests/unit/probe_lanea_1030_cap.c`, `probe_laneb_1030_horde.c`,
`probe_adv_1030_verify.c`, `probe_gate_1030.c`, `probe_laned_1030_spawn.c`,
`probe_laneE_1030_visible.c`.

⚠️ **Sonden-Lehre (aus dieser Runde, teuer bezahlt):** eine Sonde, die `g_room_rdt` /
`g_room_rdt_ok` nicht publiziert, hat den Gegner-SCA-Clamp und den LOS-Ray **stumm** und
meldete das exakte Gegenteil („die Zombies laufen durch das Tor", 12 im Raum) — mit den zwei
Zeilen kehrt sich das Ergebnis um (200 Pushes, 0 im Raum). Ebenso: ein 3200-Einheiten-
Einzelschritt **tunnelt** durch die nur 1649 tiefe Torzelle und misst nichts; realistische
60-Einheiten-Schritte verwenden. Und: eine Messung, die den ELSE-Zweig von `sub00` trifft (wo
Ziel- und Spawn-Positionen zufällig identisch sind), ist **nicht diskriminierend**.
Vor jeder Schlussfolgerung prüfen, ob der Aufbau den LIVE-Zustand reproduziert.
