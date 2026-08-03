# ROOM10D0 — Marvins SPAWN-/START-Animation (Nutzer-Report #3): RE-Dossier (2026-08-03)

**Nutzer-Report:** "Marvins Spawn + Start-Animation in ROOM10D0 sind IMMER NOCH falsch"
(dritter Report; die Fixes 18563cd3 Cutscene-Choreo + 3f5310cc Walk-Advance haben den
SZENEN-Teil adressiert — was beim Betreten des Raums VOR der Szene passiert, blieb falsch).

**Ergebnis in einem Satz:** Die Spawn-STATE-Maschine des Ports ist byte-true (INIT →
State 4 / Sub 6, Clip 1 einmal → Clip-2-Loop, Position/Yaw/Neck korrekt) — aber sie spielt
die **falsche BANK**: der Original-Executor-Kanal `+0x170/+0x174` ist per Kanal-Loader
`FUN_80022300` POSITIONS-fest das EMD-Directory-Paar **dir[4]/dir[3] (= "Bank 1", bei
EM040 14 Clips)**, der Port wählt in `re15_emd_parse_container` stattdessen die Bank mit
den **meisten Clips** ("largest bank" = dir[1] "Bank 0", 24 Clips). Marvin steht deshalb
mit den falschen Animationen da: Clip 1 mit 32 statt 16 Frames, Clip 2 mit 50 statt 52 —
komplett andere Posen. Savestate-Ground-Truth + Port-Probe decken sich exakt mit dieser
Diagnose.

---

## 1. MESSEN — Original-Ground-Truth (DuckStation-Savestate)

Savestate `r10d0_walk1.sav` (Scratchpad des Vorgänger-Laufs; ROOM10D0, Erst-Betretung,
VOR der Cutscene — Marvin auf Spawn-Position, Liegend-Zombie in +0x5=0x12 daneben).
Gelesen mit `re15_ss.py` (RAM-Base 0x31a62):

```
enemy[0] @0x800acc2c (MARVIN):
  +0x0  flags   = 0x40000001        +0x8/+0x9 = 0x40 / 0x40 (Typ / behavior erhalten)
  +0x4  word    = 0x00030604        -> state 4, sub 6, PHASE +0x6 = 3
  +0x94 clip    = 2                 +0x95 frame = 39   +0x8f = 0
  +0x1c4        = 0x0020            (Executor-Halbtakt-Toggle @0x80050c50)
  pos           = (5775, 0, 23625)  yaw +0x6a = 1142   (== Sce_em_set-Spawn)
  +0x9a hp      = -1                +0x9e = 0x78
  +0x1b8 neck   = 0x12  +0x1b9 = 8  +0x1a8 = 0x800aca54 (Player)
  KANÄLE:
  +0x84  = 0x801359e8   +0x16c = 0x801353d4     ("Bank 0")
  +0x170 = 0x8013d040   +0x174 = 0x8013cc58     ("Bank 1")  <- die IDLE-Maschine
  +0x178 = 0x80140c9c   +0x17c = 0x80140c98     ("Bank 2", leer)
  +0x180 = 0x801294b4   +0x184 = 0x8012e5ac     (RBJ-Kanal; u32@+0x180-4 = 2 = Marker REC1)
```

**Kanal-Identifikation (byte-genau):** die 48 Bytes @0x8013cc58 finden sich in
`CDEMD0.EMS` bei Datei-Offset **0x2d5090**; der EMS-Walker (identisch `re15_ems.c`)
ordnet das **Blob 18 (= Typ 0x40)**, Blob-Basis 0x2cd800, zu → rel. 0x7890 = **dir[3]**.
Analog: +0x170 → 0x2d5478 = dir[4]; +0x84 → Blob+0x620 = dir[2]; +0x16c → Blob+0xc =
dir[1]. Der RBJ-Kanal +0x180/+0x184 zeigt in die RDT-animation-Sektion (Marker-Wort 2 =
REC1) — **die eigene Bank +0x170 bleibt daneben bestehen** (zwei getrennte Kanäle).

**EM040-Cliptabellen (aus den EDD-Bytes dekodiert):**

