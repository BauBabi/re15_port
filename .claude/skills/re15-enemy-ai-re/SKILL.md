# RE1.5 Gegner-KI Reverse Engineering (Playbook)

Destilliert aus drei gelösten Referenz-Exemplaren: **Zombie** (0x10/0x11/0x16, ROOM1140, ~30 Commits) =
das Boden-Grab-Muster; **Krähe** (0x21, 3D-Flieger, komplett byte-true inkl. Flocking/Death/Präsentation,
RE15_CROW_AI.md) = Doppel-Dispatch + Flag-Globals + stage-gated Killability; **Hund** (0x20, Cerberus,
Boden-Chase/Biss, RE15_DOG_AI.md) = Dual-Dispatch-Brain + Aliasing-Tabellen + Damage-im-inner-jt. Der
schnellste bekannte Weg, die KI + Animationen eines **neuen Gegnertyps** byte-true zu RE'en und in den
Port zu bringen. **Dieser Skill besitzt den End-to-End-Bogen**
(Typ-Byte → Dispatch-Familie → FSM-Graph → Dormanz-Check → Port → Parity); die
Werkzeug-Skills liefern die Einzelschritte und werden hier nur referenziert:
`re15-psx-disasm` (statischer Disasm + Tabellen-Decoder — das PRIMÄRE Statik-Tool),
`re15-savestate-ghidra` (Savestate-Decode, Enemy-Dump, Dormanz-Sweep, Ghidra-headless),
`re15-room-capture` (DuckStation-Treiber, Live-Saves), `re15-room-probe` (Port-seitige
C-Test-Proben ohne DuckStation), `re15-parity-verify` (Port-vs-PSX-Invarianten),
`ghidra-mapping` (FUN-Katalog zuerst prüfen/pflegen).

**Kernidee: ~70 % jeder Gegner-KI ist GETEILTE Infrastruktur, die schon RE'd und portiert ist.**
Neu pro Gegner sind nur: die Overlay-Dispatch-Tabellen, die State-Handler, die Clip-Zuordnung
und die Verhaltens-/Gait-Daten. Alles andere (Steering, Anim-Advance, Kollision, FX, SE,
Pathfinding, Damage) ist EXE-seitig gemeinsam — NICHT neu reverse-engineeren (§4).

---

## 0. Vorbereitung: Typ-ID und Overlay finden

1. **Typ-ID**: aus dem Savestate (`re15_enemy_state.py <sav>` zeigt `t=XX` je Slot) oder der
   RDT-EM-Liste des Raums. Zombies: 0x10/0x11/0x16 (ein gemeinsames Root), 0x13 (eigenes Brain).
2. **Stage-Overlay**: Raum-Nummer → Stage (JUMP-Nummern sind HEX; Stage 1 = 0x100er).
   Der Gegner-Handler liegt im Overlay `STAGE{N}.BIN` @0x80100000.
3. **EXE-Dispatch @0x80072bac**: mappt Typ → Overlay-Handler. Die Overlay-Registrierung
   (STAGE1: @0x8011e864) schreibt die Einträge beim Overlay-Load — die Tabellen selbst stehen
   STATISCH in der BIN (40/40 == RAM verifiziert; kein Runtime-Patching).
4. **Erste Anlaufstelle IMMER**: `RE15_FUN_CATALOG.md` (Repo-Root) + Memory
   `reai-v2-room1140-anim-re` — der Zombie ist das vollständig gelöste Referenz-Exemplar.

## 1. Statisch: die Dispatch-Kette auflösen (das Muster)

Jeder bisher gesehene Gegner folgt derselben Schachtelung (Zombie-Adressen als Referenz):

