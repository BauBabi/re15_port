#!/usr/bin/env bash
# Runde 38 Spur A — Messlauf Hebetisch-Cursor ROOM1150/1151 am LADE-Weg (Muster tools/r34n_b/lauf_laden.sh):
# Spielstand mit probe_r30_granate_karte (Spieler (-22250,0,-18500) rot 0 = Westseite des Tischs), CONTINUE,
# Eingabeskript "W1,A0.2,W..." = Aktion am Tisch -> Cursor (Cut 4), Framedump.
#   $1 Marke (Unterordner von re15_port/build/r38_a/); RAUM (1150|1151), SKALA (1), BILD (150), ALT (0|1), EXTRA_ENV
set -uo pipefail
WT=/c/workspace/git/reAi_v2/.claude/worktrees/r38_cursor
SRC_EXE="$WT/re15_port/build/platform/pc/re15_pc.exe"
EXE="$WT/re15_port/build/platform/pc/re15_pc_r38a.exe"
[ "$SRC_EXE" -nt "$EXE" ] && cp -f "$SRC_EXE" "$EXE"
[ "${ALT:-0}" = "1" ] && EXE="$WT/re15_port/build/platform/pc/re15_pc_r38a_alt.exe"
KARTE_EXE="$WT/re15_port/build/tests/unit/probe_r30_granate_karte.exe"
RAUM="${RAUM:-1150}"
Z="$WT/re15_port/build/r38_a/$1"; mkdir -p "$Z"; cd "$Z" || exit 2
rm -f debug.log befund.log f_*.ppm lauf_rc.txt re15_card.mcr cursor.log hebetisch.log
"$KARTE_EXE" re15_card.mcr "$RAUM" ${KARTE:-} > /dev/null || { echo "Kartenwerkzeug fehlgeschlagen"; exit 3; }
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
export RE15_WINDOW_SCALE="${SKALA:-1}"
export RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60 RE15_INPUT_SCRIPT="${SKRIPT:-W1,A0.2,W4}"
export RE15_HEBETISCH_CURSOR_LOG=cursor.log RE15_HEBETISCH_LOG=hebetisch.log
export RE15_FRAMEDUMP="${BILD:-150}:f_${SKALA:-1}.ppm"
export RE15_EXIT_AT="$(( ${BILD:-150} + 2 ))#$RAUM"
if [ -n "${EXTRA_ENV:-}" ]; then for kv in $EXTRA_ENV; do export "$kv"; done; fi
timeout -k 5 "${SEK:-120}" "$EXE"
echo "rc=$? Bilder=$(ls f_*.ppm 2>/dev/null | wc -l)" > lauf_rc.txt
cat lauf_rc.txt
