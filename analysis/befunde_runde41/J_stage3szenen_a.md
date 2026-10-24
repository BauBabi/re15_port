# Runde 41 — Spur J "stage3szenen_a" (Dossier, fortlaufend)

Baum `.claude/worktrees/r41_stage3szenen_a`, Zweig `r41/stage3szenen_a`, Basis 4c667c97 (+ ae8ea0a7 AUFTRAG A-E3..A-E5).
Start 2026-10-24 (Uhr siehe Commit-Zeit). Kein Vorgaenger-Stand dieser Spur im Baum (0 eigene Commits beim Start).

## Punkte (Wortlaut, AUFTRAG.md)
1. "Auch die Animation von Marvin, am Ende seine Abschnitten in STAGE3, bevor zurueck gewechselt wird zu Leon ist falsch in ROOM 3060."
2. "In Elzas Szenario, in ROOM 3061 in der cutscene klappt das rein krabbeln von Sherry in den Schacht nicht. Am Ende von
   dieser Cutscene hat Elza auch erneut eine falsche Animation."
0. (Orchestrator-Entscheid A-E5, 2026-10-24 04:01) ERSTER Schritt: RBJ-Binder FUN_8001b3f8 byte-true bauen, in allen 12
   abweichenden Raeumen zur Laufzeit messen (welcher Record je Slot, vorher/nachher), eigener Commit mit `rbj-binder` in
   der ersten Zeile. Spur K uebernimmt ihn per cherry-pick unveraendert.

## Plan
- §0 RBJ-Binder: Disasm FUN_8001b3f8 -> Port rbj_resolve_slot / re15_emd_parse_rbj -> Laufzeitmessung 12 Raeume -> Commit `rbj-binder`.
- §1 3060 Marvin: SCD-Disasm ROOM3060 (Marvin-Abschnitt-Ende), Port-Trace (RE15_ANIM_TRACE/RE15_SCD_TRACE) + Framedump, Vergleich.
- §2 3061 Sherry-Schacht + Elza-Ende: SCD-Disasm ROOM3061, Port-Trace mit Elza, Vergleich.

## Laufendes Protokoll
(neueste Eintraege unten je Abschnitt)
