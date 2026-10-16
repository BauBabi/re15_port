#!/usr/bin/env bash
# Attrappe von release/build_android.sh (Test r38_releaseskript) - siehe attrappe_gemeinsam.sh
set -euo pipefail
source "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/attrappe_gemeinsam.sh"
attr_protokoll android "$@"
V=""
while [[ $# -gt 0 ]]; do
    case "$1" in
        --version) V="$2"; shift 2 ;;
        *) echo "Attrappe android: unbekannte Option $1" >&2; exit 2 ;;
    esac
done
[[ -n "$V" ]] || { echo "Attrappe android: --version fehlt" >&2; exit 2; }
APK="$ATTR_HERE/re15_port_${V}_android.apk"
attr_artefakt android "$APK" "PK attrappe apk $V $(date +%s%N)" "== ANDROID-BUILD-OK: $APK =="
if [[ -f "$APK" ]]; then
    ( cd "$ATTR_HERE" && sha256sum -b "re15_port_${V}_android.apk" > SHA256SUMS_android.txt )
fi
