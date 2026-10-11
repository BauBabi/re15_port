#!/bin/bash
# mk_wt.sh <spur> — schlanker Arbeitsbaum r36_<spur> auf Zweig r36/<spur> von master
set -e
cd /c/workspace/git/reAi_v2
s="$1"; wt=".claude/worktrees/r36_$s"
[ -d "$wt" ] && { echo "exists $s"; exit 0; }
git worktree add --no-checkout -b "r36/$s" "$wt" master >/dev/null 2>&1
cd "$wt"
git sparse-checkout init --cone >/dev/null 2>&1
git sparse-checkout set re15_port analysis RE_15_Quellcode_Overlays RE2_Quellcode_V2 RE_15_Quellcode_V2 extracted_re2_dokumente extracted_re2_items psx_dev synchro .claude extracted_re2_sicherung pri src shots tools stage_saves extracted_re2_feuerloescher release masken_vorschau RE2_Quellcode_Overlays memory .kiro scripts .idea gradle info/Re1.5 info/re2leon voice >/dev/null 2>&1
git checkout -q 2>/dev/null || true
echo "$s status=$(git status --porcelain | wc -l) head=$(git rev-parse --short HEAD)"
