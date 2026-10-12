#!/usr/bin/env bash
# D1 Messlauf: bash lauf.sh <name> <karte> <wegplan|-> <timeout_s> KEY=WERT ...
#   <karte> = "tool:<args mit ; getrennt>"  -> Kartenwerkzeug probe_r36_schluessel_karte
#           = "von:<lauf>[:<slot>]"         -> re15_card.mcr aus einem frueheren Lauf kopieren
set -u
BAUM="C:/workspace/git/reAi_v2/.claude/worktrees/r36_integration"
EXE="$BAUM/re15_port/build/platform/pc/re15_pc_D1.exe"
TOOL="$BAUM/re15_port/build/tests/unit/probe_r36_schluessel_karte.exe"
SP="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/d1"
name="$1"; karte="$2"; plan="$3"; [ "$plan" != "-" ] && plan="$(cd "$(dirname "$plan")" && pwd)/$(basename "$plan")"; to="$4"; shift 4
dir="$SP/runs/$name"
rm -rf "$dir"; mkdir -p "$dir"; cd "$dir" || exit 2
case "$karte" in
  tool:*) IFS=';' read -r -a A <<< "${karte#tool:}"; "$TOOL" re15_card.mcr "${A[@]}" > karte.txt 2>&1 || { cat karte.txt; exit 3; } ;;
  von:*)  cp "$SP/runs/${karte#von:}/re15_card.mcr" re15_card.mcr || exit 3 ;;
esac
WP=()
if [ "$plan" != "-" ]; then cp "$plan" wegplan_eingabe.txt; WP=(RE15_WEGPLAN="$dir/wegplan_eingabe.txt" RE15_WEGPLAN_LOG=wegplan.txt); fi
env RE15_NO_INTRO=1 RE15_NOAUDIO=1 SDL_AUDIODRIVER=dummy RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore \
    RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_INPUT_SCRIPT=W1 \
    RE15_MSG_LOG=1 RE15_EVT_TRACE=1 RE15_STATE_LOG=state.log RE15_MODAL_LOG=modal.log RE15_DISCARD_LOG=discard.log \
    "${WP[@]}" "$@" "$EXE" > run.log 2>&1 &
pid=$!
wpid=$(cat /proc/$pid/winpid 2>/dev/null)
echo "pid=$pid winpid=$wpid" > pid.txt
t=0
while kill -0 $pid 2>/dev/null; do
  sleep 2; t=$((t+2))
  if [ $t -ge $to ]; then echo "TIMEOUT $to s -> taskkill /PID $wpid" >> run.log; taskkill //PID $wpid //F >/dev/null 2>&1; break; fi
done
wait $pid 2>/dev/null; echo "exit=$? nach ${t}s" >> run.log
tail -3 run.log