```
EXE @0x80072bac[typ]  →  Overlay-Root (Zombie: FUN_80100424)
  Root-Tabelle [+0x4 low byte]      Zombie: @0x8011f7b4, 8 Einträge
    [0] init  [1] active  [2] hurt (FUN_80105a8c)  [3] death (FUN_80106ba4)
    [4] scripted-pose-FSM (Sub-Tabelle @0x8012018c)  [7] corpse-settle (FUN_80109554)
  Grid-Mode [+0x9 & 0xf]            Zombie: @0x8011f80c (13 logisch; 13-15 überlappen!)
    Mode-0-DECIDE-Tabelle           Zombie: @0x8011f840, exakt 20 Einträge [0..0x13]
    Mode-0-ANIMATE-Tabelle          Zombie: @0x8011f890, exakt 20 Einträge
  State-Word-Konvention: 32-bit-LE-Write auf +0x4 → byte0=+0x4 root, byte1=+0x5,
    byte2=+0x6, byte3=+0x7 (z.B. 0x301 grab-face, 0xb01 push-off, 0x701 turn;
    0x10201 hurt→engage nutzt byte2, um den Engage-ENTRY-Step zu skippen)
  Verhaltens-/Gait-Daten            Zombie: Behavior @0x8011faf0 (≥5 Spawns)/@0x8011fb00 (<5),
    Gait-Blob @0x8011f9f0 (variant*0x80, UNBEGRENZT — negative Rows steuern WEG = Weave),
    Lie-Tabelle @0x8011FB10, Hurt-Master @0x8011fe30, Damage-Rows @0x8011FEAC
```

**Zweiter bekannter Datenpunkt fürs Muster**: Elliot (Typ 0x47) nutzt eine EIGENE Tabelle
@0x801217a0 — das Root-Tabellen-Muster gilt, die Adressen sind je Typ-Familie verschieden.

**Dritter Datenpunkt — Krähe (0x21, Root 0x80112020, State-Tab @0x8012111c):** 3D-FLIEGER. Der
ACTIVE-State ist selbst ein DOPPEL-Dispatch auf +0x5 über ZWEI Tabellen — Steer-Tab @0x8012113c +
Move-Tab @0x80121184 (18 Einträge, NICHT 12 — Bounds vom sltiu ableiten). Zwei Flag-GLOBALS statt einem:
0x800aca50 (Schwarm-Koordination, Bit-Kommandos) + 0x800b1028 (Bit-Array, Death-Gate bit-0x1f). Killability
ist STAGE-SPEZIFISCH: bit-0x1f wird nur in STAGE3/5 gesetzt → STAGE1-Krähen byte-true UNSTERBLICH.

**Vierter Datenpunkt — Hund (0x20, Root 0x8010d7f8, State-Tab @0x80120f74, 12 Einträge):** Boden-Chase/Biss.
Zwei neue Muster: (a) **DUAL-DISPATCH-BRAIN** — der ACTIVE-State (1) dispatcht +0x5 über eine DECISION-Tab
(@0x80120f94, „think": setzt Transitionen) UND eine ACT-Tab (@0x80120fd4, „move": clip+Motion), BEIDE jeden
Frame. (b) **ALIASING/überlappende Tabellen** — @0x80120f94 = state-tab+0x20, d.h. die B-Decision-Tab[0..3]
IST state-tab[8..11]; dieselbe Adresse ist per +0x4 (state) UND per +0x5 (sub) erreichbar. Den PRIMÄR-Pfad
klären (welches +0x4/+0x5 real gesetzt wird), sonst deutet man einen Handler doppelt. (c) **Damage im
inner-jt, NICHT im State-Handler** — die Biss-Damage (player.hp−=10) war in den State-Handlern „unresolved";
sie lag im INNEREN Jumptable der BITE-Sub-State (@0x801001ec, am Connect-Frame anim_frame≥N). Die Damage/
Hitbox sitzt fast immer im tiefsten Nested-Step (Connect-Frame), nicht im Top-Level-Handler → dahin folgen.

**Vorgehen für einen neuen Typ** (Tool: `re15-psx-disasm`, `S=.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`):
1. Handler-Adresse aus @0x80072bac[typ] (Savestate-RAM oder BIN + Registrierungs-Xref).
2. Im Handler die Root-Tabelle finden: `python $S scan <handler>` (zeigt sb-Writes auf
   +0x4..+0x7 = FSM-Transitionen, jal-Ziele, sltiu-Schwellen) und
   `python $S table <tabellen_addr> <n>` (Pointer-/Dispatch-Tabellen dekodieren).
3. Tabellengrenzen aus den Bounds-Checks (`sltiu`) ableiten — NICHT raten (die 20-Einträge-
   Erkenntnis kam aus dem sltiu-Check, Phase 8.7: eine falsche Tabelle kostete Phasen 2-7).
4. Statik-vs-RAM-Kreuzcheck (die 40/40-Methode): `re15-savestate-ghidra` →
   `python scripts/re15_runtime_table.py <sav> <decide_addr> <n> --pair <animate_addr>`.
