#!/bin/bash
# Spur L Nachbesserung 1 — lauf.sh <name> <raum> "<karte args>" <exit_at> [extra env...]
B="C:/workspace/git/reAi_v2/.claude/worktrees/r35_cut1150/re15_port/build"
S="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/spurL_n1"
name=$1; raum=$2; karte=$3; ex=$4; shift 4
W="$S/runs/$name"; rm -rf "$W"; mkdir -p "$W"; cd "$W" || exit 1
"$B/tests/unit/probe_r35_cut1150_karte.exe" re15_card.mcr $raum $karte > karte.txt 2>&1 || { echo KARTE FAIL; cat karte.txt; exit 1; }
EXE="$B/platform/pc/${EXE_NAME:-re15_pc_r35l_n1.exe}"
NA="RE15_NOAUDIO=1"; [ "$NOAUDIO" = "0" ] && NA="RE15_NO_AUDIO_DUMMY=0"
env RE15_NO_INTRO=1 "$NA" RE15_WINDOW_SCALE=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 \
  RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60 "RE15_INPUT_SCRIPT=${SKRIPT:-W1,A0.2,W200}" \
  RE15_STATE_LOG=state.log RE15_IT_LOG=${ITLOG:-30} "RE15_EXIT_AT=$ex" "$@" "$EXE" > stdout.txt 2> stderr.txt
echo "exit=$?"
