#!/usr/bin/env bash
# messlauf.sh <name> <raum-hex> <x> <y> <z> <gier> <cut> <flags...> -- <ENV=WERT ...>
# Schreibt Karte, startet Messkopie der exe am LADE-Weg (CONTINUE), sammelt Logs.
BAUM="C:/workspace/git/reAi_v2/.claude/worktrees/r38_npckante"
SP="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad"
name=$1; shift
raum=$1; x=$2; y=$3; z=$4; gier=$5; cut=$6; shift 6
flags=()
while [ $# -gt 0 ] && [ "$1" != "--" ]; do flags+=("$1"); shift; done
[ "$1" = "--" ] && shift
dir="$SP/mess_$name"; rm -rf "$dir"; mkdir -p "$dir"
"$BAUM/re15_port/build/tests/unit/probe_r38_npckante_karte.exe" "$dir/re15_card.mcr" $raum $x $y $z $gier $cut "${flags[@]}" > "$dir/karte.txt" 2>&1 || { echo "KARTE FAIL"; cat "$dir/karte.txt"; exit 1; }
BUILD="${MESS_BUILD:-$BAUM/re15_port/build}"   # MESS_BUILD = anderes Bauverzeichnis (Gegenprobe build_bisect)
exe="$BUILD/platform/pc/re15_pc_r38m_$name.exe"
cp "$BUILD/platform/pc/re15_pc.exe" "$exe"
( cd "$dir" && env RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_NOAUDIO=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 \
    RE15_NPCKANTE_LOG="$dir/npckante.log" "$@" timeout 240 "$exe" > "$dir/stdout.txt" 2> "$dir/stderr.txt" ); rv=$?
rm -f "$exe"
echo "exit=$rv dir=$dir"; cat "$dir/karte.txt"
ls "$dir"
