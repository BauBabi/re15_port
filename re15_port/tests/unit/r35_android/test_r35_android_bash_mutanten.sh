#!/usr/bin/env bash
# =============================================================================================
# Runde 35 Spur N "android", Nachbesserung 2 (Abnahme 1, M3) - ctest unit_r35_android_bash_mutanten.
# Nutzer: "Kuenftige Aenderungen am Pruefskript muessen dessen Urteilslogik selbst sorgfaeltig mittesten."
# Mutiert das bash-Urteil in release/apk_pruefen.sh (Operatoren im Kopf von bash_urteil_mutanten.py) und laesst je
# Mutant urteil_kontrollen.sh (Attrappen) laufen: jeder Mutant muss rot werden oder in AEQUIVALENT begruendet sein.
# Aufruf: test_r35_android_bash_mutanten.sh <repo-wurzel> <arbeitsordner> [parallel]
# =============================================================================================
set -u
REPO="$1"; W="$2"; PAR="${3:-8}"
HIER="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$REPO" || exit 9
# shellcheck source=/dev/null
source release/python_finden.sh > /dev/null 2>&1 || { echo "kein Python >= 3.8"; exit 9; }
export PY PY_VERSION
echo "Python: $PY ($PY_VERSION)"
B="$BASH"
command -v cygpath > /dev/null 2>&1 && B="$(cygpath -m "$BASH")"
nat() { if command -v cygpath > /dev/null 2>&1; then cygpath -m "$1"; else printf '%s\n' "$1"; fi; }
"$PY" "$(nat "$HIER/bash_urteil_mutanten.py")" "$(nat "$REPO/release/apk_pruefen.sh")" \
    "$(nat "$HIER/urteil_kontrollen.sh")" "$(nat "$W")" "$B" "$PAR"
