# SPEC: Dauerfeuer-FSM + Entlade-Effekte je Waffe — Implementierungs-Spezifikation für den Port

Stand 2026-09-12. Synthese der 12 Dossiers in `analysis/waffen_fsm_2026-09-12/`
(tabellen-xrefs, fsm-entry-sub0, fsm-sub1, fsm-sub2, fsm-sub34, fsm-sub56, fsm-sub789,
entlade-ingram, entlade-mc51-flamme, entlade-schrot-w7-w13, entlade-granaten-burst,
entlade-browning-nahkampf). Jede Konstante trägt ihre `@0x`-Adresse (RE1.5 `PSX.EXE`,
t_addr=0x80010000; Flammenwerfer-Handler in `DEBUG.BIN`, resident @0x800C0000).

Ziel-Dateien im Port:
- `re15_port/engine/src/player_common.c` — Aim-FSM (Phasen, Clips, Drehen, Übergänge)
- `re15_port/engine/src/game_step_common.c` — Feuer-Pfad (Entladen, Munition, Schaden, FX-Spawns)
- `re15_port/engine/src/re15_esp.c` — ESP-Row-VM (Effekt-Spawns, Knall/Hülsen-Routinen)
- `re15_port/platform/pc/src/audio_pc.c` — `re15_audio_weapon_se(idx)` = ARMS-Bank Record idx
- `re15_port/platform/pc/main.c` — Render-Attach des Waffen-Meshes (Messer-Divergenz)

---

## 0. Grundgerüst (verbindliche Vorab-Fakten)

**Dispatch-Kette (1 Frame):** `main → FUN_80030444` (Pad, @0x80020c88) → Gameplay-Phase
`LAB_8001cbb8` → `FUN_80031c44` (Spieler-Root, @0x8001ce0c) → Mode-Tab `0x80073f90[aca58]`
(@0x80031ca0-b4) → Mode 1 Phase-A/B-Tabs `0x80073fb0`/`0x80073ff0[aca59]` (@0x80031eec/@0x80031f14,
State wird dazwischen NEU gelesen @0x80031efc — Übergang wirkt im selben Frame) → State 7 Phase B
`LAB_80032e44` → Waffen-Tab `0x80074030[aca5d]` (@0x80032e7c):

- Ids **0..2** → Nahkampf `0x80034E70` (Basis `0x80074164` @0x80034ec0)
- Ids **12, 14, 19** → **Dauerfeuer `0x80034014`** (Basis `0x80074150` @0x8003402c)
- alle übrigen 3..11, 13, 15..18 → Standard `0x80032E9C` (Basis `0x800740F4` @0x80032eb4)
- Id 20 → NULL (@0x80074080) — feuert nie.

**Spieler-Entity statisch @0x800aca54** (`DAT_800ac784 := 0x800aca54` @0x80031c6c bzw.
@0x8001d098-a0). Feld-Spiegel: +0x04 aca58 Mode, +0x05 aca59 State, **+0x06 aca5a Substate,
+0x07 aca5b Sub-Phase**, +0x09 aca5d Waffen-Id, +0x34 aca88 Position, +0x6a acabe Yaw,
+0x8c acae0 Speed, +0x8f acae3 Crossfade, **+0x94 acae8 Clip, +0x95 acae9 In-Clip-Frame**,
+0x98 acaec Aim-Wort, **+0x9e acaf2 Feuertakt-Zähler**, +0x1d0 acc24 (Leser unbekannt).

**⛔ Halbwort-Regel:** ALLE Substate-Übergänge sind `sh` auf `0x800aca5a` — Little-Endian nullt
das High-Byte = `aca5b` (Sub-Phase) im selben Store (Belegliste @0x800341f4/80034244/80034264/
8003430c/80034418/80034474/800347e4/80034d40/80034e40/800350a4/8003513c/8003523c/80035400/
80035508/80035778). **Ein Port, der Substate und Phase getrennt hält, MUSS bei jedem
Substate-Wechsel die Phase mitnullen.** Ausnahme `sh 0x101` @0x80033f70 (Substate 1 + Phase 1 =
HOLD ohne Re-Init).

**Pad-Semantik (virtuelles HELD-Wort `DAT_800ac768`, Builder FUN_80030444 @0x800304b8-e4,
Keymap Typ A @0x80073dbc):** Bit 0x40 = **ABZUG (SQUARE, LEVEL — kein Edge)**; Bit 0x100 =
**ZIELEN (R1, LEVEL)**; 0x10/0x20 = Ziel HOCH/TIEF; 0x8/0x2 = drehen links/rechts; Edge-Wort
`DAT_800ac76c`; roher L1-Edge `DAT_800ac762 & 4` (Retarget). Typ C: Abzug=CIRCLE, Zielen=R2.

**Aim-Wort `DAT_800acaec`:** Bit15 = HOCH, Bit14 = MITTE, Bit13 = TIEF, exklusiv über
`(x & 0x1fff) | bit` (RE2-Gegenprobe ghidra_re2_Leon.txt:189770-189830).

**Anim-Stepper `FUN_8001f314(EMR=DAT_800acbc4, EDD=DAT_800acbc8, a2=0 vor/1 rück, a3=0x200)`:**
liest Clip aus entity+0x94, Cursor +0x95, dekrementiert Crossfade +0x8f; **einziger
Inkrementierer von acae9** (@0x8001f610-1c); Rückgabe 1 exakt beim Clip-Ende (Cursor wrappt
auf 0, @0x8001f628-3c). PLW-Bank-Paar: `FUN_80036b68` lädt dir[0]→EDD/dir[1]→EMR
(@0x80036be4/0x80036c04).

**`FUN_800369f8` ist KEIN Bank-Setter, sondern Foot-Lock/Root-Motion** (Bein-Kette frisch
komponieren, Entity-X/Z um die Endbone-Drift zurückschieben; RE2-Strukturzwilling
RE2_Quellcode_V2/FUN_80016200.c). Dauerfeuer-Master ruft `(0,1)` @0x80034044-4c, Melee-Subs
`(0,0)`, SLASH `(0,1)` @0x80035408.

**PLW-Clip-Längen (byte-true aus den EDDs):**
```
PL00W0C == PL00W13 (16 Clips): [20,31,39,1,50,30,16, 9,19,1, 9,18,1, 9,18,1]
PL00W0E (Flammenwerfer):       [20,31,39,1,50,30,15,21, 5,1,21, 5,1,21, 5,1]
```
Feuer-Clips 7/10/13: W0C/W13 = 9 Frames, W0E = 21 Frames. Release 8/11/14: 19/18/18 bzw. 5/5/5.
Hold 9/12/15: je 1 Frame.

---

## 1. DAUERFEUER-FSM komplett (Tabelle @0x80074150, Dispatcher 0x80034014)

### 1.1 Tabellen-Doppelrolle (bindend für die Portstruktur)

Die "10er-Tabelle" @0x80074150 sind **zwei überlappende 5er-FSMs**:

