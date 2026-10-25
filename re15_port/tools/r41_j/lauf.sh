#!/usr/bin/env bash
# Runde 41 Spur J: ein Messlauf der echten exe mit eigener exe-Kopie und eigenem Laufordner.
# Aufruf: lauf.sh <name> <timeout_s> [KEY=WERT ...]
# Laufordner: $J41_LAUF (Standard: <baum>/re15_port/build/r41j_laeufe)/<name>; debug.log dort.
set -u
BAUM="$(cd "$(dirname "$0")/../../.." && pwd)"
EXE="$BAUM/re15_port/build/platform/pc/re15_pc.exe"
QUELLE="${J41_EXE:-$EXE}"   # andere Quell-exe (z.B. Vorher-Stand), Kopie liegt trotzdem neben der Baum-exe
NAME="$1"; TO="$2"; shift 2
LAUF="${J41_LAUF:-$BAUM/re15_port/build/r41j_laeufe}/$NAME"
rm -rf "$LAUF"; mkdir -p "$LAUF"
KOPIE="$(dirname "$EXE")/re15_pc_r41j_${NAME}.exe"
cp -f "$QUELLE" "$KOPIE"
for kv in "$@"; do export "$kv"; done
( cd "$LAUF" && timeout "$TO" "$KOPIE" ) ; RV=$?
rm -f "$KOPIE"
echo "lauf=$NAME rv=$RV ordner=$LAUF"
