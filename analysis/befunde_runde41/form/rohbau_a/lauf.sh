#!/usr/bin/env bash
# Ein Durchgang: fit (optional), Tonblatt, Bau, form_pruefung. bash lauf.sh NAME [--ohne-fit] [fit-Optionen]
set -u
HIER="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
cd "$HIER"
N="$1"; shift
export PYTHONIOENCODING=utf-8
if [ "${1:-}" = "--ohne-fit" ]; then shift; else python fit.py "$@" 2>&1 | tail -2 | cut -c1-200; fi
python bild.py
python bau.py 2>&1 | grep -v "^Warnung Tick" | tail -5
cd ../../../..
python re15_port/tools/form_pruefung.py pruefe analysis/befunde_runde41/form/rohbau_a/form.json --aus analysis/befunde_runde41/form/rohbau_a/zw/$N > analysis/befunde_runde41/form/rohbau_a/zw/$N.txt 2>&1
grep -E "^  (left_leg|part_of_Body|right_arm|right_leg|tail|face_and|left_arm)|^--- Bild|NEIN|^\(a\)|^\(b\)|^\(c\)|^\(d\)" analysis/befunde_runde41/form/rohbau_a/zw/$N.txt | cut -c1-132