| Idx | Handler | Dauerfeuer (Basis 0x80074150, Ids 12/14/19) | Nahkampf (Basis 0x80074164 @0x80034ec0, Ids 0..2) |
|---|---|---|---|
| 0 | 0x80034060 | **Sub 0 RAISE** | — |
| 1 | 0x80034278 | **Sub 1 AIM-HOLD** | — |
| 2 | 0x80034510 | **Sub 2 FEUERSCHLEIFE** | — |
| 3 | 0x80034C74 | **Sub 3 SENKEN** | — |
| 4 | 0x80034D68 | **Sub 4 NACHLADEN (tot)** | — |
| 5 | 0x80034EE8 | (toter Tabellenraum) | **Melee-Sub 0 RAISE** |
| 6 | 0x800350C4 | (tot) | **Melee-Sub 1 HOLD** |
| 7 | 0x80035314 | (tot) | **Melee-Sub 2 SLASH** |
| 8 | 0x80035424 | (tot) | **Melee-Sub 3 LOWER** |
| 9 | 0x80035538 | (tot) | **Melee-Sub 4 DRAW** |

Beleg: der Dauerfeuer-Pfad schreibt aca5a nur mit 0..4 (vollständige Store-Liste,
fsm-sub789 §0); die aca5a-Schreiber mit 5..9 (@0x80032168 usw.) gehören zum Nachbar-FSM
@0x8003203c und laufen nie über den Waffen-Dispatch. **Der Port bildet also ZWEI getrennte
5-Sub-Maschinen ab, keine 10er.**

### 1.2 Zustandsdiagramm Dauerfeuer (Subs 0–4)

```
            R1 gehalten + Waffe 12/14/19 (sw 0x701 @0x80032020/0x80032d08)
                     │
        ┌────────────▼───────────┐
        │ Sub 0 RAISE 0x80034060 │  Clip 6, Crossfade 7, Speed 0, Aim=MITTE
        │  Phase 0→1→2 (aca5b)   │  Auto-Aim FUN_8003703c(30000)→acaf3, Slew 0xC0
        └───┬───────────┬────────┘  Drehen ±0x18
   Clip-Ende│           │Früh-Exit: TIEF ab Frame≥8 (@0x800341bc), HOCH ab ≥9 (@0x80034214)
 (@0x80034264)          │(HOCH überschreibt TIEF im selben Frame)
        ┌───▼───────────▼────────┐
        │ Sub 1 AIM-HOLD         │  Clip 9/12/15 = 9+3*b15+6*b13 (@0x800342bc-e8)
        │ 0x80034278             │  Crossfade 7, Cursor 0; Drehen ±0x30
        └─┬──────┬──────┬────────┘
          │      │      │ Elevationswechsel (0x10/0x20/0x30, @0x80034318-…-e0):
          │      │      └─ Phase:=0, Aim-Bit neu, SOFORT-Exit (kein Feuer/Drehen/
          │      │         Anim in DIESEM Bild); Folgebild: neuer Hold-Clip, Crossfade 7
          │      │ !R1 (@0x800342f8) → sh 3 @0x8003430c
          │      ▼
          │   Sub 3 SENKEN 0x80034C74: Clip 6 RÜCKWÄRTS (a2=1 @0x80034cf0),
          │   Crossfade 7, Drehen ±0x18; Clip-Ende → aca59:=0 (@0x80034d38),
          │   sh 0 → aca5a/aca5b (@0x80034d40), aca3c &= ~0xC0 (@0x80034d44-54) = FSM-EXIT
          │
          │ ABZUG (held 0x40 @0x800343f8) + Magazin>0 (FUN_8004ea6c @0x80034404):
          │   sh 2 @0x80034418, **acae9:=0 @0x80034420, acaf2:=0 @0x80034428**
          ▼
        ┌────────────────────────────────────────────────────────────────┐
        │ Sub 2 FEUERSCHLEIFE 0x80034510 — innere Phase aca5b ∈ {0,1,2,3}│
        │  P0 @0x80034564: aca5b:=1, Crossfade 7,                        │
        │     Feuer-Clip = 7+3*b15+6*b13 = 7/10/13 (@0x80034588-a8);     │
        │     acae9 wird HIER NICHT genullt (nur der Sub-1-Eintritt tut  │
        │     es) → Aim-Wechsel mitten im Feuern behält die Schussphase. │
        │     Fällt in P1 durch.                                         │
        │  P1 @0x800345ac (JEDEN Frame):                                 │
        │     acc24:=0x5A (@0x800345b4); aca52|=1 (@0x800345d4,          │
        │       "Spieler feuert"-Latch — Krähen-Leser @0x8004f17c;       │
        │       KEIN Knockdown — Port-Kommentar enemy_ai_common.c:5509   │
        │       ist falsch zugeordnet);                                  │
        │     jalr ENTLADE-Tabelle 0x80074100[aca5d] (@0x800345ec) —     │
        │       ← der Feuertakt lebt IM Handler (§1.3);                  │
        │     Drehen ±0x18 (@0x80034618/3c);                             │
        │     !R1 (@0x8003464c) → sh 1 @0x800347e4 → Sub 1;              │
        │     f314(…,0,0x200) (@0x80034664, Wrap ignoriert — Clip looppt)│
        │     Aim-Wechsel (0x10/0x20/0x30 @0x8003467c-0x80034734):       │
        │       aca5b:=0 + neues Aim-Bit → nächster Frame neuer Feuer-   │
        │       Clip + Crossfade 7; acae9/acaf2 laufen WEITER;           │
        │     !ABZUG (0x40, @0x8003474c) → aca5b:=2                      │
        │  P2 @0x80034764: aca5b:=3; **acae9:=0 @0x8003477c (einziger    │
        │     Frame-Reset in Sub 2)**; Crossfade 7;                      │
        │     Release-Clip = 8+3*b15+6*b13 = 8/11/14 (@0x80034794)       │
        │  P3 @0x800347bc: f314; Clip-Ende → sh 1 @0x800347e4 → Sub 1    │
        └────────────────────────────────────────────────────────────────┘

        Sub 4 NACHLADEN 0x80034D68 (im Auslieferungsstand DOPPELT verriegelt TOT):
          einziger Eintritt @0x80034474 verlangt Magazin leer + SQUARE-EDGE +
          **aca5d == 0x12** (@0x80034448-58) — Id 18 dispatcht aber auf den
          Standard-Handler und erreicht diese FSM nie. Implementiert wäre:
          Clip 15 vorwärts, Aim:=MITTE, Drehen ±0x18; Clip-Ende → sh 1 (Sub 1) +
          FUN_8004ebdc (Magazin füllen) + SE 0x01030001 (@0x80034e3c-58).
          ⇒ Port: NICHT verdrahten (Original-Verhalten: Dauerfeuer-Waffen laden nie nach).
```

**Leeres Magazin, Verhalten komplett:**
- **In Sub 1 (Halten):** ABZUG mit leerem Magazin tut **NICHTS** — kein Klick, kein Reload
  (toter 0x12-Zweig @0x80034434-58). ⛔ Port-Fehler heute: game_step_common.c spielt für ALLE
  Waffen ≥3 den Klick 0x01010001 auf Press-Edge — für 12/14/19 falsch.
