#!/bin/bash
# Runde 37 Spur L, Nachbesserung 2 — Messer-Paarlaeufe an der ECHTEN exe (Aufbau Abnahme 1 Abschnitt 2): Montage-Karte
# 1150, echte Tuer -> ROOM1130, RE15_EQUIP=1 (Messer), Skript "W1,A0.2,W2,W<w>" + 50x "M0.4,MA0.1" (R1 halten, alle
# 0,5 s = 15 Bilder ein Schlag), RE15_EXIT_AT=1400#1130. w = 20 + k/30 s (k = 0..14: alle 15 Schlagphasen) + 21.3/22.2/23.7.
# Je Lauf eine Zeile von messer_ausw.py (Platz 5 = der fette Zombie Typ 0x11).
# Aufruf: messer_laeufe.sh <exe> <kartenwerkzeug> <ausgabeordner> <name> [parallel=3]
set -u
EXE="$1"; KARTE="$2"; AUS="$3"; NAME="$4"; PAR="${5:-3}"
HIER="$(cd "$(dirname "$0")" && pwd)"
mkdir -p "$AUS"
SKRIPT_SCHLAEGE=""
for i in $(seq 1 50); do SKRIPT_SCHLAEGE="$SKRIPT_SCHLAEGE,M0.4,MA0.1"; done
WS="20.000 20.033 20.067 20.100 20.133 20.167 20.200 20.233 20.267 20.300 20.333 20.367 20.400 20.433 20.467 21.3 22.2 23.7"
lauf() {
    local w="$1"
    local tag="${NAME}_w${w}"
    bash "$HIER/exe_lauf.sh" "$tag" "$AUS/$tag" "$EXE" "$KARTE" \
        "1150 -17150 -11960 0 9:71 3:94 9:73-76 4:4-5 4:15 7:0xd3-0xd7 7:0xc6-0xca" \
        "W1,A0.2,W2,W${w}${SKRIPT_SCHLAEGE}" "1400#1130" RE15_EQUIP=1 > "$AUS/$tag.lauf.txt" 2>&1
}
n=0
for w in $WS; do
    lauf "$w" &
    n=$((n + 1))
    if [ $((n % PAR)) -eq 0 ]; then wait; fi
done
wait
for w in $WS; do
    tag="${NAME}_w${w}"
    printf "%-8s " "$w"
    if [ -f "$AUS/$tag/state.log" ]; then python "$HIER/messer_ausw.py" "$AUS/$tag/state.log" 5 1400; else echo "KEIN state.log"; fi
done
