#!/usr/bin/env bash
# lauf_laden.sh — Spur A (Runde 34 Nacht), Gegenpruefungs-Auflage 4d: Laden/Speichern ueber den
# ECHTEN Karten-Weg (re15_card.mcr schreiben -> CONTINUE -> Raum -> Schalter).
#
# Schritt 0  Startkarte: probe_r33_speichern_karte (Werkzeug von integration_r33_speichern) schreibt
#            Platz 0 = ROOM1150 vor der Schreibmaschine (-22689,-19693, Blick 1864) mit Memory Card.
# Schritt 1  SPEICHERN im Spiel: CONTINUE aus Platz 0 (RE15_CONTINUE_TEST + RE15_CARD_AUTO), dann
#            RE15_SET_FLAG_AT 9:63 (= "Sicherung eingesetzt"; das Setzen selbst belegen die Laeufe
#            mit/elza_mit in lauf.sh), dann dieselben Tasten wie integration_r33_speichern Fall B
#            (150:A,200:A,290:A,370:A = Schreibmaschine, Frage, Ja, Kartenschirm) -> gespeichert in
#            Platz 1 (RE15_CARD_SLOT=0,1).
# Schritt 2  LADEN: CONTINUE aus Platz 1 -> ROOM1150 -> Debug-JUMP 1050@120 -> Leons Weg zum Schalter
#            (wie lauf.sh) -> Schalter: erwartet der AUSGELIEFERTE sub02 (keine [rolltor]-Zeile),
#            Tor faehrt (Se_on 2/0x0C,0x0A,0x0B).
# Schritt 2G GEGENPROBE: dasselbe aus Platz 0 (ohne Bit 63, ohne Sicherung) -> Port-Programm OHNE.
#
#   lauf_laden.sh <zielverzeichnis>
set -uo pipefail
ZIEL="${1:?Zielverzeichnis}"
HIER="$(cd "$(dirname "$0")" && pwd)"
BAUM="$(cd "$HIER/../../.." && pwd)"
SRC="$BAUM/re15_port/build/platform/pc/re15_pc.exe"
EXE="$BAUM/re15_port/build/platform/pc/re15_pc_r34n_a.exe"
KARTE="$BAUM/re15_port/build/tests/unit/probe_r33_speichern_karte.exe"
cp -f "$SRC" "$EXE" || exit 2
mkdir -p "$ZIEL"; ZIEL="$(cd "$ZIEL" && pwd)"
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'

WEG="U1.8,R0.3667,U0.9,L0.3667,U8.5,L0.3667,U0.7,W0.5"
TOR="A0.1,W5,A0.1,W14"

lauf() {   # lauf <unterordner> <env...>
    local d="$ZIEL/$1"; shift
    mkdir -p "$d"; rm -f "$d"/debug.log "$d"/state.log "$d"/*.ppm
    [ -f "$ZIEL/re15_card.mcr" ] && cp -f "$ZIEL/re15_card.mcr" "$d/re15_card.mcr"
    ( cd "$d" && env RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=3 RE15_CONTINUE_TEST=1 \
        RE15_CARD_AUTO=1 RE15_MSG_LOG=1 RE15_EVT_TRACE=1 RE15_SE_DEBUG=1 RE15_STATE_LOG=state.log \
        "$@" timeout -k 5 240 "$EXE" > stdout.txt 2> stderr.txt; echo "exit=$?" )
    grep -a -E "CONTINUE|PC loaded|AUTO-JUMP|setflag|\[save\]|card=|\[rolltor\]|\[msg\] room=10|Cut_chg|\[se\] SCD|EXIT_AT" "$d/debug.log" | head -40
}

echo "=== Schritt 0: Startkarte"
( cd "$ZIEL" && rm -f re15_card.mcr && "$KARTE" re15_card.mcr karte )

echo "=== Schritt 1: speichern in ROOM1150 mit (9,63)=1"
lauf s1 RE15_CARD_SLOT=0,1 RE15_SET_FLAG_AT="9:63@100" RE15_PAD_AT="150:A,200:A,290:A,370:A" \
        RE15_EXIT_AT="460#1150"
cp -f "$ZIEL/s1/re15_card.mcr" "$ZIEL/re15_card.mcr"

echo "=== Schritt 2: laden aus Platz 1 -> ROOM1050 -> Schalter"
lauf s2 RE15_CARD_SLOT=1 RE15_DEBUG_JUMP="1050@120" RE15_PLAYER_POS="17000,12000,1024" \
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=200 RE15_INPUT_SCRIPT="$WEG,$TOR" \
        RE15_FRAMEDUMP="600-1400/100:f_" RE15_EXIT_AT="1400#1050"

echo "=== Schritt 2G: Gegenprobe laden aus Platz 0 (ohne Bit 63) -> Schalter"
lauf s2g RE15_CARD_SLOT=0 RE15_DEBUG_JUMP="1050@120" RE15_PLAYER_POS="17000,12000,1024" \
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=200 RE15_INPUT_SCRIPT="$WEG,A0.1,W5,A0.1,W4,A0.1,W4" \
        RE15_EXIT_AT="1000#1050"
