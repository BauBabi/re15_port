# Runde 41 — Spur K "stage3szenen_b" (Dossier, fortlaufend)

Baum `.claude/worktrees/r41_stage3szenen_b`, Zweig `r41/stage3szenen_b`, Basis 700483de (master 4c667c97 + AUFTRAG/VERTRAG
Stand Nachtrag 11). Wortlaut des Nutzers (AUFTRAG.md, 2026-10-23 16:3x):

> K1: "Ab 30C1 wird es richtig wild... Sherrys Charakter Modell ist völlig falsch und kaputt. Außerdem sollte sie keine
>     Waffe haben."
> K2: "Die Cutscene im ROOM 3091 hat STÄNDIG falsche Animationen...."

Orchestrator A-E5: RBJ-Binder (FUN_8001b3f8) gehoert Spur J; K uebernimmt J's Commit `rbj-binder` UNVERAENDERT per
cherry-pick und misst 3091 darauf. Bits: J/K = 121-124 bei Bedarf. Nachrichten: nur vorhandene.

## Plan
1. K1 Messung vorher: welcher Typ/welches Modell/welche Bank fuer Sherry in 30C1..3091 (Port), Framedump.
2. K1 Original: Sce_em_set-Zensus, EMD/CDEMD1-TOC, Textur/CLUT, Waffen-Mesh-Frage; Savestate/Video.
3. K1 Umsetzung + Riegel (Modell-Hash/Mesh-Zahl, kein Waffen-Mesh).
4. K2 Messung vorher (RE15_ANIM_TRACE je Akteur je Zeile), Original-SCD-Subs, Clip-Folge je Akteur.
5. K2 cherry-pick rbj-binder (J), Messung darauf; Ursachen belegen, beheben, Riegel.
6. Gezielte Tests, Abschluss.

## Protokoll (neueste unten)
- 2026-10-24: Dossier angelegt.
