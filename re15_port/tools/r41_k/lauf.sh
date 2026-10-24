#!/usr/bin/env bash
# Runde 41 Spur K: ein Messlauf der echten exe mit eigener exe-Kopie (Praefix re15_pc_r41k_) und eigenem Laufordner.
# Aufruf: lauf.sh <name> <timeout_s> [KEY=WERT ...]   (Muster: tools/r41_j/lauf.sh der Spur J, tests/integration/test_r34n_b_cursor.cmake)
# Laufordner: $K41_LAUF (Standard: <baum>/re15_port/build/r41k_laeufe)/<name>; debug.log dort. K41_EXE = andere Quell-exe.
set -u
BAUM="$(cd "$(dirname "$0")/../../.." && pwd)"
EXE="$BAUM/re15_port/build/platform/pc/re15_pc.exe"
QUELLE="${K41_EXE:-$EXE}"
NAME="$1"; TO="$2"; shift 2
LAUF="${K41_LAUF:-$BAUM/re15_port/build/r41k_laeufe}/$NAME"
rm -rf "$LAUF"; mkdir -p "$LAUF"
KOPIE="$(dirname "$EXE")/re15_pc_r41k_${NAME}.exe"
cp -f "$QUELLE" "$KOPIE"
for kv in "$@"; do export "$kv"; done
( cd "$LAUF" && timeout "$TO" "$KOPIE" > konsole.txt 2>&1 ) ; RV=$?
rm -f "$KOPIE"
echo "lauf=$NAME rv=$RV ordner=$LAUF"