5. Jeden Tabellen-Slot einem Decompile-File zuordnen (`RE_15_Quellcode_Overlays/STAGE{N}/FUN_*.c`).

**Datei-Offsets (die teuerste Falle zuerst):**
- `STAGE{N}.BIN`: **KEIN 0x800-Header** → `file_off = addr − 0x80100000`
- `PSX.EXE`: `file_off = 0x800 + (addr − 0x80010000)`

## 2. Decompile lesen, aber Disasm-verifizieren

Overlay-Decompiles (`RE_15_Quellcode_Overlays/`) sind die erste Quelle — aber sie können
**komplett falsch** sein (FUN_80100424.c war es; ein „exhaustiver" Agenten-Befund zum
Forward-Walk auch). Regel: **AI-/Dispatch-/State-Logik vor dem Portieren gegen die rohen
Bytes verifizieren.**

**Primär**: `re15-psx-disasm`s `re15_disasm.py` (Subkommandos `dis`/`table`/`scan`/`bytes`/`read`,
wählt die Binary automatisch nach Adressbereich, Offsets korrekt):

```bash
S=.claude/skills/re15-psx-disasm/scripts/re15_disasm.py
python $S dis 0x80102548 60                 # Disasm ab Adresse, 60 Instruktionen
python $S scan 0x80105a8c                   # FSM-Summary: +0x4..+0x7-Writes, jal, sltiu
python $S table 0x8011f7b4 8                # Dispatch-Tabelle dekodieren
python $S read 0x8006e0d0 22 --w 2 --stride 4 --rows 0x17 --rowstride 0x58   # Daten-Tabellen
```

**Alternativ** (in `scripts/` dieses Skills; plain Windows-python — mingw64-python hat
kein capstone/PIL): `dis_ovl.py <lo> <hi> [stage]` (capstone-Vollreferenz) und
`rawdis.py <exe|st1..st6> <lo> <hi>` (dependency-frei, zeigt file_off — gut für Byte-Zitate).

