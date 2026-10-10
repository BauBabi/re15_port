#!/usr/bin/env bash
# kette_lauf.sh — Runde 36 Spur K: ein Lauf der Irons-Kette mit der ECHTEN exe (Messkopie unter eigenem Namen),
# Spielstand vor der Tuer 1130 -> 1150 + CONTINUE + Aktionstaste (Weg wie integration_r35_cut1150 Lauf A).
#
# Aufruf: kette_lauf.sh <baum> <laufordner> <exit_at "N#Raum"> [karten-schalter ...] -- [VAR=WERT ...]
#   karten-schalter: nach10f0 ersteszene [tot1140] [tot1070] [tor1040offen] ... (probe_r35_cut1150_karte)
#   VAR=WERT: zusaetzliche Umgebung, z.B. RE15_FRAMEDUMP=0-500/10:f_  RE15_AUDIO_CAP_SYNC=cap.raw
# Ergebnis im Laufordner: debug.log, state.log, ggf. f_*.ppm / cap.raw.
set -u
BAUM="$1"; LAUF="$2"; EXIT_AT="$3"; shift 3
KARTE=(); while [ $# -gt 0 ] && [ "$1" != "--" ]; do KARTE+=("$1"); shift; done
[ "${1:-}" = "--" ] && shift
BUILD="$BAUM/re15_port/build"
EXE="$BUILD/platform/pc/re15_pc.exe"
# K_KOPIE=<exe>: diese (schon vorhandene) Messkopie benutzen statt neu zu kopieren (Laeufe gegen einen festen Stand).
KOPIE="${K_KOPIE:-$BUILD/platform/pc/re15_pc_r36k_mess.exe}"
TOOL="$BUILD/tests/unit/probe_r35_cut1150_karte.exe"
mkdir -p "$LAUF"; rm -f "$LAUF"/debug.log "$LAUF"/state.log "$LAUF"/re15_card.mcr "$LAUF"/*.ppm "$LAUF"/cap.raw
[ -n "${K_KOPIE:-}" ] || cp -f "$EXE" "$KOPIE"
( cd "$LAUF" && "$TOOL" re15_card.mcr 1130 "${KARTE[@]}" ) > "$LAUF/karte.txt" 2>&1 || { echo "Karte fehlgeschlagen"; cat "$LAUF/karte.txt"; exit 1; }
cd "$LAUF"
env RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 \
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60 "RE15_INPUT_SCRIPT=W1,A0.2,W200" \
    RE15_STATE_LOG=state.log RE15_IT_LOG=10 RE15_SE_DEBUG=1 RE15_SPAWN_DIAG=1 RE15_NECK_LOG=neck.log \
    "RE15_EXIT_AT=$EXIT_AT" "$@" "$KOPIE" > stdout.txt 2>&1
echo "exit=$? $(ls "$LAUF" | wc -l) Dateien"
