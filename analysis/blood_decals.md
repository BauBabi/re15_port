# Blut-Decals auf der Spieler-Textur (RE1.5 MZD) — Apply-Mechanismus, Trigger, Persistenz, Port-Plan

**Datum:** 2026-08-02 · **Status:** Apply + Trigger + Persistenz **byte-belegt & savestate-verifiziert**
**Quellen:** `info/Re1.5/PSX.EXE` (t_addr 0x80010000; Datei-Offset = addr−0x80010000+0x800), `info/Re1.5/PSX/BIN/STAGE{1..6}.BIN` (@0x80100000, roh), `stage_saves/mzd_blood_decals_hp30.sav` (Ground Truth, Leon HP30 blutig), `stage_saves/mzd_stage1_briefing.sav` + `mzd_stage1_engage_live.sav` (Referenz sauber), `re15_port/shared_assets/PSX/PLD/PL00.PLD`.
**Korrigiert:** `RE15_KNOWLEDGE.md` §5.6 („Apply-Funktion nie lokalisiert") und `analysis/leon_injured_model.md` §0 („dritte blutige Texture-Page … kein Nutzer im Code gefunden") — **der Nutzer ist gefunden**.

---

## 0. Executive Answer

Die „zerrissene blutige Kleidung" ist ein **GPU-VRAM→VRAM-Blit-System aus 8 vorgebauten `DR_MOVE`-Primitives** (libgpu `SetDrawMove` @0x8006b824), Tabelle **@0x800b10ec** (8 Einträge × 0x1c). Quelle ist die **im PLD-TIM mitgelieferte Damage-Bank** (Texture-Page 2 des Spieler-Streifens, VRAM x=576..639: 4 Spalten × 16hw, 4 Bänder × 64 Zeilen = 2 Wund-Stufen), Ziel sind 8 feste Panels der sichtbaren Body-Pages (Per-Charakter-LUT **@0x80074208**). Getriggert wird **pro Körper-Panel über einen Wund-AKKUMULATOR** (`FUN_80037edc(panel, amount)`, Schwelle **0x78=120**): Die Stage-Overlays rufen ihn aus der Spieler-Hurt-Reaktion mit +10/+50 pro Treffer; erst wenn ein Panel kumulativ ≥120 gesammelt hat, springt seine Wund-Stufe (0→1→2) und der Blit wird in die aktuelle OT eingehängt. **Kein LoadImage, kein CPU-Pfad — der GPU kopiert die Indizes roh.** Persistenz: nur in VRAM + der RAM-Tabelle @0x800b10ec; Spieler-Load baut die Tabelle neu (= Save+Load verliert das Blut, exakt der §5.6-Befund). Heilung löscht NICHTS.

---

## 1. Korrektur der Ausgangs-Prämissen (Ground-Truth-Messung)

Die Aufgabenstellung enthielt vier Fehlannahmen, die die Suche zunächst fehlleiteten — alle savestate-gemessen widerlegt:

