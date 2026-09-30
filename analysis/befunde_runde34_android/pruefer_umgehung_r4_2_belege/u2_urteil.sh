#!/usr/bin/env bash
# u2_urteil.sh <modus> <log> <rc> [apk_eintraege] - wendet gate_urteil aus release/apk_pruefen.sh (Arbeitsbaum, unveraendert) an
set -euo pipefail
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android
die() { echo "ABBRUCH: $*" >&2; exit 1; }
PY=/c/Python310/python
source "$BAUM/release/apk_pruefen.sh"
GATE_APK_EINTRAEGE="${4:-}"
rc=0; gate_urteil "$1" "$2" "$3" || rc=$?
exit $rc
