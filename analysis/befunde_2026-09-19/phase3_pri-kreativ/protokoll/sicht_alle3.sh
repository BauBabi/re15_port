#!/bin/bash
# Sichtpruefung Phase 3 fuer die vom Nutzer genannten Raeume (10C0, 10D0, 1050, 1000,
# 10A0, 11F0) — je ein Bild mit der Figur VOR und HINTER dem Gegenstand.
cd "$(dirname "$0")/../../.."
for RC in "ROOM10C0 3" "ROOM10D0 1" "ROOM1050 6" "ROOM1000 0" "ROOM1000 2" "ROOM10A0 2" "ROOM11F0 0"; do
  echo "=== $RC ==="
  timeout 900 python -u build/p3/scripts/sicht3.py $RC 2>&1 | grep -v "^ *F[0-9]"
done
echo "=== FERTIG ==="
