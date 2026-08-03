# ROOM10D0 — Liegend-Zombie (behavior 0x0e) steht nicht auf: RE-Dossier (2026-08-03)

**Nutzer-Report:** "Der Zombie soll am Anfang liegen, sich dann nach vorne beugen und
aufstehen. Bei uns bleibt er nur nach vorne gebeugt liegen."

**Ergebnis in einem Satz:** behavior 0x0e ist im Original KEIN Pose-only-Spawn, sondern
schaltet den Zombie in die **SLEEPING-LYING-Submaschine** (`word +0x4 = 0x1201` → State 1,
`+0x5=0x12`, und `+0x9=0`); der Port behandelt sel 0x0e als "pose-only" (Decoder-Lücke),
lässt `grid_id=0x0e` stehen → der AI-Dispatch landet für immer in `default: break`, die
(im Port VORHANDENE, byte-true portierte) Sleeping-Maschine wird nie erreicht, und der
globale Anim-Advancer spielt den Liege-Clip 0x2A einmal durch und hält den LETZTEN Frame
= die nach-vorne-gebeugte Endpose statt flach-liegend Frame 0.

Dynamisch reproduziert mit `re15_port/tests/unit/probe_zlyer_10d0.c` (Diagnose-Probe,
kein ctest; Log unten wörtlich).

---

## 1. MESSEN — Spawn-Fakten + Port-Verhalten

### 1.1 RDT-Spawn (ROOM10D0.RDT, sub_scd @0x120c)

- Datei 0x126a (sub00 +0x5e): `44 01 10 0e …` = **Sce_em_set slot=1 type=0x10
  behavior=0x0e pos=(5878,0,25694)** — der liegende Zombie neben Marvins Gesprächsposition.
- Game-weiter Scope (SCD-Sektions-gegateter Scan aller 240 RDTs auf
  `44 ?? <zombie-type> 0e`): behavior 0x0e wird NUR hier benutzt —
  **ROOM10D0 (slot 1) + ROOM10D1 (slot 0**, die Alternativ-Spieler-Variante,
  Datei 0x1254, identische Position). Der Fix betriffe also genau diese Szene.

### 1.2 Port-Messung (probe_zlyer_10d0.exe, wörtlich)

```
EM010 bank: 43 clips, fc[0x29]=59 fc[0x2a]=28
  slot=2 type=0x10 grid=0x0E st=0 s1=0x00 s2=0 mo=42 fr=0 pos=(5878,0,25694)
== Phase A: Spieler fern (soll: liegend, Frame haelt 0) ==
    0 ZOMB  st=1 s1=0x00 s2=0 grid=0x0e mo=42  fr=1   dist=14372
   60   fr  st=1 s1=0x00 s2=0 grid=0x0e mo=42  fr=27  dist=14372   <- Clip 0x2A (28 Frames)
                                                                      durchgespielt, HOLD-LAST
== Phase B: Spieler nah (dist=1693 < 0xBB8) — 600 Ticks ==
  480   fr  st=1 s1=0x00 s2=0 grid=0x0e mo=42  fr=27  dist=1693    <- KEINE Reaktion
H1 grid nach INIT (soll 0x00, +0x5=0x12): grid=0x0e s1=0x00 -> DECODER-LUECKE REPRODUZIERT
H2 Wake (s2>=2 im Sub 0x12): NIE; Standup 0x0d: NIE; Engage: NIE
H3 Clip 0x2A Frame: fr=27 (= letzter Frame = nach vorne gebeugt)
```

Beides exakt das Nutzer-Symptom: er "beugt sich nach vorne" (Clip 0x2A läuft ab Spawn
durch, weil der Advancer ihn nicht pinnt) und **bleibt** so (Hold-Last auf fr=27,
kein Wake, egal wie nah der Spieler kommt).

---

## 2. ORIGINAL — die komplette Aufwach-Kette (Disasm-Belege)

### 2.1 INIT-Spawn-Decoder: sel 0x0E ist NICHT pose-only (@0x80100f64-fd4)

Der Decoder liegt IN `FUN_80100688` (INIT; Branch @0x80100904 `beq v0,zero,0x80100c20`
wenn `DAT_800aca3c & 1 == 0` — der Normalfall im Prototyp). Raw (STAGE1.BIN):

