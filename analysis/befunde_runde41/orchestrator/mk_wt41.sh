#!/bin/bash
# mk_wt41.sh <spur> [basis] — schlanker Arbeitsbaum .claude/worktrees/r41_<spur> auf Zweig r41/<spur> (von master)
# Muster analysis/befunde_runde36/orchestrator/mk_wt36.sh (sparse cone: Bau + Quellen + RE2-Daten, ohne voice/, psx_dev-Rest).
set -e
cd /c/workspace/git/reAi_v2
s="$1"; basis="${2:-master}"; wt=".claude/worktrees/r41_$s"
[ -d "$wt" ] && { echo "exists $s head=$(git -C "$wt" rev-parse --short HEAD)"; exit 0; }
git worktree add --no-checkout -b "r41/$s" "$wt" "$basis" >/dev/null 2>&1
cd "$wt"
git sparse-checkout init --cone >/dev/null 2>&1
git sparse-checkout set re15_port analysis RE_15_Quellcode_Overlays RE2_Quellcode_V2 RE_15_Quellcode_V2 RE2_Quellcode_Overlays \
    extracted_re2_dokumente extracted_re2_items extracted_re2_sicherung extracted_re2_feuerloescher psx_dev synchro pri \
    tools stage_saves release shots .claude .kiro scripts info/Re1.5 info/re2leon memory >/dev/null 2>&1
git checkout -q 2>/dev/null || true
echo "$s status=$(git status --porcelain | wc -l) head=$(git rev-parse --short HEAD) basis=$basis"