| Bank | Directory | Clips | Frame-Zahlen |
|---|---|---|---|
| Bank 0 | dir[1] EDD @Blob+0xc, dir[2] EMR @+0x620 | 24 | **34, 32, 50**, 26, 20, 20, 50, 1, 1, 1, 1, 25, … |
| **Bank 1** | dir[3] EDD @Blob+0x7890 (Datei 0x2d5090), dir[4] EMR @+0x7c78 | **14** | **22, 16, 52**, 1, 50, 30, 10, 16, 1, 17, 1, 17, 1, 1 |
| Bank 2 | dir[5]/dir[6] @+0xb8d0/+0xb8d4 | 0 (leer) | — |

Ground-Truth-Konsistenz: gemessen Clip 2 @ Frame **39** — passt NUR zu Bank 1
(Clip 2 = 52 Frames); Bank 0 Clip 2 hat 50, der RBJ-REC1-Clip 2 nur 30 Frames.

**Szenen-Start:** Der AOT-Trigger (slot 0x12, Zone x=4700..5500 / z=19400..20200,
Event `ff 18 15` → sub21) liegt MITTEN im Raum — die Szene feuert NICHT beim Betreten
(Tür-Spawn 7650/11400), sondern erst, wenn Leon auf Marvin zuläuft. Der Savestate zeigt
genau diesen Schwebezustand: Marvin loopt seit längerem den Bank-1-Idle (Frame 39).

## 2. ORIGINAL — die steuernden Mechanismen (Disasm-Belege)

### 2.1 Kanal-Loader FUN_80022300 — Kanäle NACH DIRECTORY-POSITION, keine Größen-Heuristik

Aufruf im Sce_em_set-Spawn-Pfad (`jal 0x80022300` @0x80042328). Raw (PSX.EXE):

```
80022488 sw v0,432(s2)   ; +0x1b0 = base + dir[0]
80022498 sw v0,364(s2)   ; +0x16c = base + dir[1]   (Bank-0-EDD)
800224a8 sw v0,132(s2)   ; +0x84  = base + dir[2]   (Bank-0-EMR)
800224b8 sw v0,372(s2)   ; +0x174 = base + dir[3]   (Bank-1-EDD)  ★
800224c8 sw v0,368(s2)   ; +0x170 = base + dir[4]   (Bank-1-EMR)  ★
800224d8 sw v0,380(s2)   ; +0x17c = base + dir[5]   (Bank-2-EDD)
   (folgend)             ; +0x178 = base + dir[6],  +0x10 = dir[7] MD1
```

(Decompile `RE_15_Quellcode_V2/FUN_80022300.c` Z.31-39 identisch. Die `+0x97==1/2`-
Sonderpfade greifen bei Marvin nicht: Savestate +0x97 = 0.)

### 2.2 NPC-0x40-INIT FUN_8011c6dc (STAGE1.BIN, raw verifiziert)

```
8011c6f0 sb 1,4(v1)            ; state 1
8011c738 sw &player -> +0x1a8  ; Blickziel
8011c748 sh -1 -> +0x9a        ; HP -1
8011c7bc/c0 ori v0,2; sb v0,148(v1)   ; +0x94 = CLIP 2  (Idle-Vorwahl)
8011c7d0 sb 0 -> +0x95 ; 8011c7e0 sb 0 -> +0x8f ; 8011c7f0 sb 0 -> +0x93
8011c800 lw a0,132(v0)         ; +0x84   ← EIN f314-Schritt auf dem BANK-0-Kanal
8011c804 lw a1,364(v0)         ; +0x16c    (nur dieser eine INIT-Frame)
8011c808 jal 0x8001f314        ; anim_set, a3=0x200
8011c84c lbu v0,9(v1); andi 0x40; beq -> 8011c874
8011c860 sb 4,4(v1)            ; behavior&0x40 -> STATE 4
   (folgend) +0x5 = 6          ; Executor-Sub 6
```

### 2.3 Sub 6 EVENT-REACH @0x800517f0 — Clip 1 einmal, Clip-2-Loop, Kanal +0x170/+0x174