- **In Sub 2 (Feuerschleife), Magazin läuft leer:** `FUN_8004eae4` ret 0 → `aca5b:=2`
  (Release-Clip) + Leer-Klick `FUN_80045024(0x01010001, &aca88)` (@0x800349fc-a10 Ingram,
  @0x80034c34-48 MC51); **Flammenwerfer: SE 0x01000001** (@0x800c47b8-c8).

**Timer/Zähler:**
- `acae9` (In-Clip-Frame): Reset bei Sub-2-Eintritt aus Sub 1 (@0x80034420) und beim
  Release-Einstieg (@0x8003477c); inkrementiert NUR von FUN_8001f314.
- `acaf2` (Feuertakt): Reset bei Sub-2-Eintritt (@0x80034428); `++` jeden Feuer-Frame im
  Entlade-Handler (Delay-Slot @0x800349c4/0x80034bfc/0x800c477c, unbedingt); Konsum als
  **signiertes** `%7` fürs Hülsen-Timing (u8, als s8 dividiert — nach Überlauf 127→−128
  verschiebt sich die 7er-Phase; byte-true Randeffekt).
- `acc24 := 0x5A` jeden Feuer-Frame (@0x800345b4) — Leser unauffindbar (§4).

### 1.3 Feuertakt EXAKT (liegt im Entlade-Handler, nicht in der FSM)

Der Entlade-Handler läuft **jeden Frame** der Feuerschleife (jalr @0x800345ec). Darin:

| Was | Formel | Adresse |
|---|---|---|
| **SCHUSS** (Munition −1 `FUN_8004eae4` + Schaden `FUN_80011f50(aca5d, [entity+0x7c])`) | `(acae9 & 4) == 0` — 4 Frames an / 4 aus je 8er-Fenster | @0x800349bc (W12), @0x80034bf4 (W19), @0x800c4774 (W14) |
| Mündungsfeuer | `acae9 % 3 == 0` (Magic 0xAAAAAAAB) | @0x8003483c / @0x80034a80 / @0x800c45f0 |
| Rauch | `acae9 % 6 == 0` (nur W12/W19; W14: jal entfernt) | @0x800348e0 / @0x80034b10 |
| Hülse | `(s8)acaf2 % 7 == 0` (Magic 0x92492493, signiert; nur W12/W19) | @0x80034948 / @0x80034ba4 |
| Taktzähler | `acaf2++` IMMER (Delay-Slot) | @0x800349c4 / @0x80034bfc / @0x800c477c |

**Effektive Kadenz (aus Clip-Länge, nicht als Konstante portieren!):**
- W0C/W13 (Feuer-Clip 9 Frames, Cursor 0..8): Schuss auf Frames **0,1,2,3,8** = 5 Schuss je
  9 Frames ≈ 16,7/s bei 30 fps; Mündung Frames 0/3/6; Rauch 0/6.
- W0E (Feuer-Clip 21 Frames): Schuss auf Frames 0-3, 8-11, 16-19 = 12 je 21 Frames.
- `DAT_800acaf2` ist **NICHT** der Schusstakt — nur Hülsen-Timing.

Beim allerersten Zyklus ist die Phase definiert: der Sub-1→Sub-2-Übergang nullt acae9 und
acaf2 (@0x80034420/28); ein Aim-Wechsel mitten im Feuern nullt beides NICHT (Schusstakt
läuft phasengleich weiter; nur Clip + Crossfade 7 neu).

**Reihenfolge innerhalb des Feuer-Frames (bindend):** Effekte/Munition/Schaden (Entlade-jalr)
laufen VOR dem Drehen, VOR dem R1-Test, VOR f314 und VOR den Elevations-Checks — der
Elevationswechsel-Frame feuert also noch mit dem alten Clip.

### 1.4 Zustandsdiagramm Nahkampf (Subs 5–9 = Melee 0–4; Ids 0..2)

Kalt-Einstieg @0x80034e90-a8: `aca5a==0 && !(aca54 & 0x4000)` (Messer NICHT in der Hand)
→ Substate 4 DRAW; sonst Substate 0 RAISE. Dispatcher ruft FUN_800369f8 NICHT — die Subs
selbst mit (0,0), SLASH mit (0,1).

| Sub | Handler | Zweck | Clip (Elevation) | Eintritt | Austritt | Besonderes |
|---|---|---|---|---|---|---|
| 0 RAISE | 0x80034EE8 | Wieder-Anheben | 6 | Kalt (in-hand) oder aus LOWER-Exit + R1 | Clip-Ende → Phase 2 → sh 1 → HOLD (@0x800350a4) | Aim:=MITTE, Auto-Aim-Latch **FUN_8003703c(5000)** (@0x80034f7c), Slew 0xC0 nur bei acaf3&1 (@0x80034fa8-c0); Drehrate byte0 @0x80074090 = 24 (@0x80035000) |
| 1 HOLD | 0x800350C4 | Halten | 8/10/12 = 8+2*b15+4*b13 (@0x80035104-20) | aus RAISE/SLASH/DRAW | s.u. | Drehrate byte1 @0x80074091 = 48 (@0x80035278) |
| 2 SLASH | 0x80035314 | Stich | 7/9/11 = 7+2*b15+4*b13 (@0x8003535c-74) | HOLD + ABZUG (sh 2 @0x8003523c) | Clip-Ende → sh 1 → HOLD (@0x80035400); kein Abbruch | Schwung-SE 0x01050001 IMMER (@0x8003537c); **Schaden JEDEN Frame im Fenster acae9 ∈ [6..11]** (@0x8003538c-98) via FUN_80011f50(aca5d, GunBone-Translation pose+0x7b8/bc/c0); Root-Motion Kanal 1 (@0x80035408) |
| 3 LOWER | 0x80035424 | Senken | 6 RÜCKWÄRTS (a2=1 @0x800354b4) | HOLD + !R1 (sh 3 @0x8003513c) | Clip-Ende → aca59:=0 (@0x80035500), sh 0 (@0x80035508), aca3c&=~0xC0 = Action-EXIT | Aim-Bits gelöscht (&0x1fff @0x80035470); Drehschritt FEST 24 (@0x800354a0/c4), NICHT Tabelle |
| 4 DRAW | 0x80035538 | Messer ziehen | 0x0D (13) | Kalt-Einstieg | Clip-Ende → Phase 2 → sh 1 → HOLD (@0x80035778) | Zieh-SE 0x01080001 (@0x800355f8); Auto-Aim-Latch FUN_8003703c(**2000**) (@0x800355d0); Keyframe-Event-Bit **0x10000** in `*DAT_800acbbc` (@0x80035700-08) tauscht die Messer-Hand-Zeiger in pose+0x76c/774/770/778 ein (@0x8003573c-48), setzt aca54|=0x4000 + kine+0xa14:=0 |

**HOLD-Prioritätskette (byte-true Code-Reihenfolge, @0x80035124-0x8003523c):**
1. !R1 → LOWER (@0x8003513c)  2. Elevation HOCH  3. TIEF  4. NEUTRAL  5. ABZUG → SLASH
(@0x8003523c)  6. Drehen + Anim-Tick.
**Jeder Zustands-/Elevationswechsel ist ein FRÜH-EXIT ohne f314/369f8 in diesem Frame** —
Feuerdruck im Wechselframe VERLIERT (greift frühestens im Folgeframe). Diese zwei Details
(Früh-Exit + Elevation-vor-Feuer) fehlen im Port (fsm-sub56 §6).

