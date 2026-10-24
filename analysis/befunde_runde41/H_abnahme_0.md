# Runde 41 — Spur H "kollision": Abnahme 0 (unabhaengig, 2026-10-24)

Baum `.claude/worktrees/r41_kollision`, Zweig `r41/kollision`, gepruefter Stand **103563cc** (merge-base master =
7b59e297, d.h. master ist enthalten). Arbeitsordner der Abnahme (Logs, Bilder, Skripte):
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/H41abn0/`.

Status: IN ARBEIT (fortlaufend geschrieben).

## 0. Bau
- `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`
- `bash re15_port/tools/local_build.sh build` -> `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`
- Gegenprobe, dass die exe den Diff traegt (kein altes Artefakt): `objdump -r` der Objekte —
  `enemy_ai_common.c.obj: IMAGE_REL_AMD64_REL32 re15_kollision_r41_npc_schwanz`,
  `aot_common.c.obj: ... re15_kollision_r41_kisten_pass`, `game_step_common.c.obj: ... re15_kollision_r41_objspiegel`;
  Quellen 11:27:59, `re15_pc.exe` 11:28:49, Arbeitsbaum sauber (`git status` leer).

## 1. Diff gelesen (git diff master -- re15_port)
- NEU `engine/src/kollision_r41.c` (221 Z.), `include/re15_kollision_r41.h` (49 Z.).
- Haken: `enemy_ai_common.c` (+1 Include, +1 Zeile in re15_npc_ai_tick vor re15_npc_wall_clamp),
  `aot_common.c` (+1 Include, Objekt-Pass in re15_prop_push_tick, Kopfkommentar), `game_step_common.c` (+1 Include,
  +1 Zeile re15_kollision_r41_objspiegel nach re15_objfall_nach_pass).
- Tests: `tests/unit/probes/r41_kollision.cmake`, `tests/unit/test_r41_kollision.c`, `tests/unit/probe_r41_kollision_abstand.c`,
  `tests/integration/test_r41_kollision.cmake`; FREMDER Riegel geaendert: `tests/integration/test_r39_dining1141_kampf.cmake`
  (WARTE 60 -> 240).