```
80100f64 lbu v0,9(a0)        ; +0x9 (behavior)
80100f68 ori v1,zero,0xe
80100f6c andi v0,v0,0x1f
80100f70 bne v0,v1,0x80100fd8
80100f74 ori v0,zero,0x2a
80100f78 sb  v0,148(a0)      ; +0x94 = Clip 0x2A (Liegepose, Frame 0 = flach)
80100f88 sb  zero,149(v0)    ; +0x95 = 0
80100f98 sb  zero,143(v0)    ; +0x8f = 0 (harter Pose-Cut)
80100fb0 jal 0x8001f314      ; anim_set(+0x170,+0x174, a2=0, a3=0x200)
80100fc0 ori v0,zero,0x1201
80100fc4 sw  v0,4(v1)        ; word +0x4 = 0x00001201 -> State 1, +0x5=0x12, +0x6=0, +0x7=0
80100fd4 sb  zero,9(v0)      ; +0x9 = 0  <- BEHAVIOR-BYTE KOMPLETT GELÖSCHT
```

→ Nach dem INIT hat der Zombie Nibble 0 (Standard-Dispatch `@0x8011f80c[0]` =
FUN_8010168c → decide/animate über `+0x5`), und `+0x5 = 0x12` = **SLEEPING-LYING**
(decide `@0x8011f840[0x12]` = 0x80105470, animate `@0x8011f890[0x12]` = 0x801054f4;
beide Tabellen aus STAGE1.BIN gedumpt, 20/20 Einträge). Zum Vergleich die
Nachbar-Selektoren im selben Block: sel 6 → `word=0x20c01` (Feeding) @0x80100e64,
sel 0xB → NUR Pose (Clip 3, kein word-Write) @0x80100ec4-f10, sel 0xD → `word=0x201` +
`+0x9=0` @0x80100f40-54.

### 2.2 Liegepose-Halten + Wake-Trigger: decide 0x80105470

```
8010547c lbu v0,9(a0) ; andi 0x1f ; bne v0,0xf -> 801054ac   ; SCRIPT-WAKE:
80105490 lbu v1,6(a0) ; bne v1,1  -> 801054ac                ;   (+0x9&0x1f)==0xF && +0x6==1
801054a0 sb  2,6(a0)                                         ;   -> +0x6 = 2
801054b0 lw  v0,464(a0)          ; +0x1d0 = Dist-Cache (Per-Frame-Tick FUN_80100424)
801054b8 sltiu v0,v0,0xbb8       ; dist < 0xBB8 (3000)?
801054bc beq zero -> ret
801054c8 lh  v0,player.hp        ; 0x800acaee (+0x9a)
801054d0 bltz -> ret             ; Spieler lebt?
801054d8 lbu v1,6(a0) ; bne v1,1 -> ret
801054e8 sb  2,6(a0)             ; PROXIMITY-WAKE: +0x6 1 -> 2
```

**Der Aufwach-Trigger ist reine SPIELER-NÄHE: dist < 3000 && player.hp >= 0** (nur aus
Warte-Phase `+0x6==1`). Alternativ ein Script-Wake über Behavior-Nibble 0xF — in
ROOM10D0s SCD existiert aber kein `Member_set(12,0x0f)` (Byte-Scan 0x34/0x35 über die
ganze SCD-Region: 0 Treffer) → in dieser Szene zählt nur die Nähe. Kein Schaden, keine
Sicht, kein Timer nötig.

### 2.3 Die Phasen-Maschine: animate FUN_801054f4 (Decompile prolog+case-verifiziert, Raw-Zitate)

Switch über `+0x6` (`sltiu v1,0x5`, Sprungtabelle @0x801000e4):

