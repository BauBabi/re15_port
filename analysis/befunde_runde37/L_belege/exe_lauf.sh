#!/bin/bash
# Runde 37 Spur L — Messlauf mit der ECHTEN exe (Muster Spur H Abnahme N1/N2/N4): Spielstand per Kartenwerkzeug + CONTINUE,
# Eingabeskript ab Spielbild 60, RE15_STATE_LOG, Ende per RE15_EXIT_AT. Eigene Messkopie der exe (nie taskkill /IM).
# Aufruf: exe_lauf.sh <name> <rundir> <exe> <kartenwerkzeug> "<kartenargumente>" "<eingabeskript>" "<exit_at>" [weitere ENV=WERT ...]
set -u
NAME="$1"; RUN="$2"; EXE="$3"; KARTE="$4"; KARGS="$5"; SKRIPT="$6"; EXITAT="$7"; shift 7
mkdir -p "$RUN"
EXEDIR="$(dirname "$EXE")"
KOPIE="$EXEDIR/re15_pc_r37l_mess_${NAME}.exe"
cp -f "$EXE" "$KOPIE"
( cd "$RUN" && rm -f re15_card.mcr state.log debug.log && "$KARTE" re15_card.mcr $KARGS > karte.txt 2>&1 )
[ -f "$RUN/re15_card.mcr" ] || { echo "KARTE FEHLT ($RUN/karte.txt)"; exit 1; }
(
  cd "$RUN"
  export RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
  export RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60 RE15_INPUT_SCRIPT="$SKRIPT" RE15_STATE_LOG=state.log
  export RE15_EXIT_AT="$EXITAT"
  for kv in "$@"; do export "$kv"; done
  timeout 900 "$KOPIE" > stdout.txt 2>&1
  echo "exit=$?" > exit.txt
)
rm -f "$KOPIE"
echo "LAUF $NAME fertig: $(cat "$RUN/exit.txt") state.log $(wc -l < "$RUN/state.log" 2>/dev/null) Zeilen"
