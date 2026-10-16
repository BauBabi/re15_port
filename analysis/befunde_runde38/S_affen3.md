# Runde 38 — Spur S "affen3" (Stage 3: die kleinen Affen treffen nicht)

Baum: `.claude/worktrees/r38_affen3`, Zweig `r38/affen3`, Basis master cb782bda. Beginn 2026-10-16.

## Auftrag (woertlich, AUFTRAG.md)
> - In Stage 3 die kleinen Affen können einen nicht treffen, es passiert nichts

Zuteilung (VERTRAG.md §0): "S | affen3 / r38_affen3 | Stage 3 kleine Affen treffen den Spieler wie im Original".
Keine Bank-9-Bits, keine Nachrichten-Ids, keine Items zugeteilt (keine noetig).

## Protokoll (fortlaufend)

### 2026-10-16 Start
- Baum geprueft: `git status --porcelain` leer, HEAD cb782bda, Zweig r38/affen3.
- AUFTRAG.md + VERTRAG.md vollstaendig gelesen.

### Schritt 0 — Welcher Gegner ist "die kleinen Affen"? (Zensus Stage-3-RDTs)
`python re15_port/tools/scd_dump_room.py <RDT> | grep Sce_em_set` ueber alle 32 Stage-3-RDTs
(ROOM3000..30F1). Opcode 0x44 `44 <slot> <typ> <grid> ...`. Gegnertypen ausser Zombies (0x10/0x11),
NPCs (0x40/0x42/0x49/0x4b) und Birkin (0x30/0x36):
- **Typ 0x29: ROOM3040 @0x01310 / @0x01324 und ROOM3041 @0x01310 / @0x01324** (je 2 Stueck,
  `44 00 29 00 01 ...` / `44 01 29 00 01 ...`, grid 0x00).
- Typ 0x20 in ROOM3060/3061 (3 Stueck) — STAGE3 registriert fuer 0x20 KEINEN Handler (Registrierungs-
  Scanner, memory reai-v2-enemy-roster-complete: STAGE3 = 0x29, 0x30+0x36, 0x40). Wird unten geprueft.
- Modell EM029 (CDEMD0.EMS Blob 13, s_ems_order Index 13 = 0x29): Ansichtsblatt
  `python re15_port/tools/emd_ansichtsblatt.py --typ 0x29 --clip 0` -> ein **kleiner Affe** (Fell, Affengesicht
  mit Reisszaehnen, Vierbeiner-Haltung, 18 Knochen, 19 Meshes, 32 Clips, Hoehe 1622 Einheiten). Der Port nennt den
  Typ nach dem RE2-Vorbild "COCKROACH" (BioModels.h: RE2 EM029 = COCKROACH) — in RE1.5 ist EM029 der kleine Affe.
  Der Gorilla (0x27, EM027, ROOM11C0, Stage 1) ist der GROSSE Affe und wird 1,7-fach skaliert gezeichnet.
- Port-KI: `re15_cockroach_ai_tick` (engine/src/enemy_ai_common.c), Wurzel STAGE3.BIN 0x80110b00.
