#!/usr/bin/env bash
# Runde 37 Spur N — ein Messlauf an der ECHTEN exe (Messkopie unter eigenem Namen neben der exe, Arbeitsordner im
# scratchpad; Muster tests/integration/test_r34n_b_cursor.cmake). Beendet NUR die eigene PID (nie /IM).
#   n3_port_lauf.sh <exe> <arbeitsordner> <timeout-s> KEY=WERT ...
set -u
EXE="$1"; WD="$2"; TO="$3"; shift 3
mkdir -p "$WD"
ID=$(date +%s%N | tail -c 9)
KOPIE="$(dirname "$EXE")/re15_pc_r37n_${ID}.exe"
cp "$EXE" "$KOPIE"
(
  cd "$WD" || exit 9
  for kv in "$@"; do export "$kv"; done
  timeout "$TO" "$KOPIE" > konsole.txt 2>&1
  echo "EXIT=$?" >> konsole.txt
)
rm -f "$KOPIE"
tail -1 "$WD/konsole.txt"