```
case 0 @0x80105534-50: sb 1,440(v0)  ; +0x1b8 = 1 (Liege-Latch)
                       sb 1,6(v0)    ; +0x6 = 1 -> WARTEN
case 1: NICHT im Switch (default) -> KEIN f314-Call -> +0x95 bleibt 0
        = DAS Liegepose-Halten: Clip 0x2A, Frame 0, flach auf dem Boden
case 2 @0x80105554-80: +0x94=0x2A, +0x95=0, +0x6=3, +0x8f=0   ; fällt in case 3
case 3 @0x80105584-cc: jal 0x8001f314 @0x801055a8              ; anim_set(+0x170,+0x174,
                       ori a3,0x200                            ;   a2=0, rate 0x200)
                       +0x6 += done   ; Clip 0x2A spielt DURCH = "nach vorne beugen"
case 4 @0x801055d0-608: sw 0xd01,4(v1)  ; word +0x4 = 0xd01 -> +0x5=0xD (STANDUP)
                        sb zero,440     ; +0x1b8 = 0
                        andi +0x93,0xfe ; Hit-Guard-Clear
```

### 2.4 Aufstehen: Standup-Sub [0xD] (decide 0x80104a48 = `jr ra`-Stub; animate 0x80104a50)

```
80104a9c sb  v0,6(a0)   ; Phase 0: +0x6 = 1
80104aa8 ori v0,zero,0x29 ; 80104aac sb v0,148(v1)   ; Clip 0x29 = AUFSTEHEN
80104abc sb  zero,149(v0)                            ; +0x95 = 0
80104ac8 ori v0,zero,0x7 ; 80104ad0 sb v0,143(v1)    ; +0x8f = 7 (Crossfade)
80104acc jal rng ; andi 3 ; bne -> skip
80104ae0 jal 0x800453d0 ; ori a0,zero,0x5            ; 1/4-Chance SE 5 (Ächzen)
80104afc jal 0x8001f314 ; ori a3,zero,0x200          ; play (a2=0, +0x170/+0x174)
80104b18 addu v1,v1,v0 ; sb v1,6(a0)                 ; +0x6 += done
80104a8c beq v1,2 -> 80104b24 ; ori v0,zero,0x201
80104b24 sw  v0,4(a0)                                ; Phase 2: word = 0x201 -> ENGAGE
```

### 2.5 Die Clip-Sequenz ("liegen → nach vorne beugen → aufstehen")

EM010-Bank (CDEMD0.EMS, 43 Clips): **Clip 0x2A = 28 Frames** (Liege-/Aufricht-Anim:
Frame 0 = flach liegend, Ende = nach vorne gebeugt sitzend), **Clip 0x29 = 59 Frames**
(Aufstehen; derselbe Clip, den Knien-Feeding rückwärts spielt, vgl.
`enemy_ai_common.c:1040`). Reihenfolge im Original:

| Phase | State | Clip | Frames | Sichtbar |
|---|---|---|---|---|
| Schlafen | 1/0x12/+0x6=1 | 0x2A **gehalten auf Frame 0** (kein f314-Call) | ∞ | flach liegend |
| Wake (dist<3000) | 1/0x12/+0x6=2→3 | 0x2A ab Frame 0 durchgespielt (rate 0x200) | 28 | beugt sich nach vorne / richtet sich auf |
| Aufstehen | 1/0x0D | 0x29 einmal (+0x8f=7, 1/4-SE 5) | 59 | steht auf |
| Danach | 1/0x02 | ENGAGE (word=0x201) | — | normaler Zombie |

### 2.6 Abgrenzung: NICHT die ROOM1140-Wake-Maschine

Die 1140-Lyer-Maschine `FUN_80103a58` (Memory reai-v2-zombie-hit, dossier
`zombie_hit_1140.md` §1.8) gilt hier **nicht**: sie hängt am Grid-Nibble-Dispatch
`@0x8011f80c[7]/[8]` (behavior 0x88, Typ 0x16) und re-armiert `+0x93|=1` jeden Tick
(unschießbarer Ambusher, Wake-Trigger dort weiter OFFEN). behavior 0x0e ist ein
ANDERER, vollständig RE'ter Lyer-Pfad: `+0x9` wird beim INIT gelöscht, die Maschine
lebt als `+0x5=0x12`-Sub im Standard-Dispatch, der Wake ist Distanz+Spieler-lebt, und
es gibt KEINEN per-Tick-Unverwundbarkeits-Guard (nur den `+0x93&=0xfe`-Clear beim
Übergang zu 0xD @0x80105604).

