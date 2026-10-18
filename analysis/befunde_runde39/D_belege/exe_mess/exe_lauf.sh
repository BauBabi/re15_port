#!/bin/bash
# exe_lauf.sh <name> <raum> <pos x,z,rot> <skript> <ende_bild> [extra env ...]   (Muster test_r30_cut_blitz.cmake)
S="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad"
EXE="C:/workspace/git/reAi_v2/.claude/worktrees/r39_treffer/re15_port/build/platform/pc/re15_pc_r39d_mess.exe"
name=$1; raum=$2; pos=$3; skript=$4; ende=$5; shift 5
wd="$S/lauf_$name"; rm -rf "$wd"; mkdir -p "$wd"; cd "$wd"
env -i PATH="$PATH" SYSTEMROOT="$SYSTEMROOT" TEMP="$TEMP" TMP="$TMP" USERPROFILE="$USERPROFILE" LOCALAPPDATA="$LOCALAPPDATA" APPDATA="$APPDATA" \
  RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1 \
  RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 \
  RE15_DEBUG_JUMP="${raum}@240" RE15_PLAYER_POS="$pos" RE15_EQUIP=3 \
  RE15_INPUT_SCRIPT_START=330 "RE15_INPUT_SCRIPT=$skript" \
  RE15_STATE_LOG=state.log RE15_TREFFER_LOG=treffer.log "RE15_EXIT_AT=${ende}#${raum}" "$@" \
  timeout 240 "$EXE" > stdout.txt 2> stderr.txt
echo "$name exit=$? $(wc -l < state.log 2>/dev/null) state-Zeilen, $(wc -l < treffer.log 2>/dev/null) treffer-Zeilen"
