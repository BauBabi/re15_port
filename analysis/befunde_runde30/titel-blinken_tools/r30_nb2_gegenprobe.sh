#!/usr/bin/env bash
# r30_nb2_gegenprobe.sh - Runde 30 / Thema D, zweite Nachbesserung.
# Eine Gegenprobe: Mutation im Wegwerf-Baum setzen, re15_pc bauen, integration_r30_titel_puls
# (das Skript des ARBEITSBAUMS) gegen die gebaute exe fahren.
# Aufruf: bash r30_nb2_gegenprobe.sh <wegwerf-baum> <mutation> <ausgabe-verzeichnis>
set -u
BAUM="$1"; MUT="$2"; AUS="$3"
HIER="$(cd "$(dirname "$0")" && pwd)"
ARB="$(cd "$HIER/../../.." && pwd)"
export PATH="/c/msys64/mingw64/bin:$PATH"
mkdir -p "$AUS/$MUT"
python "$HIER/r30_nb2_mutation.py" "$BAUM" "$MUT" > "$AUS/$MUT/mutation.txt" 2>&1 || { cat "$AUS/$MUT/mutation.txt"; exit 2; }
git -C "$BAUM" diff > "$AUS/$MUT/mutation.diff"
cmake --build "$BAUM/re15_port/build_mut" --target re15_pc > "$AUS/$MUT/build.log" 2>&1
BRC=$?
echo "BUILD_RC=$BRC" > "$AUS/$MUT/ergebnis.txt"
if [ $BRC -ne 0 ]; then
    grep -m5 -i "undefined reference\|error" "$AUS/$MUT/build.log" >> "$AUS/$MUT/ergebnis.txt"
    cat "$AUS/$MUT/ergebnis.txt"
    exit 0
fi
rm -rf "$AUS/$MUT/wd"; mkdir -p "$AUS/$MUT/wd"
cmake -DRE15_PC_EXE="$BAUM/re15_port/build_mut/platform/pc/re15_pc.exe" -DWORKDIR="$AUS/$MUT/wd" \
      -P "$ARB/re15_port/tests/integration/test_r30_titel_puls.cmake" > "$AUS/$MUT/test.log" 2>&1
TRC=$?
echo "TEST_RC=$TRC" >> "$AUS/$MUT/ergebnis.txt"
cat "$AUS/$MUT/ergebnis.txt"
cat "$AUS/$MUT/test.log"