1. **„3194 gestempelte Pixel"** = Diff des Bloody-Save gegen die On-Disc-`PL00.TIM` — davon sind **1792 Byte der Waffen-Composite-Block** (28×32hw @ VRAM (548,480), in ALLEN Saves vorhanden; Upload durch den PLW-Loader `FUN_80036b68`, prect `@0x80036c74`: x=`DAT_800acad4`·0x40−0x39c, y=0x1E0, w=0x1C, h=0x20). Die **echten Wund-Stempel sind 1402 8bpp-Byte** (Diff bloody↔brief), VRAM x449..479, y258..337.
2. **„Farbindizes überwiegend 224..239"** — das ist die Waffen-Palette: `FUN_80036b68` lädt den 16-Entry-CLUT-Block der PLW nach **(x=0xE0=224, y=0x1E1+`DAT_800acad5`, w=0x10)** (`@0x80036cb8`/crect-Setup `@0x80036ee8-f04` im Raw-Duplikat 0x80036dc0). Die Wund-Stempel benutzen Indizes ~0x8F..0xDD (Body-Palette).
3. **„Mesh-CLUTs 0x7840→0x7880 gepatcht"** — präziser: `FUN_80022150(2, md1, 0x17, 1)` (Aufruf `@0x800315c8`/`@0x8003182c`) addiert auf JEDES Prim tpage+=0x17 und clut+=0x40 (`*piVar9 += param_4*0x400000` = clut+1 Zeile). MD1-Basen sind **0x7800 (Page 0x80) und 0x7840 (Page 0x81)** → Laufzeit **0x7840 (Zeile 481 = TIM-CLUT-Row 0) und 0x7880 (Zeile 482 = Row 1)**. `FUN_8004ee78` lädt die 3 TIM-CLUT-Rows nach **y = 0x1E0+`DAT_800aca4d`(=1) = 481..483** (`@0x8004ef2c` crect->y-Setup). Row 483 (TIM-Row 2) bleibt unreferenziert.
4. **„0x800A73B8 = Partikel-Slots / 0x800B25CC = Sprite-State"** (§5.6-Anker) — `DAT_800a73b8` ist der **Model-Instance/Action-Pool** (96×0x84 bis 0x800aa538, Treiber `FUN_80019e20`, Katalog-belegt), `DAT_800b25cc` ist ein **Subscreen/SCD-Byte** (einziger Writer = SCD-Handler `LAB_80043194`: `sb v0,DAT_800b25cc` `@0x800431c0`; einziger Reader `@0x8004c670` im Subscreen-Mode-1-Init `LAB_8004c54c`). Beide haben mit den Decals nichts zu tun.

**Der echte Stempel-Befund** (Diff bloody↔engage, `blood_probe5.py`): zwei rect-treue Kopien mit Translation **(+128hw, +38y)** und **(+176hw, +64y)** — Quelle jeweils die Damage-Bank in Page 2. Exakt (s. §3.4): src (592,320)→dst (464,282) und src (624,320)→dst (448,256), je 16hw×64.

---

## 2. (A) DIE APPLY-FUNKTION — `DR_MOVE`-Blits, byte-belegt

### 2.1 Builder: `FUN_80037c1c` (baut die 8 Prims beim Spieler-Load)

```
80037c30 addiu s1,s1,4332        ; s1 = 0x800b10ec  (DR_MOVE-Tabelle, 8 × 0x1c)
80037c38 addiu s3,s1,-17944     ; s3 = 0x800acad4  (tpage-Slot des Spielers = 0x17)
80037c48 sb   zero,0(s1)        ; entry+0  = Wund-LEVEL := 0
80037c50 ori  v0,zero,0x10
80037c54 sh   v0,23(s0)         ; entry+24 = w := 0x10 (16 Halfwords = 32 8bpp-px)
80037c58 ori  v0,zero,0x40
80037c60 sh   v0,25(s0)         ; entry+26 = h := 0x40 (64 Zeilen)
80037c64 andi v0,a1,0x3 / sll 4 / addiu 128
80037c78 addiu v1,v1,-16 / sll 6 / addu
80037c84 sh   v1,15(s0)         ; entry+16 = srcX := (slot-16)*64 + (i&3)*16 + 128
                                ;          = 448+128 + (i&3)*16 = 576/592/608/624
80037ca8 lbu  v0, 0x80074208[char*16 + i*2]
80037cb4 sh   v0,19(s0)         ; entry+20 = dstX := LUT_x + (slot-16)*64 = 448+LUT_x
80037cd4 lbu  v0, 0x80074209[char*16 + i*2]
80037cdc addiu v0,v0,256
(80037ce4) sh v0,21(s0)         ; entry+22 = dstY := 256 + LUT_y
80037ce0 jal  0x8006b824        ; SetDrawMove(p)  (tag/code = GP0-0x80-Blit-Paket)
80037ce8 sb   zero,0(s0)        ; entry+1  = Wund-AKKUMULATOR := 0
```

**Entry-Layout** (Basis `0x800b10ec + i*0x1c`): `+0` Wund-Level (0/1/2), `+1` Akkumulator, `+4` DR_MOVE-Prim (tag `+4`, code[] ab `+8`; `+16` srcX, `+18` srcY, `+20` dstX, `+22` dstY, `+24` w, `+26` h). **srcY wird beim Bau NICHT gesetzt** — das macht der Trigger (Band-Auswahl).