```
80051800 lbu v1,6(a0)          ; Phase +0x6
80051844 sb 1,6(a0); 80051854 sb 1,148(v0)   ; Phase 0: +0x6=1, CLIP 1, +0x95=0
80051874 sb 7,143(v1)                        ;   +0x8f = 7 (Crossfade)
80051884 lw a0,368(v0); 80051888 lw a1,372(v0)  ; ★ +0x170/+0x174 = BANK 1
8005188c jal 0x8001f314 ; a3=0x200           ; Phase 1: play; done -> +0x6=2
800518b4 sb 3,6; 800518c8 sb 2,148           ; Phase 2: +0x6=3, CLIP 2, +0x8f=7
800518dc lw a0,368(v0); 800518ec lw a1,372(v0); jal f314   ; Phase 3: Clip-2-LOOP
```

→ SOLL beim Spawn: **Bank-1-Clip 1 (16 Frames) einmal, dann Bank-1-Clip 2 (52 Frames)
als Dauer-Idle** — exakt der Savestate-Zustand (Phase 3, Clip 2, Frame 39).

### 2.4 Kanal-Landkarte der Executor-Subs (alle raw disasm-verifiziert)

| Sub | Adresse | f314-Kanal | Bedeutung |
|---|---|---|---|
| 0 | 0x80050cb8 | **+0x180/+0x184** (@0x80050d40/48) | RBJ-Binder-Kanal (Cutscene-Gesten) |
| 1 | 0x80050ddc | **+0x84/+0x16c** (@0x80050e64/6c) | Bank 0 (dir[2]/dir[1]) |
| 2 | 0x80050f00 | **+0x170/+0x174** (@0x80050f88/90) | Bank 1 |
| 3 | 0x80051024 | **+0x178/+0x17c** (@0x800510ac/b4) | Bank 2 (Victim) |
| 4/5/7/8 Walk | 0x80051148 | **+0x170/+0x174** (@0x800512bc/c0, @0x80051358/5c) | Bank 1 |
| 6 Event-Reach | 0x800517f0 | **+0x170/+0x174** (@0x80051884/88) | Bank 1 |
| 9 Turn | 0x80051cf8 | **+0x170/+0x174** (@0x80051e9c/ea0) | Bank 1 |

→ ALLES außer Sub 0/1/3 spielt die Bank 1. Der Cutscene-Auftakt `Plc_motion(2,6,0)`
(sub21 +0x082a) = Sub 2 = **Bank-1-Clip 6 (10 Frames)**, und der Szenen-Walk-Clip 5 =
**Bank-1-Clip 5 (30 Frames)** — nicht Bank 0 (50 bzw. 20 Frames).

### 2.5 EMS-Census: wo die "largest bank"-Heuristik vom Original abweicht

Alle 29 Blobs von CDEMD0.EMS (EDD-Clip-Zahlen dir[1] / dir[3] / dir[5]):

- Zombies 0x10-0x18 (+ Blobs 25/28): 6 / **43** / 14 → "largest" trifft dir[3] **zufällig richtig**.
- Dog 0x20 (28/0/5), Crow 0x21 (14/0/3), Gorilla 0x27 (29/0/2), Spider u.a.: **dir[3] LEER**
  — deren AI-Handler spielen über andere Kanäle; Port-Nutzung von Bank 0 dort unberührt lassen.
- **NPCs 0x40/0x42/0x45/0x47/0x49/0x4b/0x4d: Bank 0 = 24 Clips, Bank 1 = 14/6/6/6/14/9/6**
  → "largest" wählt bei ALLEN SIEBEN die falsche Bank für den +0x170-Kanal.

## 3. PORT-IST (statisch + dynamisch belegt)

### D1 (URSACHE) — `re15_emd_parse_container` wählt die Bank nach Clip-Anzahl

`emd_common.c:482-491`: "The main set is the bank with the MOST EDD clips … Pick the
largest-EDD bank" → für EM040 wird **Bank 0 (24 Clips)** zu `eb->anim`. Das Original
kennt keine Wahl: `+0x170/+0x174` IST dir[4]/dir[3] (@0x800224b8/c8, §2.1).

