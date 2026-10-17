#!/usr/bin/env bash
# Runde 38 Spur A — allgemeiner Messlauf: echte exe unter eigenem Namen, Umgebung aus ENV (Leerzeichen-getrennt),
# Framedump BILD bei SKALA, Ende BILD+2. ALT=1 = Gegenprobe mit der vor dem Fix gesicherten exe (Stand 1bcbd0ff).
#   $1 Marke (Unterordner von re15_port/build/r38_a/)
set -uo pipefail
WT=/c/workspace/git/reAi_v2/.claude/worktrees/r38_cursor
SRC_EXE="$WT/re15_port/build/platform/pc/re15_pc.exe"
EXE="$WT/re15_port/build/platform/pc/re15_pc_r38a.exe"
[ "$SRC_EXE" -nt "$EXE" ] && cp -f "$SRC_EXE" "$EXE"
[ "${ALT:-0}" = "1" ] && EXE="$WT/re15_port/build/platform/pc/re15_pc_r38a_alt.exe"
Z="$WT/re15_port/build/r38_a/$1"; mkdir -p "$Z"; cd "$Z" || exit 2
rm -f debug.log befund.log f_*.ppm lauf_rc.txt
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_TITLE_SHOT=title.bmp
export RE15_WINDOW_SCALE="${SKALA:-1}"
export RE15_FRAMEDUMP="${BILD:-300}:f_${SKALA:-1}.ppm"
export RE15_EXIT_AT="$(( ${BILD:-300} + 2 ))"
for kv in ${ENV:-}; do export "$kv"; done
timeout -k 5 "${SEK:-120}" "$EXE"
echo "rc=$? Bilder=$(ls f_*.ppm 2>/dev/null | wc -l)" > lauf_rc.txt
cat lauf_rc.txt
