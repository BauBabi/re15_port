#!/usr/bin/env bash
# Laesst ein Bau-Skript laufen und stempelt jede Ausgabezeile mit Sekunden seit Start.
# Aufruf im Container: bash /probe/ts_run.sh <skript>
t0=$(date +%s.%N)
echo "T0 $(date -u +%FT%TZ) nproc=$(nproc) load=$(cat /proc/loadavg)"
set -o pipefail
bash "$@" 2>&1 | while IFS= read -r l; do
  n=$(date +%s.%N); printf '%8.1f %s\n' "$(awk -v a="$t0" -v b="$n" 'BEGIN{print b-a}')" "$l"
done
rc=${PIPESTATUS[0]}
n=$(date +%s.%N)
printf '%8.1f %s\n' "$(awk -v a="$t0" -v b="$n" 'BEGIN{print b-a}')" "ENDE rc=$rc load=$(cat /proc/loadavg)"
exit $rc
