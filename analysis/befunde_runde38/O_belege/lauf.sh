#!/usr/bin/env bash
# lauf.sh <name> <exe-suffix> <exit_at> <framedump-spec-or-empty> [karte-args] [extra env...]
# Messlauf der echten exe: Karte (probe_r35_cut1150_karte) + CONTINUE in ROOM1130 + Tuer -> Montage.
set -u
BAUM="C:/workspace/git/reAi_v2/.claude/worktrees/r38_montage/re15_port/build"
SP="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/o/runs"
name="$1"; suf="$2"; exitat="$3"; fd="$4"; karte="${5:-nach10f0 ersteszene}"; shift 5 || shift $#
WD="$SP/$name"; rm -rf "$WD"; mkdir -p "$WD"
EXE="$BAUM/platform/pc/re15_pc_r38o_$suf.exe"
cp -f "$BAUM/platform/pc/re15_pc.exe" "$EXE"
( cd "$WD" && "$BAUM/tests/unit/probe_r35_cut1150_karte.exe" re15_card.mcr 1130 $karte > karte.txt 2>&1 )
cd "$WD"
env RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 \
    RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 \
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60 RE15_INPUT_SCRIPT="W1,A0.2,W200" \
    RE15_STATE_LOG=state.log RE15_IT_LOG=5 RE15_EXIT_AT="$exitat" \
    ${fd:+RE15_FRAMEDUMP="$fd"} "$@" \
    "$EXE" > stdout.txt 2> stderr.txt
echo "exit $? ($name)"
rm -f "$EXE"
