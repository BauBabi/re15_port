#!/usr/bin/env bash
# Pruefer 1 (Runde 31, Tueren): Messwerkzeug, kein Spielcode. Dossier analysis/befunde_runde31/tueren_04_pruefer1.md
# Pruefer-1-Echtlauf: echte exe, Titel-Vorlauf, Debug-Sprung, Standplatz, Tastendruck (RE15_PAD_AT, ueber
# den echten AOT-Scan), FRAMEDUMP + TUER_SERIE + Ton-Mitschnitt. Aufruf:
#   lauf.sh <name> <raum-hex> <x,z,rot|-> <pad_at> <exit_at> [KEY=VAL ...]
set -uo pipefail
W=C:/workspace/git/reAi_v2/.claude/worktrees/r31_tueren
NAME=$1; RAUM=$2; POS=$3; PAD=$4; EXITAT=$5; shift 5
Z=$W/build/r31_tueren/t4p1/echt_$NAME
rm -rf "$Z"; mkdir -p "$Z/serie"
export RE15_NO_INTRO=1
[ -z "${ELZA:-}" ] && export RE15_TITLE_SHOT="$Z/titel.bmp"
[ -z "${ELZA:-}" ] && export RE15_TITLE_SHOT_AF=2
[ -n "${ELZA:-}" ] && export RE15_INPUT_SCRIPT_START=0 RE15_INPUT_SCRIPT="W4,S0.1" RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1
export RE15_DEBUG_JUMP="${RAUM}@${JUMP:-gp}"
[ "$POS" != "-" ] && export RE15_PLAYER_POS="$POS"
export RE15_PAD_AT="$PAD"
export RE15_EXIT_AT="$EXITAT"
export RE15_TUER_SERIE="$Z/serie"
export RE15_FRAMEDUMP="${DUMP:-0-400/2}:$Z/f_"
export RE15_AUDIO_CAP_SYNC="$Z/ton.pcm"
export RE15_SE_DEBUG=1
for kv in "$@"; do export "$kv"; done
cd "$Z"
timeout -k 5 "${SEK:-240}" "$W/re15_port/build/platform/pc/re15_pc.exe" > "$Z/stdout.txt" 2> "$Z/stderr.txt"
echo "exit=$?"
grep -a "AUTO-JUMP\|JUMP ->\|parity\|DOOR FIRE\|\[tuer\]\|room[0-9a-f]*.rdt\|EXIT_AT\|\[se\] Stimme\|Zwischensequenz\|\[aot\]" "$Z/debug.log" 2>/dev/null | cut -c1-220 | head -60
