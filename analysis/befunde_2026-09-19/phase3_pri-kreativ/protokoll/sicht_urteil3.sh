#!/bin/bash
# Urteil zu jedem Sichtbild MESSEN (Phase 3).
cd "$(dirname "$0")/../../.."
python -u build/p3/scripts/sicht_pruef3.py ROOM10C0 3 vor 200 -5850
python -u build/p3/scripts/sicht_pruef3.py ROOM10C0 3 hinter -5400 -2250
python -u build/p3/scripts/sicht_pruef3.py ROOM10D0 1 vor 2300 950
python -u build/p3/scripts/sicht_pruef3.py ROOM10D0 1 hinter 3500 -4650
python -u build/p3/scripts/sicht_pruef3.py ROOM1050 6 vor 23100 -19950
python -u build/p3/scripts/sicht_pruef3.py ROOM1050 6 hinter 24300 -22550
python -u build/p3/scripts/sicht_pruef3.py ROOM1000 0 vor 21400 -12050
python -u build/p3/scripts/sicht_pruef3.py ROOM1000 0 hinter 16600 -11050
python -u build/p3/scripts/sicht_pruef3.py ROOM1000 2 vor 21400 -3250
python -u build/p3/scripts/sicht_pruef3.py ROOM1000 2 hinter 16600 -2050
python -u build/p3/scripts/sicht_pruef3.py ROOM11F0 0 vor 300 -4300
python -u build/p3/scripts/sicht_pruef3.py ROOM11F0 0 hinter 4300 -1100
