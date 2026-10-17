#!/usr/bin/env bash
# Attrappe von release/build_win_cross.sh (Test r38_releaseskript) - siehe attrappe_gemeinsam.sh
set -euo pipefail
source "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/attrappe_gemeinsam.sh"
attr_protokoll windows "$@"
attr_artefakt windows "$ATTR_HERE/win_out/re15_pc.exe" "MZ attrappe windows $(date +%s%N)" "WIN-CROSS-BUILD-OK: 40 Bytes"
echo "== Wrapper gesamt: 0 s (rc=0)"