Attach-Mechanismus komplett (Messer, Id 1): Equip setzt `aca54 &= ~0x4000` + `kine+0xa14:=1`
(@0x80046694-b4); Bank-Loader legt NUR für aca5d==1 (@0x80036d38) die Hand-Zeiger beiseite
(DAT_800bb4b4/800bbe04/800b8988/800bbd90, @0x80036d74-9c) und rendert die bloße Hand; das
Draw-Event tauscht sie zurück. ⛔ Port-Divergenz: `platform/pc/main.c` ~Z.7559 zeigt das
Waffen-Mesh UNCONDITIONALLY bei aca5d!=0 — beim Messer muss bis zum Draw-Event die bloße
Hand sichtbar sein (Latch `s_knife_in_hand` existiert, wird im Render nicht konsultiert).

### 1.5 Standard-FSM-Referenz (für die Entlade-Einbettung, Ids 3..11/13/15..18)

Tabelle @0x800740F4 (Subs 0..5): 0 RAISE 0x80032F18 (Clip 6) · 1 HOLD 0x80033180 (Clip
8/10/12 @0x800331dc) · 2 FIRE 0x80033460 (Clip 7/9/11 = 7+2*b15+4*b13 @0x800334a8-bc;
Entlade-jalr **EINMAL je Schuss** per aca5b-Latch @0x800334d8-e0) · 3 LOWER 0x80033C74 ·
4 RELOAD 0x80033D7C (Clip 0xD; W7-Speedloader-FX 0x04060800 bei Bild 10 @0x80033e34-8c;
Abschluss Refill FUN_8004ebdc + SE 0x01030001 @0x80033ebc-d4) · 5 L1-RETARGET 0x80033EEC
(FUN_80037250 Ziel-Zyklus, Slew 0xC8, Kegel 0x64). Übergangs-Liste vollständig in
entlade-browning-nahkampf §5. Substate-Slots 3/4/5 ÜBERLAPPEN die Entlade-Tabelle [0..2]
(kollisionsfrei — Ids 0..2 sind Nahkampf). Reload-Gate: `sltiu id,9` @0x80033368 — Ids ≥ 9
laden nie nach (stattdessen Klick 0x01010001 @0x8003338c).

Schuss-Besonderheiten im Standard-Sub-2 (@0x80033460):
- Schrotflinte **NUR Id 8** (hart `==8` @0x80033508-14): 3 Zusatz-Resolves bei acae9==3/5/7
  (@0x80033528-54). **SPAS-12 (13) hat KEINE Streu-Resolves** — byte-true so lassen.
- **Granatwerfer NUR Id 9** (@0x8003368c): Projektil-Effekt **0x040D1000** bei acae9==19
  (HOCH, Offs {0,0x12c,0x320}), ==22 (MITTE, {0,0,0x1f4}), ==24 (TIEF, {0,0,0x12c})
  (@0x800336bc-0x800337a4). Substate 2 lebt über Cursor>tab[+2]=10 hinaus nur solange R1
  gehalten (@0x80033604) — Loslassen vor dem Spawn-Frame unterdrückt die Granate, Munition
  ist trotzdem weg.
- Recoil-Break: `!R1 && acae9 > tab5[(id-1)*5+2]` → Sub 3 (@0x80033634-4c; byte2 = 7 bzw. 10).
- Drehen: HOLD byte1=48, FIRE byte1>>1=24 (@0x800335a4/f0), RAISE byte0=24.

---

## 2. ENTLADE-EFFEKTE je Waffe (Tabelle @0x80074100, Datei-Offset 0x64900)

`FUN_80019700`-ABI: a0 = `(fx_id<<24)|(sub<<16)|scale16` (Decode @0x80019704-1c), a1 = Yaw
`lh DAT_800acabe` (→ Slot+0x2e), a2 = Gun-Bone-Matrix `[DAT_800acbdc]+0x7a4` (= Bone 11
Welt-Matrix, 11*0xAC+0x40), a3 = Offset-Triple (R*v+T, FUN_80019e20). Effekt-Ids CORE00.ESP:
2 = Mündung, 3 = Rauch, 4 = Hülse. `c = (DAT_800aca5c & 4) ? 1 : 0` (Charakter/Kostüm-Bit;
wählt auch Schadenszeile +0x58 @FUN_80011f50.c:54).

**KEIN Handler ruft den Schuss-Knall selbst** — der Knall ist ESP-Row-getrieben: Row-Routine 9
@0x80017654 = `FUN_80045024(0x01000001, &weltpos)` (ARMS Record 0, hart codiert) + Lärm-Latch
`0x800b5358:=1` auf dem 2. Slot-Tick des Mündungs-Effekts. Im Port bereits so modelliert
(re15_esp.c Routine 9 → pc_bang, audio_pc.c:1265).

