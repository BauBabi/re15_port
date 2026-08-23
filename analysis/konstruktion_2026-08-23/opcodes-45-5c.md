# SCD-Opcodes 0x45 / 0x5C

Alle Belege sind beisammen. Hier das Dossier-Rohmaterial.

---

# SCD-Opcodes 0x45 & 0x5C — RE-Dossier (EXE-Lücken 3+4)

## 1) MECHANISMUS

### Dispatch-Grundlage (beide Opcodes)

SCD-VM-Dispatch-Loop `@0x8003f0ec–0x8003f110` (in FUN_8003f0a0-Bereich, `s4` = Handler-Tabelle `PTR_LAB_800744a8` = `0x800744a8`):

```
8003f0ec: lw   v0,0x1c(s0)    ; pc  (VM-Kontext +0x1c)
8003f0f4: lbu  v0,0(v0)       ; Opcode = *pc — pc zeigt AUF das Opcode-Byte
8003f0fc: sll  v0,v0,2
8003f100: addu v0,v0,s4       ; Tabelle 0x800744a8
8003f104: lw   v0,0(v0)
8003f10c: jalr v0             ; handler(a0 = vm_ctx)
```

Tabellen-Basis verifiziert über drei unabhängige Einträge: 0x45-Handler-XREF `@0x800745bc` (0x800745bc−0x45·4 = 0x800744a8), 0x5C-Handler-XREF `@0x80074618` (−0x5C·4 = 0x800744a8), Gegenprobe 0x51/0x52 (Pad-Prädikate `LAB_80042920`/`LAB_8004295c`, XREFs 0x800745ec/0x800745f0 — deckt sich mit Memory `reai-v2-scd-pad-predicates`).

### Opcode 0x45 — Cut-Mask-Gruppen-Set (Hintergrund-Masken an/aus)

**Handler `LAB_800428d4`** (Tabelleneintrag `@0x800745bc`):
```
800428e4: lw   v0,0x1c(s0)     ; pc
800428ec: lbu  a0,0x1(v0)      ; Operand 1 = Gruppen-Index (0-basiert)
800428f0: lbu  a1,0x2(v0)      ; Operand 2 = Wert
800428f4: jal  FUN_800396a8
800428f8: _addiu a0,a0,0x1     ; a0 = Gruppe+1  (Tabelle speichert 1-basiert)
80042900: ori  v0,zero,0x1     ; return 1 (weiterlaufen)
80042904: addiu v1,v1,0x3      ; pc += 3
```
**Layout: `45 GG VV` — 3 Bytes.** GG = 0-basierter Masken-Gruppen-Index, VV = Zustandsbyte.

**Helfer `FUN_800396a8`** (einziger Caller: `@0x800428f4`):
```
800396b8: lbu a3,0(v0)         ; Anzahl = Byte 0 von *[DAT_800ac778] (deklarierter Mask-Count des aktuellen Cuts)
800396c0: lw  v1,[DAT_800b2584]; Zustands-Array, 4 B/Eintrag
800396d0: lbu v0,1(v1)         ; entry[1] = Gruppen-ID (1-basiert)
800396d8: bne v0,a0,skip
800396e0: sb  a1,0(v1)         ; TREFFER: entry[0] = VV
800396ec: addiu v1,v1,4        ; Stride 4
```

**Was ist die `[DAT_800b2584]`-Tabelle? Das per-Cut-Zustands-Array der sprite.pri-Hintergrund-MASKEN.** Vollständige Konsumenten-Menge (Ghidra-XREF: genau 4 Funktionen; der 5. Treffer `@0x8005c200` ist ein indizierter `sw` in das Nachbar-Array 0x800b2544 + s2·4, kein echter Konsument):