Randnotiz zur Tabelle @0x8011f80c: sie hat 16 Slots, `[13..15]` ALIASEN in die
decide-Tabelle (`@0x8011f840 == &@0x8011f80c[13]`, Dump oben). Nibble 0xE würde
0x80101de4 (decide[1]=Wander) treffen — im Original UNERREICHBAR, weil sel 0xE das
Nibble beim INIT löscht. Der Port-`default: break` für Nibble 0xE ist also nicht der
eigentliche Fehler — der Fehler liegt eine Ebene früher im Decoder.

---

## 3. PORT-IST — drei Befunde (statisch + dynamisch belegt)

### D1 (Ursache) — Spawn-Decoder lässt sel 0x0E aus (`enemy_ai_common.c:975-992`)

`re15_enemy_ai_live_init` behandelt sel 6 (→0x0c/2/grid=0) und sel 0x0D (→0x02/grid=0);
der Kommentar :990-991 behauptet "sel 8 / 0xb / 0xe: pose-only in the decoder → +0x5
stays 0, grid unchanged". **Für 8 (RAM-verifiziert, 1140) und 0xB (@0x80100ec4-f10:
kein word-Write) stimmt das — für 0xE ist es durch @0x80100f64-fd4 widerlegt**
(word=0x1201 + `+0x9=0`). Probe: `grid=0x0e s1=0x00` nach INIT.

### D2 (Folge) — der Zombie erreicht seine (vorhandene!) Sleeping-Maschine nie

Der AI-Dispatch `switch (e->grid_id & 0xf)` (`enemy_ai_common.c:2493`) hat für 0xE nur
`default: break` (:2606). Die byte-true portierte Maschine ist komplett da —
decide-case 0x12 :407-412 (Wake `(grid&0x1f)==0xf || dist<0xbb8 && player alive`, exakt
§2.2), animate `re15_enemy_ai_live_sleeping` :1647-1667 (Phasen exakt §2.3),
`re15_enemy_ai_standup_animate` :1066-1087 (Clip 0x29 + 1/4-SE-5 + word 0x201, exakt
§2.4) — aber sie hängt am `case 0`-Pfad (:2535-2537), der `grid_id==0` voraussetzt.
Probe Phase B: 600 Ticks bei dist=1693 — kein Wake, kein Standup, kein Engage.

### D3 (Sichtbarkeit) — der globale Advancer spielt den Liege-Clip durch statt Frame 0 zu halten

`player_common.c re15_actors_anim_advance` :716-726 pinnt nur die Bit-0x80-Spawn-Posen
(Clips 0x0C/0x0E/0x12/0x13) auf Frame 0; Clip 0x2A ist nicht dabei → :748-756 advanct
ihn und Play-once-Hold pinnt den LETZTEN Frame (fc=28 → fr=27). Original: in den
Sleeping-Phasen 0/1 existiert schlicht KEIN f314-Call (§2.3, case 1 = default ohne
Body) — der einzige Frame-Stepper des Originals läuft nicht, `+0x95` bleibt 0.
Probe Phase A: fr klettert 1→27 in den ersten 27 Ticks und hält = "nach vorne gebeugt".
**Auch nach dem D1/D2-Fix bräuchte die Schlaf-Wartephase diesen Halt**, sonst beugt er
sich weiter sofort beim Spawn.

---

## 4. FIX-PLAN (mit @0x-Zitaten; kein Engine-Code in dieser Diagnose geändert)

1. **Decoder-Zweig sel 0x0E** (`enemy_ai_common.c` nach :980, analog sel 6/0xD):
   ```c
   } else if (sel == 0x0e) {   /* @0x80100f64-fd4: +0x94=0x2A/+0x95=0/+0x8f=0,
                                * word +0x4=0x00001201 -> State1/+0x5=0x12 SLEEPING-LYING,
                                * +0x9=0 (Behavior-Byte geloescht) */
       e->sub_state_1 = 0x12; e->sub_state_2 = 0; e->sub_state_3 = 0;
       e->grid_id = 0; e->motion = 0x2a; e->anim_frame = 0; e->anim_frac = 0;
   }
   ```
   und den :990-991-Kommentar korrigieren (0xE aus der pose-only-Liste streichen).
