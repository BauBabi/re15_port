#!/usr/bin/env bash
# Runde 38 Spur A — Messlauf ROOM11F0 Cursor (Generator-Raetsel), echte exe unter eigenem Namen.
# Sprung per Debug-Menue (RE15_DEBUG_JUMP), Kamera auf Cut 10 (die Raetselkamera, sub16 `29 0a`),
# Raetsel scharf wie sub16 @0x015C2..0x015F2 (13x Set(5,b,1)), Framedump.
#   $1 Marke (Unterordner von re15_port/build/r38_a/), SKALA (1), BILD (300), EXTRA_ENV, ALT (0|1)
set -uo pipefail
WT=/c/workspace/git/reAi_v2/.claude/worktrees/r38_cursor
SRC_EXE="$WT/re15_port/build/platform/pc/re15_pc.exe"
EXE="$WT/re15_port/build/platform/pc/re15_pc_r38a.exe"
[ "$SRC_EXE" -nt "$EXE" ] && cp -f "$SRC_EXE" "$EXE"
# ALT=1: Gegenprobe mit der vor dem Fix gesicherten exe (re15_pc_r38a_alt.exe, Stand 1bcbd0ff)
[ "${ALT:-0}" = "1" ] && EXE="$WT/re15_port/build/platform/pc/re15_pc_r38a_alt.exe"
Z="$WT/re15_port/build/r38_a/$1"; mkdir -p "$Z"; cd "$Z" || exit 2
rm -f debug.log befund.log f_*.ppm lauf_rc.txt
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_TITLE_SHOT=1
export RE15_WINDOW_SCALE="${SKALA:-1}"
export RE15_DEBUG_JUMP="11F0@30" RE15_FORCE_CUT=10
export RE15_SET_FLAG_AT="5:0,5:1,5:2,5:3,5:4,5:5,5:6,5:7,5:8,5:9,5:10,5:11,5:12@240"
export RE15_FRAMEDUMP="${BILD:-300}:f_${SKALA:-1}.ppm"
export RE15_EXIT_AT="$(( ${BILD:-300} + 2 ))"
if [ -n "${EXTRA_ENV:-}" ]; then for kv in $EXTRA_ENV; do export "$kv"; done; fi
timeout -k 5 "${SEK:-90}" "$EXE"
echo "rc=$? Bilder=$(ls f_*.ppm 2>/dev/null | wc -l)" > lauf_rc.txt
cat lauf_rc.txt
