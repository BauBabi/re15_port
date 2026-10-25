#!/usr/bin/env bash
# Runde 41 Spur J: Laufzeitmessung des RBJ-Binders in den 12 abweichenden Raeumen (Leon 7, Elza 5).
# Aufruf: rbj_12raeume.sh <praefix> ; J41_EXE=<exe> fuer einen Vorher-Lauf.
P="${1:-rbj}"; D="$(dirname "$0")"
for r in 1040 2000 3060 30E0 4000 4010 6030; do
  bash "$D/lauf.sh" "${P}_$r" 150 RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=t.bmp RE15_DEBUG_JUMP=$r@5 "RE15_EXIT_AT=200#$r"
done
for r in 1040 3090 4000 5010 6030; do
  e=$(printf "%X" $((0x$r + 1)))
  bash "$D/lauf.sh" "${P}_$e" 200 RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 \
       RE15_PSELECT_AUTO_SWITCH=1 RE15_DEBUG_JUMP=$r@250 "RE15_EXIT_AT=450#$e"
done
