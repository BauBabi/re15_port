# Runde 41 — Spur A "wasser20b0": ABNAHME 2 (unabhaengig, 2026-10-24)

Baum `.claude/worktrees/r41_wasser20b0`, Zweig `r41/wasser20b0`, HEAD 9171a10e (Code-Endstand der Nachbesserung 2), Merge-Basis
master 116e8b0f (master-HEAD 71b52f4c traegt seitdem nur Doku unter analysis/befunde_runde41/ - `git diff --name-only 116e8b0f master`).
Massstab: AUFTRAG.md (Wortlaut Punkt 1/2, Nachtraege 2/3, Entscheide E1/E2, A-E3/A-E4/A-E5), VERTRAG.md §1/§2, Maengel M1..M5 aus
`A_abnahme_1.md`. Der Bau-Agent wird nicht geglaubt; nur selbst Gemessenes zaehlt. Bericht FORTLAUFEND (Stand je Abschnitt).
Scratch (nicht versioniert): `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/
scratchpad/A41abn2/` (unten kurz `A41abn2/...`; Logs, Disasm-Auszuege, Laufordner `mess/<lauf>/`). Messfahrten mit `A41abn2/lauf.sh`
(exe-Kopie `re15_pc_a41abn2_<lauf>.exe` neben der exe, `env -i`, timeout, Kopie danach geloescht); ctest mit `A41abn2/ct.sh`
(PATH msys64 zuerst, git am Ende).

## 0. Baustand (selbst gebaut)
- `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work to do.`,
  `=== LOCAL-BUILD-OK (build)` (07:13). exe `re15_port/build/platform/pc/re15_pc.exe` mtime 2026-10-24 07:10:00 ist juenger als jede
  Quelle des Diffs (juengste: re15_wassersprung_r41.h 06:39:28, test_r41_wasser20b0.c 06:10:34, wassersprung_r41.c 06:08:04,
  audio_pc.c 05:56:49); `git status` sauber -> die exe ist der Code-Stand 9171a10e.
- Delta seit Abnahme 1 (`git diff d8c7334b HEAD --stat -- re15_port`): wassersprung_r41.c (+192), re15_wassersprung_r41.h (+49),
  audio_pc.c (+5/-1: 1 Include, 2 Zeilen, 1 geaenderte Zeile), test_r35_granate.cmake (+10/-1), test_r41_wasser20b0_npc.cmake (+155),
  probes/r41_wasser20b0.cmake (+14), test_r41_wasser20b0.c (+303). Gesamt-Diff gegen die Basis (`git diff master...HEAD --stat --
  re15_port`): 23 Dateien, +2722/-41 (Aufstellung Abschnitt 6).

(Abschnitte folgen; Stand wird fortlaufend ergaenzt.)
