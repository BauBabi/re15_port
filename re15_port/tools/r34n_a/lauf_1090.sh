#!/usr/bin/env bash
# lauf_1090.sh — Spur A (Runde 34 Nacht), Gegenpruefungs-Auflage 3: ist der Suedteil von ROOM1050
# bei GESCHLOSSENEM Rolltor erreichbar, ueber ROOM10F0 -> ROOM1090 (obere Ebene) -> Tuer Slot 0?
#
# Echte re15_pc.exe (eigene Kopie), beschleunigter Renderer, RE15_FRAMEDUMP, state.log.
# Neues Spiel (RE15_TITLE_SHOT) -> Debug-JUMP 10F0@120 -> ROOM10F0 Tuer Slot 1 (main00 @0x00F52,
# Ziel Raum 0x09, Ankunft (-13600,-9000,1300), Blick 3072) per RE15_FIRE_AOT="1@200#10F0" ->
# ROOM1090. Der Bildzaehler beginnt je Raum bei 0 (main.c); das Skript startet in jedem Raum bei
# START (Standard 300 = hinter dem Tuerschuss in 10F0).
# Das Feuer brennt: (3,129)=0 (sub00 @0x021C8 / @0x021EE, Emitter Typ 0x26 @0x02214..@0x0228C).
#
#   lauf_1090.sh <zielverzeichnis>
# Umgebung: SKRIPT (Eingabe ab START), START, SERIE (Framedump), ENDE (Exit-Bild in ROOM1090 bzw.
#           ENDRAUM), ENDRAUM (hex, Standard 1090), FLAGS (RE15_SET_FLAG_AT ohne @), SEK.
set -uo pipefail
ZIEL="${1:?Zielverzeichnis}"
HIER="$(cd "$(dirname "$0")" && pwd)"
BAUM="$(cd "$HIER/../../.." && pwd)"
SRC="$BAUM/re15_port/build/platform/pc/re15_pc.exe"
EXE="$BAUM/re15_port/build/platform/pc/re15_pc_r34n_a.exe"
cp -f "$SRC" "$EXE" || exit 2
mkdir -p "$ZIEL"; ZIEL="$(cd "$ZIEL" && pwd)"
rm -f "$ZIEL"/*.ppm "$ZIEL"/debug.log "$ZIEL"/stderr.txt "$ZIEL"/state.log

export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=3
export RE15_TITLE_SHOT=titel.bmp RE15_TITLE_SHOT_AF=2
export RE15_DEBUG_JUMP="10F0@120"
export RE15_FIRE_AOT="1@200#10F0"
export RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START="${START:-300}"
export RE15_INPUT_SCRIPT="${SKRIPT:-W1,R0.3667,U1.67,L0.3667,U6,W2}"
export RE15_STATE_LOG=state.log RE15_MSG_LOG=1 RE15_EVT_TRACE=1
export RE15_FRAMEDUMP="${SERIE:-300-820/20}:f_"
export RE15_EXIT_AT="${ENDE:-820}#${ENDRAUM:-1090}"
[ -n "${FLAGS:-}" ] && export RE15_SET_FLAG_AT="$FLAGS@150"
cd "$ZIEL"
timeout -k 5 "${SEK:-240}" "$EXE" > "$ZIEL/stdout.txt" 2> "$ZIEL/stderr.txt"
echo "exit=$?"
grep -a -E "AUTO-JUMP|JUMP ->|room\] PC loaded|EXIT_AT|fire-aot|\[msg\]|Cut_chg|\[evt\]|DOOR FIRE|setflag" "$ZIEL/debug.log" 2>/dev/null | head -60
ls "$ZIEL"/*.ppm 2>/dev/null | wc -l
