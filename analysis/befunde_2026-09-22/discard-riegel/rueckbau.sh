#!/usr/bin/env bash
# rueckbau.sh — jeden Riegel dieser Runde EINZELN zurueckbauen und messen, ob er ROT wird.
#
# Ein gruener Riegel, der nicht rot werden kann, ist kein Riegel. Genau das war der
# Sperrgrund der Runde 24: fuenf Rueckbauten machten Riegel rot, EINER (die
# fail-closed-Kopplung) nicht — sie stand in keinem Riegel.
#
# Jeder Lauf aendert GENAU EINE Sache, baut nur das eine Testziel neu und nimmt die
# Aenderung danach mit `git checkout --` zurueck. Gemessen wird der Exit-Code des
# Riegels und die erste FEHLER-Zeile.
#
# Aufruf (aus dem Repo-Wurzelverzeichnis):
#   bash analysis/befunde_2026-09-22/discard-riegel/rueckbau.sh [A B1 B2 C D1 D2 E F F2 F3]
set -uo pipefail

HIER="$(cd "$(dirname "$0")" && pwd)"
WURZEL="$(cd "$HIER/../../.." && pwd)"
BUILD="$WURZEL/re15_port/build"
EXE="$BUILD/tests/unit/r21_discard_wegwerfen.exe"
PY="${RE15_PY:-python}"

export PATH="/c/msys64/mingw64/bin:/c/Program Files/CMake/bin:/c/Python310/Scripts:/usr/bin:/c/Windows/System32:/c/Windows:/c/Windows/System32/Wbem"

LISTE=("$@")
if [ "${#LISTE[@]}" -eq 0 ]; then LISTE=(A B1 B2 C D1 D2 E F F2 F3); fi

cd "$WURZEL"
# ⛔ DIE RUECKNAHME LAEUFT UEBER KOPIEN, NICHT UEBER GIT. Zwei Gruende, beide gemessen:
# (1) der minimale PATH dieses Skripts (msys64 zuerst, s.o.) enthaelt kein `git` — ein
#     `git checkout --` scheitert dann mit "command not found", und die Rueckbauten
#     STAPELN sich still auf; (2) `git checkout --` wuerde auf HEAD zuruecksetzen und
#     damit auch die noch nicht eingecheckte Arbeit wegwerfen. Genau das ist hier am
#     2026-09-22 passiert.
SICHER="$HIER/.pristine"
mkdir -p "$SICHER"
cp "$WURZEL/re15_port/engine/src/item_discard_common.c"     "$SICHER/item_discard_common.c"
cp "$WURZEL/re15_port/tests/unit/r21_discard_wegwerfen.c"   "$SICHER/r21_discard_wegwerfen.c"
zurueck() {
    cp "$SICHER/item_discard_common.c"   "$WURZEL/re15_port/engine/src/item_discard_common.c"
    cp "$SICHER/r21_discard_wegwerfen.c" "$WURZEL/re15_port/tests/unit/r21_discard_wegwerfen.c"
}
trap zurueck EXIT

for R in "${LISTE[@]}"; do
    echo "=============================================================="
    echo "RUECKBAU $R"
    "$PY" "$HIER/rueckbau.py" "$WURZEL" "$R" || { echo "  ABBRUCH"; continue; }
    cmake --build "$BUILD" --target r21_discard_wegwerfen > "$HIER/rb_$R.build.log" 2>&1
    if [ $? -ne 0 ]; then
        echo "  BAU FEHLGESCHLAGEN (s. rb_$R.build.log)"
    else
        "$EXE" > "$HIER/rb_$R.log" 2>/dev/null
        rc=$?
        echo "  EXIT=$rc"
        grep -m4 "FEHLER:" "$HIER/rb_$R.log" || echo "  KEINE FEHLER-Zeile -> NICHT ROT"
        grep -m1 "FEHLGESCHLAGEN:" "$HIER/rb_$R.log" || true
    fi
    zurueck
done
# Den Ausgangsstand wiederherstellen und einmal sauber bauen.
cmake --build "$BUILD" --target r21_discard_wegwerfen > "$HIER/rb_restore.build.log" 2>&1
"$EXE" > "$HIER/rb_ausgangsstand.log" 2>/dev/null
echo "=============================================================="
echo "AUSGANGSSTAND wieder gebaut, EXIT=$?"
