#!/usr/bin/env bash
# Sonde: der ECHTE APK-Block aus release/make_package.sh (APK_PRUEF= ... fi), per awk ausgeschnitten,
# mit frei waehlbarem HERE (dort liegen apk_asset_gate.py [echt oder Mutant] + <NAME>_android.apk).
# $1 = Repo (Quellbaum + release/make_package.sh), $2 = HERE, $3 = NAME
set -euo pipefail
REPO="$1"; HERE="$2"; NAME="$3"; DO_ZIP=1
die() { echo "ABBRUCH: $*" >&2; exit 1; }
source "$REPO/release/python_finden.sh" || die "kein Python"
eval "$(awk '/^APK_PRUEF=/{f=1} f{print} f&&/^fi$/{exit}' "$REPO/release/make_package.sh")"
echo "APK-BLOCK-OK (make_package.sh wuerde diese APK zippen)"