| Id | Waffe | Handler | Slot | Mündung (Code / Offs Leon / Elza) | Rauch | Hülse | Abrechnung | Leer-SE | ⛔ Port heute |
|---|---|---|---|---|---|---|---|---|---|
| 0–2 | Nahkampf | (Überlappung: 0x80033C74/D7C/EEC = Standard-Subs 3/4/5) | @0x80074100-08 | — nie als Entlade gerufen | | | | | korrekt (Alias erkannt) |
| 3, 4 | Browning HP | 0x800337BC | @0x8007410c/10 | 0x02000800 {0x8c,0x25d,0} @0x800337c0-f8 | 0x03000C00 {0x91,0x1f4,−25} @0x80033808-38 | 0x04000800 {0x91,0x109,−50} @0x8003383c-68 | 1× Resolve @0x80033880 + 1× Ammo @0x80033888 | — | ✅ korrekt (Referenz-Implementierung) |
| 5, 6 | M93R / Glock 18 (3-Burst) | 0x800338A8 | @0x80074114/18 | 0x0**202**0800 {0x8c,0x25d,0} @0x800338ac-e4 | 0x0**302**0c00 {0x91,0x1f4,−25} @0x800338f4-24 | 0x0**402**0800 {0x91,0x109,−50} @0x80033928-54 | 1× Resolve @0x8003396c + **3×** Ammo @0x80033974/7c/84 | — | ⛔ FX: nimmt sub 0 statt sub 2 (Browning-Codes); 3×-Ammo ist drin |
| 7 | Super Redhawk (.44 Revolver) | 0x800339A4 | @0x8007411c | 0x02000**E**00 {0x8c,0x25d,0} @0x800339a8-e4 | 0x0300**1000** {0x91,0x1f4,−25} @0x800339ec-1c | **KEINE** (Revolver!) | 1× Resolve @0x80033a34 + 1× Ammo @0x80033a3c | — | ⛔ falsche Scales (0x800/0xc00 statt 0xE00/0x1000) + fälschlich Hülse. Speedloader 0x04060800 im Reload Bild 10 ist portiert ✅ (player_common.c:800-808) |
| 8 | Remington M870 | 0x80033A58 | @0x80074120 | 0x02**03**0F00 {0xa0,0x528,3} @0x80033a5c-a4 | 0x03001400 {0xa0,**0x500**,3} @0x80033aa8-c8 (x/z VOM MÜNDUNGS-TRIPLE GEERBT — nur y neu!) | 0x04**03**0920 {0xa0,0x208,0x50} @0x80033acc-f8 | 1× Resolve @0x80033b10 + 1× Ammo @0x80033b18; +3 Resolves Frames 3/5/7 im FSM @0x80033508-58 | — | ⛔ FX komplett Browning (Codes, subs, Offsets); Streu-Fenster ✅ (re15_player_schrot_fenster) |
| 9 | Granatwerfer | 0x80033B38 | @0x80074124 | **keine** (8-Instruktions-Stub) | — | — | NUR 1× Ammo @0x80033b40; Projektil 0x040D1000 separat im FSM (Frames 19/22/24, §1.5) | — | ⛔ spawnt Browning-Trias + Resolve; Projektil-Spawn fehlt (wurde als "mis-port" entfernt — er ist für Id 9 echt) |
| 10, 11 | Granaten-Varianten | 0x80033B58/78 | @0x80074128/2c | keine | — | — | NUR 1× Ammo — im Auslieferungsstand FOLGENLOS (kein Projektil, kein Schaden) | — | ⛔ wie 9 |
| 12 | Ingram M10 | 0x800347F8 | @0x80074130 | 0x02**01**0800 %3 {70−10c, 1050−30c, −60−30c} @0x80034844-b4 | 0x03000B00 %6 gleiche Offs @0x800348e8-fc | 0x04000800 %7(acaf2) {200−40c, 100, −20−40c} @0x8003494c-a8 | pro Frame; Schuss+Resolve nur `(acae9&4)==0` @0x800349bc-ec | 0x01010001 @0x80034a04-10 | ⛔ komplett: kein Dauerfeuer, Browning-FX, Einzelschuss |
| 13 | SPAS-12 | 0x80033B98 | @0x80074134 | 0x02030F00 {0x17c,0x636,−0x1a4} @0x80033b9c-e0 | 0x03001400 {0x17c,**0x5d2**,−0x1a4} @0x80033be4-04 (x/z geerbt) | 0x04030920 {0x118,0x190,−0xa0} @0x80033c08-38 | 1× Resolve @0x80033c50 + 1× Ammo @0x80033c58; **KEINE Streu-Resolves** (Gate hart ==8) | — | ⛔ FX Browning; Port darf SPAS NICHT ins Schrot-Fenster nehmen (heute korrekt: Gate ==8) |
| 14 | Flammenwerfer | 0x800C45A8 (DEBUG.BIN@0x45A8, resident via FUN_80013b60(7,&0x800c0000,0)) | @0x80074138 | 0x03**1D**1200 %3 {150,1200,0} fest, **a1=3000 statt Yaw** @0x800c45f8-4674 | ENTFERNT (jal gepatcht weg @0x800c46a8-bc) | ENTFERNT (@0x800c4708-60) | pro Frame; Schuss/Resolve `(acae9&4)==0` @0x800c4774-a4 (Standard-Resolver, id 14!) | **0x01000001** @0x800c47bc-c8 | ⛔ komplett: kein Dauerfeuer, Browning-FX |
| 15–18 | (Werfer-Klasse, unfertig) | **NULL** | @0x8007413c-4b | jalr 0 = Absturz, falls je erreicht | | | | | Port: nie in den Feuer-Pfad routen |
| 19 | H&K MC51 | 0x80034A30 (im Ghidra-Dump nur Rohbytes; per re15_disasm.py) | @0x8007414c | 0x02010800 %3 {65, 1300, 90−160c} @0x80034a88-ae4 | 0x03000B00 %6 gleiche @0x80034b18-5c | 0x04000800 %7 {145,530,25} (ohne c) @0x80034ba8-e0 | wie Ingram @0x80034bf4-c28 | 0x01010001 @0x80034c3c-48 | ⛔ komplett |
| 20 | — | (Überlappung: 0x80074150 = Dauerfeuer-Sub-0-Zelle) | | Dispatch @0x80074080 = NULL — feuert nie | | | | | — |

**Hülsenverhalten:** Pistolen/MPs = Effekt 4 sub 0 (Init-Routine 16, 2-Tick-Eject, dann
R11-Physik: RNG-Streuung, Schwerkraft, Boden-Bounce mit Klick-SE); Schrotflinten = Effekt 4
**sub 3** (Init-Routine **38**, eigene Schrothülse; CORE00.ESP rowblk 0x18C0, sub3-Basis
0x1A00); Revolver (7) = keine; Dauerfeuer = sub 0 im 7er-acaf2-Takt; Flammenwerfer = keine.
Mündung sub 3 (Schrot) = Großblitz mit **5 Streams statt 2** (CORE00.ESP rowblk 0x0FC8,
sub3-Basis 0x1200); Mündung sub 1 (0x02010800) = Dauerfeuer-Variante (Row-Inhalt un-dekodiert,
§4).

**⛔ Kern-Portfehler heute (game_step_common.c ~1382-1444):** Der Feuer-Pfad spawnt für JEDE
Waffe ≥3 die Browning-Trias `(2,0,0x800)/(3,0,0xc00)/(4,0,0x800)` mit den Browning-Offsets
{0x8c,0x25d,0}/{0x91,0x1f4,−25}/{0x91,0x109,−50}. Einzig differenziert: 3×-Ammo (5/6) und
das Schrot-Streu-Fenster (8). Es fehlen: waffenspezifische Codes/subs/scales/Offsets,
Elza-Offsets (aca5c&4), der Revolver-ohne-Hülse-Fall, die Rauch-Offset-Vererbung, die
Granaten-Stubs (keine FX!), das Granaten-Projektil, und die komplette Dauerfeuer-Mechanik
für 12/14/19.

---

## 3. PORT-ABBILDUNG (konkrete Schritte)

### Schritt 1 — Entlade-Tabelle als Daten (`game_step_common.c`)

**Ort:** ersetzt den hartkodierten Browning-Block Z.~1400-1444 (die drei
`re15_player_gunbone_world` + `re15_esp_fx_spawn_rows`-Aufrufe).

**Mechanik:** eine statische Tabelle, Index = Waffen-Id (Spiegel von @0x80074100):

```c
typedef struct {            /* ein FUN_80019700-Spawn */
    uint8_t  fx_id, sub;    /* a0>>24, (a0>>16)&0xff */
    uint16_t scale;         /* a0&0xffff */
    int16_t  ofs[3];        /* Leon-Triple */
    int16_t  ofs_alt[3];    /* Elza-Triple (aca5c&4); gleich, wenn kein c-Zweig */
} re15_fx_spawn_t;
typedef struct {
    uint8_t ammo_per_shot;              /* 1; Burst 5/6 = 3 (@0x80033974-84) */
    uint8_t n_fx;                       /* 0 (Granaten-Stubs) .. 3 */
    re15_fx_spawn_t fx[3];              /* Mündung/Rauch/Hülse in Handler-Reihenfolge */
    uint8_t resolve;                    /* 1 = FUN_80011f50; Granaten 9/10/11 = 0! */
} re15_entlade_t;
static const re15_entlade_t s_entlade[21] = { /* Werte + @0x je Zeile aus §2 */ };
```

