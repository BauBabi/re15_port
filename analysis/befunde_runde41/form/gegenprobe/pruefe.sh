#!/bin/bash
# Zwischenstand ansehen: pruefe.sh lauf/x.pt wahl zielordner
cd "$(dirname "$0")/../gerippe"
P=$1; W=$2; A=$3
mkdir -p "$A"
python abgabe.py $P --wahl $W --aus "$A" > "$A/abgabe.txt" 2>&1
tail -12 "$A/abgabe.txt" | cut -c1-150
cd ../gegenprobe
python anatomie.py "$A/gerippe.json" > "$A/anatomie.txt" 2>&1
python richtung.py "$A/gerippe.json" > "$A/richtung.txt" 2>&1
python render.py "$A/gerippe.json" --aus "$A" > "$A/maler.txt" 2>&1
grep -E "Rumpf|Winkel|^\(|Beugung" "$A/anatomie.txt" | cut -c1-110
grep -E "Abstand Mittellinie" "$A/richtung.txt" | cut -c1-110
cat "$A/maler.txt"
