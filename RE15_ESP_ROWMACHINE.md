# RE1.5 ESP Descriptor-Row Effect Machine — byte-true RE (port STAGED)

Der spielweite Effekt-Treiber: der `model_inst_pool` @`0x800a73b8` (96 Slots × 0x84 B), der
Spawner `FUN_80019700`, der Tick `FUN_80019e20`, die Routine-Tabelle `PTR_LAB_80071d40`
(Handler @`0x80017248`–`0x80017ed8`) und der Row-Advance `FUN_800174e4`. **Treibt ALLE Effekte**
(Mündungsfeuer, Rauch, Hülse, Gore, Blut, …). Der Port implementiert bisher **nur** den
Anim-Record-Frame-Timer-Teil (`re15_esp_fx_tick`, re15_esp.c:264) + eine eigene Projektion — die
**Row-VM, die Physik, den Anchor-Follow und den Row-Advance NICHT** → Effekte sehen beim Spawn
korrekt aus, **driften/fallen/faden aber nicht** über ihre Lebenszeit.

**Status:** Mechanismus byte-verifiziert (Workflow wf_e1ece79f-44a, 489k Token, + Direkt-Disasm
2026-07-06). Port = **STAGED** (bewusst NICHT halb-belegt committet). Die byte-true Row-Block-
Quelle im ESP-File + Routinen 10-19 + das Fade-Offset sind noch offen (siehe §5) — ohne die wäre
ein Physik-Seed geraten (verletzt „kein Beleg → nicht in den Code").

---

## 1. Der 0x84-Slot (byte-verifiziert, Arbiter-arbitriert)

Indexierung: `FUN_80019700` (uint `[N]`=byte 4N), `FUN_80019e20` (ushort `[N]`=byte 2N).
Bytes **0x00–0x27 = die aktive 40-Byte-DESCRIPTOR-ROW** (von `FUN_800174e4`/`FUN_80019700`
en bloc kopiert). Alles ≥0x28 = persistenter Runtime-State.

| Offset | Typ | Bedeutung |
|---|---|---|
| 0x00 | u16 | **routineA** — Loop-1-Dispatch-Index (×4 in Tabelle 0x80071d40) = der Row-OPCODE |
| 0x02 | u16 | **routineB** — Main-Loop-Dispatch-Index (Draw/Anim-Controller) |
| 0x0e / 0x16 / 0x1e | u16 | **Row-VELOCITY** X/Y/Z (Routine 6 akkumuliert diese in die Drift) |
| 0x20 / 0x22 / 0x24 | u16 | Billboard-EULER-Rotation (RotMatrix-Input; 0x22 += base-yaw@0x2e) — **NICHT** lineare Pos |
| 0x26 | u16 | **Advance-Gate** — nonzero ⇒ Routine 6 ruft nach der Drift den Row-Advance FUN_800174e4 |
| 0x28 / 0x2a / 0x2c | s16 | gezeichnete World-XYZ (SVECTOR-Ergebnis der GTE-Transform) |
| 0x2e | u16 | base-yaw-Seed (Spawn-param_2) |
| 0x30 / 0x32 | u16 | UV/CLUT-Seed (Port rechnet UV eh aus der Bank → optional) |
| 0x34 / 0x38 / 0x3c | s32 | lokale Translation (ApplyMatrix-Input) — SEPARATE Physik-Kette (s.u.) |
| 0x40 / 0x44 / 0x48 | **s32** | **DRIFT-Akkumulator** (Routine-6-Output + Spawn-Anchor); Draw-Basis |
| 0x4c..0x6b | 8 Words | Anchor-Matrix-SNAPSHOT (von *0x74 kopiert wenn flags bit2) |
| 0x6c | u16 | **status/flags**: bit0 active, bit2 re-follow-anchor, bit3 one-shot-init (XOR 0x9 = cleart AUCH active!), bit5 freeze-physics, bit6 freeze-frame, bit7 anchor-placement |
| 0x6d | s8 | Frame-Timer (Anim-Record) |
| 0x6e | u8 | Anim-Record-Index (← der Port tickt das schon byte-true) |
| 0x6f | u8 | **Row-Index** (Row-VM-Cursor, getrennt vom Anim-Index) |
| 0x71 | u8 | group |
| 0x72 | u16 | **scale16** (Port setzt das schon; liegt AUSSERHALB der Row → sicher) |
| 0x74 | int | Anchor-Matrix-POINTER (param_3) |
| 0x78 | int | Anim-Record-Array-Basis (← Port nutzt das schon) |
| 0x80 | int | **Descriptor-ROW-Array-Basis** (Row-VM-Quelle; ≠ 0x78!) |

**Arbiter-Korrekturen zweier Fehl-Ableitungen:** (a) Row-Basis @**0x80**, nicht 0x40 (die
`FUN_800174e4.c`-Decompile-`0x40` war ein ushort-Typing-Fehler; roh `8001751c: lw v1,128(a3)`).
(b) 0x18/1a/1c = **Winkel**-Spin (nicht lineare Velocity); 0x20/22/24 ist Euler-Rotation.

## 2. Der Tick `FUN_80019e20` (byte-verifiziert)

```
Pause-Gate: return wenn 0x800aca40 & 0x10000000  (Spiel pausiert)
Iterator: ptr-Global 0x800b52c4, von pool-base bis base+0x3180 (=96×0x84)
LOOP 1 (@0x80019e84): je aktiver Slot (flags bit0) → routine_table[slot+0x00]()   // der Row-OPCODE
MAIN LOOP (@0x8001a2..):
  (i)   bit3 one-shot-init-Routine, dann XOR 0x9 (cleart bit3 UND active)
  (ii)  bit2 → Anchor-Matrix von *slot+0x74 nach slot+0x4c re-snapshoten
  (iii) RotMatrix(euler@0x20) + ApplyMatrix(xlat@0x34 + anchor-trans) → World-XYZ @0x28
  (iv)  routine_table[slot+0x02]()   // Draw/Anim-Controller (@0x8001a2b4)
  (v)   Physik (gated bit5==0): accel@0x08→drift@0x10→xlat@0x34, angvel@0x18→euler@0x20
  (vi)  Anim-Record-Timer (@0x6d/0x6e, Basis @0x78) — DER Port-Teil, byte-true
  (vii) Row-Advance FUN_800174e4 wenn Row-Timer/Gate abläuft
```
Helfer: RotMatrix `0x80068098`, ApplyMatrix `0x800661c0`.

## 3. Die Routinen `PTR_LAB_80071d40[0..19]` (0-9 RE'd, 10-19 offen)

| Idx | Addr | Klasse |
|---|---|---|
| 0 | 0x80017248 | NO-OP/hold (jr ra) |
| 1 | 0x80017250 | DRAW (pokt Draw-Byte +0x6c/6e) |
| 2 | 0x800172a0 | SPAWN (timed; per-Row-Countdown +0x0e/16) |
| 3 | 0x80017334 | DRAW (delayed) |
| 4 | 0x800173a8 | DRAW (2-Phase) |
| 5 | 0x80017434 | DRAW (alt-Feld) |
| **6** | **0x80017484** | **PHYSIK (DRIFT)** ✅ byte-verifiziert |
| **7** | **0x800174e4** | **ROW-ADVANCE** (`slot[0x6f]++`, memcpy next 40B von base @0x80 nach 0x00-0x27) |
| **8** | **0x800175dc** | SPAWN+DRAW (Muzzle: `flags=row[0x0e]`, chained Sub-Spawn 0x02040bb8, ApplyMatrix) |
| **9** | **0x80017654** | **SE (BANG)**: `FUN_80045024(0x01000001,&pos)` + noise-Latch `0x800b5358=1` + Row-Advance |
| 10-19 | 0x800176b0..0x80017d08 | **OFFEN** (Agenten am Encoding gescheitert; incl. Lunge-Action FUN_80017fa4 = idx 0x19) |

**Routine 6 = die fehlende Physik (roh @0x80017484 verifiziert):**
```c
slot[0x40] += (u16)slot[0x0e];   // 8001749c-a8: lhu 14 + lw 64 -> sw 64
slot[0x44] += (u16)slot[0x16];   // lhu 22 + lw 68 -> sw 68
slot[0x48] += (u16)slot[0x1e];   // lhu 30 + lw 72 -> sw 72
if (slot[0x26]) FUN_800174e4();   // 800174c4: advance-gate -> row-advance
```
Also: der **DRIFT-Akkumulator @0x40/44/48 (s32) += die Row-Velocity @0x0e/16/1e (u16)**, und die
Row schaltet weiter wenn +0x26 gesetzt. **KEIN Gravity/Accel in 0-9**, **kein kontinuierlicher
Alpha/Scale-Ramp** — Fade/Scale ist rein datengetrieben über die Row-Abfolge (Rows 1/3/4/5 poken
Draw-Bytes). Das ist die **Waffen-Effekt-Physik** — NICHT die tick-interne accel@0x08-Kette
(die ist ein zweiter Pfad für andere Effekt-Typen).

## 4. Spawner `FUN_80019700` (byte-verifiziert)

`a0` = packed: category = `a0>>24`, sub = `(a0>>16)&0xff`, scale16 = `a0&0xffff`. `a2` = Anchor-
Matrix, `a3` = lokaler Offset-Vektor. Sucht freien Slot (flags==0, cap 0x60), init flags=**0x03**,
Anchor von a2 (→0x74 + Snapshot 0x4c), Offset von a3, kopiert Row 0. **Zwei RUNTIME-Bank-Tabellen**
`0x800b22d4` (category-Block-Basen) + `0x800b2248` (Sub-Count-Tabelle), beide beim ESP-Load gesetzt
→ zeigen in die geladene ESP im RAM. Der Row-Block liegt IM ESP-File, indexiert per (category, sub):
`t6 = *(0x800b22d4 + cat*4); v0 = lhu(t6 + sub_lo3*2); rows = t6 + v0*4 + 4; count = lhu(rows-4)`.
Variante `FUN_800199d4` (child-Spawn) init flags=**0x0a**.

## 5. Port-Plan (STAGED) + OPEN

**Was der Port BRAUCHT** (Arbiter-Spec, PC-first, minimal-risk):
- `re15_esp_fx_t` erweitern: `routineA/B` (u16), `status` (u16), `driftvel[3]` (=Row +0x0e/16/1e),
  `drift[3]` (s32, =Slot +0x40/44/48), `row_idx` (u8), Row-Array-Ptr, Anchor (owner-actor + local
  offset, da PC keinen Pose-Buffer hat). OPEN: Fade-Byte (Offset nicht auflösbar).
- Tick → Two-Loop-Dispatch (§2); Physik = Routine 6 (§3); Draw addiert `drift[3]>>12` auf f->x/y/z.
- Routine-Tabelle IDENTISCH zu 0x80071d40 indexieren (nicht umnummerieren); nur die Routinen bauen
  die die Waffen-Rows selektieren (6 Drift, 7 Advance, 8 Muzzle, 9 SE-Bang, + Stubs).
- Danach: `s_bang_delay` (game_step_common.c:127) LÖSCHEN → wird datengetrieben durch Routine 9.
  `s_shell_pending` **NICHT** anfassen (separates recoil-clip-keyed Subsystem, korrekt in game_step).

**OPEN (Belege fehlen noch — vor dem byte-true Port zu schließen):**
1. **Der ESP-Row-Block-PARSE** — **Standort byte-verifiziert (Loader @0x80019460)**: der Loader füllt
   `DAT_800b2248[id]` = Effect-Body-Ptr und `DAT_800b22d4[id]` = **ROW-BLOCK-Ptr** = `body +
   (count_a·2 + count_b + 2)·4` (@0x800194c0-e0: `(count_a<<1 + count_b + 2)<<2`). Das ist =
   `body + 8(header) + count_a·8(anim) + count_b·4(coord)` = **direkt NACH den Coord-Records** — genau
   hinter dem Block, den re15_esp.c bereits parst. Der Port muss also nur den Parser um den dritten
   Block (ab `coord_end`) erweitern. **Noch offen = das Row-LIST-Layout innerhalb des Blocks**: der
   Spawner (@0x80019730-78) indexiert 2D per (sub&7)→u16-Offset-Tabelle @block-start und (sub>>3)·64;
   je Sub: `count = lhu(rows_base)`, dann `count` × 40-Byte-Rows. Nächster Schritt: das Sub-Offset-
   Tabellen-Format + die 40-Byte-Row-Felder direkt aus CORE00.ESP (ab coord_end je Effekt 2/3/4/0)
   dumpen und gegen `re15_esp_anim`/`re15_esp_coord`-Layout verifizieren; dann die echten Muzzle-Row-
   Velocities (@Row +0x0e/16/1e) + Routine-Selektoren (@Row +0x00/0x02) auslesen.
   ✅ **ROW-LISTEN-INDEXIERUNG byte-exakt RE'd (Spawner @0x80019728-80, 2026-07-06):**
   ```
   cat = a0>>24;  sub = (a0>>16)&0xff
   rowblk = DAT_800b22d4[cat]                         // = body + (ca*2+cb+2)*4 = coord_end
   sub_off = lhu(rowblk + (sub&7)*2)                  // 8×u16 Sub-Offset-Tabelle @rowblk
   base = rowblk + sub_off*4                          // <-- der *4 (sll v0,v0,2 @0x80019770)
   count = lhu(base);  rows = base + 4                // count×40-Byte-Rows
   // + Addend (sub>>3)*0x40 für sub>=8 (Waffen-FX nutzen sub<8)
   ```
   Damit parst CORE00.ESP SAUBER (kein Garbage mehr): id 2 (muzzle) sub0 count=2 vel=(0,20,0);
   **id 4 (shell) sub1 vel=(10,−50,0) = Abwärts-Drift**; id 3 (smoke) vel=(0,20,0) = aufwärts.
   Row-Feld-Offsets: Velocity @+0x0e/16/1e (von Routine 6 gelesen = bestätigt), Advance-Gate @+0x26.
   **ABER: die Waffen-Rows selektieren Routinen 0/2/3 (noop/spawn/draw), NICHT Routine 6 (Drift)** —
   die Drift ist also für ANDERE Effekt-Typen (Blut/Gore-Fall), nicht das Mündungsfeuer. Die Row-+0x00-
   Werte sind teils >19 (z.B. 3372) = „Daten-Rows" die eine Multi-Row-Routine liest, NICHT jede Row
   ist ein Opcode. → Der byte-true Port braucht die exakte Row-VM-SEMANTIK (welche Row Opcode vs Daten)
   + die Routinen-Bodies 0-19, um zu wissen WAS je Effekt tatsächlich läuft. Parser-Einstieg + Indexierung
   sind gelöst; die VM-Interpretation ist der offene Kern.
2. **Routinen 10-19** (@0x800176b0..0x80017d08): scan+dis je Routine (die 2 Workflow-Agenten scheiterten
   am StructuredOutput-Encoding, NICHT an der RE — Weg wechseln: Direkt-Disasm wie Routine 6).
3. **Fade/Alpha-Offset**: von einer un-decompilten Routine 0-19 geschrieben; nach §5.2 pinnen.
4. **Physik-Modell-Reconcile**: tick-accel-Kette (0x08→0x10→0x34) vs Routine-6-Drift (0x0e→0x40) —
   welcher Pfad je Effekt-Typ? Die Waffen-Rows selektieren Routine 6 (verifiziert); der accel-Pfad
   ist für andere Typen (Row-Velocity-Quelle bestätigt via die tatsächlichen Row-Bytes, OPEN 1).

**Warum staged statt jetzt committen:** Die Effekte rendern beim Spawn bereits korrekt (visuell
verifiziert). Der Gap (Drift/Fall/Fade über die Lebenszeit) ist Politur, kein Bug. Ein byte-true
Physik-Seed braucht die echten Row-Velocities (OPEN 1) — die ohne den Row-Block-Parse zu raten
verletzt die RE-Disziplin. Der Mechanismus ist hier vollständig belegt; der Port folgt byte-true,
sobald OPEN 1-3 geschlossen sind (klar umrissener nächster Arbeitsblock).