- Zeile W3/W4: fx = {2,0,0x800,{0x8c,0x25d,0}}, {3,0,0xc00,{0x91,0x1f4,−25}},
  {4,0,0x800,{0x91,0x109,−50}} (@0x800337c0/0x80033808/0x8003383c).
- Zeile W5/W6: subs = 2 (@0x800338ac/f4/28), ammo_per_shot = 3.
- Zeile W7: nur 2 Spawns (0x02000E00 / 0x03001000), keine Hülse (@0x800339a8/ec).
- Zeile W8: (2,3,0xF00,{0xa0,0x528,3}), (3,0,0x1400,{0xa0,0x500,3}), (4,3,0x920,{0xa0,0x208,0x50})
  — die Rauch-Offset-Vererbung ist in der Tabelle bereits ausmaterialisiert (@0x80033ac8:
  nur y neu, x/z aus dem Mündungs-Triple).
- Zeile W13: (2,3,0xF00,{0x17c,0x636,−0x1a4}), (3,0,0x1400,{0x17c,0x5d2,−0x1a4}),
  (4,3,0x920,{0x118,0x190,−0xa0}).
- Zeilen W9/W10/W11: n_fx=0, resolve=0, ammo_per_shot=1 (@0x80033b38-98: reine Ammo-Stubs).
- Zeilen W12/W19/W14: eigene Struktur (Schritt 3), nicht über diesen Einzelschuss-Pfad.
- Zeilen W15..W18, W20: NULL-Marker — Feuer-Pfad überspringen (Original: jalr 0 = Crash;
  der Port darf hier still nichts tun, die Ids sind ohnehin nicht equipbar).

**Verbrauch:** an der bisherigen Stelle
`for (i<n_fx): re15_player_gunbone_world(fx[i].ofs…) → re15_esp_fx_spawn_rows(bank,
fx[i].fx_id, fx[i].sub, fx[i].scale, gp[0],gp[1],gp[2], pl->y, 0)`; danach
`if (resolve) re15_player_weapon_fire(eq_item);` und `ammo_per_shot × re15_ammo_consume()`.
Charakter-Zweig: `ofs_alt`, wenn das Port-Äquivalent von `DAT_800aca5c & 4` gesetzt ist
(Leser existiert bereits: platform/pc/main.c:1671/936).

**Granatwerfer-Projektil (nur Id 9) wieder einbauen:** im Recoil-Watcher-Stil des
Schrot-Fensters (`re15_player_schrot_fenster` als Muster): neuer Export in player_common.c
`int re15_player_granate_frame(void)` — liefert bei eq==9 && s_aim_recoil den anim_frame;
game_step: bei Frame==19 && elev>0 → spawn (4,0x0d,0x1000) an Gunbone-Offs {0,0x12c,0x320};
==22 && elev==0 → {0,0,0x1f4}; ==24 && elev<0 → {0,0,0x12c} (@0x80033690/0x800336a4/
0x80033758, Codes @0x800336bc-0x8003377c). Der alte Watcher wurde zu Unrecht als Mis-Port
gelöscht — er war nur an der falschen Waffe (Browning) verdrahtet. Recoil-Break-Schwelle 10
für Id 9 ist im Port schon korrekt (recoil_break-Tabelle) — R1-Loslassen vor dem Spawn-Frame
unterdrückt das Projektil byte-true.

### Schritt 2 — Dauerfeuer-FSM (`player_common.c`)

**Ort:** der Aim-Block (Statics ab Z.~150, FSM-Tick ab Z.~660). Vorhandene Struktur:
`s_player_aim_phase` (RAISE/READY/RELOAD/LOWER), `s_aim_cur_clip`, `s_aim_recoil`,
`s_aim_elev`, `s_aim_melee`, `s_aim_clip_fcs[RE15_AIM_CLIP_MAX=16]` (per Waffe vom
Platform-Loader gefüttert — die 16-Clip-Bänke W0C/W0E/W13 sind adressierbar).

1. **Klassen-Latch beim Raise-Eintritt** (neben `s_aim_melee`):
   `s_aim_auto = (eq==12 || eq==14 || eq==19);` — Spiegel des Dispatches @0x80074030.
2. **Neue Statics:** `static int s_auto_phase;` (= aca5b der Feuerschleife, 0..3) und
   `static uint8_t s_feuertakt;` (= acaf2). Beim Feuerschleifen-Eintritt beide/Frame nullen
   (@0x80034420/28).
3. **Hold-Clip-Formel verzweigen:** auto → `9 + 3*up + 6*down` (9/12/15, @0x800342bc-e8);
   sonst wie heute `8 + 2*up + 4*down`. Damit ist der bisherige Verdacht "Clip 15 =
   Halten-runter" ersetzt: **Clip 15 = Aim-TIEF-Hold** (1 Frame, loopt).
4. **Feuer-Eintritt** (SQUARE HELD in READY, mag>0): auto → neuer Phasenwert
   `RE15_AIM_AUTOFIRE` (oder `s_aim_recoil=1 + s_aim_auto_firing=1`), Clip
   `7 + 3*up + 6*down` (7/10/13, @0x80034588-a8), anim_frame=0, anim_frac=7, s_feuertakt=0.
5. **Pro-Frame-Hook** (Muster `re15_player_schrot_fenster`):
   ```c
   /* liefert 1 solange die Feuerschleife läuft und füllt die Takt-Flags dieses Frames */
   int re15_player_autofire_tick(int *discharge, int *muzzle, int *smoke, int *shell);
   ```
   Implementierung: `f = anim_frame; *muzzle = (f%3==0); *smoke = (f%6==0);
   *shell = (((int8_t)s_feuertakt % 7) == 0); *discharge = ((f & 4) == 0);
   s_feuertakt++;` — **signed %7 und u8-Überlauf byte-true** (@0x80034910-44).
   game_step konsumiert (Schritt 3). Der Feuer-Clip loopt (f314-Wrap ignoriert @0x80034664):
   im Port darf der bestehende Recoil-Ende-Zweig (`anim_frame >= fc-1 → HOLD`) für
   `s_aim_auto` NICHT greifen — stattdessen anim_frame auf 0 wrappen und weiterfeuern.
6. **Abzug-loslassen (SQUARE):** Phase → `RE15_AIM_AUTOREL`: Clip `8 + 3*up + 6*down`
   (8/11/14, @0x80034794), anim_frame=0 (**einziger Frame-Reset**, @0x8003477c), frac=7;
   Clip-Ende → READY mit Hold-Clip-Reinit (sh 1 @0x800347e4).
7. **R1-loslassen während des Feuerns:** direkt → READY (Hold-Reinit; @0x800347e4);
   der bestehende `!R1 → LOWER`-Zweig übernimmt im Folgeframe (Original: Sub 1 sieht !R1
   → Sub 3). Der Gun-Recoil-Break (`anim_frame > rb_thr`) gilt für auto NICHT.
