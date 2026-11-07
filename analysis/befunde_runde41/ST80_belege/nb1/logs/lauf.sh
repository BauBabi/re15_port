#!/usr/bin/env bash
# ST80 Nachbesserung 1 - eigener exe-Lauf (Messkopie unter eigenem Namen), Muster ST80_belege/abn1/logs/lauf.sh.
#   lauf.sh <name> <exe-basis> "<karten-args>" "<input-skript>" <exit_at> <wachhund-s> [ENV=WERT ...]
# Ausgabe: $M/laeufe/<name>/ (debug.log, state.log, fd_*.ppm bei RE15_FRAMEDUMP). Beendet nur die eigene PID.
set -uo pipefail
M="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/st80nb1"
NAME="$1"; BASIS="$2"; KARTE="$3"; SKRIPT="$4"; EXITAT="$5"; WACH="$6"; shift 6
RUN="$M/laeufe/$NAME"; rm -rf "$RUN"; mkdir -p "$RUN"; cd "$RUN" || exit 2
EXEN="re15_pc_st80nb1_${NAME}"
cp "$M/bin/$BASIS.exe" "$M/bin/$EXEN.exe" || exit 2
KT="$M/bin/karte_neu.exe"
# shellcheck disable=SC2086
"$KT" re15_card.mcr $KARTE > karte.log 2>&1 || { cat karte.log; exit 2; }
export SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_WINDOW_SCALE=2 RE15_MSG_LOG=1 RE15_STATE_LOG=state.log
export RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 SDL_AUDIODRIVER=dummy
export RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60 "RE15_INPUT_SCRIPT=$SKRIPT" "RE15_EXIT_AT=$EXITAT"
for kv in "$@"; do export "$kv"; done
T0=$(date +%s)
"$M/bin/$EXEN.exe" > stdout.log 2> stderr.log &
GP=$!
PS=/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe
WPID=""
for i in $(seq 1 30); do
  WPID=$("$PS" -NoProfile -Command "Get-Process $EXEN -ErrorAction SilentlyContinue | Select-Object -First 1 | ForEach-Object { \$_.Id }" 2>/dev/null | tr -d '\r')
  [ -n "$WPID" ] && break
  sleep 1
done
echo "exe $EXEN PID ${WPID:-?}" | tee lauf.info
while kill -0 "$GP" 2>/dev/null; do
  if [ $(( $(date +%s) - T0 )) -gt "$WACH" ]; then
    echo "WACHHUND: $WACH s ueberschritten -> eigene PID $WPID beenden" | tee -a lauf.info
    [ -n "$WPID" ] && taskkill //PID "$WPID" //T //F >/dev/null 2>&1
    break
  fi
  sleep 2
done
wait "$GP" 2>/dev/null; RC=$?
echo "exit=$RC dauer=$(( $(date +%s) - T0 ))s" | tee -a lauf.info
rm -f "$M/bin/$EXEN.exe"
grep -a "EXIT_AT\|CONTINUE: resumed" debug.log 2>/dev/null | head -3
grep -a "\[msgtext\]\|\[story80\]" debug.log 2>/dev/null