1. **`FUN_80039270`** — Arena-Carve pro Raum: `DAT_800b2584 = DAT_800ac77c` (Bump-Arena), Größe = RDT-Header-Byte[7]·4 (Flags) + 2 Prim-Puffer `DAT_800bb4d4`/`DAT_800bb4d8` à Header[7]·0x20 (`@0x8003928c lbu v0,7(a0)`, Arena-Advance `@0x800392bc`). Deckt sich mit dem bereits dokumentierten Modell in `re15_pri.h:38-49`.
2. **`FUN_800392d4`** — Builder bei Kamera-Cut-Wechsel (einziger Caller `FUN_80021bbc @0x80021c28`, dem Cut-Wechsel-Handler, Gate `DAT_800aca38 & 0x100000`): liest pri-Daten via `[DAT_800ac778]+0x24` (Cut-Record-Array, 0x20 B/Cut) + `+0x1c` (pri-Pointer = `re15_camera_cut_t.pri_offset`). Schreibt:
   - `@0x80039334/38`: erster u32 == 0xFFFFFFFF (NULL-Sektion) → `sb zero,0(a0)` = Count 0;
   - `@0x80039358`: `sb t2,0(a0)` — **deklarierter Mask-Count** (u32>>16) in Header-Byte 0;
   - `@0x8003936c–0x80039384`: Zero-Loop über **Kapazität Header[7]** — löscht **NUR Byte 0** jedes 4-B-Eintrags (`sb zero,0(s5); s5+=4` — entry[1..3] bleiben stale!);
   - pro gebauter Maske `@0x800393e0/e4`: `ori v0,v0,1; sb v0,0(s5)` — entry[0] |= 1 (sichtbar); `@0x800393e8/ec`: `sb (gruppe+1),1(s5)` — **entry[1] = Gruppen-Index+1**; `@0x800393f0/f8`: `sh depth,2(s5)` — entry[2..3] = Depth-Halfword.
3. **`FUN_80039590`** — per-Frame-Zeichner (Caller `@0x8001ce54`): Loop über deklarierten Count, Gate `@0x800395e8/f0/f4`: `lbu v0,0(s3); andi v0,v0,0x1; beq → skip` — **gezeichnet wird nur bei entry[0]-Bit 0 = 1**; AddPrim in OT-Bucket = Depth·1 (`@0x80039658 sll a0,a0,2`, bereits dokumentiert in `re15_pri.h:75-81`).
4. **`FUN_800396a8`** — der Opcode-0x45-Helfer (oben).

**Semantik komplett:** `45 GG VV` setzt bei ALLEN Masken der pri-Gruppe GG (0-basiert im Script, 1-basiert in der Tabelle) das Zustandsbyte auf VV; Renderer testet Bit 0 → `VV=0` blendet die Maskengruppe aus, `VV=1` ein. Wirkung hält bis zum nächsten Kamera-Cut-Wechsel (FUN_800392d4 re-initialisiert alle Flags auf `|1`). Ausgelieferte Scripts nutzen ausschließlich VV ∈ {0x00, 0x01} (Zensus unten). Das Skill-Label „0x45 = Col_chg_set / Set collision region" (`.claude/skills/scd-disassembly/SKILL.md:143`) ist damit **widerlegt** — Kollision wird nirgends berührt.

**Byte-true-Randfall:** Der Zero-Loop löscht nur entry[0]; entry[1] (Gruppen-ID) aus früheren Cuts bleibt in Slots jenseits des aktuellen Builds stehen. Da Opcode-Loop und Zeichner über den **deklarierten** Count laufen (nicht den gebauten — die zwei differieren real, ROOM1210/1211, siehe `pri_common.c:76-82`), kann 0x45 in solchen Räumen theoretisch Stale-Slots matchen. Ein Port muss dieselben Loop-Grenzen und dieselbe Nur-Byte-0-Löschung nachbilden.

### Opcode 0x5C — degenerierter BGM-Tabellen-Schreiber (NICHT „shake")

**Handler `LAB_80042a10`** (Tabelleneintrag `@0x80074618`):
```
80042a20: lw   v0,0x1c(s0)     ; pc (zeigt auf das Opcode-Byte, s.o.)
80042a28: lhu  a0,0x0(v0)      ; !!! Halfword bei pc+0 = (Operand1<<8) | 0x5C
80042a2c: jal  FUN_80044fec
80042a3c: addiu v1,v1,0x4      ; pc += 4  (Bytes 2,3 werden NIE gelesen)
```
**Layout: `5C B1 B2 B3` — 4 Bytes; nur B1 fließt ein, als High-Byte des Arguments; das Low-Byte des Arguments ist das Opcode-Byte 0x5C selbst.**

**Helfer `FUN_80044fec`** (einziger Caller: `@0x80042a2c`; einziger Schreiber der Tabelle):
```
80044fec: srl  v0,a0,24            ; Stage = param>>24
80044ffc: lbu  v1,DAT_800748fc[v0] ; per-Stage-Basisindex
80045008: sll  v1,v1,1             ; ×2 (u16-Einträge)
8004500c: addu v1,v1,0x80074828
80045010: srl  v0,a0,15
80045014: andi v0,v0,0x1fe         ; ((param>>16)&0xff)·2 = Raum·2
80045018: addu v1,v1,v0
80045020: sh   a0,0(v1)            ; Eintrag = low16(param)
```
**Die `UNK_80074828`-Tabelle = die per-Raum-BGM-Zuweisung** (106 u16-Einträge, 0x80074828..0x800748fb; Stage-Basen `DAT_800748fc` = `{0x00,0x26,0x32,0x41,0x4d,0x62}`, aus der EXE gedumpt — identisch mit `audio_pc.c:2066 SS_BGMTBL`/`SS_STAGE_OFF`). Leser: `FUN_80044210` und `FUN_800443ec` (Raumwechsel-BGM: `entry = tbl[DAT_800748fc[DAT_800b0fe0] + _DAT_800b0fe2]`; low byte&0x3f = MAIN-Slot, >>6 = Typ; high byte&0x3f = SUB-Slot, Bits 6/7 Flags — deckt sich mit Memory `reai-v2-bgm-container`).

