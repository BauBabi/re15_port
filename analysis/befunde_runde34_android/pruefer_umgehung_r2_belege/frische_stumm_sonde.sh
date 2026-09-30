#!/usr/bin/env bash
# check_binary_fresh aus make_package.sh (per awk UNVERAENDERT) unter set -euo pipefail, in einem
# Mini-Repo, in dem git log fuer die APK-Pfade KEINEN Commit findet (src_t leer).
set -euo pipefail
MP=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android/release/make_package.sh
die() { echo "ABBRUCH: $*" >&2; exit 1; }
HERE="$1/release"
eval "$(awk '/^check_binary_fresh\(\) \{/{f=1} f{print} f&&/^}$/{exit}' $MP)"
echo "vor check_binary_fresh"
check_binary_fresh "$HERE/x.apk" "Android-APK" re15_port/engine re15_port/include re15_port/platform/pc re15_port/platform/android
echo "nach check_binary_fresh (so gemeint: ohne Commit-Zeit wird die Frische uebersprungen)"
