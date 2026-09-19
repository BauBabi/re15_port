#!/bin/bash
# Spiel-Lauf ROOM2090 (Phase 2 gator-und-audio): Debug-Sprung, Gator-Aggro sofort, Logs.
# Aufruf: bash run_gator.sh <run-verzeichnis> <sekunden> [FRAMEDUMP-Spec]
RUNDIR="$1"; SECS="${2:-150}"; FDD="$3"
EXE="$(cd "$(dirname "$0")/../../../re15_port/build_p2/platform/pc" && pwd)/re15_pc.exe"
mkdir -p "$RUNDIR"
cd "$RUNDIR" || exit 2
cp -n C:/workspace/git/reAi_v2/re15_card.mcr . 2>/dev/null
rm -f gator_boss.log befund.log re2se.log state.log debug.log stdout.log
export RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_DEBUG_JUMP=2090@gp RE15_GB_TEST=1
export RE15_AI_FLAVOR=re2 RE15_RE2SE_LOG=re2se.log RE15_STATE_LOG=state.log RE15_BEFUND_MARKE=1
if [ -n "$FDD" ]; then export RE15_FRAMEDUMP="$FDD"; export RE15_BOOT_EXIT_AT=2; fi   # Ende beim Continue-Reboot: sonst ueberschreibt der 2. Boot die Dumps (gleiche Frame-Namen)
"$EXE" > stdout.log 2>&1 &
PID=$!
sleep "$SECS"
# NUR die eigene PID beenden: ein "taskkill //IM re15_pc.exe" trifft auch die exe-Laeufe
# anderer Agenten/Worktrees und der Integrationstests (gemessen 2026-09-19:
# integration_save_counter_pin startet drei re15_pc.exe und fiel dadurch mitten im Lauf aus).
kill "$PID" 2>/dev/null || taskkill //F //PID "$PID" > /dev/null 2>&1
sleep 2
echo "run beendet (pid $PID) nach $SECS s"
ls -la
echo "=== debug.log"; tail -5 debug.log
echo "=== gator_boss"; grep -n "gator da\|FRESS-P2\|FSYNC2\|FSYNC3\|VERSCHWUNDEN" gator_boss.log 2>&1 | head -30
echo "=== re2se"; head -40 re2se.log 2>&1
echo "=== state"; wc -l state.log 2>&1
grep -n "t=23[^]]*mo=5 " state.log | head -2
grep -n "t=23[^]]*mo=11 " state.log | head -2
echo "=== befund"; head -5 befund.log 2>&1