**Effektive Laufzeit-Wirkung von 0x5C wie ausgeliefert:** Da `lhu` nur 16 Bit liefert, ist param>>24 = 0 → `DAT_800748fc[0]` = 0x00, und `(param>>15)&0x1fe` = 0 (param ≤ 0xFFFF ⇒ Bit-15-Shift wird von der geraden Maske geschluckt). **Der Schreibzugriff trifft also IMMER Eintrag 0** (= Stage 1, Raum 0 = ROOM1000) **mit dem Wert `(B1<<8)|0x5C`** (Low-Byte fest 0x5C ⇒ MAIN-Slot 0x1C, Typ 1). Zusätzliche Hardware-Falle: bei ungeradem Script-Offset wäre das `lhu` unaligned → AdEL-Exception.

**RE2-Vergleich belegt die verstümmelte Absicht:** RE2-Leons Pendant `LAB_80057e98` (Tabelleneintrag `@0x800a7624`) ruft dieselbe Setter-Mathematik `FUN_8005b9f4` (identisch bis auf Tabellen-Adressen `DAT_800a80af`/RE2-Tabelle) mit korrekt gebautem Argument:
```
80057eb0: lhu a0,0x2(v0)   ; Stage/Raum-Halfword bei pc+2
80057eb4: lhu v1,0x4(v0)
80057eb8: lhu v0,0x6(v0)
80057ebc: sll a0,a0,0x10   ; param = (hw@2<<16)|hw@4|hw@6
...
80057ed4: addiu v1,v1,0x8  ; pc += 8
```
RE1.5 hat diesen 8-Byte-`Sce_bgmtbl_set` auf einen 4-Byte-Rumpf reduziert, dessen Operanden-Fetch (`lhu 0(pc)` statt `lhu 2(pc)`…) das Opcode-Byte mit einliest. Ob Absicht oder Bug ist aus dem Binary nicht entscheidbar — **laufzeit-irrelevant, weil kein Script ihn aufruft** (Zensus). Das Port-Label „Sce_shake_on" ist eine reine RE2-Namens-Import-Fiktion; mit Screen-Shake hat der Handler nichts zu tun.

## 2) GAME-WEITER ZENSUS

Walker: main-SCD (RDT u32@0x40) + Sub-SCD-Tabelle (RDT u32@0x44) aller **240 RDTs** unter `info/Re1.5/PSX/STAGE1..6` + der SCD-Block des einzigen `DOOR/DOOR00.DO2` (Layout per `DO2Extractor.java:196-214`), linear dekodiert mit der disasm-verifizierten Längentabelle aus `scd_vm.c:166-218`. Skripte: `<scratchpad>/scd_census_45_5c.py`, `scd_census_do2.py`.

**Opcode 0x5C: 0 Fundstellen. Game-weit unbenutzt → DORMANT.** (Konsequenz: die BGM-Tabelle hat außer 0x5C keinen Schreiber ⇒ sie ist zur Laufzeit effektiv konstant ⇒ das `static const SS_BGMTBL` des Ports ist durch den Zensus als byte-true gerechtfertigt.)

**Opcode 0x45: 120 Fundstellen in 11 RDTs (9 Räume + Spieler-Varianten), alle Werte ∈ {00, 01}:**

