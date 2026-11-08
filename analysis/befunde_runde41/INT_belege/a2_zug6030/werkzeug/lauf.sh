#!/usr/bin/env bash
# Integration A2 - eigener exe-Lauf (Messkopie re15_pc_inta2_<name>.exe im Bau-Ordner, Muster test_r36_story.cmake),
# beendet nur die eigene PID.
#   lauf.sh <name> "<karten-args>" "<input-skript>" <exit_at> <wachhund-s> [ENV=WERT ...]
set -uo pipefail
M="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/int_a2"
B="C:/workspace/git/reAi_v2/.claude/worktrees/r41_integration/re15_port/build"
NAME="$1"; KARTE="$2"; SKRIPT="$3"; EXITAT="$4"; WACH="$5"; shift 5
RUN="$M/laeufe/$NAME"; rm -rf "$RUN"; mkdir -p "$RUN"; cd "$RUN" || exit 2
EXEN="re15_pc_inta2_${NAME}"
cp "$B/platform/pc/re15_pc.exe" "$B/platform/pc/$EXEN.exe" || exit 2
if [ -n "$KARTE" ]; then
  # shellcheck disable=SC2086
  "$B/tests/unit/probe_r36_story_karte.exe" re15_card.mcr $KARTE > karte.log 2>&1 || { cat karte.log; exit 2; }
  export RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
fi
export SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_MSG_LOG=1 RE15_STATE_LOG=state.log
export SDL_AUDIODRIVER=dummy
export RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START="${START:-60}" "RE15_INPUT_SCRIPT=$SKRIPT" "RE15_EXIT_AT=$EXITAT"
for kv in "$@"; do export "$kv"; done
T0=$(date +%s)
"$B/platform/pc/$EXEN.exe" > stdout.log 2> stderr.log &
GP=$!
PS=/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe
WPID=""
for i in $(seq 1 30); do
  WPID=$("$PS" -NoProfile -Command "Get-Process $EXEN -ErrorAction SilentlyContinue | Select-Object -First 1 | ForEach-Object { \$_.Id }" 2>/dev/null | tr -d '\r')
  [ -n "$WPID" ] && break
  sleep 1
done
echo "exe $EXEN PID ${WPID:-?} sha1 $(sha1sum "$B/platform/pc/$EXEN.exe" | cut -c1-40)" | tee lauf.info
while kill -0 "$GP" 2>/dev/null; do
  if [ $(( $(date +%s) - T0 )) -gt "$WACH" ]; then
    echo "WACHHUND: $WACH s ueberschritten -> eigene PID $WPID beenden" | tee -a lauf.info
    [ -n "$WPID" ] && MSYS_NO_PATHCONV=1 taskkill /PID "$WPID" /T /F >/dev/null 2>&1
    break
  fi
  sleep 2
done
wait "$GP" 2>/dev/null; RC=$?
echo "exit=$RC dauer=$(( $(date +%s) - T0 ))s" | tee -a lauf.info
rm -f "$B/platform/pc/$EXEN.exe"
