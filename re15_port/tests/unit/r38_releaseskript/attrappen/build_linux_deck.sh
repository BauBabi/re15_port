#!/usr/bin/env bash
# Attrappe von release/build_linux_deck.sh (Test r38_releaseskript) - siehe attrappe_gemeinsam.sh
set -euo pipefail
source "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/attrappe_gemeinsam.sh"
attr_protokoll linux "$@"
echo "100% tests passed, 0 tests failed out of 5"
echo "   Tests: 5 gelaufen = 5 registriert (Untergrenze 5)"
attr_artefakt linux "$ATTR_HERE/linux_out/re15_pc" "ELF attrappe linux $(date +%s%N)" "LINUX-BUILD-OK"