| RDT | Sektion | Fundstellen (Datei-Offset: Bytes) |
|---|---|---|
| STAGE1/ROOM1150 (Irons-Büro) | sub[05] | +0x10b6..+0x10c5: `45 05..0a 00` (Gruppen 5–10 AUS) → +0x10c8 `09 0a 14 00` (Sleep 20) → +0x10cc..+0x10db: `45 05..0a 01` (wieder AN) |
| STAGE1/ROOM1151 | sub[05] | identisches Muster @ +0x1094..+0x10b9 |
| STAGE1/ROOM1211 | sub[01] | +0x1f64/+0x1f67: `45 00 00`, `45 01 00` |
| STAGE3/ROOM3000 | sub[01] | +0x15d6..+0x1624: Gruppen 0–2, 7–11 (2×), 3–7 AUS (18 Stück) |
| STAGE3/ROOM3001 | sub[01] | identisch @ +0x160e..+0x165c |
| STAGE3/ROOM3010 | sub[01] | +0x2274..+0x22bf: Gruppen {0,1},{0,1},{5,6},{0},{0,1},{0},{2,3} AUS |
| STAGE3/ROOM3011 | sub[01] | identisch @ +0x22a8..+0x22f3 |
| STAGE3/ROOM3071 | sub[01] | +0x3542..+0x3560: Gruppen 2..12 AUS (nach `54`-BGM-Control, `21`-Ck, `13`-Switch, `14`-Case) |
| STAGE3/ROOM3071 | sub[02] | +0x35c2..+0x3626: Lauflicht-Sequenz Gruppe 12→2: je `45 GG 01` → `09 0a 14 00` (Sleep 20) → `45 GG 00` (21 Stück; davor `43 00 80 00` Plc_flg, `09 0a 50 00` Sleep 80, `41 …` Plc_neck, `22 05 03 01` Set) |
| STAGE5/ROOM5060 | sub[01] | +0x2eca: `45 00 00` |
| STAGE5/ROOM5061 | sub[01] | +0x2eca: `45 00 00` |

Plausibilitäts-Gegenprobe gegen die echten pri-Daten (Gruppen-Counts pro Cut aus den RDTs geparst): ROOM1150 max 11 Gruppen (Script nutzt 5..10 ✔), ROOM3071 max 20 (nutzt 2..12 ✔), ROOM3000 max 13 (nutzt 0..11 ✔), ROOM3010 max 15 ✔, ROOM5060 max 17 ✔, ROOM1211 max 34 ✔ — Operand ist bestätigt der 0-basierte Gruppen-Index. Wichtig: `sub[01]` ist das per-Frame reseedete sub01 (Memory `reai-v2-scd-per-frame-model`) → die STAGE3/5-Fälle sind Gameplay-zustandsabhängige Maskenschaltungen; die `sub[05]`/`sub[02]`-Fälle sind Event-Scripts (ROOM1150 = Irons-Cutscene: 20-Frame-Masken-Blackout; ROOM3071 sub02 = animierte Einzel-Gruppen-Pulse).

## 3) PORT-STAND

- `re15_port/engine/src/scd_vm.c:187` — Länge `[0x45] = 3` ✔ korrekt; **kein** `s_op_table[0x45]`-Eintrag → fällt auf `op_unknown` (scd_vm.c:3616), 3-Byte-Skip, **No-op** ⇒ die 120 realen Aufrufe verpuffen (Masken bleiben immer sichtbar).
- `scd_vm.c:349` + `scd_vm.c:4112-4113` — `s_op_table[0x5C] = op_sce_shake_on`, No-op `pc += 4` ✔ Advance korrekt, Name irreführend (RE2-Import).
- Masken-System existiert byte-true: `engine/src/pri_common.c` + `include/re15_pri.h` (Parser FUN_800392d4-treu inkl. deklariert-vs-gebaut, Kapazität 105) — aber `re15_pri_mask_t` (re15_pri.h:52-57) hält **weder Gruppen-ID noch Flag-Byte**; beides wird beim Parsen weggeworfen.
- Konsum: `platform/pc/main.c:4080-4125` parst pro Cut-Wechsel und pusht einen **einmaligen Snapshot** via `re15_render_pc_set_pri_rects` (render_pc.c:1747); Zeichnen render_pc.c:736-830 — alle Masken bedingungslos, kein per-Frame-Flag-Test.
- BGM-Tabelle: `platform/pc/src/audio_pc.c:2058-2104` — `SS_BGMTBL[106]`/`SS_STAGE_OFF[6]` als `static const`, Werte identisch mit meinem EXE-Dump @0x80074828/@0x800748fc ✔.

## 4) IMPLEMENTIERUNGS-PLAN

