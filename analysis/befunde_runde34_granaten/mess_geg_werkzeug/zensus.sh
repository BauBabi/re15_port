#!/usr/bin/env bash
# Raum-Zensus: je Raum Debug-Sprung (Bild 250), 60 Bilder, Gegnerzeile des letzten Bilds.
W=$(dirname "$0")
for r in "$@"; do
  JUMP=$r@250 EXIT_AT="60#$r" SEK=120 bash $W/lauf.sh zensus_$r > /dev/null 2>&1
done
