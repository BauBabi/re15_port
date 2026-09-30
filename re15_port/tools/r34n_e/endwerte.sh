#!/bin/bash
# endwerte.sh - Runde 34 Nacht, Spur E: ALLE Sonden-Messungen fuer die ENDWERTE des Bauplans
# (E_dokumente.md 5.2) in einem Lauf, damit jede Zahl der Konstanten-Tabelle aus derselben Quelle kommt.
# Sonde: re15_port/build/tests/unit/probe_r34n_e_dokumente.exe (probes/r34n_e_dokumente.cmake).
#   huelle    Masken ueber dem gedrehten Modell-Quader je Cut (Klemmen)
#   abdeckung Standorte/Treffer des Aufhebe-Rechtecks, Abfangen durch fruehere Saetze (Vorrang)
#   druck     EIN echter Aktionsdruck ueber re15_aot_scan (Item 0x48 als Stellvertreter: die
#             Dokument-Tabelle kennt 0x49..0x4C noch nicht - der Leser-Weg ist derselbe)
#   licht     Vertexfarbe je Achsnormale im Lichtsatz jedes Cuts am Ort
# Quader: Buch mesh00/mesh03 bbox x -137..137 y -13..13 z -189..189 (um den Ursprung);
#         Blatt mesh01/mesh04 bbox x -144..142 y -98..0 z -225..211 -> Mitte (-1,-49,-7) gedreht.
# Aufruf: bash re15_port/tools/r34n_e/endwerte.sh > build/r34n_e/endwerte.txt
set -u
export PATH="/c/msys64/mingw64/bin:$PATH"
BAUM="$(cd "$(dirname "$0")/../../.." && pwd -W)"
P="$BAUM/re15_port/build/tests/unit/probe_r34n_e_dokumente.exe"
s() { echo; echo "### $*"; "$P" "$@" 2>&1 | grep -v "^\["; }

echo "=== Dok 1 (ROOM1050/1051) Ursprung (16474,-360,-6592) rot_y 0, Slot 15, Rechteck (15250,-7250,1000,1000)"
for R in 1050 1051; do
  s huelle $R 16474 -360 -6592 0 137 13 189
  s abdeckung $R 15250 -7250 1000 1000 15 12000 18500 -11000 -3000 50
  s licht $R 16474 -360 -6592
done
s druck 1050 14900 -6750 0 15250 -7250 1000 1000 15 0x48

echo; echo "=== Dok 2 (ROOM1000/1001) Ursprung (19226,-398,-11723) rot_y 0, Slot 10, Rechteck (18726,-12223,1000,1000)"
for R in 1000 1001; do
  s huelle $R 19226 -398 -11723 0 137 13 189
  s abdeckung $R 18726 -12223 1000 1000 10 16000 23000 -15000 -9000 50
done
s licht 1000 19226 -398 -11723
s strahl 1000 0 193.0 172.5 -385
for r in 0 1024 2048 3072; do s druck 1000 18250 -11723 $r 18726 -12223 1000 1000 10 0x48; done

echo; echo "=== Dok 3 (ROOM1020/1021) Ursprung (-9975,-1410,-16428) rot_y 0, Slot 14, Rechteck (-11100,-16928,2200,1000)"
echo "    Tisch-Nachricht: ROOM1020 Slot 10 -> 15, ROOM1021 Slot 11 -> 15"
for R in 1020 1021; do s huelle $R -9976 -1459 -16435 0 143 49 218; done
s abdeckung 1020 -11100 -16928 2200 1000 14 -14000 -6000 -21000 -12000 50 10 15
s abdeckung 1021 -11100 -16928 2200 1000 14 -14000 -6000 -21000 -12000 50 11 15
s abdeckung 1020 -11100 -16928 2200 1000 14 -14000 -6000 -21000 -12000 50
s licht 1020 -9975 -1410 -16428
s strahl 1020 6 156.5 105.5 -1410
s druck 1020 -11500 -16428 0 -11100 -16928 2200 1000 14 0x48 10 15
s druck 1020 -11500 -16428 0 -11100 -16928 2200 1000 14 0x48
s druck 1021 -11500 -16428 0 -11100 -16928 2200 1000 14 0x48 11 15

echo; echo "=== Dok 4 (ROOM1010/1011) Ursprung (450,-1600,5600) rot_y 3840, Slot 9, Rechteck (-50,5100,1000,1000)"
for R in 1010 1011; do
  s huelle $R 452 -1649 5593 3840 143 49 218
  s abdeckung $R -50 5100 1000 1000 9 -3000 4000 3000 9000 50
done
s licht 1010 450 -1600 5600
s strahl 1010 0 220.0 177.0 -1600
s druck 1010 700 6400 1024 -50 5100 1000 1000 9 0x48
s druck 1010 300 6400 1024 -50 5100 1000 1000 9 0x48
