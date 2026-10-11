#!/bin/bash
# seeds52v.sh <ausgabe> <anker> <vortritt 0|1|2> [lage]: 52 Seeds x 6000 Bilder AN, je Lauf eigener Prozess, 6 parallel
T="C:/workspace/git/reAi_v2/.claude/worktrees/r36_zombieki/re15_port/build/tests/unit/test_r36_zombieki_affen_block.exe"
out="$1"; anker="$2"; vt="$3"; lg="${4:-0}"; : > "$out"
seq 0 51 | xargs -P 6 -I{} sh -c "\"$T\" eins {} 1 6000 $anker $lg $vt 2>/dev/null | grep -E '^(EINS|LAGE)' >> \"$out\""
