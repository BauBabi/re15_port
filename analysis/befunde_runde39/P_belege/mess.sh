#!/bin/bash
# Messlauf Spur P Nachbesserung 1: mess.sh <name> "<karte args>" <wegplan-datei> <exit_at> [KEY=VAL ...]
# Elza, Spielstand + CONTINUE, eigene exe-Kopie re15_pc_p39n_<name>.exe neben der exe.
set -u
BAUM="C:/workspace/git/reAi_v2/.claude/worktrees/r39_johnruf1041"
B="$BAUM/re15_port/build"
S="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/p39n"
name="$1"; karte="$2"; plan="$3"; ende="$4"; shift 4
L="$S/l_$name"; rm -rf "$L"; mkdir -p "$L"
cp "$plan" "$L/wegplan.txt"
"$B/tests/unit/probe_r36_elza_karte.exe" "$L/re15_card.mcr" $karte > "$L/karte.txt" 2>&1 || { echo "KARTE FAIL"; cat "$L/karte.txt"; exit 2; }
EXE="$B/platform/pc/re15_pc_p39n_$name.exe"
cp "$B/platform/pc/re15_pc.exe" "$EXE"
cd "$L"
# keine geerbten RE15_* Variablen
for v in $(env | grep -o '^RE15_[A-Za-z0-9_]*'); do unset "$v"; done
env RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 \
    RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 RE15_MSG_LOG=1 RE15_SE_DEBUG=1 RE15_BGM_CTL_DEBUG=1 \
    RE15_STATE_LOG=state.txt "RE15_WEGPLAN=$L/wegplan.txt" RE15_WEGPLAN_LOG=wegplan_log.txt \
    "RE15_EXIT_AT=$ende" "$@" timeout 900 "$EXE" > stdout.txt 2>&1
rv=$?
rm -f "$EXE"
echo "exit=$rv lauf=$L"