8. **Elevationswechsel mitten im Feuern:** neuer Feuer-Clip 7/10/13 + frac=7, **anim_frame
   und s_feuertakt NICHT nullen** (@0x800346d8-e4: nur aca5b:=0; acae9/acaf2 laufen weiter).
   Im HOLD: Elevationswechsel kostet ein Bild (Früh-Exit ohne Anim-Tick, @0x80034348/94/e0)
   — dieselbe Regel wie beim Melee-HOLD (Schritt 5).
9. **Drehen:** HOLD ±0x30=48 (@0x800344b4/d8), FEUERN ±0x18=24 (@0x80034618/3c), SENKEN
   ±0x18 (@0x80034cdc/d00) — die vorhandene rate-Logik (`READY? (recoil?24:48):24`) liefert
   dieselben Zahlen; nur sicherstellen, dass die Feuerschleife als "recoil" zählt.
10. **RAISE-Feinheiten (auto):** Auto-Aim-Latch Radius 30000, Slew **0xC0** (@0x800340f4/
    0x80034128 — der Port nutzt für Gun 0xC8; für die drei Auto-Waffen 0xC0 eintragen);
    Früh-Exit in den Hold: TIEF ab Frame≥8, HOCH ab Frame≥9 (@0x800341bc/0x80034214,
    HOCH gewinnt im selben Frame).
11. **Leer-Verhalten:** in READY (Hold) mit leerem Magazin tut ABZUG bei auto-Waffen
    NICHTS — das Klick/Reload-Gate in game_step (Z.~1367-1379) um
    `&& !(eq==12||eq==14||eq==19)` ergänzen (toter 0x12-Zweig @0x80034448-58).
    Reload-Gate `eq < 9` schließt sie bereits aus ✅. Läuft das Magazin IN der Schleife
    leer: Übergang in `RE15_AIM_AUTOREL` + Klick (Schritt 3).
12. **Kein Nachladen für 12/14/19** (Sub 4 tot); `re15_player_reload_start` bleibt für sie
    unerreichbar.

### Schritt 3 — Feuer-Pfad Dauerfeuer (`game_step_common.c`)

**Ort:** im R1-Block nach dem bestehenden Einzelschuss-Zweig. Neuer Zweig:

```c
if (eq_item==12 || eq_item==14 || eq_item==19) {
    int dis, mz, sm, sh;
    if (re15_player_autofire_tick(&dis, &mz, &sm, &sh)) {
        const re15_auto_entlade_t *ae = &s_auto_entlade[idx(eq_item)];
        /* Effekte (Reihenfolge wie im Handler: Mündung → Rauch → Hülse) */
        if (mz) spawn(ae->muzzle);                 /* W12/W19: (2,1,0x800); W14: (3,0x1D,0x1200, param=3000) */
        if (mz && sm && ae->has_smoke) spawn(ae->smoke);   /* (3,0,0xB00); W14: keiner */
        if (sh && ae->has_shell) spawn(ae->shell);         /* (4,0,0x800); W14: keiner */
        if (dis) {
            if (re15_ammo_mag_nonzero()) { re15_ammo_consume();
                re15_player_weapon_fire(eq_item); }        /* FUN_80011f50 @0x800349ec */
            else { re15_player_autofire_empty();           /* → AUTOREL (aca5b=2) */
                re15_audio_weapon_se(eq_item==14 ? 0 : 1); /* 0x01000001 / 0x01010001 */ }
        }
        /* aca52-Bit0 "Spieler feuert" + Krähen-Lärm: den vorhandenen Schuss-Lärm-Pfad
         * JEDEN Feuer-Frame setzen (@0x800345c8-d4) */
    }
}
```

Offsets/Codes je Waffe (mit Elza-Zweig `c`):
- W12: Mündung (2,1,0x800) {70−10c, 1050−30c, −60−30c} @0x80034884-b4; Rauch (3,0,0xB00)
  gleiche Offs @0x800348e8; Hülse (4,0,0x800) {200−40c, 100, −20−40c} @0x80034968-a8.
- W19: Mündung {65, 1300, 90−160c} @0x80034a94-adc; Rauch gleiche @0x80034b38-54;
  Hülse {145, 530, 25} @0x80034bc4-d4.
- W14: NUR Mündung=Flammenstrahl (3, 0x1D, 0x1200) {150,1200,0} @0x800c464c/465c/4674,
  **param=3000 statt Yaw** (@0x800c466c) — der `param`-Parameter von
  `re15_esp_fx_spawn_rows` (→ Slot+0x2e) trägt die 3000; kein Rauch, keine Hülse.

Wichtig: der Schaden der Dauerfeuer-Waffen ist **pro Patrone** (FUN_80011f50 je
Schuss-Frame) — die vorhandene once-per-target-Latch-Logik in `re15_player_weapon_fire`
prüfen: sie darf Mehrfach-Treffer über die Frames NICHT unterdrücken (der +0x93
Ein-Treffer-Latch des Ziels gilt pro Resolve-Salve, siehe Memory reai-v2-hit-latch-93 —
hier je Frame ein eigener Resolve).

### Schritt 4 — ESP/FX (`re15_esp.c`)

- `re15_esp_fx_spawn_rows(bank, fx_id, sub, scale, x,y,z, floor_y, param)` trägt bereits
  alles Nötige: `sub` wählt den Row-Stream (Schrot-sub-3 = 5 Mündungs-Streams / Routine-38-
  Hülse; CLUT-Zusatz `(sub>>3)*0x40` ist implementiert), `param` landet am Slot (+0x2e) —
  für den Flammenstrahl 3000 durchreichen.
- **Routine 38** (Schrothülsen-Init, Dispatch @0x80071d40[38]): im Row-VM-Dispatch prüfen,
  ob 38 implementiert ist; wenn nicht, ist das ein RE-Gate — Körper zuerst disassemblieren
  (§4), NICHT mit Routine 16 raten.
- **Effekt 2 sub 1** (Dauerfeuer-Mündung 0x02010800): Streams existieren datenseitig
  (CORE00.ESP); der Spawn läuft ohne Codeänderung. Der visuelle Row-Inhalt ist un-dekodiert
  (§4) — für den Spawn selbst irrelevant.
- Knall: unverändert Row-Routine 9 (2. Slot-Tick) — gilt automatisch auch für die
  Dauerfeuer-Mündungen und den Schrot-Großblitz (SE-Id hart in der Routine).

### Schritt 5 — Melee-Feinschliff (`player_common.c`)

Aus fsm-sub56 §6 (fehlt im Port):
1. **Früh-Exit ohne Anim-Tick** im Wechselframe: R1-Release→LOWER, Elevationswechsel und
   ABZUG→SLASH beenden das Bild sofort (kein f314/369f8) — im Port: in diesen Zweigen den
   Anim-Advance dieses Ticks überspringen (`return` vor dem Frame-Inkrement des Aim-Pfads).
2. **Prioritätsreihenfolge im HOLD:** !R1 → Elevation (hoch/tief/neutral) → ABZUG → Drehen.
   Ein Feuerdruck im Elevationswechsel-Frame verliert. Heute prüft der Port Feuer in
   game_step unabhängig — Gate: `re15_player_fire_start` ablehnen, wenn in diesem Tick ein
   Elevationswechsel stattfand (Flag aus dem Aim-Tick exportieren).