Was disasm-verifiziert werden MUSS (Zombie-Lehren):
- Jump-Table-Basen + Index-Berechnung (Decompile zeigt oft die falsche Tabelle)
- Vorzeichen (der SIGNED Gait-Row-Parameter machte das Weave; unsigned gelesen = „immer toward")
- Feld-Semantik: `+0x1ba` war GROUND-Y-Cache, kein HP; `+0xbc/+0xbe` Schatten-Halbmaße, kein Y-Sink
- Post- vs. Prä-Inkrement (f314 POST-inkrementiert → Frame-0 wird am Latch-Tick geposed)

## 3. Dynamisch: der Savestate ist der Schiedsrichter

Statik sagt, was der Code KANN — nur Live-RAM sagt, was er TUT. Für jede Kern-Behauptung
(Steering-Ziel, State-Übergang, Clip-Nummer) einen Live-Beweis holen:

1. **Raum mit dem Gegner laden**: `re15-room-capture`-Skill →
   `python re15_quickload.py --left/--right N [--triangle M] --postload 10 --out probe.sav`
   (JUMP-Nummern HEX lesen; Cross-Stage `--postload ≥16`). ⚠️ JUMP-Quickload ist wall-clock-
   NICHT-deterministisch — reproduzierbare MESSUNGEN nur mit `re15_walk_probe.py`
   (Direct-Load + Ein-Richtungs-Hold); `re15_advance.py` = Parity-Referenz „N Sekunden ohne
   Input laufen lassen" (KI advanct) für den Diff gegen den Port.
2. **State-Felder lesen**: `re15-savestate-ghidra` →
   `python scripts/re15_enemy_state.py <sav> --ai` (Typ, +0x5, Grid, Clip, Distanzen je Slot;
   `--ai` ergänzt +0x90-Gate/+0x6a/+0x94/+0x1d4/+0x1dc). ⚠️ Die State-Labels dort sind die
   ZOMBIE-Map (@0x8011f7b4) — für einen neuen Typ erst die eigene Tabelle mappen (§1),
   dann das Script um die neue Label-Map erweitern.
3. **Dormanz-Check (Pflicht!)**: State-erreicht ≠ aktiv. Gate-Bits über ALLE Saves sweepen:
   `python scripts/re15_flag_sweep.py 0x800aca3c 0x1` — der Zombie-„Lunge-Arm" war über
   9 Saves dormant; der echte Angriff war der GRAB.
4. **Transiente States**: LB-Live-Save-Sampling (Hotkey `SaveSelectedSaveState = SDL-0/LeftShoulder`,
   SINGLE-Binding, kein `&`-Chord) bzw. `scripts/dyn_trace.py --state <sav> --path "U2,R0.3"
   --total 25 --interval 1 --outdir dyn_run` = ein Filmstreifen derselben Playthrough
   (Sampler-Thread + Fahr-Thread; `--mash N` = Grab-Escape-Probe; `--state` mit BACKSLASH-Pfad!).
5. **Unbekannte Globals finden**: `re15_diff.py` (Constraint-Diff über 2+ Saves) →
   `re15_ghidra.py <addr>` (headless-Decompile aller Referenzierer).
6. **Arbitrierung**: Widerspricht der Live-RAM dem statischen Befund, gewinnt der RAM —
   dann fehlt ein Writer (per-Tick-Writer entgehen Offset-Literal-Scans: der +0x1bc-Steer-
   Writer war EXE-seitig FUN_80039e7c in Pointer-Form).

## 4. Geteilte EXE-Infrastruktur — NICHT neu RE'en

Diese Helfer nutzt jeder Gegner; sie sind RE'd und im Port implementiert. Katalog-Status
(Stand 2026-07-05): ✓ = eigene Zeile in `RE15_FUN_CATALOG.md`, (m) = nur in-row erwähnt,
(r) = per Raw-Disasm 2026-07 RE'd, noch OHNE Katalog-Zeile — beim nächsten `ghidra-mapping`-
Durchgang nachtragen. Port-Fundstellen: `re15_port/engine/src/` — grep nach der Adresse.

| EXE-Adresse | Rolle | Port |
|---|---|---|
| 0x8001aac4 ✓ | Steer-toward (SIGNED slew; neg = away) | enemy_ai_common.c `re15_enemy_steer_point` |
| 0x8001a8f8 ✓ | Yaw-Slew (Snap bei Δ<2·slew) | mehrfach (turn/aim) |
| 0x8001a780 (m) | Seiten-/Arc-Test (front/behind) | re15_damage.c u.a. |
| 0x8001a6d4 ✓ | Bearing (atan2-Q12) | `re15_atan2_q12` |
| 0x8001f314 ✓ | Clip-Advance (+0x8f-Blend-Decay, POST-inc) | actors_anim_advance |
| 0x8001ad68 (m) | Root-Motion aus Keyframes | re15_emd_get_keyframe_speed |
| 0x8001f8b4 (m) | Frame-Wrap | monotonic-frame-Äquivalenz (s. §6) |
| 0x8002b544 / 0x8002b498 (r) | Body-Push-Loop / Contact-Clear (NACH Dispatch!) | run_all-Reihenfolge |
| 0x8003b0a4 ✓ | SCA-Wall-Clamp (auch Gegner; Slope-Typen 2/4-7 deferred) | re15_collision.c |
| 0x80019700 ✓ / 0x80019e20 ✓ | FX-Spawn / FX-Tick (Pool @0x800a73b8, 96×0x84). ACHTUNG Doppelrolle: 19700 schreibt beim Typ-Init auch die Hitbox-Dim-Pointer +0x78/+0x7c! | re15_esp.c + re15_damage.c (apply_hitbox) |
| 0x80045024 (m) | Se_on (arg-top-byte = Bank-Selector; byte[3] = Voice-Count-Gate) | re15_audio_* |
| 0x8003703c (r) | Autoaim-Latch (Radius als arg: 2000 melee / 30000 gun) | re15_player_aim_target |
| 0x80039e7c / 0x8003a524 (r) | Nav-Steer / DFS-Pathfind (RDT-BLK-Zonen; First-Hop in SHARED Globals 0x800AFBAE) | nav_zone_common.c |
| 0x8001bc08 (r) | LOS-Probe (16-Tick-amortisiert, FOV ±0x5e8 → +0x1d8 bit 0x10) | re15_enemy_los_probe |
| 0x80037024 (r) | Mash-Test (press-EDGE, Maske 0xf0f0; Fenster −(1+5·mash)) | re15_mash_pressed |
| 0x8002b5d0 ✓ | Hitbox-Test (Attack vs. Actor; Sektor-Pfad deferred) | re15_hitbox_test |
| 0x80011f50 (m) / 0x80012d60 ✓ | Waffen-Fire (SEPARATER Resolver!) / Damage-Entry | re15_damage.c |
| „0x80012fcc" | KEINE eigene Funktion — Instruktion IM Enemy-Branch von FUN_80012d60 | re15_enemy_take_damage |
| 0x8001af20 (m) | RNG: `(x + (x>>7)) & 0xff` über evolvierenden State | re15_engine_rand8 (Session-Entropie außer bei INPUT_SCRIPT/POSE_DUMP) |

## 5. Entity-Struct-Spickzettel (PSX-Offset ↔ Port-Feld)

Kanonische Quelle: `re15_port/include/re15_actor.h` (Kommentare zitieren die Offsets).
Die beim Zombie-RE bestätigten Kern-Offsets:

| Offset | Port-Feld | Bedeutung |
|---|---|---|
| +0x0 bit 0x1000 | — | Pair-Freeze (Grab-Exemption: AND beider Bits) |
| +0x4 / +0x5 | state / sub_state_1 | Root-State / Sub-State (State-Word 0xSSTT) |
| +0x6 / +0x7 | sub_state_2 / sub_state_3 | Sub-Step (Jump-Table; von jedem State-Word-Commit genullt) / Phase-Byte (Byte 3 des Words — 0x10201s drittes Byte skippt den Engage-ENTRY-Step!) |
| +0x9 | grid_id | Low-Nibble = Grid-Dispatch; bit 0x20 = Skip-Tick, bit 0x40 = stationärer Spawn, bit 0x80 = downed |
| +0x6a | rot_y | Yaw (0..4095) |
| +0x8f | anim_frac | Blend-Zähler (7 = ~8-Frame-Blend, Decay 1/Render-Frame) |
| +0x93 | hit_react | Hit-Flags (bit0 already-hit-guard @80012eec, bit1 2nd-hit-gore, bit7 front/back-Latch) |
| +0x94 / +0x95 | motion / anim_frame | Clip-Index / Frame (Original wrappt via f8b4; AI-Entries schreiben explizit) |
| +0x9a | hp | HP (signed s16!) |
| +0x9c / +0x9e | ai_timer / grab_kill_ctr | Countdown-FENSTER (Search-Decision, Engage-Wait, Grab-Escape 0x6e) / zweiter Window-Slot (Grab-Kill 0x64, Corpse-Pool-Budget 0x5a) |
| +0x90 | ai_contact | WALL-Kontakt-Byte (Writer = SCA-Resolver 0x8003b0a4) — ≠ Body-Kontakt! |
| +0x91 | repath_timer | Nav-Repath (Zyklus 7) |
| +0x1ac / +0x1c2 | contact_slot / contact_flags | BODY-Push-Kontakt: Entity-Ptr / Bits (|=1 Player, |=2 Enemy; Handler lesen den VORTICK — Clear an der b498-Position) |
| +0x1ba | — | GROUND-Y-Cache (NICHT HP — teure Falle) |
| +0x1bc / +0x1be | steer_x/z | Steering-Ziel (per-Tick von FUN_80039e7c) |
| +0x1d0 | ai_dist | Per-Tick gecachte Spieler-Distanz (SquareRoot0) |
| +0x1d4 | hurt_clip | Clip-Variante {2,3,4,5}[rng&7], EINMAL am Spawn (@0x8010079c), nie re-rollt |
| +0x1d5 | s_grab_mercy_timer | Mercy-Timer (Throw-off setzt 0x5a + DAT_800aca50-Bit) |
| +0x1d6 / +0x1d8 | ai_wp_node / ai_flags | Nav-Zone / AI-Flags (bit 0x8 roam-one-shot, bit 0x10 LOS „Spieler sichtbar") |
| +0x1da | ai_attack_timer | Lunge-Windup (feuert @0x12c=300) |
| +0x1dc | hit_stun / ai_target_x | **TIME-SHARED**: im Search-State = AI-Ziel-X (+0x1de=Z), im HURT = Poise (Seed (rand&3)+4, EINMAL pro Hit dekrementiert) |
| +0x1c4 | anim_flags | Anim-Flags (bit 0x80 = Reverse-Playback, Render honoriert) |
| +0x178 / +0x17c | bank | Anim-Bänke: dir[6]=EMR / dir[5]=EDD (bank2 = Victim-Set!) |

**EMD-Bank-Konvention**: bank0 = Locomotion, bank1 = Attack, bank2 = Victim-Set (animiert den
SPIELER während des Grabs — Leons PL00-Knochen + bank2-Keyframes), bank3 = Parts/Spezial.

## 6. Portieren: Konventionen aus enemy_ai_common.c

- **State-Words** via `re15_ai_set_state_word(e, 0xSSTT)` — exakt die Original-Kodierung.
- **Monotoner anim_frame** (kein Wrap): `frame=0` beim Clip-Wechsel IST byte-äquivalent,
  wenn das Original per f8b4 gerade gewrappt hätte — aber NUR dann (Grab-[2]-Regression).
- **rot_face(dx,dz) = (atan2_q12(dz,dx) − 0x400) & 0xfff = −θ_math** — Offline-Checks nicht
  mit θ−0x400 rechnen.
- **run_all-Reihenfolge** (byte-true): dispatch → fx → contact_flags=0 (b498-Position) →
  pushes → wall clamp (b0a4-Position). Handler lesen die Kontakte des VORTICKS.
- Verhaltens-/Gait-Blobs **verbatim aus der BIN einbetten** (`s_gait_blob`-Muster) statt
  sie zu „verstehen" — unbegrenzte Fetches inklusive (v12-Pointer-Bytes waren Verhalten).
- **Clip-Frame-Counts (EM0XX-EDD) VERBATIM einbetten** (`static const uint8_t s_dog_clip_len[28]={49,18,…}`,
  aus dem Modell-EDD via re15_emd_parse_animation), NICHT aus der render-geladenen Bank lesen. Grund
  (Crow-Falle, teuer): die AI an `re15_enemy_find(type)->anim` zu koppeln FROR den Gegner ein, sobald er
  off-camera war — die Bank lädt LAZY im Render-Loop (`pc_enemy_load`), also gab `re15_enemy_find` NULL →
  fc=1 → anim_frame wrappt jeden Tick → frame-N-Trigger (Re-Thrust/Connect) feuert nie. Zusätzlich den
  Gegner aus dem geteilten `re15_actors_anim_advance` AUSNEHMEN (`if (a->type==0xNN) continue;`) — sein
  Flug/Attack-Brain besitzt +0x95 selbst (byte-true 0x8001f314: POST-inc + Wrap an der echten Clip-Länge).
- **Grid-gated Handler FREEZE-sicher routen:** wenn ein byte-true Pfad in einen Sub-State führt, dessen
  Handler per Grid-Gate (`+0x9==0xNN`) für den häufigen Fall SOFORT exit't (leerer State → Standbild), darf
  der Port den nicht-passenden Fall NICHT einfrieren lassen — sicher zurück in den Chase/Active-Loop routen
  (Dog-Lunge→state-5-Pounce-Land war grid-0x43-gated; ein normaler Dog wäre sonst in state 5 erfroren). Das
  Original verlässt sich darauf, dass der häufige Fall den State gar nicht erst erreicht.
- **Zwei-seitiges Damage-Modell prüfen:** die EMPFANGENDE Seite (Hurt/Death/Corpse-States, meist [2]/[3]/[7],
  spiegeln den Zombie) hängt an der GETEILTEN `re15_enemy_take_damage` → der Gegner ist killbar „gratis" (Dog).
  ODER es gibt KEINE Death-Promotion im erreichbaren State (Crow: state-4+grid&0x40+bit-0x1f, nie STAGE1) →
  byte-true unsterblich. Für 100%-Portierbarkeit die unerreichbaren States TROTZDEM porten (mit einem
  Test-Trigger für den Gate-Flag), aber den STAGE1-Pfad nicht ändern.
- Jede Konstante mit Disassembly-Adresse im Kommentar; Approximationen als „faithful-line"
  markieren und den Mechanismus zitieren (nie als byte-true verkaufen).

## 7. Verifizieren: deterministische Port-Proben + Parity

**Schnellste Schleife (kein DuckStation): `re15-room-probe`** — C-Test-Template
(`re15_port/tests/unit/test_room1140_combat.c` als Vorlage): Raum-RDT laden, SCD starten,
`re15_enemy_ai_run_all` ticken, `g_actors` direkt prüfen. Gotcha: Test-Gegner isolieren
(andere auf x=z=30000, grid_id=0x86 parken); die exe lockt sich selbst
(`taskkill //F //IM re15_pc.exe` vor dem Build).

Port-Harness (env — die exe ignoriert argv!; `re15_port/build/platform/pc/re15_pc.exe`):

```bash
RE15_START_ROOM=1140 RE15_PLAYER_POS="x,z,rot" \
RE15_INPUT_SCRIPT="MA6" \            # Tokens kombinierbar: U/D/L/R Tank, X=Cross(run) A=Square M=R1 W=wait B=Mash; "<Buchstaben><Sekunden>"; Lead-in RE15_INPUT_SCRIPT_START (Default 90 Frames)
RE15_STATE_LOG=probe.log \           # 1 Zeile/Tick: "F%u pad=%04x PL(x,z,rot,hp) mo ac fx sl" + je Gegner "[slot t st ss1 g mo af d @(x,z,r)]"
RE15_POSE_DUMP=pose.txt \            # + .leon: Leon-Render-Pose (vs/mo/af/frac/kf/ovr/rot + Bones b9/b13)
RE15_FX_LOG=fx.log \                 # per-Particle-Draw-Trace (id/eidx/frame/world/view/slot/q=Queue-Füllstand)
RE15_AUTOSHOT=1 RE15_AUTOSHOT_SERIES="98,114,2" ./re15_pc.exe   # Frames als shots/series_f*.bmp, auto-quit nach Serie
```

Weitere Harness-Hebel: `RE15_ENEMY_DIAG=1` (welches Modell rendert der Gegner wirklich —
diagnostiziert „statischer Leon"), `RE15_FORCE_MOTION=N` (nur Player-Clip pinnen),
`RE15_CLIP_TEST=N|ALL` (Pose-Katalog-Sweep), `RE15_FORCE_CUT=N`, `RE15_GOTO_ROOM=<hex>`,
`RE15_FORCE_EVENT=N`, `RE15_MOTRACE=1`, `RE15_FPS` (Default 30 = PSX-Kadenz).

- `RE15_INPUT_SCRIPT`/`RE15_POSE_DUMP` gesetzt ⇒ deterministischer RNG-Seed (Vergleichsläufe).
- Wichtige Proben-Fallen: Teleport-`z` wird von der Kollision GECLAMPT (Log lesen, nicht Soll
  annehmen); Auto-Aim trifft nur FRONT-Ziele (rot passend wählen); Kill-Proben brauchen
  Distanz > Grab-Range, sonst stirbt Leon vor dem ersten Messpunkt.
- **Parity-Orakel**: gleiches Manöver im Original (DuckStation: `dyn_trace.py` bzw.
  `re15_walk_probe.py`/`re15_advance.py` für reproduzierbare Messungen) und im Port
  (`RE15_STATE_LOG`) fahren, State-Timelines diffen. Divergenz = Port-Bug — erst Daten/Reader
  prüfen, dann Logik (`re15-parity-verify` für Invarianten + Frame-Matching).

## 8. Workflow-Rezept (das 10-Agenten-Audit-Muster)

Für einen kompletten neuen Gegner hat sich diese Orchestrierung bewährt:

1. **ENUMERATE** (1 Agent): alle Tabellen + Handler-Adressen + Grenzen (aus Bounds-Checks).
2. **CLUSTER** (6-8 Agenten parallel): je ein State-Cluster (Locomotion/Attack/Hurt/Death/
   Spezial/Contact/Sensorik/Gait-Daten) — jeder MUSS Disasm-Zitate liefern, kein Decompile-only.
3. **DYN-TRACE** (1 Agent oder selbst): Live-Timeline der Kern-Übergänge als Schiedsrichter.
4. **ADVERSARIAL VERIFY**: jeden „fertig"-Befund von einem Refuter angreifen lassen —
   zwei als „exhaustiv" gelieferte Befunde waren beim Zombie falsch (Steering-Snapshot,
   Forward-Walk-Modus). „Observably equal" ist NICHT der Mechanismus (Victim-Yaw-Lehre).
5. Pro Welle: portieren → ctest → deterministische Live-Probe → committen → Memory updaten.

## 9. Fallen-Katalog (teuer bezahlt — vor JEDER Welle lesen)

1. **Overlay-Offsets**: STAGE*.BIN ohne Header; +0x800 liest Garbage („runtime-patched"-Trugschluss).
2. **State erreicht ≠ aktiv**: Gate-Bits sweepen (Dormanz); der sichtbare Angriff kann ein anderer sein.
3. **Decompiles lügen**: FUN_80100424.c war komplett falsch; immer Bytes prüfen.
4. **Per-Tick-Writer entgehen Literal-Scans**: „nie geschrieben" heißt nur „kein Offset-Literal gefunden".
5. **Feld-Fehldeutungen**: +0x1ba ≠ HP; +0xbc ≠ Y-Sink; HP ist +0x9a (signed).
6. **Falsche Tabelle**: Bounds-Check ableiten, RAM-vergleichen (40/40-Methode).
7. **Render-Pfad separat verifizieren**: der motion-Wert kann stimmen und der gerenderte Clip
   trotzdem falsch sein (clip_override-Literal-Bug) — Pose-Dump + Screenshot, nicht nur FSM-State.
8. **Tri-Queue-Sättigung**: Effekte/Sprites, die „nicht erscheinen", können still gedroppt sein
   (TEXTRI_QUEUE_MAX; FX_LOG zeigt q=).
9. **JUMP-Nummern sind HEX** ('A' ≠ '4'); kurze Einzeldrücke (Auto-Repeat-Falle).
10. **DuckStation-Pfade**: `--state` braucht Backslash-Pfad (Forward-Slash → rc=0 sofort, stale Samples).
11. **vgamepad-Chords**: `&` in Bindings = CHORD, nicht OR; Shoulder-Buttons zuverlässig, L3/R3 nicht.
12. **Vor dem Bauen greppen**: existiert die Implementierung schon? (ESP-VM-Duplikat-Lehre.)
13. **Nachbar-Handler ≠ dieser Typ** (Crow-Falle, teuer): ein CLUSTER-Agent fand „Touch-Damage −2" in
    0x80116288 — das ist aber der ROOT von Nachbar-Typ 0x26, NICHT der Krähe. Der Adress-nahe Code gehört
    oft dem nächsten registrierten Typ. Der adversariale Verify prüfte die Adress-Bytes, aber NICHT die
    Typ-Zugehörigkeit. → Jeden gefundenen Handler gegen die Registrierungs-Tabelle (@0x8011e8xx, `sw v0,
    0x80072bXX`) prüfen: gehört die Adresse WIRKLICH zu deinem Typ-Slot?
14. **„Kein X" ist nur so gut wie die Scan-Vollständigkeit** (Crow): „die Krähe macht keinen Schaden" war
    FALSCH — die Damage lag in den noch nicht gescannten Flock-Angriffs-Substates (11/12/13/16 = −4/−8).
    Eine „kein-Damage/kein-Hitbox/kein-X"-Aussage braucht ALLE erreichbaren States inkl. der tiefen Subs.
15. **Damage/Hitbox sitzt im inner-jt, nicht im State-Handler** (Dog): die Biss-Damage (−10 HP) war in den
    State-Handlern „unresolved" — sie lag im INNEREN Jumptable der BITE-Sub-State (@0x801001ec, Connect-
    Frame anim_frame≥N). Zum tiefsten Nested-Step folgen, nicht am Top-Level aufgeben.
16. **Aliasing-Dispatch-Tabellen**: State-Tab (per +0x4) und ACTIVE-Sub-Tab (per +0x5) können ÜBERLAPPEN
    (Dog: B-Tab @0x80120f94 = state-tab+0x20). Dieselbe Adresse ist über zwei Pfade erreichbar → nicht als
    zwei Handler fehldeuten; den real gesetzten Index (+0x4 vs +0x5) als Primär-Pfad bestimmen.
17. **Killability stage-gated**: vor „State X unerreichbar/deferred" die Stage prüfen, die das Gate-Flag
    setzt (Crow-Death-bit-0x1f: nur STAGE3/5, nie STAGE1). Für 100%-Port trotzdem porten (Test-Trigger),
    aber den STAGE1-Pfad nicht ändern.
18. **Grid-gated-Freeze**: ein byte-true Pfad kann den häufigen Fall einfrieren (Dog-Lunge→grid-0x43-
    Pounce-Land; normaler Dog erfriert in state 5) — den nicht-passenden Fall sicher zurück routen (§6).
