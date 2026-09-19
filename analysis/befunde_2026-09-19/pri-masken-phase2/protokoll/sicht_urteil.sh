#!/bin/bash
# Urteil zu jedem Sichtbild MESSEN (Phase 2, 2026-09-19).
cd "$(dirname "$0")/../../.."
python build/p2/scripts/sicht_pruef.py ROOM10C0 3 vor 200 -5850
python build/p2/scripts/sicht_pruef.py ROOM10C0 3 hinter -5400 -2250
python build/p2/scripts/sicht_pruef.py ROOM10D0 6 vor 4700 25550
python build/p2/scripts/sicht_pruef.py ROOM10D0 6 hinter 5300 25550
python build/p2/scripts/sicht_pruef.py ROOM1050 6 vor 23100 -19950
python build/p2/scripts/sicht_pruef.py ROOM1050 6 hinter 24300 -22550
python build/p2/scripts/sicht_pruef.py ROOM1000 5 vor -600 -1450
python build/p2/scripts/sicht_pruef.py ROOM1000 5 hinter 4200 -6050
python build/p2/scripts/sicht_pruef.py ROOM11F0 6 vor 3100 -100
python build/p2/scripts/sicht_pruef.py ROOM11F0 6 hinter 4300 -900
python build/p2/scripts/sicht_pruef.py ROOM11F0 1 vor -1100 -8900
python build/p2/scripts/sicht_pruef.py ROOM11F0 1 hinter -700 -8300
python build/p2/scripts/sicht_pruef.py ROOM1000 0 hinter 17600 -10450
