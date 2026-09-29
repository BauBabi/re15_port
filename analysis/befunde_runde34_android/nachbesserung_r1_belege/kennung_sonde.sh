#!/usr/bin/env bash
# Nachbesserung R1: apk_kennung + verify_apk_im_zip aus make_package.sh (per awk ausgeschnitten, unveraendert)
# gegen echte Artefakte. $1 = APK, $2 = letztes Volume eines Android-Split-Satzes, $3 = Eintragsname darin
set -euo pipefail
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android
die() { echo "ABBRUCH: $*" >&2; exit 1; }
source release/python_finden.sh
source release/apk_pruefen.sh
eval "$(awk '/^apk_kennung\(\) \{/{f=1} f{print} f&&/^}$/{exit}' release/make_package.sh)"
eval "$(awk '/^verify_apk_im_zip\(\) \{/{f=1} f{print} f&&/^}$/{exit}' release/make_package.sh)"
k="$(apk_kennung "$1")"; echo "Kennung $1: $k"
rc=0; verify_apk_im_zip "$2" "$3" "$k" || rc=$?; echo "verify_apk_im_zip (richtige Kennung): rc=$rc"
falsch="${k%% *} 00000000 ${k##* }"
rc=0; verify_apk_im_zip "$2" "$3" "$falsch" || rc=$?; echo "verify_apk_im_zip (CRC 00000000): rc=$rc"
rc=0; verify_apk_im_zip "$2" "anderer_name.apk" "$k" || rc=$?; echo "verify_apk_im_zip (anderer Name): rc=$rc"
