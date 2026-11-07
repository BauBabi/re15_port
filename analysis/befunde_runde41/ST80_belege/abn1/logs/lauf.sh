#!/usr/bin/env bash
# ST80 Abnahme 1 - eigener exe-Lauf (Messkopie), echtes Fenster, Renderer des Nutzers (kein SOFTWARE_RENDER/AUTOSHOT).
#   lauf.sh <name> <kartenwerkzeug.exe> "<karten-args>" "<input-skript>" <exit_at> <wachhund-s> [ENV=WERT ...]
# Ausgabe: $M/laeufe/<name>/ (debug.log, state.log, stimme.log, fd_*.ppm bei RE15_FRAMEDUMP).
set -uo pipefail
M="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/testint/st80_abn1"
NAME="$1"; TOOL="$2"; KARTE="$3"; SKRIPT="$4"; EXITAT="$5"; WACH="$6"; shift 6
RUN="$M/laeufe/$NAME"; rm -rf "$RUN"; mkdir -p "$RUN"; cd "$RUN" || exit 2
EXEN="re15_pc_st80a1_${NAME}"
cp "$M/bin/re15_pc_st80a1.exe" "$M/bin/$EXEN.exe" || exit 2
# shellcheck disable=SC2086
"$M/bin/$TOOL" re15_card.mcr $KARTE > karte.log 2>&1 || { cat karte.log; exit 2; }
export SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_WINDOW_SCALE=2 RE15_MSG_LOG=1 RE15_STIMME_LOG=1 RE15_STATE_LOG=state.log
export RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
export RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60 "RE15_INPUT_SCRIPT=$SKRIPT" "RE15_EXIT_AT=$EXITAT"
for kv in "$@"; do export "$kv"; done
T0=$(date +%s)
"$M/bin/$EXEN.exe" > stdout.log 2> stderr.log &
GP=$!
PS=/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe
WPID=""; HWND=0
for i in $(seq 1 30); do
  set -- $("$PS" -NoProfile -Command "Get-Process $EXEN -ErrorAction SilentlyContinue | Select-Object -First 1 | ForEach-Object { '{0} {1}' -f \$_.Id, \$_.MainWindowHandle }" 2>/dev/null | tr -d '\r')
  WPID="${1:-}"; HWND="${2:-0}"
  [ -n "$WPID" ] && [ "$HWND" != "0" ] && break
  sleep 1
done
echo "exe $EXEN PID ${WPID:-?} Fenster $HWND" | tee lauf.info
if [ -n "$WPID" ] && [ "$HWND" != "0" ]; then
  "$PS" -NoProfile -Command "Add-Type -Name F -Namespace R -MemberDefinition '[DllImport(\"user32.dll\")] public static extern bool SetWindowPos(System.IntPtr h, System.IntPtr a, int x, int y, int w, int g, uint f);'; [void][R.F]::SetWindowPos([System.IntPtr]$HWND, [System.IntPtr](-1), 1200, 40, 0, 0, 0x41)" >/dev/null 2>&1
fi
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
