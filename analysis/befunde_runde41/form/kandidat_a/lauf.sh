#!/usr/bin/env bash
# bash lauf.sh passung.json lauf-ordner  : Volumen, Zusammenbau, Pruefung
set -e
export PYTHONIOENCODING=utf-8
H="$(cd "$(dirname "$0")" && pwd)"
cd "$H"
P="$1"; L="$2"
python volumen.py "$P" | grep -E "^TEIL" | awk '{s+=$5} END {print "Flaechen nach Blender:", s}'
python zusammenbau.py "$P" --lauf "$L" | grep -v "^Atlas"
cd ../../../..
python re15_port/tools/form_pruefung.py pruefe "analysis/befunde_runde41/form/kandidat_a/$L/A.json" --aus "analysis/befunde_runde41/form/kandidat_a/$L/pr" > "analysis/befunde_runde41/form/kandidat_a/$L/pr.log" 2>&1
cd "$H"
python auszug.py "$L/pr/bericht.json"
