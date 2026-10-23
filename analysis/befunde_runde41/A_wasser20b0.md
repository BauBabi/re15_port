# Runde 41 — Spur A: wasser20b0 (Dossier, fortlaufend)

Baum: `.claude/worktrees/r41_wasser20b0`, Zweig `r41/wasser20b0`, Basis master 116e8b0f.
Punkte (AUFTRAG.md):
1. 20B0-Wasserschwall im Original messen + EM024 (Typ 0x24) im Port reparieren.
2. STAGE2: Wassereffekt beim Sprung von der Kante ins Wasser; NPCs teilen die Wasser-Effekte.

## Stand / Protokoll (neueste Einträge unten)

### 2026-10-23 Start
- Baum sauber (`git status --porcelain` leer), HEAD 116e8b0f.
- Pflichtlektüre: AUFTRAG.md, VERTRAG.md, UEBERGABE.md (R40, Befund STAGE2) gelesen.
- Port-Stand: `enemy_ai_common.c` ab Z. 13563 "AMBIENT FX-EMITTER (type 0x24, EM024)":
  grid-0 (0x8010f130) / grid-1 (0x8010f3fc) Partikel-Sequencer -> 0x80019700 stehen als OFFEN (nicht gebaut).
