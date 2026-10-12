#!/usr/bin/env bash
# Runde 37 Spur D: Nachlaeufe mit den Vorbedingungs-Flags des Spiels (eine Messkopie, nacheinander).
#   spas: (3,126) = Lift-Strom (Set ROOM2060 sub @0x012AE; ROOM2030 sub01 @0x01E14 retypt sonst slot 5/7 auf die Meldung
#         "The elevator is not responding").
#   2040: (3,146) + (3,53) = Tor-Raetsel geloest (ROOM2040 sub00 @0x0141A/@0x01452: sonst Sca_id_set Zelle 19 u0 0xff).
D="$(cd "$(dirname "$0")" && pwd)"
L="$D/lauf_d.sh"
timeout 600 bash "$L" n2030_spas_weg2 "$D/wp_2030_spas_weg.txt" RE15_SET_FLAG=3:126 RE15_DEBUG_JUMP=2030@250 RE15_PLAYER_POS=6550,-100,3072,3 RE15_EXIT_AT=2600
timeout 500 bash "$L" n2040_weg2 "$D/wp_2040_weg.txt" RE15_SET_FLAG=3:146,3:53 RE15_DEBUG_JUMP=2040@250 RE15_PLAYER_POS=21700,-5400,2048,3 RE15_EXIT_AT=2400
echo BATCH2-FERTIG