**Caller (Binary-Scan, jal `9e3b010c`-analog `0x0C00DF07`):** nur die beiden Spieler-Load-Instanzen `@0x800316c8` und `@0x800318cc` (Leon-/Zweit-Loader, beide Teil des `FUN_800314b0`-Pfads mit Slot 0x17/CLUT-Delta 1) → **die Tabelle wird bei jedem Spieler-Load auf Level 0 zurückgesetzt.**

### 2.2 Ziel-Koordinaten: LUT `@0x80074208` (Datei-Offset 0x64a08; 4 Charaktere × 8 (x,y)-Paare, Halfword-Koordinaten)

| char | p0 | p1 | p2 | p3 | p4 | p5 | p6 | p7 |
|---|---|---|---|---|---|---|---|---|
| 0 (Leon) | (67,35) | (43,44) | (31,192) | (66,175) | (22,116) | (16,26) | (6,102) | (0,0) |
| 1 | (67,35) | (43,44) | (31,192) | (66,175) | (23,99) | (7,34) | (7,115) | (24,6) |
| 2 | (67,35) | (43,44) | (31,192) | (93,113) | (16,117) | (0,40) | (0,109) | (16,11) |
| 3 | (67,35) | (48,44) | (31,192) | (22,134) | (66,175) | (16,26) | (6,102) | (0,0) |

dst = (448+x, 256+y). Panels p0/p3 liegen in Page 1 (x≥64), der Rest in Page 0. Leon-p5/p7 decken die Torso-UV-Region von MD1-Mesh 0 (u0..95/v0..250, Page 0x80) ab.

### 2.3 Quelle: die Damage-Bank in Page 2 (x576..639) — Band-Formel

Aus dem Trigger (§2.4) und dem Re-Insert-Loop `LAB_80037d1c`:

```
srcX = 576 + (panel & 3)*16            ; 4 Spalten: 576/592/608/624
srcY = level*128 + 128 + (panel>=4 ? 0x40 : 0)
     ; level=1: y=256 (Panels 0..3) / y=320 (Panels 4..7)
     ; level=2: y=384 (Panels 0..3) / y=448 (Panels 4..7)
```

