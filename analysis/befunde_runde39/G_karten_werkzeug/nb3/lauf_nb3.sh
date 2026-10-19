#!/bin/bash
# Nachbesserung 3 (N2): lauf_nb3.sh <exe> <arbeitsordner> <oeffnen_raum_hex> <timeout_s> <karte-args...> -- <ENV=...>...
# = Muster lauf.sh der Abnahme 2 (G39abn2): Spielstand per probe_r36_karte karte (Slot 0), exe-KOPIE mit eigenem Namen
# re15_pc_g39nb3_<id>.exe neben der Quelle (nie fremde Kopien loeschen), Titel -> LOAD GAME (RE15_CONTINUE_TEST +
# RE15_CARD_AUTO), Kartenschirm per RE15_INV_OPEN_AT=40#<raum>, Abzug per RE15_INV_FB_SHOT (RE15_WINDOW_SCALE=1).
WT="C:/workspace/git/reAi_v2/.claude/worktrees/r39_karten"
PROBE="$WT/re15_port/build/tests/unit/probe_r36_karte.exe"
SRC="$1"; shift; WD="$1"; shift; RAUM="$1"; shift; TO="$1"; shift
KA=()
while [ $# -gt 0 ] && [ "$1" != "--" ]; do KA+=("$1"); shift; done
[ "$1" = "--" ] && shift
mkdir -p "$WD"; rm -f "$WD/re15_card.mcr" "$WD"/*.bmp
( cd "$WD" && "$PROBE" karte re15_card.mcr "${KA[@]}" ) > "$WD/karte.txt" 2>&1 || { echo "KARTE FAIL"; cat "$WD/karte.txt"; exit 1; }
ID=$RANDOM$RANDOM
D="$(dirname "$SRC")"
EXE="$D/re15_pc_g39nb3_$ID.exe"
cp "$SRC" "$EXE"
( cd "$WD" && env RE15_NO_INTRO=1 RE15_NOAUDIO=1 SDL_AUDIODRIVER=dummy RE15_WINDOW_SCALE=1 \
     RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 "RE15_INV_OPEN_AT=40#$RAUM" "$@" \
     timeout "$TO" "$EXE" > "$WD/stdout.txt" 2> "$WD/stderr.txt" ); echo "exit=$?"
rm -f "$EXE"