2. **Schlaf-Frame-Halt** (`player_common.c` neben dem Getup-Release :725, STATE-gegatet
   wie dort dokumentiert "The PSX distinguishes by STATE, not clip"):
   ```c
   /* SLEEPING-LYING Wartephase: FUN_801054f4 hat in +0x6=0/1 KEINEN f314-Call
    * (case 1 fehlt im Switch @0x801000e4; erster f314 erst case 3 @0x801055a8) —
    * der Original-Frame-Stepper laeuft nicht, +0x95 bleibt 0. */
   if (a->state == 1 && a->sub_state_1 == 0x12 && a->sub_state_2 < 3 && mo == 0x2A)
       { a->anim_frame = 0; continue; }
   ```
   (`< 3`: Phase 2 setzt selbst Frame 0 und fällt nach 3; ab 3 muss der Clip laufen.
   State-Gate nötig, weil `+0x5=0x12` im HURT-State 2 die WAFFEN-Klasse ist.)
3. Optional (Konvention, kosmetisch): `anim_blend_rate = 0x200` in sleeping-case 2 und
   standup (f314 a3=0x200 @0x801055ac/@0x80104b00) — derzeit ungesetzt; der
   Port-Advancer wertet die Rate ohnehin nicht aus (siehe §5.3).

**Verifikation danach:** `probe_zlyer_10d0.exe` — erwartet: Phase A `grid=0x00
s1=0x12 s2=1 fr=0` konstant; Phase B Wake im ersten Tick (dist 1693<3000): s2 1→2→3,
fr 0..27 (Clip 0x2A), dann `s1=0x0d` + Clip 0x29 (59 Frames), dann `s1=0x02` ENGAGE.
Danach Live-Verifikation per gdigrab (Skill re15-port-visual-verify) im echten Flow.

---

## 5. OFFEN (ehrlich)

1. **Schießbarkeit im Schlaf:** Da `+0x9&0x80==0`, pflegt FUN_80101224 das LEVEL-Band —
   formal wäre der Schläfer mit LEVEL-Aim treffbar (und ein Treffer erzwänge per
   FUN_80011f50 `+0x4=2` → Standing-HURT-Router = implizites "Aufwachen durch
   Schaden"). Ob der Waffen-Hit-Tester das flach liegende Modell in Y überhaupt
   schneidet, ist NICHT gemessen (weder PSX noch Port). Nächster Schritt bei Bedarf:
   DuckStation-Savestate in 10D0 + Schuss auf den Schläfer.
2. **`+0x1b8`-Semantik (Liege-Latch):** dieselbe Offset-Adresse ist beim Plc_neck-Pfad
   das Neck-Flag-Byte (marvin_10d0.md §2.5). Wert 1 im Schlaf / 0 beim Standup —
   Konsument für Enemies nicht RE'd; der Port hat keinen (dokumentiert, wie schon beim
   1140-Lyer und Feeding).
3. **f314-Rate a3 (0x100/0x200/0x400)** wird vom Port-Advancer nicht modelliert
   (flat 1 Frame/Tick) — port-weite bestehende Konvention, nicht Teil dieses Bugs.
4. **Script-Wake Nibble 0xF:** in 10D0 kein `Member_set(12,0xf)` gefunden (Byte-Scan
   Opcodes 0x34/0x35 über die SCD-Region); andere Schreibwege (Member-RMW 0x5A/0x5B)
   nicht geprüft. Der Port-decide :409-410 implementiert den Check bereits.
5. **ROOM10D1** (Alternativ-Variante, slot 0, gleiche Position) nicht dynamisch
   geprobt — derselbe Decoder-Pfad, derselbe Fix.

## 6. Artefakte

- Probe: `re15_port/tests/unit/probe_zlyer_10d0.c` (+ CMake-Eintrag, kein ctest).
- Disasm: `re15_disasm.py` gegen STAGE1.BIN (INIT-Decoder, decide/animate 0x12,
  Standup 0xD, Tabellen @0x8011f80c/@0x8011f840/@0x8011f890).
- Scope-Scan: SCD-gegateter RDT-Scan (Session-Skript; Ergebnis §1.1).
