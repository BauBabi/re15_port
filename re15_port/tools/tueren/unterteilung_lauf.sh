#!/usr/bin/env bash
# unterteilung_lauf.sh <zielordner> [Seitenliste|ALLE]   (Runde 32, analysis/befunde_runde32/tueren_unterteilung.md)
# Kontaktbogen-Bilder (Anfang + Mitte je Seite, RE15_TUER_BOGEN) und bei ALLE die beiden Tor-Serien
# (RE15_TUER_TEST=0/1, RE15_TUER_SERIE) mit der ECHTEN exe des eigenen Baus. Laeuft ueber eine
# umbenannte Kopie r32_mess.exe im selben Verzeichnis: parallele Sitzungen beenden re15_pc.exe per
# Bildname (Runde 31/32: Laeufe endeten mit exit 1 ohne Absturzeintrag).
set -u
ROOT=$(cd "$(dirname "$0")/../../.." && pwd -W)
mkdir -p "$1"; Z=$(cd "$1" && pwd -W); S="${2:-ALLE}"
PC="$ROOT/re15_port/build/platform/pc"
cp -f "$PC/re15_pc.exe" "$PC/r32_mess.exe"
mkdir -p "$Z/bogen"
cd "$PC"
RE15_TUER_SEITE="$S" RE15_TUER_BOGEN="$Z/bogen" RE15_TUER_SCHNELL=1 ./r32_mess.exe > /dev/null 2>&1
echo "bogen exit $? $(ls "$Z/bogen" | wc -l) Bilder; $(grep 'Seiten gespielt' debug.log)"
cp debug.log "$Z/lauf_bogen.log"
if [ "$S" = "ALLE" ]; then
  for v in 0 1; do
    rm -rf "$Z/tor$v"; mkdir -p "$Z/tor$v"
    RE15_TUER_TEST=$v RE15_TUER_SERIE="$Z/tor$v" RE15_TUER_SCHNELL=1 ./r32_mess.exe > /dev/null 2>&1
    echo "tor$v exit $? $(ls "$Z/tor$v" | wc -l) Bilder"
  done
fi
