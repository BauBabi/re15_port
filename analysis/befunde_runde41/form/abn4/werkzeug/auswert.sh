#!/usr/bin/env bash
# Abnahme 4 (eigenes Werkzeug): Auswertung eines Laufs - Szene (mass.py --szene), Kampffenster vor der Szene (mass.py --von 1150),
# HP-Weg (hp_weg.py), Ton (ton.py), Kontaktblaetter (blatt.py) der Szene (jedes 6. Bild) und des Kampfs (jedes 8. Bild).
#   auswert.sh <laufname> [von] [bis]     (von/bis = Kampffenster, Default 1150 .. Szenenbeginn-1 bzw. Laufende)
set -uo pipefail
PY=/c/Python310/python.exe
S=/c/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formnb4/abn4e
W="$(dirname "$0")"
B="$W/../blatt"
NAME="$1"; VON="${2:-1150}"; BIS="${3:-}"
L="$S/laeufe/$NAME"
echo "## $NAME ($(cat "$L/lauf_ende.txt" 2>/dev/null); exe $(cut -c1-16 "$L/exe_sha.txt"))"
"$PY" "$W/hp_weg.py" "$L/state.log"
"$PY" "$W/ton.py" "$L"
SZ=$("$PY" "$W/mass.py" "$L" --szene)
echo "$SZ"
S0=$(echo "$SZ" | sed -n 's/.*Fenster F\([0-9]*\)-F\([0-9]*\).*/\1/p' | head -1)
S1=$(echo "$SZ" | sed -n 's/.*Fenster F\([0-9]*\)-F\([0-9]*\).*/\2/p' | head -1)
if [ -z "$BIS" ]; then
  if [ -n "$S0" ]; then BIS=$((S0 - 1)); else BIS=$(ls "$L/fd" | tail -1 | sed 's/f0*\([0-9]*\)\.ppm/\1/'); fi
fi
"$PY" "$W/mass.py" "$L" --von "$VON" --bis "$BIS"
if [ -n "$S0" ]; then "$PY" "$W/blatt.py" "$L" "$S0" "$S1" 6 "$B/${NAME}_szene.png" 8 200; fi
"$PY" "$W/blatt.py" "$L" "$VON" "$BIS" 8 "$B/${NAME}_kampf.png" 8 200