Die Bank ist Teil des normalen `PL00.PLD`-TIM (Pixel-Array @PLD-Offset 0x162A4, Stride 384 B; identisch in `PL00.TIM` — die Stempel-Sequenzen existieren auf der ganzen Disc NUR dort). Sie wird vom Mesh nicht referenziert (reiner Storage; bestätigt `analysis/leon_injured_model.md`, dort aber als „ohne Nutzer" fehlgedeutet).

### 2.4 Trigger-/Blit-Funktion: `FUN_80037edc(panel, amount)` — der Wund-Akkumulator

```
80037edc sll/sra a0,24            ; panel (s8)
80037ee4..ec v0 = panel*28
80037ef4 addiu a3,a3,4332         ; 0x800b10ec
80037efc lbu  v0,1(t0)
80037f04 addu v0,v0,a1
80037f08 sb   v0,1(t0)            ; AKKU += amount
80037f14 sltiu v0,v0,0x78
80037f18 bne  → return            ; < 120 → nur akkumulieren, KEIN Stempel
80037f24 sb   zero,1(t0)          ; Akku := 0
80037f28 addiu v0,v0,1
80037f2c sb   v0,0(t0)            ; LEVEL += 1
80037f40 sltiu v1,v1,0x3          ; Level-Clamp:
80037f50 sb   0x2,0(t0)           ;   >=3 → 2 (zwei sichtbare Stufen)
80037f48 sll  a0,v0,6             ; (panel>=4)*0x40
80037f68 sll  v0,v0,7 / +128 / +a0
80037f74 sh   v0,18(t0)           ; srcY = level*128+128+(panel>=4)*0x40
80037f7c lbu  v0, DAT_800aca34    ; Frame-Flip
80037f84 sll 6 / addu 0x800ab5ec  ; OT-Basis + flip*64
80037f8c..fc4                     ; Prim in die OT einhängen (tag-Link-Splice,
                                  ;   beide Buffer) → GPU führt GP0(0x80)-Blit aus
```

Der Blit kopiert **rohe 8bpp-Indizes** 16hw×64 von der Bank auf das Panel — CLUTs bleiben unangetastet (die Bank ist gegen die normalen Body-CLUT-Rows 481/482 ausgemalt).

### 2.5 Nebeneingänge (beide ohne Caller = tot/Debug)

- `LAB_80037d1c`: Re-Insert aller Prims mit `flag≠0` (srcY neu berechnet, dann OT-Splice) — **kein jal, kein Daten-Pointer** in EXE+allen Overlays.
- `LAB_80037de4`: setzt ALLE 8 Levels := Byte `@0x80074288` (EXE-DATA, initial 1), blittet alle, und inkrementiert das Byte (Zyklus 1→2→1, `@0x80037e9c-ed0`) — ebenfalls **ohne Caller** (Debug-„alles-verwunden"-Knopf).
- Verwandt, aber getrennt: eine zweite DR_MOVE-Tabelle **@0x800b267c** (Stride 0x34, Builder um `@0x8002bb04-bb88`, per-Typ-Daten @0x80073c6d/6e) im Entity-Loader-Umfeld — mutmaßlich dasselbe System für **Gegner**-Texturen; dritter SetDrawMove-User `FUN_80020b04` (Boot). Beide hier nicht weiter verfolgt (→ Offene Punkte).

---

## 3. (B) DER TRIGGER — Spieler-Hurt-Reaktion in den Stage-Overlays

### 3.1 Dispatch (STAGE1, Spieler-Hurt-Entry)

```
8010a580 lbu  v0, DAT_800aca59    ; = Spieler+0x5 (Hurt-Substate)
8010a594 sb   0x7, DAT_800acae3   ; Spieler+0x8f
8010a598 sll  v0,v0,2
8010a5a0 addiu at,at,0x1b8        ; Tabelle 0x801201b8 (= 0x80120190[10..13])
8010a5a8 lw   v0,0(at)
8010a5b0 jalr v0                  ; → Wund-Helper[Substate]
```

### 3.2 Die Helper (STAGE1; jeder ruft `FUN_80037edc(panel, amount)`)

| Substate (+0x5) | Helper | Panels (amount) |
|---|---|---|
| 0 | `0x8010a1cc` | 0 (+10), **5 (+50), 7 (+50)** — Front-Torso schwer |
| 1 | `0x8010a208` | 0 (+10), 4 (+50), 7 (+50) — Rück-Torso schwer |
| 2 | `0x8010a244` | 1 (+50) |
| 3 | `0x8010a268` | 2 (+50) |

(`FUN_80012d60` schreibt im Standard-Hit +0x5 = front/back+2 = **2/3** `@0x80012ea4-f04`; die Writer von 0/1 = Schwer-/Knockdown-Pfad, s. Offene Punkte.)

### 3.3 Weitere Wund-Quellen (jal-Zensus `FUN_80037edc`, Binary-Scan aller BINs)

- **STAGE1**: zusätzlich `0x8010f360` (+50), `0x80111b64` (+10), `0x80111e78/84/90` (+10/+50/+50), `0x8011620c/2c/38/44/60/70` (+10er, distanz-gegated `slti 1500` @0x80116200 — Krähe?), `0x8011c55c/68/74` (+10/+50/+50).
- STAGE2 (10 Sites), STAGE3 (17), STAGE4 (21), STAGE5 (21) — gleiche Muster (per-Gegner-Attack-Handler). **STAGE6: NULL Sites.** EXE: keine.

### 3.4 Ground-Truth-Verifikation (mzd_blood_decals_hp30.sav)

Tabelle @0x800b10ec im Savestate:

| Panel | Level | Akku | src (VRAM) | dst (VRAM) | w×h |
|---|---|---|---|---|---|
| 0 | 0 | **30** | — | (515,291) | 16×64 |
| 5 | **1** | 0 | **(592,320)** | (464,282) | 16×64 |
| 7 | **1** | 0 | **(624,320)** | (448,256) | 16×64 |
| übrige | 0 | 0 | srcY=0 (nie gefeuert) | LUT | 16×64 |

= exakt **3 × Helper-`0x8010a1cc`** (Substate 0): Panel 0: 3×10=30 <120 (unsichtbar); Panels 5/7: 3×50=150 ≥120 → Level 1, srcY=320. Der VRAM-Diff (1402 Byte in genau diesen zwei 16hw×64-Rects, Quelle Bank-Band y320) stimmt Byte für Byte. In den Clean-Saves: alle Level/Akku 0, srcY 0.

**Warum stempelte der 15s-Grab-Biss-Test (100→10 HP) NICHTS:** Der Grab-Biss-Damage-Loop läuft nicht über den Hurt-State-Entry (§3.1) — und selbst ein einzelner Hurt-Treffer (+50 < 120) bleibt unter der Schwelle. Sichtbares Blut braucht **≥3 akkumulierte Treffer auf dasselbe Panel** (bzw. 2 nach Vorschaden). Der Trigger IST Schaden — aber akkumuliert pro Körper-Panel mit Schwelle 120, gefeuert nur aus dem Hurt-Reaktions-Pfad der Overlays.

---

## 4. (C) UV-MAPPING — es gibt keins zur Laufzeit

Keine Treffer-Position→UV-Berechnung: Die Panel-Geometrie ist **vollständig statisch** (LUT @0x80074208 + Spalten/Band-Formel). Die Zuordnung Körperteil→Panel steckt ausschließlich in den **per-Attack-Helpern der Overlays** (welcher Angriff welches Panel akkumuliert) und im Art-Layout des TIM (Panels = Ausschnitte der Body-UV-Inseln; Leon p7=(0,0)-(31,63) und p5=(32,26)-(63,89) liegen in der Torso-Insel von MD1-Mesh 0).

---

## 5. (D) PERSISTENZ

- **Raumwechsel: Stempel bleiben.** Kein Code lädt das Spieler-TIM beim Raumwechsel neu (Upload nur im Spieler-Load-Pfad `FUN_800314b0`/`0x800317xx` via `FUN_8004ee78`, Slot 0x17). Die RAM-Tabelle (Level/Akku) bleibt ebenfalls stehen — Wunden akkumulieren über Räume hinweg weiter.
- **Save+Load: Stempel weg** (bestätigt §5.6): Spieler-Load ruft `FUN_80037c1c` (`@0x800316c8`/`0x800318cc`) → Level/Akku := 0, TIM frisch von CD; die Wund-Tabelle steht in keinem Save-Record. `LAB_80037d1c` (Re-Apply) wird nie gerufen.
- **Subscreens zerstören nichts:** Karte (Mode 1, `FUN_8004c058`: StoreImage page0→0x801a0000, Map-TIM aus File-Tabelle @0x80074c4c[`DAT_800b260e`] via `LAB_8004c328`→0x801a8000, Restore bei Exit) und Mode 2 (`LAB_8004d7cc` @0x8004d7fc) sichern/restaurieren die Page **inklusive** Stempel (Backup nach dem Stempeln).
- **Heilung: löscht NICHTS.** Writer-Zensus auf entry+0/+1: nur Builder und `FUN_80037edc`. (`FUN_8004ed6c` HP-Status @0x8004ed6c — ≥0x50→0, ≥0x14→1, sonst 2, Flag&2→3 — steuert nur Inventar-Portrait v=base+status*16 + CLUT-Tabelle @0x800b2610, `@0x800479ec-80047a54`.)
- **Level-Cap:** 2 Stufen (Band 2/3); Level clamp @0x80037f40-50.

---

## 6. (E) PORT-IST & Implementierungsplan

**IST:** Der Port hat **nichts** davon. Spieler-Textur = Slot-0-Atlas als dekodiertes RGBA (`render_pc.c`: Stash `@~:1845`, Weapon-Composite `re15_render_pc_composite_slot0` `@~:254`). `re15_player_take_damage` (re15_damage.c, Port von FUN_80012d60) setzt +0x5, hat aber keinen Wund-Hook. Kein DR_MOVE/VRAM-Blit-Äquivalent, keine Wund-Tabelle, die Damage-Bank-Pixel liegen ungenutzt im geladenen TIM.

**Plan (D1..D6):**
- **D1 — Wund-Tabelle:** `re15_wound_t {u8 level; u8 acc;} g_wounds[8]` + Konstanten aus §2 (Schwelle 0x78 @0x80037f14, Clamp 2 @0x80037f50, LUT @0x80074208 als `.inc`, srcX/srcY-Formeln @0x80037c64-84/0x80037f48-74). Reset im Spieler-Load (Äquivalent `FUN_80037c1c`).
- **D2 — Blit:** `re15_wound_apply(panel)` kopiert 32×64 8bpp-Indizes (16hw) im **Index-Atlas** (vor RGBA-Dekode) von (srcX,srcY) nach (dstX,dstY) — Atlas-Koordinaten relativ (448,256); danach Slot-0/1-Textur-Rebuild wie beim Weapon-Composite. Achtung: Panels p0/p3 liegen in Page 1 → auch Slot 1 rebuilden.
- **D3 — Trigger-Hook:** `re15_wound_add(panel, amount)` (Äquivalent FUN_80037edc) aus dem Hurt-Entry rufen: Dispatch `player+0x5` → Helper-Tabelle pro Stage (STAGE1-Werte §3.2 als Startpunkt; restliche Stages aus den jal-Sites §3.3 extrahieren). NICHT aus dem Grab-Biss-Pfad.
- **D4 — Persistenz:** Tabelle lebt im Game-State, überlebt Raumwechsel, wird bei Player-Load (New Game/Load) genullt; NICHT in den Save-Record (byte-true: Original speichert sie nicht).
- **D5 — Verifikation:** Headless-Room-Probe: 3× Hurt-Substate-0-Treffer → Atlas-Diff muss exakt die 1402 Bytes des Ground-Truth-Save reproduzieren (`blood_probe*.py`-Werkzeuge im Scratchpad; Referenz mzd_blood_decals_hp30.sav).
- **D6 (optional, dormant):** Debug-Entry Äquivalent `LAB_80037de4` hinter Debug-Flag.

---

## 7. Offene Punkte (ehrlich OFFEN, mit nächstem Schritt)

1. **Writer von Hurt-Substate 0/1** (die Schwer-Treffer-Pfade; FUN_80012d60 schreibt nur 2/3). Nächster Schritt: Schreib-Zensus auf `DAT_800aca59` in STAGE1.BIN (sb-Scan) + Savestate-Repro mit einem Zombie-Lunge-Knockdown.
2. **Panel↔Körperteil-Benennung** (p0..p7 anatomisch): aus den UV-Inseln von PL00.MD1 ableitbar (Mesh-UV-Bboxen liegen vor); kosmetisch, nicht mechanik-relevant.
3. **Gegner-Wund-System** (zweite DR_MOVE-Tabelle @0x800b267c, Builder @0x8002bb04ff, per-Typ-Tabellen @0x80073c6d/6e): eigenes RE-Ticket; gleicher Mechanismus vermutet.
4. **STAGE2-5-Helper-Argumente** vollständig tabellieren (jal-Sites liegen vor, §3.3) — nötig für D3 über STAGE1 hinaus.
5. `FUN_80020b04`-DR_MOVE-User (Boot) identifizieren (nicht Spieler-Wunden; vermutlich VRAM-Arrangement beim Screen-Setup).

---

## 8. Werkzeug-Referenz

Probes (Scratchpad, reproduzierbar): `blood_probe1.py` (Statusblock+Buffer), `blood_probe2.py` (Save↔TIM-Diff; entlarvte den Waffen-Block), `blood_probe3.py` (Stempel-Struktur/CLUT), `blood_probe4.py` (Template-Suche → Bank), `blood_probe5.py` (exakte Rects/Translationen), Tabellen-Dump @0x800b10ec (Panels/Level/Akku). Savestate-Zugriff: `.claude/skills/re15-savestate-ghidra/scripts/re15_ss.py` (`Ram.vpix`).