Konsumenten der falschen Bank (alle Spawn-relevant):
- `re15_npc_channel_anim` (enemy_ai_common.c:6286-6303): für Sub 6 fällt es auf
  `bank->anim` = Bank 0 → `re15_npc_motion_clip_len` liefert 32/50 statt 16/52.
- Render `anim_select_common.c:207`: `out->anim = &eb->anim` = Bank 0 → die POSE auf dem
  Bildschirm kommt aus Bank-0-Clips. Der Render-Kanal-Override (main.c:5628 ff.) greift
  nur für Subs 0/1/3 — Sub 6 posiert die av-Default-Bank.
- Fallback `s_irons_clip_len` (enemy_ai_common.c:6260) = die Bank-0-Tabelle
  ("dir[1]") — auch der ungeladene Fallback beschreibt den falschen Kanal.

### D2 (MESSUNG) — Port-Probe reproduziert die falsche Bank am Frame-Wrap

`probe_marvin_spawn.exe` (neu; lädt ROOM10D0.RDT + EM040 aus CDEMD0.EMS, tickt die
Spawn-Phase OHNE Szenen-Start):

```
EM040 eb->anim: 24 clips, fc[0..9]=34 32 50 26 20 20 50 1 1 1     <- BANK 0!
 0 MARVIN st=4 s1=6 s2=0 mo=2 fr=0 rot=1142        (INIT-Frame, wie Original)
 1 MARVIN st=4 s1=6 s2=1 mo=1 fr=1 af=0x0020       (Sub-6-Phase 0->1, Clip 1)
32 MARVIN WRAP: mo=1 frame 31 -> 0  (Cliplaenge im Port = 32)     SOLL: 16
33 MARVIN st=4 s1=6 s2=3 mo=2 fr=1                 (Phase 2->3, Clip-2-Loop)
82/132/182/232 WRAP: mo=2 49 -> 0   (Cliplaenge im Port = 50)     SOLL: 52
```

State-Maschine, Reihenfolge, Phasen, Position (5775,0,23625), Yaw 1142, af=0x20,
Neck 0x12 — alles identisch mit dem Savestate. **Einzige Divergenz: die Bank** (und
damit alle Posen der Start-Animation).

### D3 (BESTÄTIGT KORREKT — kein Fix nötig)

INIT-Äquivalent (enemy_ai_common.c:6563-6583) = byte-true (Clip 2 @0x8011c7bc, HP -1,
Neck-Setup, `grid&0x40` → State 4/Sub 6 @0x8011c860 + Neck 0x12 @0x8004260c-18);
`re15_npc_sub_event_reach` (:6432-6443) = byte-true Phasen-FSM von @0x800517f0.

## 4. FIX-PLAN (mit @0x-Zitaten; KEIN Engine-Code in dieser Diagnose geändert)

1. **Bank-1-Parser** (`emd_common.c`, Muster = `re15_emd_parse_victim_bank`):
   `re15_emd_parse_own_bank` = EDD dir[3] + Keyframe-Pool dir[4], Bone-Struktur aus
   dir[2] (Kanal-Loader @0x800224b8/c8). In `re15_enemy_bank_t` als
   `skel_own/anim_own/own_ok` ablegen (pc_enemy_load + PSX-Pendant).
   `own_ok=0` bei leerem dir[3] (Dog/Crow/…) → keinerlei Verhaltensänderung dort.
2. **Kanal-Map vervollständigen** (`re15_npc_channel_anim`, enemy_ai_common.c): für
   State 4 mit Sub ∉ {0,1,3} (also 2, 4/5/7/8, 6, 9 — @0x80050f88, @0x800512bc,
   @0x80051884, @0x80051e9c) `anim_own` zurückgeben, wenn `own_ok`. Damit stimmen
   `re15_npc_motion_clip_len` (Spawn-Clips 16/52, Cutscene-Clip 6 = 10, Walk-Clip 5 = 30)
   und die Sub-FSMs.
3. **Render-Override ergänzen** (main.c neben :5628): gleicher Zweig — State 4,
   Sub ∉ {0,1,3} → `skel_own/anim_own` posieren (clip_override = motion). Muss mit
   Fix 2 identisch mappen (eine Quelle für Länge UND Pose).
