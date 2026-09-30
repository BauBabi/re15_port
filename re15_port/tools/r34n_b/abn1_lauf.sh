#!/usr/bin/env bash
# Spur B (Runde 34 Nacht) — ABNAHME 1: Messlauf an der ECHTEN exe unter EIGENEM Namen (re15_abn1.exe;
# local_build.sh beendet nur Prozesse namens re15_pc unter seinem Bauverzeichnis), beschleunigter
# Renderer, RE15_FRAMEDUMP bei RE15_WINDOW_SCALE=3, kein AUTOSHOT/SOFTWARE_RENDER.
# Zwei Wege:
#   WEG=sprung (Default): RE15_DEBUG_JUMP="<RAUM>@<JUMP_AT>" + RE15_PLAYER_POS (Debug-Menue-Weg)
#   WEG=laden:            Spielstand mit probe_r30_granate_karte (KARTE = "sicherung granate ..."), CONTINUE
#   $1 Marke (Unterordner von build/r34n_b_abn1/)
#   Umgebung: RAUM (1150), SKRIPT, START (200 bzw. 60), SERIE ("a-b/s"), EXIT_AT, SEK (200),
#             POS ("x,z,rot"), JUMP_AT (240), EXTRA_ENV ("A=1 B=2"), AUDIO=1 (Ton + RE15_SE_DEBUG)
set -uo pipefail
WT=/c/workspace/git/reAi_v2/.claude/worktrees/r34n_hebetisch
SRC_EXE="$WT/re15_port/build/platform/pc/re15_pc.exe"
EXE="$WT/re15_port/build/platform/pc/re15_abn1.exe"
KARTE_EXE="$WT/re15_port/build/tests/unit/probe_r30_granate_karte.exe"
[ "$SRC_EXE" -nt "$EXE" ] && cp -f "$SRC_EXE" "$EXE"
RAUM="${RAUM:-1150}"
WEG="${WEG:-sprung}"
Z="$WT/build/r34n_b_abn1/$1"; mkdir -p "$Z"; cd "$Z" || exit 2
rm -f debug.log befund.log f_*.ppm fb_*.ppm modal.log hebetisch.log cursor.log lauf_rc.txt re15_card.mcr
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
if [ "${AUDIO:-0}" = "1" ]; then export RE15_SE_DEBUG=1; else export RE15_NOAUDIO=1; fi
export RE15_NO_INTRO=1 RE15_WINDOW_SCALE=3 RE15_MSG_LOG=1
[ "$WEG" = "laden" ] || export RE15_TITLE_SHOT="title.bmp" RE15_TITLE_SHOT_AF=2   # Titel automatisch weiter (wie lauf.sh)
if [ "$WEG" = "laden" ]; then
    "$KARTE_EXE" re15_card.mcr "$RAUM" ${KARTE:-} || { echo "Kartenwerkzeug fehlgeschlagen"; exit 3; }
    export RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
    export RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START="${START:-60}"
else
    export RE15_DEBUG_JUMP="${RAUM}@${JUMP_AT:-240}"
    [ -n "${POS:-}" ] && export RE15_PLAYER_POS="$POS"
    export RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START="${START:-200}"
fi
export RE15_INPUT_SCRIPT="${SKRIPT:-W60}"
[ -n "${SERIE:-}" ] && export RE15_FRAMEDUMP="${SERIE}:f_"
export RE15_MODAL_LOG="modal.log" RE15_HEBETISCH_LOG="hebetisch.log" RE15_HEBETISCH_CURSOR_LOG="cursor.log"
export RE15_EXIT_AT="${EXIT_AT:-400#$RAUM}"
if [ -n "${EXTRA_ENV:-}" ]; then for kv in $EXTRA_ENV; do export "$kv"; done; fi
timeout -k 5 "${SEK:-200}" "$EXE"
echo "rc=$? Bilder=$(ls f_*.ppm 2>/dev/null | wc -l)" > lauf_rc.txt
cat lauf_rc.txt
