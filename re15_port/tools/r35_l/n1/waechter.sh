#!/bin/bash
# waechter.sh <rundir>: verschiebt f_*.ppm laufend nach <rundir>/d/<laufnr>_<name> (Reihenfolge = Raumfolge)
W="$1"; mkdir -p "$W/d"; n=0
while [ ! -f "$W/.fertig" ]; do
  for f in "$W"/f_*.ppm; do [ -f "$f" ] || continue; sleep 0.2; n=$((n+1)); mv "$f" "$W/d/$(printf %05d $n)_$(basename "$f")"; done
  sleep 0.5
done
for f in "$W"/f_*.ppm; do [ -f "$f" ] || continue; n=$((n+1)); mv "$f" "$W/d/$(printf %05d $n)_$(basename "$f")"; done