4. **Fallback-Tabelle korrigieren**: `s_irons_clip_len` beschreibt Bank 0 (dir[1]);
   für den +0x170-Kanal-Fallback die Bank-1-Tabelle verwenden
   {22,16,52,1,50,30,10,16,1,17,1,17,1,1} (CDEMD0.EMS Blob 18, dir[3]-EDD @Datei
   0x2d5090) — oder den Fallback auf die geladene Bank beschränken (PC lädt immer).
5. NICHT anfassen: `eb->anim`-Konsumenten der Nicht-NPC-Typen (Dog/Crow/Gorilla-AI
   nutzt Bank 0 als Action-Bank — deren dir[3] ist leer, Census §2.5).

**Verifikation danach:** `probe_marvin_spawn.exe` — erwartet Wraps 16 (mo=1) und 52
(mo=2) statt 32/50; danach `probe_marvin_10d0.exe` (Szene: Clip 6 = 10 Frames im Sub 2)
und Live-Verifikation per gdigrab (Skill re15-port-visual-verify) gegen den Savestate.

## 5. Warum der Nutzer es dreimal melden musste

Report 1+2 adressierten die SZENE (Neck-Routing, RBJ-Kanal Sub 0, Mode-6-Walk, Loop-
statt-Hold) — alles Mechanismen NACH Szenenstart. Die Spawn-/Idle-Animation läuft aber
über den vierten Kanal (+0x170/+0x174), dessen Bank-Identität nie gemessen war: die
State-Logs sahen korrekt aus (st=4/s1=6, Clip 1→2), nur die BANK hinter den Clip-Indizes
war eine andere. Erst der Savestate-Pointer-Abgleich (+0x174 → Datei-Offset 0x2d5090 =
dir[3]) macht die Divergenz sichtbar — ein weiterer Fall der Lektion "State-Log korrekt
≠ gerenderte Pose korrekt" (vgl. reai-v2-aim-lower-land-hunch).

## 6. OFFEN (ehrlich)

1. **Visuelle Poseninhalte** der Bank-0- vs Bank-1-Clips nicht gerendert verglichen
   (nur Kanal-/Längen-Beweis). Live-gdigrab nach dem Fix nötig; ideal Frame-Vergleich
   gegen `r10d0_walk1.sav`-VRAM.
2. **Kanal +0x84/+0x16c (Bank 0)**: wofür die geteilte 24-Clip-NPC-Bibliothek game-weit
   benutzt wird (`Plc_motion(1,…)`-Fundstellen) ist nicht kartiert; nur der INIT-
   Einzelframe (@0x8011c800-08) und Sub 1 sind belegt.
3. **Dog/Crow-Handler-Kanäle:** dass deren Overlay-AI über +0x84/+0x16c (nicht +0x170)
   spielt, ist aus dir[3]=leer GESCHLOSSEN, nicht pro Handler disasm-verifiziert —
   irrelevant solange Fix (1) per `own_ok` gated ist.
4. **ROOM10D1** (Alternativ-Charakter-Variante): selber Spawn, selber Fix — nicht geprobt.
5. Player-Positionsfelder im walk1-Savestate (+0x34-Familie) nicht vollständig dekodiert
   (für den Befund irrelevant; Marvin-Felder eindeutig).

## 7. Artefakte

- Ground-Truth: Scratchpad `r10d0_walk1.sav` (+ `.png`; Marvin PRE-Szene) — Vorgänger-Lauf.
- Probe: `re15_port/tests/unit/probe_marvin_spawn.c` (+ CMake-Eintrag, kein ctest).
- Disasm: `re15_disasm.py` gegen PSX.EXE (FUN_80022300, Subs 0-9) und STAGE1.BIN
  (FUN_8011c6dc); Decompile `RE_15_Quellcode_V2/FUN_80022300.c`,
  `RE_15_Quellcode_Overlays/STAGE1_full/FUN_8011c6dc.c`.
- EMS-Census: Session-Skript (EDD-Zählung dir[1]/[3]/[5] aller 29 Blobs, §2.5).