**Opcode 0x45 — implementieren (120 reale Call-Sites, davon ROOM1150/1151 in der aktiven STAGE1-Raumketten-Kampagne):**
1. `re15_pri.h`/`pri_common.c`: `re15_pri_mask_t` um `uint8_t group` (1-basiert, Build-Reihenfolge, `@0x800393e8`) erweitern; Laufzeit-Zustand als Engine-Global (Analogon `[DAT_800b2584]` + Count `[*DAT_800ac778]`): persistentes Array `{flags, group, depth}` über Kapazität; Rebuild bei Cut-Wechsel = NUR flags über Kapazität nullen (`@0x8003936c-84`), dann pro gebauter Maske `flags|=1`/group/depth (`@0x800393e0-f8`); Loop-Grenze für Set+Draw = deklarierter Count (`@0x80039358`, = `draw_count`).
2. Neue Engine-API `re15_pri_mask_group_set(uint8_t group_1based, uint8_t value)` — exakte FUN_800396a8-Schleife.
3. `scd_vm.c`: Handler `op_pri_mask_set` registrieren: `re15_pri_mask_group_set(pc[1]+1, pc[2]); pc += 3; return 1;` (Zitate `@0x800428ec/f0/f8/904`). Skill-Tabelle `.claude/skills/scd-disassembly` (0x45-Zeile) korrigieren.
4. Renderer: PC (`render_pc.c` Maskenschleife 736-830) und PSX (`platform/psx`) filtern pro Frame auf `flags & 1` (`@0x800395f0`) — entweder Flags-Pointer mitgeben oder bei Set dirty-repush; `main.c:4084ff` befüllt beim Cut-Wechsel zusätzlich den Zustand.
5. Verifikation: (a) room-probe-ctest: ROOM1150 laden, `sub_scd[5]` starten (RE15_FORCE_EVENT-Pfad existiert), asserten: Gruppen 5–10 flags→0, nach Sleep(20) →1; ROOM3071 sub02 Lauflicht-Sequenz asserten. (b) Live: DuckStation-Savestate in der Irons-Szene, `[DAT_800b2584]`-Bytes lesen (Pointer-Global @0x800b2584) und gegen Port-Zustand diffen. (c) Visuell nur per gdigrab-Skill.

**Opcode 0x5C — mit Beleg als DORMANT dokumentieren, Stub umbenennen:**
1. `op_sce_shake_on` → z.B. `op_bgmtbl_set_degenerate` umbenennen; Kommentar: Handler `LAB_80042a10` (`lhu 0(pc)` `@0x80042a28`, len 4 `@0x80042a3c`), Helfer FUN_80044fec schreibt `(pc[1]<<8)|0x5C` immer in BGM-Tabellen-Eintrag 0; RE2-Vergleich `LAB_80057e98`; **Zensus 2026-08-23: 0/240 RDTs + 0/1 DO2 → nie ausgeführt**. No-op + `pc += 4` ist damit in jedem ausgelieferten Script beobachtungs-äquivalent; `SS_BGMTBL` darf `const` bleiben (kein anderer Schreiber existiert — Ghidra-XREF FUN_80044fec: einziger Caller 0x80042a2c). Keine Funktions-Implementierung nötig; sollte je ein Custom-Script ihn nutzen, wäre die literale Schreibsemantik oben vollständig spezifiziert.

## OFFEN (ehrlich)

1. **0x45 live nicht gemessen:** Die Reset-bei-Cut-Wechsel-Semantik und der Stale-entry[1]-Randfall sind rein statisch belegt (Disasm oben); DuckStation-Live-Dump von `[DAT_800b2584]` während ROOM1150-sub05 steht aus. Nächster Schritt: re15-room-capture ROOM1150, Savestate während der Irons-Szene, 4-B-Einträge lesen.
2. **Visuelle Bedeutung pro Fundstelle** (was die Gruppen 5–10 in ROOM1150 konkret zeigen — vermutlich Cutscene-Blackout von Vordergrund-Occludern) ist aus Daten gefolgert, nicht Frame-genau am Original gesichtet.
3. **0x5C Absicht (Bug vs. Repurposing):** dass der Fetch `lhu 0(pc)` eine Verstümmelung des RE2-8-Byte-Opcodes ist, ist durch den RE2-Vergleich stark belegt, aber nicht beweisbar; laufzeit-irrelevant (0 Call-Sites).
4. **Zensus-Restrisiko:** linearer Walk kann in Daten-Regionen desyncen (bekannte Falle); alle 120 Treffer sind durch saubere Muster + Gruppen-Count-Gegenprobe verifiziert, ein theoretischer False-Negative in einer desyncten Region ist nicht ausschließbar. EXE-interne/DEBUG.BIN-SCDs (falls existent) waren nicht im Scope; gescannt: 240 RDTs + DOOR00.DO2.
5. **Nebenbefund:** Skill-Label 0x57 „Sce_bgmtbl_set" ist unverifiziert (Handler `LAB_80042ab4 @0x80074604` ruft NICHT FUN_80044fec; sein Callee wurde hier nicht analysiert) — separater RE-Punkt.

Dateien: Zensus-Skripte in `C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\e15d7177-43f5-413e-8f99-d5e5f369c973\scratchpad\scd_census_45_5c.py` und `scd_census_do2.py` (Repo unverändert).
