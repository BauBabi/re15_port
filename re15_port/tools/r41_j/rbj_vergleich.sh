#!/usr/bin/env bash
# Runde 41 Spur J: Vorher/Nachher-Tabelle der RBJ-Laufzeitmessung (rbj_12raeume.sh vor/nach).
L="${1:-$(cd "$(dirname "$0")/../../build/r41j_laeufe" && pwd)}"
for r in 1040 1041 2000 3060 3091 30E0 4000 4001 4010 5011 6030 6031; do
  v="$L/vor_$r/debug.log"; n="$L/nach_$r/debug.log"
  vo=$(grep -ah "cinematic overlay" "$v" 2>/dev/null | grep -v "1240" | tail -1 | sed 's/.*overlay: //')
  no=$(grep -ah "cinematic overlay" "$n" 2>/dev/null | grep -v "1240" | tail -1 | sed 's/.*overlay: //')
  nb=$(grep -ah "\[rbj-binder\] ROOM$r Spieler" "$n" 2>/dev/null | tail -1 | sed 's/.*\] //')
  na=$(grep -ah "\[rbj-binder\] ROOM$r Aktor" "$n" 2>/dev/null | sed 's/.*Aktor /Aktor /' | tr '\n' ';')
  echo "ROOM$r | vorher Spieler-Overlay: ${vo:-?} | nachher: ${no:-?} | $nb | $na"
done
