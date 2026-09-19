#!/bin/bash
# Sichtpruefung fuer die vom Nutzer genannten Cuts (Phase 2, 2026-09-19).
cd "$(dirname "$0")/../../.."
for RC in "ROOM10C0 3" "ROOM10D0 6" "ROOM1050 6" "ROOM1000 5" "ROOM10A0 8" "ROOM11F0 6" "ROOM11F0 1" "ROOM1000 0"; do
  echo "=== $RC ==="
  timeout 600 python build/p2/scripts/sicht2.py $RC 2>&1 | grep -v "^ *F[0-9]"
done
echo "=== FERTIG ==="