3. Dieselben zwei Regeln gelten für den Dauerfeuer-HOLD (Sub 1, @0x80034348/94/e0).

### Schritt 6 — Render-Attach Messer (`platform/pc/main.c` ~Z.7559)

Waffen-Mesh-Attach an `re15_player_knife_in_hand()` koppeln: bei `aca5d==1 &&
!s_knife_in_hand` die bloße Hand rendern (Loader-Stow @0x80036d74-9c); Attach erst beim
Draw-Event (im Port: beim DRAW-Abschluss, bis das Keyframe-Event-Bit 0x10000 aus der
PL00W01-EDD geparst ist — §4).

### Schritt 7 — Kommentar-Korrekturen

- `enemy_ai_common.c:5509`: "player knockdown command FSM" → aca52-Bit0 =
  "Spieler-feuert"-Latch (Setter @0x800345c8 Dauerfeuer-Case-1, @0x800334e8-504
  Standard-Sub-2; Clear jeden Frame @0x80031c9c).
- `RE15_FUN_CATALOG.md` FUN_80019700: "Enemy hitbox-data setup" → generischer
  typ-indizierter Pool-Spawner (Pool @0x800a73b8, 0x60 Slots, Stride 0x84).
- player_common.c Kopf: "Clips 14/15 = Feuer-Ende/Halten-runter — PRÜFEN" ersetzen durch
  die belegte Belegung (14 = Release-TIEF, 15 = Hold-TIEF bzw. tote Nachlade-Anim ist
  Clip 15 NICHT — Nachladen = Clip 15 nur im toten Sub 4; Feuer-Ende = Clip 6 rückwärts).

**Test-/Verifikationspfad:** ctest-Ebene über `re15_player_set_aim_clip_lens` mit den echten
W0C-Längen [.., 9,19,1, 9,18,1, 9,18,1] und Pin auf die Schussfolge {0,1,2,3,8} je 9 Frames,
Hülse alle 7 acaf2 (inkl. s8-Überlauf-Fall 127→−128); Parity per re15-parity-verify (gleicher
Raum, Ingram, SQUARE 60 Frames halten: Munitionsverbrauch, Frame-genaue FX-Spawn-Zählung).

---

## 4. OFFENE PUNKTE (kein Dossier konnte sie klären) + bester nächster RE-Weg

| # | Offen | Bester nächster Weg |
|---|---|---|
| 1 | **Leser von DAT_800acc24** (+0x1d0, =0x5A je Feuer-Frame; 3 Writes, 0 Reads im Dump, Overlays leer) | Laufzeit-Watchpoint auf Read 0x800acc24 (Skill `re15-pcsx-watchpoint`); bis dahin im Port als toter Spiegel mitführen |
| 2 | **Row-Inhalt Effekt 2 sub 1** (Mündung 0x02010800, Dauerfeuer-Variante) — visueller Unterschied zu sub 0 | CORE00.ESP-Rowblock 0x0FC8 dekodieren (gleiches Vorgehen wie das sub-3-Parse im Schrot-Dossier); Routinen-Kette der sub-1-Streams gegen Row-VM-Katalog |
| 3 | **Effekt 3 / sub 0x1D + param 3000** (Flammenstrahl): wie die Row-VM Slot+0x2e interpretiert (Winkel/Reichweite/Seed); Konsument der oberen Sub-Bits (0x18 in Slot+0x71) | Row-Definition des Effekts 3 in CORE00.ESP + der geladenen W0E-Bank durchziehen; dynamisch: Savestate mit feuerndem Flammenwerfer, Slot-RAM @0x800a73b8 lesen (re15-savestate-ghidra) |
| 4 | **Effekt 0x040D1000 (Granate)**: Flugbahn, Aufschlag, Explosionsschaden — FUN_80011f50 ist nachweislich NICHT der Träger | Bank-4-Tabellen DAT_800b22d4/DAT_800b2248 zur Laufzeit (Overlay-gepatcht) dumpen; Savestate mit fliegender Granate + Row-VM-Trace |
| 5 | **Routine 38** (Schrothülsen-Init, @0x80071d40[38]): Körper nicht disassembliert | `re15_disasm.py dis` auf den Tabellen-Eintrag @0x80071d40[38]; Vergleich mit Routine 16 (bekannt aus Trace wf_a18487d9) |
| 6 | **Waffen-Id 0x12-Gate** im toten Sub-1-Leer-Zweig (@0x80034448) — Build-Historie | statisch nicht klärbar; für den Port irrelevant (Verhalten: nichts tun) — dokumentieren, nicht raten |
| 7 | **DAT_800aca5c-Semantik** (Bit 2 wählt Alt-Offsets/Alt-Bank/Schadenszeile; Vermutung Leon/Elza unbelegt) | Options-Global DAT_800b0ff0 → Schreiber @0x8003978c verfolgen; dynamisch: Savestate Leon vs. Elza vergleichen. Port führt beide Offset-Sätze bereits datengetrieben (Schritt 1/3) |
| 8 | **param_4=0x200 an FUN_8001f314** (nur Interp-Pfad FUN_8001f8b4) | FUN_8001f8b4 disassemblieren (Bit-Auswertung des Flag-Worts); betrifft die Crossfade-Qualität, nicht die FSM |
| 9 | **Slot+0x9A** im Zielscan FUN_8003703c (Klasse-C-Kriterium <0; HP vs. am-Boden) und acaf3-Bit1-Leser | RE2-Feld-Vergleich +0x9A/+0x144; dynamisch HP eines Ziels manipulieren und Klassenwechsel beobachten |
| 10 | **Welcher Keyframe des Draw-Clips 0x0D das Attach-Bit 0x10000 trägt** (PL00W01-EDD-Daten, nicht Code); Pflege-PC von DAT_800acbbc | EDD-Frame-Flags von PL00W01 byte-parsen (Java-Extraktor/EDD-Spec §1.4); Pflege-PC per Watchpoint |
| 11 | **FUN_800369f8 Bein-Ketten-Zuordnung** (Bones 2/3/4 vs 5/6/7 = links/rechts?) und Kanal-Semantik (SLASH nutzt Kanal 1) | EMR-Bone-Hierarchie des PL00-Skeletts aus den Assets lesen; Port-D6/OPEN-Marker |
| 12 | **SE-Sample-Identitäten** 0x01010001 (Leerklick) / 0x01030001 (Reload) / 0x01000001 (Knall & Flamm-Leerklick — dasselbe Record!) | VAB-Dump der ARMS-Bank (Deskriptoren @0x801fcd00) → WAV-Render; funktional sind die Codes belegt, nur der Klang-Inhalt nicht |
| 13 | Ob Ids 0/2 je als anlegbare Waffe vorkommen (DRAW würde dort veraltete Stow-Zeiger einwechseln — Loader-Gate ist ==1) | Inventar-Item-Belegung prüfen (Item-Katalog DAT_800c4a28 / Debug-Menü-Equip-Test) |
