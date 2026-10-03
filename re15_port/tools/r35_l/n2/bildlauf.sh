#!/bin/bash
# Spur L Nachbesserung 2 — bildlauf.sh <name> "<karte args>" <exit_at> <framedump-spec>
# Karte (probe_r35_cut1150_karte) + CONTINUE in ROOM1130, Tuer -> 1150, Kette; Bilder per RE15_FRAMEDUMP,
# laufend weggesichert (Bildzaehler beginnt je Raum neu -> Dateinamen wiederholen sich).
B="C:/workspace/git/reAi_v2/.claude/worktrees/r35_cut1150/re15_port/build"
S="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/n2"
name=$1; karte=$2; ex=$3; fd=$4
W="$S/runs/$name"; rm -rf "$W"; mkdir -p "$W/d"; cd "$W" || exit 1
"$B/tests/unit/probe_r35_cut1150_karte.exe" re15_card.mcr 1130 $karte > karte.txt 2>&1 || { echo KARTE FAIL; cat karte.txt; exit 1; }
cp "$B/platform/pc/re15_pc.exe" "$B/platform/pc/re15_pc_r35l_n2.exe"
( n=0; while [ ! -f "$W/.fertig" ]; do
    for f in "$W"/f_*.ppm; do [ -f "$f" ] || continue; sleep 0.15; n=$((n+1)); mv "$f" "$W/d/$(printf %05d $n)_$(basename "$f")"; done
    sleep 0.3; done ) &
env RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 \
  RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60 "RE15_INPUT_SCRIPT=W1,A0.2,W200" \
  RE15_STATE_LOG=state.log RE15_IT_LOG=30 "RE15_EXIT_AT=$ex" ${fd:+"RE15_FRAMEDUMP=$fd"} \
  "$B/platform/pc/re15_pc_r35l_n2.exe" > stdout.txt 2> stderr.txt
echo "exit=$?"
sleep 1; touch "$W/.fertig"; wait
[ -f "$W/debug.log" ] || echo "kein debug.log im Laufordner"
ls "$W/d" | wc -l
